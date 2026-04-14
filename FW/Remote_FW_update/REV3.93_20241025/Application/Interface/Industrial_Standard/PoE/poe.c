#define __POE_C__
    #include "poe.h"
#undef  __POE_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "lwip/opt.h"
#include "lwip/init.h"
#include "lwip/timeouts.h"
#include "netif/etharp.h"
#include "app_ethernet.h"
#include "httpd_cgi_ssi.h"
#include "tcp_echoserver.h"
#include "eeprom.h"
#include "option.h"
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
static void GPIO_Init( void );
static void Netif_Config( void );
static void Phy_Reset( void );
/* End Function */

/* Start Variable */
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool POE_Init( void )
{
	bool	err = false;
	
	if( (SystemOption.Item.IsPoeMode == true) || (SystemOption.Item.IsUniMode == true) ){
		
		PoeHandle.Conf.MC[0] = HI_BYTE( EepHandle.Sram[SYSTEM_EEP_POE_MC_0_1] );
		PoeHandle.Conf.MC[1] = LO_BYTE( EepHandle.Sram[SYSTEM_EEP_POE_MC_0_1] );
		PoeHandle.Conf.MC[2] = HI_BYTE( EepHandle.Sram[SYSTEM_EEP_POE_MC_2_3] );
		PoeHandle.Conf.MC[3] = LO_BYTE( EepHandle.Sram[SYSTEM_EEP_POE_MC_2_3] );
		PoeHandle.Conf.MC[4] = HI_BYTE( EepHandle.Sram[SYSTEM_EEP_POE_MC_4_5] );
		PoeHandle.Conf.MC[5] = LO_BYTE( EepHandle.Sram[SYSTEM_EEP_POE_MC_4_5] );
		PoeHandle.Conf.IP[0] = HI_BYTE( EepHandle.Sram[SYSTEM_EEP_POE_IP_0_1] );
		PoeHandle.Conf.IP[1] = LO_BYTE( EepHandle.Sram[SYSTEM_EEP_POE_IP_0_1] );
		PoeHandle.Conf.IP[2] = HI_BYTE( EepHandle.Sram[SYSTEM_EEP_POE_IP_2_3] );
		PoeHandle.Conf.IP[3] = LO_BYTE( EepHandle.Sram[SYSTEM_EEP_POE_IP_2_3] );
		PoeHandle.Conf.NM[0] = HI_BYTE( EepHandle.Sram[SYSTEM_EEP_POE_NM_0_1] );
		PoeHandle.Conf.NM[1] = LO_BYTE( EepHandle.Sram[SYSTEM_EEP_POE_NM_0_1] );
		PoeHandle.Conf.NM[2] = HI_BYTE( EepHandle.Sram[SYSTEM_EEP_POE_NM_2_3] );
		PoeHandle.Conf.NM[3] = LO_BYTE( EepHandle.Sram[SYSTEM_EEP_POE_NM_2_3] );
		PoeHandle.Conf.GW[0] = HI_BYTE( EepHandle.Sram[SYSTEM_EEP_POE_GW_0_1] );
		PoeHandle.Conf.GW[1] = LO_BYTE( EepHandle.Sram[SYSTEM_EEP_POE_GW_0_1] );
		PoeHandle.Conf.GW[2] = HI_BYTE( EepHandle.Sram[SYSTEM_EEP_POE_GW_2_3] );
		PoeHandle.Conf.GW[3] = LO_BYTE( EepHandle.Sram[SYSTEM_EEP_POE_GW_2_3] );
		
		PoeHandle.Modbus.pTxBuf = &PoeHandle.TxBuf[0];
		PoeHandle.Modbus.pTxLen = &PoeHandle.TxLen;
		
		MODBUS_TCP_Init_Slave( &PoeHandle.Modbus );
		
		GPIO_Init();
		
		/* Initialize the LwIP stack */
		lwip_init();
		
		/* Configure the Network interface */
		Netif_Config();
		
		/* tcp echo server Init */
		tcp_echoserver_init();
		
		/* Http webserver Init */
		http_server_init();
		
		/* Notify user about the network interface config */
		User_notification( &PoeHandle.NetIf );
	}
	
	return err;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void POE_ReInit( void )
{
	ethernetif_update_config( &PoeHandle.NetIf );
	ethernetif_init( &PoeHandle.NetIf );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void POE_Task( void )
{
	/* Read a received packet from the Ethernet buffers and send it to the lwIP for handling */
	ethernetif_input( &PoeHandle.NetIf );
	
	/* Handle timeouts */
	sys_check_timeouts();
	
#ifdef USE_DHCP
	/* handle periodic timers for LwIP */
	DHCP_Periodic_Handle( &PoeHandle.NetIf );
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HAL_GPIO_EXTI_Callback( uint16_t GPIO_Pin )
{
	if( GPIO_Pin == GPIO_PIN_11 ){
		ethernetif_set_link( &PoeHandle.NetIf );
		dprintf( "ETH Power Down\n" );
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void GPIO_Init( void )
{
	GPIO_InitTypeDef	GPIO_InitStructure = { 0 };
	
    /** ETH Reset Pin */
	__HAL_RCC_GPIOD_CLK_ENABLE();
	
    GPIO_InitStructure.Pin		= GPIO_PIN_13;
    GPIO_InitStructure.Mode		= GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pull		= GPIO_NOPULL;
    GPIO_InitStructure.Speed	= GPIO_SPEED_FREQ_LOW;
	
    HAL_GPIO_Init( GPIOD, &GPIO_InitStructure );
	
	/* Enable PA11 to IT mode: Ethernet Link interrupt */
	__HAL_RCC_GPIOA_CLK_ENABLE();
	
	GPIO_InitStructure.Pin	= GPIO_PIN_11;
	GPIO_InitStructure.Mode	= GPIO_MODE_IT_FALLING;
	GPIO_InitStructure.Pull	= GPIO_NOPULL;
	
	HAL_GPIO_Init( GPIOA, &GPIO_InitStructure );
	
	/* Enable EXTI Line interrupt */
	HAL_NVIC_SetPriority( EXTI15_10_IRQn, 0xF, 0 );
	HAL_NVIC_EnableIRQ( EXTI15_10_IRQn );
	
	Phy_Reset();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Phy_Reset( void )
{
	HAL_GPIO_WritePin( GPIOD, GPIO_PIN_13, GPIO_PIN_RESET );
	HAL_Delay( 100 );
	HAL_GPIO_WritePin( GPIOD, GPIO_PIN_13, GPIO_PIN_SET );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Netif_Config( void )
{
	ip_addr_t	ipaddr;
	ip_addr_t	netmask;
	ip_addr_t	gw;
	
#ifdef USE_DHCP
	ip_addr_set_zero_ip4( &ipaddr );
	ip_addr_set_zero_ip4( &netmask );
	ip_addr_set_zero_ip4( &gw );
#else
	IP_ADDR4( &ipaddr	, PoeHandle.Conf.IP[0], PoeHandle.Conf.IP[1], PoeHandle.Conf.IP[2], PoeHandle.Conf.IP[3] );
	IP_ADDR4( &netmask	, PoeHandle.Conf.NM[0], PoeHandle.Conf.NM[1], PoeHandle.Conf.NM[2], PoeHandle.Conf.NM[3] );
	IP_ADDR4( &gw		, PoeHandle.Conf.GW[0], PoeHandle.Conf.GW[1], PoeHandle.Conf.GW[2], PoeHandle.Conf.GW[3] );
#endif /* USE_DHCP */
	  
	/* Add the network interface */    
	netif_add( &PoeHandle.NetIf, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &ethernet_input );
	
	/*  Registers the default network interface */
	netif_set_default( &PoeHandle.NetIf );
	
	if( netif_is_link_up( &PoeHandle.NetIf ) ){
		/* When the netif is fully configured this function must be called */
		netif_set_up( &PoeHandle.NetIf );
	}
	else {
		/* When the netif link is down this function must be called */
		netif_set_down( &PoeHandle.NetIf );
	}
	
	/* Set the link callback function, this function is called on change of link status*/
	netif_set_link_callback( &PoeHandle.NetIf, ethernetif_update_config );
}


