#ifndef   __DISPLAY_H__
#define   __DISPLAY_H__


#ifdef __DISPLAY_C__
	#define DISPLAY_EXT
#else
	#define DISPLAY_EXT extern
#endif

/* Start abbreviation */
/* End abbreviation */

/* Start Include */ 
#include "common.h"
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
typedef enum
{
	nNONE_SENSOR_FW,
	nNONE_MAIN_FW,
	nRESPONSE_ERR,
	nINDICATOR_MAX
}DisplayIndicator;
/* End Enum */

/* Start Struct */
typedef union{
  	struct {
	  	bool	bComplete	:1;
		bool	bFail		:1;
		bool	bProgress	:1;
		bool	bEmpty3		:1;
		bool	bEmpty4		:1;
		bool	bEmpty5		:1;
		bool	bEmpty6		:1;
		bool	bEmpty7		:1;
	}Bit;
	U8	All;
}Upload_Display;
/* End Struct */

/* Start Function */
DISPLAY_EXT void Title_Display(void);
//DISPLAY_EXT void Menu_Display(U8 ucMenupos);
DISPLAY_EXT void MenuSelect_Display(U8 ucMenu_pos);
DISPLAY_EXT void StatusBox_Display(void);
DISPLAY_EXT void Status_Display(void);
DISPLAY_EXT void UploadStatus_Display(U8 ucSelect_menu);
DISPLAY_EXT void Show_Display(DisplayIndicator nDisplay);
DISPLAY_EXT void Display_Off(void);

DISPLAY_EXT void Remote_Title_Display(void);
DISPLAY_EXT void Remote_Status_Display(void);
/* End Function */

/* Start Variable */
DISPLAY_EXT U8 ucDisplay_Progress;
DISPLAY_EXT Upload_Display Display;
/* End Variable */


#endif // __DISPLAY_H__