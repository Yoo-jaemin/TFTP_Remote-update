#ifndef   __SENSOR_LIST_H__
#define   __SENSOR_LIST_H__


#ifdef __SENSOR_LIST_C__
	#define SENSOR_LIST_EXT
#else
	#define SENSOR_LIST_EXT extern
#endif

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
enum {
	IR_SEN_03_330,
	IR_SEN_04_850,
	IR_SEN_08_000,
	IR_SEN_09_440,
	IR_SEN_08_925,
	IR_SEN_03_720,
	IR_SEN_03_900,
	IR_SEN_03_950,
	IR_SEN_04_260,
	IR_SEN_04_520,
	IR_SEN_04_660,
	IR_SEN_09_076,
	IR_SEN_10_100,
	IR_SEN_03_370,
	IR_SEN_03_420,
	IR_SEN_09_150,
	IR_SEN_09_230,
	IR_SEN_09_790,
	IR_SEN_10_990,
	IR_SEN_MAX
};

enum {
	CT_SEN_UNDEFINED,
	CT_SEN_EC_DC_CL2_3,		// Negative
	CT_SEN_EC_DC_H2_2000,
	CT_SEN_EC_DC_HCL_15,
	CT_SEN_EC_DC_HF_9,		// Negative
	CT_SEN_EC_DC_O3_1,		// Negative
	CT_SEN_ETO_C_20,		// Negative
	CT_SEN_H2_C_20000,
	CT_SEN_H2O2_CB_100,
	CT_SEN_H2S_C_50,
	CT_SEN_HCN_C_100,
	CT_SEN_NH3_CR_200,
	CT_SEN_NO_CF_100,
	CT_SEN_NO2_C_20,		// Negative
	CT_SEN_PH3_C_5,
	CT_SEN_SIH4_CD_50,
	CT_SEN_SO2_C_20,
	CT_SEN_C2H4_C_1500,
	CT_SEN_CH2O_C_10,
	CT_SEN_CO_CF_200,
	CT_SEN_O2_MP_100,		// Negative
	CT_SEN_ASH3_3E_1,
	CT_SEN_COCL2_3E_1,
	CT_SEN_NAP_100AM,
	CT_SEN_GS_100A,
	CT_SEN_GS_100SC,
	CT_SEN_MAX
};
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
/* End Function */

/* Start Variable */
/* End Variable */


#endif // __SENSOR_LIST_H__


