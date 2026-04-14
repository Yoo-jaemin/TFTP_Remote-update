#ifndef   __VIMAGE_H__
#define   __VIMAGE_H__


/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "common.h"
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
typedef struct {
	void	(*AlarmArrow)(bool, uint16_t, uint16_t, uint16_t);
	void	(*Disk)(uint16_t, uint16_t);
	void	(*ComStatus)(uint16_t, uint16_t, bool);
	void	(*Intro)(uint16_t, uint16_t);
	void	(*Flow)(uint16_t, uint16_t, float);
	void	(*iSD)(uint16_t, uint16_t);
	void	(*nSD)(uint16_t, uint16_t);
	void	(*Filter)(uint16_t, uint16_t);
	void	(*iGas)(uint16_t, uint16_t);
	void	(*Normal)(uint16_t, uint16_t);
	void	(*Bat)(uint16_t, uint16_t, U8, bool);
}vImage_DrawType;
/* End Struct */

/* Start Function */
/* End Function */

/* Start Variable */
extern vImage_DrawType		vDrawImg;
/* End Variable */


#endif /* __VIMAGE_H */


