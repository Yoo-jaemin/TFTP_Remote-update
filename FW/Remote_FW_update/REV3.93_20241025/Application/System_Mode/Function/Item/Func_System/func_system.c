#define __FUNC_SYSTEM_C__
    #include "func_system.h"
#undef  __FUNC_SYSTEM_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "vfunc_system.h"
#include "password_mode.h"
#include "eeprom.h"
#include "lcd.h"
#include "option.h"
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
static void KeyProcess( Func_HandleType *pFunc );
/* End Function */

/* Start Variable */
static System_SetType		SysSet;
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Func_System( Func_HandleType *pFunc )
{
	KeyProcess( pFunc );
	
	if( pFunc->ViewEvt == true ){ vFunc_System( pFunc, &SysSet ); }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static uint8_t MaxDay_Search( RTC_DateTypeDef *pDate )
{
	uint8_t		Day[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	uint8_t		Max = Day[pDate->Month-1];
	
	if( pDate->Month == 2 ){
		if( ((pDate->Year % 4 == 0) && (pDate->Year % 100 != 0)) || (pDate->Year % 400 == 0) ){
			Max++;
		}
	}
	
	return Max;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void KeyProcess( Func_HandleType *pFunc )
{
	uint8_t		md = MaxDay_Search( &SysSet.sDate );
	
	switch( pFunc->Level ){
		case FUNC_LEVEL_1:
			SysSet.pDate = &SysCalendar.Date;
			SysSet.pTime = &SysCalendar.Time24;
			SysSet.pPW = &PwHandle.SysPW;
			SysSet.pBrightness = &LcdDrive.BrightnessLevel;
			SysSet.pBuzVolume = &BuzDrive.Volume;
			pFunc->Item.Lv2 = 0;
			break;
			
		case FUNC_LEVEL_2: // Menu Set
			switch( pFunc->KeyData ){
				case Function:
					switch( pFunc->Item.Lv2 ){
						case 0:
							SysSet.sDate = *SysSet.pDate;
							SysSet.sTime = *SysSet.pTime;
							break;
							
						case 1:
							SysSet.sPW = *SysSet.pPW;
							break;
							
						case 2:
							SysSet.sBrightness = *SysSet.pBrightness;
							break;
							
						case 3:
							SysSet.sBuzVolume = *SysSet.pBuzVolume;
							break;
					}
					pFunc->Level = FUNC_LEVEL_3;
					pFunc->IsApproved = false;
					pFunc->Item.Lv3 = 0;
					pFunc->Item.Lv4 = 0;
					break;
					
				case Up:
					if( pFunc->Item.Lv2 > 0 ){
						pFunc->Item.Lv2--;
					}
					break;
					
				case Down:
					if( SystemOption.Item.BuzPresent == true ){
						if( pFunc->Item.Lv2 < 3 ){
							pFunc->Item.Lv2++;
						}
					}
					else {
						if( pFunc->Item.Lv2 < 2 ){
							pFunc->Item.Lv2++;
						}
					}
					break;
					
				case Reset:
					MoveFuncTop();
					break;
			}
			break;
			
		case FUNC_LEVEL_3:
			switch( pFunc->Item.Lv2 ){
				case 0: // Calendar Set
					switch( pFunc->KeyData ){
						case Function:
							switch( pFunc->Item.Lv3 ){
								case 6:
									if( pFunc->IsApproved == true ){
										if( RTC_Set_Calendar( &SysSet.sDate, &SysSet.sTime ) == false ){
											pFunc->Item.Lv3 = 7;
										}
									}
									else {
										pFunc->Level = FUNC_LEVEL_2;
									}
									break;
									
								case 7:
									pFunc->Level = FUNC_LEVEL_2;
									break;
									
								default:
									pFunc->Item.Lv3++;
									break;
							}
							break;
							
						case Up:
							switch( pFunc->Item.Lv3 ){
								case 0: if( SysSet.sDate.Year		< 99 ){ SysSet.sDate.Year++;	} break;
								case 1: if( SysSet.sDate.Month		< 12 ){ SysSet.sDate.Month++;	} break;
								case 2: if( SysSet.sDate.Date		< md ){ SysSet.sDate.Date++;	} break;
								case 3: if( SysSet.sTime.Hours		< 23 ){ SysSet.sTime.Hours++;	} break;
								case 4: if( SysSet.sTime.Minutes	< 59 ){ SysSet.sTime.Minutes++;	} break;
								case 5: if( SysSet.sTime.Seconds	< 59 ){ SysSet.sTime.Seconds++;	} break;
								case 6: pFunc->IsApproved ^= true; break;
							}
							break;
							
						case Down:
							switch( pFunc->Item.Lv3 ){
								case 0: if( SysSet.sDate.Year		>  0 ){ SysSet.sDate.Year--;	} break;
								case 1: if( SysSet.sDate.Month		>  1 ){ SysSet.sDate.Month--;	} break;
								case 2: if( SysSet.sDate.Date		>  1 ){ SysSet.sDate.Date--;	} break;
								case 3: if( SysSet.sTime.Hours		>  0 ){ SysSet.sTime.Hours--;	} break;
								case 4: if( SysSet.sTime.Minutes	>  0 ){ SysSet.sTime.Minutes--;	} break;
								case 5: if( SysSet.sTime.Seconds	>  0 ){ SysSet.sTime.Seconds--;	} break;
								case 6: pFunc->IsApproved ^= true; break;
							}
							break;
							
						case Reset:
							switch( pFunc->Item.Lv3 ){
								case 0:
								case 7:
									pFunc->Level = FUNC_LEVEL_2;
									break;
									
								default:
									pFunc->Item.Lv3--;
									break;
							}
							break;
					}
					md = MaxDay_Search( &SysSet.sDate );
					if( SysSet.sDate.Date > md ){ SysSet.sDate.Date = md; }
					break;
					
				case 1: // Password Set
					switch( pFunc->KeyData ){
						case Function:
							if( pFunc->Item.Lv3 < 1 ){
								pFunc->Item.Lv3++;
							}
							else {
								if( pFunc->IsApproved == true ){
									if( *SysSet.pPW != SysSet.sPW ){
										*SysSet.pPW  = SysSet.sPW;
										EEPROM_Update( &SysSet.sPW, SYSTEM_EEP_PASSWORD, 1 );
									}
								}
								pFunc->Level = FUNC_LEVEL_2;
							}
							break;
							
						case Up:
							switch( pFunc->Item.Lv3 ){
								case 0: if( SysSet.sPW < 99 ){ SysSet.sPW++; } break;
								case 1: pFunc->IsApproved ^= true; break;
							}
							break;
							
						case Down:
							switch( pFunc->Item.Lv3 ){
								case 0: if( SysSet.sPW >  0 ){ SysSet.sPW--; } break;
								case 1: pFunc->IsApproved ^= true; break;
							}
							break;
							
						case Reset:
							if( pFunc->Item.Lv3 ){
								pFunc->Item.Lv3--;
							}
							else {
								pFunc->Level = FUNC_LEVEL_2;
							}
							break;
					}
					break;
					
				case 2: // LCD Brightness Set
					switch( pFunc->KeyData ){
						case Function:
							if( pFunc->Item.Lv3 < 1 ){
								pFunc->Item.Lv3++;
							}
							else {
								if( pFunc->IsApproved == true ){
									if( *SysSet.pBrightness != SysSet.sBrightness ){
										*SysSet.pBrightness  = SysSet.sBrightness;
										SysSet.sBrightness |= LCD_BRIGHTNESS_EEP_PREFIX;
										EEPROM_Update( &SysSet.sBrightness, SYSTEM_EEP_LCD_BRIGHTNESS_LEVEL, 1 );
									}
								}
								else {
									LcdDrive.Brightness_Set( *SysSet.pBrightness );
								}
								pFunc->Level = FUNC_LEVEL_2;
							}
							break;
							
						case Up:
							switch( pFunc->Item.Lv3 ){
								case 0:
									if( SysSet.sBrightness < 100 ){
										SysSet.sBrightness++;
										LcdDrive.Brightness_Set( SysSet.sBrightness );
									}
									break;
									
								case 1:
									pFunc->IsApproved ^= true;
									break;
							}
							break;
							
						case Down:
							switch( pFunc->Item.Lv3 ){
								case 0:
									if( SysSet.sBrightness ){
										SysSet.sBrightness--;
										LcdDrive.Brightness_Set( SysSet.sBrightness );
									}
									break;
									
								case 1:
									pFunc->IsApproved ^= true;
									break;
							}
							break;
							
						case Reset:
							if( pFunc->Item.Lv3 ){
								pFunc->Item.Lv3--;
							}
							else {
								LcdDrive.Brightness_Set( *SysSet.pBrightness );
								pFunc->Level = FUNC_LEVEL_2;
							}
							break;
					}
					break;
					
				case 3: // Buzzer Set
					switch( pFunc->KeyData ){
						case Function:
							if( pFunc->Item.Lv3 < 1 ){
								pFunc->Item.Lv3++;
							}
							else {
								if( pFunc->IsApproved == true ){
									if( *SysSet.pBuzVolume != SysSet.sBuzVolume ){
										*SysSet.pBuzVolume  = SysSet.sBuzVolume;
										EEPROM_Update( SysSet.pBuzVolume, SYSTEM_EEP_BUZZER_VOLUME, 1 );
									}
								}
								Buzzer_Volume_Set( *SysSet.pBuzVolume );
								pFunc->Level = FUNC_LEVEL_2;
							}
							break;
							
						case Up:
							switch( pFunc->Item.Lv3 ){
								case 0:
									if( SysSet.sBuzVolume < 8 ){
										SysSet.sBuzVolume++;
									}
									Buzzer_PlayTone( BUZZER_RATED_FREQ, SysSet.sBuzVolume, 10 );
									break;
									
								case 1:
									pFunc->IsApproved ^= true;
									break;
							}
							break;
							
						case Down:
							switch( pFunc->Item.Lv3 ){
								case 0:
									if( SysSet.sBuzVolume ){
										SysSet.sBuzVolume--;
									}
									Buzzer_PlayTone( BUZZER_RATED_FREQ, SysSet.sBuzVolume, 10 );
									break;
									
								case 1:
									pFunc->IsApproved ^= true;
									break;
							}
							break;
							
						case Reset:
							if( pFunc->Item.Lv3 ){
								pFunc->Item.Lv3--;
							}
							else {
								Buzzer_Volume_Set( *SysSet.pBuzVolume );
								pFunc->Level = FUNC_LEVEL_2;
							}
							break;
					}
					break;
			}
			break;
	}
}


