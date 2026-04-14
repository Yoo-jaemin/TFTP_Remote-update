#define __UART_C__
    #include "uart.h"
#undef  __UART_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "modbus.h"
/* End Include */

/* Start Define */
#define DEBG_PORT							( DioClrPin( pADI_GPIO2, PIN4 ) )
#define MAIN_PORT							( DioSetPin( pADI_GPIO2, PIN4 ) )
#define UART_PORT_SELECTION( port )			( port )
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

int putchar( int c )
{
	UART_PORT_SELECTION( DEBG_PORT );
	
	if(c == '\n'){
		//LogHandle.UsedLF = true;
		c = '\r';
		UrtTx( pADI_UART0, c );
		while( !(pADI_UART0->COMLSR & BITM_UART_COMLSR_TEMT) );
		c = '\n';
	}

	UrtTx( pADI_UART0, c );
	while( !(pADI_UART0->COMLSR & BITM_UART_COMLSR_TEMT) );
	
	UART_PORT_SELECTION( MAIN_PORT );
	
	return c;
}
 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
U16		LineStatus;
U16		IntID;

U16		NumRxInts;
U16		NumLsInts;
U16		NumTxInts;
U16		NumToInts;

void UART_Init( void )
{
	GPIO_Init();
	
	UrtCfg( pADI_UART0, B115200, (BITM_UART_COMLCR_WLS | 3), 0 );
	UrtIntCfg( pADI_UART0, BITM_UART_COMIEN_ERBFI | BITM_UART_COMIEN_ETBEI | BITM_UART_COMIEN_ELSI );
	
	NVIC_EnableIRQ( UART_EVT_IRQn );
}
 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void UART_Int_Handler( void )
{
	LineStatus = pADI_UART0->COMLSR;
	IntID = pADI_UART0->COMIIR & 0x0E;
	
	switch( IntID ){
		case 0x02: // Transmit buffer empty.
			NumTxInts++;
			MODBUS_Tx_Interrupt_Callback();
			break;
			
		case 0x04: // Receive buffer full.
			NumRxInts++;
			MODBUS_Rx_Interrupt_Callback();
			break;
			
		case 0x06: // Receive line status.
			NumLsInts++;
			break;
			
		case 0x0C: // Receive FIFO timed out.
			NumToInts++;
			break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void GPIO_Init( void )
{
	DioCfgPin( pADI_GPIO0, PIN10, 1 );
	DioOenPin( pADI_GPIO0, PIN10, 1 );
	DioPulPin( pADI_GPIO0, PIN10, 1 );
	
	DioCfgPin( pADI_GPIO0, PIN11, 1 );
	DioIenPin( pADI_GPIO0, PIN11, 1 );
	DioPulPin( pADI_GPIO0, PIN11, 1 );
	
	DioCfgPin( pADI_GPIO2, PIN4, 0 );
	DioOenPin( pADI_GPIO2, PIN4, 1 ); // P2.4 as an output pin
	DioPulPin( pADI_GPIO2, PIN4, 1 );
	DioDsPin ( pADI_GPIO2, PIN4, 1 ); // Set drive strngth to full power
	
	UART_PORT_SELECTION( MAIN_PORT );
}


