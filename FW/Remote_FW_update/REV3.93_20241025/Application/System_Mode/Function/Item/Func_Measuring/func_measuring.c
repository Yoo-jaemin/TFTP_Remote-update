#define __FUNC_MEASURING_C__
    #include "func_measuring.h"
#undef  __FUNC_MEASURING_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "vfunc_measuring.h"
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
static void Range_Update( void );
static void Alarm_Update( void );
static void Cross_Scale_Update( void );
static void Gas_Name_Update( U8 i );
static void KeyProcess( Func_HandleType *pFunc );
/* End Function */

/* Start Variable */
static Measuring_SetType		mSet;
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Func_Measuring( Func_HandleType *pFunc )
{
	KeyProcess( pFunc );
	if( pFunc->ViewEvt == true ){ vFunc_Measuring( pFunc, &mSet ); }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Range_Update( void )
{
	U16		eep[4];
	U8		i;
	float	roc;
	
	for( i=0; i<mSet.pMeas->Conf.OutMax; i++ ){
		
		if( mSet.pMeas->pGas[i]->Inf.Dp != mSet.sGas[i].Dp ){
			mSet.pMeas->pGas[i]->Inf.Dp  = mSet.sGas[i].Dp;
			
			SenEEP_Write_CHn( &mSet.pMeas->pGas[i]->Inf.Dp, mSet.pMeas->Conf.iOut[i], SENSOR_EEP_CHn_DP, 1 );
		}
		
		if( mSet.pMeas->pGas[i]->Inf.Unit != mSet.sGas[i].Unit ){
			mSet.pMeas->pGas[i]->Inf.Unit  = mSet.sGas[i].Unit;
			
			SenEEP_Write_CHn( &mSet.pMeas->pGas[i]->Inf.Unit, mSet.pMeas->Conf.iOut[i], SENSOR_EEP_CHn_UNIT, 1 );
		}
		
		if( mSet.pMeas->pGas[i]->Inf.HighScale != mSet.sGas[i].HighScale ){
			
			roc = (float)mSet.sGas[i].HighScale / (float)mSet.pMeas->pGas[i]->Inf.HighScale;
			
			mSet.pMeas->pGas[i]->Inf.HighScale = mSet.sGas[i].HighScale;
			mSet.pMeas->pGas[i]->Inf.SpanGas *= roc;
			mSet.pMeas->pAlr[i]->Inf._1stLevel *= roc;
			mSet.pMeas->pAlr[i]->Inf._2ndLevel *= roc;
				
			eep[0] = mSet.pMeas->pGas[i]->Inf.HighScale;
			eep[1] = mSet.pMeas->pGas[i]->Inf.SpanGas;
			eep[2] = mSet.pMeas->pAlr[i]->Inf._1stLevel;
			eep[3] = mSet.pMeas->pAlr[i]->Inf._2ndLevel;
			
			Measuring_Gas_MSD_Load( &mSet.pMeas->pGas[i]->Inf );
			
			SenEEP_Write_CHn( &eep[0], mSet.pMeas->Conf.iOut[i], SENSOR_EEP_CHn_HIGH_SCALE, 1 );
			SenEEP_Write_CHn( &eep[1], mSet.pMeas->Conf.iOut[i], SENSOR_EEP_CHn_SPAN_GAS, 1 );
			SenEEP_Write_CHn( &eep[2], mSet.pMeas->Conf.iOut[i], SENSOR_EEP_CHn_ALARM_1ST_LEVEL, 1 );
			SenEEP_Write_CHn( &eep[3], mSet.pMeas->Conf.iOut[i], SENSOR_EEP_CHn_ALARM_2ND_LEVEL, 1 );
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Alarm_Update( void )
{
	U16		eep[10];
	U8		i;
	
	for( i=0; i<mSet.pMeas->Conf.OutMax; i++ ){
		
		if( (mSet.pMeas->pAlr[i]->Inf._1stLatchEnabled	!= mSet.sAlr[i]._1stLatchEnabled) ||
			(mSet.pMeas->pAlr[i]->Inf._1stTypeIncrease	!= mSet.sAlr[i]._1stTypeIncrease) ||
			(mSet.pMeas->pAlr[i]->Inf._1stLevel			!= mSet.sAlr[i]._1stLevel		) ||
			(mSet.pMeas->pAlr[i]->Inf._1stDead			!= mSet.sAlr[i]._1stDead		) ||
			(mSet.pMeas->pAlr[i]->Inf._1stDelaySec		!= mSet.sAlr[i]._1stDelaySec	) ||
			(mSet.pMeas->pAlr[i]->Inf._2ndLatchEnabled	!= mSet.sAlr[i]._2ndLatchEnabled) ||
			(mSet.pMeas->pAlr[i]->Inf._2ndTypeIncrease	!= mSet.sAlr[i]._2ndTypeIncrease) ||
			(mSet.pMeas->pAlr[i]->Inf._2ndLevel			!= mSet.sAlr[i]._2ndLevel		) ||
			(mSet.pMeas->pAlr[i]->Inf._2ndDead			!= mSet.sAlr[i]._2ndDead		) ||
			(mSet.pMeas->pAlr[i]->Inf._2ndDelaySec		!= mSet.sAlr[i]._2ndDelaySec	) )
		{
			eep[0] = mSet.pMeas->pAlr[i]->Inf._1stLatchEnabled	= mSet.sAlr[i]._1stLatchEnabled;
			eep[1] = mSet.pMeas->pAlr[i]->Inf._1stTypeIncrease	= mSet.sAlr[i]._1stTypeIncrease;
			eep[2] = mSet.pMeas->pAlr[i]->Inf._1stLevel			= mSet.sAlr[i]._1stLevel;
			eep[3] = mSet.pMeas->pAlr[i]->Inf._1stDead			= mSet.sAlr[i]._1stDead;
			eep[4] = mSet.pMeas->pAlr[i]->Inf._1stDelaySec		= mSet.sAlr[i]._1stDelaySec;
			eep[5] = mSet.pMeas->pAlr[i]->Inf._2ndLatchEnabled	= mSet.sAlr[i]._2ndLatchEnabled;
			eep[6] = mSet.pMeas->pAlr[i]->Inf._2ndTypeIncrease	= mSet.sAlr[i]._2ndTypeIncrease;
			eep[7] = mSet.pMeas->pAlr[i]->Inf._2ndLevel			= mSet.sAlr[i]._2ndLevel;
			eep[8] = mSet.pMeas->pAlr[i]->Inf._2ndDead			= mSet.sAlr[i]._2ndDead;
			eep[9] = mSet.pMeas->pAlr[i]->Inf._2ndDelaySec		= mSet.sAlr[i]._2ndDelaySec;
				
			SenEEP_Write_CHn( eep, mSet.pMeas->Conf.iOut[i], SENSOR_EEP_CHn_ALARM_1ST_LATCH, 10 );
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Cross_Scale_Update( void )
{
	U8		i;
	
	for( i=0; i<mSet.pMeas->Conf.OutMax; i++ ){
		
		if( mSet.pMeas->pGas[i]->Inf.CrossScale != mSet.sGas[i].CrossScale ){
			mSet.pMeas->pGas[i]->Inf.CrossScale  = mSet.sGas[i].CrossScale;
			
			SenEEP_Write_CHn( &mSet.sGas[i].CrossScale, mSet.pMeas->Conf.iOut[i], SENSOR_EEP_CHn_CROSS_SENS_SCALE_x100, 1 );
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Gas_Name_Update( U8 i )
{
	if( strcmp( (char *)mSet.pMeas->pGas[i]->Inf.GasName, mSet.GasName ) != 0 ){
		strcpy( (char *)mSet.pMeas->pGas[i]->Inf.GasName, mSet.GasName );
		
		Measuring_Gas_Name_Update( mSet.pMeas->Conf.iOut[i] );
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void KeyProcess( Func_HandleType *pFunc )
{
	U8						ItemMin;
	U8						i;
	Measuring_GasInfo		*pGas = &mSet.sGas[mSet.sCh];
	Measuring_AlarmInfo		*pAlr = &mSet.sAlr[mSet.sCh];
	U16						AlrSetMin;
	
	switch( pFunc->Level ){
		case FUNC_LEVEL_1:
			mSet.pMeas = &MeasHandle;
			break;
			
		case FUNC_LEVEL_2:
			switch( pFunc->KeyData ){
				case Function:
					switch( pFunc->Item.Lv2 ){
						case 0: // Rnage
						case 2: // Cross Scale
							for( i=0; i<mSet.pMeas->Conf.OutMax; i++ ){
								mSet.sGas[i] = mSet.pMeas->pGas[i]->Inf;
							}
							pFunc->Item.Lv3 = 0;
							break;
							
						case 1: // Alarm
							for( i=0; i<mSet.pMeas->Conf.OutMax; i++ ){
								mSet.sGas[i] = mSet.pMeas->pGas[i]->Inf;
								mSet.sAlr[i] = mSet.pMeas->pAlr[i]->Inf;
							}
							if( mSet.pMeas->Conf.OutMax > 1 ){
								pFunc->Item.Lv3 = 0;
							}
							else {
								pFunc->Item.Lv3 = 1 + mSet.sGas[0].Msd;
							}
							mSet.sCh = 0;
							break;
							
						case 3: // Gas Name
							pFunc->Item.Lv3 = 0;
							break;
					}
					pFunc->Level = FUNC_LEVEL_3;
					pFunc->IsApproved = false;
					break;
					
				case Up:
					if( pFunc->Item.Lv2 ){
						pFunc->Item.Lv2--;
					}
					break;
					
				case Down:
					if( pFunc->Item.Lv2 < 3 ){
						pFunc->Item.Lv2++;
					}
					break;
					
				case Reset:
					MoveFuncTop();
					break;
			}
			break;
			
		case FUNC_LEVEL_3:
			switch( pFunc->KeyData ){
				case Function:
					switch( pFunc->Item.Lv2 ){
						case 0: // Rnage
							if( pFunc->Item.Lv3 < 12 ){
								if( pFunc->KeyType == Long ){
									pFunc->Item.Lv3 = 12;
								}
								else {
									if( (pFunc->Item.Lv3 == 5) && (mSet.pMeas->Conf.OutMax < 2) ){
										pFunc->Item.Lv3 = 12;
									}
									else {
										pFunc->Item.Lv3++;
									}
								}
							}
							else {
								if( pFunc->IsApproved == true ){
									Range_Update();
								}
								pFunc->Level = FUNC_LEVEL_2;
							}
							break;
							
						case 1: // Alarm
							if( pFunc->Item.Lv3 < 17 ){
								if( pFunc->KeyType == Long ){
									pFunc->Item.Lv3 = 17;
								}
								else {
									switch( pFunc->Item.Lv3 ){
										case  0: pFunc->Item.Lv3 = 1 + pGas->Msd; break;
										case  8: pFunc->Item.Lv3 = 9 + pGas->Msd; break;
										default: pFunc->Item.Lv3++; break;
									}
								}
							}
							else {
								if( pFunc->IsApproved == true ){
									Alarm_Update();
								}
								pFunc->Level = FUNC_LEVEL_2;
							}
							break;
							
						case 2: // Cross Scale
							if( pFunc->Item.Lv3 < 8 ){
								if( pFunc->KeyType == Long ){
									pFunc->Item.Lv3 = 8;
								}
								else {
									if( pFunc->Item.Lv3 == 3 ){
										if( mSet.pMeas->Conf.OutMax < 2 ){
											pFunc->Item.Lv3 = 8;
										}
										else {
											pFunc->Item.Lv3 = 4;
										}
									}
									else {
										pFunc->Item.Lv3++;
									}
								}
							}
							else {
								if( pFunc->IsApproved == true ){
									Cross_Scale_Update();
								}
								pFunc->Level = FUNC_LEVEL_2;
							}
							break;
							
						case 3: // Gas Name
							if( pFunc->Item.Lv3 == 2 ){ // Init
								mSet.IsSelected[0] = false;
								mSet.IsSelected[1] = false;
							}
							else {
								strcpy( mSet.GasName, (char *)mSet.pMeas->pGas[pFunc->Item.Lv3]->Inf.GasName );
							}
							pFunc->Item.Lv4 = 0;
							pFunc->Level = FUNC_LEVEL_4;
							pFunc->IsApproved = false;
							break;
					}
					break;
					
				case Up:
					switch( pFunc->Item.Lv2 ){
						case 0: // Rnage
							switch( pFunc->Item.Lv3 ){
								case  0: Func_uInc(    3,    1, &mSet.sGas[0].Dp		); break;
								case  1: Func_uInc( 9999, 1000, &mSet.sGas[0].HighScale	); break;
								case  2: Func_uInc( 9999,  100, &mSet.sGas[0].HighScale	); break;
								case  3: Func_uInc( 9999,   10, &mSet.sGas[0].HighScale	); break;
								case  4: Func_uInc( 9999,    1, &mSet.sGas[0].HighScale	); break;
								case  5: Func_uInc(    3,    1, &mSet.sGas[0].Unit		); break;
								case  6: Func_uInc(    3,    1, &mSet.sGas[1].Dp		); break;
								case  7: Func_uInc( 9999, 1000, &mSet.sGas[1].HighScale	); break;
								case  8: Func_uInc( 9999,  100, &mSet.sGas[1].HighScale	); break;
								case  9: Func_uInc( 9999,   10, &mSet.sGas[1].HighScale	); break;
								case 10: Func_uInc( 9999,    1, &mSet.sGas[1].HighScale	); break;
								case 11: Func_uInc(    3,    1, &mSet.sGas[1].Unit		); break;
								case 12: pFunc->IsApproved ^= true; break;
							}
							break;
							
						case 1: // Alarm
							switch( pFunc->Item.Lv3 ){
								case  0: mSet.sCh ^= 1; break;
								case  1: Func_uInc( pGas->HighScale, 1000, (U16 *)&pAlr->_1stLevel ); break;
								case  2: Func_uInc( pGas->HighScale,  100, (U16 *)&pAlr->_1stLevel ); break;
								case  3: Func_uInc( pGas->HighScale,   10, (U16 *)&pAlr->_1stLevel ); break;
								case  4: Func_uInc( pGas->HighScale,    1, (U16 *)&pAlr->_1stLevel ); break;
								case  5: Func_uInc( 10, 1, &pAlr->_1stDead		); break;
								case  6: Func_uInc( 30, 1, &pAlr->_1stDelaySec	); break;
								case  7: pAlr->_1stLatchEnabled ^= true; break;
								case  8: pAlr->_1stTypeIncrease ^= true; break;
								case  9: Func_uInc( pGas->HighScale, 1000, (U16 *)&pAlr->_2ndLevel ); break;
								case 10: Func_uInc( pGas->HighScale,  100, (U16 *)&pAlr->_2ndLevel ); break;
								case 11: Func_uInc( pGas->HighScale,   10, (U16 *)&pAlr->_2ndLevel ); break;
								case 12: Func_uInc( pGas->HighScale,    1, (U16 *)&pAlr->_2ndLevel ); break;
								case 13: Func_uInc( 10, 1, &pAlr->_2ndDead		); break;
								case 14: Func_uInc( 30, 1, &pAlr->_2ndDelaySec	); break;
								case 15: pAlr->_2ndLatchEnabled ^= true; break;
								case 16: pAlr->_2ndTypeIncrease ^= true; break;
								case 17: pFunc->IsApproved ^= true; break;
							}
							break;
							
						case 2: // Cross Scale
							switch( pFunc->Item.Lv3 ){
								case 0: Func_uInc( 5000, 1000, &mSet.sGas[0].CrossScale ); break;
								case 1: Func_uInc( 5000,  100, &mSet.sGas[0].CrossScale ); break;
								case 2: Func_uInc( 5000,   10, &mSet.sGas[0].CrossScale ); break;
								case 3: Func_uInc( 5000,    1, &mSet.sGas[0].CrossScale ); break;
								case 4: Func_uInc( 5000, 1000, &mSet.sGas[1].CrossScale ); break;
								case 5: Func_uInc( 5000,  100, &mSet.sGas[1].CrossScale ); break;
								case 6: Func_uInc( 5000,   10, &mSet.sGas[1].CrossScale ); break;
								case 7: Func_uInc( 5000,    1, &mSet.sGas[1].CrossScale ); break;
								case 8: pFunc->IsApproved ^= true; break;
							}
							break;
							
						case 3: // Gas Name
							if( mSet.pMeas->Conf.OutMax > 1 ){
								if( pFunc->Item.Lv3 ){
									pFunc->Item.Lv3--;
								}
							}
							else {
								pFunc->Item.Lv3 = 0;
							}
							break;
					}
					break;
					
				case Down:
					switch( pFunc->Item.Lv2 ){
						case 0: // Rnage
							switch( pFunc->Item.Lv3 ){
								case  0: Func_uDec(   0,    1, &mSet.sGas[0].Dp			); break;
								case  1: Func_uDec( 100, 1000, &mSet.sGas[0].HighScale	); break;
								case  2: Func_uDec( 100,  100, &mSet.sGas[0].HighScale	); break;
								case  3: Func_uDec( 100,   10, &mSet.sGas[0].HighScale	); break;
								case  4: Func_uDec( 100,    1, &mSet.sGas[0].HighScale	); break;
								case  5: Func_uDec(   0,    1, &mSet.sGas[0].Unit		); break;
								case  6: Func_uDec(   0,    1, &mSet.sGas[1].Dp			); break;
								case  7: Func_uDec( 100, 1000, &mSet.sGas[1].HighScale	); break;
								case  8: Func_uDec( 100,  100, &mSet.sGas[1].HighScale	); break;
								case  9: Func_uDec( 100,   10, &mSet.sGas[1].HighScale	); break;
								case 10: Func_uDec( 100,    1, &mSet.sGas[1].HighScale	); break;
								case 11: Func_uDec(   0,    1, &mSet.sGas[1].Unit		); break;
								case 12: pFunc->IsApproved ^= true; break;
							}
							break;
							
						case 1: // Alarm
							AlrSetMin = pGas->ZeroSkipPer / 10 * pGas->HighScale / 100 + 1;
							switch( pFunc->Item.Lv3 ){
								case  0: mSet.sCh ^= 1; break;
								case  1: Func_uDec( AlrSetMin, 1000, (U16 *)&pAlr->_1stLevel ); break;
								case  2: Func_uDec( AlrSetMin,  100, (U16 *)&pAlr->_1stLevel ); break;
								case  3: Func_uDec( AlrSetMin,   10, (U16 *)&pAlr->_1stLevel ); break;
								case  4: Func_uDec( AlrSetMin,    1, (U16 *)&pAlr->_1stLevel ); break;
								case  5: Func_uDec( 0 , 1, &pAlr->_1stDead		); break;
								case  6: Func_uDec( 0 , 1, &pAlr->_1stDelaySec	); break;
								case  7: pAlr->_1stLatchEnabled ^= true; break;
								case  8: pAlr->_1stTypeIncrease ^= true; break;
								case  9: Func_uDec( AlrSetMin, 1000, (U16 *)&pAlr->_2ndLevel ); break;
								case 10: Func_uDec( AlrSetMin,  100, (U16 *)&pAlr->_2ndLevel ); break;
								case 11: Func_uDec( AlrSetMin,   10, (U16 *)&pAlr->_2ndLevel ); break;
								case 12: Func_uDec( AlrSetMin,    1, (U16 *)&pAlr->_2ndLevel ); break;
								case 13: Func_uDec( 0 , 1, &pAlr->_2ndDead		); break;
								case 14: Func_uDec( 0 , 1, &pAlr->_2ndDelaySec	); break;
								case 15: pAlr->_2ndLatchEnabled ^= true; break;
								case 16: pAlr->_2ndTypeIncrease ^= true; break;
								case 17: pFunc->IsApproved ^= true; break;
							}
							break;
							
						case 2: // Cross Scale
							switch( pFunc->Item.Lv3 ){
								case 0: Func_uDec( 1, 1000, &mSet.sGas[0].CrossScale ); break;
								case 1: Func_uDec( 1,  100, &mSet.sGas[0].CrossScale ); break;
								case 2: Func_uDec( 1,   10, &mSet.sGas[0].CrossScale ); break;
								case 3: Func_uDec( 1,    1, &mSet.sGas[0].CrossScale ); break;
								case 4: Func_uDec( 1, 1000, &mSet.sGas[1].CrossScale ); break;
								case 5: Func_uDec( 1,  100, &mSet.sGas[1].CrossScale ); break;
								case 6: Func_uDec( 1,   10, &mSet.sGas[1].CrossScale ); break;
								case 7: Func_uDec( 1,    1, &mSet.sGas[1].CrossScale ); break;
								case 8: pFunc->IsApproved ^= true; break;
							}
							break;
							
						case 3: // Gas Name
							if( mSet.pMeas->Conf.OutMax > 1 ){
								if( pFunc->Item.Lv3 < 2 ){
									pFunc->Item.Lv3++;
								}
							}
							else {
								pFunc->Item.Lv3 = 2;
							}
							break;
					}
					break;
					
				case Reset:
					switch( pFunc->Item.Lv2 ){
						case 0: // Rnage
							if( pFunc->Item.Lv3 ){
								if( pFunc->KeyType == Long ){
									pFunc->Item.Lv3 = 0;
								}
								else {
									if( (pFunc->Item.Lv3 == 12) && (mSet.pMeas->Conf.OutMax < 2) ){
										pFunc->Item.Lv3 = 5;
									}
									else {
										pFunc->Item.Lv3--;
									}
								}
							}
							else {
								pFunc->Level = FUNC_LEVEL_2;
							}
							break;
							
						case 1: // Alarm
							if( mSet.pMeas->Conf.OutMax > 1 ){
								ItemMin = 0;
							}
							else {
								ItemMin = 1 + pGas->Msd;
							}
							if( pFunc->Item.Lv3 > ItemMin ){
								if( pFunc->KeyType == Long ){
									pFunc->Item.Lv3 = ItemMin;
								}
								else {
									switch( pFunc->Item.Lv3 ){
										case  2:
										case  3:
										case  4:
											if( pGas->Msd == (pFunc->Item.Lv3 - 1) ){
												if( ItemMin ){
													pFunc->Level = FUNC_LEVEL_2;
												}
												else {
													pFunc->Item.Lv3 = 0;
												}
											}
											else {
												pFunc->Item.Lv3--;
											}
											break;
											
										case 10:
										case 11:
										case 12:
											if( pGas->Msd == (pFunc->Item.Lv3 - 9) ){
												pFunc->Item.Lv3 = 8;
											}
											else {
												pFunc->Item.Lv3--;
											}
											break;
											
										default:
											pFunc->Item.Lv3--;
											break;
									}
								}
							}
							else {
								pFunc->Level = FUNC_LEVEL_2;
							}
							break;
							
						case 2: // Cross Scale
							if( pFunc->Item.Lv3 ){
								if( pFunc->KeyType == Long ){
									pFunc->Item.Lv3 = 0;
								}
								else {
									if( (pFunc->Item.Lv3 == 8) && (mSet.pMeas->Conf.OutMax < 2) ){
										pFunc->Item.Lv3 = 3;
									}
									else {
										pFunc->Item.Lv3--;
									}
								}
							}
							else {
								pFunc->Level = FUNC_LEVEL_2;
							}
							break;
							
						case 3: // Gas Name
							pFunc->Level = FUNC_LEVEL_2;
							break;
					}
					break;
			}
			break;
			
		case FUNC_LEVEL_4:
			switch( pFunc->KeyData ){
				case Function:
					switch( pFunc->Item.Lv2 ){
						case 3: // Gas Name
							switch( pFunc->Item.Lv3 ){
								case 0:
								case 1:
									if( pFunc->Item.Lv4 < 10 ){
										if( pFunc->KeyType == Long ){
											pFunc->Item.Lv4 = 10;
										}
										else {
											pFunc->Item.Lv4++;
										}
									}
									else {
										if( pFunc->IsApproved == true ){
											Gas_Name_Update( pFunc->Item.Lv3 );
										}
										pFunc->Level = FUNC_LEVEL_3;
									}
									break;
									
								case 2:
									switch( pFunc->Item.Lv4 ){
										case 0:
										case 1:
											mSet.IsSelected[pFunc->Item.Lv4] ^= true;
											break;
											
										case 2:
											if( mSet.IsConfirmMode == true ){
												if( pFunc->IsApproved == true ){
													for( i=0; i<mSet.pMeas->Conf.OutMax; i++ ){
														if( mSet.IsSelected[i] == true ){
															Measuring_Default_Gas_Name_Load( &mSet.pMeas->pGas[i]->Inf );
															Measuring_Gas_Name_Update( mSet.pMeas->Conf.iOut[i] );
														}
													}
												}
												mSet.IsConfirmMode = false;
											}
											else {
												if( (mSet.IsSelected[0] == true) || (mSet.IsSelected[1] == true) ){
													pFunc->IsApproved = false;
													mSet.IsConfirmMode = true;
												}
											}
											break;
									}
									break;
							}
							break;
					}
					break;
					
				case Up:
					switch( pFunc->Item.Lv2 ){
						case 3: // Gas Name
							switch( pFunc->Item.Lv3 ){
								case 0:
								case 1:
									if( pFunc->Item.Lv4 == 10 ){
										pFunc->IsApproved ^= true;
									}
									else {
										switch( mSet.GasName[pFunc->Item.Lv4] ){
											case '_': mSet.GasName[pFunc->Item.Lv4] = 'z';	break;
											case ' ': mSet.GasName[pFunc->Item.Lv4] = '_';	break;
											case '#': mSet.GasName[pFunc->Item.Lv4] = ' ';	break;
											case '%': mSet.GasName[pFunc->Item.Lv4] = '#';	break;
											case '(': mSet.GasName[pFunc->Item.Lv4] = '&';	break;
											case '-': mSet.GasName[pFunc->Item.Lv4] = ')';	break;
											case '>': mSet.GasName[pFunc->Item.Lv4] = '<';	break;
											case '[': mSet.GasName[pFunc->Item.Lv4] = '>';	break;
											case ']': mSet.GasName[pFunc->Item.Lv4] = '[';	break;
											case 'A': mSet.GasName[pFunc->Item.Lv4] = ']';	break;
											case 'a': mSet.GasName[pFunc->Item.Lv4] = 'Z';	break;
											default	: mSet.GasName[pFunc->Item.Lv4]--;		break;
										}
									}
									break;
									
								case 2:
									if( mSet.IsConfirmMode == true ){
										pFunc->IsApproved ^= true;
									}
									else {
										pFunc->Item.Lv4 = 0;
									}
									break;
							}
							break;
					}
					break;
					
				case Down:
					switch( pFunc->Item.Lv2 ){
						case 3: // Gas Name
							switch( pFunc->Item.Lv3 ){
								case 0:
								case 1:
									if( pFunc->Item.Lv4 == 10 ){
										pFunc->IsApproved ^= true;
									}
									else {
										switch( mSet.GasName[pFunc->Item.Lv4] ){
											case '_': mSet.GasName[pFunc->Item.Lv4] = ' ';	break;
											case ' ': mSet.GasName[pFunc->Item.Lv4] = '#';	break;
											case '#': mSet.GasName[pFunc->Item.Lv4] = '%';	break;
											case '&': mSet.GasName[pFunc->Item.Lv4] = '(';	break;
											case ')': mSet.GasName[pFunc->Item.Lv4] = '-';	break;
											case '<': mSet.GasName[pFunc->Item.Lv4] = '>';	break;
											case '>': mSet.GasName[pFunc->Item.Lv4] = '[';	break;
											case '[': mSet.GasName[pFunc->Item.Lv4] = ']';	break;
											case ']': mSet.GasName[pFunc->Item.Lv4] = 'A';	break;
											case 'Z': mSet.GasName[pFunc->Item.Lv4] = 'a';	break;
											case 'z': mSet.GasName[pFunc->Item.Lv4] = '_';	break;
											default	: mSet.GasName[pFunc->Item.Lv4]++;		break;
										}
									}
									break;
									
								case 2:
									if( mSet.IsConfirmMode == true ){
										pFunc->IsApproved ^= true;
									}
									else {
										if( mSet.pMeas->Conf.OutMax > 1 ){
											if( pFunc->Item.Lv4 < 2 ){
												pFunc->Item.Lv4++;
											}
										}
										else {
											pFunc->Item.Lv4 = 2;
										}
									}
									break;
							}
							break;
					}
					break;
					
				case Reset:
					switch( pFunc->Item.Lv2 ){
						case 3: // Gas Name
							switch( pFunc->Item.Lv3 ){
								case 0:
								case 1:
									if( pFunc->Item.Lv4 ){
										if( pFunc->KeyType == Long ){
											pFunc->Item.Lv4 = 0;
										}
										else {
											pFunc->Item.Lv4--;
										}
									}
									else {
										pFunc->Level = FUNC_LEVEL_3;
									}
									break;
									
								case 2:
									if( mSet.IsConfirmMode == true ){
										mSet.IsConfirmMode = false;
									}
									else {
										pFunc->Level = FUNC_LEVEL_3;
									}
									break;
							}
							break;
					}
					break;
			}
			break;
	}
}


