#ifndef   __IO_CTRL_H__
#define   __IO_CTRL_H__

#ifdef __IO_CTRL_C__
	#define IO_CTRL_EXT
#else
	#define IO_CTRL_EXT extern
#endif

/* Start abbreviation */
/* End abbreviation */

/* Start Include */ 
#include "common.h"
#include "Task.h"
/* End Include */

/* Start Define */
#define KEYCHECK_TIME 30

#define SWITCH_ON 0
#define SWITCH_OFF 1

//RST <-> Up key port ¹Ù²ñ 
#define FUCN_KEY_ON 0x01
#define RST_KEY_ON 	0x02 
#define UP_KEY_ON	0x04 
#define DOWN_KEY_ON	0x08

/* End Define */

/* Start Enum */

typedef enum {
	nMAIN_UPLOAD,
	nCT_SENSOR_UPLOAD, 
	nIR_SENSOR_UPLOAD,
	nEXIT,
	nUI_MENU_MAX,
}UI_MENU;

typedef enum {
	nFUNC,
	nRST,
	nUP,
	nDOWN,
	nKEY_MAX,
}KeyMenu;
/* End Enum */

/* Start Struct */
typedef union{
  	struct {
	  	bool	bRead_Func		:1;
		bool	bRead_RST		:1;
		bool	bRead_Up		:1;
		bool	bRead_Down		:1;
		bool	bOperate_Func	:1;
		bool	bOperate_RST	:1;
		bool	bOperate_Up		:1;
		bool	bOperate_Down	:1;
	}Bit;
	U8	All;
}Key_Input;

typedef union{
  	U8	ucPosition;
  	struct {
	  	bool	bPosition0	:1;
		bool	bPosition1	:1;
		bool	bPosition2	:1;
		bool	bPosition3	:1;
		bool	bEmpty		:1;
		bool	bFucn		:1;
		bool	bRST		:1;
		bool	bFlag		:1;
	}Bit;
}Key_Set;

typedef struct {
  	U16	uCheck_timer[nKEY_MAX];
	Key_Set Setting;
  	Key_Input Check;
}Key_Handle;

/* End Struct */

/* Start Function */
IO_CTRL_EXT void Key_Check(void);
IO_CTRL_EXT void Key_Operation(MainStatus* Status);
/* End Function */

/* Start Variable */
IO_CTRL_EXT Key_Handle Key;
/* End Variable */


#endif // __IO_CTRL_H__