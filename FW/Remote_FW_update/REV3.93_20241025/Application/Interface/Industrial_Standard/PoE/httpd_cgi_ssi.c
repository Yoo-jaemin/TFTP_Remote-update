#define __HTTPD_CGI_SSI_C__
    #include "httpd_cgi_ssi.h"
#undef  __HTTPD_CGI_SSI_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "main.h"
#include "lwip/debug.h"
#include "lwip/tcp.h"
#include "lwip/apps/httpd.h"
#include "eeprom.h"
#include "measuring.h"
#include "httpd_parsing.h"
#include "relay.h"
#include "poe.h"

#include <string.h>
#include <stdlib.h>
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
enum {
	CGI_INDEX_PARAMETER_CH,
	CGI_INDEX_PARAMETER,
	CGI_INDEX_ALARM_CH,
	CGI_INDEX_ALARM,
	CGI_INDEX_RELAY,
	CGI_INDEX_TEST_CH,
	CGI_INDEX_TEST,
	CGI_INDEX_NETWORK,
	CGI_INDEX_USER,
	CGI_INDEX_FACTORY,
	CGI_INDEX_MAX
};
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
static void Replace_Char( char *pStr, char Old, char New );
static void URL_Decoding( char *pStr );
static const char *CGI_Handler_Parameter_Ch( int iIndex, int iNumParams, char *pcParam[], char *pcValue[] );
static const char *CGI_Handler_Parameter( int iIndex, int iNumParams, char *pcParam[], char *pcValue[] );
static const char *CGI_Handler_Alarm_Ch( int iIndex, int iNumParams, char *pcParam[], char *pcValue[] );
static const char *CGI_Handler_Alarm( int iIndex, int iNumParams, char *pcParam[], char *pcValue[] );
static const char *CGI_Handler_Relay( int iIndex, int iNumParams, char *pcParam[], char *pcValue[] );
static const char *CGI_Handler_Test_Ch( int iIndex, int iNumParams, char *pcParam[], char *pcValue[] );
static const char *CGI_Handler_Test( int iIndex, int iNumParams, char *pcParam[], char *pcValue[] );
static const char *CGI_Handler_Network( int iIndex, int iNumParams, char *pcParam[], char *pcValue[] );
static const char *CGI_Handler_User( int iIndex, int iNumParams, char *pcParam[], char *pcValue[] );
static const char *CGI_Handler_Factory( int iIndex, int iNumParams, char *pcParam[], char *pcValue[] );
/* End Function */

/* Start Variable */
char const*						TAGCHAR = "t";
char const**					TAGS = &TAGCHAR;

static const tCGI				CGI_Parameter_Ch	={ "/parameter_ch.cgi"	, CGI_Handler_Parameter_Ch	};
static const tCGI				CGI_Parameter		={ "/parameter.cgi"		, CGI_Handler_Parameter		};
static const tCGI				CGI_Alarm_Ch		={ "/alarm_ch.cgi"		, CGI_Handler_Alarm_Ch		};
static const tCGI				CGI_Alarm			={ "/alarm.cgi"			, CGI_Handler_Alarm			};
static const tCGI				CGI_Relay			={ "/relay.cgi"			, CGI_Handler_Relay			};
static const tCGI				CGI_Test_Ch			={ "/test_ch.cgi"		, CGI_Handler_Test_Ch		};
static const tCGI				CGI_Test			={ "/test.cgi"			, CGI_Handler_Test			};
static const tCGI				CGI_Network			={ "/network.cgi"		, CGI_Handler_Network		};
static const tCGI				CGI_User			={ "/user.cgi"			, CGI_Handler_User			};
static const tCGI				CGI_Factory			={ "/factory.cgi"		, CGI_Handler_Factory		};

static tCGI						CGI_TAB[CGI_INDEX_MAX];

static Measuring_HandleType		*pMeas = &MeasHandle;
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void http_server_init( void )
{
	/* Httpd Init */
	httpd_init();
	
	/* configure CGI handlers */
	CGI_TAB[CGI_INDEX_PARAMETER_CH]	= CGI_Parameter_Ch;
	CGI_TAB[CGI_INDEX_PARAMETER]	= CGI_Parameter;
	CGI_TAB[CGI_INDEX_ALARM_CH]		= CGI_Alarm_Ch;
	CGI_TAB[CGI_INDEX_ALARM]		= CGI_Alarm;
	CGI_TAB[CGI_INDEX_RELAY]		= CGI_Relay;
	CGI_TAB[CGI_INDEX_TEST_CH]		= CGI_Test_Ch;
	CGI_TAB[CGI_INDEX_TEST]			= CGI_Test;
	CGI_TAB[CGI_INDEX_NETWORK]		= CGI_Network;
	CGI_TAB[CGI_INDEX_USER]			= CGI_User;
	CGI_TAB[CGI_INDEX_FACTORY]		= CGI_Factory;
	
	http_set_cgi_handlers( CGI_TAB, CGI_INDEX_MAX );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Replace_Char( char *pStr, char Old, char New )
{
	while( *pStr ){
		if( *pStr == Old ){ *pStr = New; }
		pStr++;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void URL_Decoding( char *pStr )
{
	char	*pBuf = pStr;
	char	Dest[32];
	U8		i = 0;
	
	for( i=0; *pBuf; i++ ){
		if( *pBuf == '%' ){
			if		( (pBuf[1] == '2') && (pBuf[2] == '1') ){ Dest[i] = 0x21; }		/*   !   */
			else if	( (pBuf[1] == '2') && (pBuf[2] == '3') ){ Dest[i] = 0x23; }		/*   #   */
			else if	( (pBuf[1] == '2') && (pBuf[2] == '4') ){ Dest[i] = 0x24; }		/*   $   */
			else if	( (pBuf[1] == '2') && (pBuf[2] == '5') ){ Dest[i] = 0x25; }		/*   %   */
			else if	( (pBuf[1] == '2') && (pBuf[2] == '6') ){ Dest[i] = 0x26; }		/*   &   */
			else if	( (pBuf[1] == '2') && (pBuf[2] == '7') ){ Dest[i] = 0x27; }		/*   '   */
			else if	( (pBuf[1] == '2') && (pBuf[2] == '8') ){ Dest[i] = 0x28; }		/*   (   */
			else if	( (pBuf[1] == '2') && (pBuf[2] == '9') ){ Dest[i] = 0x29; }		/*   )   */
			else if	( (pBuf[1] == '2') && (pBuf[2] == 'A') ){ Dest[i] = 0x2A; }		/*   *   */
			else if	( (pBuf[1] == '2') && (pBuf[2] == 'B') ){ Dest[i] = 0x2B; }		/*   +   */
			else if	( (pBuf[1] == '2') && (pBuf[2] == 'C') ){ Dest[i] = 0x2C; }		/*   ,   */
			else if	( (pBuf[1] == '2') && (pBuf[2] == 'D') ){ Dest[i] = 0x2D; }		/*   -   */
			else if	( (pBuf[1] == '2') && (pBuf[2] == 'E') ){ Dest[i] = 0x2E; }		/*   .   */
			else if	( (pBuf[1] == '2') && (pBuf[2] == 'F') ){ Dest[i] = 0x2F; }		/*   /   */
			else if	( (pBuf[1] == '3') && (pBuf[2] == 'A') ){ Dest[i] = 0x3A; }		/*   :   */
			else if	( (pBuf[1] == '3') && (pBuf[2] == 'B') ){ Dest[i] = 0x3B; }		/*   ;   */
			else if	( (pBuf[1] == '3') && (pBuf[2] == 'C') ){ Dest[i] = 0x3C; }		/*   <   */
			else if	( (pBuf[1] == '3') && (pBuf[2] == 'D') ){ Dest[i] = 0x3D; }		/*   =   */
			else if	( (pBuf[1] == '3') && (pBuf[2] == 'E') ){ Dest[i] = 0x3E; }		/*   >   */
			else if	( (pBuf[1] == '3') && (pBuf[2] == 'F') ){ Dest[i] = 0x3F; }		/*   ?   */
			else if	( (pBuf[1] == '4') && (pBuf[2] == '0') ){ Dest[i] = 0x40; }		/*   @   */
			else if	( (pBuf[1] == '5') && (pBuf[2] == 'B') ){ Dest[i] = 0x5B; }		/*   [   */
			else if	( (pBuf[1] == '5') && (pBuf[2] == 'C') ){ Dest[i] = 0x5C; }		/*   \   */
			else if	( (pBuf[1] == '5') && (pBuf[2] == 'D') ){ Dest[i] = 0x5D; }		/*   ]   */
			else if	( (pBuf[1] == '5') && (pBuf[2] == 'E') ){ Dest[i] = 0x5E; }		/*   ^   */
			else if	( (pBuf[1] == '5') && (pBuf[2] == 'F') ){ Dest[i] = 0x5F; }		/*   _   */
			else if	( (pBuf[1] == '6') && (pBuf[2] == '0') ){ Dest[i] = 0x60; }		/*   `   */
			else if	( (pBuf[1] == '7') && (pBuf[2] == 'B') ){ Dest[i] = 0x7B; }		/*   {   */
			else if	( (pBuf[1] == '7') && (pBuf[2] == 'C') ){ Dest[i] = 0x7C; }		/*   |   */
			else if	( (pBuf[1] == '7') && (pBuf[2] == 'D') ){ Dest[i] = 0x7D; }		/*   }   */
			else if	( (pBuf[1] == '7') && (pBuf[2] == 'E') ){ Dest[i] = 0x7E; }		/*   ~   */
			else											{ Dest[i] = 0x20; }   
			pBuf += 3;
		}
		else {
			Dest[i] = *pBuf;
			pBuf += 1;
		}
	}
	
	Dest[i] = 0;
	
	strcpy( pStr, Dest );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char *CGI_Handler_Parameter_Ch( int iIndex, int iNumParams, char *pcParam[], char *pcValue[] )
{
	uint32_t	i = 0;
	
	if( iIndex == CGI_INDEX_PARAMETER_CH ){
		
		if( strcmp( pcParam[i], "OUTPUT_SELECT" ) == 0 ){
			if( strcmp( pcValue[i], "OUT1" ) == 0 ){
				iParameter = 0;
			}
			else if( strcmp( pcValue[i], "OUT2" ) == 0 ){
				if( pMeas->Conf.OutMax > 1 ){ iParameter = 1; }
			}
		}
	}
	
	return "/parameter.htm";  
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char *CGI_Handler_Parameter( int iIndex, int iNumParams, char *pcParam[], char *pcValue[] )
{
	uint32_t	i = 0;
	char		*pDp;
	U16			Dp;
	U16			HighScale;
	U16			Unit;
	
	U16			eep[4];
	float		roc;
	
	if( iIndex == CGI_INDEX_PARAMETER ){
		
		for( i=0; i<iNumParams; i++ ){
			
			if( strcmp( pcParam[i], "HIGH_SCALE" ) == 0 ){
				Replace_Char( pcValue[i], '+', ' ' );
				URL_Decoding( pcValue[i] );
				pDp = strchr( pcValue[i], '.' );
				if( pDp ){
					Dp = strlen( pDp + 1 );
				}
				else{
					Dp = 0;
				}
				if( Dp > 3 ){
					Dp = 3;
				}
				HighScale = (U16)(atof( pcValue[i] ) * pow( 10, Dp ));
			}
			else if( strcmp( pcParam[i], "UNIT" ) == 0 ){
				if		( strcmp( pcValue[i], "PPB" ) == 0 ){ Unit = MEASURING_UNIT_PPB; }
				else if	( strcmp( pcValue[i], "PPM" ) == 0 ){ Unit = MEASURING_UNIT_PPM; }
				else if	( strcmp( pcValue[i], "LEL" ) == 0 ){ Unit = MEASURING_UNIT_LEL; }
				else										{ Unit = MEASURING_UNIT_VOL; }
			}
		}
		
		if( (HighScale > 99) && (HighScale < 10000) ){
			if( pMeas->pGas[iParameter]->Inf.Dp != Dp ){
				pMeas->pGas[iParameter]->Inf.Dp  = Dp;
				SenEEP_Write_CHn( &pMeas->pGas[iParameter]->Inf.Dp, pMeas->Conf.iOut[iParameter], SENSOR_EEP_CHn_DP, 1 );
			}
			if( pMeas->pGas[iParameter]->Inf.HighScale != HighScale ){
				
				roc = (float)HighScale / (float)pMeas->pGas[iParameter]->Inf.HighScale;
				
				pMeas->pGas[iParameter]->Inf.HighScale = HighScale;
				pMeas->pGas[iParameter]->Inf.SpanGas *= roc;
				pMeas->pAlr[iParameter]->Inf._1stLevel *= roc;
				pMeas->pAlr[iParameter]->Inf._2ndLevel *= roc;
					
				eep[0] = pMeas->pGas[iParameter]->Inf.HighScale;
				eep[1] = pMeas->pGas[iParameter]->Inf.SpanGas;
				eep[2] = pMeas->pAlr[iParameter]->Inf._1stLevel;
				eep[3] = pMeas->pAlr[iParameter]->Inf._2ndLevel;
				
				Measuring_Gas_MSD_Load( &pMeas->pGas[iParameter]->Inf );
				
				SenEEP_Write_CHn( &eep[0], pMeas->Conf.iOut[iParameter], SENSOR_EEP_CHn_HIGH_SCALE, 1 );
				SenEEP_Write_CHn( &eep[1], pMeas->Conf.iOut[iParameter], SENSOR_EEP_CHn_SPAN_GAS, 1 );
				SenEEP_Write_CHn( &eep[2], pMeas->Conf.iOut[iParameter], SENSOR_EEP_CHn_ALARM_1ST_LEVEL, 1 );
				SenEEP_Write_CHn( &eep[3], pMeas->Conf.iOut[iParameter], SENSOR_EEP_CHn_ALARM_2ND_LEVEL, 1 );
			}
		}
		if( pMeas->pGas[iParameter]->Inf.Unit != Unit ){
			pMeas->pGas[iParameter]->Inf.Unit  = Unit;
			
			SenEEP_Write_CHn( &pMeas->pGas[iParameter]->Inf.Unit, pMeas->Conf.iOut[iParameter], SENSOR_EEP_CHn_UNIT, 1 );
		}
	}
	
	return "/parameter.htm";  
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char *CGI_Handler_Alarm_Ch( int iIndex, int iNumParams, char *pcParam[], char *pcValue[] )
{
	uint32_t	i = 0;
	
	if( iIndex == CGI_INDEX_ALARM_CH ){
		
		if( strcmp( pcParam[i], "OUTPUT_SELECT" ) == 0 ){
			if( strcmp( pcValue[i], "OUT1" ) == 0 ){
				iAlarm = 0;
			}
			else if( strcmp( pcValue[i], "OUT2" ) == 0 ){
				if( pMeas->Conf.OutMax > 1 ){ iAlarm = 1; }
			}
		}
	}
	
	return "/alarm.htm";  
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char *CGI_Handler_Alarm( int iIndex, int iNumParams, char *pcParam[], char *pcValue[] )
{
	uint32_t				i = 0;
	char					*pDp;
	U16						Dp;
	Measuring_AlarmInfo		Alr;
	Measuring_AlarmInfo		*pAlr;
	Measuring_GasInfo		*pGas;
	
	U16						eep;
	
	if( iIndex == CGI_INDEX_ALARM ){
		
		pAlr = &pMeas->pAlr[iAlarm]->Inf;
		pGas = &pMeas->pGas[iAlarm]->Inf;
		
		for( i=0; i<iNumParams; i++ ){
			
			if( strcmp( pcParam[i], "AL1_LEVEL" ) == 0 ){
				Replace_Char( pcValue[i], '+', ' ' );
				URL_Decoding( pcValue[i] );
				pDp = strchr( pcValue[i], '.' );
				if( pDp ){
					Dp = strlen( pDp + 1 );
				}
				else {
					Dp = 0;
				}
				if( Dp > 3 ){
					Dp = 3;
				}
				Alr._1stLevel = (S16)(atof( pcValue[i] ) * pow( 10, Dp ));
				if( (Alr._1stLevel <= pGas->HighScale) &&
					(Alr._1stLevel >= (pGas->ZeroSkipPer / 10 * pGas->HighScale / 100 + 1)) &&
					(pGas->Dp == Dp) )
				{
					if( pAlr->_1stLevel != Alr._1stLevel ){
						pAlr->_1stLevel  = Alr._1stLevel;
						SenEEP_Write_CHn( (U16 *)&pAlr->_1stLevel, pMeas->Conf.iOut[iAlarm], SENSOR_EEP_CHn_ALARM_1ST_LEVEL, 1 );
					}
				}
			}
			else if( strcmp( pcParam[i], "AL1_DIR" ) == 0 ){
				if( strcmp( pcValue[i], "AL1_DI" ) == 0 ){ Alr._1stTypeIncrease = true; }
				else { Alr._1stTypeIncrease = false; }
				if( pAlr->_1stTypeIncrease != Alr._1stTypeIncrease ){
					pAlr->_1stTypeIncrease  = Alr._1stTypeIncrease;
					eep = pAlr->_1stTypeIncrease;
					SenEEP_Write_CHn( &eep, pMeas->Conf.iOut[iAlarm], SENSOR_EEP_CHn_ALARM_1ST_INCREASE_TYPE, 1 );
				}
			}
			else if( strcmp( pcParam[i], "AL1_DBAND" ) == 0 ){
				Replace_Char( pcValue[i], '+', ' ' );
				URL_Decoding( pcValue[i] );
				Alr._1stDead = atoi( pcValue[i] );
				if( Alr._1stDead <= 10 ){
					if( pAlr->_1stDead != Alr._1stDead ){
						pAlr->_1stDead  = Alr._1stDead;
						SenEEP_Write_CHn( &pAlr->_1stDead, pMeas->Conf.iOut[iAlarm], SENSOR_EEP_CHn_ALARM_1ST_DEAD_BAND, 1 );
					}
				}
			}
			else if( strcmp( pcParam[i], "AL1_DELAY" ) == 0 ){
				Replace_Char( pcValue[i], '+', ' ' );
				URL_Decoding( pcValue[i] );
				Alr._1stDelaySec = atoi( pcValue[i] );
				if( Alr._1stDelaySec <= 30 ){
					if( pAlr->_1stDelaySec != Alr._1stDelaySec ){
						pAlr->_1stDelaySec  = Alr._1stDelaySec;
						SenEEP_Write_CHn( &pAlr->_1stDelaySec, pMeas->Conf.iOut[iAlarm], SENSOR_EEP_CHn_ALARM_1ST_DELAY_SEC, 1 );
					}
				}
			}
			else if( strcmp( pcParam[i], "AL1_LAT" ) == 0 ){
				if( strcmp( pcValue[i], "AL1_LE" ) == 0 ){ Alr._1stLatchEnabled = true; }
				else { Alr._1stLatchEnabled = false; }
				if( pAlr->_1stLatchEnabled != Alr._1stLatchEnabled ){
					pAlr->_1stLatchEnabled  = Alr._1stLatchEnabled;
					eep = pAlr->_1stLatchEnabled;
					SenEEP_Write_CHn( &eep, pMeas->Conf.iOut[iAlarm], SENSOR_EEP_CHn_ALARM_1ST_LATCH, 1 );
				}
			}
			else if( strcmp( pcParam[i], "AL2_LEVEL" ) == 0 ){
				Replace_Char( pcValue[i], '+', ' ' );
				URL_Decoding( pcValue[i] );
				pDp = strchr( pcValue[i], '.' );
				if( pDp ){
					Dp = strlen( pDp + 1 );
				}
				else {
					Dp = 0;
				}
				if( Dp > 3 ){
					Dp = 3;
				}
				Alr._2ndLevel = (S16)(atof( pcValue[i] ) * pow( 10, Dp ));
				if( (Alr._2ndLevel <= pGas->HighScale) &&
					(Alr._2ndLevel >= (pGas->ZeroSkipPer / 10 * pGas->HighScale / 100 + 1)) &&
					(pGas->Dp == Dp) )
				{
					if( pAlr->_2ndLevel != Alr._2ndLevel ){
						pAlr->_2ndLevel  = Alr._2ndLevel;
						SenEEP_Write_CHn( (U16 *)&pAlr->_2ndLevel, pMeas->Conf.iOut[iAlarm], SENSOR_EEP_CHn_ALARM_2ND_LEVEL, 1 );
					}
				}
			}
			else if( strcmp( pcParam[i], "AL2_DIR" ) == 0 ){
				if( strcmp( pcValue[i], "AL2_DI" ) == 0 ){ Alr._2ndTypeIncrease = true; }
				else { Alr._2ndTypeIncrease = false; }
				if( pAlr->_2ndTypeIncrease != Alr._2ndTypeIncrease ){
					pAlr->_2ndTypeIncrease  = Alr._2ndTypeIncrease;
					eep = pAlr->_2ndTypeIncrease;
					SenEEP_Write_CHn( &eep, pMeas->Conf.iOut[iAlarm], SENSOR_EEP_CHn_ALARM_2ND_INCREASE_TYPE, 1 );
				}
			}
			else if( strcmp( pcParam[i], "AL2_DBAND" ) == 0 ){
				Replace_Char( pcValue[i], '+', ' ' );
				URL_Decoding( pcValue[i] );
				Alr._2ndDead = atoi( pcValue[i] );
				if( Alr._2ndDead <= 10 ){
					if( pAlr->_2ndDead != Alr._2ndDead ){
						pAlr->_2ndDead  = Alr._2ndDead;
						SenEEP_Write_CHn( &pAlr->_2ndDead, pMeas->Conf.iOut[iAlarm], SENSOR_EEP_CHn_ALARM_2ND_DEAD_BAND, 1 );
					}
				}
			}
			else if( strcmp( pcParam[i], "AL2_DELAY" ) == 0 ){
				Replace_Char( pcValue[i], '+', ' ' );
				URL_Decoding( pcValue[i] );
				Alr._2ndDelaySec = atoi( pcValue[i] );
				if( Alr._2ndDelaySec <= 30 ){
					if( pAlr->_2ndDelaySec != Alr._2ndDelaySec ){
						pAlr->_2ndDelaySec  = Alr._2ndDelaySec;
						SenEEP_Write_CHn( &pAlr->_2ndDelaySec, pMeas->Conf.iOut[iAlarm], SENSOR_EEP_CHn_ALARM_2ND_DELAY_SEC, 1 );
					}
				}
			}
			else if( strcmp( pcParam[i], "AL2_LAT" ) == 0 ){
				if( strcmp( pcValue[i], "AL2_LE" ) == 0 ){ Alr._2ndLatchEnabled = true; }
				else { Alr._2ndLatchEnabled = false; }
				if( pAlr->_2ndLatchEnabled != Alr._2ndLatchEnabled ){
					pAlr->_2ndLatchEnabled  = Alr._2ndLatchEnabled;
					eep = pAlr->_2ndLatchEnabled;
					SenEEP_Write_CHn( &eep, pMeas->Conf.iOut[iAlarm], SENSOR_EEP_CHn_ALARM_2ND_LATCH, 1 );
				}
			}
		}
	}
	
	return "/alarm.htm";  
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char *CGI_Handler_Relay( int iIndex, int iNumParams, char *pcParam[], char *pcValue[] )
{
	uint32_t				i = 0;
	Relay_DriveType			Set;
	U16						eep;
	
	if( iIndex == CGI_INDEX_RELAY ){
		
		Set.TRB.Opt.Word = 0;
		Set.AL1.Opt.Word = 0;
		Set.AL2.Opt.Word = 0;
		
		for( i=0; i<iNumParams; i++ ){
			
			if( (strcmp( pcParam[i], "TRB_OUT" ) == 0) && (strcmp( pcValue[i], "TRB_OE" ) == 0) ){
				Set.TRB.Opt.Item.IsOutputEnable = true;
			}
			else if( (strcmp( pcParam[i], "TRB_ENG" ) == 0) && (strcmp( pcValue[i], "TRB_EE" ) == 0) ){
				Set.TRB.Opt.Item.IsEnergizeMode = true;
			}
			else if( (strcmp( pcParam[i], "TRB_BLK" ) == 0) && (strcmp( pcValue[i], "TRB_BE" ) == 0) ){
				Set.TRB.Opt.Item.IsBlinkingMode = true;
			}
			else if( strcmp( pcParam[i], "TRB_ACT_TIME" ) == 0 ){
				Replace_Char( pcValue[i], '+', ' ' );
				URL_Decoding( pcValue[i] );
				Set.TRB.Blk.Item.Act_Time = ((U8)atoi( pcValue[i] ) & 0x3F);
				if( (Set.TRB.Blk.Item.Act_Time < 1) || (Set.TRB.Blk.Item.Act_Time > 60) ){
					 Set.TRB.Blk.Item.Act_Time = RelayDrive.TRB.Blk.Item.Act_Time;
				}
			}
			else if( strcmp( pcParam[i], "TRB_REL_TIME" ) == 0 ){
				Replace_Char( pcValue[i], '+', ' ' );
				URL_Decoding( pcValue[i] );
				Set.TRB.Blk.Item.Rel_Time = ((U8)atoi( pcValue[i] ) & 0x3F);
				if( (Set.TRB.Blk.Item.Rel_Time < 1) || (Set.TRB.Blk.Item.Rel_Time > 60) ){
					 Set.TRB.Blk.Item.Rel_Time = RelayDrive.TRB.Blk.Item.Rel_Time;
				}
			}
			else if( (strcmp( pcParam[i], "AL1_OUT" ) == 0) && (strcmp( pcValue[i], "AL1_OE" ) == 0) ){
				Set.AL1.Opt.Item.IsOutputEnable = true;
			}
			else if( (strcmp( pcParam[i], "AL1_ENG" ) == 0) && (strcmp( pcValue[i], "AL1_EE" ) == 0) ){
				Set.AL1.Opt.Item.IsEnergizeMode = true;
			}
			else if( (strcmp( pcParam[i], "AL1_BLK" ) == 0) && (strcmp( pcValue[i], "AL1_BE" ) == 0) ){
				Set.AL1.Opt.Item.IsBlinkingMode = true;
			}
			else if( strcmp( pcParam[i], "AL1_ACT_TIME" ) == 0 ){
				Replace_Char( pcValue[i], '+', ' ' );
				URL_Decoding( pcValue[i] );
				Set.AL1.Blk.Item.Act_Time = ((U8)atoi( pcValue[i] ) & 0x3F);
				if( (Set.AL1.Blk.Item.Act_Time < 1) || (Set.AL1.Blk.Item.Act_Time > 60) ){
					 Set.AL1.Blk.Item.Act_Time = RelayDrive.AL1.Blk.Item.Act_Time;
				}
			}
			else if( strcmp( pcParam[i], "AL1_REL_TIME" ) == 0 ){
				Replace_Char( pcValue[i], '+', ' ' );
				URL_Decoding( pcValue[i] );
				Set.AL1.Blk.Item.Rel_Time = ((U8)atoi( pcValue[i] ) & 0x3F);
				if( (Set.AL1.Blk.Item.Rel_Time < 1) || (Set.AL1.Blk.Item.Rel_Time > 60) ){
					 Set.AL1.Blk.Item.Rel_Time = RelayDrive.AL1.Blk.Item.Rel_Time;
				}
			}
			else if( (strcmp( pcParam[i], "AL2_OUT" ) == 0) && (strcmp( pcValue[i], "AL2_OE" ) == 0) ){
				Set.AL2.Opt.Item.IsOutputEnable = true;
			}
			else if( (strcmp( pcParam[i], "AL2_ENG" ) == 0) && (strcmp( pcValue[i], "AL2_EE" ) == 0) ){
				Set.AL2.Opt.Item.IsEnergizeMode = true;
			}
			else if( (strcmp( pcParam[i], "AL2_BLK" ) == 0) && (strcmp( pcValue[i], "AL2_BE" ) == 0) ){
				Set.AL2.Opt.Item.IsBlinkingMode = true;
			}
			else if( strcmp( pcParam[i], "AL2_ACT_TIME" ) == 0 ){
				Replace_Char( pcValue[i], '+', ' ' );
				URL_Decoding( pcValue[i] );
				Set.AL2.Blk.Item.Act_Time = ((U8)atoi( pcValue[i] ) & 0x3F);
				if( (Set.AL2.Blk.Item.Act_Time < 1) || (Set.AL2.Blk.Item.Act_Time > 60) ){
					 Set.AL2.Blk.Item.Act_Time = RelayDrive.AL2.Blk.Item.Act_Time;
				}
			}
			else if( strcmp( pcParam[i], "AL2_REL_TIME" ) == 0 ){
				Replace_Char( pcValue[i], '+', ' ' );
				URL_Decoding( pcValue[i] );
				Set.AL2.Blk.Item.Rel_Time = ((U8)atoi( pcValue[i] ) & 0x3F);
				if( (Set.AL2.Blk.Item.Rel_Time < 1) || (Set.AL2.Blk.Item.Rel_Time > 60) ){
					 Set.AL2.Blk.Item.Rel_Time = RelayDrive.AL2.Blk.Item.Rel_Time;
				}
			}
		}
		
		if( RelayDrive.TRB.Opt.Word != Set.TRB.Opt.Word ){
			if( RelayDrive.TRB.Opt.Item.IsEnergizeMode != Set.TRB.Opt.Item.IsEnergizeMode ){
				RelayDrive.TRB.Out = RelayDrive.TRB.Set ^ true;
			}
			RelayDrive.TRB.Opt.Word = Set.TRB.Opt.Word;
			EEPROM_Update( &RelayDrive.TRB.Opt.Word, SYSTEM_EEP_RELAY_OPTION_TRB, 1 );
		}
		if( RelayDrive.AL1.Opt.Word != Set.AL1.Opt.Word ){
			if( RelayDrive.AL1.Opt.Item.IsEnergizeMode != Set.AL1.Opt.Item.IsEnergizeMode ){
				RelayDrive.AL1.Out = RelayDrive.AL1.Set ^ true;
			}
			RelayDrive.AL1.Opt.Word = Set.AL1.Opt.Word;
			EEPROM_Update( &RelayDrive.AL1.Opt.Word, SYSTEM_EEP_RELAY_OPTION_AL1, 1 );
		}
		if( RelayDrive.AL2.Opt.Word != Set.AL2.Opt.Word ){
			if( RelayDrive.AL2.Opt.Item.IsEnergizeMode != Set.AL2.Opt.Item.IsEnergizeMode ){
				RelayDrive.AL2.Out = RelayDrive.AL2.Set ^ true;
			}
			RelayDrive.AL2.Opt.Word = Set.AL2.Opt.Word;
			EEPROM_Update( &RelayDrive.AL2.Opt.Word, SYSTEM_EEP_RELAY_OPTION_AL2, 1 );
		}
		
		if( RelayDrive.TRB.Blk.Item.Act_Time != Set.TRB.Blk.Item.Act_Time ||
			RelayDrive.TRB.Blk.Item.Rel_Time != Set.TRB.Blk.Item.Rel_Time ){
			RelayDrive.TRB.Blk.Item.Act_Time  = Set.TRB.Blk.Item.Act_Time;	
			RelayDrive.TRB.Blk.Item.Rel_Time  = Set.TRB.Blk.Item.Rel_Time;
			eep = ((U16)RelayDrive.TRB.Blk.Item.Act_Time << 8 & 0xFF00) | ((U16)RelayDrive.TRB.Blk.Item.Rel_Time & 0x00FF);
			EEPROM_Update( &eep, SYSTEM_EEP_RELAY_BLINK_TIME_TRB, 1 );
		}
		if( RelayDrive.AL1.Blk.Item.Act_Time != Set.AL1.Blk.Item.Act_Time ||
			RelayDrive.AL1.Blk.Item.Rel_Time != Set.AL1.Blk.Item.Rel_Time ){
			RelayDrive.AL1.Blk.Item.Act_Time  = Set.AL1.Blk.Item.Act_Time;	
			RelayDrive.AL1.Blk.Item.Rel_Time  = Set.AL1.Blk.Item.Rel_Time;
			eep = ((U16)RelayDrive.AL1.Blk.Item.Act_Time << 8 & 0xFF00) | ((U16)RelayDrive.AL1.Blk.Item.Rel_Time & 0x00FF);
			EEPROM_Update( &eep, SYSTEM_EEP_RELAY_BLINK_TIME_AL1, 1 );
		}
		if( RelayDrive.AL2.Blk.Item.Act_Time != Set.AL2.Blk.Item.Act_Time ||
			RelayDrive.AL2.Blk.Item.Rel_Time != Set.AL2.Blk.Item.Rel_Time ){
			RelayDrive.AL2.Blk.Item.Act_Time  = Set.AL2.Blk.Item.Act_Time;	
			RelayDrive.AL2.Blk.Item.Rel_Time  = Set.AL2.Blk.Item.Rel_Time;
			eep = ((U16)RelayDrive.AL2.Blk.Item.Act_Time << 8 & 0xFF00) | ((U16)RelayDrive.AL2.Blk.Item.Rel_Time & 0x00FF);
			EEPROM_Update( &eep, SYSTEM_EEP_RELAY_BLINK_TIME_AL2, 1 );
		}
	}
	
	return "/relay.htm";  
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char *CGI_Handler_Test_Ch( int iIndex, int iNumParams, char *pcParam[], char *pcValue[] )
{
	uint32_t	i = 0;
	
	if( iIndex == CGI_INDEX_TEST_CH ){
		
		if( strcmp( pcParam[i], "OUTPUT_SELECT" ) == 0 ){
			if( strcmp( pcValue[i], "OUT1" ) == 0 ){
				iTest = 0;
			}
			else if( strcmp( pcValue[i], "OUT2" ) == 0 ){
				if( pMeas->Conf.OutMax > 1 ){ iTest = 1; }
			}
		}
	}
	
	return "/test.htm";  
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char *CGI_Handler_Test( int iIndex, int iNumParams, char *pcParam[], char *pcValue[] )
{
	uint32_t	i = 0;
	char		*pDp;
	U16			Dp;
	S16			SetGas;
	
	if( iIndex == CGI_INDEX_TEST ){
		
		for( i=0; i<iNumParams; i++ ){
			
			if( strcmp( pcParam[i], "MODE" ) == 0 ){
				if ( strcmp( pcValue[i], "TEST" ) == 0 ){
					switch( iTest ){
						case 0: if( pMeas->Test.Item.RemoteTest_CH1 == false ){ pMeas->Test.Item.WebTest_CH1 = true; } break;
						case 1: if( pMeas->Test.Item.RemoteTest_CH2 == false ){ pMeas->Test.Item.WebTest_CH2 = true; } break;
					}
				}
				else {
					switch( iTest ){
						case 0: pMeas->Test.Item.WebTest_CH1 = false; break;
						case 1: pMeas->Test.Item.WebTest_CH2 = false; break;
					}
				}
			}
			else if( strcmp( pcParam[i], "RELAY_OUTPUT" ) == 0 ){
				if( strcmp( pcValue[i], "RLY_OE" ) == 0 ){ RelayDrive.UseWebTest =  true; }
				else									 { RelayDrive.UseWebTest = false; }
			}
			else if( strcmp( pcParam[i], "TEST_GAS" ) == 0 ){
				Replace_Char( pcValue[i], '+', ' ' );
				URL_Decoding( pcValue[i] );
				pDp = strchr( pcValue[i], '.' );
				if( pDp ){
					Dp = strlen( pDp + 1 );
				}
				else{
					Dp = 0;
				}
				if( Dp > 3 ){
					Dp = 3;
				}
				SetGas = (S16)(atof( pcValue[i] ) * pow( 10, Dp ));
				if( (SetGas > -10000) && (SetGas < 10000) ){ pMeas->pGas[iTest]->Set = SetGas; }
			}
		}
	}
	
	return "/test.htm";  
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char *CGI_Handler_Network( int iIndex, int iNumParams, char *pcParam[], char *pcValue[] )
{
	uint32_t			i = 0;
	POE_ConfigType		Set;
	U16					eep[6];
	
	if( iIndex == CGI_INDEX_NETWORK ){
		
		for( i=0; i<iNumParams; i++ ){
			
			Replace_Char( pcValue[i], '+', ' ' );
			URL_Decoding( pcValue[i] );
			
			if		( strcmp( pcParam[i], "IP1" ) == 0 ){ Set.IP[0] = (uint8_t)atoi( pcValue[i] ) & 255; }
			else if	( strcmp( pcParam[i], "IP2" ) == 0 ){ Set.IP[1] = (uint8_t)atoi( pcValue[i] ) & 255; }
			else if	( strcmp( pcParam[i], "IP3" ) == 0 ){ Set.IP[2] = (uint8_t)atoi( pcValue[i] ) & 255; }
			else if	( strcmp( pcParam[i], "IP4" ) == 0 ){ Set.IP[3] = (uint8_t)atoi( pcValue[i] ) & 255; }
			else if	( strcmp( pcParam[i], "NM1" ) == 0 ){ Set.NM[0] = (uint8_t)atoi( pcValue[i] ) & 255; }
			else if	( strcmp( pcParam[i], "NM2" ) == 0 ){ Set.NM[1] = (uint8_t)atoi( pcValue[i] ) & 255; }
			else if	( strcmp( pcParam[i], "NM3" ) == 0 ){ Set.NM[2] = (uint8_t)atoi( pcValue[i] ) & 255; }
			else if	( strcmp( pcParam[i], "NM4" ) == 0 ){ Set.NM[3] = (uint8_t)atoi( pcValue[i] ) & 255; }
			else if	( strcmp( pcParam[i], "GW1" ) == 0 ){ Set.GW[0] = (uint8_t)atoi( pcValue[i] ) & 255; }
			else if	( strcmp( pcParam[i], "GW2" ) == 0 ){ Set.GW[1] = (uint8_t)atoi( pcValue[i] ) & 255; }
			else if	( strcmp( pcParam[i], "GW3" ) == 0 ){ Set.GW[2] = (uint8_t)atoi( pcValue[i] ) & 255; }
			else if	( strcmp( pcParam[i], "GW4" ) == 0 ){ Set.GW[3] = (uint8_t)atoi( pcValue[i] ) & 255; }
		}
		
		if( (PoeHandle.Conf.IP[0] != Set.IP[0]) || (PoeHandle.Conf.IP[1] != Set.IP[1]) ||
			(PoeHandle.Conf.IP[2] != Set.IP[2]) || (PoeHandle.Conf.IP[3] != Set.IP[3]) ||
			(PoeHandle.Conf.NM[0] != Set.NM[0]) || (PoeHandle.Conf.NM[1] != Set.NM[1]) ||
			(PoeHandle.Conf.NM[2] != Set.NM[2]) || (PoeHandle.Conf.NM[3] != Set.NM[3]) ||
			(PoeHandle.Conf.GW[0] != Set.GW[0]) || (PoeHandle.Conf.GW[1] != Set.GW[1]) ||
			(PoeHandle.Conf.GW[2] != Set.GW[2]) || (PoeHandle.Conf.GW[3] != Set.GW[3]) )
		{
			PoeHandle.Conf.IP[0] = Set.IP[0];
			PoeHandle.Conf.IP[1] = Set.IP[1];
			PoeHandle.Conf.IP[2] = Set.IP[2];
			PoeHandle.Conf.IP[3] = Set.IP[3];
			PoeHandle.Conf.NM[0] = Set.NM[0];
			PoeHandle.Conf.NM[1] = Set.NM[1];
			PoeHandle.Conf.NM[2] = Set.NM[2];
			PoeHandle.Conf.NM[3] = Set.NM[3];
			PoeHandle.Conf.GW[0] = Set.GW[0];
			PoeHandle.Conf.GW[1] = Set.GW[1];
			PoeHandle.Conf.GW[2] = Set.GW[2];
			PoeHandle.Conf.GW[3] = Set.GW[3];
			eep[0] = ((U16)PoeHandle.Conf.IP[0] << 8 & 0xFF00) | ((U16)PoeHandle.Conf.IP[1] & 0x00FF);
			eep[1] = ((U16)PoeHandle.Conf.IP[2] << 8 & 0xFF00) | ((U16)PoeHandle.Conf.IP[3] & 0x00FF);
			eep[2] = ((U16)PoeHandle.Conf.NM[0] << 8 & 0xFF00) | ((U16)PoeHandle.Conf.NM[1] & 0x00FF);
			eep[3] = ((U16)PoeHandle.Conf.NM[2] << 8 & 0xFF00) | ((U16)PoeHandle.Conf.NM[3] & 0x00FF);
			eep[4] = ((U16)PoeHandle.Conf.GW[0] << 8 & 0xFF00) | ((U16)PoeHandle.Conf.GW[1] & 0x00FF);
			eep[5] = ((U16)PoeHandle.Conf.GW[2] << 8 & 0xFF00) | ((U16)PoeHandle.Conf.GW[3] & 0x00FF);
			
			EEPROM_Update( eep, SYSTEM_EEP_POE_IP_0_1, 6 );
			POE_ReInit();
			
			return NULL;
		}
	}
	
	return "/network.htm";  
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char *CGI_Handler_User( int iIndex, int iNumParams, char *pcParam[], char *pcValue[] )
{
	uint32_t	i = 0;
	uint16_t	len;
	char		str[22];
	
	if( iIndex == CGI_INDEX_USER ){
		
		for( i=0; i<iNumParams; i++ ){
			
			Replace_Char( pcValue[i], '+', ' ' );
			URL_Decoding( pcValue[i] );
			len = strlen( pcValue[i] );
			
			if( len < 22 ){
				
				strcpy( str, "                     " );
				
				if( strcmp( pcParam[i], "NAME" ) == 0 ){
					if( strcmp( (char *)EepHandle.Tag.Name, pcValue[i] ) != 0 ){
						strcpy( str, pcValue[i] );
						if( len < 21 ){ str[len] = ' '; }
						strcpy( (char *)EepHandle.Tag.Name, str );
						EEPROM_Tag_Write( EepHandle.Tag.Name, SYSTEM_TAG_NAME_00, 11 );
					}
				}
				else if( strcmp( pcParam[i], "LOCATION" ) == 0 ){
					if( strcmp( (char *)EepHandle.Tag.Location, pcValue[i] ) != 0 ){
						strcpy( str, pcValue[i] );
						if( len < 21 ){ str[len] = ' '; }
						strcpy( (char *)EepHandle.Tag.Location, str );
						EEPROM_Tag_Write( EepHandle.Tag.Location, SYSTEM_TAG_LOCATION_00, 11 );
					}
				}
				else if( strcmp( pcParam[i], "MESSAGE" ) == 0 ){
					if( strcmp( (char *)EepHandle.Tag.Message, pcValue[i] ) != 0 ){
						strcpy( str, pcValue[i] );
						if( len < 21 ){ str[len] = ' '; }
						strcpy( (char *)EepHandle.Tag.Message, str );
						EEPROM_Tag_Write( EepHandle.Tag.Message, SYSTEM_TAG_MESSAGE_00, 11 );
					}
				}
			}
		}
	}
	
	return "/user.htm";  
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char *CGI_Handler_Factory( int iIndex, int iNumParams, char *pcParam[], char *pcValue[] )
{
	uint32_t	i = 0;
	uint16_t	len;
	char		str[12];
	
	if( iIndex == CGI_INDEX_FACTORY ){
		
		for( i=0; i<iNumParams; i++ ){
			
			Replace_Char( pcValue[i], '+', ' ' );
			URL_Decoding( pcValue[i] );
			len = strlen( pcValue[i] );
			
			if( len < 12 ){ 
				
				strcpy( str, "           " );
			
				if( strcmp( pcParam[i], "SERIAL_NUM" ) == 0 ){
					if( strcmp( (char *)EepHandle.Tag.SerialNumber, pcValue[i] ) != 0 ){
						strcpy( str, pcValue[i] );
						if( len < 11 ){ str[len] = ' '; }
						strcpy( (char *)EepHandle.Tag.SerialNumber, str );
						EEPROM_Tag_Write( EepHandle.Tag.SerialNumber, SYSTEM_TAG_SERIAL_NUM_00, 6 );
					}
				}
			}
		}
	}
	
	return "/factory.htm";  
}

