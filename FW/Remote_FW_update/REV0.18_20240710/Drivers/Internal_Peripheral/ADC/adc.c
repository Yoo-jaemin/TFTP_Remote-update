#define __ADC_C__
    #include "adc.h"
#undef  __ADC_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "led.h"
#include "main.h"
/* End Include */

/* Start Define */
#define ADC_ERR_CNT			( 1000 )
#define ADC_MUXP			( 0 )
#define ADC_MUXN			( 1 )
#define ADC_STOP()			( pADI_AFE->AFECON &= ~(BITM_AFE_AFECON_ADCCONVEN | BITM_AFE_AFECON_TEMPCONVEN) )

#define ADC_TP_Pin			( PIN3 )
#define ADC_TP_GPIO_Port	( pADI_GPIO0 )

#define ADC_TP_H()			DioSetPin( ADC_TP_GPIO_Port, ADC_TP_Pin )
#define ADC_TP_L()			DioClrPin( ADC_TP_GPIO_Port, ADC_TP_Pin )
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
static bool ADC_Select_Mux( U8 Mux );
/* End Function */

/* Start Variable */
static U16			ADC_ErrCntMin;
static const U32	ADmux[ADMUX_MAX][2] =
{
	{ MUXSELP_TEMP		, MUXSELN_TEMPN   },
	{ MUXSELP_AVDD_2	, MUXSELN_VSET1P1 },
	{ MUXSELP_DVDD_2	, MUXSELN_VSET1P1 },
	{ MUXSELP_AIN0		, MUXSELN_VSET1P1 },
	{ MUXSELP_LPTIA0_LPF, MUXSELN_VSET1P1 },
	{ MUXSELP_VZERO0	, MUXSELN_VSET1P1 },
	{ MUXSELP_VBIAS0	, MUXSELN_VSET1P1 },
	{ MUXSELP_AIN2		, MUXSELN_VSET1P1 },
	//{ MUXSELP_VCE0		, MUXSELN_VSET1P1 },
	//{ MUXSELP_VRE0		, MUXSELN_VSET1P1 },
	{ MUXSELP_LPTIA1_LPF, MUXSELN_VSET1P1 },
	{ MUXSELP_VZERO1	, MUXSELN_VSET1P1 },
	{ MUXSELP_VBIAS1	, MUXSELN_VSET1P1 },
	{ MUXSELP_AIN1		, MUXSELN_VSET1P1 }//,
	//{ MUXSELP_VCE1		, MUXSELN_VSET1P1 },
	//{ MUXSELP_VRE1		, MUXSELN_VSET1P1 }
};
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                 
void ADC_Init( void )
{
	U8		i;
	U32		Status;
	
	// Tp
	DioCfgPin( ADC_TP_GPIO_Port, ADC_TP_Pin, 0 );
	DioOenPin( ADC_TP_GPIO_Port, ADC_TP_Pin, 1 );
	
	ADC_Handle.iMux = 0;
	for( i=0; i<ADMUX_MAX; i++ ){
		if( i == ADMUX_TEMP ){
			ADC_Handle.ConvItem[i] = BITM_AFE_AFECON_ADCCONVEN | BITM_AFE_AFECON_TEMPCONVEN;
			ADC_Handle.CheckRdy[i] = BITM_AFE_ADCINTSTA_TEMPRDY;
			ADC_Handle.pReadReg[i] = &pADI_AFE->TEMPSENSDAT;
		}
		else {
			ADC_Handle.ConvItem[i] = BITM_AFE_AFECON_ADCCONVEN;
			ADC_Handle.CheckRdy[i] = BITM_AFE_ADCINTSTA_ADCRDY;
			ADC_Handle.pReadReg[i] = &pADI_AFE->ADCDAT;
		}
	}
	ADC_Handle.ConvComplete = false;
	
	Status = pADI_AFE->ADCINTSTA;
	pADI_AFE->ADCINTSTA = Status; //clear
	
	// ADC Interrupt
	AfeAdcIntCfg( BITM_AFE_ADCINTIEN_TEMPRDYIEN | BITM_AFE_ADCINTIEN_ADCRDYIEN );
	NVIC_EnableIRQ( AFE_ADC_IRQn );
	
	// ADC Init
	AfeSysCfg( ENUM_AFE_PMBW_LP, ENUM_AFE_PMBW_BW250 );
	AfeAdcPgaCfg( GNPGA_1_5, 0 );
	ADC_Select_Mux( ADC_Handle.iMux );
	AfeAdcPwrUp( BITM_AFE_AFECON_TEMPSENSEN0 | BITM_AFE_AFECON_ADCEN );
	AfeAdcFiltCfg( SINC3OSR_4, SINC2OSR_178, LFPBYPEN_BYP, ADCSAMPLERATE_800K );
	
	delay_10us( 5 );
	
	ADC_ErrCntMin = ADC_ERR_CNT;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                 
U16 ADC_CHn_Polling( U32 MuxP, U32 MuxN )
{
	U32		Status;
	U16		err_cnt = ADC_ERR_CNT;
	bool	err = false;
	U16		Data;
	
	/*ADC initialization*/
	
	AfeSysCfg( ENUM_AFE_PMBW_LP, ENUM_AFE_PMBW_BW50 );
	AfeAdcPgaCfg( GNPGA_1_5, 0 );
	AfeAdcChan( MuxP, MuxN );
	AfeAdcPwrUp( BITM_AFE_AFECON_ADCEN );
	pADI_AFE->ADCINTSTA = BITM_AFE_ADCINTSTA_ADCRDY; //clear
	AfeAdcFiltCfg( SINC3OSR_4, SINC2OSR_178, LFPBYPEN_BYP, ADCSAMPLERATE_800K );
	delay_10us( 5 );
	
	AfeAdcGo( BITM_AFE_AFECON_ADCCONVEN ); // start ADC conversion
	
	while( !(pADI_AFE->ADCINTSTA & BITM_AFE_ADCINTSTA_ADCRDY) ){ //wait for data ready
		if( --err_cnt == 0 ){
			err = true;
		}
	}
	
	AfeAdcGo( ADCIDLE );
	AfeAdcPwrUp( 0 );
	
	Status = pADI_AFE->ADCINTSTA;
	pADI_AFE->ADCINTSTA = Status; //clear
	
	Data = pADI_AFE->ADCDAT;
	
	/*power down ADC*/
	
	SystemError.Bit.iADC = err;
	
	if( ADC_ErrCntMin > err_cnt ){
		ADC_ErrCntMin = err_cnt;
	}
	
	return Data;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ADC_Conversion_Ready( void )
{
	ADC_Handle.iMux = 0;
	AfeAdcChan( ADmux[ADC_Handle.iMux][ADC_MUXP], ADmux[ADC_Handle.iMux][ADC_MUXN] );
	ADC_Handle.ConvReady = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ADC_Conversion_Start( void )
{
	if( ADC_Handle.ConvReady == true ){
		ADC_Handle.ConvReady = false;
		
		AfeAdcGo( ADC_Handle.ConvItem[ADC_Handle.iMux] );
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                 
static bool ADC_Select_Mux( U8 Mux )
{
	bool	err = false;
	U32		rADCCON;
	
	rADCCON = AfeAdcChan( ADmux[Mux][ADC_MUXP], ADmux[Mux][ADC_MUXN] );
	
	if( pADI_AFE->ADCCON != rADCCON ){
		err = true;
	}
	
	return err;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void AfeAdc_Int_Handler( void )
{
	U32		Status;
	
	ADC_TP_H();
	
	ADC_STOP();
    
	Status = pADI_AFE->ADCINTSTA;
	pADI_AFE->ADCINTSTA = Status; // Interrupt Clear
	
	if( Status & ADC_Handle.CheckRdy[ADC_Handle.iMux] ){
		ADC_Handle.Raw.Word[ADC_Handle.iMux] = *ADC_Handle.pReadReg[ADC_Handle.iMux];
		//ADC_Handle.Cnt[ADC_Handle.iMux]++;
	}
	
	ADC_Handle.iMux++;
	if( ADC_Handle.iMux == ADMUX_MAX ){
		ADC_Handle.ConvComplete = true;
	}
	else {
		AfeAdcChan( ADmux[ADC_Handle.iMux][ADC_MUXP], ADmux[ADC_Handle.iMux][ADC_MUXN] );
		ADC_Handle.ConvReady = true;
	}
	
	ADC_TP_L();
}


