#define __OPTION_C__
    #include "option.h"
#undef  __OPTION_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "version.h"
/* End Include */

/* Start Define */
#define n5200x_Select_GPIO_Port			( pADI_GPIO0 )
#define n5200x_Select_Pin				( PIN0 )

#define n5200N_Select_GPIO_Port			( pADI_GPIO0 )
#define n5200N_Select_Pin				( PIN1 )

#define n5200x_CH2_Enable_GPIO_Port		( pADI_GPIO0 )
#define n5200x_CH2_Enable_Pin			( PIN2 )

#define n5000N_Select_GPIO_Port			( pADI_GPIO1 )
#define n5000N_Select_Pin				( PIN0 )
#define nModel_Enable_GPIO_Port			( pADI_AGPIO2 )
#define nModel_Enable_Pin				( PIN0 )
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
static void GPIO_Init( void ); 
/* End Function */

/* Start Variable */
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void System_Opt_Load( void )
{
	System_OptionType	dat;
	U8					buf, cop;
	U16					i, cnt;
	
	GPIO_Init();
	
	for( i=0, cnt=0, buf=0, cop=0; i<0xFFFF; i++ ){
		
		dat.Byte = 0;
		
        if( (DioRd( n5200x_Select_GPIO_Port ) & n5200x_Select_Pin) == 0 ){
			if( (DioRd( n5200x_CH2_Enable_GPIO_Port ) & n5200x_CH2_Enable_Pin) == 0 ){
				if( (DioRd( n5200N_Select_GPIO_Port ) & n5200N_Select_Pin) == 0 )	{ dat.Item.Model = MODEL_5200N_DUAL	; }
				else																{ dat.Item.Model = MODEL_5200FN_DUAL; }
			}
			else {
				if( (DioRd( n5200N_Select_GPIO_Port ) & n5200N_Select_Pin) == 0 )	{ dat.Item.Model = MODEL_5200N		; }
				else																{ dat.Item.Model = MODEL_5200FN		; }
			}
		}
		else {
			if(  (AfeDioRd( nModel_Enable_GPIO_Port ) & nModel_Enable_Pin) == 0 ){
				if( (DioRd( n5000N_Select_GPIO_Port ) & n5000N_Select_Pin) == 0 )	{ dat.Item.Model = MODEL_5000N		; }
				else																{ dat.Item.Model = MODEL_5100FN		; }
			}
			else {
				dat.Item.Model = MODEL_PROTO;
			}
		}
		
		cop = dat.Byte ^ buf;
		
		if( cop ){
			cnt = 0;
		}
		else {
			cnt++;
			if( cnt == 1000 ){
				SystemOption = dat;
				break;
			}
		}
		buf = dat.Byte;
		
		delay_10us( 100 );
	}
	
	dprintf( "\n  SYSTEM OPTION: 0x%02X\n", SystemOption.Byte );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void GPIO_Init( void )
{
	DioCfgPin( n5200x_Select_GPIO_Port, n5200x_Select_Pin, 0 );	// Configure
	DioIenPin( n5200x_Select_GPIO_Port, n5200x_Select_Pin, 1 );	// enable input path
	DioPulPin( n5200x_Select_GPIO_Port, n5200x_Select_Pin, 1 );	// Enable pull-up resistors
	
	DioCfgPin( n5200N_Select_GPIO_Port, n5200N_Select_Pin, 0 );	// Configure
	DioIenPin( n5200N_Select_GPIO_Port, n5200N_Select_Pin, 1 );	// enable input path
	DioPulPin( n5200N_Select_GPIO_Port, n5200N_Select_Pin, 1 );	// Enable pull-up resistors
	
	DioCfgPin( n5200x_CH2_Enable_GPIO_Port, n5200x_CH2_Enable_Pin, 0 ); // Configure
	DioIenPin( n5200x_CH2_Enable_GPIO_Port, n5200x_CH2_Enable_Pin, 1 ); // enable input path
	DioPulPin( n5200x_CH2_Enable_GPIO_Port, n5200x_CH2_Enable_Pin, 1 ); // Enable pull-up resistors
	
	DioCfgPin( n5000N_Select_GPIO_Port, n5000N_Select_Pin, 0 );	// Configure
	DioIenPin( n5000N_Select_GPIO_Port, n5000N_Select_Pin, 1 );	// enable input path
	DioPulPin( n5000N_Select_GPIO_Port, n5000N_Select_Pin, 1 );	// Enable pull-up resistors
	
	AfeDioCfgPin( nModel_Enable_GPIO_Port, nModel_Enable_Pin, 2 );	// Configure
	AfeDioIenPin( nModel_Enable_GPIO_Port, nModel_Enable_Pin, 1 );	// enable input path
	AfeDioPulPin( nModel_Enable_GPIO_Port, nModel_Enable_Pin, 1 );	// Enable pull-up resistors
}


