#define __MODBUS_TCP_SLAVE_C__
    #include "modbus_tcp_slave.h"
#undef  __MODBUS_TCP_SLAVE_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "measuring.h"
/* End Include */

/* Start Define */
//#define PACKET_PRINT
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
// Read
static void MODBUS_TCP_Read_Coils_Slave( MODBUS_TCP_SlaveHandleType *pSlave );
static void MODBUS_TCP_Read_Discretes_Input_Slave( MODBUS_TCP_SlaveHandleType *pSlave );
static void MODBUS_TCP_Read_Input_Registers_Slave( MODBUS_TCP_SlaveHandleType *pSlave );
static void MODBUS_TCP_Read_Holding_Registers_Slave( MODBUS_TCP_SlaveHandleType *pSlave );

// Write
static void MODBUS_TCP_Write_Single_Coil_Slave( MODBUS_TCP_SlaveHandleType *pSlave );
static void MODBUS_TCP_Write_Multiple_Coils_Slave( MODBUS_TCP_SlaveHandleType *pSlave );
static void MODBUS_TCP_Write_Single_Register_Slave( MODBUS_TCP_SlaveHandleType *pSlave );
static void MODBUS_TCP_Write_Multiple_Registers_Slave( MODBUS_TCP_SlaveHandleType *pSlave );
/* End Function */

/* Start Variable */
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MODBUS_TCP_Init_Slave( MODBUS_TCP_SlaveHandleType *pSlave )
{
	pSlave->pReg_0x = Modbus_Reg_0xxxx;
	pSlave->pReg_1x = Modbus_Reg_1xxxx;
	pSlave->pReg_3x = Modbus_Reg_3xxxx;
	pSlave->pReg_4x = Modbus_Reg_4xxxx;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MODBUS_TCP_Packet_Decoder_Slave( MODBUS_TCP_SlaveHandleType *pSlave )
{
#ifdef PACKET_PRINT
	U8		i;

	dprintf( "FC-%02u: RX<-", pSlave->pRxBuf[7] );
	for( i=0; i<*pSlave->pRxLen; i++ ){ dprintf( " %02X", pSlave->pRxBuf[i] ); }
	dprintf( "\n" );
#endif
	
	*pSlave->pTxLen = 0;
	
	MODBUS_Mapping();
		
	switch( pSlave->pRxBuf[7] ){
		case  1: MODBUS_TCP_Read_Coils_Slave( pSlave );					break;
		case  2: MODBUS_TCP_Read_Discretes_Input_Slave( pSlave );		break;
		case  3: MODBUS_TCP_Read_Holding_Registers_Slave( pSlave );		break;
		case  4: MODBUS_TCP_Read_Input_Registers_Slave( pSlave );		break;
		case  5: MODBUS_TCP_Write_Single_Coil_Slave( pSlave );			break;
		case  6: MODBUS_TCP_Write_Single_Register_Slave( pSlave );		break;
		case 15: MODBUS_TCP_Write_Multiple_Coils_Slave( pSlave );		break;
		case 16: MODBUS_TCP_Write_Multiple_Registers_Slave( pSlave );	break;
	}
	
	if( *pSlave->pTxLen ){
#ifdef PACKET_PRINT
		dprintf( "       TX->" );
		for( i=0; i<*pSlave->pTxLen; i++ ){ dprintf( " %02X", pSlave->pTxBuf[i] ); }
		dprintf( "\n" );
#endif
		MODBUS_Map_Update_Check();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void MODBUS_TCP_Read_Coils_Slave( MODBUS_TCP_SlaveHandleType *pSlave )
{
	U16		Start = ((U16)pSlave->pRxBuf[ 8] << 8) | ((U16)pSlave->pRxBuf[ 9]);
	U16		BiCnt = ((U16)pSlave->pRxBuf[10] << 8) | ((U16)pSlave->pRxBuf[11]);
	U8		ByCnt = (BiCnt + 7) / 8;
	U16		RefBy = Start / 8;
	U16		RefBi = Start % 8;
	U16		i;
	
	if( (Start + BiCnt) > (MODBUS_0xxxx_MAX * 8) ){
		dprintf( "  Maximum index of read coils command has been exceeded.\n" );
		return;
	}
	
	pSlave->pTxBuf[0] = pSlave->pRxBuf[0];
	pSlave->pTxBuf[1] = pSlave->pRxBuf[1];
	pSlave->pTxBuf[2] = pSlave->pRxBuf[2];
	pSlave->pTxBuf[3] = pSlave->pRxBuf[3];
	pSlave->pTxBuf[4] = pSlave->pRxBuf[4];
	pSlave->pTxBuf[5] = 3 + ByCnt;
	pSlave->pTxBuf[6] = pSlave->pRxBuf[6];
	pSlave->pTxBuf[7] = pSlave->pRxBuf[7];
	pSlave->pTxBuf[8] = ByCnt;
	
	for( i=0; i<ByCnt; i++ ){
		if( (RefBi == 0) || (i == (ByCnt - 1)) ){
			pSlave->pTxBuf[9+i] = (pSlave->pReg_0x[RefBy+i].Byte >> RefBi);
		}
		else {
			pSlave->pTxBuf[9+i] = (pSlave->pReg_0x[RefBy+i].Byte >> RefBi) | (pSlave->pReg_0x[RefBy+i+1].Byte << (8 - RefBi));
		}
	}
	
	*pSlave->pTxLen = 9 + ByCnt;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void MODBUS_TCP_Read_Discretes_Input_Slave( MODBUS_TCP_SlaveHandleType *pSlave )
{
	U16		Start = ((U16)pSlave->pRxBuf[ 8] << 8) | ((U16)pSlave->pRxBuf[ 9]);
	U16		BiCnt = ((U16)pSlave->pRxBuf[10] << 8) | ((U16)pSlave->pRxBuf[11]);
	U8		ByCnt = (BiCnt + 7) / 8;
	U16		RefBy = Start / 8;
	U16		RefBi = Start % 8;
	U16		EndBi = Start + BiCnt;
	U16		i;
	
	if( EndBi > (MODBUS_1xxxx_MAX * 8) ){
		dprintf( "  Maximum index of read input discretes command has been exceeded.\n" );
		return;
	}
	
	pSlave->pTxBuf[0] = pSlave->pRxBuf[0];
	pSlave->pTxBuf[1] = pSlave->pRxBuf[1];
	pSlave->pTxBuf[2] = pSlave->pRxBuf[2];
	pSlave->pTxBuf[3] = pSlave->pRxBuf[3];
	pSlave->pTxBuf[4] = pSlave->pRxBuf[4];
	pSlave->pTxBuf[5] = 3 + ByCnt;
	pSlave->pTxBuf[6] = pSlave->pRxBuf[6];
	pSlave->pTxBuf[7] = pSlave->pRxBuf[7];
	pSlave->pTxBuf[8] = ByCnt;
	
	for( i=Start; i<EndBi; i++ ){
		switch( i ){
			case  7: Modbus_Heart.CH1_Tog_1xxxx ^= 1; break;
			case 15: Modbus_Heart.CH2_Tog_1xxxx ^= 1; break;
		}
	}
	
	for( i=0; i<ByCnt; i++ ){
		if( (RefBi == 0) || (i == (ByCnt - 1)) ){
			pSlave->pTxBuf[9+i] = (pSlave->pReg_1x[RefBy+i].Byte >> RefBi);
		}
		else {
			pSlave->pTxBuf[9+i] = (pSlave->pReg_1x[RefBy+i].Byte >> RefBi) | (pSlave->pReg_1x[RefBy+i+1].Byte << (8 - RefBi));
		}
	}
	
	*pSlave->pTxLen = 9 + ByCnt;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void MODBUS_TCP_Read_Input_Registers_Slave( MODBUS_TCP_SlaveHandleType *pSlave )
{
	U16		Start = ((U16)pSlave->pRxBuf[ 8] << 8) | ((U16)pSlave->pRxBuf[ 9]);
	U16		WoCnt = ((U16)pSlave->pRxBuf[10] << 8) | ((U16)pSlave->pRxBuf[11]);
	U16		ByCnt = WoCnt * 2;
	U16		i;
	
	if( (Start + WoCnt) > MODBUS_3xxxx_MAX ){
		dprintf( "  Maximum index of read input registers command has been exceeded.\n" );
		return;
	}
	
	pSlave->pTxBuf[0] = pSlave->pRxBuf[0];
	pSlave->pTxBuf[1] = pSlave->pRxBuf[1];
	pSlave->pTxBuf[2] = pSlave->pRxBuf[2];
	pSlave->pTxBuf[3] = pSlave->pRxBuf[3];
	pSlave->pTxBuf[4] = pSlave->pRxBuf[4];
	pSlave->pTxBuf[5] = 3 + ByCnt;
	pSlave->pTxBuf[6] = pSlave->pRxBuf[6];
	pSlave->pTxBuf[7] = pSlave->pRxBuf[7];
	pSlave->pTxBuf[8] = ByCnt;
	
	for( i=0; i<WoCnt; i++ ){
		switch( Start + i ){
			case MODBUS_30089: Modbus_Heart.Rpt_Cnt_3xxxx++; break;
		}
		pSlave->pTxBuf[ 9+i*2] = HI_BYTE( pSlave->pReg_3x[Start+i] );
		pSlave->pTxBuf[10+i*2] = LO_BYTE( pSlave->pReg_3x[Start+i] );
	}
	
	*pSlave->pTxLen = 9 + ByCnt;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void MODBUS_TCP_Read_Holding_Registers_Slave( MODBUS_TCP_SlaveHandleType *pSlave )
{
	U16		Start = ((U16)pSlave->pRxBuf[ 8] << 8) | ((U16)pSlave->pRxBuf[ 9]);
	U16		WoCnt = ((U16)pSlave->pRxBuf[10] << 8) | ((U16)pSlave->pRxBuf[11]);
	U16		ByCnt = WoCnt * 2;
	U16		i;
	
	if( (Start + WoCnt) > MODBUS_4xxxx_MAX ){
		dprintf( "  Maximum index of read multiple registers command has been exceeded.\n" );
		return;
	}
	
	pSlave->pTxBuf[0] = pSlave->pRxBuf[0];
	pSlave->pTxBuf[1] = pSlave->pRxBuf[1];
	pSlave->pTxBuf[2] = pSlave->pRxBuf[2];
	pSlave->pTxBuf[3] = pSlave->pRxBuf[3];
	pSlave->pTxBuf[4] = pSlave->pRxBuf[4];
	pSlave->pTxBuf[5] = 3 + ByCnt;
	pSlave->pTxBuf[6] = pSlave->pRxBuf[6];
	pSlave->pTxBuf[7] = pSlave->pRxBuf[7];
	pSlave->pTxBuf[8] = ByCnt;
	
	for( i=0; i<WoCnt; i++ ){
		switch( Start + i ){
			case MODBUS_40089: Modbus_Heart.Rpt_Cnt_4xxxx++; break;
		}
		pSlave->pTxBuf[ 9+i*2] = HI_BYTE( pSlave->pReg_4x[Start+i] );
		pSlave->pTxBuf[10+i*2] = LO_BYTE( pSlave->pReg_4x[Start+i] );
	}
	
	*pSlave->pTxLen = 9 + ByCnt;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void MODBUS_TCP_Write_Single_Coil_Slave( MODBUS_TCP_SlaveHandleType *pSlave )
{
	U16		BiAdd = ((U16)pSlave->pRxBuf[ 8] << 8) | ((U16)pSlave->pRxBuf[ 9]);
	U16		BiVal = ((U16)pSlave->pRxBuf[10] << 8) | ((U16)pSlave->pRxBuf[11]);
	U16		RefBy = BiAdd / 8;
	U16		RefBi = BiAdd % 8;
	bool	Set;
	U16		i;
	
	if( BiAdd > (MODBUS_0xxxx_MAX * 8 - 1) ){
		dprintf( "  Maximum index of write single coil command has been exceeded.\n" );
		return;
	}
	
	if		( BiVal == 0xFF00 ){ Set =  true; }
	else if	( BiVal == 0x0000 ){ Set = false; }
	
	switch( RefBi ){
		case 0: pSlave->pReg_0x[RefBy].Bit._0 = Set; break;
		case 1: pSlave->pReg_0x[RefBy].Bit._1 = Set; break;
		case 2: pSlave->pReg_0x[RefBy].Bit._2 = Set; break;
		case 3: pSlave->pReg_0x[RefBy].Bit._3 = Set; break;
		case 4: pSlave->pReg_0x[RefBy].Bit._4 = Set; break;
		case 5: pSlave->pReg_0x[RefBy].Bit._5 = Set; break;
		case 6: pSlave->pReg_0x[RefBy].Bit._6 = Set; break;
		case 7: pSlave->pReg_0x[RefBy].Bit._7 = Set; break;
	}
	
	for( i=0; i<12; i++ ){
		pSlave->pTxBuf[i] = pSlave->pRxBuf[i];
	}
	
	*pSlave->pTxLen = 12;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void MODBUS_TCP_Write_Multiple_Coils_Slave( MODBUS_TCP_SlaveHandleType *pSlave )
{
	U16					Start = ((U16)pSlave->pRxBuf[ 8] << 8) | ((U16)pSlave->pRxBuf[ 9]);
	U16					BiCnt = ((U16)pSlave->pRxBuf[10] << 8) | ((U16)pSlave->pRxBuf[11]);
	//U16					ByCnt = pSlave->pRxBuf[12];
	U32					EndBi = Start + BiCnt;
	U16					RefBy;
	U16					RefBi;
	U8					SetBi;
	bool				Set;
	U16					i;
	
	if( EndBi > (MODBUS_0xxxx_MAX * 8) ){
		dprintf( "  Maximum index of write multiple coils command has been exceeded.\n" );
		return;
	}
	
	for( SetBi=0, i=Start; i<EndBi; i++, SetBi++ ){
		RefBy = i / 8;
		RefBi = i % 8;
		Set = pSlave->pRxBuf[13+SetBi/8] >> (SetBi & 7) & true;
		switch( RefBi ){
			case 0: pSlave->pReg_0x[RefBy].Bit._0 = Set; break;
			case 1: pSlave->pReg_0x[RefBy].Bit._1 = Set; break;
			case 2: pSlave->pReg_0x[RefBy].Bit._2 = Set; break;
			case 3: pSlave->pReg_0x[RefBy].Bit._3 = Set; break;
			case 4: pSlave->pReg_0x[RefBy].Bit._4 = Set; break;
			case 5: pSlave->pReg_0x[RefBy].Bit._5 = Set; break;
			case 6: pSlave->pReg_0x[RefBy].Bit._6 = Set; break;
			case 7: pSlave->pReg_0x[RefBy].Bit._7 = Set; break;
		}
	}
	
	for( i=0; i<12; i++ ){
		if( i == 5 ){
			pSlave->pTxBuf[i] = 6;
		}
		else {
			pSlave->pTxBuf[i] = pSlave->pRxBuf[i];
		}
	}
	
	*pSlave->pTxLen = 12;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void MODBUS_TCP_Write_Single_Register_Slave( MODBUS_TCP_SlaveHandleType *pSlave )
{
	U16		Add = ( (U16)pSlave->pRxBuf[ 8] << 8 ) | ( (U16)pSlave->pRxBuf[ 9] );
	U16		Val = ( (U16)pSlave->pRxBuf[10] << 8 ) | ( (U16)pSlave->pRxBuf[11] );
	U16		i;
	
	if( Add > MODBUS_4xxxx_MAX ){
		dprintf( "  Maximum index of write single register command has been exceeded.\n" );
		return;
	}
	
	switch( Add ){
		case MODBUS_40007:
		case MODBUS_40008:
		case MODBUS_40009:
		case MODBUS_40010:
		case MODBUS_40011:
		case MODBUS_40012:
		case MODBUS_40027:
		case MODBUS_40028:
		case MODBUS_40029:
		case MODBUS_40030:
		case MODBUS_40031:
		case MODBUS_40032:
			pSlave->pReg_4x[Add] = Val;
			break;
			
		default:
			dprintf( "  It's write protected address.\n" );
			return;
	}
	
	for( i=0; i<12; i++ ){
		pSlave->pTxBuf[i] = pSlave->pRxBuf[i];
	}
	
	*pSlave->pTxLen = 12;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void MODBUS_TCP_Write_Multiple_Registers_Slave( MODBUS_TCP_SlaveHandleType *pSlave )
{
	U16		Add = ( (U16)pSlave->pRxBuf[ 8] << 8 ) | ( (U16)pSlave->pRxBuf[ 9] );
	U16		Len = ( (U16)pSlave->pRxBuf[10] << 8 ) | ( (U16)pSlave->pRxBuf[11] );
	U16		i;
	U16		a;
	uuword	Buf;
	
	if( (Add + Len) > MODBUS_4xxxx_MAX ){
		dprintf( "  Maximum index of write multiple registers command has been exceeded.\n" );
		return;
	}
	
	for( i=0; i<Len; i++ ){
		a = Add + i;
		if( (a != MODBUS_40007) &&
			(a != MODBUS_40008) &&
			(a != MODBUS_40009) &&
			(a != MODBUS_40010) &&
			(a != MODBUS_40011) &&
			(a != MODBUS_40012) &&
			(a != MODBUS_40027) &&
			(a != MODBUS_40028) &&
			(a != MODBUS_40029) &&
			(a != MODBUS_40030) &&
			(a != MODBUS_40031) &&
			(a != MODBUS_40032) )
		{
			dprintf( "  It's write protected address.\n" );
			return;
		}
	}
	
	for( i=0; i<Len; i++ ){
		a = i * 2;
		Buf.b.h = pSlave->pRxBuf[13+a];
		Buf.b.l = pSlave->pRxBuf[14+a];
		pSlave->pReg_4x[Add+i] = Buf.w;
	}
	
	for( i=0; i<12; i++ ){
		if( i == 5 ){
			pSlave->pTxBuf[i] = 6;
		}
		else {
			pSlave->pTxBuf[i] = pSlave->pRxBuf[i];
		}
	}
	
	*pSlave->pTxLen = 12;
}


