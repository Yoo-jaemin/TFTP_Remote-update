#define __RTC_C__
    #include "rtc.h"
#undef  __RTC_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "btimer.h"
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
enum {
	RTC_BACKUP_REG_PREFIX = RTC_BKP_DR0,
	RTC_BACKUP_REG_YEAR,
	RTC_BACKUP_REG_MONTH,
	RTC_BACKUP_REG_DATE,
	RTC_BACKUP_REG_WEEKDAY,
	RTC_BACKUP_REG_HOURS,
	RTC_BACKUP_REG_MINUTES,
	RTC_BACKUP_REG_SECONDS,
	RTC_BACKUP_REG_MAX
};
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
static bool RTC_CalendarConfig( void );
/* End Function */

/* Start Variable */
const uint32_t RTC_Backup_Register_Default[RTC_BACKUP_REG_MAX] =
{
	0x5100F,				// RTC_BACKUP_REG_PREFIX 		
	19,						// RTC_BACKUP_REG_YEAR	 
	1,						// RTC_BACKUP_REG_MONTH	 
	25,						// RTC_BACKUP_REG_DATE	 
	RTC_WEEKDAY_FRIDAY,		// RTC_BACKUP_REG_WEEKDAY
	11,						// RTC_BACKUP_REG_HOURS	 
	59,						// RTC_BACKUP_REG_MINUTES	
	00,						// RTC_BACKUP_REG_SECONDS	
};
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool RTC_Init( void )
{
	bool	err = false;
	
	RCC_OscInitTypeDef				RCC_OscInitStruct;
	RCC_PeriphCLKInitTypeDef		PeriphClkInitStruct;
		
	RtcHandle.Instance				= RTC;
	RtcHandle.Init.HourFormat		= RTC_HOURFORMAT_24;
	RtcHandle.Init.AsynchPrediv		= 127;
	RtcHandle.Init.SynchPrediv		= 255;
	RtcHandle.Init.OutPut			= RTC_OUTPUT_DISABLE;
	RtcHandle.Init.OutPutPolarity	= RTC_OUTPUT_POLARITY_HIGH;
	RtcHandle.Init.OutPutType		= RTC_OUTPUT_TYPE_OPENDRAIN;
	
	__HAL_RTC_RESET_HANDLE_STATE( &RtcHandle );
	
	 if( RtcHandle.State == HAL_RTC_STATE_RESET ){
		 
		/* Allocate lock resource and initialize it */
		RtcHandle.Lock = HAL_UNLOCKED;
		
		/* Initialize RTC MSP */
		RCC_OscInitStruct.OscillatorType			= RCC_OSCILLATORTYPE_LSE;
		RCC_OscInitStruct.PLL.PLLState				= RCC_PLL_NONE;
		RCC_OscInitStruct.LSEState					= RCC_LSE_ON;
		
		if( HAL_RCC_OscConfig( &RCC_OscInitStruct ) != HAL_OK ){ err = true; }
		
		PeriphClkInitStruct.PeriphClockSelection	= RCC_PERIPHCLK_RTC;
		PeriphClkInitStruct.RTCClockSelection		= RCC_RTCCLKSOURCE_LSE;
		
		if( HAL_RCCEx_PeriphCLKConfig( &PeriphClkInitStruct ) != HAL_OK ){ err = true; }
		
		__HAL_RCC_RTC_ENABLE();
	}
	
	if( HAL_RTC_Init( &RtcHandle ) != HAL_OK ){ err = true; }
	
	err |= RTC_CalendarConfig();
	
	SysCalendar.SecondEvt = true;
	SysCalendar.MinuteEvt = true;
	
	return err;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RTC_Get_Calendar( void )
{
	if( HAL_RTC_GetTime( &RtcHandle, &SysCalendar.Time24, RTC_FORMAT_BIN ) == HAL_OK ){                  
		if( HAL_RTC_GetDate( &RtcHandle, &SysCalendar.Date, RTC_FORMAT_BIN ) == HAL_OK ){
			
			if( SysCalendar.Time12.Seconds != SysCalendar.Time24.Seconds ){ SysCalendar.SecondEvt = true; }
			if( SysCalendar.Time12.Minutes != SysCalendar.Time24.Minutes ){ SysCalendar.MinuteEvt = true; }
			
			if( SysCalendar.Time24.Hours > 12 ){
				SysCalendar.Time12.Hours = SysCalendar.Time24.Hours - 12;
			}
			else {
				SysCalendar.Time12.Hours = SysCalendar.Time24.Hours;
			}
			
			SysCalendar.Time12.Minutes		= SysCalendar.Time24.Minutes;
			SysCalendar.Time12.Seconds		= SysCalendar.Time24.Seconds;
			SysCalendar.Time12.SubSeconds	= (uint32_t)(SysCalendar.Time24.SubSeconds * 0.388235294f);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool RTC_Set_Calendar( RTC_DateTypeDef *pDate, RTC_TimeTypeDef *pTime )
{
	bool				err = false;
	
	SysCalendar.Date				= *pDate;
	SysCalendar.Time24				= *pTime;
	SysCalendar.Time24.SubSeconds	= 0;
	                                                                                                  
	if( HAL_RTC_SetTime( &RtcHandle, &SysCalendar.Time24, RTC_FORMAT_BIN ) != HAL_OK ){ err = true; }
	if( HAL_RTC_SetDate( &RtcHandle, &SysCalendar.Date  , RTC_FORMAT_BIN ) != HAL_OK ){ err = true; }
	
	return err;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static bool RTC_CalendarConfig( void )
{
	RTC_DateTypeDef		sDate;
	RTC_TimeTypeDef		sTime;
	U8					i;
	bool				err = false;
	
	if( HAL_RTCEx_BKUPRead( &RtcHandle, RTC_BACKUP_REG_PREFIX ) != RTC_Backup_Register_Default[RTC_BACKUP_REG_PREFIX] ){
		
		for( i=0; i<RTC_BACKUP_REG_MAX; i++ ){
			HAL_RTCEx_BKUPWrite( &RtcHandle, i, RTC_Backup_Register_Default[i] );
		}
		
		sDate.Year				= (uint8_t)RTC_Backup_Register_Default[RTC_BACKUP_REG_YEAR];
		sDate.Month				= (uint8_t)RTC_Backup_Register_Default[RTC_BACKUP_REG_MONTH];
		sDate.Date				= (uint8_t)RTC_Backup_Register_Default[RTC_BACKUP_REG_DATE];
		sDate.WeekDay			= (uint8_t)RTC_Backup_Register_Default[RTC_BACKUP_REG_WEEKDAY];
		
		if( HAL_RTC_SetDate( &RtcHandle, &sDate, RTC_FORMAT_BIN ) != HAL_OK ){ err = true; }
		
		sTime.Hours				= (uint8_t)RTC_Backup_Register_Default[RTC_BACKUP_REG_HOURS];
		sTime.Minutes			= (uint8_t)RTC_Backup_Register_Default[RTC_BACKUP_REG_MINUTES];
		sTime.Seconds			= (uint8_t)RTC_Backup_Register_Default[RTC_BACKUP_REG_SECONDS];
		sTime.TimeFormat		= (uint8_t)RTC_HOURFORMAT12_AM;
		sTime.DayLightSaving	= RTC_DAYLIGHTSAVING_NONE;
		sTime.StoreOperation	= RTC_STOREOPERATION_RESET;
		
		if( HAL_RTC_SetTime( &RtcHandle, &sTime, RTC_FORMAT_BIN ) != HAL_OK ){ err = true; }
	}
	else {
		/* Check if the Power On Reset flag is set */  
		if( __HAL_RCC_GET_FLAG( RCC_FLAG_PORRST ) != RESET ){
		}
		/* Check if Pin Reset flag is set */
		if( __HAL_RCC_GET_FLAG( RCC_FLAG_PINRST ) != RESET ){
		}
		
		/* Clear source Reset Flag */
		__HAL_RCC_CLEAR_RESET_FLAGS();
	}
	
	return err;
}

