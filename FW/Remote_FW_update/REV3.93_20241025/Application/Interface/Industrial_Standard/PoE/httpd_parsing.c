#define __HTTPD_PARSING_C__
    #include "httpd_parsing.h"
#undef  __HTTPD_PARSING_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "vmeasuring.h"
#include "rtc.h"
#include "eeprom.h"
#include "version.h"
#include "poe.h"
#include "relay.h"
#include "function_mode.h"
/* End Include */

/* Start Define */
#define HTTP_RAW_BUF_SIZE	4096
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
static char *StrMeasGas( U8 i );
static char *StrGasConc( U16 Dec, float Gas );
static void Httpd_Parsing_Head( char *pRaw );
static void Httpd_Parsing_Status( char *pRaw );
static void Httpd_Parsing_Parameter( char *pRaw );
static void Httpd_Parsing_Alarm( char *pRaw );
static void Httpd_Parsing_Relay( char *pRaw );
static void Httpd_Parsing_Test( char *pRaw );
static void Httpd_Parsing_Network( char *pRaw );
static void Httpd_Parsing_User( char *pRaw );
static void Httpd_Parsing_Factory( char *pRaw );
/* End Function */

/* Start Variable */
static char						RawBuf[HTTP_RAW_BUF_SIZE];
static Measuring_HandleType		*pMeas = &MeasHandle;

static char						*pUnit[] = { "PPB ", "PPM ", "%LEL", "%VOL" };
static char						*pActive[] = { "Normal", "Active" };
static char						*pIncrease[] = { "Decrease", "Increase" };
static char						*pEnable[] = { "Disable", "Enable " };

U8								iParameter = 0;
U8								iAlarm = 0;
U8								iTest = 0;
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char *Httpd_Parsing( const unsigned char *pName, const unsigned char *pData, int Len )
{
	memset( RawBuf, 0, HTTP_RAW_BUF_SIZE );
	memcpy( RawBuf, pData, Len );
	
	if		( strcmp( (const char *)pName, "/head.htm"		) == 0 ){ Httpd_Parsing_Head		( RawBuf ); }
	else if	( strcmp( (const char *)pName, "/status.htm"	) == 0 ){ Httpd_Parsing_Status		( RawBuf ); }
	else if	( strcmp( (const char *)pName, "/parameter.htm"	) == 0 ){ Httpd_Parsing_Parameter	( RawBuf ); }
	else if	( strcmp( (const char *)pName, "/alarm.htm"		) == 0 ){ Httpd_Parsing_Alarm		( RawBuf ); }
	else if	( strcmp( (const char *)pName, "/relay.htm"		) == 0 ){ Httpd_Parsing_Relay		( RawBuf ); }
	else if	( strcmp( (const char *)pName, "/test.htm"		) == 0 ){ Httpd_Parsing_Test		( RawBuf ); }
	else if	( strcmp( (const char *)pName, "/network.htm"	) == 0 ){ Httpd_Parsing_Network		( RawBuf ); }
	else if	( strcmp( (const char *)pName, "/user.htm"		) == 0 ){ Httpd_Parsing_User		( RawBuf ); }
	else if	( strcmp( (const char *)pName, "/factory.htm"	) == 0 ){ Httpd_Parsing_Factory		( RawBuf ); }
	
	return (const char *)RawBuf;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static char *StrMeasGas( U8 i )
{
	static char		Ret[11];
	
	if( (i == 1) && (pMeas->Conf.OutMax < 2) ){
		strcpy( Ret, "Disabled  " );
	}
	else {
		if( SystemError.Code[i] ){
			sprintf( Ret, "E-%02u      ", SystemError.Code[i] );
		}
		else {
			if( pMeas->pAlr[i]->Item.Bit.aOvr ){ sprintf( Ret, "OVER      " ); }
			else {
				switch( pMeas->pGas[i]->Inf.Dp ){
					case 0: sprintf( Ret, "%5.0f %s", pMeas->pGas[i]->Dsp * 1.000f, pUnit[pMeas->pGas[i]->Inf.Unit] ); break;
					case 1: sprintf( Ret, "%5.1f %s", pMeas->pGas[i]->Dsp * 0.100f, pUnit[pMeas->pGas[i]->Inf.Unit] ); break;
					case 2: sprintf( Ret, "%5.2f %s", pMeas->pGas[i]->Dsp * 0.010f, pUnit[pMeas->pGas[i]->Inf.Unit] ); break;
					case 3: sprintf( Ret, "%5.3f %s", pMeas->pGas[i]->Dsp * 0.001f, pUnit[pMeas->pGas[i]->Inf.Unit] ); break;
				}
			}
		}
	}
	
	return Ret;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static char *StrGasConc( U16 Dec, float Gas )
{                             
	static char	Ret[6];
	
	switch( Dec ){
		case 0: sprintf( Ret, "%-5.0f", Gas * 1.000f ); break;
		case 1: sprintf( Ret, "%-5.1f", Gas * 0.100f ); break;
		case 2: sprintf( Ret, "%-5.2f", Gas * 0.010f ); break;
		case 3: sprintf( Ret, "%-5.3f", Gas * 0.001f ); break;
	}
	
	return Ret;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Httpd_Parsing_Head( char *pRaw )
{
	char	*pTag;
	U8		Len;
	
	while( pRaw = strchr( pRaw, '$' ) ){
		
		Len = 22;
		
		if( pTag = strstr( pRaw, "$NAME" ) ){
			pRaw = pTag;
			sprintf( pRaw, "%s", (char *)EepHandle.Tag.Name );
			pRaw[21] = ' ';
		}
		else if( pTag = strstr( pRaw, "$LOCATION" ) ){
			pRaw = pTag;
			sprintf( pRaw, "%s", (char *)EepHandle.Tag.Location );
			pRaw[21] = ' ';
		}
		else if( pTag = strstr( pRaw, "$MESSAGE" ) ){
			pRaw = pTag;
			sprintf( pRaw, "%s", (char *)EepHandle.Tag.Message );
			pRaw[21] = ' ';
		}
		else if( pTag = strstr( pRaw, "$IP_ADDRESS" ) ){
			pRaw = pTag;
			sprintf( pRaw, "%3u.%3u.%3u.%3u      ",
				PoeHandle.Conf.IP[0],
				PoeHandle.Conf.IP[1],
				PoeHandle.Conf.IP[2],
				PoeHandle.Conf.IP[3]
			);
			pRaw[21] = ' ';
		}
		else {
			Len = 1;
		}
		
		pRaw += Len;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Httpd_Parsing_Status( char *pRaw )
{
	char	*pTag;
	U8		Len;
	
	while( pRaw = strchr( pRaw, '$' ) ){
		
		Len = 20;
		
		if( pTag = strstr( pRaw, "$SYS_MODE" ) ){
			pRaw = pTag;
			switch( SystemState ){
				case SYSTEM_STATE_INIT:			strcpy( pRaw, "Initialize         " ); break;
				case SYSTEM_STATE_CONFIG:		strcpy( pRaw, "Configuration      " ); break;
				case SYSTEM_STATE_SELF_TEST:	strcpy( pRaw, "Self-Test          " ); break;
				case SYSTEM_STATE_NORMAL:		strcpy( pRaw, "Normal             " ); break;
				case SYSTEM_STATE_ALLBASE:		strcpy( pRaw, "All Base           " ); break;
				case SYSTEM_STATE_PASSWORD:		strcpy( pRaw, "Password           " ); break;
				case SYSTEM_STATE_FUNCTION:
					if		( FuncHandle.State.Bit.Flow			){ strcpy( pRaw, "Flow Setting       " ); }
					else if	( FuncHandle.State.Bit.Test			){ strcpy( pRaw, "Test               " ); }
					else if	( FuncHandle.State.Bit.Calibration	){ strcpy( pRaw, "Calibration        " ); }
					else										 { strcpy( pRaw, "Function           " ); }
					break;
			}
		}
		else if( pTag = strstr( pRaw, "$SYS_CALENDAR" ) ){
			pRaw = pTag;
			RTC_Get_Calendar();
			sprintf( pRaw, "20%02u/%02u/%02u %02u:%02u:%02u",
				SysCalendar.Date.Year,
				SysCalendar.Date.Month,
				SysCalendar.Date.Date,
				SysCalendar.Time24.Hours,
				SysCalendar.Time24.Minutes,
				SysCalendar.Time24.Seconds
			);
		}
		else if( pTag = strstr( pRaw, "$SYS_FLOWRATE" ) ){
			pRaw = pTag;
			sprintf( pRaw, "%-5.0f mL/min       ", pMeas->pCur->FlowRate );
		}
		else if( pTag = strstr( pRaw, "$SERIAL_NUM" ) ){
			pRaw = pTag;
			sprintf( pRaw, "%s", (char *)EepHandle.Tag.SerialNumber );
			Len = 12;
		}
		else if( pTag = strstr( pRaw, "$CH1_GAS_NAME" ) ){
			pRaw = pTag;
			sprintf( pRaw, "%s         ", (char *)pMeas->pGas[0]->Inf.GasName );
		}
		else if( pTag = strstr( pRaw, "$CH1_CONCENTRATION"	) ){
			pRaw = pTag;
			sprintf( pRaw, "%s         ", StrMeasGas( 0 ) );
		}
		else if( pTag = strstr( pRaw, "$CH1_AL1_STATUS") ){
			pRaw = pTag;
			sprintf( pRaw, "%s             ", pActive[pMeas->pAlr[0]->Item.Bit.a1st] );
		}
		else if( pTag = strstr( pRaw, "$CH1_AL2_STATUS") ){
			pRaw = pTag;
			sprintf( pRaw, "%s             ", pActive[pMeas->pAlr[0]->Item.Bit.a2nd] );
		}
		else if( pTag = strstr( pRaw, "$CH1_TRB_STATUS") ){
			pRaw = pTag;
			if( SystemError.Code[0] ){ sprintf( pRaw, "%s             ", pActive[1] ); }
			else					 { sprintf( pRaw, "%s             ", pActive[0] ); }
		}
		else if( pTag = strstr( pRaw, "$CH2_GAS_NAME") ){
			pRaw = pTag;
			if( pMeas->Conf.OutMax > 1 ){
				sprintf( pRaw, "%s         ", (char *)pMeas->pGas[0]->Inf.GasName );
			}
			else {
				strcpy( pRaw, "Disabled           " );
			}
		}
		else if( pTag = strstr( pRaw, "$CH2_CONCENTRATION"	) ){
			pRaw = pTag;
			if( pMeas->Conf.OutMax > 1 ){
				sprintf( pRaw, "%s         ", StrMeasGas( 1 ) );
			}
			else {
				strcpy( pRaw, "Disabled           " );
			}
		}
		else if( pTag = strstr( pRaw, "$CH2_AL1_STATUS") ){
			pRaw = pTag;
			if( pMeas->Conf.OutMax > 1 ){
				sprintf( pRaw, "%s             ", pActive[pMeas->pAlr[1]->Item.Bit.a1st] );
			}
			else {
				strcpy( pRaw, "Disabled           " );
			}
		}
		else if( pTag = strstr( pRaw, "$CH2_AL2_STATUS") ){
			pRaw = pTag;
			if( pMeas->Conf.OutMax > 1 ){
				sprintf( pRaw, "%s             ", pActive[pMeas->pAlr[1]->Item.Bit.a2nd] );
			}
			else {
				strcpy( pRaw, "Disabled           " );
			}
		}
		else if( pTag = strstr( pRaw, "$CH2_TRB_STATUS") ){
			pRaw = pTag;
			if( pMeas->Conf.OutMax > 1 ){
				if( SystemError.Code[1] ){ sprintf( pRaw, "%s             ", pActive[1] ); }
				else					 { sprintf( pRaw, "%s             ", pActive[0] ); }
			}
			else {
				strcpy( pRaw, "Disabled           " );
			}
		}
		else if( pTag = strstr( pRaw, "$SYS_VER") ){
			pRaw = pTag;
			sprintf( pRaw, "H/W %3.1f | F/W %4.2f ", HardwareVersion, FirmwareVersion );
		}
		else if( pTag = strstr( pRaw, "$CTG_VER") ){
			pRaw = pTag;
			if( pMeas->pSen->pCtg->IsPresent == true ){
				sprintf( pRaw, "H/W %3.1f | F/W %4.2f ",
					pMeas->pSen->pCtg->Inf[CARTRIDGE_INF_HW_VER] * 0.1f,
					pMeas->pSen->pCtg->Inf[CARTRIDGE_INF_FW_VER] * 0.01f
				);
			}
			else {
				strcpy( pRaw, "Disabled           " );
			}
		}
		else if( pTag = strstr( pRaw, "$CVT_VER") ){
			pRaw = pTag;
			if( pMeas->pSen->pCvt->IsPresent == true ){
				sprintf( pRaw, "H/W %3.1f | F/W %4.2f ",
					pMeas->pSen->pCvt->Inf[CAVITY_INF_HW_VER] * 0.1f,
					pMeas->pSen->pCvt->Inf[CAVITY_INF_FW_VER] * 0.01f
				);
			}
			else {
				strcpy( pRaw, "Disabled           " );
			}
		}
		else {
			Len = 1;
		}
		
		pRaw += Len;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Httpd_Parsing_Parameter( char *pRaw )
{
	char	*pTag;
	U8		Len;
	U8		i = iParameter;
	
	while( pRaw = strchr( pRaw, '$' ) ){
		
		Len = 8;
		
		if( pTag = strstr( pRaw, "$CH1_GAS_NAME" ) ){
			pRaw = pTag;
			sprintf( pRaw, "CH1: %s    ", (char *)pMeas->pGas[0]->Inf.GasName );
			Len = 20;
		}
		else if( pTag = strstr( pRaw, "$CH2_GAS_NAME") ){
			pRaw = pTag;
			if( pMeas->Conf.OutMax > 1 ){
				sprintf( pRaw, "CH2: %s    ", (char *)pMeas->pGas[1]->Inf.GasName );
			}
			else {
				strcpy( pRaw, "CH2: Disabled      " );
			}
			Len = 20;
		}
		else if( pTag = strstr( pRaw, "$GAS_NAME"	) ){
			pRaw = pTag;
			sprintf( pRaw, "%s", (char *)pMeas->pGas[i]->Inf.GasName );
			Len = 11;
		}
		else if( pTag = strstr( pRaw, "$HIGH_SCALE") ){
			pRaw = pTag;
			sprintf( pRaw, "%s     ", StrGasConc( pMeas->pGas[i]->Inf.Dp, pMeas->pGas[i]->Inf.HighScale ) );
			pRaw[10] = ' ';
			Len = 11;
		}
		else if( pTag = strstr( pRaw, "$PPB") ){
			pRaw = pTag;
			if( pMeas->pGas[i]->Inf.Unit == MEASURING_UNIT_PPB ){ memcpy( pRaw, "checked ", 8 ); }
		}
		else if( pTag = strstr( pRaw, "$PPM") ){
			pRaw = pTag;
			if( pMeas->pGas[i]->Inf.Unit == MEASURING_UNIT_PPM ){ memcpy( pRaw, "checked ", 8 ); }
		}
		else if( pTag = strstr( pRaw, "$LEL") ){
			pRaw = pTag;
			if( pMeas->pGas[i]->Inf.Unit == MEASURING_UNIT_LEL ){ memcpy( pRaw, "checked ", 8 ); }
		}
		else if( pTag = strstr( pRaw, "$VOL") ){
			pRaw = pTag;
			if( pMeas->pGas[i]->Inf.Unit == MEASURING_UNIT_VOL ){ memcpy( pRaw, "checked ", 8 ); }
		}
		else {
			Len = 1;
		}
		
		pRaw += Len;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Httpd_Parsing_Alarm( char *pRaw )
{
	char	*pTag;
	U8		Len;
	U8		i = iAlarm;
	
	while( pRaw = strchr( pRaw, '$' ) ){
		
		Len = 8;
		
		if( pTag = strstr( pRaw, "$CH1_GAS_NAME" ) ){
			pRaw = pTag;
			sprintf( pRaw, "CH1: %s    ", (char *)pMeas->pGas[0]->Inf.GasName );
			Len = 20;
		}
		else if( pTag = strstr( pRaw, "$CH2_GAS_NAME") ){
			pRaw = pTag;
			if( pMeas->Conf.OutMax > 1 ){
				sprintf( pRaw, "CH2: %s    ", (char *)pMeas->pGas[1]->Inf.GasName );
			}
			else {
				strcpy( pRaw, "CH2: Disabled      " );
			}
			Len = 20;
		}
		else if( pTag = strstr( pRaw, "$GAS_NAME"	) ){
			pRaw = pTag;
			sprintf( pRaw, "%s", (char *)pMeas->pGas[i]->Inf.GasName );
			Len = 11;
		}
		else if( pTag = strstr( pRaw, "$HIGH_SCALE") ){
			pRaw = pTag;
			sprintf( pRaw, "%s %s",
				StrGasConc( pMeas->pGas[i]->Inf.Dp, pMeas->pGas[i]->Inf.HighScale ),
				pUnit[pMeas->pGas[i]->Inf.Unit]
			);
			Len = 11;
		}
		else if( pTag = strstr( pRaw, "$AL1_LEVEL") ){
			pRaw = pTag;
			sprintf( pRaw, "%s    ", StrGasConc( pMeas->pGas[i]->Inf.Dp, pMeas->pAlr[i]->Inf._1stLevel ) );
			pRaw[9] = ' ';
			Len = 10;
		}			  
		else if( pTag = strstr( pRaw, "$AL1_UNIT") ){
			pRaw = pTag;
			sprintf( pRaw, "%s    ", pUnit[pMeas->pGas[i]->Inf.Unit] );
			Len = 9;
		}
		else if( pTag = strstr( pRaw, "$AL1_DI") ){
			pRaw = pTag;
			if( pMeas->pAlr[i]->Inf._1stTypeIncrease ==  true ){ memcpy( pRaw, "checked ", 8 ); }
		}
		else if( pTag = strstr( pRaw, "$AL1_DD") ){
			pRaw = pTag;
			if( pMeas->pAlr[i]->Inf._1stTypeIncrease == false ){ memcpy( pRaw, "checked ", 8 ); }
		}
		else if( pTag = strstr( pRaw, "$AL1_DBAND") ){
			pRaw = pTag;
			sprintf( pRaw, "%-2u       ", pMeas->pAlr[i]->Inf._1stDead );
			pRaw[9] = ' ';
			Len = 10;
		}
		else if( pTag = strstr( pRaw, "$AL1_DELAY") ){
			pRaw = pTag;
			sprintf( pRaw, "%-2u       ", pMeas->pAlr[i]->Inf._1stDelaySec );
			pRaw[9] = ' ';
			Len = 10;
		}
		else if( pTag = strstr( pRaw, "$AL1_LE") ){
			pRaw = pTag;
			if( pMeas->pAlr[i]->Inf._1stLatchEnabled ==  true ){ memcpy( pRaw, "checked ", 8 ); }
		}
		else if( pTag = strstr( pRaw, "$AL1_LD") ){
			pRaw = pTag;
			if( pMeas->pAlr[i]->Inf._1stLatchEnabled == false ){ memcpy( pRaw, "checked ", 8 ); }
		}			  
		else if( pTag = strstr( pRaw, "$AL2_LEVEL") ){
			pRaw = pTag;
			sprintf( pRaw, "%s    ", StrGasConc( pMeas->pGas[i]->Inf.Dp, pMeas->pAlr[i]->Inf._2ndLevel ) );
			pRaw[9] = ' ';
			Len = 10;
		}			  
		else if( pTag = strstr( pRaw, "$AL2_UNIT") ){
			pRaw = pTag;
			sprintf( pRaw, "%s    ", pUnit[pMeas->pGas[i]->Inf.Unit] );
			Len = 9;
		}
		else if( pTag = strstr( pRaw, "$AL2_DI") ){
			pRaw = pTag;
			if( pMeas->pAlr[i]->Inf._2ndTypeIncrease ==  true ){ memcpy( pRaw, "checked ", 8 ); }
		}
		else if( pTag = strstr( pRaw, "$AL2_DD") ){
			pRaw = pTag;
			if( pMeas->pAlr[i]->Inf._2ndTypeIncrease == false ){ memcpy( pRaw, "checked ", 8 ); }
		}
		else if( pTag = strstr( pRaw, "$AL2_DBAND") ){
			pRaw = pTag;
			sprintf( pRaw, "%-2u       ", pMeas->pAlr[i]->Inf._2ndDead );
			pRaw[9] = ' ';
			Len = 10;
		}
		else if( pTag = strstr( pRaw, "$AL2_DELAY") ){
			pRaw = pTag;
			sprintf( pRaw, "%-2u       ", pMeas->pAlr[i]->Inf._2ndDelaySec );
			pRaw[9] = ' ';
			Len = 10;
		}
		else if( pTag = strstr( pRaw, "$AL2_LE") ){
			pRaw = pTag;
			if( pMeas->pAlr[i]->Inf._2ndLatchEnabled ==  true ){ memcpy( pRaw, "checked ", 8 ); }
		}
		else if( pTag = strstr( pRaw, "$AL2_LD") ){
			pRaw = pTag;
			if( pMeas->pAlr[i]->Inf._2ndLatchEnabled == false ){ memcpy( pRaw, "checked ", 8 ); }
		}
		else {
			Len = 1;
		}
		
		pRaw += Len;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Httpd_Parsing_Relay( char *pRaw )
{
	char	*pTag;
	U8		Len;
	
	while( pRaw = strchr( pRaw, '$' ) ){
		
		Len = 8;
		
		if( pTag = strstr( pRaw, "$TRB_OE") ){
			pRaw = pTag;
			if( RelayDrive.TRB.Opt.Item.IsOutputEnable ==  true ){ memcpy( pRaw, "checked ", 8 ); }
		}
		else if( pTag = strstr( pRaw, "$TRB_OD") ){
			pRaw = pTag;
			if( RelayDrive.TRB.Opt.Item.IsOutputEnable == false ){ memcpy( pRaw, "checked ", 8 ); }
		}
		else if( pTag = strstr( pRaw, "$TRB_EE") ){
			pRaw = pTag;
			if( RelayDrive.TRB.Opt.Item.IsEnergizeMode ==  true ){ memcpy( pRaw, "checked ", 8 ); }
		}
		else if( pTag = strstr( pRaw, "$TRB_ED") ){
			pRaw = pTag;
			if( RelayDrive.TRB.Opt.Item.IsEnergizeMode == false ){ memcpy( pRaw, "checked ", 8 ); }
		}
		else if( pTag = strstr( pRaw, "$TRB_BE") ){
			pRaw = pTag;
			if( RelayDrive.TRB.Opt.Item.IsBlinkingMode ==  true ){ memcpy( pRaw, "checked ", 8 ); }
		}
		else if( pTag = strstr( pRaw, "$TRB_BD") ){
			pRaw = pTag;
			if( RelayDrive.TRB.Opt.Item.IsBlinkingMode == false ){ memcpy( pRaw, "checked ", 8 ); }
		}
		else if( pTag = strstr( pRaw, "$TRB_ACT") ){
			pRaw = pTag;
			sprintf( pRaw, "%-2u     ", RelayDrive.TRB.Blk.Item.Act_Time );
			pRaw[7] = ' ';
		}
		else if( pTag = strstr( pRaw, "$TRB_REL") ){
			pRaw = pTag;
			sprintf( pRaw, "%-2u     ", RelayDrive.TRB.Blk.Item.Rel_Time );
			pRaw[7] = ' ';
		}
		else if( pTag = strstr( pRaw, "$AL1_OE") ){
			pRaw = pTag;
			if( RelayDrive.AL1.Opt.Item.IsOutputEnable ==  true ){ memcpy( pRaw, "checked ", 8 ); }
		}
		else if( pTag = strstr( pRaw, "$AL1_OD") ){
			pRaw = pTag;
			if( RelayDrive.AL1.Opt.Item.IsOutputEnable == false ){ memcpy( pRaw, "checked ", 8 ); }
		}
		else if( pTag = strstr( pRaw, "$AL1_EE") ){
			pRaw = pTag;
			if( RelayDrive.AL1.Opt.Item.IsEnergizeMode ==  true ){ memcpy( pRaw, "checked ", 8 ); }
		}
		else if( pTag = strstr( pRaw, "$AL1_ED") ){
			pRaw = pTag;
			if( RelayDrive.AL1.Opt.Item.IsEnergizeMode == false ){ memcpy( pRaw, "checked ", 8 ); }
		}
		else if( pTag = strstr( pRaw, "$AL1_BE") ){
			pRaw = pTag;
			if( RelayDrive.AL1.Opt.Item.IsBlinkingMode ==  true ){ memcpy( pRaw, "checked ", 8 ); }
		}
		else if( pTag = strstr( pRaw, "$AL1_BD") ){
			pRaw = pTag;
			if( RelayDrive.AL1.Opt.Item.IsBlinkingMode == false ){ memcpy( pRaw, "checked ", 8 ); }
		}
		else if( pTag = strstr( pRaw, "$AL1_ACT") ){
			pRaw = pTag;
			sprintf( pRaw, "%-2u     ", RelayDrive.AL1.Blk.Item.Act_Time );
			pRaw[7] = ' ';
		}
		else if( pTag = strstr( pRaw, "$AL1_REL") ){
			pRaw = pTag;
			sprintf( pRaw, "%-2u     ", RelayDrive.AL1.Blk.Item.Rel_Time );
			pRaw[7] = ' ';
		}
		else if( pTag = strstr( pRaw, "$AL2_OE") ){
			pRaw = pTag;
			if( RelayDrive.AL2.Opt.Item.IsOutputEnable ==  true ){ memcpy( pRaw, "checked ", 8 ); }
		}
		else if( pTag = strstr( pRaw, "$AL2_OD") ){
			pRaw = pTag;
			if( RelayDrive.AL2.Opt.Item.IsOutputEnable == false ){ memcpy( pRaw, "checked ", 8 ); }
		}
		else if( pTag = strstr( pRaw, "$AL2_EE") ){
			pRaw = pTag;
			if( RelayDrive.AL2.Opt.Item.IsEnergizeMode ==  true ){ memcpy( pRaw, "checked ", 8 ); }
		}
		else if( pTag = strstr( pRaw, "$AL2_ED") ){
			pRaw = pTag;
			if( RelayDrive.AL2.Opt.Item.IsEnergizeMode == false ){ memcpy( pRaw, "checked ", 8 ); }
		}
		else if( pTag = strstr( pRaw, "$AL2_BE") ){
			pRaw = pTag;
			if( RelayDrive.AL2.Opt.Item.IsBlinkingMode ==  true ){ memcpy( pRaw, "checked ", 8 ); }
		}
		else if( pTag = strstr( pRaw, "$AL2_BD") ){
			pRaw = pTag;
			if( RelayDrive.AL2.Opt.Item.IsBlinkingMode == false ){ memcpy( pRaw, "checked ", 8 ); }
		}
		else if( pTag = strstr( pRaw, "$AL2_ACT") ){
			pRaw = pTag;
			sprintf( pRaw, "%-2u     ", RelayDrive.AL2.Blk.Item.Act_Time );
			pRaw[7] = ' ';
		}
		else if( pTag = strstr( pRaw, "$AL2_REL") ){
			pRaw = pTag;
			sprintf( pRaw, "%-2u     ", RelayDrive.AL2.Blk.Item.Rel_Time );
			pRaw[7] = ' ';
		}
		else {
			Len = 1;
		}
		
		pRaw += Len;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Httpd_Parsing_Test( char *pRaw )
{
	char	*pTag;
	U8		Len;
	U8		i = iTest;
	bool	IsTestMode[2] = { pMeas->Test.Item.WebTest_CH1, pMeas->Test.Item.WebTest_CH2 };
	
	while( pRaw = strchr( pRaw, '$' ) ){
		
		Len = 8;
		
		if( pTag = strstr( pRaw, "$CH1_GAS_NAME" ) ){
			pRaw = pTag;
			sprintf( pRaw, "CH1: %s    ", (char *)pMeas->pGas[0]->Inf.GasName );
			Len = 20;
		}
		else if( pTag = strstr( pRaw, "$CH2_GAS_NAME") ){
			pRaw = pTag;
			if( pMeas->Conf.OutMax > 1 ){
				sprintf( pRaw, "CH2: %s    ", (char *)pMeas->pGas[1]->Inf.GasName );
			}
			else {
				strcpy( pRaw, "CH2: Disabled      " );
			}
			Len = 20;
		}
		else if( pTag = strstr( pRaw, "$GAS_NAME"	) ){
			pRaw = pTag;
			sprintf( pRaw, "%s", (char *)pMeas->pGas[i]->Inf.GasName );
			Len = 11;
		}
		else if( pTag = strstr( pRaw, "$HIGH_SCALE") ){
			pRaw = pTag;
			sprintf( pRaw, "%s %s",
				StrGasConc( pMeas->pGas[i]->Inf.Dp, pMeas->pGas[i]->Inf.HighScale ),
				pUnit[pMeas->pGas[i]->Inf.Unit]
			);
			Len = 11;
		}
		else if( pTag = strstr( pRaw, "$AL1_CONFIG") ){
			pRaw = pTag;
			sprintf( pRaw, "%s %s, %s, %2u%% Dead, %2us Delay, Latch %s",
				StrGasConc( pMeas->pGas[i]->Inf.Dp, pMeas->pAlr[i]->Inf._1stLevel ),
				pUnit[pMeas->pGas[i]->Inf.Unit],
				pIncrease[pMeas->pAlr[i]->Inf._1stTypeIncrease],
				pMeas->pAlr[i]->Inf._1stDead,
				pMeas->pAlr[i]->Inf._1stDelaySec,
				pEnable[pMeas->pAlr[i]->Inf._1stLatchEnabled]
			);
			pRaw[56] = ' ';
			Len = 57;
		}
		else if( pTag = strstr( pRaw, "$AL2_CONFIG") ){
			pRaw = pTag;
			sprintf( pRaw, "%s %s, %s, %2u%% Dead, %2us Delay, Latch %s",
				StrGasConc( pMeas->pGas[i]->Inf.Dp, pMeas->pAlr[i]->Inf._2ndLevel ),
				pUnit[pMeas->pGas[i]->Inf.Unit],
				pIncrease[pMeas->pAlr[i]->Inf._2ndTypeIncrease],
				pMeas->pAlr[i]->Inf._2ndDead,
				pMeas->pAlr[i]->Inf._2ndDelaySec,
				pEnable[pMeas->pAlr[i]->Inf._2ndLatchEnabled]
			);
			pRaw[56] = ' ';
			Len = 57;
		}
		else if( pTag = strstr( pRaw, "$NORMAL") ){
			pRaw = pTag;
			if( IsTestMode[i] == false ){ memcpy( pRaw, "checked ", 8 ); }
		}
		else if( pTag = strstr( pRaw, "$TEST") ){
			pRaw = pTag;
			if( IsTestMode[i] ==  true ){ memcpy( pRaw, "checked ", 8 ); }
		}
		else if( pTag = strstr( pRaw, "$RLY_OE") ){
			pRaw = pTag;
			if( RelayDrive.UseWebTest ==  true ){ memcpy( pRaw, "checked ", 8 ); }
		}
		else if( pTag = strstr( pRaw, "$RLY_OD") ){
			pRaw = pTag;
			if( RelayDrive.UseWebTest == false ){ memcpy( pRaw, "checked ", 8 ); }
		}
		else if( pTag = strstr( pRaw, "$TGAS") ){
			pRaw = pTag;
			sprintf( pRaw, "%s", StrGasConc( pMeas->pGas[i]->Inf.Dp, pMeas->pGas[i]->Set ) );
			pRaw[5] = ' ';
			Len = 6;
		}
		else if( pTag = strstr( pRaw, "$UNIT") ){
			pRaw = pTag;
			sprintf( pRaw, "%s", pUnit[pMeas->pGas[i]->Inf.Unit] );
			Len = 5;
		}
		else {
			Len = 1;
		}
		
		pRaw += Len;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Httpd_Parsing_Network( char *pRaw )
{
	char	*pTag;
	U8		Len;
	
	while( pRaw = strchr( pRaw, '$' ) ){
		
		if( pTag = strstr( pRaw, "$IP1" ) ){
			pRaw = pTag;
			sprintf( pRaw, "%-3u", PoeHandle.Conf.IP[0] );
			pRaw[3] = ' ';
			Len = 4;
		}
		else if( pTag = strstr( pRaw, "$IP2" ) ){
			pRaw = pTag;
			sprintf( pRaw, "%-3u", PoeHandle.Conf.IP[1] );
			pRaw[3] = ' ';
			Len = 4;
		}
		else if( pTag = strstr( pRaw, "$IP3" ) ){
			pRaw = pTag;
			sprintf( pRaw, "%-3u", PoeHandle.Conf.IP[2] );
			pRaw[3] = ' ';
			Len = 4;
		}
		else if( pTag = strstr( pRaw, "$IP4" ) ){
			pRaw = pTag;
			sprintf( pRaw, "%-3u", PoeHandle.Conf.IP[3] );
			pRaw[3] = ' ';
			Len = 4;
		}
		else if( pTag = strstr( pRaw, "$NM1" ) ){
			pRaw = pTag;
			sprintf( pRaw, "%-3u", PoeHandle.Conf.NM[0] );
			pRaw[3] = ' ';
			Len = 4;
		}
		else if( pTag = strstr( pRaw, "$NM2" ) ){
			pRaw = pTag;
			sprintf( pRaw, "%-3u", PoeHandle.Conf.NM[1] );
			pRaw[3] = ' ';
			Len = 4;
		}
		else if( pTag = strstr( pRaw, "$NM3" ) ){
			pRaw = pTag;
			sprintf( pRaw, "%-3u", PoeHandle.Conf.NM[2] );
			pRaw[3] = ' ';
			Len = 4;
		}
		else if( pTag = strstr( pRaw, "$NM4" ) ){
			pRaw = pTag;
			sprintf( pRaw, "%-3u", PoeHandle.Conf.NM[3] );
			pRaw[3] = ' ';
			Len = 4;
		}
		else if( pTag = strstr( pRaw, "$GW1" ) ){
			pRaw = pTag;
			sprintf( pRaw, "%-3u", PoeHandle.Conf.GW[0] );
			pRaw[3] = ' ';
			Len = 4;
		}
		else if( pTag = strstr( pRaw, "$GW2" ) ){
			pRaw = pTag;
			sprintf( pRaw, "%-3u", PoeHandle.Conf.GW[1] );
			pRaw[3] = ' ';
			Len = 4;
		}
		else if( pTag = strstr( pRaw, "$GW3" ) ){
			pRaw = pTag;
			sprintf( pRaw, "%-3u", PoeHandle.Conf.GW[2] );
			pRaw[3] = ' ';
			Len = 4;
		}
		else if( pTag = strstr( pRaw, "$GW4" ) ){
			pRaw = pTag;
			sprintf( pRaw, "%-3u", PoeHandle.Conf.GW[3] );
			pRaw[3] = ' ';
			Len = 4;
		}
		else {
			Len = 1;
		}
		
		pRaw += Len;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Httpd_Parsing_User( char *pRaw )
{
	char	*pTag;
	U8		Len;
	
	while( pRaw = strchr( pRaw, '$' ) ){
		
		Len = 22;
		
		if( pTag = strstr( pRaw, "$NAME" ) ){
			pRaw = pTag;
			sprintf( pRaw, "%s", (char *)EepHandle.Tag.Name );
			pRaw[21] = ' ';
		}
		else if( pTag = strstr( pRaw, "$LOCATION" ) ){
			pRaw = pTag;
			sprintf( pRaw, "%s", (char *)EepHandle.Tag.Location );
			pRaw[21] = ' ';
		}
		else if( pTag = strstr( pRaw, "$MESSAGE" ) ){
			pRaw = pTag;
			sprintf( pRaw, "%s", (char *)EepHandle.Tag.Message );
			pRaw[21] = ' ';
		}
		else {
			Len = 1;
		}
		
		pRaw += Len;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Httpd_Parsing_Factory( char *pRaw )
{
	char	*pTag;
	U8		Len;
	
	while( pRaw = strchr( pRaw, '$' ) ){
		
		if( pTag = strstr( pRaw, "$SERIAL_NUM" ) ){
			pRaw = pTag;
			sprintf( pRaw, "%s", (char *)EepHandle.Tag.SerialNumber );
			pRaw[11] = ' ';
			Len = 12;
		}
		else {
			Len = 1;
		}
		
		pRaw += Len;
	}
}
