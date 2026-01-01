//-------- include device specific files----------------
#include "..\\..\\rlDevice\\Include\\dIOdefine.h"
#include "..\\..\\rlDevice\\Include\\dI2c.h"
#include "..\\..\\rlDevice\\Include\\dUart.h"
#include "..\\..\\rlDevice\\Include\\dRtc.h"
#include "..\\..\\rlDevice\\Include\\dWatchDog.h"
#include "..\\..\\rlDevice\\Include\\dUser_includes.h"
#include "..\\..\\rlDevice\\Include\\d8bitTimer.h"
//-----------------------------------------------------

//-------- include app specific files-------------------
#include "..\\Include\\AppTampers.h"
#include "..\\Include\\AppCommunication.h"
#include "..\\Include\\AppCustomerProtocol.h"
#include "..\\Include\\AppCalibration.h"
#include "..\\Include\\AppLcd.h"
#include "..\\Include\\AppVariables.h"
#include "..\\Include\\AppEeprom.h"
#include "..\\Include\\AppMacros.h"
#include "..\\Include\\AppBlockDailyLS.h"

#include "..\\Include\\AppConfig.h"
#include "..\\Include\\AppMisc.h"
#include "..\\Include\\AppBilling.h"
#include "..\\Include\\AppTOD.h"

//-------- include dlms specific files-------------------
#include "..\\..\\rlDlms\\meter_app\\r_dlms_user_interface.h"



#if (defined(IRDA_TYPE_METER_HP) && (IRDA_TYPE_METER_HP == 1))
/*----------------------------------------------------------------------------*/
extern Unsigned32	g_Class07_Event_EntriesInUse[];
extern const uint8_t dlmsEventCode[MAX_TAMPER_COUNT][2];
const uint8_t FirmwareVersion[]= "          EEOr10";

void setComFrameBytes(uint16_t bytes);
void setComFrameBytes_hex(uint16_t bytes);
void Read_Command(uint8_t commandId );
void Read_Command_tamper(uint8_t commandId );
void AppMeter_Response(void);
void AppMeter_Response_tamper(void);
void addCheckSum(uint8_t length);
void addCheckSum_tamper(uint8_t length);
uint8_t ValidateCheckSum(uint8_t length);
uint8_t ValidateCheckSum_tamper(uint8_t length);
uint16_t ls_entries_requested=0;
uint8_t calculateChecksum(uint8_t *data, uint8_t length);
uint8_t eventtype_irda=0;
void GetDailyLoadEntry(uint16_t index);
/********************* Communication Protocol *********************************/
uint8_t irda_buff[3];
uint32_t Xtime_count, XkWh_value, XkVAh_value, XMD_value, XMD_date_value;
void CustomerProtocol_tamper(void)
{   
    uint8_t cmdID;
    cmdID=ConvertPAN((uint8_t*)&RxTxBuffer[PROTO_IDENTIFIER_TAMPER]);
    
    mcu_flag &= ~COMM_RECEIVE_ENABLE;
    if(ValidateCheckSum_tamper(ConvertPAN((uint8_t*)&RxTxBuffer[PROTO_NO_BYTES_TAMPER])))
	{
		setParamIndex(MSG_DNLD_INDEX,1);
	    if((cmdID >= START_CMD_ID)&&(cmdID <= END_CMD_ID))
	    {   
			Read_Command_tamper(cmdID);
	    }
	    else
	    { 
			RxTxBuffer[PROTO_IDENTIFIER_TAMPER]=RxTxBuffer[PROTO_IDENTIFIER_TAMPER]|0x80;
			RxTxBuffer[PROTO_DATA_BYTES_TAMPER] = 0x01;
			setComFrameBytes(1);
	    }
	}
	else
	{
			RxTxBuffer[PROTO_IDENTIFIER_TAMPER]=RxTxBuffer[PROTO_IDENTIFIER_TAMPER]|0x80;		
			RxTxBuffer[PROTO_DATA_BYTES_TAMPER] = 0x02;
			setComFrameBytes(1);
	}
	
	if(PROTO_READ_FORMAT_DATA!=cmdID)
    	AppMeter_Response_tamper();
	
    
}
void CustomerProtocol(void)
{   
    uint8_t cmdID;
	uint8_t i;
  // cmdID=ConvertPAN((uint8_t*)&RxTxBuffer[PROTO_IDENTIFIER]);
    cmdID=RxTxBuffer[PROTO_IDENTIFIER];//ConvertPAN((uint8_t*)&RxTxBuffer[PROTO_IDENTIFIER]);
	for (i = 0; i < 3; i++) 
	{
		irda_buff[i] = RxTxBuffer[7 + i];
	}
    mcu_flag &= ~COMM_RECEIVE_ENABLE;
 //if(ValidateCheckSum(ConvertPAN((uint8_t*)&RxTxBuffer[PROTO_NO_BYTES])))
    if(ValidateCheckSum(RxTxBuffer[PROTO_NO_BYTES]))
	{
		setParamIndex(MSG_DNLD_INDEX,1);
	  //  if((cmdID >= START_CMD_ID)&&(cmdID <= END_CMD_ID))
	   if((cmdID == 0x96)||(cmdID == 0x00)||(cmdID == 0x03)||(cmdID == 0x04)||(cmdID == 0x9E))

	    {   
			Read_Command(cmdID);
	    }
	    else
	    { 
			RxTxBuffer[PROTO_IDENTIFIER]=RxTxBuffer[PROTO_IDENTIFIER]|0x80;
			RxTxBuffer[PROTO_DATA_BYTES] = 0x01;
			setComFrameBytes_hex(1);
	    }
	}
	else
	{
			RxTxBuffer[PROTO_IDENTIFIER]=RxTxBuffer[PROTO_IDENTIFIER]|0x80;		
			RxTxBuffer[PROTO_DATA_BYTES] = 0x02;
			setComFrameBytes_hex(1);
	}
	
	if(PROTO_READ_FORMAT_DATA!=cmdID)
    	AppMeter_Response();
	
    
}
/*----------------------------------------------------------------------------*/

/***************** Read the data from meter ***********************************/
uint32_t bytes;
extern uint8_t current_hours_hex,current_minutes_hex,current_seconds_hex;
extern uint8_t current_day_hex,current_month_hex,current_year_hex;
uint32_t t;
uint16_t uartBlockCounter=0;
uint8_t  uartBlocked = 0;
uint16_t offset=10;
uint16_t startIdx, endIdx;

void Read_Command(uint8_t commandId )
{   
//	uint8_t j;
//	uint16_t loc;
	Apprtc rtc;
	uint16_t crc;
//	uint8_t entryCount = 0;              // How many entries written into output
    	uint16_t i=0, pos;

    switch(commandId)
    {   
        //----------------------------------------------------------------------
        case PROTO_READ_START:
		//	setComFrameBytes_hex(18);
			RxTxBuffer[0]=0x95;
			RxTxBuffer[1]=0x95;
			RxTxBuffer[2]=irda_buff[2];
			RxTxBuffer[3]=irda_buff[1];
			RxTxBuffer[4]=irda_buff[0];
			RxTxBuffer[5]=0x14;
			RxTxBuffer[6]=0x96;
        		bytes= getMeterSerial_hex((uint8_t *)&RxTxBuffer[7]);
			RxTxBuffer[10]=0x01;
			bytes= getMeterSerial_hex((uint8_t *)&RxTxBuffer[11]);
			RxTxBuffer[14]=0x51;
			RxTxBuffer[15]=0x18;
			RxTxBuffer[16]=0x00;
			if(getTotalTamperCount()>0)
				eventtype_irda=1;
			else
				eventtype_irda=0;
			RxTxBuffer[17]=eventtype_irda;
			RxTxBuffer[18]=0x1E;
			crc = calculateChecksum(&RxTxBuffer[10], 9);
			RxTxBuffer[19]=crc;
        	break;
        //----------------------------------------------------------------------
		case PROTO_READ_INSTANT:
		//	setComFrameBytes_hex(18);
			RxTxBuffer[0]=0x95;
			RxTxBuffer[1]=0x95;
			RxTxBuffer[2]=irda_buff[2];
			RxTxBuffer[3]=irda_buff[1];
			RxTxBuffer[4]=irda_buff[0];
			RxTxBuffer[5]=0x83;
			RxTxBuffer[6]=0x00;
        		bytes= getMeterSerial_hex((uint8_t *)&RxTxBuffer[7]);
		 //	Data.long_data=ConvertTimeCounterToTime(InsSave.timeCounter,TIME_VAL,&rtc);
			ConvertTimeCounterToTime(InsSave.timeCounter,TIME_VAL_HEX,&rtc);
            		RxTxBuffer[10] =current_seconds_hex;// Data.long_data & 0xFF;            // Least significant byte (LSB)
    			RxTxBuffer[11] = current_minutes_hex;//(Data.long_data >> 8) & 0xFF;     // Middle byte
   			RxTxBuffer[12] = current_hours_hex;//(Data.long_data >> 16) & 0xFF;    // Most significant byte (MSB)
		//	Data.long_data=ConvertTimeCounterToTime(InsSave.timeCounter,DATE_VAL_HEX,&rtc);
			ConvertTimeCounterToTime(InsSave.timeCounter,DATE_VAL_HEX,&rtc);
			RxTxBuffer[13] = current_year_hex;//Data.long_data & 0xFF;            // Least significant byte (LSB)
    			RxTxBuffer[14] = current_month_hex;//(Data.long_data >> 8) & 0xFF;     // Middle byte
   			RxTxBuffer[15] = current_day_hex;//(Data.long_data >> 16) & 0xFF;    // Most significant byte (MSB)
			bytes = Ins.Voltage/10;
			RxTxBuffer[16] = bytes & 0xFF;            // Least significant byte (LSB)
    			RxTxBuffer[17] = (bytes >> 8) & 0xFF;     // Middle byte
				RxTxBuffer[18] = 0x00;           
    			RxTxBuffer[19] = 0x00; 
				RxTxBuffer[20] = 0x00;            
    			RxTxBuffer[21] = 0x00; 
				bytes = Ins.EffectiveI/10;
				RxTxBuffer[22] = bytes & 0xFF;            // Least significant byte (LSB)
    			RxTxBuffer[23] = (bytes >> 8) & 0xFF;     // Middle byte
				for (i = 24; i <= 31; i++)
				{
    				RxTxBuffer[i] = 0;
				}   
				RxTxBuffer[32] = 0x00;
				RxTxBuffer[33] = 0x00;
				RxTxBuffer[34] = 0x00;
				RxTxBuffer[35] = 0x00;
				bytes = InsSave.CumkWh + (InsSave.ZkWhCounter / 32);
				if (bytes != 0)
				{
    				RxTxBuffer[32] = bytes & 0xFF;
    				RxTxBuffer[33] = (bytes >> 8) & 0xFF;
    				RxTxBuffer[34] = (bytes >> 16) & 0xFF;
    				RxTxBuffer[35] = (bytes >> 24) & 0xFF;
				}
		/*	bytes = InsSave.CumkWh+InsSave.ZkWhCounter/32;
			RxTxBuffer[32] = bytes & 0xFF;            // Least significant byte (LSB)
    			RxTxBuffer[33] = (bytes >> 8) & 0xFF;     // Middle byte
   			RxTxBuffer[34] = (bytes >> 16) & 0xFF;    // Most significant byte (MSB)
			RxTxBuffer[35] = (bytes >> 24) & 0xFF;    // Most significant byte (MSB)
			*/ 
			for (i = 36; i <= 124; i++)
			{
    				RxTxBuffer[i] = 0;
			}
			bytes = InsSave.BillMD.Current.kW;
			RxTxBuffer[125] = bytes & 0xFF;            // Least significant byte (LSB)
    			RxTxBuffer[126] = (bytes >> 8) & 0xFF;     // Middle byte
		//	Data.long_data=ConvertTimeCounterToTime(InsSave.BillMD.Current.kW_Date,DATE_VAL,&rtc);
			ConvertTimeCounterToTime(InsSave.BillMD.Current.kW_Date,DATE_VAL_HEX,&rtc);
			RxTxBuffer[127] = current_year_hex; //Data.long_data & 0xFF;            // Least significant byte (LSB)
    			RxTxBuffer[128] = current_month_hex;//(Data.long_data >> 8) & 0xFF;     // Middle byte
   			RxTxBuffer[129] = current_day_hex;//(Data.long_data >> 16) & 0xFF;    // Most significant byte (MSB)
			crc = calculateChecksum(&RxTxBuffer[10], 120);
			RxTxBuffer[130]=crc;
        	break;

		case PROTO_READ_BILL_PKT_1:

    			RxTxBuffer[0] = 0x95;
    			RxTxBuffer[1] = 0x95;
    			RxTxBuffer[2] = irda_buff[2];
    			RxTxBuffer[3] = irda_buff[1];
    			RxTxBuffer[4] = irda_buff[0];
   				RxTxBuffer[5] = 0xFB;
    			RxTxBuffer[6] = 0x03;
    			getMeterSerial_hex((uint8_t *)&RxTxBuffer[7]);
			// Daily load data
			for(i=0;i<20;i++)
    			{
         			pos = 10 + (12 * i); 
					GetDailyLoadEntry(i);
        			ConvertTimeCounterToTime(Xtime_count, DATE_VAL_HEX, &rtc);
        			// DATE
        			RxTxBuffer[pos + 0] = current_year_hex;
        			RxTxBuffer[pos + 1] = current_month_hex;
        			RxTxBuffer[pos + 2] = current_day_hex;
        			// kWh (4 bytes)
        			RxTxBuffer[pos + 3] = (XkWh_value >> 0) & 0xFF;
        			RxTxBuffer[pos + 4] = (XkWh_value >> 8) & 0xFF;
        			RxTxBuffer[pos + 5] = (XkWh_value >> 16) & 0xFF;
        			RxTxBuffer[pos + 6] = (XkWh_value >> 24) & 0xFF;
				// MD (2 bytes)
        			RxTxBuffer[pos + 7] = (XMD_value >> 0) & 0xFF;
        			RxTxBuffer[pos + 8] = (XMD_value >> 8) & 0xFF;
        			// MD Time (3 bytes: hh mm ss)
				ConvertTimeCounterToTime(XMD_date_value, TIME_VAL_HEX, &rtc);
        			RxTxBuffer[pos + 9]  = current_seconds_hex;
        			RxTxBuffer[pos + 10] = current_minutes_hex;
        			RxTxBuffer[pos + 11] = current_hours_hex;
    			}
    			// CRC 
    				crc = calculateChecksum(&RxTxBuffer[10], 240);
    				RxTxBuffer[250] = crc;
		 break;


		case PROTO_READ_BILL_PKT_2:
		//	setComFrameBytes_hex(18);
			RxTxBuffer[0]=0x95;
			RxTxBuffer[1]=0x95;
			RxTxBuffer[2]=irda_buff[2];
			RxTxBuffer[3]=irda_buff[1];
			RxTxBuffer[4]=irda_buff[0];
			RxTxBuffer[5]=0xFB;
			RxTxBuffer[6]=0x03;
        		getMeterSerial_hex((uint8_t *)&RxTxBuffer[7]);
			for(i=0;i<20;i++)
    			{
         			pos = 10 + (12 * i); 
				GetDailyLoadEntry(i+20);
        			ConvertTimeCounterToTime(Xtime_count, DATE_VAL_HEX, &rtc);
        			// DATE
        			RxTxBuffer[pos + 0] = current_year_hex;
        			RxTxBuffer[pos + 1] = current_month_hex;
        			RxTxBuffer[pos + 2] = current_day_hex;
        			// kWh (4 bytes)
        			RxTxBuffer[pos + 3] = (XkWh_value >> 0) & 0xFF;
        			RxTxBuffer[pos + 4] = (XkWh_value >> 8) & 0xFF;
        			RxTxBuffer[pos + 5] = (XkWh_value >> 16) & 0xFF;
        			RxTxBuffer[pos + 6] = (XkWh_value >> 24) & 0xFF;
				// MD (2 bytes)
        			RxTxBuffer[pos + 7] = (XMD_value >> 0) & 0xFF;
        			RxTxBuffer[pos + 8] = (XMD_value >> 8) & 0xFF;
        			// MD Time (3 bytes: hh mm ss)
        			ConvertTimeCounterToTime(XMD_date_value, TIME_VAL_HEX, &rtc);
        			RxTxBuffer[pos + 9]  = current_seconds_hex;
        			RxTxBuffer[pos + 10] = current_minutes_hex;
        			RxTxBuffer[pos + 11] = current_hours_hex;
    			}
    			// CRC 
    				crc = calculateChecksum(&RxTxBuffer[10], 240);
    				RxTxBuffer[250] = crc;
        	break;

		case PROTO_READ_CLOSE:
		
			setParamIndex(MSG_DONE_INDEX,1);
			uartBlocked=1;
			uartBlockCounter=0;
        	break;
			
       //------------ Incase invalid command ----------------------------------
        default:
            RxTxBuffer[PROTO_DATA_BYTES] = 0x01;
			setComFrameBytes_hex(1);
        break;
        //----------------------------------------------------------------------
    }
}
void Read_Command_tamper(uint8_t commandId )
{   
	uint8_t i,no_of_itr=0;
    uint32_t temp;
	uint16_t loc;
	Apprtc rtc;
	uint16_t data_position;
//	uint8_t buff_t[2];
	
    switch(commandId)
    {   
        //----------------------------------------------------------------------
        case PROTO_READ_RTC_DATA:
			
            loc=0;
            Dec_Ascii(Ins.Voltage/10,4,(uint8_t *)&RxTxBuffer[PROTO_DATA_BYTES_TAMPER],1);
			loc=loc+5;
            RxTxBuffer[PROTO_DATA_BYTES_TAMPER+loc]=FIELD_SEPERATOR;
			loc+=1;

            Dec_Ascii(Ins.EffectiveI/10,4,(uint8_t *)&RxTxBuffer[PROTO_DATA_BYTES_TAMPER+loc],2);
			loc=loc+5;
            RxTxBuffer[PROTO_DATA_BYTES_TAMPER+loc]=FIELD_SEPERATOR;
			loc+=1;
            
            Data.long_data=ConvertTimeCounterToTime(InsSave.timeCounter,DATE_VAL,&rtc);
            Dec_Ascii(Data.long_data,6,(uint8_t *)&RxTxBuffer[PROTO_DATA_BYTES_TAMPER+loc],0);
			loc=loc+6;
            RxTxBuffer[PROTO_DATA_BYTES_TAMPER+loc]=FIELD_SEPERATOR;
			loc+=1;
            
            Data.long_data=ConvertTimeCounterToTime(InsSave.timeCounter,TIME_VAL,&rtc);
            Dec_Ascii(Data.long_data,6,(uint8_t *)&RxTxBuffer[PROTO_DATA_BYTES_TAMPER+loc],0);
			loc=loc+6;
            RxTxBuffer[PROTO_DATA_BYTES_TAMPER+loc]=FIELD_SEPERATOR;
			loc+=1;
            
            Dec_Ascii(Ins.PowerFactor,3,(uint8_t *)&RxTxBuffer[PROTO_DATA_BYTES_TAMPER+loc],2);
			loc=loc+4;
            RxTxBuffer[PROTO_DATA_BYTES_TAMPER+loc]=FIELD_SEPERATOR;
			loc+=1;
            
			
            Dec_Ascii(Ins.Frequency,4,(uint8_t *)&RxTxBuffer[PROTO_DATA_BYTES_TAMPER+loc],2);
			loc=loc+5;
            RxTxBuffer[PROTO_DATA_BYTES_TAMPER+loc]=FIELD_SEPERATOR;
			loc+=1;
			
            Dec_Ascii(Ins.EffectiveP,7,(uint8_t *)&RxTxBuffer[PROTO_DATA_BYTES_TAMPER+loc],2);
			loc=loc+8;
            RxTxBuffer[PROTO_DATA_BYTES_TAMPER+loc]=FIELD_SEPERATOR;
			loc+=1;
			
            Dec_Ascii(Ins.AppPower,7,(uint8_t *)&RxTxBuffer[PROTO_DATA_BYTES_TAMPER+loc],2);
			loc=loc+8;
            RxTxBuffer[PROTO_DATA_BYTES_TAMPER+loc]=FIELD_SEPERATOR;
			loc+=1;
            
			Dec_Ascii(InsSave.MDResetCount,3,(uint8_t *)&RxTxBuffer[PROTO_DATA_BYTES_TAMPER+loc],0);
			loc=loc+3;
            RxTxBuffer[PROTO_DATA_BYTES_TAMPER+loc]=FIELD_SEPERATOR;
			loc+=1;
			Dec_Ascii(getSelftDgFeature(),1,(uint8_t *)&RxTxBuffer[PROTO_DATA_BYTES_TAMPER+loc],0);
			loc+=1;
            RxTxBuffer[PROTO_DATA_BYTES_TAMPER+loc]=FIELD_SEPERATOR;
			loc+=1;
			Dec_Ascii(getTotalTamperCount(),3,(uint8_t *)&RxTxBuffer[PROTO_DATA_BYTES_TAMPER+loc],0);
			loc+=3;
            RxTxBuffer[PROTO_DATA_BYTES_TAMPER+loc]=FIELD_SEPERATOR;
			loc+=1;
			Dec_Ascii(3600/md_period,4,(uint8_t *)&RxTxBuffer[PROTO_DATA_BYTES_TAMPER+loc],0);
			loc+=4;
            RxTxBuffer[PROTO_DATA_BYTES_TAMPER+loc]=FIELD_SEPERATOR;
			loc+=1;
			Dec_Ascii(3600/ls_period,4,(uint8_t *)&RxTxBuffer[PROTO_DATA_BYTES_TAMPER+loc],0);
			loc+=4;
            RxTxBuffer[PROTO_DATA_BYTES_TAMPER+loc]=FIELD_SEPERATOR;
			loc+=1;
			//--------------	
			setComFrameBytes(loc);
            
        break;
        //----------------------------------------------------------------------
        //----------------------------------------------------------------------
        case PROTO_READ_SERIAL_NO:
			setComFrameBytes(8);
            getMeterSerial((uint8_t *)&RxTxBuffer[PROTO_DATA_BYTES_TAMPER],SR_TYPE_ASCII);
     
        break;
        //----------------------------------------------------------------------
        //----------------------------------------------------------------------
        case PROTO_READ_MFG_ID:
			setComFrameBytes(16);
            for(i=0;i<16;i++)
              RxTxBuffer[PROTO_DATA_BYTES_TAMPER+i]=FirmwareVersion[i];
        break;
        //----------------------------------------------------------------------
        //----------------------------------------------------------------------
        case PROTO_READ_CUM_KWH:
			setComFrameBytes(9);
            Dec_Ascii(InsSave.CumkWh+InsSave.ZkWhCounter/32,8,(uint8_t *)&RxTxBuffer[PROTO_DATA_BYTES_TAMPER],2);
        break;
        //----------------------------------------------------------------------
        //----------------------------------------------------------------------
              
	   case PROTO_READ_TSTATUS_HISTORY:
            setComFrameBytes(17);
            for(i=0;i<6;i++)
            {
              read_eeprom((uint8_t *)&Data.long_data,GetBillLoc(i)+39,1);
							
              Dec_Ascii(Data.byte[0],2,(uint8_t *)&RxTxBuffer[PROTO_DATA_BYTES_TAMPER+i*3],0);
              if(i<5)
                RxTxBuffer[PROTO_DATA_BYTES_TAMPER+i*3+2]=RECORD_SEPERATOR;
            }
        break;
        //----------------------------------------------------------------------
        //----------------------------------------------------------------------
        case PROTO_READ_KWH_CON_HISTORY:
			setComFrameBytes(47);
            for(i=0;i<6;i++)
            {
              read_eeprom((uint8_t *)&Data.long_data,GetBillLoc(i)+31,4);
              Dec_Ascii(Data.long_data,6,(uint8_t *)&RxTxBuffer[PROTO_DATA_BYTES_TAMPER+i*8],2);
              if(i<5)
                RxTxBuffer[PROTO_DATA_BYTES_TAMPER+i*8+7]=RECORD_SEPERATOR;
            }        
        break;
        //----------------------------------------------------------------------
        //----------------------------------------------------------------------
        case PROTO_READ_MD_HISTORY:
			setComFrameBytes(107);
            for(i=0;i<6;i++)
            {
              read_eeprom((uint8_t *)&Data.long_data,GetBillLoc(i)+12,2);
              Dec_Ascii(Data.Short.lsb,4,(uint8_t *)&RxTxBuffer[PROTO_DATA_BYTES_TAMPER+i*18],2);
              RxTxBuffer[PROTO_DATA_BYTES_TAMPER+i*18+5]=FIELD_SEPERATOR;
              
              read_eeprom((uint8_t *)&temp,GetBillLoc(i)+14,4);
              Data.long_data=ConvertTimeCounterToTime(temp,DATE_VAL,&rtc);
              Dec_Ascii(Data.long_data,6,(uint8_t *)&RxTxBuffer[PROTO_DATA_BYTES_TAMPER+i*18+6],0);
              RxTxBuffer[PROTO_DATA_BYTES_TAMPER+i*18+12]=FIELD_SEPERATOR;
              
              Data.long_data=ConvertTimeCounterToTime(temp,TIME_VAL,&rtc);
              Dec_Ascii(Data.long_data/100,4,(uint8_t *)&RxTxBuffer[PROTO_DATA_BYTES_TAMPER+i*18+13],0);
              
              if(i<5)
                RxTxBuffer[PROTO_DATA_BYTES_TAMPER+i*18+17]=RECORD_SEPERATOR;
            }
        break;
        //----------------------------------------------------------------------
        //----------------------------------------------------------------------
				case PROTO_READ_LATEST_NM_TAMPER   :
							setComFrameBytes(119);	
						 	getCustomTamperData((uint8_t *)&RxTxBuffer[PROTO_DATA_BYTES_TAMPER],0,RxTxBuffer[PROTO_DATA_BYTES_TAMPER]);
							break;
				case PROTO_READ_LATEST_REV_TAMPER   :	
							setComFrameBytes(119);	
						 	getCustomTamperData((uint8_t *)&RxTxBuffer[PROTO_DATA_BYTES_TAMPER],1,RxTxBuffer[PROTO_DATA_BYTES_TAMPER]);
							break;
				case PROTO_READ_LATEST_EARTH_TAMPER  :
							setComFrameBytes(119);	
						 	getCustomTamperData((uint8_t *)&RxTxBuffer[PROTO_DATA_BYTES_TAMPER],2,RxTxBuffer[PROTO_DATA_BYTES_TAMPER]);
							break;
				case PROTO_READ_LATEST_CO_TAMPER  :
							setComFrameBytes(119);	
						 	getCustomTamperData((uint8_t *)&RxTxBuffer[PROTO_DATA_BYTES_TAMPER],3,RxTxBuffer[PROTO_DATA_BYTES_TAMPER]);
							break;
				case PROTO_READ_LATEST_MAG_TAMPER   :
							setComFrameBytes(119);	
						 	getCustomTamperData((uint8_t *)&RxTxBuffer[PROTO_DATA_BYTES_TAMPER],4,RxTxBuffer[PROTO_DATA_BYTES_TAMPER]);
							break;
				case PROTO_READ_LATEST_ND_TAMPER	:
							setComFrameBytes(119);	
						 	getCustomTamperData((uint8_t *)&RxTxBuffer[PROTO_DATA_BYTES_TAMPER],5,RxTxBuffer[PROTO_DATA_BYTES_TAMPER]);
							break;
							
				// Tamper All			
				
				case PROTO_READ_LATEST_NM_TAMPER_FIVE   :
							setComFrameBytes(159);	
				 			getCustomTamperData_occures((uint8_t *)&RxTxBuffer[PROTO_DATA_BYTES_TAMPER],0,RxTxBuffer[PROTO_DATA_BYTES_TAMPER]);
							break;
				
				case PROTO_READ_LATEST_REV_TAMPER_FIVE   :
							setComFrameBytes(159);	
				 			getCustomTamperData_occures((uint8_t *)&RxTxBuffer[PROTO_DATA_BYTES_TAMPER],1,RxTxBuffer[PROTO_DATA_BYTES_TAMPER]);
							break;
							
				case PROTO_READ_LATEST_EARTH_TAMPER_FIVE  :
							setComFrameBytes(159);	
						 	getCustomTamperData_occures((uint8_t *)&RxTxBuffer[PROTO_DATA_BYTES_TAMPER],2,RxTxBuffer[PROTO_DATA_BYTES_TAMPER]);
							break;
							
				case PROTO_READ_LATEST_CO_TAMPER_FIVE   :
							setComFrameBytes(159);	
						 	getCustomTamperData_occures((uint8_t *)&RxTxBuffer[PROTO_DATA_BYTES_TAMPER],3,RxTxBuffer[PROTO_DATA_BYTES_TAMPER]);
							break;
							
				case PROTO_READ_LATEST_MAG_TAMPER_FIRST_FIVE  :
							setComFrameBytes(79);	
						 	getCustomTamperData_only20mag((uint8_t *)&RxTxBuffer[PROTO_DATA_BYTES_TAMPER],4,0);
							break;
				
				case PROTO_READ_LATEST_MAG_TAMPER_SEC_FIVE    :
							setComFrameBytes(79);	
						 	getCustomTamperData_only20mag((uint8_t *)&RxTxBuffer[PROTO_DATA_BYTES_TAMPER],4,5);
							break;
				
				case PROTO_READ_LATEST_MAG_TAMPER_THIRD_FIVE  :
							setComFrameBytes(79);	
						 	getCustomTamperData_only20mag((uint8_t *)&RxTxBuffer[PROTO_DATA_BYTES_TAMPER],4,10);
							break;
				
				case PROTO_READ_LATEST_MAG_TAMPER_FOURTH_FIVE :
							setComFrameBytes(79);	
						 	getCustomTamperData_only20mag((uint8_t *)&RxTxBuffer[PROTO_DATA_BYTES_TAMPER],4,15);
							break;
				case PROTO_READ_LATEST_ND_TAMPER_FIVE   :
							setComFrameBytes(159);	
						 	getCustomTamperData_occures((uint8_t *)&RxTxBuffer[PROTO_DATA_BYTES_TAMPER],5,RxTxBuffer[PROTO_DATA_BYTES_TAMPER]);
							break;			
							
		case PROTO_READ_SW_LOG:
			loc=getCustomSWLog(&RxTxBuffer[PROTO_DATA_BYTES_TAMPER]);
			setComFrameBytes(loc);
		break;
		case PROTO_READ_FORMAT_DATA:
			loc=getFormatData(&RxTxBuffer[PROTO_DATA_BYTES_TAMPER]);
			if(received_uart==0)
			{
			#if (defined(UART_0_ENABLE) && (UART_0_ENABLE == 1))
				Meter_Response(RxTxBuffer+PROTO_DATA_BYTES_TAMPER,loc);
			#endif
			}
			#if (defined(UART_2_ENABLE) && (UART_2_ENABLE == 1))
			else
			{
				Meter2_Response(RxTxBuffer+PROTO_DATA_BYTES_TAMPER,loc);
			}
			#endif
		break;
			
       //------------ Incase invalid command ----------------------------------
        default:
            RxTxBuffer[PROTO_DATA_BYTES_TAMPER] = 0x01;
			setComFrameBytes(1);
        break;
        //----------------------------------------------------------------------
    }
}
/*----------------------------------------------------------------------------*/


/***************** Response after executing command ***************************/
void AppMeter_Response(void)
{   
    uint16_t length;
    length=RxTxBuffer[PROTO_NO_BYTES];//ConvertPAN((uint8_t*)&RxTxBuffer[PROTO_NO_BYTES])+4;
    addCheckSum(length);
    //length=length+7;
    //RxTxBuffer[length-2]=0x0D;   
    //RxTxBuffer[length-1]=0x0A;
	if(received_uart==0)
	{
	#if (defined(UART_0_ENABLE) && (UART_0_ENABLE == 1))
		Meter_Response(RxTxBuffer,length);
	#endif
	}
	#if (defined(UART_2_ENABLE) && (UART_2_ENABLE == 1))
	else
	{
		Meter2_Response(RxTxBuffer,length);
	}
	#endif
	
    
}
void AppMeter_Response_tamper(void)
{   
    uint16_t length;
    length=ConvertPAN((uint8_t*)&RxTxBuffer[PROTO_NO_BYTES_TAMPER])+4;
    addCheckSum_tamper(length);
    length=length+7;
    RxTxBuffer[length-2]=0x0D;
    RxTxBuffer[length-1]=0x0A;
	if(received_uart==0)
	{
	#if (defined(UART_0_ENABLE) && (UART_0_ENABLE == 1))
		Meter_Response(RxTxBuffer,length);
	#endif
	}
	#if (defined(UART_2_ENABLE) && (UART_2_ENABLE == 1))
	else
	{
		Meter2_Response(RxTxBuffer,length);
	}
	#endif
	
    
}
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
uint8_t Hex_BCD(uint8_t convert)
{   
    return (convert %10+((convert / 10)*16));
}
/*----------------------------------------------------------------------------*/


void addCheckSum_tamper(uint8_t length)
{
    uint8_t i,first_byte;
    uint16_t second_byte=0;
	
    for(i=0;i<length;i++)
      second_byte += RxTxBuffer[PROTO_NO_BYTES_TAMPER+i];
    
    
	first_byte=second_byte;
    first_byte = ~first_byte;   // 2's
    first_byte += 1;
	
    
    RxTxBuffer[length+1] = first_byte>>4;
    if(RxTxBuffer[length+1] >9)
        RxTxBuffer[length+1] += 0x37;
    else
        RxTxBuffer[length+1] += 0x30;
      
    
    
    RxTxBuffer[length+2] = first_byte & 0x0F;
    if(RxTxBuffer[length+2] >9)
        RxTxBuffer[length+2] += 0x37;
    else
        RxTxBuffer[length+2] += 0x30;

    first_byte = ~first_byte;
    
    
    RxTxBuffer[length+3] = first_byte>>4;
    if(RxTxBuffer[length+3] >9)
        RxTxBuffer[length+3] += 0x37;
    else
        RxTxBuffer[length+3] += 0x30;
    
    
    RxTxBuffer[length+4] = first_byte & 0x0F;
    if(RxTxBuffer[length+4] >9)
        RxTxBuffer[length+4] += 0x37;
    else
        RxTxBuffer[length+4] += 0x30;
  
}
void addCheckSum(uint8_t length)
{
    uint8_t i;
    uint16_t sum = 0;
    uint8_t checksum;

    // Step 1: Sum the data bytes in RxTxBuffer from PROTO_NO_BYTES to PROTO_NO_BYTES + length
    for (i = 0; i < length; i++) {
        sum += RxTxBuffer[PROTO_NO_BYTES + i];
    }

    // Step 2: Apply 2's complement (invert bits and add 1) to the sum
    checksum = ~sum;  // 2's complement (invert the bits)
    checksum += 1;    // Add 1 to complete the 2's complement

    // Step 3: Store the checksum directly in RxTxBuffer (raw hex byte)
    RxTxBuffer[length + 1] = checksum;
}

uint8_t ValidateCheckSum(uint8_t length) {
    // Each byte assumed 0x00 ? one's complement = 0xFF
    uint16_t sum = 0;
	uint8_t i;
    for (i = 0; i < length; i++) {
        sum += 0xFF;  // 0xFF - 0x00 = 0xFF
    }
    return (uint8_t)(sum & 0xFF);  // Keep lower 8 bits
}

uint8_t calculateChecksum(uint8_t *data, uint8_t length)
{
    uint8_t checksum = 0;
	uint8_t i;
    for(i = 0; i < length; i++)
    {
        checksum += ~(data[i]);  // bitwise NOT of each byte, then sum
    }
    return checksum;
}
uint8_t ValidateCheckSum_tamper(uint8_t length)
{
    uint8_t i,first_byte,ascii_0,ascii_1,ascii_2,ascii_3;
    uint16_t sum=0;
	
	length+=4;
    for(i=0;i<length;i++)
      sum += RxTxBuffer[PROTO_NO_BYTES_TAMPER+i];
    
	first_byte = sum;
    first_byte = ~first_byte;   // 2's
    first_byte += 1;
	
   
    
    ascii_0 = first_byte>>4;
    if(ascii_0 >9)
        ascii_0 += 0x37;
    else
        ascii_0 += 0x30;
      
    
    
    ascii_1 = first_byte & 0x0F;
    if(ascii_1 >9)
        ascii_1 += 0x37;
    else
        ascii_1 += 0x30;

    first_byte = ~first_byte;
    
    
    ascii_2 = first_byte>>4;
    if(ascii_2 >9)
        ascii_2 += 0x37;
    else
        ascii_2 += 0x30;
    
    
    ascii_3 = first_byte & 0x0F;
    if(ascii_3 >9)
        ascii_3 += 0x37;
    else
        ascii_3 += 0x30;
		
	if((ascii_0==RxTxBuffer[PROTO_NO_BYTES_TAMPER+length])&&(ascii_1==RxTxBuffer[PROTO_NO_BYTES_TAMPER+length+1])&&(ascii_2==RxTxBuffer[PROTO_NO_BYTES_TAMPER+length+2])&&(ascii_3==RxTxBuffer[PROTO_NO_BYTES_TAMPER+length+3]))
		return 1;
	else
		return 0;
}

uint8_t ConvertPAN(const uint8_t * bptr)
{
    uint8_t byteValue;
      if(bptr[1] >0x40)
        byteValue=bptr[1]- 0x37;
    else
        byteValue= bptr[1]-0x30;          
    
    if(bptr[0] >0x40)
        byteValue=  ((bptr[0]- 0x37))*16+byteValue;
    else
        byteValue=  (bptr[0]-0x30)*16+byteValue;  
    return byteValue;
}



/*uint8_t getCustomTODZonedata(uint8_t* bptr,uint8_t zone_no)
{
	Apprtc rtc;
	uint16_t loc;
	
	//kwh
	loc=0;
	if(zone_no==currentZoneID)
		Dec_Ascii(InsSave.TODEnergy.TOD[zone_no].kWh+InsSave.ZkWhCounter/METER_CONSTANT,8,bptr+loc,0);
	else
		Dec_Ascii(InsSave.TODEnergy.TOD[zone_no].kWh,8,bptr+loc,0);
	loc+=8;
	bptr[loc]=FIELD_SEPERATOR;
	loc+=1;
	
	// kVAh
	if(zone_no==currentZoneID)
		Dec_Ascii(InsSave.TODEnergy.TOD[zone_no].kVAh+InsSave.ZkVAhCounter/METER_CONSTANT,8,bptr+loc,0);
	else
		Dec_Ascii(InsSave.TODEnergy.TOD[zone_no].kVAh,8,bptr+loc,0);
	loc+=8;
	bptr[loc]=FIELD_SEPERATOR;
	loc+=1;
	
	// kW MD
	Dec_Ascii(InsSave.TODEnergy.TOD[zone_no].TODMD.kW,4,bptr+loc,0);
	loc+=4;
	bptr[loc]=FIELD_SEPERATOR;
	loc+=1;
	
	//kW MD Date
    Data.long_data=ConvertTimeCounterToTime(InsSave.TODEnergy.TOD[zone_no].TODMD.kW_Date,DATE_VAL,&rtc);
    Dec_Ascii(Data.long_data,6,bptr+loc,0);
	loc+=6;
	bptr[loc]=FIELD_SEPERATOR;
	loc+=1;
	//kW MD Time
  	Data.long_data=ConvertTimeCounterToTime(InsSave.TODEnergy.TOD[zone_no].TODMD.kW_Date,TIME_VAL,&rtc);
    Dec_Ascii(Data.long_data/100,4,bptr+loc,0);
	loc+=4;
	bptr[loc]=RECORD_SEPERATOR;
	loc+=1;
	//35
	return loc;
	
}*/

/*uint8_t getCustomHistorydata(uint8_t* bptr,uint8_t history_no)
{
	uint8_t buff[BILLING_DATA_LENGTH];
	Apprtc rtc;
	uint16_t loc;
	
	if(history_no==0)
	{
		loc=0;
		
		//billing Date
	    Data.long_data=ConvertTimeCounterToTime(InsSave.timeCounter,DATE_VAL,&rtc);
	    Dec_Ascii(Data.long_data,6,bptr+loc,0);
		loc+=6;
		bptr[loc]=FIELD_SEPERATOR;
		loc+=1;
		//billing Time
	  	Data.long_data=ConvertTimeCounterToTime(InsSave.timeCounter,TIME_VAL,&rtc);
	    Dec_Ascii(Data.long_data/100,4,bptr+loc,0);
		loc+=4;
		bptr[loc]=FIELD_SEPERATOR;
		loc+=1;
		
		
		Dec_Ascii(InsSave.CumkWh+InsSave.ZkWhCounter/METER_CONSTANT,8,bptr+loc,0);
		loc+=8;
		bptr[loc]=FIELD_SEPERATOR;
		loc+=1;
		// kVAh
		Dec_Ascii(InsSave.CumkVAh+InsSave.ZkVAhCounter/METER_CONSTANT,8,bptr+loc,0);
		loc+=8;
		bptr[loc]=FIELD_SEPERATOR;
		loc+=1;
		// kW MD
		Dec_Ascii(InsSave.BillMD.Current.kW,4,bptr+loc,0);
		loc+=4;
		bptr[loc]=FIELD_SEPERATOR;
		loc+=1;
		
		//kW MD Date
	    Data.long_data=ConvertTimeCounterToTime(InsSave.BillMD.Current.kW_Date,DATE_VAL,&rtc);
	    Dec_Ascii(Data.long_data,6,bptr+loc,0);
		loc+=6;
		bptr[loc]=FIELD_SEPERATOR;
		loc+=1;
		//kW MD Time
	  	Data.long_data=ConvertTimeCounterToTime(InsSave.BillMD.Current.kW_Date,TIME_VAL,&rtc);
	    Dec_Ascii(Data.long_data/100,4,bptr+loc,0);
		loc+=4;
		//46
		bptr[loc]=FIELD_SEPERATOR;
		loc+=1;
		Dec_Ascii((InsSave.MonthPowerOnDuration+InsSave.PowerOn30)/60,8,bptr+loc,0);
		loc+=8;
		bptr[loc]=FIELD_SEPERATOR;
		loc+=1;
		Dec_Ascii(getAvgPf(),3,bptr+loc,1);
		loc+=4;
	}
	else
	{
		loc=GetBillLoc(history_no-1);
		read_eeprom(buff,loc,BILLING_DATA_LENGTH);
		
		loc=0;
		//billing Date
	    Data.long_data=ConvertTimeCounterToTime(getByteValue(buff,25,4),DATE_VAL,&rtc);
	    Dec_Ascii(Data.long_data,6,bptr+loc,0);
		loc+=6;
		bptr[loc]=FIELD_SEPERATOR;
		loc+=1;
		//billing Time
	  	Data.long_data=ConvertTimeCounterToTime(getByteValue(buff,25,4),TIME_VAL,&rtc);
	    Dec_Ascii(Data.long_data/100,4,bptr+loc,0);
		loc+=4;
		bptr[loc]=FIELD_SEPERATOR;
		loc+=1;
		
		//kwh
		Dec_Ascii(getByteValue(buff,0,4),8,bptr+loc,0);
		loc+=8;
		bptr[loc]=FIELD_SEPERATOR;
		loc+=1;
		// kVAh
		Dec_Ascii(getByteValue(buff,4,4),8,bptr+loc,0);
		loc+=8;
		bptr[loc]=FIELD_SEPERATOR;
		loc+=1;
		// kW MD
		Dec_Ascii(getByteValue(buff,12,2),4,bptr+loc,0);
		loc+=4;
		bptr[loc]=FIELD_SEPERATOR;
		loc+=1;
		
		//kW MD Date
	    Data.long_data=ConvertTimeCounterToTime(getByteValue(buff,14,4),DATE_VAL,&rtc);
	    Dec_Ascii(Data.long_data,6,bptr+loc,0);
		loc+=6;
		bptr[loc]=FIELD_SEPERATOR;
		loc+=1;
		//kW MD Time
	  	Data.long_data=ConvertTimeCounterToTime(getByteValue(buff,14,4),TIME_VAL,&rtc);
	    Dec_Ascii(Data.long_data/100,4,bptr+loc,0);
		loc+=4;
		//46
		bptr[loc]=FIELD_SEPERATOR;
		loc+=1;
		Dec_Ascii(getByteValue(buff,8,4)/60,8,bptr+loc,0);
		loc+=8;
		bptr[loc]=FIELD_SEPERATOR;
		loc+=1;
		Dec_Ascii(getByteValue(buff,24,1),3,bptr+loc,1);
		loc+=4;		
	}
	return loc;	
}
*/
/*uint8_t getCustomTODZoneHistorydata(uint8_t* bptr,uint8_t zone_no,uint8_t history_no)
{
	uint8_t buff[ZONE_LEN];
	Apprtc rtc;
	uint16_t loc=GetTODLatestBillLoc(history_no);
	loc=loc+zone_no*ZONE_LEN;
	read_eeprom(buff,loc,ZONE_LEN);
	//kwh
	loc=0;
	Dec_Ascii(getByteValue(buff,12,4),8,bptr+loc,0);
	loc+=8;
	bptr[loc]=FIELD_SEPERATOR;
	loc+=1;
	// kVAh
	Dec_Ascii(getByteValue(buff,16,4),8,bptr+loc,0);
	loc+=8;
	bptr[loc]=FIELD_SEPERATOR;
	loc+=1;
	// kW MD
	Dec_Ascii(getByteValue(buff,0,2),4,bptr+loc,0);
	loc+=4;
	bptr[loc]=FIELD_SEPERATOR;
	loc+=1;
	
	//kW MD Date
    Data.long_data=ConvertTimeCounterToTime(getByteValue(buff,4,4),DATE_VAL,&rtc);
    Dec_Ascii(Data.long_data,6,bptr+loc,0);
	loc+=6;
	bptr[loc]=FIELD_SEPERATOR;
	loc+=1;
	//kW MD Time
  	Data.long_data=ConvertTimeCounterToTime(getByteValue(buff,4,4),TIME_VAL,&rtc);
    Dec_Ascii(Data.long_data/100,4,bptr+loc,0);
	loc+=4;
	bptr[loc]=RECORD_SEPERATOR;
	loc+=1;
	//35
	return loc;
	
}*/

//getCustomTamperData((uint8_t *)&RxTxBuffer[PROTO_DATA_BYTES],commandId-PROTO_READ_LATEST_NM_TAMPER,RxTxBuffer[PROTO_DATA_BYTES]);

//#if 0
// this is the original function
uint8_t getCustomTamperData(uint8_t *bptr,uint8_t TamperNo,uint8_t eventno)
{
	uint8_t buff[TAMPER_DATA_LENGTH];// currently 25
	Apprtc rtc;
	int i;
	uint8_t loc_temp=0;

	for(i =0; i<10; i++)
	{
	uint16_t loc=GetTamperForwardLoc(i,TamperNo);
	read_eeprom(buff,loc,TAMPER_DATA_LENGTH);
	
	loc=0;
	//Tamper Date
    Data.long_data=ConvertTimeCounterToTime(getByteValue(buff,0,4),DATE_VAL,&rtc);
    Dec_Ascii(Data.long_data,6,bptr+loc_temp,0);
	  loc+=6; // 36 , now 6
		loc_temp+=6;
	  bptr[loc_temp]=FIELD_SEPERATOR;
	  loc+=1;	// 37	, now 7
		loc_temp+=1;
	//Tamper Time
  	Data.long_data=ConvertTimeCounterToTime(getByteValue(buff,0,4),TIME_VAL,&rtc);
    Dec_Ascii(Data.long_data/100,4,bptr+loc_temp,0);
	  loc+=4;	// now 11
		loc_temp+=4;
		bptr[loc_temp]=RECORD_SEPERATOR;
		loc+=1;	// now 12
		loc_temp+=1;
	//43
	}
	return loc_temp;
}
//#endif

uint8_t getCustomTamperData_occures(uint8_t *bptr,uint8_t TamperNo,uint8_t eventno)
{
	uint8_t buff[TAMPER_DATA_LENGTH];// currently 25
	Apprtc rtc;
	int i;
	uint8_t loc_temp=0;

	for(i =0; i<10; i++)
	{
	uint16_t loc=GetTamperForwardLoc(i,TamperNo);
	read_eeprom(buff,loc,TAMPER_DATA_LENGTH);
	
	  loc=0;
	//Tamper Date
    Data.long_data=ConvertTimeCounterToTime(getByteValue(buff,0,4),DATE_VAL,&rtc);
    Dec_Ascii(Data.long_data,6,bptr+loc_temp,0);
	  loc+=6; // 36 , now 6
		loc_temp+=6; //6
	  bptr[loc_temp]=FIELD_SEPERATOR;
	  loc+=1;	// 37	, now 7
		loc_temp+=1; //7
	//Tamper Time
  	Data.long_data=ConvertTimeCounterToTime(getByteValue(buff,0,4),TIME_VAL,&rtc);
    Dec_Ascii(Data.long_data/100,4,bptr+loc_temp,0);
	  loc+=4;	
		loc_temp+=4; //11
		bptr[loc_temp]=FIELD_SEPERATOR;
	  loc+=1;	
		loc_temp+=1; //12
	// Event type	
		Dec_Ascii(getByteValue(buff,4,2),3,bptr+loc_temp,0);
	  loc+=3;
		loc_temp+=3; //15
		bptr[loc_temp]=RECORD_SEPERATOR;
		loc+=1;
		loc_temp+=1; //16

	}
	return loc_temp;
}

uint8_t getCustomTamperData_only20mag(uint8_t *bptr,uint8_t TamperNo,uint8_t eventno)
{
	uint8_t buff[TAMPER_DATA_LENGTH];// currently 25
	Apprtc rtc;
	int i;
	uint8_t loc_temp=0;
	uint8_t length=5;
	if(eventno==5)length=10;
	if(eventno==10)length=15;
	if(eventno==15)length=20;

	for(i =eventno; i<length; i++)
	{
	uint16_t loc=GetTamperForwardLoc(i,TamperNo);
	read_eeprom(buff,loc,TAMPER_DATA_LENGTH);
	
	  loc=0;
	//Tamper Date
    Data.long_data=ConvertTimeCounterToTime(getByteValue(buff,0,4),DATE_VAL,&rtc);
    Dec_Ascii(Data.long_data,6,bptr+loc_temp,0);
	  loc+=6; // 36 , now 6
		loc_temp+=6; //6
	  bptr[loc_temp]=FIELD_SEPERATOR;
	  loc+=1;	// 37	, now 7
		loc_temp+=1; //7
	//Tamper Time
  	Data.long_data=ConvertTimeCounterToTime(getByteValue(buff,0,4),TIME_VAL,&rtc);
    Dec_Ascii(Data.long_data/100,4,bptr+loc_temp,0);
	  loc+=4;	
		loc_temp+=4; //11
		bptr[loc_temp]=FIELD_SEPERATOR;
	  loc+=1;	
		loc_temp+=1; //12
	// Event type	
		Dec_Ascii(getByteValue(buff,4,2),3,bptr+loc_temp,0);
	  loc+=3;
		loc_temp+=3; //15
		bptr[loc_temp]=RECORD_SEPERATOR;
		loc+=1;
		loc_temp+=1; //16

	}
	return loc_temp;
}


#if 0
// this is the original function
uint8_t getCustomTamperData(uint8_t *bptr,uint8_t TamperNo,uint8_t eventno)
{


	uint8_t buff[TAMPER_DATA_LENGTH];
	Apprtc rtc;
	uint16_t loc=GetTamperForwardLoc(eventno,TamperNo);
	read_eeprom(buff,loc,TAMPER_DATA_LENGTH);
	
	loc=0;
	// event type
	Dec_Ascii(getByteValue(buff,4,2),3,bptr+loc,0);
	loc+=3;
	bptr[loc]=FIELD_SEPERATOR;
	loc+=1;

	//Voltage
	Dec_Ascii(getByteValue(buff,14,2)/10,4,bptr+loc,1);
	loc+=5;
    bptr[loc]=FIELD_SEPERATOR;
	loc+=1;

	//Current 
    Dec_Ascii(getByteValue(buff,6,4)/10,4,bptr+loc,2);
	loc+=5;
    bptr[loc]=FIELD_SEPERATOR;
	loc+=1;
	
	//power Factor
    Dec_Ascii(getByteValue(buff,16,1),3,bptr+loc,2);
	loc+=4;
    bptr[loc]=FIELD_SEPERATOR;
	loc+=1;

	//kwh
	Dec_Ascii(getByteValue(buff,17,4),8,bptr+loc,0);
	loc+=8;
	bptr[loc]=FIELD_SEPERATOR;
	loc+=1;
	
	//Tamper Date
    Data.long_data=ConvertTimeCounterToTime(getByteValue(buff,0,4),DATE_VAL,&rtc);
    Dec_Ascii(Data.long_data,6,bptr+loc,0);
	loc+=6;
	bptr[loc]=FIELD_SEPERATOR;
	loc+=1;
	//Tamper Time
  	Data.long_data=ConvertTimeCounterToTime(getByteValue(buff,0,4),TIME_VAL,&rtc);
    Dec_Ascii(Data.long_data,6,bptr+loc,0);
	loc+=6;
	//43
	return loc;
	
	
}

#endif


uint8_t getFormatData(uint8_t *bptr)
{
	
	Apprtc rtc;
	uint8_t no_of_bytes;
	getMeterSerial(bptr,SR_TYPE_ASCII);
	no_of_bytes=10;

	//Current Date
    Data.long_data=ConvertTimeCounterToTime(InsSave.timeCounter,DATE_VAL,&rtc);
    Dec_Ascii(Data.long_data,6,bptr+no_of_bytes,0);
	no_of_bytes+=6;
	//Current Time
  	Data.long_data=ConvertTimeCounterToTime(InsSave.timeCounter,TIME_VAL,&rtc);
    Dec_Ascii(Data.long_data/100,4,bptr+no_of_bytes,0);
	no_of_bytes+=4;
	
	//Make
	bptr[no_of_bytes++]='E';
	bptr[no_of_bytes++]='E';
	bptr[no_of_bytes++]='P';
	bptr[no_of_bytes++]='L';
	
	//KNO
	//Data.long_data=1234567890;
	getKNO(bptr+no_of_bytes);
	//Dec_Ascii(Data.long_data,10,bptr+no_of_bytes,0);
	no_of_bytes+=10;
	
	//kWh
	Data.long_data=InsSave.CumkWh+InsSave.ZkWhCounter/METER_CONSTANT;
	Data.long_data=Data.long_data/100;
	Dec_Ascii(Data.long_data,5,bptr+no_of_bytes,0);	
	no_of_bytes+=5;
	//kVAh
	Data.long_data=InsSave.CumkVAh+InsSave.ZkVAhCounter/METER_CONSTANT;
	Data.long_data=Data.long_data/100;
	Dec_Ascii(Data.long_data,5,bptr+no_of_bytes,0);	
	no_of_bytes+=5;	
	
	//kW MD
	Dec_Ascii(InsSave.BillMD.Current.kW/10,3,bptr+no_of_bytes,1);	
	no_of_bytes+=4;	
	
	//Power off Count
	Dec_Ascii(InsSave.PFailCount,4,bptr+no_of_bytes,0);	
	no_of_bytes+=4;	

	//Voltage
	Dec_Ascii(Ins.Voltage/100,3,bptr+no_of_bytes,0);	
	no_of_bytes+=3;	
	
	//Current
	Dec_Ascii(Ins.EffectiveI/100,3,bptr+no_of_bytes,1);	
	no_of_bytes+=4;	
	
	//Power factor
	Dec_Ascii(Ins.PowerFactor,3,bptr+no_of_bytes,2);	
	no_of_bytes+=4;	
	//rev
	no_of_bytes+=getTamperCustomdata(3,bptr+no_of_bytes);
	//nm
	no_of_bytes+=getTamperCustomdata(0,bptr+no_of_bytes);
	//mag
	no_of_bytes+=getTamperCustomdata(2,bptr+no_of_bytes);
	
	//high spark
	no_of_bytes+=getTamperCustomdata(1,bptr+no_of_bytes);
	//nd
	no_of_bytes+=getTamperCustomdata(4,bptr+no_of_bytes);
	
	bptr[no_of_bytes++]='E';
	return no_of_bytes;
	
}

uint8_t getTamperCustomdata(uint8_t TamperNo,uint8_t * bptr)
{
	uint16_t loc;
	Apprtc rtc;
	uint8_t no_of_bytes=0,isRes=0;
	uint8_t buff[TAMPER_DATA_LENGTH*2];
	
	buff[4]=0x00;
	buff[5]=0x00;

	buff[TAMPER_DATA_LENGTH+4]=0x00;
	buff[TAMPER_DATA_LENGTH+5]=0x00;
	
	loc=GetTamperForwardLoc(0,TamperNo);
	read_eeprom(buff,loc,TAMPER_DATA_LENGTH);

	loc=GetTamperForwardLoc(1,TamperNo);
	read_eeprom(buff+TAMPER_DATA_LENGTH,loc,TAMPER_DATA_LENGTH);
	
	isRes=0;
	if(dlmsEventCode[TamperNo][0]==getByteValue(buff,4,2))
		isRes=0;
	else if(dlmsEventCode[TamperNo][1]==getByteValue(buff,4,2))
		isRes=1;
	
	//Occ
	//Tamper Date
    Data.long_data=ConvertTimeCounterToTime(getByteValue(buff,TAMPER_DATA_LENGTH*isRes,4),DATE_VAL,&rtc);
    Dec_Ascii(Data.long_data,6,bptr+no_of_bytes,0);
	no_of_bytes+=6;
	//Tamper Time
  	Data.long_data=ConvertTimeCounterToTime(getByteValue(buff,TAMPER_DATA_LENGTH*isRes,4),TIME_VAL,&rtc);
    Dec_Ascii(Data.long_data/100,4,bptr+no_of_bytes,0);
	no_of_bytes+=4;	
	

	if(isRes==0)
		isRes=1;
	else
		isRes=0;
	//Rec
	//Tamper Date
    Data.long_data=ConvertTimeCounterToTime(getByteValue(buff,TAMPER_DATA_LENGTH*isRes,4),DATE_VAL,&rtc);
    Dec_Ascii(Data.long_data,6,bptr+no_of_bytes,0);
	no_of_bytes+=6;
	//Tamper Time
  	Data.long_data=ConvertTimeCounterToTime(getByteValue(buff,TAMPER_DATA_LENGTH*isRes,4),TIME_VAL,&rtc);
    Dec_Ascii(Data.long_data/100,4,bptr+no_of_bytes,0);
	no_of_bytes+=4;	
	
    Dec_Ascii(TamperRecord.TamperCount[TamperNo],4,bptr+no_of_bytes,0);
	no_of_bytes+=4;	
	return no_of_bytes;
	
	
}
void setKNO(const uint8_t * bptr)
{
	write_eeprom(bptr,KNO_LOC,10);
}
uint8_t validateBillDay(uint8_t billday)
{
	if((billday<1)||(billday>31))
		billday=24;
	return billday;
}
void setBillingDate(const uint8_t * bptr)
{
	uint8_t billday;
	billday=ConvertPAN(bptr);
	validateBillDay(billday);
	write_eeprom((uint8_t*)&billday,PRE_BILL_DATE,1);
	
}
uint16_t getBillingDate(void)
{
	uint8_t billday;
	read_eeprom((uint8_t*)&billday,PRE_BILL_DATE,1);
	billday=validateBillDay(billday);
	
	return makePANByte(billday);
}
void getKNO(uint8_t * bptr)
{
	read_eeprom(bptr,KNO_LOC,10);
}
uint16_t makePANByte(uint8_t byteValue)
{
	uint8_t nibL=byteValue&0x0F;
	uint8_t nibU=byteValue>>4;
	uint16_t panbyte;
	
	if(nibU>9)
		panbyte=nibU+0x37;
	else
		panbyte=nibU+0x30;
	panbyte<<=8;
		
	if(nibL>9)
		panbyte|=nibL+0x37;
	else
		panbyte|=nibL+0x30;
		
	return panbyte;
}
void setComFrameBytes(uint16_t bytes)
{
	
	bytes=makePANByte(bytes);
	RxTxBuffer[PROTO_NO_BYTES_TAMPER]=bytes>>8;
	RxTxBuffer[PROTO_NO_BYTES_TAMPER+1]=bytes;
}

void setComFrameBytes_hex(uint16_t bytes)
{
	
//	bytes=makePANByte(bytes);
	RxTxBuffer[0]=bytes>>8;
	RxTxBuffer[0+1]=bytes;
}

void getCustomTODTiming(uint8_t* bptr)
{
	uint8_t todBuffR[26];
	uint8_t byteno=0;
	uint16_t panno;
	
	read_eeprom(todBuffR,ZONE_TIME,25);
	while(byteno<25)
	{
		panno=makePANByte(*(todBuffR+byteno));
		*bptr++=panno>>8;
		*bptr++=panno;
		byteno++;
	}
}
uint8_t setCustomTODTiming(const uint8_t* bptr)
{
	uint8_t todBuffR[30];
	uint8_t byteno=0;
	
	while(byteno<29)
	{
		todBuffR[byteno]=ConvertPAN(bptr);
		byteno++;
		bptr+=2;
	}
	
	if(Crc_Cal(todBuffR,27)==0)
	{
		write_eeprom(todBuffR,PASSIVE_ZONE_TIME,25);
		R_USER_Class20_SETACTION_Time(InsSave.timeCounter,0);
		return 1;
	}
	return 0;
}
uint8_t getCustomSWLog(uint8_t* bptr)
{
	uint8_t bytes=0,no_of_itr=SW_EVENTS;
	uint8_t buff[6];
	
	while(no_of_itr>0)
	{
		getSWLog(buff,no_of_itr--);
		bytes+=getCustomDateTimeASCIIFormat(getByteValue(buff,0,4),bptr+bytes,6);
	    bptr[bytes]=FIELD_SEPERATOR;
		bytes+=1;
		
	    Dec_Ascii(getByteValue(buff,4,1),3,bptr+bytes,0);
		bytes=bytes+3;
		if(no_of_itr>0)
		{
		    bptr[bytes]=FIELD_SEPERATOR;
			bytes+=1;
		}
	}
	
	return bytes;
	
}
#endif

void GetDailyLoadEntry(uint16_t index)
{
	uint32_t	addr;
	uint8_t lsBuffer[LS_DAILY_LENGTH];
	if(InsSave.DailyDayCount<DAILYLS_TOTAL_DAYS)
	{
	  if(InsSave.DailyDayCount<index)
	   {
		Xtime_count=0;
		XkWh_value=0;
		XkVAh_value=0;
		XMD_value=0;
		XMD_date_value=0;
	   }
	}
	addr=GetDailyLoc(index);		
	read_eeprom((uint8_t *)&lsBuffer, addr, LS_DAILY_LENGTH);	
	Xtime_count=getByteValue(lsBuffer, 0, 4);
	//R_USER_GetRTCTime(&buf->clock_value,Xtime_count);
	XkWh_value = getByteValue(lsBuffer, 4, 4);// kWh
	XkVAh_value = getByteValue(lsBuffer, 8, 4);// KVAh
	XMD_value= getByteValue(lsBuffer, 12, 2);// KVAh
	XMD_date_value= getByteValue(lsBuffer, 14, 4);// KVAh
}

/*void GetDailyLoadEntry(uint16_t index)
{
    uint8_t lsBuffer[LS_DAILY_LENGTH];
    uint32_t addr = GetDailyLoc(index);
    read_eeprom(lsBuffer, addr, LS_DAILY_LENGTH);
    // 3-byte timeCounter
    Xtime_count = lsBuffer[0] | (lsBuffer[1] << 8) | (lsBuffer[2] << 16);
    // kWh
    XkWh_value = lsBuffer[3] | (lsBuffer[4] << 8) | (lsBuffer[5] << 16) | (lsBuffer[6] << 24);
    // MD (kW)
    XMD_value = lsBuffer[7] | (lsBuffer[8] << 8);
    // MD timestamp — 3 bytes
    XMD_date_value = lsBuffer[9] | (lsBuffer[10] << 8) | (lsBuffer[11] << 16);
}*/
