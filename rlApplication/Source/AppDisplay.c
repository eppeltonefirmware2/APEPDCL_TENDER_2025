//-------- include device specific files----------------
#include "..\\..\\rlDevice\\Include\\dDisplay.h"
#include "..\\..\\rlDevice\\Include\\dIOdefine.h"
#include "..\\..\\rlDevice\\Include\\dTypedef.h"
#include "..\\..\\rlDevice\\Include\\dDeltaSigmaADC.h"
#include "..\\..\\rlDevice\\Include\\dI2c.h"
#include "..\\..\\rlDevice\\Include\\dIOCtrl.h"
#include "..\\..\\rlDevice\\Include\\dWatchDog.h"
//-----------------------------------------------------

//-------- include app specific files-------------------
#include "..\\Include\\AppLcd.h"
#include "..\\Include\\AppEeprom.h"
#include "..\\Include\\AppMisc.h"
#include "..\\Include\\AppBilling.h"
#include "..\\Include\\AppConfig.h"
#include "..\\Include\\AppTampers.h"
#include "..\\Include\\AppTOD.h"
#include "..\\Include\\AppCustomerProtocol.h"
#include "..\\Include\\AppAccessProd.h"
//------------------------------------------------------

#if (defined(LCD18D) && (LCD18D == 1))
	#if (defined(LCD18A) && (LCD18A == 1))
		#define OK_BIT		0x80
		
	#endif
	#if (defined(LCD18D) && (LCD18A == 0))
		#define OK_BIT		0x01
		
	#endif
	
#endif

#if (defined(LCD18C) && (LCD18C == 1))
	#define OK_BIT		0x80
	#define VBAT_BIT	0x20
#endif





#define LCD_DATA_REG_PTR	(uint8_t*)&SEG0

uint8_t pinConfig[10]={
	4,
	5,
	6,
	7,
	8,
	9,
	10,
	11,
	16,
	17,
};


//0-Auto
//1-Push
//2-Cover Open in Normal Mode
//3- Power fail Normal
//4- Cover Open in Power fail
//5- production
//6- mode 3
//7- NM mode display
//8- production bush mode


#if (defined(IS_DISPLAY_CONFIGURABLE) && (IS_DISPLAY_CONFIGURABLE == 1))
#define MAX_DISPLAY_PARA_SIZE	62
static uint8_t lcdRAMBuff[MAX_DISPLAY_PARA_SIZE+3];
#endif

#if (defined(BOARD_TARGET_MVVNL) && (BOARD_TARGET_MVVNL == 1))
#define DPTIME	10
const uint8_t lcdParamIDBuff[][64]={
	{8,0x29,0x0B,0x0C,0x3F,0x00,0x1F,0x0D,0x15},
	{55,0x29,0x0B,0x0C,0x3F,0x00,0x1F,0x0D,0x15,0x10,0x43,0x48,0x4D,0x52,0x57,0x5C,0x40,0x45,0x4A,0x4F,0x54,0x59,0x80,0x81,0x82,0x83,0x84,0x85,0x01,0x04,0x14,0x09,0x6F,0x44,0x1E,0x6E,0x26,0x25,0x37,0x38,0x39,0x3A,0x3B,0x24,0x74,0x70,0x71,0x75,0x72,0x73,0x33,0x3E,0x3D,0x90,0x91,0x27},
	{2,0x33,0x33,0x3E,0x3D},
	{5,0x00,0x0D,0x6F,0x44,0x1E},
	{2,0x33,0x33,0x33,0x3D},
	{8,0x29,0x0C,0x0B,0x01,0x04,0x07,0x09,0x6E},
	{2,0x10,0x19},
	{30,0x00,0x15,0x00,0x15,0x00,0x15,0x00,0x15,0x00,0x15,0x00,0x15,0x00,0x15,0x00,0x15,0x00,0x15,0x00,0x15,0x00,0x15,0x00,0x15,0x00,0x15,0x00,0x15,0x00,0x15},
	{4,0x29,0x02,0x03,0x0A}
	
};
#elif (defined(IRDA_TYPE_METER_HP) && (IRDA_TYPE_METER_HP == 1)&&(IRDA_TYPE_METER_AP==0))
#define DPTIME	12
const uint8_t lcdParamIDBuff[][64]={
	{12,0x22,0x01,0x02,0x03,0x09,0x0A,0x40,0x42,0x41,0x33,0x3E,0x3D},
	{30,0x22,0x01,0x02,0x03,0x09,0x0A,0x40,0x42,0x41,0x33,0x3E,0x3D,0x29,0x3F,0x0C,0x0B,0x10,0x07,0x43,0x48,0x4D,0x52,0x57,0x5C,0x0D,0x0F,0x0E,0x44,0x95,0x6E},
	{12,0x22,0x01,0x02,0x03,0x09,0x0A,0x40,0x42,0x41,0x33,0x3E,0x3D},
	{2,0x22,0x40},
	{5,0x22,0x40,0x33,0x3E,0x03D},
	{8,0x29,0x0C,0x0B,0x01,0x04,0x07,0x09,0x6E},
	{2,0x10,0x19},
	{30,0x22,0x15,0x22,0x15,0x22,0x15,0x22,0x15,0x22,0x15,0x22,0x15,0x22,0x15,0x22,0x15,0x22,0x15,0x22,0x15,0x22,0x15,0x22,0x15,0x22,0x15,0x22,0x15,0x22,0x15},
	{4,0x29,0x02,0x03,0x0A}
	
};
#elif (defined(IRDA_TYPE_METER_HP) && (IRDA_TYPE_METER_HP == 1)&&(IRDA_TYPE_METER_AP==1)&&(IRDA_TYPE_METER_AP_WARNGL==0))
#define DPTIME	10
const uint8_t lcdParamIDBuff[][64]={
	//	{0x0B,0x00,0x3F,0x0C,0x0B,0x09,0x0D,0x0F,0x0E,0x02,0x03,0x01},
		{0x02,0x00,0x0D},
		{61,0x43,0x48,0x4d,0x52,0x57,0x5C,0x40,0x42,0x41,0x45,0x47,0x46,0x4A,0x4C,0x4B,0x4f,0x51,0x50,0x54,0x56,0x55,0x59,0x5B,0x5A,0x01,0x02,0x03,0x29,0x3F,0x0C,0x09,0x14,0x0B,0x10,0x26,0x25,0x37,0xA0,0xA1,0x38,0xA2,0xA3,0x39,0xA4,0xA5,0x3B,0xA6,0xA7,0x3A,0xA8,0xA9,0x33,0x3E,0x3D,0xAb,0x3E,0x3D,0xAA,0xA8,0xA9,0x6E},
		{0x2,0x33,0x33},
		{0x0B,0x00,0xF3,0x0C,0x0B,0x09,0x0D,0x0F,0x0E,0x02,0x03,0x01},
		{0x2,0x33,0x33},
		{8,0x29,0x0C,0x0B,0x01,0xFC,0x07,0x09,0x6E},
		{2,0x10,0x19},
		{61,0x43,0x48,0x4d,0x52,0x57,0x5C,0x40,0x42,0x41,0x45,0x47,0x46,0x4A,0x4C,0x4B,0x4f,0x51,0x50,0x54,0x56,0x55,0x59,0x5B,0x5A,0x01,0x02,0x03,0x29,0x3F,0x0C,0x09,0x14,0x0B,0x10,0x26,0x25,0x37,0x93,0x94,0x38,0x95,0x96,0x39,0x97,0x98,0x3B,0x99,0x9A,0xF0,0xF2,0xF1,0xED,0xEE,0xEF,0x24,0x3E,0x3D,0x3A,0x91,0x92,0x6E},
		{4,0x29,0x02,0x03,0x0A}
};
#elif (defined(IRDA_TYPE_METER_HP) && (IRDA_TYPE_METER_HP == 1)&&(IRDA_TYPE_METER_AP==1)&&(IRDA_TYPE_METER_AP_WARNGL==1))
#define DPTIME	10
const uint8_t lcdParamIDBuff[][64]={
		{0x02,0x00,0x0D},
		//{24,0x43,0x48,0x4D,0x52,0x57,0x5C,0x40,0x45,0x4A,0x4f,0x54,0x59,0x01,0x02,0x03,0x0C,0x0B,0x09,0x14,0x10,0x19,0x29,0x25,0x6E},
		{34,0x3F,0x0C,0x0B,0x01,0x02,0x03,0x00,0x43,0x48,0x4D,0x52,0x57,0x5C,0x61,0x66,0x6B,0x70,0x75,0x7A,0x0D,0x40,0x45,0x4A,0x4f,0x54,0x59,0x5E,0x63,0x68,0x6D,0x72,0x77,0x10,0x09},
		{0x2,0x33,0x33},
		{0x02,0x00,0x0D},
		{0x2,0x33,0x33},
		{9,0x29,0xFE,0x0C,0x0B,0x01,0x02,0x03,0x14,0x09},
		{2,0x10,0x19},
		{34,0x3F,0x0C,0x0B,0x01,0x02,0x03,0x00,0x43,0x48,0x4D,0x52,0x57,0x5C,0x61,0x66,0x6B,0x70,0x75,0x7A,0x0D,0x40,0x45,0x4A,0x4f,0x54,0x59,0x5E,0x63,0x68,0x6D,0x72,0x77,0x10,0x09},
		{5,0xFE,0x29,0x02,0x03,0x0A}
};
#else
#error "no display selected."
#endif

#if (defined(IS_UP_DWN_DISPLAY) && (IS_UP_DWN_DISPLAY == 1))
static uint8_t isforward=1;
static uint8_t push_hold_time=0;
void moveback(void);
#endif


struct lcdmap
{   
const uint8_t *Data;
    	uint8_t eep_addr;
   	uint8_t digits;
    	uint8_t offset;
    	uint8_t modifier;
    	uint8_t divisor;
    	uint8_t symbol1;
    	uint8_t symbol2;
	uint8_t symbol3;
    	uint8_t id;
	uint8_t txtid;
	uint16_t time;
	uint8_t startdigit;
};

const uint8_t txtmsg[][8]={
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,NONE	},	//0
{CHR_C	,CHR__	,CHR_O	,CHR_P	,CHR_E	,CHR_n	,NONE	,NONE	},	//1-C-open
#if (defined(LCD18D) && (LCD18D == 1))
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_P	,NONE	},	//2-P
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_n	,NONE	},	//3-n
#else
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_P	,CHR_h	},	//2-Ph
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_n	,CHR_U	},	//3-nU
#endif
{NONE	,CHR_A	,CHR_u	,CHR_t	,CHR_o	,NONE	,NONE	,NONE	},	//4-Auto
{NONE	,CHR_P	,CHR_u	,CHR_S	,CHR_h	,NONE	,NONE	,NONE	},	//5-Push
{CHR_E	,CHR_E	,CHR_P	,CHR_L	,CHR_0	,CHR_1	,NONE	,NONE	},	//6-EEPL01
#if (defined(PCB_VER_4_6_8) && (PCB_VER_4_6_8 == 1))
#if (defined(IRDA_TYPE_METER_HP) && (IRDA_TYPE_METER_HP==1))	
{CHR_r	,CHR_L	,CHR_4	,CHR_6	,CHR_8	,CHR_0	,CHR_0	,CHR_4 	},	//7-FORMAT 4.68.004
#else
{CHR_r	,CHR_L	,CHR_4	,CHR_6	,CHR_8	,CHR_5	,CHR_3	,CHR_1 	},	//7-FORMAT 4.68.531
#endif
#elif (defined(PCB_VER_4_6_6) && (PCB_VER_4_6_6 == 1))
{CHR_r	,CHR_E	,CHR_4	,CHR_6	,CHR_6	,CHR_6	,CHR_2	,CHR_1 	},	//7-FORMAT 4.66.621
#else
#error "no config"
#endif
#if (defined(LCD18D) && (LCD18D == 1))
{CHR_P	,CHR_F	,NONE	,NONE	,NONE	,NONE	,NONE	,NONE	},	//8-Pf
#else
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_P	,CHR_F	},	//8-Pf
#endif
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_1	},//9-01
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_2	},//10-02
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_3	},//11-03
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_4	},//12-04
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_5	},//13-05
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_6	},//14-06
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_7	},//15-07
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_8	},//16-08
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_9	},//17-09
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_1	,CHR_0	},//18-10
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_1	,CHR_1	},//19-11
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_1	,CHR_2	},//20-12
#if (defined(LCD18D) && (LCD18D == 1))
{NONE	,NONE	,NONE	,NONE	,NONE	,CHR_F	,CHR_r	,NONE	},//21-Fr
#else
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_F	,CHR_r	},//21-Fr
#endif
{CHR_d	,CHR_O	,CHR_n	,CHR_E	,NONE	,NONE	,NONE	,NONE	},//22-Done
{CHR_F	,CHR_A	,CHR_I	,CHR_L	,NONE	,NONE	,NONE	,NONE	},//23-Fail
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_n	},//24-Sr
{0xFF	,0xFF	,0xFF	,0xFF	,0xFF	,0xFF	,0xFF	,0xFF	},//25-lcd check
{CHR_A	,CHR_C	,CHR_t	,CHR__	,CHR_0	,CHR_0	,NONE	,NONE	},//26-Act00
{CHR_A	,CHR_C	,CHR_t	,CHR__	,CHR_0	,CHR_1	,NONE	,NONE	},//27-Act01
{NONE	,NONE	,CHR_O	,CHR_n	,NONE	,NONE	,NONE	,NONE	},//28-On
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_O	,CHR_H	},//29-OH
{CHR_P	,CHR_F	,NONE	,NONE	,NONE	,NONE	,CHR_H	,CHR_1	},//30-Pf-1
{CHR_P	,CHR_F	,NONE	,NONE	,NONE	,NONE	,CHR_H	,CHR_2	},//31-Pf-2
{CHR_P	,CHR_F	,NONE	,NONE	,NONE	,NONE	,CHR_H	,CHR_3	},//32-Pf-3
{CHR_P	,CHR_F	,NONE	,NONE	,NONE	,NONE	,CHR_H	,CHR_4	},//33-Pf-4
{CHR_P	,CHR_F	,NONE	,NONE	,NONE	,NONE	,CHR_H	,CHR_5	},//34-Pf-5
{CHR_P	,CHR_F	,NONE	,NONE	,NONE	,NONE	,CHR_H	,CHR_6	},//35-Pf-6
{CHR_P	,CHR_F	,NONE	,NONE	,NONE	,NONE	,CHR_H	,CHR_7	},//36-Pf-7
{CHR_P	,CHR_F	,NONE	,NONE	,NONE	,NONE	,CHR_H	,CHR_8	},//37-Pf-8
{CHR_P	,CHR_F	,NONE	,NONE	,NONE	,NONE	,CHR_H	,CHR_9	},//38-Pf-9
{CHR_P	,CHR_F	,NONE	,NONE	,NONE	,NONE	,CHR_1	,CHR_0	},//39-Pf-10
{CHR_P	,CHR_F	,NONE	,NONE	,NONE	,NONE	,CHR_1	,CHR_1	},//40-Pf-11
{CHR_P	,CHR_F	,NONE	,NONE	,NONE	,NONE	,CHR_1	,CHR_2	},//41-Pf-12
{CHR_P	,CHR_U	,CHR_L	,CHR_S	,CHR_E	,NONE	,NONE	,NONE	},//42-Pulse
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_t	,CHR_C	},//43-tc
{CHR_ml	,CHR_mr	,CHR_O	,CHR_d	,CHR_E	,NONE	,CHR_1	,NONE	},//44-Mode 1
{CHR_ml	,CHR_mr	,CHR_O	,CHR_d	,CHR_E	,NONE	,CHR_2	,NONE	},//45-Mode 2
{CHR_ml	,CHR_mr	,CHR_O	,CHR_d	,CHR_E	,NONE	,CHR_3	,NONE	},//46-Mode 3
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_t	,CHR_1	},//47-t1
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_t	,CHR_2	},//48-t2
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_t	,CHR_3	},//49-t3
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_t	,CHR_4	},//50-t4
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_t	,CHR_5	},//51-t5
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_t	,CHR_6	},//52-t6
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_t	,CHR_7	},//53-t7
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_t	,CHR_8	},//54-t8
{CHR_A	,CHR_U	,NONE	,NONE	,NONE	,NONE	,CHR_P	,CHR_F	},//55-AvPf
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_E	},//56-E
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_E	,CHR_1	},//57-E1
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_E	,CHR_2	},//58-E2
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_E	,CHR_3	},//59-E3
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_E	,CHR_4	},//60-E4
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_E	,CHR_5	},//61-E5
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_E	,CHR_6	},//62-E6
{CHR_n	,CHR_ml	,CHR_mr	,NONE	,NONE	,NONE	,NONE	,NONE	},//63-nm
{CHR_n	,CHR_d	,NONE	,NONE	,NONE	,NONE	,NONE	,NONE	},//64-nd
{CHR_r	,CHR_E	,CHR_U	,NONE	,NONE	,NONE	,NONE	,NONE	},//65-rev
{CHR_ml	,CHR_mr	,CHR_G	,NONE	,NONE	,NONE	,NONE	,NONE	},//66-mag
{CHR_E	,CHR_a	,CHR_r	,NONE	,NONE	,NONE	,NONE	,NONE	},//67-Ear
{CHR_E	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_n	,CHR_S	},//68-non solar
{CHR_N	,CHR_F	,CHR_A	,CHR_I	,CHR_L	,NONE	,NONE	,NONE	},//69-nFail
{CHR_n	,CHR_O	,CHR_C	,CHR_L	,CHR_r	,NONE	,NONE	,NONE	},//70-noClr
{NONE	,NONE	,NONE	,CHR__	,CHR__	,CHR__	,CHR_P	,CHR_F	},//71-Pf -.--
{CHR_n	,CHR_O	,CHR_t	,CHR_a	,CHR_P	,CHR_r	,NONE	,NONE	},//72-noTapr
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_P	,CHR_n	},//73- Pn
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_r	,CHR_C	},//74- rC
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_C	,CHR_u	},//75- Cu
{CHR__	,CHR__	,CHR__	,CHR__	,CHR__	,CHR__	,NONE	,NONE	},//76-dots for date & time
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_o	,CHR_C	},//77- ID to
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_r	,CHR_C	},//78- ID tr
{CHR_I	,CHR_d	,NONE	,CHR__	,CHR__	,CHR__	,CHR_t	,CHR_o	},//79- id --- to
{CHR_I	,CHR_d	,NONE	,CHR__	,CHR__	,CHR__	,CHR_t	,CHR_r	},//80- id --- tr
{CHR__	,CHR__	,CHR__	,CHR__	,CHR__	,CHR__	,CHR_o	,CHR_C	},//81-dots for date & time to
{CHR__	,CHR__	,CHR__	,CHR__	,CHR__	,CHR__	,CHR_r	,CHR_C	},//82-dots for date & time tr
{NONE	,CHR_G	,CHR_O	,CHR_O	,CHR_d	,NONE	,CHR_b	,CHR_t	},//83- good bt
{NONE	,NONE	,CHR_b	,CHR_a	,CHR_d	,NONE	,CHR_b	,CHR_t	},//84- bad bt
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_o	,CHR_C	},//85- oC
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_r	,CHR_C	},//86- rC
{CHR_t	,CHR_a	,CHR_n	,CHR_P	,CHR_E	,CHR_r	,NONE	,NONE	},//87-TanPer
{CHR_C	,CHR_O	,NONE	,NONE	,NONE	,NONE	,NONE	,NONE	},//88-CO
{NONE	,CHR_G	,CHR_O	,CHR_O	,CHR_d	,NONE	,CHR_r	,CHR_t	},//89- good rt
{NONE	,NONE	,CHR_b	,CHR_a	,CHR_d	,NONE	,CHR_r	,CHR_t	},//90- bad rt
{NONE	,CHR_G	,CHR_O	,CHR_O	,CHR_d	,NONE	,NONE	,NONE	},//91- good nvm
{NONE	,CHR_d	,CHR_n	,CHR_L	,CHR_d	,NONE	,NONE	,NONE	},//92- dnld
{NONE	,NONE  	,CHR_U	,CHR_P	,NONE	,NONE	,NONE	,NONE	},//93-Frd
{NONE  	,NONE	,CHR_d	,CHR_n	,NONE	,NONE	,NONE	,NONE	}, //94-back
{CHR_O	,CHR_n	,NONE	,NONE	,NONE	,NONE	,CHR_H	,CHR_1	}//95- On h1



};



#if (defined(LCD18D) && (LCD18D == 1))
const struct lcdmap disp[] = {


{RAM_KWH		,0xFF	,DIGIT_6	,NONE	,LONG_VAL	,DIV_100	,DISP_k+DISP_W		,DISP_h+DISP_CUM		,NONE						,0x00	,0x00	,DPTIME,0x02},	//kWh
{RAM_VOLT   	,0xFF 	,DIGIT_2  	,NONE  	,SHORT_VAL	,DIV_10 	,NONE           	,DISP_V					,DISP_DEC2       			,0x01	,0x00	,DPTIME,0x02},	//U
{RAM_PH_I   	,0xFF 	,DIGIT_4  	,NONE  	,SHORT_VAL	,DIV_1     	,NONE				,DISP_A 				,DISP_DEC5					,0x02	,0x02	,DPTIME,0x03},	// Ph Current
{RAM_NU_I   	,0xFF 	,DIGIT_4  	,NONE  	,SHORT_VAL	,DIV_1     	,NONE				,DISP_A 				,DISP_DEC5					,0x03	,0x03	,DPTIME,0x03},	// Nu Current
{RAM_EFF_I  	,0xFF 	,DIGIT_4  	,NONE  	,SHORT_VAL	,DIV_1     	,NONE				,DISP_A 				,DISP_DEC5					,0x04	,0x00	,DPTIME,0x03},	// Eff. Current
{RAM_PH_P   	,0xFF	,DIGIT_2  	,NONE  	,LONG_VAL 	,DIV_10  	,DISP_W         	,NONE					,DISP_DEC2					,0x05	,0x02	,DPTIME,0x02},	// Ph Power W
{RAM_NU_P   	,0xFF	,DIGIT_2  	,NONE  	,LONG_VAL 	,DIV_10  	,DISP_W         	,NONE					,DISP_DEC2					,0x06	,0x03	,DPTIME,0x02},	// Nu Power W
{RAM_EFF_P  	,0xFF	,DIGIT_2  	,NONE  	,LONG_VAL 	,DIV_10  	,DISP_W         	,NONE					,DISP_DEC2					,0x07	,0x00	,DPTIME,0x02},	// Eff. Power W
{RAM_S  		,0xFF	,DIGIT_3  	,NONE  	,LONG_VAL 	,DIV_1  	,DISP_W         	,DISP_V+DISP_A			,DISP_DEC3					,0x08	,0x00	,DPTIME,0x02},	// Eff. Power kVA
{RAM_PF     	,0xFF  	,DIGIT_3  	,NONE  	,CHAR_VAL 	,DIV_1     	,NONE          		,NONE					,DISP_DEC3                  ,0x09	,0x08	,DPTIME,0x02},	// Pf.
{RAM_FREQ   	,0xFF  	,DIGIT_3    ,NONE  	,SHORT_VAL	,DIV_1     	,NONE          		,NONE					,DISP_DEC5                  ,0x0A	,0x15	,DPTIME,0x04},	//FRQ
{RAM_TIME   	,0xFF 	,DIGIT_6  	,NONE  	,TIME_VAL	,DIV_1     	,DISP_TIME  		,DISP_DOTS				,NONE 	                    ,0x0B	,0x00	,DPTIME,0x02},	//TIME
{RAM_DATE   	,0xFF 	,DIGIT_6  	,NONE  	,DATE_VAL	,DIV_1     	,DISP_DATE  		,DISP_DOTS				,NONE 	                    ,0x0C	,0x00	,DPTIME,0x02},	//Date
{RAM_MD_kW  	,0xFF 	,DIGIT_4  	,NONE  	,SHORT_VAL	,DIV_1  	,DISP_k+DISP_W		,DISP_MD				,DISP_DEC3  				,0x0D	,0x00	,DPTIME,0x02},	// Current kW MD 
{RAM_kW_DATE   	,0xFF 	,DIGIT_6  	,NONE  	,TIME_VAL	,DIV_1     	,DISP_TIME+DISP_k+DISP_W,DISP_MD+DISP_DOTS	,NONE 	   	                ,0x0E	,0x00	,DPTIME,0x02},	//Current kW MD TIME
{RAM_kW_DATE   	,0xFF 	,DIGIT_6  	,NONE  	,DATE_VAL	,DIV_1     	,DISP_DATE+DISP_k+DISP_W,DISP_MD+DISP_DOTS	,NONE 	  					,0x0F	,0x00	,DPTIME,0x02},	//Current kW MD Date
{RAM_KWH		,0xFF	,DIGIT_5	,NONE	,LONG_VAL	,DIV_1		,DISP_k+DISP_W		,DISP_h					,DISP_DEC5					,0x10	,0x00	,240,0x02},	//High ResolutionkWh
{RAM_VOLT   	,0xFF 	,DIGIT_1  	,NONE  	,SHORT_VAL	,DIV_1 		,NONE           	,NONE					,NONE       				,0x11	,0x00	,0x20	,0x02},	//Erase Counter
{RAM_PH_P   	,0xFF	,DIGIT_3  	,NONE  	,LONG_VAL 	,DIV_1000  	,DISP_W         	,NONE					,DISP_DEC3					,0x12	,0x02	,DPTIME,0x02},	// Ph Power kW
{RAM_NU_P   	,0xFF	,DIGIT_3  	,NONE  	,LONG_VAL 	,DIV_1000  	,DISP_W         	,NONE					,DISP_DEC3					,0x13	,0x03	,DPTIME,0x02},	// Nu Power kW
{RAM_EFF_P  	,0xFF	,DIGIT_3  	,NONE  	,LONG_VAL 	,DIV_1000  	,DISP_k+DISP_W     	,NONE					,DISP_DEC3					,0x14	,0x00	,DPTIME,0x02},	// Eff. Power kW

{RAM_KVAH		,0xFF	,DIGIT_1	,NONE	,LONG_VAL	,DIV_100	,DISP_k				,DISP_h+DISP_V+DISP_A+DISP_CUM	,NONE				,0x15	,0x00	,DPTIME,0x02},	//kVAh
{RAM_MD_kVA  	,0xFF 	,DIGIT_3  	,NONE  	,SHORT_VAL	,DIV_1  	,DISP_k				,DISP_V+DISP_A+DISP_MD		,DISP_DEC3				,0x16	,0x00	,DPTIME,0x02},	// Current kVA MD
{RAM_kVA_DATE   ,0xFF 	,DIGIT_6  	,NONE  	,TIME_VAL	,DIV_1     	,DISP_TIME+DISP_k	,DISP_MD+DISP_DOTS+DISP_V+DISP_A,NONE				,0x17	,0x00	,DPTIME,0x02},	//Current kVA MD TIME
{RAM_kVA_DATE   ,0xFF 	,DIGIT_6  	,NONE  	,DATE_VAL	,DIV_1     	,DISP_DATE+DISP_k	,DISP_MD+DISP_DOTS+DISP_V+DISP_A,NONE 				,0x18	,0x00	,DPTIME,0x02},	//Current kVA MD Date
{RAM_KVAH		,0xFF	,DIGIT_5	,NONE	,LONG_VAL	,DIV_1		,DISP_k				,DISP_h+DISP_V+DISP_A		,DISP_DEC5				,0x19	,0x00	,DPTIME,0x02},	//High resolution kVAh

//{RAM_EXPKWH	,0xFF	,DIGIT_1	,NONE	,LONG_VAL	,DIV_100	,DISP_k+DISP_W		,DISP_h+DISP_CUM		,NONE						,0x1A	,0x38	,DPTIME,0x02},	//kWh
//{RAM_EXPKVAH	,0xFF	,DIGIT_1	,NONE	,LONG_VAL	,DIV_100	,DISP_k				,DISP_V+DISP_A			,NONE						,0x1B	,0x38	,DPTIME,0x02},	//kVAh
//{RAM_EXPKWH	,0xFF	,DIGIT_5	,NONE	,LONG_VAL	,DIV_1		,DISP_k+DISP_W		,DISP_h+DISP_CUM		,DISP_DEC5					,0x1C	,0x38	,DPTIME,0x01},	//High ResolutionkWh
//{RAM_EXPKVAH	,0xFF	,DIGIT_5	,NONE	,LONG_VAL	,DIV_1		,DISP_k				,DISP_h+DISP_V+DISP_A	,DISP_DEC5					,0x1D	,0x38	,DPTIME,0x01},	//High resolution kVAh

//{RAM_CUMPON	,0xFF	,DIGIT_1	,NONE	,LONG_VAL	,DIV_3600	,NONE				,DISP_h					,NONE						,0x1E	,0x49	,DPTIME,0x02},	// cumulative Power ON hours
//{RAM_CUMD_kW  	,0xFF 	,DIGIT_3  	,NONE  	,LONG_VAL	,DIV_1  	,DISP_k+DISP_W		,DISP_MD  				,DISP_DEC3					,0x1F	,0x4B	,DPTIME,0x02},	// Cumulative kW MD{RAM_RESET_CNT 	,0xFF 	,DIGIT_1  	,NONE  	,CHAR_VAL	,DIV_1  	,NONE				,NONE					,NONE						,0x20	,0x4A	,DPTIME,0x02},	// reset count
//{RAM_CUMD_kVA  	,0xFF 	,DIGIT_3  	,NONE  	,LONG_VAL	,DIV_1  	,DISP_k				,DISP_V+DISP_A+DISP_MD  ,DISP_DEC3					,0x21	,0x4B	,DPTIME,0x02},	// Cumulative kVA MD
{RAM_KWH	,0xFF	,DIGIT_2	,NONE	,LONG_VAL	,DIV_10		,DISP_k+DISP_W		,DISP_h+DISP_CUM		,DISP_DEC2					,0x22	,0x00	,DPTIME,0x02},	//kWh-1 DECIMAL

// C-OPEN
{MSG_CO   	,0xFF  	,DIGIT_2  	,NONE  	,SHORT_VAL	,DIV_1     	,NONE  			,NONE                   	,NONE    					,0x24	,0x58	,DPTIME,0x02},
// tamper
{MSG_NO_TAPER 	,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  			,NONE                   	,NONE    					,0x25	,0x57	,DPTIME,0x08},
// notamper
{MSG_NO_TAPER 	,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  			,NONE                   	,NONE    					,0x26	,0x48	,DPTIME,0x08},
// backup battery status
{MSG_BAT   	,0x00  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  			,NONE                   	,NONE    					,0x27	,0x53	,DPTIME,0x08},
// noClr
{MSG_NO_CLR   	,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  			,NONE                   	,NONE    					,0x28	,0x46	,0x02,0x08},


//lcd ALL
{MSG_ALL    	,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,0xFF  			,0xFF 						,0xFF                      	,0x29	,0x19	,0x05,0x08},
// Auto
{MSG_AUTO   	,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  			,NONE 						,NONE                      	,0x2A	,0x04	,0x02,0x08},
// Push
{MSG_PUSH   	,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  			,NONE 						,NONE                      	,0x2B	,0x05	,0x02,0x08},
// Done
{MSG_DONE   	,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  			,NONE 						,NONE                      	,0x2C	,0x16	,DPTIME,0x08},
// Fail
{MSG_FAIL   	,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  			,NONE 						,NONE                      	,0x2D	,0x17	,DPTIME,0x08},
// FW Ver
{MSG_FW   	,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  			,NONE 						,NONE                      	,0x2E	,0x07	,0x05,0x08},
// ACT-00
{MSG_ACT00 	,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  			,NONE 						,NONE                      	,0x2F	,0x1A	,0x05,0x08},
// ACT-01
{MSG_ACT01  	,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  			,NONE 						,NONE                       ,0x30	,0x1B	,0x05,0x08},
// ON
{MSG_ON  	,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  			,NONE 						,NONE                       ,0x31	,0x1C	,0x05,0x08},
// PULSE
{MSG_PULSE  	,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  			,NONE 						,NONE                       ,0x32	,0x2A	,0x05,0x08},
// C-OPEN
{MSG_C_OPEN 	,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  			,NONE 						,NONE                       ,0x33	,0x01	,DPTIME,0x08},

// Mode 1
{MSG_MODE1 	,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  			,NONE 						,NONE                       ,0x34	,0x2C	,0x05,0x08},
// Mode 2
{MSG_MODE2 	,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  			,NONE 						,NONE                       ,0x35	,0x2D	,0x05,0x08},
// Mode 3
{MSG_MODE3 	,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  			,NONE 						,NONE                       ,0x36	,0x2E	,0x05,0x08},
// NM
{MSG_NM 	,0xFF  	,DIGIT_2  	,NONE  	,SHORT_VAL 	,DIV_1     	,NONE  			,NONE                   	,NONE    					,0x37	,0x3F	,DPTIME,0x02},
// ND
{MSG_ND 	,0xFF  	,DIGIT_2  	,NONE  	,SHORT_VAL 	,DIV_1     	,NONE  			,NONE                   	,NONE    					,0x38	,0x40	,DPTIME,0x02},
// REV	
{MSG_REV 	,0xFF  	,DIGIT_2  	,NONE  	,SHORT_VAL 	,DIV_1     	,NONE  			,NONE                   	,NONE    					,0x39	,0x41	,DPTIME,0x02},
// MAG
{MSG_MAG 	,0xFF  	,DIGIT_2  	,NONE  	,SHORT_VAL 	,DIV_1     	,NONE  			,NONE                   	,NONE    					,0x3A	,0x42	,DPTIME,0x02},
// EAR
{MSG_EAR 	,0xFF  	,DIGIT_2  	,NONE  	,SHORT_VAL 	,DIV_1     	,NONE  			,NONE                   	,NONE   					,0x3B	,0x43	,DPTIME,0x02},
// nFail
{MSG_FAIL   	,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  			,NONE                   	,NONE    					,0x3C	,0x45	,DPTIME,0x08},




// Cover TIME
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x00  	,TIME_VAL	,DIV_1     	,DISP_TIME  	,DISP_DOTS					,NONE 	   	                ,0x3D	,0x00	,0x05,0x02},	
//Cover Date
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x00  	,DATE_VAL	,DIV_1     	,DISP_DATE  	,DISP_DOTS					,NONE 	   	                ,0x3E	,0x00	,0x05,0x02},
//Serial Number
{EEP_DATA   	,0x00  	,DIGIT_1  	,NONE  	,LONG_VAL 	,DIV_1     	,NONE        	,NONE						,NONE						,0x3F	,0x00	,DPTIME,0x08},



// H1 kW MD
{EEP_DATA  	,0x00 	,DIGIT_3  	,0x0C  	,SHORT_VAL	,DIV_1  	,DISP_k+DISP_W	,DISP_MD					,DISP_DEC3  				,0x40	,0x09	,DPTIME,0x02},	
// H1 kW MD TIME
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x0E  	,TIME_VAL	,DIV_1     	,DISP_TIME  	,DISP_MD|DISP_DOTS			,NONE 	   	                ,0x41	,0x09	,DPTIME,0x02},	
// H1 kW MD Date
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x0E  	,DATE_VAL	,DIV_1     	,DISP_DATE  	,DISP_MD|DISP_DOTS			,NONE 	   	                ,0x42	,0x09	,DPTIME,0x02},
// H1 kWh
{EEP_DATA	,0x00	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100	,DISP_k+DISP_W	,DISP_h+DISP_CUM			,NONE						,0x43	,0x09	,DPTIME,0x02},
// H1 Avg.Pf
{EEP_DATA   	,0x00  	,DIGIT_3  	,0x18  	,CHAR_VAL 	,DIV_1  	,NONE          	,NONE						,DISP_DEC3                  ,0x44	,0x1E	,DPTIME,0x02},

// H2 kW MD
{EEP_DATA  	,0x00 	,DIGIT_3  	,0x0C  	,SHORT_VAL	,DIV_1  	,DISP_k+DISP_W	,DISP_MD					,DISP_DEC3  				,0x45	,0x0A	,DPTIME,0x02},	
// H2 kW MD TIME
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x0E  	,TIME_VAL	,DIV_1     	,DISP_TIME  	,DISP_MD|DISP_DOTS			,NONE 	   	                ,0x46	,0x0A	,DPTIME,0x02},	
// H2 kW MD Date
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x0E  	,DATE_VAL	,DIV_1     	,DISP_DATE  	,DISP_MD|DISP_DOTS			,NONE 	   	                ,0x47	,0x0A	,DPTIME,0x02},
// H2 kWh
{EEP_DATA	,0x00	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100	,DISP_k+DISP_W	,DISP_h+DISP_CUM			,NONE						,0x48	,0x0A	,DPTIME,0x02},
// H2 Avg.Pf
{EEP_DATA   	,0x00  	,DIGIT_3  	,0x18  	,CHAR_VAL 	,DIV_1  	,NONE          	,NONE						,DISP_DEC3                  ,0x49	,0x1F	,DPTIME,0x02},

// H3 kW MD
{EEP_DATA  	,0x00 	,DIGIT_3  	,0x0C  	,SHORT_VAL	,DIV_1  	,DISP_k+DISP_W	,DISP_MD					,DISP_DEC3  				,0x4A	,0x0B	,DPTIME,0x02},	
// H3 kW MD TIME
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x0E  	,TIME_VAL	,DIV_1     	,DISP_TIME  	,DISP_MD|DISP_DOTS			,NONE 	   	                ,0x4B	,0x0B	,DPTIME,0x02},	
// H3 kW MD Date
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x0E  	,DATE_VAL	,DIV_1     	,DISP_DATE  	,DISP_MD|DISP_DOTS			,NONE 	   	             	,0x4C	,0x0B	,DPTIME,0x02},
// H3 kWh
{EEP_DATA	,0x00	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100	,DISP_k+DISP_W	,DISP_h+DISP_CUM			,NONE				 		,0x4D	,0x0B	,DPTIME,0x02},
// H3 Avg.Pf
{EEP_DATA   	,0x00  	,DIGIT_3  	,0x18  	,CHAR_VAL 	,DIV_1  	,NONE          	,NONE						,DISP_DEC3                  ,0x4E	,0x20	,DPTIME,0x02},

// H4 kW MD
{EEP_DATA  	,0x00 	,DIGIT_3  	,0x0C  	,SHORT_VAL	,DIV_1  	,DISP_k+DISP_W	,DISP_MD					,DISP_DEC3  			 	,0x4F	,0x0C	,DPTIME,0x02},	
// H4 kW MD TIME
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x0E  	,TIME_VAL	,DIV_1     	,DISP_TIME  	,DISP_MD|DISP_DOTS			,NONE 	   	                ,0x50	,0x0C	,DPTIME,0x02},	
// H4 kW MD Date
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x0E  	,DATE_VAL	,DIV_1     	,DISP_DATE  	,DISP_MD|DISP_DOTS			,NONE 	   	                ,0x51	,0x0C	,DPTIME,0x02},
// H4 kWh
{EEP_DATA	,0x00	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100	,DISP_k+DISP_W	,DISP_h+DISP_CUM			,NONE				 		,0x52	,0x0C	,DPTIME,0x02},
// H4 Avg.Pf
{EEP_DATA   	,0x00  	,DIGIT_3  	,0x18  	,CHAR_VAL 	,DIV_1  	,NONE          	,NONE						,DISP_DEC3                  ,0x53	,0x21	,DPTIME,0x02},

// H5 kW MD
{EEP_DATA  	,0x00 	,DIGIT_3  	,0x0C  	,SHORT_VAL	,DIV_1  	,DISP_k+DISP_W	,DISP_MD					,DISP_DEC3  			 	,0x54	,0x0D	,DPTIME,0x02},	
// H5 kW MD TIME
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x0E  	,TIME_VAL	,DIV_1     	,DISP_TIME  	,DISP_MD|DISP_DOTS			,NONE 	   	                ,0x55	,0x0D	,DPTIME,0x02},	
// H5 kW MD Date
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x0E  	,DATE_VAL	,DIV_1     	,DISP_DATE  	,DISP_MD|DISP_DOTS			,NONE 	   	                ,0x56	,0x0D	,DPTIME,0x02},
// H5 kWh
{EEP_DATA	,0x00	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100	,DISP_k+DISP_W	,DISP_h+DISP_CUM			,NONE				 		,0x57	,0x0D	,DPTIME,0x02},
// H5 Avg.Pf
{EEP_DATA   	,0x00  	,DIGIT_3  	,0x18  	,CHAR_VAL 	,DIV_1  	,NONE          	,NONE						,DISP_DEC3                  ,0x58	,0x22	,DPTIME,0x02},

// H6 kW MD
{EEP_DATA  	,0x00 	,DIGIT_3  	,0x0C  	,SHORT_VAL	,DIV_1  	,DISP_k+DISP_W	,DISP_MD					,DISP_DEC3  			 	,0x59	,0x0E	,DPTIME,0x02},	
// H6 kW MD TIME
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x0E  	,TIME_VAL	,DIV_1     	,DISP_TIME  	,DISP_MD|DISP_DOTS			,NONE 	   	                ,0x5A	,0x0E	,DPTIME,0x02},	
// H6 kW MD Date
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x0E  	,DATE_VAL	,DIV_1     	,DISP_DATE  	,DISP_MD|DISP_DOTS			,NONE 	   	                ,0x5B	,0x0E	,DPTIME,0x02},
// H6 kWh
{EEP_DATA	,0x00	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100	,DISP_k+DISP_W	,DISP_h+DISP_CUM			,NONE				 		,0x5C	,0x0E	,DPTIME,0x02},
// H6 Avg.Pf
{EEP_DATA   	,0x00  	,DIGIT_3  	,0x18  	,CHAR_VAL 	,DIV_1  	,NONE          	,NONE						,DISP_DEC3                  ,0x5D	,0x23	,DPTIME,0x02},

// H7 kW MD
{EEP_DATA  	,0x00 	,DIGIT_3  	,0x0C  	,SHORT_VAL	,DIV_1  	,DISP_k+DISP_W	,DISP_MD					,DISP_DEC3  				,0x5E	,0x0F	,DPTIME,0x02},	
// H7 kW MD TIME
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x0E  	,TIME_VAL	,DIV_1     	,DISP_TIME  	,DISP_MD|DISP_DOTS			,NONE 	   	                ,0x5F	,0x0F	,DPTIME,0x02},	
// H7 kW MD Date
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x0E  	,DATE_VAL	,DIV_1     	,DISP_DATE  	,DISP_MD|DISP_DOTS			,NONE 	   	                ,0x60	,0x0F	,DPTIME,0x02},
// H7 kWh
{EEP_DATA	,0x00	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100	,DISP_k+DISP_W	,DISP_h+DISP_CUM			,NONE						,0x61	,0x0F	,DPTIME,0x02},
// H7 Avg.Pf
{EEP_DATA   	,0x00  	,DIGIT_3  	,0x18  	,CHAR_VAL 	,DIV_1  	,NONE          	,NONE						,DISP_DEC3                  ,0x62	,0x24	,DPTIME,0x02},

// H8 kW MD
{EEP_DATA  	,0x00 	,DIGIT_3  	,0x0C  	,SHORT_VAL	,DIV_1  	,DISP_k+DISP_W	,DISP_MD					,DISP_DEC3  				,0x63	,0x10	,DPTIME,0x02},	
// H8 kW MD TIME
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x0E  	,TIME_VAL	,DIV_1     	,DISP_TIME  	,DISP_MD|DISP_DOTS			,NONE 	   	                ,0x64	,0x10	,DPTIME,0x02},	
// H8 kW MD Date
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x0E  	,DATE_VAL	,DIV_1     	,DISP_DATE  	,DISP_MD|DISP_DOTS			,NONE 	   	                ,0x65	,0x10	,DPTIME,0x02},
// H8 kWh
{EEP_DATA	,0x00	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100	,DISP_k+DISP_W	,DISP_h+DISP_CUM			,NONE						,0x66	,0x10	,DPTIME,0x02},
// H8 Avg.Pf
{EEP_DATA   	,0x00  	,DIGIT_3  	,0x18  	,CHAR_VAL 	,DIV_1  	,NONE          	,NONE						,DISP_DEC3                  ,0x67	,0x25	,DPTIME,0x02},

// H9 kW MD
{EEP_DATA  	,0x00 	,DIGIT_3  	,0x0C  	,SHORT_VAL	,DIV_1  	,DISP_k+DISP_W	,DISP_MD					,DISP_DEC3  				,0x68	,0x11	,DPTIME,0x02},	
// H9 kW MD TIME
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x0E  	,TIME_VAL	,DIV_1     	,DISP_TIME  	,DISP_MD|DISP_DOTS			,NONE 	   	                ,0x69	,0x11	,DPTIME,0x02},	
// H9 kW MD Date
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x0E  	,DATE_VAL	,DIV_1     	,DISP_DATE  	,DISP_MD|DISP_DOTS			,NONE 	   	             	,0x6A	,0x11	,DPTIME,0x02},
// H9 kWh
{EEP_DATA	,0x00	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100	,DISP_k+DISP_W	,DISP_h+DISP_CUM			,NONE				 		,0x6B	,0x11	,DPTIME,0x02},
// H9 Avg.Pf
{EEP_DATA   	,0x00  	,DIGIT_3  	,0x18  	,CHAR_VAL 	,DIV_1  	,NONE          	,NONE						,DISP_DEC3                  ,0x6C	,0x26	,DPTIME,0x02},

// H10 kW MD
{EEP_DATA  	,0x00 	,DIGIT_3  	,0x0C  	,SHORT_VAL	,DIV_1  	,DISP_k+DISP_W	,DISP_MD					,DISP_DEC3  			 	,0x6D	,0x12	,DPTIME,0x02},	
// H10 kW MD TIME
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x0E  	,TIME_VAL	,DIV_1     	,DISP_TIME  	,DISP_MD|DISP_DOTS			,NONE 	   	                ,0x6E	,0x12	,DPTIME,0x02},	
// H10 kW MD Date
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x0E  	,DATE_VAL	,DIV_1     	,DISP_DATE  	,DISP_MD|DISP_DOTS			,NONE 	   	                ,0x6F	,0x12	,DPTIME,0x02},
// H10 kWh
{EEP_DATA	,0x00	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100	,DISP_k+DISP_W	,DISP_h+DISP_CUM			,NONE				 		,0x70	,0x12	,DPTIME,0x02},
// H10 Avg.Pf
{EEP_DATA   	,0x00  	,DIGIT_3  	,0x18  	,CHAR_VAL 	,DIV_1  	,NONE          	,NONE						,DISP_DEC3                  ,0x71	,0x27	,DPTIME,0x02},

// H11 kW MD
{EEP_DATA  	,0x00 	,DIGIT_3  	,0x0C  	,SHORT_VAL	,DIV_1  	,DISP_k+DISP_W	,DISP_MD					,DISP_DEC3  			 	,0x72	,0x13	,DPTIME,0x02},	
// H11 kW MD TIME
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x0E  	,TIME_VAL	,DIV_1     	,DISP_TIME  	,DISP_MD|DISP_DOTS			,NONE 	   	                ,0x73	,0x13	,DPTIME,0x02},	
// H11 kW MD Date
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x0E  	,DATE_VAL	,DIV_1     	,DISP_DATE  	,DISP_MD|DISP_DOTS			,NONE 	   	                ,0x74	,0x13	,DPTIME,0x02},
// H11 kWh
{EEP_DATA	,0x00	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100	,DISP_k+DISP_W	,DISP_h+DISP_CUM			,NONE				 		,0x75	,0x13	,DPTIME,0x02},
// H11 Avg.Pf
{EEP_DATA   	,0x00  	,DIGIT_3  	,0x18  	,CHAR_VAL 	,DIV_1  	,NONE          	,NONE						,DISP_DEC3                  ,0x76	,0x28	,DPTIME,0x02},

// H12 kW MD
{EEP_DATA  	,0x00 	,DIGIT_3  	,0x0C  	,SHORT_VAL	,DIV_1  	,DISP_k+DISP_W	,DISP_MD					,DISP_DEC3  			 	,0x77	,0x14	,DPTIME,0x02},	
// H12 kW MD TIME
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x0E  	,TIME_VAL	,DIV_1     	,DISP_TIME  	,DISP_MD|DISP_DOTS			,NONE 	   	                ,0x78	,0x14	,DPTIME,0x02},	
// H12 kW MD Date
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x0E  	,DATE_VAL	,DIV_1     	,DISP_DATE  	,DISP_MD|DISP_DOTS			,NONE 	   	                ,0x79	,0x14	,DPTIME,0x02},
// H12 kWh
{EEP_DATA	,0x00	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100	,DISP_k+DISP_W	,DISP_h+DISP_CUM			,NONE				 		,0x7A	,0x14	,DPTIME,0x02},
// H12 Avg.Pf
{EEP_DATA   	,0x00  	,DIGIT_3  	,0x18  	,CHAR_VAL 	,DIV_1  	,NONE          	,NONE						,DISP_DEC3                  ,0x7B	,0x29	,DPTIME,0x02},


// total tamper count
//{RET_DATA   	,0xFE  	,DIGIT_1  	,0x00  	,SHORT_VAL 	,DIV_1  	,NONE 			,DISP_CUM    				,NONE        		    	,0x6E	,0x2B	,DPTIME,0x02},
// current month avg. pf
//{RET_DATA   	,0xFE  	,DIGIT_3  	,0x00  	,CHAR_VAL 	,DIV_1  	,NONE 			,NONE						,DISP_DEC3            		,0x6F	,0x37	,DPTIME,0x02},

// tamper last OCC date
//{EEP_DATA   	,0x00  	,DIGIT_6  	,0x00  	,DATE_VAL 	,DIV_1     	,DISP_DATE  	,DISP_DOTS 	            	,NONE          			 	,0x70	,0x55	,DPTIME,0x02},
// tamper last OCC time
//{EEP_DATA   	,0x00 	,DIGIT_6  	,0x00  	,TIME_VAL	,DIV_1     	,DISP_TIME  	,DISP_DOTS 	            	,NONE          			 	,0x71	,0x55	,DPTIME,0x02},	

// tamper last restore date
//{EEP_DATA   	,0x00  	,DIGIT_6  	,0x00  	,DATE_VAL 	,DIV_1     	,DISP_DATE  	,DISP_DOTS 	            	,NONE          			 	,0x72	,0x56	,DPTIME,0x02},
// tamper last restore time
//{EEP_DATA   	,0x00 	,DIGIT_6  	,0x00  	,TIME_VAL	,DIV_1     	,DISP_TIME  	,DISP_DOTS 	            	,NONE          			 	,0x73	,0x56	,DPTIME,0x02},	

// TAMPER id occ
//{MSG_T_NAME   	,0xFE  ,DIGIT_8  	,0x00  	,SHORT_VAL 	,DIV_1  	,NONE 			,NONE            			,NONE				 		,0x74	,0x4D	,DPTIME,0x08},
// TAMPER id res
//{MSG_T_NAME   	,0xFE  ,DIGIT_8  	,0x00  	,SHORT_VAL 	,DIV_1  	,NONE 			,NONE            			,NONE				 		,0x75	,0x4E	,DPTIME,0x08},


//H1 kVAh
//{EEP_DATA		,0x00	,DIGIT_1	,0x04	,LONG_VAL	,DIV_100	,DISP_k			,DISP_h|DISP_V+DISP_A|DISP_CUM		,NONE				 ,0x80	,0x09	,DPTIME,0x02},
//H2 kVAh
//{EEP_DATA		,0x00	,DIGIT_1	,0x04	,LONG_VAL	,DIV_100	,DISP_k			,DISP_h|DISP_V+DISP_A|DISP_CUM		,NONE				 ,0x81	,0x0A	,DPTIME,0x02},
//H3 kVAh
//{EEP_DATA		,0x00	,DIGIT_1	,0x04	,LONG_VAL	,DIV_100	,DISP_k			,DISP_h|DISP_V+DISP_A|DISP_CUM		,NONE				 ,0x82	,0x0B	,DPTIME,0x02},
//H4 kVAh
//{EEP_DATA		,0x00	,DIGIT_1	,0x04	,LONG_VAL	,DIV_100	,DISP_k			,DISP_h|DISP_V+DISP_A|DISP_CUM		,NONE				 ,0x83	,0x0C	,DPTIME,0x02},
//H5 kVAh
//{EEP_DATA		,0x00	,DIGIT_1	,0x04	,LONG_VAL	,DIV_100	,DISP_k			,DISP_h|DISP_V+DISP_A|DISP_CUM		,NONE				 ,0x84	,0x0D	,DPTIME,0x02},
//H6 kVAh
//{EEP_DATA		,0x00	,DIGIT_1	,0x04	,LONG_VAL	,DIV_100	,DISP_k			,DISP_h|DISP_V+DISP_A|DISP_CUM		,NONE			 	 ,0x85	,0x0E	,DPTIME,0x02},

// rtc battery status
{MSG_BAT   		,0xFF  	,DIGIT_8  	,NONE  	,CHAR_VAL 	,DIV_1     	,NONE  			,NONE                 				,NONE    			 ,0x90	,0x59	,DPTIME,0x08},
// nvm status
{MSG_BAT   		,0x00  	,DIGIT_8  	,NONE  	,CHAR_VAL 	,DIV_1     	,NONE  			,NONE                 				,NONE    			 ,0x91	,0x5B	,DPTIME,0x08},
// dnld
{MSG_DNLD   	,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  			,NONE                 				,NONE    			 ,0x92	,0x5C	,0x04  ,0x08},
//back
//{MSG_BC   		,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  			,NONE                 				,NONE    			 ,0x93	,0x5E	,0x02  ,0x08},
//forward
//{MSG_FR   		,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  			,NONE                 				,NONE    			 ,0x94	,0x5D	,0x02  ,0x08},
// h1 Power ON hours
{EEP_DATA		,0x00	,DIGIT_1	,0x08	,LONG_VAL	,DIV_3600	,NONE			,DISP_h				  				,NONE				 ,0x95	,0x5F	,DPTIME,0x02},
#if(defined(IRDA_TYPE_METER_AP) && (IRDA_TYPE_METER_AP == 1))
//Serial Number from ram 
{RET_DATA   	,0xFE  	,DIGIT_1  	,NONE  	,LONG_VAL 	,DIV_1     	,NONE        	,NONE				  				,NONE			     ,0xF3	,0x00	,DPTIME,0x08},
//--------------------------------
//present tamper date & time
// tamper nm last OCC date
{EEP_DATA   	,0x00  	,DIGIT_6  	,0x00  	,DATE_VAL 	,DIV_1     	,NONE  			,DISP_DOTS 	            			,DISP_DATE           ,0xA0	,0x55	,DPTIME,0x02},
// tamper nm last OCC time
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x00  	,TIME_VAL	,DIV_1     	,NONE  			,DISP_DOTS 	            			,DISP_TIME     		 ,0xA1	,0x55	,DPTIME,0x02},	
// tamper nd last OCC date
{EEP_DATA   	,0x00  	,DIGIT_6  	,0x00  	,DATE_VAL 	,DIV_1     	,NONE  			,DISP_DOTS 	            			,DISP_DATE           ,0xA2	,0x55	,DPTIME,0x02},
// tamper nd last OCC time
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x00  	,TIME_VAL	,DIV_1     	,NONE  			,DISP_DOTS 	            			,DISP_TIME     		 ,0xA3	,0x55	,DPTIME,0x02},	
// tamper rev last OCC date
{EEP_DATA   	,0x00  	,DIGIT_6  	,0x00  	,DATE_VAL 	,DIV_1     	,NONE  			,DISP_DOTS 	            			,DISP_DATE           ,0xA4	,0x55	,DPTIME,0x02},
// tamper rev last OCC time
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x00  	,TIME_VAL	,DIV_1     	,NONE  			,DISP_DOTS 	            			,DISP_TIME     		 ,0xA5	,0x55	,DPTIME,0x02},	
// tamper earth last OCC date
{EEP_DATA   	,0x00  	,DIGIT_6  	,0x00  	,DATE_VAL 	,DIV_1     	,NONE  			,DISP_DOTS 	            			,DISP_DATE           ,0xA6	,0x55	,DPTIME,0x02},
// tamper earth last OCC time
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x00  	,TIME_VAL	,DIV_1     	,NONE  			,DISP_DOTS 	            			,DISP_TIME     		 ,0xA7	,0x55	,DPTIME,0x02},
// mag last OCC date
{EEP_DATA   	,0x00  	,DIGIT_6  	,0x00  	,DATE_VAL 	,DIV_1     	,NONE  			,DISP_DOTS 	          				,DISP_DATE        	 ,0xA8	,0x55	,DPTIME,0x02},
// mag last OCC time
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x00  	,TIME_VAL	,DIV_1     	,NONE  			,DISP_DOTS 	          				,DISP_TIME     		 ,0xA9	,0x55	,DPTIME,0x02},	

// MAG
{MSG_MAG 			,0xFF  	,DIGIT_2  	,NONE  	,SHORT_VAL 	,DIV_1     	,NONE  			,NONE                   			,NONE    			 ,0xAA	,0x42	,DPTIME,0x02},
// C-OPEN
{MSG_CO   		,0xFF  	,DIGIT_2  	,NONE  	,SHORT_VAL	,DIV_1     	,NONE  			,NONE                   			,NONE    			 ,0xAB	,0x58	,DPTIME,0x02},

//production status
{RET_DATA   	,0xFE 	,DIGIT_7  	,0x00  	,LONG_VAL	,DIV_1     	,NONE  			,NONE								,NONE 	   			,0xFE	,0x00	,DPTIME,0x01}
/*
// RAM H1 kWh
{RAM_KWH_H1		,0xFF	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100	,DISP_h			,DISP_V|DISP_T3|DISP_AL+DISP_k		,DISP_BP			 ,0xB0	,0x09	,DPTIME,0x02},
// RAM H2 kWh
{RAM_KWH_H1		,0xFF	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100	,DISP_h			,DISP_V|DISP_T3|DISP_AL+DISP_k		,DISP_BP			 ,0xB1	,0x0A	,DPTIME,0x02},
// RAM H3 kWh
{RAM_KWH_H1		,0xFF	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100	,DISP_h			,DISP_V|DISP_T3|DISP_AL+DISP_k		,DISP_BP			 ,0xB2	,0x0B	,DPTIME,0x02},
// RAM H4 kWh
{RAM_KWH_H1		,0xFF	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100	,DISP_h			,DISP_V|DISP_T3|DISP_AL+DISP_k		,DISP_BP			 ,0xB3	,0x0C	,DPTIME,0x02},
// RAM H5 kWh
{RAM_KWH_H1		,0xFF	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100	,DISP_h			,DISP_V|DISP_T3|DISP_AL+DISP_k		,DISP_BP			 ,0xB4	,0x0D	,DPTIME,0x02},
// RAM H6 kWh
{RAM_KWH_H1		,0xFF	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100	,DISP_h			,DISP_V|DISP_T3|DISP_AL+DISP_k		,DISP_BP			 ,0xB5	,0x0E	,DPTIME,0x02},

// RAM H1 kW MD
{RAM_KW_MD_H1  	,0xFF 	,DIGIT_3  	,0x00  	,SHORT_VAL	,DIV_1  	,DISP_MD		,DISP_V|DISP_T3|DISP_AL+DISP_k|DISP_DEC2,DISP_BP  		,0xB6	,0x09	,DPTIME,0x02},
// RAM H2 kW MD
{RAM_KW_MD_H2  	,0xFF 	,DIGIT_3  	,0x00  	,SHORT_VAL	,DIV_1  	,DISP_MD		,DISP_V|DISP_T3|DISP_AL+DISP_k|DISP_DEC2,DISP_BP  		,0xB7	,0x0A	,DPTIME,0x02},
// RAM H3 kW MD
{RAM_KW_MD_H3  	,0xFF 	,DIGIT_3  	,0x00  	,SHORT_VAL	,DIV_1  	,DISP_MD		,DISP_V|DISP_T3|DISP_AL+DISP_k|DISP_DEC2,DISP_BP  		,0xB8	,0x0B	,DPTIME,0x02},
// RAM H4 kW MD
{RAM_KW_MD_H4  	,0xFF 	,DIGIT_3  	,0x00  	,SHORT_VAL	,DIV_1  	,DISP_MD		,DISP_V|DISP_T3|DISP_AL+DISP_k|DISP_DEC2,DISP_BP  		,0xB9	,0x0C	,DPTIME,0x02},
// RAM H5 kW MD
{RAM_KW_MD_H5  	,0xFF 	,DIGIT_3  	,0x00  	,SHORT_VAL	,DIV_1  	,DISP_MD		,DISP_V|DISP_T3|DISP_AL+DISP_k|DISP_DEC2,DISP_BP  		,0xBA	,0x0D	,DPTIME,0x02},
// RAM H6 kW MD
{RAM_KW_MD_H6  	,0xFF 	,DIGIT_3  	,0x00  	,SHORT_VAL	,DIV_1  	,DISP_MD		,DISP_V|DISP_T3|DISP_AL+DISP_k|DISP_DEC2,DISP_BP  		,0xBB	,0x0E	,DPTIME,0x02},
*/
//------------------------------------------------
#endif


};
#endif

#if (defined(LCD18C) && (LCD18C == 1))
const struct lcdmap disp[] = {
	
{RAM_KWH			,0xFF	,DIGIT_6	,NONE	,LONG_VAL	,DIV_100	,DISP_h					,DISP_V|DISP_T3|DISP_AL+DISP_k					,NONE		,0x00	,0x00	,DPTIME,0x02},	//kWh
{RAM_VOLT   	,0xFF 	,DIGIT_2  	,NONE  	,SHORT_VAL	,DIV_10 	,NONE           		,DISP_V|DISP_DEC1			  					,NONE  		,0x01	,0x00	,DPTIME,0x02},	//U
{RAM_PH_I   	,0xFF 	,DIGIT_4  	,NONE  	,SHORT_VAL	,DIV_1     	,DISP_AR				,DISP_AL|DISP_DEC3 								,NONE		,0x02	,0x02	,DPTIME,0x02},	// Ph Current
{RAM_NU_I   	,0xFF 	,DIGIT_4  	,NONE  	,SHORT_VAL	,DIV_1     	,DISP_AR				,DISP_AL|DISP_DEC3 								,NONE		,0x03	,0x03	,DPTIME,0x02},	// Nu Current
{RAM_EFF_I  	,0xFF 	,DIGIT_4  	,NONE  	,SHORT_VAL	,DIV_1     	,DISP_AR				,DISP_AL|DISP_DEC3 								,NONE		,0x04	,0x00	,DPTIME,0x02},	// Eff. Current
{RAM_PH_P   	,0xFF	,DIGIT_4  	,NONE  	,LONG_VAL 	,DIV_1  	,NONE         			,DISP_V|DISP_T3|DISP_AL|DISP_DEC2				,NONE		,0x05	,0x02	,DPTIME,0x02},	// Ph Power W
{RAM_NU_P   	,0xFF	,DIGIT_4  	,NONE  	,LONG_VAL 	,DIV_1  	,NONE         			,DISP_V|DISP_T3|DISP_AL|DISP_DEC2				,NONE		,0x06	,0x03	,DPTIME,0x02},	// Nu Power W
{RAM_EFF_P  	,0xFF	,DIGIT_4  	,NONE  	,LONG_VAL 	,DIV_1  	,NONE         			,DISP_V|DISP_T3|DISP_AL|DISP_DEC2				,NONE		,0x07	,0x00	,DPTIME,0x02},	// Eff. Power W
{RAM_S  			,0xFF	,DIGIT_3  	,NONE  	,LONG_VAL 	,DIV_1000  	,NONE         			,DISP_V|DISP_T3|DISP_AL|DISP_DEC2				,NONE		,0x08	,0x00	,DPTIME,0x02},	// Eff. Power kVA

{RAM_PF     	,0xFF  	,DIGIT_3  	,NONE  	,CHAR_VAL 	,DIV_1     	,NONE          			,DISP_DEC2										,NONE       ,0x09	,0x08	,DPTIME,0x02},	// Pf.
{RAM_FREQ   	,0xFF  	,DIGIT_3    ,NONE  	,SHORT_VAL	,DIV_1     	,NONE          			,DISP_DEC2										,NONE       ,0x0A	,0x15	,DPTIME,0x02},	//FRQ
{RAM_TIME   	,0xFF 	,DIGIT_6  	,NONE  	,TIME_VAL	,DIV_1     	,NONE  					,DISP_DOTS										,DISP_TIME 	,0x0B	,0x00	,DPTIME,0x02},	//TIME
{RAM_DATE   	,0xFF 	,DIGIT_6  	,NONE  	,DATE_VAL	,DIV_1     	,NONE  					,DISP_DOTS										,DISP_DATE 	,0x0C	,0x00	,DPTIME,0x02},	//Date
{RAM_MD_kW  	,0xFF 	,DIGIT_4  	,NONE  	,SHORT_VAL	,DIV_1  	,DISP_MD				,DISP_V|DISP_T3|DISP_AL+DISP_k|DISP_DEC2		,NONE  		,0x0D	,0x00	,DPTIME,0x02},	// Current kW MD 
{RAM_kW_DATE  ,0xFF 	,DIGIT_6  	,NONE  	,TIME_VAL	,DIV_1     	,DISP_MD				,DISP_V|DISP_T3|DISP_AL+DISP_k|DISP_DOTS		,DISP_TIME	,0x0E	,0x00	,DPTIME,0x02},	//Current kW MD TIME
{RAM_kW_DATE  ,0xFF 	,DIGIT_6  	,NONE  	,DATE_VAL	,DIV_1     	,DISP_MD				,DISP_V|DISP_T3|DISP_AL+DISP_k|DISP_DOTS		,DISP_DATE 	,0x0F	,0x00	,DPTIME,0x02},	//Current kW MD Date
{RAM_KWH			,0xFF	,DIGIT_6	,NONE	,LONG_VAL	,DIV_1		,DISP_h|DISP_DEC4		,DISP_V|DISP_T3|DISP_AL+DISP_k					,NONE		,0x10	,0x00	,DPTIME,0x02},	//High ResolutionkWh

{RAM_VOLT   	,0xFF 	,DIGIT_1  	,NONE  	,SHORT_VAL	,DIV_1 		,NONE           		,NONE											,NONE       ,0x11	,0x00	,0x20,0x02},	//Erase Counter
{RAM_PH_P   	,0xFF	,DIGIT_3  	,NONE  	,LONG_VAL 	,DIV_1000  	,NONE         			,DISP_V|DISP_T3|DISP_AL|DISP_DEC2|DISP_k		,NONE		,0x12	,0x02	,DPTIME,0x02},	// Ph Power kW

{RAM_NU_P   	,0xFF	,DIGIT_3  	,NONE  	,LONG_VAL 	,DIV_1000  	,NONE         			,DISP_V|DISP_T3|DISP_AL|DISP_DEC2|DISP_k		,NONE		,0x13	,0x03	,DPTIME,0x02},	// Nu Power kW
{RAM_EFF_P  	,0xFF	,DIGIT_3  	,NONE  	,LONG_VAL 	,DIV_1000  	,NONE         			,DISP_V|DISP_T3|DISP_AL|DISP_DEC2|DISP_k		,NONE		,0x14	,0x00	,DPTIME,0x02},	// Eff. Power kW

{RAM_KVAH			,0xFF	,DIGIT_6	,NONE	,LONG_VAL	,DIV_100	,DISP_AR|DISP_h			,DISP_k|DISP_V|DISP_AL							,NONE		,0x15	,0x00	,DPTIME,0x02},	//kVAh

{RAM_MD_kVA  	,0xFF 	,DIGIT_4  	,NONE  	,SHORT_VAL	,DIV_1  	,DISP_MD|DISP_AR			,DISP_V|DISP_AL+DISP_k|DISP_DEC2			,NONE  		,0x16	,0x00	,DPTIME,0x02},	// Current kVA MD
{RAM_kVA_DATE ,0xFF 	,DIGIT_6  	,NONE  	,TIME_VAL	,DIV_1     	,DISP_MD|DISP_AR			,DISP_V|DISP_AL+DISP_k|DISP_DOTS			,DISP_TIME	,0x17	,0x00	,DPTIME,0x02},	//Current kVA MD TIME
{RAM_kVA_DATE ,0xFF 	,DIGIT_6  	,NONE  	,DATE_VAL	,DIV_1     	,DISP_MD|DISP_AR			,DISP_V|DISP_AL+DISP_k|DISP_DOTS			,DISP_DATE	,0x18	,0x00	,DPTIME,0x02},	//Current kVA MD Date
{RAM_KVAH			,0xFF	,DIGIT_6	,NONE	,LONG_VAL	,DIV_1		,DISP_AR|DISP_h|DISP_DEC4	,DISP_k|DISP_V|DISP_AL						,NONE		,0x19	,0x00	,DPTIME,0x02},	//High resolution kVAh

{RAM_EXPKWH		,0xFF	,DIGIT_1	,NONE	,LONG_VAL	,DIV_100	,DISP_h						,DISP_V|DISP_T3|DISP_AL+DISP_k				,NONE		,0x1A	,0x38	,DPTIME,0x02},	//kWh
{RAM_EXPKVAH	,0xFF	,DIGIT_1	,NONE	,LONG_VAL	,DIV_100	,DISP_AR|DISP_h				,DISP_k|DISP_V|DISP_AL						,NONE		,0x1B	,0x38	,DPTIME,0x02},	//kVAh
{RAM_EXPKWH		,0xFF	,DIGIT_5	,NONE	,LONG_VAL	,DIV_1		,DISP_h|DISP_DEC4			,DISP_V|DISP_T3|DISP_AL+DISP_k				,NONE		,0x1C	,0x38	,DPTIME,0x02},	//High ResolutionkWh
{RAM_EXPKVAH	,0xFF	,DIGIT_5	,NONE	,LONG_VAL	,DIV_1		,DISP_AR|DISP_h|DISP_DEC4	,DISP_k|DISP_V|DISP_AL						,NONE		,0x1D	,0x38	,DPTIME,0x02},	//High resolution kVAh

{RAM_CUMPON		,0xFF	,DIGIT_1	,NONE	,LONG_VAL	,DIV_3600	,DISP_h						,NONE							,NONE		,0x1E	,0x49	,DPTIME,0x02},	// cumulative Power ON hours
{RAM_CUMD_kW  	,0xFF 	,DIGIT_4  	,NONE  	,LONG_VAL	,DIV_1  	,DISP_MD					,DISP_V|DISP_T3|DISP_AL+DISP_k|DISP_DEC2		,NONE  		,0x1F	,0x4B	,DPTIME,0x02},	// Cumulative kW MD
{RAM_RESET_CNT 	,0xFF 	,DIGIT_1  	,NONE  	,CHAR_VAL	,DIV_1  	,NONE						,NONE							,NONE		,0x20	,0x4A	,DPTIME,0x02},	// reset count
//{RAM_CUMD_kVA  	,0xFF 	,DIGIT_3  	,NONE  	,LONG_VAL	,DIV_1  	,DISP_MD|DISP_AR				,DISP_V|DISP_AL+DISP_k|					,NONE  		,0x21	,0x4B	,DPTIME,0x02},	// Cumulative kVA MD
{RAM_KWH		,0xFF	,DIGIT_2	,NONE	,LONG_VAL	,DIV_10		,DISP_h						,DISP_V|DISP_T3|DISP_AL+DISP_k|DISP_DEC1		,NONE		,0x22	,0x00	,DPTIME,0x02},	//kWh-1 DECIMAL

// C-OPEN
{MSG_CO   		,0xFF  	,DIGIT_2  	,NONE  	,SHORT_VAL	,DIV_1     	,NONE  						,NONE                   				,NONE    	,0x24	,0x58	,DPTIME,0x02},
// tamper
{MSG_NO_TAPER 	,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  						,NONE                   				,NONE    	,0x25	,0x57	,DPTIME,0x08},
// notamper
{MSG_NO_TAPER 	,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  						,NONE                   				,NONE    	,0x26	,0x48	,DPTIME,0x08},
// backup battery status
{MSG_BAT   	,0x00  	,DIGIT_8  	,NONE  	,CHAR_VAL 	,DIV_1     	,NONE  						,NONE                   				,NONE    	,0x27	,0x53	,DPTIME,0x08},
// noClr
{MSG_NO_CLR   	,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  						,NONE                   				,NONE    	,0x28	,0x46	,0x02,0x08},


//lcd ALL
{MSG_ALL    	,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,0xFF  						,0xFF 							,0xFF           ,0x29	,0x19	,DPTIME,0x08},
// Auto
{MSG_AUTO   	,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  						,NONE 							,NONE           ,0x2A	,0x04	,0x02,0x08},
// Push
{MSG_PUSH   	,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  						,NONE 							,NONE           ,0x2B	,0x05	,0x02,0x08},
// Done
{MSG_DONE   	,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  						,NONE 							,NONE           ,0x2C	,0x16	,DPTIME,0x08},
// Fail
{MSG_FAIL   	,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  						,NONE 							,NONE           ,0x2D	,0x17	,DPTIME,0x08},
// FW Ver
{MSG_FW   	,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE 						,DISP_DEC3|DISP_DEC1   					,NONE     	,0x2E	,0x07	,0x05,0x08},
// ACT-00
{MSG_ACT00 	,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  						,NONE 							,NONE           ,0x2F	,0x1A	,0x05,0x08},
// ACT-01
{MSG_ACT01  	,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  						,NONE 							,NONE           ,0x30	,0x1B	,0x05,0x08},
// ON
{MSG_ON  	,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  						,NONE 							,NONE           ,0x31	,0x1C	,0x05,0x08},
// PULSE
{MSG_PULSE  	,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  						,NONE 							,NONE           ,0x32	,0x2A	,0x05,0x08},
// C-OPEN
{MSG_C_OPEN 	,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  						,NONE 							,NONE           ,0x33	,0x01	,DPTIME,0x08},

// Mode 1
{MSG_MODE1 	,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  						,NONE 							,NONE           ,0x34	,0x2C	,0x05,0x08},
// Mode 2
{MSG_MODE2 	,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  						,NONE 							,NONE           ,0x35	,0x2D	,0x05,0x08},
// Mode 3
{MSG_MODE3 	,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  						,NONE 							,NONE           ,0x36	,0x2E	,0x05,0x08},
// NM
{MSG_NM 	,0xFF  	,DIGIT_2  	,NONE  	,SHORT_VAL 	,DIV_1     	,NONE  						,NONE                  		 			,NONE    	,0x37	,0x3F	,DPTIME,0x02},
// ND
{MSG_ND 	,0xFF  	,DIGIT_2  	,NONE  	,SHORT_VAL 	,DIV_1     	,NONE  						,NONE                   				,NONE    	,0x38	,0x40	,DPTIME,0x02},
// REV
{MSG_REV 	,0xFF  	,DIGIT_2  	,NONE  	,SHORT_VAL 	,DIV_1     	,NONE  						,NONE                   				,NONE    	,0x39	,0x41	,DPTIME,0x02},
// MAG
{MSG_MAG 	,0xFF  	,DIGIT_2  	,NONE  	,SHORT_VAL 	,DIV_1     	,NONE  						,NONE                   				,NONE    	,0x3A	,0x42	,DPTIME,0x02},
// EAR
{MSG_EAR 	,0xFF  	,DIGIT_2  	,NONE  	,SHORT_VAL 	,DIV_1     	,NONE  						,NONE                   				,NONE   	,0x3B	,0x43	,DPTIME,0x02},
// nFail
{MSG_FAIL   	,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  						,NONE                   				,NONE    	,0x3C	,0x45	,DPTIME,0x08},




// Cover TIME
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x00  	,TIME_VAL	,DIV_1     	,NONE  						,DISP_DOTS						,DISP_TIME 	,0x3D	,0x00	,DPTIME,0x02},	
//Cover Date
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x00  	,DATE_VAL	,DIV_1     	,NONE  						,DISP_DOTS						,DISP_DATE 	,0x3E	,0x00	,DPTIME,0x02},
//Serial Number
{EEP_DATA   	,0x00  	,DIGIT_1  	,NONE  	,LONG_VAL 	,DIV_1     	,NONE        					,NONE							,NONE		,0x3F	,0x00	,DPTIME,0x08},



// H1 kW MD
{EEP_DATA  	,0x00 	,DIGIT_3  	,0x0C  	,SHORT_VAL	,DIV_1  	,DISP_MD					,DISP_V|DISP_T3|DISP_AL+DISP_k|DISP_DEC2		,DISP_BP  	,0x40	,0x09	,DPTIME,0x02},	
// H1 kW MD TIME
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x0E  	,TIME_VAL	,DIV_1     	,DISP_MD					,DISP_V|DISP_T3|DISP_AL+DISP_k|DISP_DOTS		,DISP_BP|DISP_TIME,0x41	,0x09	,DPTIME,0x02},	
// H1 kW MD Date
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x0E  	,DATE_VAL	,DIV_1     	,DISP_MD					,DISP_V|DISP_T3|DISP_AL+DISP_k|DISP_DOTS		,DISP_BP|DISP_DATE,0x42	,0x09	,DPTIME,0x02},
// H1 kWh
{EEP_DATA	,0x00	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100	,DISP_h						,DISP_V|DISP_T3|DISP_AL+DISP_k				,DISP_BP	,0x43	,0x09	,DPTIME,0x02},
// H1 Avg.Pf
{EEP_DATA   	,0x00  	,DIGIT_3  	,0x18  	,CHAR_VAL 	,DIV_1  	,NONE          					,DISP_DEC2						,DISP_BP	,0x44	,0x1E	,DPTIME,0x02},

// H2 kW MD
{EEP_DATA  	,0x00 	,DIGIT_3  	,0x0C  	,SHORT_VAL	,DIV_1  	,DISP_MD					,DISP_V|DISP_T3|DISP_AL+DISP_k|DISP_DEC2		,DISP_BP  	,0x45	,0x0A	,DPTIME,0x02},	
// H2 kW MD TIME
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x0E  	,TIME_VAL	,DIV_1     	,DISP_MD					,DISP_V|DISP_T3|DISP_AL+DISP_k|DISP_DOTS		,DISP_BP|DISP_TIME,0x46	,0x0A	,DPTIME,0x02},	
// H2 kW MD Date
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x0E  	,DATE_VAL	,DIV_1     	,DISP_MD					,DISP_V|DISP_T3|DISP_AL+DISP_k|DISP_DOTS		,DISP_BP|DISP_DATE,0x47	,0x0A	,DPTIME,0x02},
// H2 kWh
{EEP_DATA	,0x00	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100	,DISP_h						,DISP_V|DISP_T3|DISP_AL+DISP_k				,DISP_BP	,0x48	,0x0A	,DPTIME,0x02},
// H2 Avg.Pf
{EEP_DATA   	,0x00  	,DIGIT_3  	,0x18  	,CHAR_VAL 	,DIV_1  	,NONE          					,DISP_DEC2						,DISP_BP       	,0x49	,0x1F	,DPTIME,0x02},

// H3 kW MD
{EEP_DATA  	,0x00 	,DIGIT_3  	,0x0C  	,SHORT_VAL	,DIV_1  	,DISP_MD					,DISP_V|DISP_T3|DISP_AL+DISP_k|DISP_DEC2		,DISP_BP  	,0x4A	,0x0B	,DPTIME,0x02},	
// H3 kW MD TIME
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x0E  	,TIME_VAL	,DIV_1     	,DISP_MD					,DISP_V|DISP_T3|DISP_AL+DISP_k|DISP_DOTS		,DISP_BP|DISP_TIME,0x4B	,0x0B	,DPTIME,0x02},	
// H3 kW MD Date
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x0E  	,DATE_VAL	,DIV_1     	,DISP_MD					,DISP_V|DISP_T3|DISP_AL+DISP_k|DISP_DOTS		,DISP_BP|DISP_DATE,0x4C	,0x0B	,DPTIME,0x02},
// H3 kWh
{EEP_DATA	,0x00	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100	,DISP_h						,DISP_V|DISP_T3|DISP_AL+DISP_k				,DISP_BP	,0x4D	,0x0B	,DPTIME,0x02},
// H3 Avg.Pf
{EEP_DATA   	,0x00  	,DIGIT_3  	,0x18  	,CHAR_VAL 	,DIV_1  	,NONE          					,DISP_DEC2						,DISP_BP       	,0x4E	,0x20	,DPTIME,0x02},

// H4 kW MD
{EEP_DATA  	,0x00 	,DIGIT_3  	,0x0C  	,SHORT_VAL	,DIV_1  	,DISP_MD					,DISP_V|DISP_T3|DISP_AL+DISP_k|DISP_DEC2		,DISP_BP  	,0x4F	,0x0C	,DPTIME,0x02},	
// H4 kW MD TIME
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x0E  	,TIME_VAL	,DIV_1     	,DISP_MD					,DISP_V|DISP_T3|DISP_AL+DISP_k|DISP_DOTS		,DISP_BP|DISP_TIME,0x50	,0x0C	,DPTIME,0x02},	
// H4 kW MD Date
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x0E  	,DATE_VAL	,DIV_1     	,DISP_MD					,DISP_V|DISP_T3|DISP_AL+DISP_k|DISP_DOTS		,DISP_BP|DISP_DATE,0x51	,0x0C	,DPTIME,0x02},
// H4 kWh
{EEP_DATA	,0x00	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100	,DISP_h						,DISP_V|DISP_T3|DISP_AL+DISP_k				,DISP_BP	,0x52	,0x0C	,DPTIME,0x02},
// H4 Avg.Pf
{EEP_DATA   	,0x00  	,DIGIT_3  	,0x18  	,CHAR_VAL 	,DIV_1  	,NONE          					,DISP_DEC2						,DISP_BP       	,0x53	,0x21	,DPTIME,0x02},

// H5 kW MD
{EEP_DATA  	,0x00 	,DIGIT_3  	,0x0C  	,SHORT_VAL	,DIV_1  	,DISP_MD					,DISP_V|DISP_T3|DISP_AL+DISP_k|DISP_DEC2		,DISP_BP  	,0x54	,0x0D	,DPTIME,0x02},	
// H5 kW MD TIME
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x0E  	,TIME_VAL	,DIV_1     	,DISP_MD					,DISP_V|DISP_T3|DISP_AL+DISP_k|DISP_DEC2		,DISP_BP|DISP_TIME,0x55	,0x0D	,DPTIME,0x02},	
// H5 kW MD Date
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x0E  	,DATE_VAL	,DIV_1     	,DISP_MD					,DISP_V|DISP_T3|DISP_AL+DISP_k|DISP_DEC2		,DISP_BP|DISP_DATE,0x56	,0x0D	,DPTIME,0x02},
// H5 kWh
{EEP_DATA	,0x00	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100	,DISP_h						,DISP_V|DISP_T3|DISP_AL+DISP_k				,DISP_BP	,0x57	,0x0D	,DPTIME,0x02},
// H5 Avg.Pf
{EEP_DATA   	,0x00  	,DIGIT_3  	,0x18  	,CHAR_VAL 	,DIV_1  	,NONE          					,DISP_DEC2						,DISP_BP       	,0x58	,0x22	,DPTIME,0x02},

// H6 kW MD
{EEP_DATA  	,0x00 	,DIGIT_3  	,0x0C  	,SHORT_VAL	,DIV_1  	,DISP_MD					,DISP_V|DISP_T3|DISP_AL+DISP_k|DISP_DEC2		,DISP_BP  	,0x59	,0x0E	,DPTIME,0x02},	
// H6 kW MD TIME
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x0E  	,TIME_VAL	,DIV_1     	,DISP_MD					,DISP_V|DISP_T3|DISP_AL+DISP_k|DISP_DOTS		,DISP_BP|DISP_TIME,0x5A	,0x0E	,DPTIME,0x02},	
// H6 kW MD Date
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x0E  	,DATE_VAL	,DIV_1     	,DISP_MD					,DISP_V|DISP_T3|DISP_AL+DISP_k|DISP_DOTS		,DISP_BP|DISP_DATE,0x5B	,0x0E	,DPTIME,0x02},
// H6 kWh
{EEP_DATA	,0x00	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100	,DISP_h						,DISP_V|DISP_T3|DISP_AL+DISP_k				,DISP_BP	,0x5C	,0x0E	,DPTIME,0x02},
// H6 Avg.Pf
{EEP_DATA   	,0x00  	,DIGIT_3  	,0x18  	,CHAR_VAL 	,DIV_1  	,NONE          					,DISP_DEC2						,DISP_BP       	,0x5D	,0x23	,DPTIME,0x02},


// total tamper count
{RET_DATA   	,0xFE  	,DIGIT_1  	,0x00  	,SHORT_VAL 	,DIV_1  	,NONE 						,NONE    						,NONE        	,0x6E	,0x2B	,DPTIME,0x02},
// current month avg. pf
{RET_DATA   	,0xFE  	,DIGIT_3  	,0x00  	,CHAR_VAL 	,DIV_1  	,NONE 						,DISP_DEC2						,NONE           ,0x6F	,0x37	,DPTIME,0x02},

// tamper last OCC date
{EEP_DATA   	,0x00  	,DIGIT_6  	,0x00  	,DATE_VAL 	,DIV_1     	,NONE  						,DISP_DOTS 	           	 			,DISP_DATE      ,0x70	,0x55	,DPTIME,0x02},
// tamper last OCC time
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x00  	,TIME_VAL	,DIV_1     	,NONE  						,DISP_DOTS 	            				,DISP_TIME     	,0x71	,0x55	,DPTIME,0x02},	

// tamper last restore date
{EEP_DATA   	,0x00  	,DIGIT_6  	,0x00  	,DATE_VAL 	,DIV_1     	,NONE  						,DISP_DOTS 	            				,DISP_DATE      ,0x72	,0x56	,DPTIME,0x02},
// tamper last restore time
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x00  	,TIME_VAL	,DIV_1     	,NONE  						,DISP_DOTS 	            				,DISP_TIME      ,0x73	,0x56	,DPTIME,0x02},	

// TAMPER id occ
{MSG_T_NAME   	,0xFE  ,DIGIT_8  	,0x00  	,SHORT_VAL 	,DIV_1  	,NONE 						,NONE            					,NONE		,0x74	,0x4D	,DPTIME,0x08},
// TAMPER id res
{MSG_T_NAME   	,0xFE  ,DIGIT_8  	,0x00  	,SHORT_VAL 	,DIV_1  	,NONE 						,NONE            					,NONE		,0x75	,0x4E	,DPTIME,0x08},


//H1 kVAh
{EEP_DATA	,0x00	,DIGIT_1	,0x04	,LONG_VAL	,DIV_100	,DISP_AR|DISP_h					,DISP_k|DISP_V|DISP_AL,DISP_BP						,0x80	,0x09	,DPTIME,0x02},
//H2 kVAh
{EEP_DATA	,0x00	,DIGIT_1	,0x04	,LONG_VAL	,DIV_100	,DISP_AR|DISP_h					,DISP_k|DISP_V|DISP_AL,DISP_BP						,0x81	,0x0A	,DPTIME,0x02},
//H3 kVAh
{EEP_DATA	,0x00	,DIGIT_1	,0x04	,LONG_VAL	,DIV_100	,DISP_AR|DISP_h					,DISP_k|DISP_V|DISP_AL,DISP_BP						,0x82	,0x0B	,DPTIME,0x02},
//H4 kVAh
{EEP_DATA	,0x00	,DIGIT_1	,0x04	,LONG_VAL	,DIV_100	,DISP_AR|DISP_h					,DISP_k|DISP_V|DISP_AL,DISP_BP						,0x83	,0x0C	,DPTIME,0x02},
//H5 kVAh
{EEP_DATA	,0x00	,DIGIT_1	,0x04	,LONG_VAL	,DIV_100	,DISP_AR|DISP_h					,DISP_k|DISP_V|DISP_AL,DISP_BP						,0x84	,0x0D	,DPTIME,0x02},
//H6 kVAh
{EEP_DATA	,0x00	,DIGIT_1	,0x04	,LONG_VAL	,DIV_100	,DISP_AR|DISP_h					,DISP_k|DISP_V|DISP_AL,DISP_BP						,0x85	,0x0E	,DPTIME,0x02},

// rtc battery status
{MSG_BAT   	,0xFF  	,DIGIT_8  	,NONE  	,CHAR_VAL 	,DIV_1     	,NONE  						,NONE                 					,NONE    	,0x90	,0x59	,DPTIME,0x08},
// nvm status
{MSG_BAT   	,0x00  	,DIGIT_8  	,NONE  	,CHAR_VAL 	,DIV_1     	,NONE  						,NONE                 					,NONE    	,0x91	,0x5B	,DPTIME,0x08},
// dnld
{MSG_DNLD   	,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  						,NONE                				 	,NONE    	,0x92	,0x5C	,0x04,0x08},
//back
{MSG_BC   	,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  						,NONE                 					,NONE    	,0x93	,0x5E	,0x02,0x08},
//forward
{MSG_FR   	,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  						,NONE                 					,NONE    	,0x94	,0x5D	,0x02,0x08},
// h1 Power ON hours
{EEP_DATA	,0x00	,DIGIT_1	,0x08	,LONG_VAL	,DIV_3600	,DISP_h						,NONE		      					,DISP_BP	,0x95	,0x5F	,DPTIME,0x02},
#if(defined(IRDA_TYPE_METER_AP) && (IRDA_TYPE_METER_AP == 1))
//Serial Number from ram 
{RET_DATA   	,0xFE  	,DIGIT_1  	,NONE  	,LONG_VAL 	,DIV_1     	,NONE        					,NONE		      					,NONE		,0xF3	,0x00	,DPTIME,0x08},
//--------------------------------
//present tamper date & time
// tamper nm last OCC date
{EEP_DATA   	,0x00  	,DIGIT_6  	,0x00  	,DATE_VAL 	,DIV_1     	,NONE  						,DISP_DOTS 	      					,DISP_DATE      ,0xA0	,0x55	,DPTIME,0x02},
// tamper nm last OCC time
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x00  	,TIME_VAL	,DIV_1     	,NONE  						,DISP_DOTS 	      					,DISP_TIME     	,0xA1	,0x55	,DPTIME,0x02},	
// tamper nd last OCC date
{EEP_DATA   	,0x00  	,DIGIT_6  	,0x00  	,DATE_VAL 	,DIV_1     	,NONE  						,DISP_DOTS 	      					,DISP_DATE      ,0xA2	,0x55	,DPTIME,0x02},
// tamper nd last OCC time
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x00  	,TIME_VAL	,DIV_1     	,NONE  						,DISP_DOTS 	      					,DISP_TIME     	,0xA3	,0x55	,DPTIME,0x02},	
// tamper rev last OCC date
{EEP_DATA   	,0x00  	,DIGIT_6  	,0x00  	,DATE_VAL 	,DIV_1     	,NONE  						,DISP_DOTS 	         				,DISP_DATE      ,0xA4	,0x55	,DPTIME,0x02},
// tamper rev last OCC time
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x00  	,TIME_VAL	,DIV_1     	,NONE  						,DISP_DOTS 	            				,DISP_TIME     	,0xA5	,0x55	,DPTIME,0x02},	
// tamper earth last OCC date
{EEP_DATA   	,0x00  	,DIGIT_6  	,0x00  	,DATE_VAL 	,DIV_1     	,NONE  						,DISP_DOTS 	            				,DISP_DATE      ,0xA6	,0x55	,DPTIME,0x02},
// tamper earth last OCC time
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x00  	,TIME_VAL	,DIV_1     	,NONE  						,DISP_DOTS 	            				,DISP_TIME     	,0xA7	,0x55	,DPTIME,0x02},
// mag last OCC date
{EEP_DATA   	,0x00  	,DIGIT_6  	,0x00  	,DATE_VAL 	,DIV_1     	,NONE  						,DISP_DOTS 	          				,DISP_DATE      ,0xA8	,0x55	,DPTIME,0x02},
// mag last OCC time
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x00  	,TIME_VAL	,DIV_1     	,NONE  						,DISP_DOTS 	          				,DISP_TIME     	,0xA9	,0x55	,DPTIME,0x02},	

// MAG
{MSG_MAG 	,0xFF  	,DIGIT_2  	,NONE  	,SHORT_VAL 	,DIV_1     	,NONE  						,NONE                  	 				,NONE    	,0xAA	,0x42	,DPTIME,0x02},
// C-OPEN
{MSG_CO   	,0xFF  	,DIGIT_2  	,NONE  	,SHORT_VAL	,DIV_1     	,NONE  						,NONE                   				,NONE    	,0xAB	,0x58	,DPTIME,0x02},

// RAM H1 kWh
{RAM_KWH_H1	,0xFF	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100	,DISP_h						,DISP_V|DISP_T3|DISP_AL+DISP_k				,DISP_BP	,0xB0	,0x09	,DPTIME,0x02},
// RAM H2 kWh
{RAM_KWH_H1	,0xFF	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100	,DISP_h						,DISP_V|DISP_T3|DISP_AL+DISP_k				,DISP_BP	,0xB1	,0x0A	,DPTIME,0x02},
// RAM H3 kWh
{RAM_KWH_H1	,0xFF	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100	,DISP_h						,DISP_V|DISP_T3|DISP_AL+DISP_k				,DISP_BP	,0xB2	,0x0B	,DPTIME,0x02},
// RAM H4 kWh
{RAM_KWH_H1	,0xFF	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100	,DISP_h						,DISP_V|DISP_T3|DISP_AL+DISP_k				,DISP_BP	,0xB3	,0x0C	,DPTIME,0x02},
// RAM H5 kWh
{RAM_KWH_H1	,0xFF	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100	,DISP_h						,DISP_V|DISP_T3|DISP_AL+DISP_k				,DISP_BP	,0xB4	,0x0D	,DPTIME,0x02},
// RAM H6 kWh
{RAM_KWH_H1	,0xFF	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100	,DISP_h						,DISP_V|DISP_T3|DISP_AL+DISP_k				,DISP_BP	,0xB5	,0x0E	,DPTIME,0x02},

// RAM H1 kW MD
{RAM_KW_MD_H1  	,0xFF 	,DIGIT_3  	,0x00  	,SHORT_VAL	,DIV_1  	,DISP_MD					,DISP_V|DISP_T3|DISP_AL+DISP_k|DISP_DEC2		,DISP_BP  	,0xB6	,0x09	,DPTIME,0x02},
// RAM H2 kW MD
{RAM_KW_MD_H2  	,0xFF 	,DIGIT_3  	,0x00  	,SHORT_VAL	,DIV_1  	,DISP_MD					,DISP_V|DISP_T3|DISP_AL+DISP_k|DISP_DEC2		,DISP_BP  	,0xB7	,0x0A	,DPTIME,0x02},
// RAM H3 kW MD
{RAM_KW_MD_H3  	,0xFF 	,DIGIT_3  	,0x00  	,SHORT_VAL	,DIV_1  	,DISP_MD					,DISP_V|DISP_T3|DISP_AL+DISP_k|DISP_DEC2		,DISP_BP  	,0xB8	,0x0B	,DPTIME,0x02},
// RAM H4 kW MD
{RAM_KW_MD_H4  	,0xFF 	,DIGIT_3  	,0x00  	,SHORT_VAL	,DIV_1  	,DISP_MD					,DISP_V|DISP_T3|DISP_AL+DISP_k|DISP_DEC2		,DISP_BP  	,0xB9	,0x0C	,DPTIME,0x02},
// RAM H5 kW MD
{RAM_KW_MD_H5  	,0xFF 	,DIGIT_3  	,0x00  	,SHORT_VAL	,DIV_1  	,DISP_MD					,DISP_V|DISP_T3|DISP_AL+DISP_k|DISP_DEC2		,DISP_BP  	,0xBA	,0x0D	,DPTIME,0x02},
// RAM H6 kW MD
{RAM_KW_MD_H6  	,0xFF 	,DIGIT_3  	,0x00  	,SHORT_VAL	,DIV_1  	,DISP_MD					,DISP_V|DISP_T3|DISP_AL+DISP_k|DISP_DEC2		,DISP_BP  	,0xBB	,0x0E	,DPTIME,0x02},

//------------------------------------------------
#endif

};
#endif

const uint8_t digit[]={LCD_0,LCD_1,LCD_2,LCD_3,LCD_4,LCD_5,LCD_6,LCD_7,LCD_8,LCD_9};
static uint8_t paramIndex;
//extern uint8_t mag_permanent_save;

/************** Display data at LCD *******************************************/
void Display(uint8_t isCodeType)
{   
	uint8_t i,byteVal;
	const uint8_t *lcddReadptr;
	uint16_t loc;
	uint8_t *lcddWriteptr;
	uint32_t data;
	const struct lcdmap *dispIndex;
	uint8_t lcdbuff[10];

	dispIndex = &disp[paramIndex];	//paramID


	for(i=0;i<10;i++) 
		lcdbuff[i]=0x00;// clear buffer
		
    
	if((dispIndex->eep_addr) == 0xFF)// take data from ram
	{
		data=0;
		lcddWriteptr=(uint8_t*)&data;
		lcddReadptr=dispIndex->Data;
		switch(dispIndex->modifier)
		{
			case LONG_VAL:
			case TIME_VAL:
			case DATE_VAL:
				*lcddWriteptr|= *lcddReadptr;
				lcddReadptr++;
				lcddWriteptr++;
				*lcddWriteptr|= *lcddReadptr;
				lcddReadptr++;
				lcddWriteptr++;
			case SHORT_VAL:
				*lcddWriteptr|= *lcddReadptr;
				lcddReadptr++;
				lcddWriteptr++;
			case CHAR_VAL:
				*lcddWriteptr|= *lcddReadptr;
				break;
		}
		
		if(dispIndex->id==HIGH_RESO_KWH_INDEX)
		{
			data=data*100;
			data=data+100UL*InsSave.ZkWhCounter/METER_CONSTANT;
			
			
			
		}
		else if((dispIndex->id==HIGH_RESO_KWH_2DEC_INDEX)||(dispIndex->id==HIGH_RESO_KWH_1DEC_INDEX))
		{
			data=data+InsSave.ZkWhCounter/METER_CONSTANT;
		}
		else if((dispIndex->id>=TOD_KWH_START_INDEX)&&(dispIndex->id<=TOD_KWH_END_INDEX))
		{
			if(dispIndex->id==(TOD_KWH_START_INDEX+currentZoneID))
				data=data+InsSave.ZkWhCounter/METER_CONSTANT;		
		}		
		else if(dispIndex->id==HIGH_RESO_KVAH_INDEX)
		{
			data=data*100;
			data=data+100UL*InsSave.ZkVAhCounter/METER_CONSTANT;
			
		}
		else if(dispIndex->id==HIGH_RESO_KVAH_2DEC_INDEX)
		{
			data=data+InsSave.ZkVAhCounter/METER_CONSTANT;
		}
		else if((dispIndex->id>=TOD_KVAH_START_INDEX)&&(dispIndex->id<=TOD_KVAH_END_INDEX))
		{
			if(dispIndex->id==(TOD_KVAH_START_INDEX+currentZoneID))
				data=data+InsSave.ZkVAhCounter/METER_CONSTANT;
		}
		
		else if(dispIndex->id==HIGH_RESO_EXPKWH_INDEX)
		{
			data=data*100;
			data=data+100UL*InsSave.ExpZkWhCounter/METER_CONSTANT;
			
		}
		else if(dispIndex->id==HIGH_RESO_EXPKWH_2DEC_INDEX)
		{
			data=data+InsSave.ExpZkWhCounter/METER_CONSTANT;
		}		
		else if(dispIndex->id==HIGH_RESO_EXPKVAH_INDEX)
		{
			data=data*100;
			data=data+100UL*InsSave.ExpZkVAhCounter/METER_CONSTANT;
			
		}
		else if(dispIndex->id==HIGH_RESO_EXPKVAH_2DEC_INDEX)
		{
			data=data+InsSave.ExpZkVAhCounter/METER_CONSTANT;
		}
		else if(dispIndex->id==CUM_POWER_ON_INDEX)
		{
			data=data+InsSave.PowerOn30;
			
		}		

	}
	if((dispIndex->eep_addr) == 0xFE)// take data from function return value 
	{
		#if(defined(IRDA_TYPE_METER_AP) && (IRDA_TYPE_METER_AP == 1))
			if(dispIndex->id==0xF3)
			{
				lcdbuff[2]=gblserialnumberLCD[7];
				lcdbuff[3]=gblserialnumberLCD[6];
				lcdbuff[4]=gblserialnumberLCD[5];
				lcdbuff[5]=gblserialnumberLCD[4];
				lcdbuff[6]=gblserialnumberLCD[3];
				lcdbuff[7]=gblserialnumberLCD[2];
				lcdbuff[8]=gblserialnumberLCD[1];
				lcdbuff[9]=gblserialnumberLCD[0];
				
			}else
		#endif	
		if(dispIndex->id==0x6E)
			data=0;//getTotalTamperCount();
		else if(dispIndex->id==0x6F)
			data=0;//getAvgPf();
		else if((dispIndex->id >0xB7)&&((dispIndex->id <0xC0)))
		{
			data=getTODConsumption(dispIndex->id-0xB8,ENERGY_TYPE_KWH);
			#if (defined(LCD18D) && (LCD18D == 1))
				lcdbuff[0] |= DISP_BP;
			#endif
			
		}
		else if((dispIndex->id >0xBF)&&((dispIndex->id <0xC8)))
		{
			data=getTODConsumption(dispIndex->id-0xC0,ENERGY_TYPE_KVAH);
			#if (defined(LCD18D) && (LCD18D == 1))
				lcdbuff[0] |= DISP_BP;
			#endif
		}
	/*	else if((dispIndex->id >0x73)&&((dispIndex->id <0x76)))
		{
			data=0;//getlastEventID(dispIndex->id-0x74);
		}*/
		else if(dispIndex->id ==0xFE)
		{
			data=getProdStatusDec();		
		}		
			
	}
	else// get value from eeprom
	{
		if((dispIndex->id >0x3F)&&((dispIndex->id <0x7C)))
		{
			loc=GetBillLoc((dispIndex->id-0x40)/5);	// as it starts from id 0x40
			#if (defined(LCD18D) && (LCD18D == 1))
				lcdbuff[0] |= DISP_BP;
			#endif
		}
	/*	if((dispIndex->id >0x7F)&&((dispIndex->id <0x86)))
		{
			loc=GetBillLoc((dispIndex->id-0x80));// as it starts from id 0x80
			#if (defined(LCD18D) && (LCD18D == 1))
				lcdbuff[0] |= DISP_BP;
			#endif
		}*/
		if((dispIndex->id >=39)&&((dispIndex->id <=44)))
		{
			//loc=GetBillLoc(dispIndex->id-39);// as it starts from id 39
		}
		else if(dispIndex->id ==0x3F)
		{
			getMeterSerial(lcdbuff,SR_TYPE_LCD);
//			#if (defined(LCD18D) && (LCD18D == 1))
//				#if (defined(LCD18A) && (LCD18A == 0))
//					data=lcdbuff[7];
//					lcdbuff[7]=lcdbuff[8];
//					lcdbuff[8]=data;
//				#endif
//			#endif
			
		}
		else if((dispIndex->id >0x3C)&&((dispIndex->id <0x3F)))
		{
			#if (defined(IRDA_TYPE_METER_HP) && (IRDA_TYPE_METER_HP == 0))
				loc=TAMPER_NON_ROLLOVER_LOC;
			#endif
			#if (defined(IRDA_TYPE_METER_HP) && (IRDA_TYPE_METER_HP == 1))
				loc=TAMPER_CO_LOC;
			#endif
		}
	/*	if((dispIndex->id >0x97)&&((dispIndex->id <0x9E)))
		{
			loc=GetBillLoc((dispIndex->id-0x98));	// Export kWh as it starts from id 0x98
			#if (defined(LCD18D) && (LCD18D == 1))
				lcdbuff[0] |= DISP_BP;
			#endif
		}*/
	//	else if((dispIndex->id >0x6F)&&((dispIndex->id <0x72)))
	//		loc=TamperRecord.lastOccRes[0];
	//	else if((dispIndex->id >0x71)&&((dispIndex->id <0x74)))
	//		loc=TamperRecord.lastOccRes[1];
		else if(dispIndex->id ==0x27)
		{
			loc=RTC_BAT_STATUS_LOC;
		}
		else if(dispIndex->id ==0x95)
		{
			loc=GetBillLoc(0);// as it starts from id 0x80
			#if (defined(LCD18D) && (LCD18D == 1))
				lcdbuff[0] |= DISP_BP;
			#endif		
		}
		#if(defined(IRDA_TYPE_METER_AP) && (IRDA_TYPE_METER_AP == 1))
		else if((dispIndex->id ==0xA0)||(dispIndex->id ==0xA1))
		{
			//nm
			loc=TamperRecord.lastEventAddr[0];
		}
		else if((dispIndex->id ==0xA8)||(dispIndex->id ==0xA9))
		{
			//mag
			loc=TamperRecord.lastEventAddr[2];
		}
		else if((dispIndex->id ==0xA4)||(dispIndex->id ==0xA5))
		{
			//rev
			loc=TamperRecord.lastEventAddr[3];
		}		
		else if((dispIndex->id ==0xA2)||(dispIndex->id ==0xA3))
		{
			//nd
			loc=TamperRecord.lastEventAddr[4];
		}	
		else if((dispIndex->id ==0xA5)||(dispIndex->id ==0xA6))
		{
			//earth
			loc=TamperRecord.lastEventAddr[5];
		}		
		#endif
		
	}
	
		
	if(dispIndex->eep_addr ==0x00)// eeprom location
	{   
		loc += (dispIndex->offset & 0xFF);
		read_eeprom((uint8_t *)&data,loc,4);
		if(dispIndex->modifier == CHAR_VAL)
			data = data&0xFF;
		else if(dispIndex->modifier == SHORT_VAL)
			data = data&0xFFFF;
	}
	 
#if (defined(LCD18D) && (LCD18D == 1))
	if(dispIndex->symbol1&DISP_TIME)
	{   
		data = ConvertTimeCounterToTime(data,TIME_VAL,0);
	}
	else if(dispIndex->symbol1&DISP_DATE) 	
	{ 
		data = ConvertTimeCounterToTime(data,DATE_VAL,0);
	}
#endif
    
#if (defined(LCD18C) && (LCD18C == 1))
	if(dispIndex->symbol3&DISP_TIME)
	{   
		data = ConvertTimeCounterToTime(data,TIME_VAL,0);
	}
	else if(dispIndex->symbol3&DISP_DATE) 	
	{ 
		data = ConvertTimeCounterToTime(data,DATE_VAL,0);
	}
#endif

	
	if(dispIndex->divisor==DIV_3600)
		data=data/3600;
	else if(dispIndex->divisor==DIV_32)
		data=data/32;
	else if(dispIndex->divisor==DIV_10)
		data=data/10;
	else if(dispIndex->divisor==DIV_100)
		data=data/100;
	else if(dispIndex->divisor==DIV_1000)
		data=data/1000;
	else if(dispIndex->divisor==DIV_320)
		data=data/320;
  
				
	for(i=0; i<8; i++)
	{  
		byteVal=txtmsg[dispIndex->txtid][7-i];
			
		if((dispIndex->id==0x94)||(dispIndex->id==0x95))
		{
			if(i<2)
			{
				if(data%2)
					byteVal=txtmsg[85][7-i];
				else
					byteVal=txtmsg[86][7-i];
			}
			else
			{
				
				if((data==207)||(data==208))	// NM
					byteVal=txtmsg[63][7-i];
				else if((data==201)||(data==202))	// MAG
					byteVal=txtmsg[66][7-i];
				else if((data==51)||(data==52))	// REV
					byteVal=txtmsg[65][7-i];
				else if((data==203)||(data==204))	// ND
					byteVal=txtmsg[64][7-i];
				else if((data==69)||(data==70))	// EARTH
					byteVal=txtmsg[67][7-i];
				else if((data==251))	// C-OPEN
					byteVal=txtmsg[88][7-i];
			}
					
		}
		/*else if((dispIndex->id==0x70)||(dispIndex->id==0x71))
		{
			if(TamperRecord.lastOccRes[0]==0)
				byteVal=txtmsg[81][7-i];
		}
		else if((dispIndex->id==0x72)||(dispIndex->id==0x73))
		{
			if(TamperRecord.lastOccRes[1]==0)
				byteVal=txtmsg[82][7-i];
		}*/
		else if(dispIndex->id==0x90)
		{
			if(power_fail_state&RTC_BAT_STATUS_FLAG)
				byteVal=txtmsg[90][7-i];
		}
		else if(dispIndex->id==0x27)
		{
			if((data==0x00)&&(isCodeType!=BATTERY_MODE))
				byteVal=txtmsg[84][7-i];
		}
		#if (defined(IS_COVER_BLINK) && (IS_COVER_BLINK == 1))
		else if((isCodeType==USER_CODE)&&(dispIndex->id==0x33)&&(ModeID!=DISPLAY_MODE_PUSH))
		{
			if(TamperRecord.TamperStatusFlag & TAMPER_COVEROPEN)
			{
				if(refresh_time%2==0)
				{
					byteVal=txtmsg[0][7-i];	
				}
			}
		}
		#endif
		#if(defined(IRDA_TYPE_METER_AP) && (IRDA_TYPE_METER_AP == 1))
		else if((dispIndex->id ==0xA0)||(dispIndex->id ==0xA1))
		{
			//nm
			if(TamperRecord.lastEventAddr[0]==0)
				byteVal=txtmsg[81][7-i];
		}
		else if((dispIndex->id ==0xA8)||(dispIndex->id ==0xA9))
		{
			//mag
			if(TamperRecord.lastEventAddr[2]==0)
				byteVal=txtmsg[81][7-i];
		}
		else if((dispIndex->id ==0xA4)||(dispIndex->id ==0xA5))
		{
			//rev
			if(TamperRecord.lastEventAddr[1]==0)
				byteVal=txtmsg[81][7-i];
		}		
		else if((dispIndex->id ==0xA2)||(dispIndex->id ==0xA3))
		{
			//nd
			if(TamperRecord.lastEventAddr[4]==0)
				byteVal=txtmsg[81][7-i];
		}	
		else if((dispIndex->id ==0xA5)||(dispIndex->id ==0xA6))
		{
			//earth
			if(TamperRecord.lastEventAddr[5]==0)
				byteVal=txtmsg[81][7-i];
		}		
		else if((dispIndex->id ==0x3E)||(dispIndex->id ==0x3D))
		{
			//co-open
			if(TamperRecord.lastEventAddr[3]==0)
				byteVal=txtmsg[81][7-i];
		}		
		#endif
		
		#if (defined(LCD18D) && (LCD18D == 1))
			#if (defined(LCD18A) && (LCD18A==1))
				byteVal|=((display_flag &(~DISP_MAG)) & (1<<i)?0x80:0)|(dispIndex->symbol3&(1<<i)?0x80:0);	
			#endif
			#if (defined(LCD18D) && (LCD18A==0))
				byteVal|=((display_flag &(~DISP_MAG)) & (1<<i)?0x01:0)|(dispIndex->symbol3&(1<<i)?0x01:0);	
			#endif
			
		#endif
		#if (defined(LCD18C) && (LCD18C == 1))
			byteVal|=((display_flag &(~DISP_MAG)) & (1<<i)?0x80:0)|(dispIndex->symbol3&(1<<i)?0x80:0);	
		#endif
		
		#if (defined(LCD18D) && (LCD18D == 1))
			#if (defined(LCD18A) && (LCD18A==1))
				if(((byteVal&0x7F)==0)&&(i>=dispIndex->startdigit))
			#endif
			#if (defined(LCD18D) && (LCD18A==0))
				if(((byteVal&0xFE)==0)&&(i>=dispIndex->startdigit))
			#endif
			
		#endif
		#if (defined(LCD18C) && (LCD18C == 1))
			if(((byteVal&0x7F)==0)&&(i>=dispIndex->startdigit))
		#endif
		{
			byteVal|=digit[data % 10];

			//--------------------------------------------------
			if(data==0)//remove 0 padding
			{
				if(i>=(dispIndex->digits+dispIndex->startdigit))
				{
					#if (defined(LCD18D) && (LCD18D == 1))
						#if (defined(LCD18A) && (LCD18A==1))
							byteVal &= ~0x7F;
						#endif
						#if (defined(LCD18D) && (LCD18A==0))
							byteVal &= ~0xFE;
						#endif
						
					#endif
					#if (defined(LCD18C) && (LCD18C == 1))
						byteVal &= ~0x7F;
					#endif
				}
			}
			//--------------------------------------------------
			data = data/10;
			
		}
		if(i==7)
		{
			if((dispIndex->id==0x02)&&(mcu_flag&PH_POWER_REV))
				byteVal|=CHR__;
			if((dispIndex->id==0x03)&&(mcu_flag&NU_POWER_REV))
				byteVal|=CHR__;
			if((dispIndex->id==0x14)&&((mcu_flag&NU_POWER_REV)||(mcu_flag&PH_POWER_REV)))	
				byteVal|=CHR__;
			#if (defined(LCD18D) && (LCD18D == 0))	
			if((dispIndex->id==0x09)&&(Ins.LeadLag==1)&&(Ins.EffectiveP>STARTING_POWER_THRESHOLD_L)&&((display_flag&DISP_NM)==0)&&(Ins.PowerFactor<98))
				byteVal|=CHR__;
			#endif
		}		
		else if(i==5)
		{
			#if (defined(LCD18D) && (LCD18D == 1))
			if((dispIndex->id==0x09)&&(Ins.LeadLag==1)&&(Ins.EffectiveP>STARTING_POWER_THRESHOLD_L)&&((display_flag&DISP_NM)==0)&&(Ins.PowerFactor<98))
				byteVal|=CHR__;
			#endif
		}

		#if (defined(LCD18D) && (LCD18D == 1))
			if(i<2)
				lcdbuff[8+i]|=byteVal;
			else
				lcdbuff[i]|=byteVal;
		#else
				lcdbuff[2+i]|=byteVal;
		#endif
			
	}   
	
/*	if(TamperRecord.TamperCount[3]>0)
	{
		lcdbuff[1]|=DISP_SCO;
	}*/
	// symbol 1
	lcdbuff[1]|=dispIndex->symbol1;
	
	if(display_flag&DISP_MAG)
	{
		#if (defined(LCD18D) && (LCD18D == 1))
			lcdbuff[1]|=dispIndex->symbol1|DISP_SMAG;
		#endif
		#if (defined(LCD18C) && (LCD18C == 1))
			lcdbuff[3]|=0x80;
		#endif
		
	}
	
	
	if((isCodeType==USER_CODE)&&(Ins.Voltage>NM_THRESHOLD_VOLTAGE))
	{
		#if (defined(LCD18D) && (LCD18D == 1))
			lcdbuff[8]|=OK_BIT;
		#endif
		
		#if (defined(LCD18C) && (LCD18C == 1))
			lcdbuff[9]|=OK_BIT;
		#endif
		
	}
	else if(isCodeType==BATTERY_MODE)
	{
		#if (defined(LCD18C) && (LCD18C == 1))
			lcdbuff[1]|=VBAT_BIT;
		#endif
		
	}
	
/*	if(TamperRecord.TamperStatusFlag & TAMPER_MAG ||(mag_permanent_save==1))
		{
			#if (defined(LCD18C) && (LCD18C == 1))
//			lcdbuff[1]|=R_BIT;
			#else
				#if (defined(LCD18A) && (LCD18A == 0))
					lcdbuff[0]|=DISP_SCO;
				#else
					lcdbuff[1]|=DISP_SMAG;
					
				#endif
			#endif
		}*/

	#if (defined(R_SYMBOL_FOR_COOPEN) && (R_SYMBOL_FOR_COOPEN == 1))
		if((TamperRecord.TamperStatusFlag & TAMPER_COVEROPEN)||(TamperRecord.TamperCount[3]>0))
		{
			#if (defined(LCD18C) && (LCD18C == 1))
//			lcdbuff[1]|=R_BIT;
			#else
				#if (defined(LCD18A) && (LCD18A == 0))
					lcdbuff[0]|=DISP_SCO;
				#else
					lcdbuff[1]|=DISP_SCO;
				#endif
			#endif
		}
	#endif
	

	// symbol 2

	lcdbuff[0]|=dispIndex->symbol2;

		
	for(i=0;i<10;i++)
	{
		#if (defined(LCD18D) && (LCD18D == 1))
			#if (defined(LCD18A) && (LCD18A==1))
				byteVal=lcdbuff[i];
			#endif
			#if (defined(LCD18A) && (LCD18A==0))
				if(lcdbuff[i]&0x80)
					byteVal=0x01;
				else
					byteVal=0x00;
					
				if(lcdbuff[i]&0x40)
					byteVal|=0x02;

				if(lcdbuff[i]&0x20)
					byteVal|=0x04;
				if(lcdbuff[i]&0x10)
					byteVal|=0x08;
				if(lcdbuff[i]&0x08)
					byteVal|=0x10;
				if(lcdbuff[i]&0x04)
					byteVal|=0x20;
				if(lcdbuff[i]&0x02)
					byteVal|=0x40;
				if(lcdbuff[i]&0x01)
					byteVal|=0x80;
			#endif
				
		#elif (defined(LCD18C) && (LCD18C == 1))
			byteVal=lcdbuff[i];
		#endif			
		*(LCD_DATA_REG_PTR+pinConfig[i])=byteVal;


	}		
}

/*----------------------------------------------------------------------------*/

void Clear_Lcd(void)
{   
	uint8_t i;

	for(i=0;i<10; i++)
		*(LCD_DATA_REG_PTR+pinConfig[i])=0x00;
}

void PushButton(void)
{
	
	pushModeTime=DISPLAY_PUSH_MODE2_HOLD_TIME;
	if(ModeID!=DISPLAY_MODE_PUSH)
	{
		ModeID=DISPLAY_MODE_PUSH;
		setParaBuff(DISPLAY_MODE_PUSH);
		setParamIndex(MSG_PUSH_INDEX,1);
	}
	else
	{
		refresh_time=0xFE;
	}
	

}
void setParaBuff(uint8_t ioption)
{
	
	
	ModeID=ioption;
	if(ioption==DISPLAY_MODE_NORMAL)
	{
		if((TamperRecord.TamperStatusFlag&TAMPER_COVEROPEN)||(TamperRecord.TamperCount[3]>0))
		{
		//	ModeID=DISPLAY_MODE_AUTO_COVER;
		ModeID=DISPLAY_MODE_NORMAL;
		}
	}
	else if(ioption==DISPLAY_MODE_UOUT_NORMAL)
	{
		if((TamperRecord.TamperStatusFlag&TAMPER_COVEROPEN)||(TamperRecord.TamperCount[3]>0))
		{
			//ModeID=DISPLAY_MODE_UOUT_COVER;
			ModeID=DISPLAY_MODE_NORMAL;
		}		
	}
	#if (defined(IS_DISPLAY_CONFIGURABLE) && (IS_DISPLAY_CONFIGURABLE == 1))
		getConfiguredDP(ModeID);
		displayParamPtr=lcdRAMBuff;
	#endif
	#if (defined(IS_DISPLAY_CONFIGURABLE) && (IS_DISPLAY_CONFIGURABLE == 0))
		displayParamPtr=lcdParamIDBuff[ModeID];
	#endif
	displayParamPtr++;
	
	setParamIndex(*displayParamPtr,0);
	
	
}

void setParamIndex(uint8_t paramID,uint8_t isMsg)
{
	uint8_t para_count=sizeof(disp)/sizeof(struct lcdmap);
	
	while(para_count--)
	{
		if(disp[para_count].id==paramID)
			break;
	}
	
	paramIndex=para_count;
	refresh_time=0;
	if(isMsg)
		mcu_flag|=DISPLAY_CHANGE_FLAG;
}

void CheckParaTimeOut(void)
{
	 static uint8_t paramIndexCounter = 0;
    const struct lcdmap *dispIndex = &disp[paramIndex];  // paramID
    static uint16_t buttonPressTime = 0;  // Timer to track button press duration

	if(mcu_flag&PUSH_BUTTON_RELEASED)
	{
		PushButton();
		mcu_flag&=~PUSH_BUTTON_RELEASED;
		#if (defined(IS_UP_DWN_DISPLAY) && (IS_UP_DWN_DISPLAY == 1))
			isforward|=0x80;
			push_hold_time=0;
		#endif
        buttonPressTime = 0;  // Reset press time when button is released
    }

    // Handle button press duration in push mode
    if (mcu_flag & PUSH_BUTTON_PRESSED_FLAG) {
        // Increment the button press time if it's pressed
        if (++buttonPressTime >= 5) {  // 500ms * 10 = 5 seconds
            // Switch to normal mode if long press is detected (5 seconds)
            if (ModeID == DISPLAY_MODE_PUSH) {
                ModeID = DISPLAY_MODE_NORMAL;  // Switch to normal mode
                setParaBuff(DISPLAY_MODE_NORMAL);  // Reset the parameter buffer
                setParamIndex(MSG_AUTO_INDEX, 1);  // Reset parameter index for normal mode
                buttonPressTime = 0;  // Reset button press timer
            }
        }
    }

    // If we're in Push Mode, manage automatic scrolling
    if (ModeID == DISPLAY_MODE_PUSH) {
        // If time to refresh the display (e.g., 10 seconds interval)
        if (++refresh_time >= dispIndex->time) {
            // Reset refresh_time to 0 for the next cycle
            refresh_time = 0;

	    if (paramIndexCounter < 34) 
	    {
                // Update the display with the new parameter
                setParamIndex(displayParamPtr[0], 1);  // Pass the first byte of the parameter for display
                paramIndexCounter++;  // Increment the counter to show the next parameter
            	setNextParaSeq();
	    } 
	    else 
	    {
                // If we've gone through all 34 parameters, return to normal mode
                ModeID = DISPLAY_MODE_NORMAL;
                setParaBuff(DISPLAY_MODE_NORMAL);
                setParamIndex(MSG_AUTO_INDEX, 1);  // Show "auto" or default message
				paramIndexCounter=0;
            }
        }
	
    } else {
        // Handle normal mode timeouts and changes
	if(++refresh_time>=dispIndex->time)
	{
		setParamIndex(*displayParamPtr,0);
		
		#if (defined(IS_UP_DWN_DISPLAY) && (IS_UP_DWN_DISPLAY == 1))
		if(isforward&0x02)
			moveback();
		else
		#endif
		setNextParaSeq();
		
		
		mcu_flag&=~DISPLAY_CHANGE_FLAG;
		
	}
	

	if((mcu_flag&DISPLAY_CHANGE_FLAG)==0)	
	{
		if((ModeID==DISPLAY_MODE_PUSH) && (pushModeTime>0))
		{
			pushModeTime--;
			refresh_time=0;
		}
		else if(ModeID==DISPLAY_MODE_PUSH)
		{
			ModeID=DISPLAY_MODE_NORMAL;
			setParaBuff(DISPLAY_MODE_NORMAL);
			setParamIndex(MSG_AUTO_INDEX,1);
		}
	}
    }
}

void setNextParaSeq(void)
{
	
	displayParamPtr++;
	#if (defined(IS_DISPLAY_CONFIGURABLE) && (IS_DISPLAY_CONFIGURABLE == 1))
		if(displayParamPtr>((uint8_t*)lcdRAMBuff+lcdRAMBuff[0]))
		{
			displayParamPtr=lcdRAMBuff+1;
		}
	#endif
	
	#if (defined(IS_DISPLAY_CONFIGURABLE) && (IS_DISPLAY_CONFIGURABLE == 0))
		if(displayParamPtr>((uint8_t*)lcdParamIDBuff[ModeID]+lcdParamIDBuff[ModeID][0]))
		{
			displayParamPtr=lcdParamIDBuff[ModeID]+1;
		}
	#endif
	#if(defined(IRDA_TYPE_METER_AP) && (IRDA_TYPE_METER_AP == 0))
		if(getTotalTamperCount()>0)
		{
			if(*displayParamPtr==0x26)
				displayParamPtr++;
			if((*displayParamPtr==0x37)&&(TamperRecord.TamperStatusFlag&TAMPER_NEU_MISS)==0)
				displayParamPtr++;
			if((*displayParamPtr==0x38)&&(TamperRecord.TamperStatusFlag&TAMPER_AC_DC)==0)
				displayParamPtr++;
			if((*displayParamPtr==0x39)&&(TamperRecord.TamperStatusFlag&TAMPER_REV)==0)
				displayParamPtr++;
			if((*displayParamPtr==0x3a)&&(TamperRecord.TamperStatusFlag&TAMPER_MAG)==0)
				displayParamPtr++;
			if((*displayParamPtr==0x3b)&&(TamperRecord.TamperStatusFlag&TAMPER_EARTH)==0)
				displayParamPtr++;
			if((*displayParamPtr==0x24)&&(TamperRecord.TamperStatusFlag&TAMPER_COVEROPEN)==0)
				displayParamPtr++;
			
			if(*displayParamPtr==0x75)
				if(TamperRecord.lastOccRes[1]==0)
					displayParamPtr+=3;
			if((*displayParamPtr==0x33)&&(TamperRecord.TamperStatusFlag&TAMPER_COVEROPEN)==0)
				displayParamPtr+=3;
			
		}
		else if(*displayParamPtr==0x25)
			displayParamPtr+=16;
		#if (defined(IRDA_TYPE_METER_HP) && (IRDA_TYPE_METER_HP == 1))
		else
		{
			if((*displayParamPtr==0x33)&&(TamperRecord.TamperStatusFlag&TAMPER_COVEROPEN)==0)
				displayParamPtr+=3;		
		}
		#endif
	#endif
	
	#if(defined(IRDA_TYPE_METER_AP) && (IRDA_TYPE_METER_AP == 1))
		if(TamperRecord.TamperStatusFlag)
		{
			if(*displayParamPtr==0x26)
				displayParamPtr++;
			if((*displayParamPtr==0x37)&&(TamperRecord.TamperStatusFlag&TAMPER_NEU_MISS)==0)
				displayParamPtr+=3;
			if((*displayParamPtr==0x38)&&(TamperRecord.TamperStatusFlag&TAMPER_AC_DC)==0)
				displayParamPtr+=3;
			if((*displayParamPtr==0x39)&&(TamperRecord.TamperStatusFlag&TAMPER_REV)==0)
				displayParamPtr+=3;
				
			if((*displayParamPtr==0x3b)&&(TamperRecord.TamperStatusFlag&TAMPER_EARTH)==0)
				displayParamPtr+=3;
			if((*displayParamPtr==0x33)&&(TamperRecord.TamperStatusFlag&TAMPER_COVEROPEN)==0)
				displayParamPtr+=3;
			if((*displayParamPtr==0x3A)&&((TamperRecord.TamperStatusFlag&TAMPER_MAG)==0))
				displayParamPtr+=3;
		}
		else if(*displayParamPtr==0x25)
		{
			displayParamPtr+=1;
		}	
	#endif
		
	
	#if (defined(IS_DISPLAY_CONFIGURABLE) && (IS_DISPLAY_CONFIGURABLE == 1))
		if(displayParamPtr>((uint8_t*)lcdRAMBuff+lcdRAMBuff[0]))
		{
			displayParamPtr=lcdRAMBuff+1;
		}
	#endif
	
	#if (defined(IS_DISPLAY_CONFIGURABLE) && (IS_DISPLAY_CONFIGURABLE == 0))
		if(displayParamPtr>((uint8_t*)lcdParamIDBuff[ModeID]+lcdParamIDBuff[ModeID][0]))
		{
			displayParamPtr=lcdParamIDBuff[ModeID]+1;
		}
	#endif
	
}


uint8_t getParamTime(void)
{
	refresh_time=0xFE;
	return disp[paramIndex].time; 
}

uint8_t getParaCount(void)
{
	#if (defined(IS_DISPLAY_CONFIGURABLE) && (IS_DISPLAY_CONFIGURABLE == 1))
		return lcdRAMBuff[0]; 
	#endif
	#if (defined(IS_DISPLAY_CONFIGURABLE) && (IS_DISPLAY_CONFIGURABLE == 0))
		return lcdParamIDBuff[ModeID][0];
	#endif
}

uint8_t getMeterSerial(uint8_t* srBuff,uint8_t srType)
{
	uint8_t digitSR,srLength=0,srNumeric=0;
	uint8_t digitSRbuff[16];
	uint32_t srNumber=0;
		
	read_eeprom(digitSRbuff,SERIAL_NO_LOC,16);
	
	while(srLength<16)
	{
		digitSR=digitSRbuff[srLength];
		if(digitSR==':')
			break;

		if((SR_TYPE_LCD==srType)||(SR_TYPE_NUM==srType))
		{
			if(digitSR>='0'&&digitSR<='9')
			{
				digitSR=digitSR-0x30;
				if(SR_TYPE_LCD==srType)		
				{
					#if (defined(LCD18D) && (LCD18D == 1))
						if(srNumeric>5)
							srBuff[15-srNumeric]=digit[digitSR];
						else
							srBuff[7-srNumeric]=digit[digitSR];
					#endif
					#if (defined(LCD18C) && (LCD18C == 1))
						srBuff[9-srNumeric]=digit[digitSR];
					#endif
					
				}
				else
				{
					srBuff[srNumeric]=digitSR;
					
				}
				srNumeric=(srNumeric+1)%9;
				
			}
		}
		else
		{
			srBuff[srNumeric]=digitSRbuff[srNumeric];
			srNumeric++;
		}
			
		srLength++;
	}
	
	if(SR_TYPE_NUM==srType)
	{
		
		srNumber=srBuff[0];
		srType=1;
		while(srType<srNumeric)
		{
			srNumber=srNumber*10+srBuff[srType];
			srType++;
		}
		Data.long_data=srNumber;
	}
	else if(SR_TYPE_LCD==srType)
	{
		#if (defined(LCD18D) && (LCD18D == 1))
			#if (defined(LCD18A) && (LCD18A == 0))
				if(srNumeric==8)
				{
					// from top left-0
					digitSRbuff[0]=srBuff[7];
					digitSRbuff[1]=srBuff[6];
					digitSRbuff[2]=srBuff[5];
					digitSRbuff[3]=srBuff[4];
					digitSRbuff[4]=srBuff[3];
					digitSRbuff[5]=srBuff[2];
					digitSRbuff[6]=srBuff[9];
					digitSRbuff[7]=srBuff[8];
					
					srBuff[8]=digitSRbuff[0];
					srBuff[7]=digitSRbuff[1];
					srBuff[6]=digitSRbuff[2];
					srBuff[5]=digitSRbuff[3];
					srBuff[4]=digitSRbuff[4];
					srBuff[3]=digitSRbuff[5];
					srBuff[2]=digitSRbuff[6];
					srBuff[9]=digitSRbuff[7];
					
				}
			#endif
		#endif
	}
	
	
	
	return (srNumeric%16);
	
}
uint8_t getMeterSerial_hex(uint8_t* srBuff);
uint8_t getMeterSerial_hex(uint8_t* srBuff)
{
    uint8_t digitSRbuff[16];
    uint8_t srLength = 0;
    uint32_t srNumber = 0;
    uint8_t c;

    // Read the ASCII serial number from EEPROM
    read_eeprom(digitSRbuff, SERIAL_NO_LOC, 16);

    // Step 1: ASCII → Decimal Conversion
    while (srLength < 16)
    {
        c = digitSRbuff[srLength];

        // Stop on terminator
        if (c == ':' || c == 0xFF || c == 0x00)
            break;

        // Valid digit? (ASCII '0'..'9')
        if (c >= '0' && c <= '9')
        {
            // Convert ASCII to numeric value
            uint8_t digit = c - '0';
            // Build full decimal number
            srNumber = srNumber * 10 + digit;
        }

        srLength++;
    }

    // Step 2: Decimal → Hexadecimal bytes (in little-endian order)
    srBuff[0] = srNumber & 0xFF;            // Least significant byte (LSB)
    srBuff[1] = (srNumber >> 8) & 0xFF;     // Middle byte
    srBuff[2] = (srNumber >> 16) & 0xFF;    // Most significant byte (MSB)

    // Return the number of bytes used
    return 3;
}

/*uint8_t getMeterSerial_hex(uint8_t* srBuff)
{
    uint8_t digitSRbuff[16];
    uint8_t srLength = 0;
    uint32_t srNumber = 0;
	uint8_t c;

    // EEPROM se ASCII serial number read karo
    read_eeprom(digitSRbuff, SERIAL_NO_LOC, 16);

    // Step 1: ASCII → Decimal Conversion
    while (srLength < 16)
    {
     c = digitSRbuff[srLength];

        // Stop on terminator
        if (c == ':' || c == 0xFF || c == 0x00)
            break;

        // Valid digit? (ASCII '0'..'9')
        if (c >= '0' && c <= '9')
        {
            // convert ASCII to numeric value
            uint8_t digit = c - '0';
            // Build full decimal number
            srNumber = srNumber * 10 + digit;
        }

        srLength++;
    }

    // Step 2: Decimal → Hexadecimal bytes
    // Example: 1234567 = 0x12D687
    srBuff[0] = (srNumber >> 16) & 0xFF;
    srBuff[1] = (srNumber >> 8) & 0xFF;
    srBuff[2] = srNumber & 0xFF;

    // Return number of bytes used
    return 3;
}*/



uint8_t getMeterKitID(uint8_t* kiBuff,uint8_t srType)
{
	uint8_t digitSR,srLength=0,srNumeric=0;
	uint8_t digitSRbuff[16];
//	uint32_t srNumber=0;
		
	read_eeprom(digitSRbuff,METER_KIT_ID,16);
	
	while(srLength<16)
	{
		digitSR=digitSRbuff[srLength];
		if(digitSR==':')
			break;

		
		{
			kiBuff[srNumeric]=digitSRbuff[srNumeric];
			srNumeric++;
		}
			
		srLength++;
	}
	
	return (srNumeric%16);
	
}

#if (defined(IS_DISPLAY_CONFIGURABLE) && (IS_DISPLAY_CONFIGURABLE == 1))
void getConfiguredDP(uint8_t iMode)
{
	uint16_t DPlcd[]={AUTO_MODE_DP,PUSH_MODE_DP},loc;
	
	if(DISPLAY_MODE_NORMAL==iMode)
	{
		loc=DPlcd[0];
	}
	else if(DISPLAY_MODE_PUSH==iMode)
	{
		loc=DPlcd[1];
	}
	
	if((DISPLAY_MODE_NORMAL==iMode)||(DISPLAY_MODE_PUSH==iMode))
	{
		read_eeprom(lcdRAMBuff,loc,MAX_DISPLAY_PARA_SIZE+2);
		if(Crc_Cal(lcdRAMBuff,MAX_DISPLAY_PARA_SIZE+2))
			copyDPfromFlash2Ram(iMode);
		
	}
	else
		copyDPfromFlash2Ram(iMode);
}
void copyDPfromFlash2Ram(uint8_t imode)
{
	uint8_t paraNos;
	uint16_t totalPara=lcdParamIDBuff[imode][0]%MAX_DISPLAY_PARA_SIZE+1;
	
	
	for(paraNos=0;paraNos<MAX_DISPLAY_PARA_SIZE;paraNos++)
	{
		if(paraNos<totalPara)
			lcdRAMBuff[paraNos]=lcdParamIDBuff[imode][paraNos];
		else
			lcdRAMBuff[paraNos]=0x00;
	}
		
	totalPara=Crc_Cal(lcdRAMBuff,paraNos);
	lcdRAMBuff[paraNos]=totalPara;
	lcdRAMBuff[paraNos+1]=totalPara>>8;
	
}

void validatelcdRAMBuff(void)
{
	if(Crc_Cal(lcdRAMBuff,MAX_DISPLAY_PARA_SIZE+2))
		getConfiguredDP(ModeID);
		
}
uint8_t configDisplayPara(const uint8_t *dbuff)
{
	uint8_t  paracount;
	uint8_t  setMode;
	uint16_t DPlcd[]={AUTO_MODE_DP,PUSH_MODE_DP};
	
	if((dbuff[0]=='0')||(dbuff[0]=='1'))	// mode id
	{
		// start parameter counting
		paracount=0;
		setMode=dbuff[0]-0x30;
		dbuff++;
		while(paracount<(MAX_DISPLAY_PARA_SIZE+2))	//+2 due to 2 bytes crc
		{
			RxTxBuffer[paracount++]=ConvertPAN(dbuff);	// get no of parameters in the mode + parameters' id+crc.
			if(RxTxBuffer[0]>=MAX_DISPLAY_PARA_SIZE)
				return 0;
			dbuff+=2;
			
		}
		if(Crc_Cal(RxTxBuffer,MAX_DISPLAY_PARA_SIZE+2)==0)
		{
			if((setMode==0)||(setMode==1))
			{
				write_eeprom(RxTxBuffer,DPlcd[setMode],MAX_DISPLAY_PARA_SIZE+2);
				return 1;
			}
		}
		
		
	}
	return 0;
	
	
}
uint8_t readConfiguredDP2Send(uint8_t *bptr,uint8_t imode)
{
	uint16_t panbyte;
	uint8_t parano;
	getConfiguredDP(imode);
	
	#if(MAX_COMM_BUFFER_LENGTH<((MAX_DISPLAY_PARA_SIZE+2)*2+12))
		#error "buffer size is less than display size."
	#endif
	
	for(parano=0;parano<(MAX_DISPLAY_PARA_SIZE+2);parano++)
	{
		panbyte=makePANByte(lcdRAMBuff[parano]);
		bptr[parano*2]=panbyte>>8;
		bptr[parano*2+1]=panbyte;
	}
	
	return (parano*2);

}
void ResetConfigDP(void)
{
	uint16_t DPlcd[]={AUTO_MODE_DP,PUSH_MODE_DP};
	fillComBufferZero();
	write_eeprom(RxTxBuffer,DPlcd[0],MAX_DISPLAY_PARA_SIZE+2);
	write_eeprom(RxTxBuffer,DPlcd[1],MAX_DISPLAY_PARA_SIZE+2);
	
}
#endif

#if (defined(IS_UP_DWN_DISPLAY) && (IS_UP_DWN_DISPLAY == 1))
void moveback(void)
{
	displayParamPtr--;
	if((*displayParamPtr==0x3D)&&(TamperRecord.TamperStatusFlag&TAMPER_COVEROPEN)==0)
		displayParamPtr-=3;
	
	#if (defined(IS_DISPLAY_CONFIGURABLE) && (IS_DISPLAY_CONFIGURABLE == 0))
	if(displayParamPtr<((uint8_t*)lcdParamIDBuff[ModeID]+1))
	{
		displayParamPtr=((uint8_t*)lcdParamIDBuff[ModeID]+lcdParamIDBuff[ModeID][0]);
	}
	#endif
	#if (defined(IS_DISPLAY_CONFIGURABLE) && (IS_DISPLAY_CONFIGURABLE == 1))
	if(displayParamPtr<(lcdRAMBuff+1))
	{
		displayParamPtr=lcdRAMBuff+lcdRAMBuff[0];
	}
	#endif	
	
}
#endif
