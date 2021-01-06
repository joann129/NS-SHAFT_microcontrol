//
// TMR_LED : change LED on/off by Timer1 interrupt
//
#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include <math.h>
#include <string.h>
#include "LCD.h"
#include "Scankey.h"
#include "Seven_Segment.h"
#define yseed 7
#define totalPlat 7
#define ylong 8
#define platylong 16
#define peopleylong 8
#define xlong 8
uint32_t u32ADCvalue;//seed


//8*16 8*8
//8*64
//0x04, 0x18, 0x10, 0x52, 0x3c, 0x3e, 0x3e, 
	//0x24, 0x18, 0x10, 0x7e, 0x18, 0x3c, 0x3c, 0x18
unsigned char people[8] = {
	0x00, 0x28, 0x10, 0x7c, 0x18, 0x3c, 0x3c, 0x18
};

unsigned char clearPeople[8] = {
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

unsigned char plat[16] = {
0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x00
};

unsigned char sting[16] = {
0x00, 0xff, 0xff, 0xff, 0xff, 0x5a, 0x42, 0x00, 0x00, 0x7f, 0x7f, 0x7f, 0x7f, 0x6b, 0x6a, 0x00
};

unsigned char bigsting[64] = {
0xf8, 0xfc, 0xfe, 0xff, 0xff, 0xff, 0xfe, 0x00, 0xc0, 0xe1, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 
0x03, 0x87, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x0f, 0x1f, 0x7f, 0xff, 0xff, 0xff, 0xff, 0x00, 
0x38, 0x7c, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0xc0, 0xe0, 0xf9, 0xff, 0xff, 0xff, 0xff, 0x00, 
0x00, 0x83, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x07, 0x1f, 0x3f, 0x3f, 0x7f, 0x7f, 0x3f, 0x00
};

unsigned char platSting[5][16] = {
0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x00,
0x00, 0xff, 0xff, 0xff, 0xff, 0x5a, 0x42, 0x00, 0x00, 0x7f, 0x7f, 0x7f, 0x7f, 0x6b, 0x6a, 0x00,
0x00, 0xff, 0xff, 0xff, 0xff, 0x5a, 0x42, 0x00, 0x00, 0x7f, 0x7f, 0x7f, 0x7f, 0x6b, 0x6a, 0x00,
0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x00,
0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x00
};

/*int16_t peoplex = 64;
int16_t peopley = 32;
int16_t platx[3] = {0,47,0};
int16_t platy[3] = {24,40,56};*/
int i, j;
int16_t peoplex = 56;
int16_t peopley = 24;
int16_t platx[totalPlat] = {96,80,64,48,32,16,0};
int16_t platy[totalPlat] = {0,0,0,24,0,0,0};//47
int platType[totalPlat] = {0,0,0,0,0,0,0};
int score=1, cnt=1, live=4, flag=0, GG=0;
uint8_t ledState = 0;
uint32_t keyin = 0;
int input; //Scankey test

volatile uint8_t u8ADF;
volatile uint16_t X, Y;
void life(int num){
	if(num == 4){
		PC12=0;
		PC13=0;
		PC14=0;
		PC15=0;
	}else if(num == 3){
		PC12=1;
		PC13=0;
		PC14=0;
		PC15=0;
	}else if(num == 2){
		PC12=1;
		PC13=1;
		PC14=0;
		PC15=0;
	}else if(num == 1){
		PC12=1;
		PC13=1;
		PC14=1;
		PC15=0;
	}else if(num == 0){
		PC12=1;
		PC13=1;
		PC14=1;
		PC15=1;		
		GG = 1;
		TIMER_Close(TIMER1);
		
	}
}
void ADC_IRQHandler(void)
{
    uint32_t u32Flag;

    u32Flag = ADC_GET_INT_FLAG(ADC, ADC_ADF_INT);
	
    if(u32Flag & ADC_ADF_INT){
				u32ADCvalue = ADC_GET_CONVERSION_DATA(ADC, 6);
				X = ADC_GET_CONVERSION_DATA(ADC, 0);
				Y = ADC_GET_CONVERSION_DATA(ADC, 1);
		}
    ADC_CLR_INT_FLAG(ADC, u32Flag);
}

void Init_ADC(void)
{
    ADC_Open(ADC, ADC_INPUT_MODE, ADC_OPERATION_MODE, ADC_CHANNEL_MASK);
		ADC_POWER_ON(ADC);
    ADC_EnableInt(ADC, ADC_ADF_INT);
    NVIC_EnableIRQ(ADC_IRQn);
		ADC_START_CONV(ADC);
}

void TMR1_IRQHandler(void)
{	
	char str[10],line[50];
	life(live);
	Clear(peoplex,peopley, 1);
	for(i=0; i<totalPlat; i++) {
		if( ( peoplex==platx[i]+xlong || peoplex==platx[i]+xlong+1 || peoplex==platx[i]+xlong-1 )
			&& ( platy[i]<peopley+peopleylong && peopley<platy[i]+platylong ) ) {	//whether on the plat
			break;
		}
	}
	//
	if(peoplex+8 == 110){
		live--;
		life(live);
	}
	//people move
	if(i == totalPlat) {	//not on the plat
		peoplex -= 2;
		flag = 0;
	}else{								//on the plat
		peoplex += 1;
		if(platType[i]==1 || platType[i]==2 ){
			if(flag==0){
				flag=1;
				live--;
				life(live);
			}
		}else{
			if(flag==1){
				flag=0;
			}
		}
	}
	if(peoplex==0 || peoplex==1 || peoplex==-1) {
		GG=1;
	}
	//peopley -= 1;
	for(i=0; i<totalPlat; i++) { // plate move up
		Clear(platx[i],platy[i], 2);
		platx[i] += 1;
	}

		for(j=0; j<totalPlat; j++) {
			draw_Bmp8x16(platx[j],platy[j],FG_COLOR,BG_COLOR,platSting[platType[j]]);
			draw_Bmp8x8(peoplex,peopley,FG_COLOR,BG_COLOR,people);
		}
	
	if(platx[0]==104) { //plate disappear
			Clear(platx[0],platy[0], 2);
			for(i=0; i<totalPlat-1; i++) {
				platy[i] = platy[i+1];
				platx[i] = platx[i+1];
				platType[i] = platType[i+1];
			}
			cnt++;
			if(cnt%7==0) score++;
			strcpy(line,"Floor -");
			sprintf(str,"%d",score);
			printS_5x7(0,0,line);
			printS_5x7(35,0,str);
	}		
	if(platx[5]==16) {  //add plate
			platy[6] = (rand() % yseed) * ylong;
			platx[6] = 0;
			platType[6] = rand() % 5;			
	}
	TIMER_ClearIntFlag(TIMER1); // Clear Timer1 time-out interrupt flag
}

void Init_Timer1(void)
{
  TIMER_Open(TIMER1, TIMER_PERIODIC_MODE, 16);	//250000
  TIMER_EnableInt(TIMER1);
  NVIC_EnableIRQ(TMR1_IRQn);
  TIMER_Start(TIMER1);
}
void Init_Timer2(void)
{
  TIMER_Open(TIMER2, TIMER_PERIODIC_MODE, 16);	//250000
  TIMER_EnableInt(TIMER2);
  NVIC_EnableIRQ(TMR2_IRQn);
  TIMER_Start(TIMER2);
}
void OpenAll(void) {
	OpenKeyPad();
	GPIO_SetMode(PD, BIT14, GPIO_MODE_OUTPUT);
	GPIO_SetMode(PC, BIT12, GPIO_MODE_OUTPUT);
	GPIO_SetMode(PC, BIT13, GPIO_MODE_OUTPUT);
	GPIO_SetMode(PC, BIT14, GPIO_MODE_OUTPUT);
	GPIO_SetMode(PC, BIT15, GPIO_MODE_OUTPUT);
	init_LCD();
  clear_LCD();
}

int m,n;//for
char Text[32];
char Text0[16];
char Text1[16];
int main(void)
{
	int stop=0;
  SYS_Init();
	OpenAll();

	PD14 = 0;
	Init_ADC();
	
	//Y<2000 left ;  Y>3500 right
	
	Init_Timer1();
	srand(u32ADCvalue);
	for(m=0; m<totalPlat; m++) {
		platy[m] = (rand() % yseed) * ylong;
		platType[m] = rand()%5;
	}
	platy[3] = 24;;
	draw_Bmp8x64(120,0,FG_COLOR,BG_COLOR,bigsting);
	
	draw_Bmp8x8(peoplex,peopley,FG_COLOR,BG_COLOR,people);
	
  while(1)
	{
		if(GG == 1){
			break;
		}
		
		//input = ScanKey();	//ScanKey test
		//if(input == 2) {	//Scankey test
		if(Y<2000) {	//people left
			Clear(peoplex,peopley, 1);
			if(peopley!=0) {
				peopley -= 8;
			}
			clearBuff();
			draw_Bmp8x8(peoplex,peopley,FG_COLOR,BG_COLOR,people);
		}//else if(input == 8) {	//Scankey test
		else if(Y > 3500) {	//people right
			Clear(peoplex,peopley, 1);
			if(peopley<56) {
				peopley += 8;
			}
			clearBuff();
			draw_Bmp8x8(peoplex,peopley,FG_COLOR,BG_COLOR,people);
		}
		
		CLK_SysTickDelay(1000000);
	}
	clear_LCD();
	print_Line(1,"Game Over");
	strcpy(Text,"-");
	sprintf(Text1,"%d",score);
	strcat(Text,Text1);
	print_Line(2,Text);
	life(0);
}
