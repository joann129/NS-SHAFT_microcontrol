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
#define yseed 7
#define totalPlat 7
#define ylong 8
#define platylong 16
#define peopleylong 8
#define xlong 8


//8*16 8*8
//8*64

unsigned char people[8] = {
//0x04, 0x18, 0x10, 0x52, 0x3c, 0x3e, 0x3e, 
	//0x24, 0x18, 0x10, 0x7e, 0x18, 0x3c, 0x3c, 0x18
	0x00, 0x28, 0x10, 0x7c, 0x18, 0x3c, 0x3c, 0x18
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

unsigned char platSting[5][16] = 
{
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
int16_t peoplex = 56;
int16_t peopley = 24;
int16_t platx[totalPlat] = {96,80,64,48,32,16,0};
int16_t platy[totalPlat] = {0,0,0,24,0,0,0};//47
int platType[totalPlat] = {0,0,0,0,0,0,0};

uint8_t ledState = 0;
uint32_t keyin = 0;

volatile uint8_t u8ADF;

void ADC_IRQHandler(void)
{
    uint32_t u32Flag;

    u32Flag = ADC_GET_INT_FLAG(ADC, ADC_ADF_INT);
	
    if(u32Flag & ADC_ADF_INT)
        u8ADF = 1;

    ADC_CLR_INT_FLAG(ADC, u32Flag);
}

void Init_ADC(void)
{
    ADC_Open(ADC, ADC_INPUT_MODE, ADC_OPERATION_MODE, ADC_CH_6_MASK);
   ADC_POWER_ON(ADC);
    ADC_EnableInt(ADC, ADC_ADF_INT);
    NVIC_EnableIRQ(ADC_IRQn);
}

void TMR1_IRQHandler(void)
{	
	int i, j;
	ledState = ~ ledState;  // changing ON/OFF state
	//print_Line(2,"gan");
  /*if(ledState)
	{
		PC12 = 0;	
		PC13 = 1;	
	} 
  else	
	{
		PC12 = 1;	
		PC13 = 0;	
	}*/
	Clear(peoplex,peopley, 1);
	for(i=0; i<totalPlat; i++) {
		if(peoplex == platx[i]+xlong && ( platy[i]<=peopley+peopleylong && peopley<platy[i]+platylong ) ) {	//whether on the plat
			break;
		}
	}
	if(i == totalPlat) {	//not on the plat
		peoplex -= 1;
	}else{								//on the plat
		peoplex += 1;
	}
	//peopley -= 1;
	for(i=0; i<totalPlat; i++) {
		Clear(platx[i],platy[i], 2);
		platx[i] += 1;
	}
	
	//for(i=0; i<3; i++) {	//Prevent flicker
		for(j=0; j<totalPlat; j++) {
			draw_Bmp8x16(platx[j],platy[j],FG_COLOR,BG_COLOR,platSting[platType[j]]);
			draw_Bmp8x8(peoplex,peopley,FG_COLOR,BG_COLOR,people);
		}
		
	//}
	
	if(platx[0]==104) {
			Clear(platx[0],platy[0], 2);
			for(i=0; i<totalPlat-1; i++) {
				platy[i] = platy[i+1];
				platx[i] = platx[i+1];
				platType[i] = platType[i+1];
			}
	}		
	if(platx[5]==16) {
			platy[6] = (rand() % yseed) * ylong;
			platx[6] = 0;
			platType[6] = rand() % 5;
	}
	
	TIMER_ClearIntFlag(TIMER1); // Clear Timer1 time-out interrupt flag
}

void Init_Timer1(void)
{
  TIMER_Open(TIMER1, TIMER_PERIODIC_MODE, 4);
  TIMER_EnableInt(TIMER1);
  NVIC_EnableIRQ(TMR1_IRQn);
  TIMER_Start(TIMER1);
}

void OpenAll(void) {
	OpenKeyPad();
	init_LCD();
  clear_LCD();
}

int m,n;//for
int temp = 0, input;//scankey
uint32_t u32ADCvalue;//seed
char Text[32];
int main(void)
{
	
  SYS_Init();
	OpenAll();

	PD14 = 1;
	Init_ADC();

	ADC_START_CONV(ADC);
  while (u8ADF == 0);
  u32ADCvalue = ADC_GET_CONVERSION_DATA(ADC, 6);
	//sprintf(Text,"T = %5d", u32ADCvalue);
	//print_Line(1, Text);
	NVIC_DisableIRQ(ADC_IRQn);
	srand(u32ADCvalue);
	Init_Timer1();
	for(m=0; m<totalPlat; m++) {
		platy[m] = (rand() % yseed) * ylong;
		platType[m] = rand()%5;
	}
	platy[3] = 24;;
	draw_Bmp8x64(120,0,FG_COLOR,BG_COLOR,bigsting);
	for(m=0; m<totalPlat; m++) {
		draw_Bmp8x16(platx[m],platy[m],FG_COLOR,BG_COLOR,platSting[platType[m]]);
	}
	draw_Bmp8x8(peoplex,peopley,FG_COLOR,BG_COLOR,people);
	
  while(1)
	{
		input = ScanKey();
		if(input == 2) {	//people left
			Clear(peoplex,peopley, 1);
			peopley -= 8;
			draw_Bmp8x8(peoplex,peopley,FG_COLOR,BG_COLOR,people);
		}else if(input == 8) {	//people right
			Clear(peoplex,peopley, 1);
			peopley +=8;
			draw_Bmp8x8(peoplex,peopley,FG_COLOR,BG_COLOR,people);
		}
		CLK_SysTickDelay(1000000);
	}
}
