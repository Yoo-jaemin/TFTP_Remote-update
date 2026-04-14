#define __FUNC_INTERFACE_C__
    #include "func_interface.h"
#undef  __FUNC_INTERFACE_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "vfunc_interface.h"
#include "poe.h"
#include "eeprom.h"
#include "iwdg.h"
#include "option.h"
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
static void Interface_Config_Update_POE( void );
static void Interface_Config_Update_Relay( void );
static void Interface_Config_Update_485( void );
static void Interface_Config_Update_mA_CalVal( void );
static void Interface_Config_Update_mA_Offset( void );
static void uA_Set( U8 ch, float uA );
static void mA_Test_Handler( void );
static void KeyProcess( Func_HandleType *pFunc );
/* End Function */

/* Start Variable */
static Interface_SetType		IfSet;
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Func_Interface( Func_HandleType *pFunc )
{
	KeyProcess( pFunc );
	mA_Test_Handler();
	
	if( pFunc->ViewEvt == true ){ vFunc_Poe( pFunc, &IfSet ); }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Interface_Config_Update_POE( void )
{
	U16		eep[9];
	
	if( (IfSet.pPoe->MC[0] != IfSet.sPoe.MC[0]) || (IfSet.pPoe->MC[1] != IfSet.sPoe.MC[1]) ||
		(IfSet.pPoe->MC[2] != IfSet.sPoe.MC[2]) || (IfSet.pPoe->MC[3] != IfSet.sPoe.MC[3]) ||
		(IfSet.pPoe->MC[4] != IfSet.sPoe.MC[4]) || (IfSet.pPoe->MC[5] != IfSet.sPoe.MC[5]) ||
		(IfSet.pPoe->IP[0] != IfSet.sPoe.IP[0]) || (IfSet.pPoe->IP[1] != IfSet.sPoe.IP[1]) ||
		(IfSet.pPoe->IP[2] != IfSet.sPoe.IP[2]) || (IfSet.pPoe->IP[3] != IfSet.sPoe.IP[3]) ||
		(IfSet.pPoe->NM[0] != IfSet.sPoe.NM[0]) || (IfSet.pPoe->NM[1] != IfSet.sPoe.NM[1]) ||
		(IfSet.pPoe->NM[2] != IfSet.sPoe.NM[2]) || (IfSet.pPoe->NM[3] != IfSet.sPoe.NM[3]) ||
		(IfSet.pPoe->GW[0] != IfSet.sPoe.GW[0]) || (IfSet.pPoe->GW[1] != IfSet.sPoe.GW[1]) ||
		(IfSet.pPoe->GW[2] != IfSet.sPoe.GW[2]) || (IfSet.pPoe->GW[3] != IfSet.sPoe.GW[3]) )
	{
		IfSet.pPoe->MC[0] = IfSet.sPoe.MC[0];
		IfSet.pPoe->MC[1] = IfSet.sPoe.MC[1];
		IfSet.pPoe->MC[2] = IfSet.sPoe.MC[2];
		IfSet.pPoe->MC[3] = IfSet.sPoe.MC[3];
		IfSet.pPoe->MC[4] = IfSet.sPoe.MC[4];
		IfSet.pPoe->MC[5] = IfSet.sPoe.MC[5];
		IfSet.pPoe->IP[0] = IfSet.sPoe.IP[0];
		IfSet.pPoe->IP[1] = IfSet.sPoe.IP[1];
		IfSet.pPoe->IP[2] = IfSet.sPoe.IP[2];
		IfSet.pPoe->IP[3] = IfSet.sPoe.IP[3];
		IfSet.pPoe->NM[0] = IfSet.sPoe.NM[0];
		IfSet.pPoe->NM[1] = IfSet.sPoe.NM[1];
		IfSet.pPoe->NM[2] = IfSet.sPoe.NM[2];
		IfSet.pPoe->NM[3] = IfSet.sPoe.NM[3];
		IfSet.pPoe->GW[0] = IfSet.sPoe.GW[0];
		IfSet.pPoe->GW[1] = IfSet.sPoe.GW[1];
		IfSet.pPoe->GW[2] = IfSet.sPoe.GW[2];
		IfSet.pPoe->GW[3] = IfSet.sPoe.GW[3];
		eep[0] = ((U16)IfSet.pPoe->MC[0] << 8 & 0xFF00) | ((U16)IfSet.pPoe->MC[1] & 0x00FF);
		eep[1] = ((U16)IfSet.pPoe->MC[2] << 8 & 0xFF00) | ((U16)IfSet.pPoe->MC[3] & 0x00FF);
		eep[2] = ((U16)IfSet.pPoe->MC[4] << 8 & 0xFF00) | ((U16)IfSet.pPoe->MC[5] & 0x00FF);
		eep[3] = ((U16)IfSet.pPoe->IP[0] << 8 & 0xFF00) | ((U16)IfSet.pPoe->IP[1] & 0x00FF);
		eep[4] = ((U16)IfSet.pPoe->IP[2] << 8 & 0xFF00) | ((U16)IfSet.pPoe->IP[3] & 0x00FF);
		eep[5] = ((U16)IfSet.pPoe->NM[0] << 8 & 0xFF00) | ((U16)IfSet.pPoe->NM[1] & 0x00FF);
		eep[6] = ((U16)IfSet.pPoe->NM[2] << 8 & 0xFF00) | ((U16)IfSet.pPoe->NM[3] & 0x00FF);
		eep[7] = ((U16)IfSet.pPoe->GW[0] << 8 & 0xFF00) | ((U16)IfSet.pPoe->GW[1] & 0x00FF);
		eep[8] = ((U16)IfSet.pPoe->GW[2] << 8 & 0xFF00) | ((U16)IfSet.pPoe->GW[3] & 0x00FF);
		
		EEPROM_Update( eep, SYSTEM_EEP_POE_MC_0_1, 9 );
		POE_ReInit();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Interface_Config_Update_Relay( void )
{
	U16		eep;
	
	if( IfSet.pRly->TRB.Opt.Word != IfSet.sRly.TRB.Opt.Word ){
		IfSet.pRly->TRB.Opt.Word  = IfSet.sRly.TRB.Opt.Word;
		IfSet.pRly->TRB.Set = false;
		IfSet.pRly->TRB.Out = true;
		EEPROM_Update( &IfSet.pRly->TRB.Opt.Word, SYSTEM_EEP_RELAY_OPTION_TRB, 1 );
	}
	if( IfSet.pRly->AL1.Opt.Word != IfSet.sRly.AL1.Opt.Word ){
		IfSet.pRly->AL1.Opt.Word  = IfSet.sRly.AL1.Opt.Word;
		IfSet.pRly->AL1.Set = false;
		IfSet.pRly->AL1.Out = true;
		EEPROM_Update( &IfSet.pRly->AL1.Opt.Word, SYSTEM_EEP_RELAY_OPTION_AL1, 1 );
	}
	if( IfSet.pRly->AL2.Opt.Word != IfSet.sRly.AL2.Opt.Word ){
		IfSet.pRly->AL2.Opt.Word  = IfSet.sRly.AL2.Opt.Word;
		IfSet.pRly->AL2.Set = false;
		IfSet.pRly->AL2.Out = true;
		EEPROM_Update( &IfSet.pRly->AL2.Opt.Word, SYSTEM_EEP_RELAY_OPTION_AL2, 1 );
	}
	
	if( IfSet.pRly->TRB.Blk.Item.Act_Time != IfSet.sRly.TRB.Blk.Item.Act_Time ||
		IfSet.pRly->TRB.Blk.Item.Rel_Time != IfSet.sRly.TRB.Blk.Item.Rel_Time ){
		IfSet.pRly->TRB.Blk.Item.Act_Time  = IfSet.sRly.TRB.Blk.Item.Act_Time;	
		IfSet.pRly->TRB.Blk.Item.Rel_Time  = IfSet.sRly.TRB.Blk.Item.Rel_Time;
		eep = ((U16)IfSet.pRly->TRB.Blk.Item.Act_Time << 8 & 0xFF00) | ((U16)IfSet.pRly->TRB.Blk.Item.Rel_Time & 0x00FF);
		EEPROM_Update( &eep, SYSTEM_EEP_RELAY_BLINK_TIME_TRB, 1 );
	}
	
	if( IfSet.pRly->AL1.Blk.Item.Act_Time != IfSet.sRly.AL1.Blk.Item.Act_Time ||
		IfSet.pRly->AL1.Blk.Item.Rel_Time != IfSet.sRly.AL1.Blk.Item.Rel_Time ){
		IfSet.pRly->AL1.Blk.Item.Act_Time  = IfSet.sRly.AL1.Blk.Item.Act_Time;	
		IfSet.pRly->AL1.Blk.Item.Rel_Time  = IfSet.sRly.AL1.Blk.Item.Rel_Time;
		eep = ((U16)IfSet.pRly->AL1.Blk.Item.Act_Time << 8 & 0xFF00) | ((U16)IfSet.pRly->AL1.Blk.Item.Rel_Time & 0x00FF);
		EEPROM_Update( &eep, SYSTEM_EEP_RELAY_BLINK_TIME_AL1, 1 );
	}
	
	if( IfSet.pRly->AL2.Blk.Item.Act_Time != IfSet.sRly.AL2.Blk.Item.Act_Time ||
		IfSet.pRly->AL2.Blk.Item.Rel_Time != IfSet.sRly.AL2.Blk.Item.Rel_Time ){
		IfSet.pRly->AL2.Blk.Item.Act_Time  = IfSet.sRly.AL2.Blk.Item.Act_Time;	
		IfSet.pRly->AL2.Blk.Item.Rel_Time  = IfSet.sRly.AL2.Blk.Item.Rel_Time;
		eep = ((U16)IfSet.pRly->AL2.Blk.Item.Act_Time << 8 & 0xFF00) | ((U16)IfSet.pRly->AL2.Blk.Item.Rel_Time & 0x00FF);
		EEPROM_Update( &eep, SYSTEM_EEP_RELAY_BLINK_TIME_AL2, 1 );
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Interface_Config_Update_485( void )
{
	U16		eep[3];
	
	if( (IfSet.p485->UnitID	!= IfSet.s485.UnitID) ||
		(IfSet.p485->Baud	!= IfSet.s485.Baud	) ||
		(IfSet.p485->Type	!= IfSet.s485.Type	) )
	{
		eep[0] = IfSet.p485->UnitID	= IfSet.s485.UnitID;
		eep[1] = IfSet.p485->Baud	= IfSet.s485.Baud;
		eep[2] = IfSet.p485->Type	= IfSet.s485.Type;
		
		EEPROM_Update( eep, SYSTEM_EEP_MODBUS_UNIT_ID, 3 );
		RS485_ReInit();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Interface_Config_Update_mA_CalVal( void )
{
	U16		eep[2];
	U8		i;
	
	for( i=0; i<MEASURING_GAS_OUTPUT_MAX; i++ ){
		
		if( (IfSet.pmA[i]->Inf.Zero != IfSet.smA[i].Inf.Zero) ||
			(IfSet.pmA[i]->Inf.Span != IfSet.smA[i].Inf.Span) ){
			
			IfSet.pmA[i]->Inf.Zero = IfSet.smA[i].Inf.Zero;
			IfSet.pmA[i]->Inf.Span = IfSet.smA[i].Inf.Span;
			
			eep[0] = IfSet.pmA[i]->Inf.Zero;
			eep[1] = IfSet.pmA[i]->Inf.Span;
			
			if( i == 0 ){ EEPROM_Update( eep, SYSTEM_EEP_mA_OUTPUT_CAL_ZERO_uA_CH1, 2 ); }
			else		{ EEPROM_Update( eep, SYSTEM_EEP_mA_OUTPUT_CAL_ZERO_uA_CH2, 2 ); }
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Interface_Config_Update_mA_Offset( void )
{
	U16		eep;
	U8		i;
	
	for( i=0; i<MEASURING_GAS_OUTPUT_MAX; i++ ){
		
		if( IfSet.pmA[i]->Inf.Offs != IfSet.smA[i].Inf.Offs ){
			IfSet.pmA[i]->Inf.Offs  = IfSet.smA[i].Inf.Offs;
			
			eep = IfSet.pmA[i]->Inf.Offs;
			
			if( i == 0 ){ EEPROM_Update( &eep, SYSTEM_EEP_mA_OUTPUT_CAL_OFFS_uA_CH1, 1 ); }
			else		{ EEPROM_Update( &eep, SYSTEM_EEP_mA_OUTPUT_CAL_OFFS_uA_CH2, 1 ); }
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void uA_Set( U8 ch, float uA )
{
	DAC8571_Write( ch, uAmpere_To_Dac( uA ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void mA_Test_Handler( void )
{
	U8				i;
	
	for( i=0; i<MEASURING_GAS_OUTPUT_MAX; i++ ){
		
		if( IfSet.smA[i].Out != IfSet.smA[i].Set ){
			IfSet.smA[i].Out  = IfSet.smA[i].Set;
			
			DAC8571_Write( i, uAmpere_To_Dac( uA_Calculation( &IfSet.smA[i] ) ) );
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void KeyProcess( Func_HandleType *pFunc )
{
	U8		i;
	
	switch( pFunc->Level ){
		case FUNC_LEVEL_1:
			IfSet.pMeas = &MeasHandle;
			IfSet.pPoe = &PoeHandle.Conf;
			IfSet.p485 = &RS485_Handle.Conf;
			IfSet.pmA[0] = &mA_Handle[0];
			IfSet.pmA[1] = &mA_Handle[1];
			IfSet.pRly = &RelayDrive;
			if( (SystemOption.Item.IsPoeMode == true) || (SystemOption.Item.IsUniMode == true) ){
				pFunc->Item.Lv2 = 0;
			}
			else {
				if( IfSet.pMeas->Mode.Item.Maintenance == true ){
					pFunc->Item.Lv2 = 2;
				}
				else {
					pFunc->Item.Lv2 = 1;
				}
			}
			break;
			
		case FUNC_LEVEL_2: // Interface Item Set
			switch( pFunc->KeyData ){
				case Function:
					switch( pFunc->Item.Lv2 ){
						case 0: IfSet.sPoe = *IfSet.pPoe; break;
						case 1: IfSet.sRly = *IfSet.pRly; break;
						case 2: IfSet.s485 = *IfSet.p485; break;
						case 3:
							for( i=0; i<IfSet.pMeas->Conf.OutMax; i++ ){
								IfSet.smA[i].Inf = IfSet.pmA[i]->Inf;
								IfSet.smA[i].Set = 4000;
								IfSet.smA[i].Out = 4000;
								uA_Set( i, 4000 );
							}
							break;
							
						case 4:
							for( i=0; i<IfSet.pMeas->Conf.OutMax; i++ ){
								IfSet.smA[i].Inf = IfSet.pmA[i]->Inf;
								IfSet.smA[i].Set = 4000;
								IfSet.smA[i].Out = 0;
							}
							break;
					}
					pFunc->Level = FUNC_LEVEL_3;
					pFunc->IsApproved = false;
					pFunc->Item.Lv3 = 0;
					pFunc->Item.Lv4 = 0;
					break;
					
				case Up:
					if( IfSet.pMeas->Mode.Item.Maintenance == false ){
						if( (SystemOption.Item.IsPoeMode == true) || (SystemOption.Item.IsUniMode == true) ){
							if( pFunc->Item.Lv2 > 0 ){ pFunc->Item.Lv2--; }
						}
						else {
							if( pFunc->Item.Lv2 > 1 ){ pFunc->Item.Lv2--; }
						}
					}
					break;
					
				case Down:
					if( IfSet.pMeas->Mode.Item.Maintenance == false ){
						if( SystemOption.Item.IsPoeMode == true ){
							if( pFunc->Item.Lv2 < 1 ){ pFunc->Item.Lv2++; }
						}
						else {
							if( pFunc->Item.Lv2 < 4 ){ pFunc->Item.Lv2++; }
						}
					}
					break;
					
				case Reset:
					MoveFuncTop();
					break;
			}
			break;
			
		case FUNC_LEVEL_3: // Item Parameter Set
			switch( pFunc->KeyData ){
				case Function:
					switch( pFunc->Item.Lv2 ){
						case 0: // PoE
							if( pFunc->Item.Lv3 < 18 ){
								if( pFunc->KeyType == Long ){
									pFunc->Item.Lv3 = 18;
								}
								else {
									pFunc->Item.Lv3++;
								}
							}
							else {
								if( pFunc->IsApproved == true ){
									Interface_Config_Update_POE();
								}
								pFunc->Level = FUNC_LEVEL_2;
							}
							break;
							
						case 1: // Relay
							if( pFunc->Item.Lv3 < 15 ){
								if( pFunc->KeyType == Long ){
									pFunc->Item.Lv3 = 15;
								}
								else {
									pFunc->Item.Lv3++;
								}
							}
							else {
								if( pFunc->IsApproved == true ){
									Interface_Config_Update_Relay();
								}
								pFunc->Level = FUNC_LEVEL_2;
							}
							break;
							
						case 2: // 485
							if( pFunc->Item.Lv3 < 3 ){
								pFunc->Item.Lv3++;
							}
							else {
								if( pFunc->IsApproved == true ){
									Interface_Config_Update_485();
								}
								pFunc->Level = FUNC_LEVEL_2;
							}
							break;
							
						case 3: // mA Calibration
							if( pFunc->Item.Lv3 < 18 ){
								if( pFunc->KeyType == Long ){
									pFunc->Item.Lv3 = 18;
								}
								else {
									if( (IfSet.pMeas->Conf.OutMax < 2) && (pFunc->Item.Lv3 == 8) ){
										pFunc->Item.Lv3 = 18;
									}
									else {
										pFunc->Item.Lv3++;
									}
								}
								switch( pFunc->Item.Lv3 ){
									case  3: uA_Set( 0, 20000 );	break;
									case  6: IfSet.smA[0].Out = 0;	break;
									case  9: uA_Set( 1,  4000 );	break;
									case 12: uA_Set( 1, 20000 );	break;
									case 15: IfSet.smA[1].Out = 0;	break;
								} 
							}
							else {
								if( pFunc->IsApproved == true ){
									Interface_Config_Update_mA_CalVal();
								}
								IfSet.pmA[0]->Out = 0;
								IfSet.pmA[1]->Out = 0;
								pFunc->Level = FUNC_LEVEL_2;
							}
							break;
							
						case 4: // mA Zero Offset
							if( pFunc->Item.Lv3 < 6 ){
								if( (IfSet.pMeas->Conf.OutMax < 2) && (pFunc->Item.Lv3 == 2) ){
									pFunc->Item.Lv3 = 6;
								}
								else {
									pFunc->Item.Lv3++;
								}
								if( pFunc->Item.Lv3 == 3 ){
									IfSet.smA[1].Out = 0;
								}
							}
							else {
								if( pFunc->IsApproved == true ){
									Interface_Config_Update_mA_Offset();
								}
								IfSet.pmA[0]->Out = 0;
								IfSet.pmA[1]->Out = 0;
								pFunc->Level = FUNC_LEVEL_2;
							}
							break;
					}
					break;
					
				case Up:
					switch( pFunc->Item.Lv2 ){
						case 0: // PoE
							switch( pFunc->Item.Lv3 ){
								case  0: IfSet.sPoe.MC[0]++; break;
								case  1: IfSet.sPoe.MC[1]++; break;
								case  2: IfSet.sPoe.MC[2]++; break;
								case  3: IfSet.sPoe.MC[3]++; break;
								case  4: IfSet.sPoe.MC[4]++; break;
								case  5: IfSet.sPoe.MC[5]++; break;
								case  6: IfSet.sPoe.IP[0]++; break;
								case  7: IfSet.sPoe.IP[1]++; break;
								case  8: IfSet.sPoe.IP[2]++; break;
								case  9: IfSet.sPoe.IP[3]++; break;
								case 10: IfSet.sPoe.NM[0]++; break;
								case 11: IfSet.sPoe.NM[1]++; break;
								case 12: IfSet.sPoe.NM[2]++; break;
								case 13: IfSet.sPoe.NM[3]++; break;
								case 14: IfSet.sPoe.GW[0]++; break;
								case 15: IfSet.sPoe.GW[1]++; break;
								case 16: IfSet.sPoe.GW[2]++; break;
								case 17: IfSet.sPoe.GW[3]++; break;
								case 18: pFunc->IsApproved ^= true; break;
							}
							break;
							
						case 1: // Relay
							switch( pFunc->Item.Lv3 ){
								case  0: IfSet.sRly.TRB.Opt.Item.IsOutputEnable ^= true; break;
								case  1: IfSet.sRly.TRB.Opt.Item.IsEnergizeMode ^= true; break;
								case  2: IfSet.sRly.TRB.Opt.Item.IsBlinkingMode ^= true; break;
								case  3: if( IfSet.sRly.TRB.Blk.Item.Act_Time < 60 ){ IfSet.sRly.TRB.Blk.Item.Act_Time++; } break;
								case  4: if( IfSet.sRly.TRB.Blk.Item.Rel_Time < 60 ){ IfSet.sRly.TRB.Blk.Item.Rel_Time++; } break;
								case  5: IfSet.sRly.AL1.Opt.Item.IsOutputEnable ^= true; break;
								case  6: IfSet.sRly.AL1.Opt.Item.IsEnergizeMode ^= true; break;
								case  7: IfSet.sRly.AL1.Opt.Item.IsBlinkingMode ^= true; break;
								case  8: if( IfSet.sRly.AL1.Blk.Item.Act_Time < 60 ){ IfSet.sRly.AL1.Blk.Item.Act_Time++; } break;
								case  9: if( IfSet.sRly.AL1.Blk.Item.Rel_Time < 60 ){ IfSet.sRly.AL1.Blk.Item.Rel_Time++; } break;
								case 10: IfSet.sRly.AL2.Opt.Item.IsOutputEnable ^= true; break;
								case 11: IfSet.sRly.AL2.Opt.Item.IsEnergizeMode ^= true; break;
								case 12: IfSet.sRly.AL2.Opt.Item.IsBlinkingMode ^= true; break;
								case 13: if( IfSet.sRly.AL2.Blk.Item.Act_Time < 60 ){ IfSet.sRly.AL2.Blk.Item.Act_Time++; } break;
								case 14: if( IfSet.sRly.AL2.Blk.Item.Rel_Time < 60 ){ IfSet.sRly.AL2.Blk.Item.Rel_Time++; } break;
								case 15: pFunc->IsApproved ^= true; break;
							}
							break;
							
						case 2: // 485
							switch( pFunc->Item.Lv3 ){
								case 0:
									if( IfSet.s485.UnitID < 247 ){
										IfSet.s485.UnitID++;
									}
									else {
										IfSet.s485.UnitID = 1;
									}
									break;
									
								case 1: if( IfSet.s485.Baud < 6 ){ IfSet.s485.Baud++; } break;
								case 2: if( IfSet.s485.Type < 5 ){ IfSet.s485.Type++; } break;
								case 3: pFunc->IsApproved ^= true; break;
							}
							break;
							
						case 3: // mA Calibration
							switch( pFunc->Item.Lv3 ){
								case  0: Func_uInc(  5000,  100, &IfSet.smA[0].Inf.Zero	); break;
								case  1: Func_uInc(  5000,   10, &IfSet.smA[0].Inf.Zero	); break;
								case  2: Func_uInc(  5000,    1, &IfSet.smA[0].Inf.Zero	); break;
								case  3: Func_uInc( 21000,  100, &IfSet.smA[0].Inf.Span	); break;
								case  4: Func_uInc( 21000,   10, &IfSet.smA[0].Inf.Span	); break;
								case  5: Func_uInc( 21000,    1, &IfSet.smA[0].Inf.Span	); break;
								case  6: Func_fSum( 22000, 1000, &IfSet.smA[0].Set		); break;
								case  7: Func_fSum( 22000,  100, &IfSet.smA[0].Set		); break;
								case  8: Func_fSum( 22000,   10, &IfSet.smA[0].Set		); break;
								case  9: Func_uInc(  5000,  100, &IfSet.smA[1].Inf.Zero	); break;
								case 10: Func_uInc(  5000,   10, &IfSet.smA[1].Inf.Zero	); break;
								case 11: Func_uInc(  5000,    1, &IfSet.smA[1].Inf.Zero	); break;
								case 12: Func_uInc( 21000,  100, &IfSet.smA[1].Inf.Span	); break;
								case 13: Func_uInc( 21000,   10, &IfSet.smA[1].Inf.Span	); break;
								case 14: Func_uInc( 21000,    1, &IfSet.smA[1].Inf.Span	); break;
								case 15: Func_fSum( 22000, 1000, &IfSet.smA[1].Set		); break;
								case 16: Func_fSum( 22000,  100, &IfSet.smA[1].Set		); break;
								case 17: Func_fSum( 22000,   10, &IfSet.smA[1].Set		); break;
								case 18: pFunc->IsApproved ^= true; break;
							}
							break;
							
						case 4: // mA Zero Offset
							switch( pFunc->Item.Lv3 ){
								case 0: Func_iSum( 2000, 100, &IfSet.smA[0].Inf.Offs ); IfSet.smA[0].Out = 0; break;
								case 1: Func_iSum( 2000,  10, &IfSet.smA[0].Inf.Offs ); IfSet.smA[0].Out = 0; break;
								case 2: Func_iSum( 2000,   1, &IfSet.smA[0].Inf.Offs ); IfSet.smA[0].Out = 0; break;
								case 3: Func_iSum( 2000, 100, &IfSet.smA[1].Inf.Offs ); IfSet.smA[1].Out = 0; break;
								case 4: Func_iSum( 2000,  10, &IfSet.smA[1].Inf.Offs ); IfSet.smA[1].Out = 0; break;
								case 5: Func_iSum( 2000,   1, &IfSet.smA[1].Inf.Offs ); IfSet.smA[1].Out = 0; break;
								case 6: pFunc->IsApproved ^= true; break;
							}
							break;
					}
					break;
					
				case Down:
					switch( pFunc->Item.Lv2 ){
						case 0: // PoE
							switch( pFunc->Item.Lv3 ){
								case  0: IfSet.sPoe.MC[0]--; break;
								case  1: IfSet.sPoe.MC[1]--; break;
								case  2: IfSet.sPoe.MC[2]--; break;
								case  3: IfSet.sPoe.MC[3]--; break;
								case  4: IfSet.sPoe.MC[4]--; break;
								case  5: IfSet.sPoe.MC[5]--; break;
								case  6: IfSet.sPoe.IP[0]--; break;
								case  7: IfSet.sPoe.IP[1]--; break;
								case  8: IfSet.sPoe.IP[2]--; break;
								case  9: IfSet.sPoe.IP[3]--; break;
								case 10: IfSet.sPoe.NM[0]--; break;
								case 11: IfSet.sPoe.NM[1]--; break;
								case 12: IfSet.sPoe.NM[2]--; break;
								case 13: IfSet.sPoe.NM[3]--; break;
								case 14: IfSet.sPoe.GW[0]--; break;
								case 15: IfSet.sPoe.GW[1]--; break;
								case 16: IfSet.sPoe.GW[2]--; break;
								case 17: IfSet.sPoe.GW[3]--; break;
								case 18: pFunc->IsApproved ^= true; break;
							}
							break;
							
						case 1: // Relay
							switch( pFunc->Item.Lv3 ){
								case  0: IfSet.sRly.TRB.Opt.Item.IsOutputEnable ^= true; break;
								case  1: IfSet.sRly.TRB.Opt.Item.IsEnergizeMode ^= true; break;
								case  2: IfSet.sRly.TRB.Opt.Item.IsBlinkingMode ^= true; break;
								case  3: if( IfSet.sRly.TRB.Blk.Item.Act_Time > 1 ){ IfSet.sRly.TRB.Blk.Item.Act_Time--; } break;
								case  4: if( IfSet.sRly.TRB.Blk.Item.Rel_Time > 1 ){ IfSet.sRly.TRB.Blk.Item.Rel_Time--; } break;
								case  5: IfSet.sRly.AL1.Opt.Item.IsOutputEnable ^= true; break;
								case  6: IfSet.sRly.AL1.Opt.Item.IsEnergizeMode ^= true; break;
								case  7: IfSet.sRly.AL1.Opt.Item.IsBlinkingMode ^= true; break;
								case  8: if( IfSet.sRly.AL1.Blk.Item.Act_Time > 1 ){ IfSet.sRly.AL1.Blk.Item.Act_Time--; } break;
								case  9: if( IfSet.sRly.AL1.Blk.Item.Rel_Time > 1 ){ IfSet.sRly.AL1.Blk.Item.Rel_Time--; } break;
								case 10: IfSet.sRly.AL2.Opt.Item.IsOutputEnable ^= true; break;
								case 11: IfSet.sRly.AL2.Opt.Item.IsEnergizeMode ^= true; break;
								case 12: IfSet.sRly.AL2.Opt.Item.IsBlinkingMode ^= true; break;
								case 13: if( IfSet.sRly.AL2.Blk.Item.Act_Time > 1 ){ IfSet.sRly.AL2.Blk.Item.Act_Time--; } break;
								case 14: if( IfSet.sRly.AL2.Blk.Item.Rel_Time > 1 ){ IfSet.sRly.AL2.Blk.Item.Rel_Time--; } break;
								case 15: pFunc->IsApproved ^= true; break;
							}
							break;
							
						case 2: // 485
							switch( pFunc->Item.Lv3 ){
								case 0:
									if( IfSet.s485.UnitID > 1 ){
										IfSet.s485.UnitID--;
									}
									else {
										IfSet.s485.UnitID = 247;
									}
									break;
									
								case 1: if( IfSet.s485.Baud ){ IfSet.s485.Baud--; } break;
								case 2: if( IfSet.s485.Type ){ IfSet.s485.Type--; } break;
								case 3: pFunc->IsApproved ^= true; break;
							}
							break;
							
						case 3: // mA Calibration
							switch( pFunc->Item.Lv3 ){
								case  0: Func_uDec(  3000,   100, &IfSet.smA[0].Inf.Zero	); break;
								case  1: Func_uDec(  3000,    10, &IfSet.smA[0].Inf.Zero	); break;
								case  2: Func_uDec(  3000,     1, &IfSet.smA[0].Inf.Zero	); break;
								case  3: Func_uDec( 19000,   100, &IfSet.smA[0].Inf.Span	); break;
								case  4: Func_uDec( 19000,    10, &IfSet.smA[0].Inf.Span	); break;
								case  5: Func_uDec( 19000,     1, &IfSet.smA[0].Inf.Span	); break;
								case  6: Func_fSum(   500, -1000, &IfSet.smA[0].Set			); break;
								case  7: Func_fSum(   500,  -100, &IfSet.smA[0].Set			); break;
								case  8: Func_fSum(   500,   -10, &IfSet.smA[0].Set			); break;
								case  9: Func_uDec(  3000,   100, &IfSet.smA[1].Inf.Zero	); break;
								case 10: Func_uDec(  3000,    10, &IfSet.smA[1].Inf.Zero	); break;
								case 11: Func_uDec(  3000,     1, &IfSet.smA[1].Inf.Zero	); break;
								case 12: Func_uDec( 19000,   100, &IfSet.smA[1].Inf.Span	); break;
								case 13: Func_uDec( 19000,    10, &IfSet.smA[1].Inf.Span	); break;
								case 14: Func_uDec( 19000,     1, &IfSet.smA[1].Inf.Span	); break;
								case 15: Func_fSum(   500, -1000, &IfSet.smA[1].Set			); break;
								case 16: Func_fSum(   500,  -100, &IfSet.smA[1].Set			); break;
								case 17: Func_fSum(   500,   -10, &IfSet.smA[1].Set			); break;
								case 18: pFunc->IsApproved ^= true; break;
							}
							break;
							
						case 4: // mA Zero Offset
							switch( pFunc->Item.Lv3 ){
								case 0: Func_iSum( -2000, -100, &IfSet.smA[0].Inf.Offs ); IfSet.smA[0].Out = 0; break;
								case 1: Func_iSum( -2000,  -10, &IfSet.smA[0].Inf.Offs ); IfSet.smA[0].Out = 0; break;
								case 2: Func_iSum( -2000,   -1, &IfSet.smA[0].Inf.Offs ); IfSet.smA[0].Out = 0; break;
								case 3: Func_iSum( -2000, -100, &IfSet.smA[1].Inf.Offs ); IfSet.smA[1].Out = 0; break;
								case 4: Func_iSum( -2000,  -10, &IfSet.smA[1].Inf.Offs ); IfSet.smA[1].Out = 0; break;
								case 5: Func_iSum( -2000,   -1, &IfSet.smA[1].Inf.Offs ); IfSet.smA[1].Out = 0; break;
								case 6: pFunc->IsApproved ^= true; break;
							}
							break;
					}
					break;
					
				case Reset:
					switch( pFunc->Item.Lv2 ){
						case 0: // PoE
						case 1: // Relay
						case 2: // 485
							if( pFunc->Item.Lv3 ){
								if( pFunc->KeyType == Long ){
									pFunc->Item.Lv3 = 0;
								}
								else {
									pFunc->Item.Lv3--;
								}
							}
							else {
								pFunc->Level = FUNC_LEVEL_2;
							}
							break;
							
						case 3: // mA Calibration
							if( pFunc->Item.Lv3 ){
								if( pFunc->KeyType == Long ){
									pFunc->Item.Lv3 = 0;
								}
								else {
									if( (IfSet.pMeas->Conf.OutMax < 2) && (pFunc->Item.Lv3 == 18) ){
										pFunc->Item.Lv3 = 8;
									}
									else {
										pFunc->Item.Lv3--;
									}
								}
								switch( pFunc->Item.Lv3 ){
									case  2: uA_Set( 0,  4000 );	break;
									case  5: uA_Set( 0, 20000 );	break;
									case  8: IfSet.smA[0].Out = 0;	break;
									case 11: uA_Set( 1,  4000 );	break;
									case 14: uA_Set( 1, 20000 );	break;
									case 17: IfSet.smA[1].Out = 0;	break;
								}
							}
							else {
								IfSet.pmA[0]->Out = 0;
								IfSet.pmA[1]->Out = 0;
								pFunc->Level = FUNC_LEVEL_2;
							}
							break;
							
						case 4: // mA Zero Offset
							if( pFunc->Item.Lv3 ){
								if( (IfSet.pMeas->Conf.OutMax < 2) && (pFunc->Item.Lv3 == 6) ){
									pFunc->Item.Lv3 = 2;
								}
								else {
									pFunc->Item.Lv3--;
								}
								switch( pFunc->Item.Lv3 ){
									case 2: IfSet.smA[0].Out = 0; break;
									case 5: IfSet.smA[1].Out = 0; break;
								}
							}
							else {
								IfSet.pmA[0]->Out = 0;
								IfSet.pmA[1]->Out = 0;
								pFunc->Level = FUNC_LEVEL_2;
							}
							break;
					}
					break;
			}
			break;
	}
}


