#define __FUNC_TEST_C__
    #include "func_test.h"
#undef  __FUNC_TEST_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "vfunc_test.h"
#include "option.h"
#include "buzzer.h"
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
static void IoSet( void );
static void KeyProcess( Func_HandleType *pFunc );
/* End Function */

/* Start Variable */
static Test_SetType		Tset;
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Func_Test( Func_HandleType *pFunc )
{
	KeyProcess( pFunc );
	
	if( pFunc->ViewEvt == true ){ vFunc_Test( pFunc, &Tset ); }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void IoSet( void )
{
	bool	AL1[2] = { false, false };
	bool	AL2[2] = { false, false };
	bool	OVR[2] = { false, false };
	bool	BuzAL1;
	bool	BuzAL2;
	U8		i;
	
	for( i=0; i<MeasHandle.Conf.OutMax; i++ ){
		
		// Alarm Scan
		if( Tset.pMeas->pGas[i]->Set == (S16)(Tset.pMeas->pGas[i]->Inf.HighScale * 1.1f) ){ OVR[i] = true; }
		
		if( Tset.pMeas->pAlr[i]->Inf._1stTypeIncrease == true ){
			if( Tset.pMeas->pGas[i]->Set >= Tset.pMeas->pAlr[i]->Inf._1stLevel ){ AL1[i] = true; }
		}
		else {
			if( Tset.pMeas->pGas[i]->Set <= Tset.pMeas->pAlr[i]->Inf._1stLevel ){ AL1[i] = true; }
		}
		if( Tset.pMeas->pAlr[i]->Inf._2ndTypeIncrease == true ){
			if( Tset.pMeas->pGas[i]->Set >= Tset.pMeas->pAlr[i]->Inf._2ndLevel ){ AL2[i] = true; }
		}
		else {
			if( Tset.pMeas->pGas[i]->Set <= Tset.pMeas->pAlr[i]->Inf._2ndLevel ){ AL2[i] = true; }
		}
		
		// mA Set
		if( ((SystemOption.Item.IsPoeMode == false) || (SystemOption.Item.IsUniMode == true)) && (Tset.With_mA == true) ){
			if( Tset.Trouble[i] == true ){
				Tset.pmA[i]->Set = 0.f;
			}
			else if( OVR[i] == true ){
				Tset.pmA[i]->Set = 22000.f;
			}
			else {
				Tset.pmA[i]->Set = 16000.f * ((float)Tset.pMeas->pGas[i]->Set / (float)Tset.pMeas->pGas[i]->Inf.HighScale) + 4000.f;
			}
		}
		
		// Alarm Item Update
		Tset.pMeas->pAlr[i]->Item.Bit.aOvr = OVR[i];
		Tset.pMeas->pAlr[i]->Item.Bit.a1st = AL1[i];
		Tset.pMeas->pAlr[i]->Item.Bit.a2nd = AL2[i];
		
		if( Tset.Trouble[i] == true ){
			SystemError.Code[i] = 1; // System Peripheral Trouble
		}
		else {
			SystemError.Code[i] = 0;
		}
	}
	
	// Led Set
	Tset.pLed->Set.Bit.TRB = Tset.Trouble[0] | Tset.Trouble[1];
	
	switch( MeasHandle.Conf.OutMax ){
		case 1:
			BuzAL1 = AL1[0];
			BuzAL2 = AL2[0];
			break;
			
		case 2:
			BuzAL1 = AL1[0] | AL2[0];
			BuzAL2 = AL1[1] | AL2[1];
			break;
	}
	
	if( (SystemOption.Item.BuzPresent == true) && (Tset.With_Buz == true) ){
		if( (BuzAL1 != Tset.pLed->Set.Bit.AL1) || (BuzAL2 != Tset.pLed->Set.Bit.AL2) ){
			if		( BuzAL2 == true )	{ BuzDrive.Melody.AL2(); }
			else if	( BuzAL1 == true )	{ BuzDrive.Melody.AL1(); }
			else						{ Buzzer_Stop(); }
		}
	}
	
	Tset.pLed->Set.Bit.AL1 = BuzAL1;
	Tset.pLed->Set.Bit.AL2 = BuzAL2;
		
	// Relay Set
	if( Tset.With_Relay == true ){
		Tset.pRelay->TRB.Set = Tset.pLed->Set.Bit.TRB;
		Tset.pRelay->AL1.Set = Tset.pLed->Set.Bit.AL1;
		Tset.pRelay->AL2.Set = Tset.pLed->Set.Bit.AL2;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void KeyProcess( Func_HandleType *pFunc )
{
	S16		SetGas;
	S16		MaxGas;
	
	switch( pFunc->Level ){
		case FUNC_LEVEL_1:
			Tset.pMeas = &MeasHandle;
			Tset.pmA[0] = &mA_Handle[0];
			Tset.pmA[1] = &mA_Handle[1];
			Tset.pRelay = &RelayDrive;
			Tset.pLed = &LedDrive;
			Tset.pFlow = &FlowCtrl;
			if( (Tset.pMeas->Mode.Item.Maintenance == true) || (SystemOption.Item.IsBatMode == true) ){
				pFunc->Item.Lv2 = 2;
			}
			else {
				if( SystemOption.Item.IsPoeMode == true ){
					pFunc->Item.Lv2 = 1;
				}
				else {
					pFunc->Item.Lv2 = 0;
				}
			}
			pFunc->State.Bit.Test = true;
			break;
			
		case FUNC_LEVEL_2:
			switch( pFunc->KeyData ){
				case Function:
					switch( pFunc->Item.Lv2 ){
						case 0: // mA Output Test
							Tset.pmA[0]->Set = 4000;
							Tset.pmA[1]->Set = 4000;
							pFunc->Item.Lv3 = 0;
							break;
							
						case 1: // Relay Test
							Tset.pRelay->AL1.Set = false;
							Tset.pRelay->AL2.Set = false;
							Tset.pRelay->TRB.Set = false;
							Tset.pRelay->IsTestMode = true;
							pFunc->Item.Lv3 = 0;
							break;
							
						case 2: // Display Test
							Tset.TestColor = 0;
							pFunc->Item.Lv3 = 0;
							break;
							
						case 3: // Gas Simulation
							Tset.With_mA = false;
							Tset.With_Relay = false;
							Tset.With_Buz = false;
							Tset.Trouble[0] = false;
							Tset.Trouble[1] = false;
							if( SystemOption.Item.IsBatMode == true ){
								pFunc->Item.Lv3 = 2;
							}
							else if( SystemOption.Item.IsPoeMode == true ){
								pFunc->Item.Lv3 = 1;
							}
							else {
								pFunc->Item.Lv3 = 0;
							}
							break;
							
						case 4: // Maximum Power Test
							Tset.pmA[0]->Set = 0;
							Tset.pmA[1]->Set = 0;
							Tset.pRelay->AL1.Set = false;
							Tset.pRelay->AL2.Set = false;
							Tset.pRelay->TRB.Set = false;
							Tset.pRelay->IsTestMode = true;
							Tset.pLed->Set.Bit.AL1 = false;
							Tset.pLed->Set.Bit.AL2 = false;
							Tset.pLed->Set.Bit.TRB = false;
							Tset.pFlow->pPump->IsEnabled = false;
							Pump_Off();
							Tset.With_mA = false;
							Tset.With_Relay = false;
							Tset.With_Buz = false;
							Tset.With_Led = false;
							Tset.With_Pump = false;
							if( SystemOption.Item.IsBatMode == true ){
								pFunc->Item.Lv3 = 2;
							}
							else if( SystemOption.Item.IsPoeMode == true ){
								pFunc->Item.Lv3 = 1;
							}
							else {
								pFunc->Item.Lv3 = 0;
							}
							break;
					}
					pFunc->Item.Lv4 = 0;
					pFunc->IsApproved = false;
					pFunc->Level = FUNC_LEVEL_3;
					break;
					
				case Up:
					if( Tset.pMeas->Mode.Item.Maintenance == false ){
						if( SystemOption.Item.IsBatMode == true ){
							if( pFunc->Item.Lv2 > 2 ){
								pFunc->Item.Lv2--;
							}
						}
						else if( SystemOption.Item.IsPoeMode == true ){
							if( pFunc->Item.Lv2 > 1 ){
								pFunc->Item.Lv2--;
							}
						}
						else { 
							if( pFunc->Item.Lv2 > 0 ){
								pFunc->Item.Lv2--;
							}
						}
					}
					break;
					
				case Down:
					if( Tset.pMeas->Mode.Item.Maintenance == false ){
						if( pFunc->Item.Lv2 < 4 ){
							pFunc->Item.Lv2++;
						}
					}
					break;
					
				case Reset:
					pFunc->State.Bit.Test = false;
					MoveFuncTop();
					break;
			}
			break;
			
		case FUNC_LEVEL_3: // Item Parameter Set
			switch( pFunc->KeyData ){
				case Function:
					switch( pFunc->Item.Lv2 ){
						case 0: // mA Output Test
							if( pFunc->Item.Lv3 < 8 ){
								if( (Tset.pMeas->Conf.OutMax < 2) && (pFunc->Item.Lv3 == 3) ){
									pFunc->Item.Lv3 = 8;
								}
								else {
									pFunc->Item.Lv3++;
								}
							}
							else {
								Tset.pmA[0]->Set = pFunc->mA[0];
								Tset.pmA[1]->Set = pFunc->mA[1];
								pFunc->Level = FUNC_LEVEL_2;
							}
							break;
							
						case 1: // Relay Test
							switch( pFunc->Item.Lv3 ){
								case 0: Tset.pRelay->TRB.Set ^= true; break;
								case 1: Tset.pRelay->AL1.Set ^= true; break;
								case 2: Tset.pRelay->AL2.Set ^= true; break;
								case 3:
									Tset.pRelay->AL1.Set = false;
									Tset.pRelay->AL2.Set = false;
									Tset.pRelay->TRB.Set = false;
									Tset.pRelay->AL1.Out = true;
									Tset.pRelay->AL2.Out = true;
									Tset.pRelay->TRB.Out = true;
									Tset.pRelay->IsTestMode = false;
									pFunc->Level = FUNC_LEVEL_2;
									break;
							}
							break;
							
						case 2: // Display Test
							if( Tset.TestColor < 3 ){
								Tset.TestColor++;
							}
							else {
								Tset.TestColor = 0;
							}
							break;
							
						case 3: // Gas Simulation
							switch( pFunc->Item.Lv3 ){
								case 0: Tset.With_mA	^= true; break;
								case 1: Tset.With_Relay	^= true; break;
								case 2: Tset.With_Buz	^= true; break;
								case 3:
									MeasHandle.Test.Item.GasSimulation = true;
									Tset.pMeas->pGas[0]->Set = 0;
									Tset.pMeas->pGas[1]->Set = 0;
									IoSet();
									pFunc->pMsgBox->ClearMsg = true;
									pFunc->Level = FUNC_LEVEL_4;
									break;
							}
							break;
							
						case 4: // Maximum Power Test
							switch( pFunc->Item.Lv3 ){
								case 0:
									Tset.With_mA ^= true;
									if( Tset.With_mA == true ){
										Tset.pmA[0]->Set = 22000;
										Tset.pmA[1]->Set = 22000;
									}
									else {
										Tset.pmA[0]->Set = 0;
										Tset.pmA[1]->Set = 0;
									}
									break;
									
								case 1:
									Tset.With_Relay	^= true;
									Tset.pRelay->AL1.Set = Tset.With_Relay;
									Tset.pRelay->AL2.Set = Tset.With_Relay;
									Tset.pRelay->TRB.Set = Tset.With_Relay;
									break;
									
								case 2:
									Tset.With_Buz ^= true;
									if( Tset.With_Buz == true ){
										BuzDrive.Melody.Max();
									}
									else {
										Buzzer_Stop();
									}
									break;
									
								case 3:
									Tset.With_Led ^= true;
									Tset.pLed->Set.Bit.AL1 = Tset.With_Led;
									Tset.pLed->Set.Bit.AL2 = Tset.With_Led;
									Tset.pLed->Set.Bit.TRB = Tset.With_Led;
									break;
									
								case 4:
									Tset.With_Pump ^= true;
									if( Tset.With_Pump == true ){
										Tset.pFlow->pPump->SetDuty = PUMP_CTRL_DUTY_MAX;
										Pump_On();
									}
									else {
										Pump_Off();
									}
									break;
									
								case 5:
									Tset.pmA[0]->Set = pFunc->mA[0];
									Tset.pmA[1]->Set = pFunc->mA[1];
									Tset.pRelay->AL1.Set = false;
									Tset.pRelay->AL2.Set = false;
									Tset.pRelay->TRB.Set = false;
									Tset.pRelay->AL1.Out = true;
									Tset.pRelay->AL2.Out = true;
									Tset.pRelay->TRB.Out = true;
									Tset.pRelay->IsTestMode = false;
									Tset.pLed->Set.Bit.AL1 = false;
									Tset.pLed->Set.Bit.AL2 = false;
									Tset.pLed->Set.Bit.TRB = false;
									Tset.pFlow->pPump->IsEnabled = true;
									Tset.pFlow->pPump->SetDuty = Tset.pFlow->Cal.SpanDuty;
									Pump_On();
									Buzzer_Stop();
									pFunc->Level = FUNC_LEVEL_2;
									break;
							}
							break;
					}
					break;
					
				case Up:
					switch( pFunc->Item.Lv2 ){
						case 0: // mA Output Test
							switch( pFunc->Item.Lv3 ){
								case 0: Func_fSum( 22000, 1000, &Tset.pmA[0]->Set ); break;
								case 1: Func_fSum( 22000,  100, &Tset.pmA[0]->Set ); break;
								case 2: Func_fSum( 22000,   10, &Tset.pmA[0]->Set ); break;
								case 3: Func_fSum( 22000,    1, &Tset.pmA[0]->Set ); break;
								case 4: Func_fSum( 22000, 1000, &Tset.pmA[1]->Set ); break;
								case 5: Func_fSum( 22000,  100, &Tset.pmA[1]->Set ); break;
								case 6: Func_fSum( 22000,   10, &Tset.pmA[1]->Set ); break;
								case 7: Func_fSum( 22000,    1, &Tset.pmA[1]->Set ); break;
							}
							break;
							
						case 1: // Relay Test
							if( pFunc->Item.Lv3 ){
								pFunc->Item.Lv3--;
							}
							break;
							
						case 3: // Gas Simulation
						case 4: // Maximum Power Test
							if( pFunc->Item.Lv3 == 3 ){
								if( SystemOption.Item.BuzPresent == true ){
									pFunc->Item.Lv3 = 2;
								}
								else {
									pFunc->Item.Lv3 = 1;
								}
							}
							else {
								if( SystemOption.Item.IsBatMode == true ){
									if( pFunc->Item.Lv3 > 2 ){ pFunc->Item.Lv3--; }
								}
								else if( SystemOption.Item.IsPoeMode == true ){
									if( pFunc->Item.Lv3 > 1 ){ pFunc->Item.Lv3--; }
								}
								else {
									if( pFunc->Item.Lv3 > 0 ){ pFunc->Item.Lv3--; }
								}
							}
							break;
					}
					break;
					
				case Down:
					switch( pFunc->Item.Lv2 ){
						case 0: // mA Output Test
							switch( pFunc->Item.Lv3 ){
								case 0: Func_fSum( 500, -1000, &Tset.pmA[0]->Set ); break;
								case 1: Func_fSum( 500,  -100, &Tset.pmA[0]->Set ); break;
								case 2: Func_fSum( 500,   -10, &Tset.pmA[0]->Set ); break;
								case 3: Func_fSum( 500,    -1, &Tset.pmA[0]->Set ); break;
								case 4: Func_fSum( 500, -1000, &Tset.pmA[1]->Set ); break;
								case 5: Func_fSum( 500,  -100, &Tset.pmA[1]->Set ); break;
								case 6: Func_fSum( 500,   -10, &Tset.pmA[1]->Set ); break;
								case 7: Func_fSum( 500,    -1, &Tset.pmA[1]->Set ); break;
							}
							break;
							
						case 1: // Relay Test
							if( pFunc->Item.Lv3 < 3 ){ pFunc->Item.Lv3++; }
							break;
							
						case 3: // Gas Simulation
							if( pFunc->Item.Lv3 == 1 ){
								if( SystemOption.Item.BuzPresent == true ){
									pFunc->Item.Lv3 = 2;
								}
								else {
									pFunc->Item.Lv3 = 3;
								}
							}
							else {
								if( pFunc->Item.Lv3 < 3 ){
									pFunc->Item.Lv3++;
								}
							}
							break;
							
						case 4: // Maximum Power Test
							if( pFunc->Item.Lv3 == 1 ){
								if( SystemOption.Item.BuzPresent == true ){
									pFunc->Item.Lv3 = 2;
								}
								else {
									pFunc->Item.Lv3 = 3;
								}
							}
							else {
								if( pFunc->Item.Lv3 < 5 ){
									pFunc->Item.Lv3++;
								}
							}
							break;
					}
					break;
					
				case Reset:
					switch( pFunc->Item.Lv2 ){
						case 0: // mA Output Test
							if( pFunc->Item.Lv3 ){
								if( (Tset.pMeas->Conf.OutMax < 2) && (pFunc->Item.Lv3 == 8) ){
									pFunc->Item.Lv3 = 3;
								}
								else {
									pFunc->Item.Lv3--;
								}
							}
							else {
								Tset.pmA[0]->Set = pFunc->mA[0];
								Tset.pmA[1]->Set = pFunc->mA[1];
								pFunc->Level = FUNC_LEVEL_2;
							}
							break;
							
						case 1: // Relay Test
							Tset.pRelay->AL1.Set = false;
							Tset.pRelay->AL2.Set = false;
							Tset.pRelay->TRB.Set = false;
							Tset.pRelay->AL1.Out = true;
							Tset.pRelay->AL2.Out = true;
							Tset.pRelay->TRB.Out = true;
							Tset.pRelay->IsTestMode = false;
							pFunc->Level = FUNC_LEVEL_2;
							break;
							
						case 2: // Display Test
							pFunc->pMsgBox->ClearTitle = true;
							pFunc->pMsgBox->ClearMsg = true;
							GDI_Clear( RGB.BLACK );
							pFunc->Level = FUNC_LEVEL_2;
							break;
							
						case 3: // Gas Simulation
							pFunc->Level = FUNC_LEVEL_2;
							break;
							
						case 4: // Maximum Power Test
							Tset.pmA[0]->Set = pFunc->mA[0];
							Tset.pmA[1]->Set = pFunc->mA[1];
							Tset.pRelay->AL1.Set = false;
							Tset.pRelay->AL2.Set = false;
							Tset.pRelay->TRB.Set = false;
							Tset.pRelay->AL1.Out = true;
							Tset.pRelay->AL2.Out = true;
							Tset.pRelay->TRB.Out = true;
							Tset.pRelay->IsTestMode = false;
							Tset.pLed->Set.Bit.AL1 = false;
							Tset.pLed->Set.Bit.AL2 = false;
							Tset.pLed->Set.Bit.TRB = false;
							Tset.pFlow->pPump->IsEnabled = true;
							Tset.pFlow->pPump->SetDuty = Tset.pFlow->Cal.SpanDuty;
							Pump_On();
							Buzzer_Stop();
							pFunc->Level = FUNC_LEVEL_2;
							break;
					}
					break;
			}
			break;
			
		case FUNC_LEVEL_4: // Item Parameter Set
			switch( pFunc->KeyData ){
				case Function:
					switch( pFunc->Item.Lv2 ){
						case 3: // Gas Simulation
							if( pFunc->Item.Lv4 < MeasHandle.Conf.OutMax ){
								pFunc->Item.Lv4++;
							}
							else {
								Tset.pmA[0]->Set = pFunc->mA[0];
								Tset.pmA[1]->Set = pFunc->mA[1];
								Tset.pRelay->AL1.Set = false;
								Tset.pRelay->AL2.Set = false;
								Tset.pRelay->TRB.Set = false;
								Tset.pLed->Set.Bit.AL1 = false;
								Tset.pLed->Set.Bit.AL2 = false;
								Tset.pLed->Set.Bit.TRB = false;
								Tset.pMeas->pAlr[0]->Item.Bit.aOvr = false;
								Tset.pMeas->pAlr[0]->Item.Bit.tUdr = false;
								Tset.pMeas->pAlr[0]->Item.Bit.a1st = false;
								Tset.pMeas->pAlr[0]->Item.Bit.a2nd = false;
								Tset.pMeas->pAlr[1]->Item.Bit.aOvr = false;
								Tset.pMeas->pAlr[1]->Item.Bit.tUdr = false;
								Tset.pMeas->pAlr[1]->Item.Bit.a1st = false;
								Tset.pMeas->pAlr[1]->Item.Bit.a2nd = false;
								MeasHandle.Test.Item.GasSimulation = false;
								Buzzer_Stop();
								pFunc->Level = FUNC_LEVEL_2;
							}
							break;
					}
					break;
					
				case Up:
					switch( pFunc->Item.Lv2 ){
						case 3: // Gas Simulation
							if( (pFunc->Item.Lv4 == 0) || ((pFunc->Item.Lv4 == 1) && (MeasHandle.Conf.OutMax > 1)) ){
								SetGas = (S16)(Tset.pMeas->pGas[pFunc->Item.Lv4]->Inf.HighScale * 0.1f);
								MaxGas = (S16)(Tset.pMeas->pGas[pFunc->Item.Lv4]->Inf.HighScale * 1.1f);
								if( Tset.pMeas->pGas[pFunc->Item.Lv4]->Set == MaxGas ){
									if( Tset.Trouble[pFunc->Item.Lv4] == false ){
										Tset.Trouble[pFunc->Item.Lv4] = true;
										IoSet();
									}
								}
								else {
									Func_iSum( MaxGas, SetGas, &Tset.pMeas->pGas[pFunc->Item.Lv4]->Set );
									IoSet();
								}
							}
							break;
					}
					break;
					
				case Down:
					switch( pFunc->Item.Lv2 ){
						case 3: // Gas Simulation
							if( (pFunc->Item.Lv4 == 0) || ((pFunc->Item.Lv4 == 1) && (MeasHandle.Conf.OutMax > 1)) ){
								SetGas = (S16)(Tset.pMeas->pGas[pFunc->Item.Lv4]->Inf.HighScale * 0.1f);
								if( Tset.pMeas->pGas[pFunc->Item.Lv4]->Set > 0 ){
									if( Tset.Trouble[pFunc->Item.Lv4] == true ){
										Tset.Trouble[pFunc->Item.Lv4] = false;
									}
									else {
										Func_iSum( 0, -SetGas, &Tset.pMeas->pGas[pFunc->Item.Lv4]->Set );
									}
									IoSet();
								}
							}
							break;
					}
					break;
					
				case Reset:
					if( pFunc->Item.Lv4 ){
						pFunc->Item.Lv4--;
					}
					else {
						switch( pFunc->Item.Lv2 ){
							case 3: // Gas Simulation      
								Tset.pmA[0]->Set = pFunc->mA[0];
								Tset.pmA[1]->Set = pFunc->mA[1];
								Tset.pRelay->AL1.Set = false;
								Tset.pRelay->AL2.Set = false;
								Tset.pRelay->TRB.Set = false;
								Tset.pLed->Set.Bit.AL1 = false;
								Tset.pLed->Set.Bit.AL2 = false;
								Tset.pLed->Set.Bit.TRB = false;
								Tset.pMeas->pAlr[0]->Item.Bit.aOvr = false;
								Tset.pMeas->pAlr[0]->Item.Bit.tUdr = false;
								Tset.pMeas->pAlr[0]->Item.Bit.a1st = false;
								Tset.pMeas->pAlr[0]->Item.Bit.a2nd = false;
								Tset.pMeas->pAlr[1]->Item.Bit.aOvr = false;
								Tset.pMeas->pAlr[1]->Item.Bit.tUdr = false;
								Tset.pMeas->pAlr[1]->Item.Bit.a1st = false;
								Tset.pMeas->pAlr[1]->Item.Bit.a2nd = false;
								MeasHandle.Test.Item.GasSimulation = false;
								Buzzer_Stop();
								pFunc->Level = FUNC_LEVEL_2;
								break;
						}
					}
					break;
			}
			break;
	}
}


