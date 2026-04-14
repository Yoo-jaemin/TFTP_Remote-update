#ifndef   __BUZZER_H__
#define   __BUZZER_H__


#ifdef __BUZZER_C__
	#define BUZZER_EXT
#else
	#define BUZZER_EXT extern
#endif 

/* Start abbreviation */
/* End abbreviation */

/* Start Include */ 
#include "common.h"
/* End Include */

/* Start Define */
#define BUZZER_RATED_FREQ		(3200.f)
/* End Define */

/* Start Enum */
typedef enum {
	BUZ_STATE_NOT_PRESENT,
	BUZ_STATE_STOP,
	BUZ_STATE_TONE,
	BUZ_STATE_NOTE,
	BUZ_STATE_MAX_POWER
}Buzzer_StatusType;

typedef struct {
	U16		Freq;
	U16		mSec;
}Buzzer_NoteType;

typedef struct {
	void	(*AL1)(void);
	void	(*AL2)(void);
	void	(*Max)(void);
}Buzzer_NoteFunc;
/* End Enum */

/* Start Struct */
typedef struct {
	Buzzer_StatusType	Status;
	
	U16					Volume;
	
	Buzzer_NoteType		*pNote;
	U16					NoteLength;
	bool				IsRepeatMode;
	
	Buzzer_NoteType		*pTone;
	U32					ToneDuration;
	U16					ToneCnt;
	
	Buzzer_NoteFunc		Melody;
}Buzzer_DriveType;
/* End Struct */

/* Start Function */
BUZZER_EXT bool Buzzer_Init( void );
BUZZER_EXT void Buzzer_Drive( void );
BUZZER_EXT void Buzzer_Volume_Set( U8 Volume );
BUZZER_EXT void Buzzer_PlayTone( float Freq, U8 Volume, U16 mSec );
BUZZER_EXT void Buzzer_PlayNote( const Buzzer_NoteType *pNote, U16 NoteLength, bool IsRepeat );
BUZZER_EXT void Buzzer_Stop( void );
/* End Function */

/* Start Variable */
BUZZER_EXT Buzzer_DriveType			BuzDrive;
/* End Variable */


#endif // __BUZZER_H__


