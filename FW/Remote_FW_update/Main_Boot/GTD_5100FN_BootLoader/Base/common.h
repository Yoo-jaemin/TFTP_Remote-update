#ifndef   __COMMMON_H__
#define   __COMMMON_H__

#include <stdio.h>
#include <string.h>
#include <stdint.h>
                 
/* Start abbreviation */
typedef unsigned char				U8;
typedef	signed char					S8;
typedef	unsigned short				U16;
typedef	signed short				S16;
typedef	unsigned long				U32;
typedef	signed long					S32;
typedef	unsigned long long			U64;
typedef	signed long long			S64;

typedef unsigned char				bool;
/* End abbreviation */
                     
/* Start Include */
#include "stm32f4xx_hal.h"
#include "stm32f4xx_it.h"
#include "stm32f4xx.h"
#include "main.h"
/* End Include */

/* Start Define */
#define _DEBUG_BUILD
#define _FLASH_CHECK
#define _PACKET_CHECK

#define FirmwareVersion		(0.7f)

// REV0.0 (2019-10-23)
// App DownLoad 및 Start 확인 완료
// SD to Main UI 완료.
// UI 전면 수정
// UI 완료 (2019-11-01)
// REV0.1 (2019-11-04)
// IR Sensor Module Upload 진행 (2019-11-04)
// IR Sensor Upload Test 완료 (2019-12-03)
// 보완 작업 및 Display 방식 변경 필요
// Key 입력 없을 시 바로 시작 수정 (2019-12-04)
// LCD Backlight 수정 (2019-12-19)

// REV0.2 (2019-12-24)
// 요청/응담에 의한 App Start 추가
// Sensor Bios Version 확인 추가
// REV0.3 (2019-12-27)
// UI 수정 ( F/W Version 표시, Upload 완료 표시)
// (2020-01-03)
// Main F/W가 있어야 Sensor 동작 Packet Send
// Key 입력 없을 경우에는 Sensor 응답 상관없이 바로 시작하게끔 수정 

// REV0.4 (2023-04-12)
// Cartridge model 검증 진행
// CRC 확인
// Receive Data 확인
// Sensor Version Check  &  App Start Time 10 -> 500ms 
// 전원 On -> 응답 상관없이 바로 시작
// Boot Mode에서 시작 -> 응답 확인 후 시작
// (2023-04-17)
// Sensor_AppStart ack, nak 초기화 진행.
// Cartridge Flash Meory Set 0x4000 - 0x1FFFF (110K)
// Sensor_UploadStart() -> Start Add 구분 (IR / CT)  
// (2023-04-18)
// Sensor_Upload() -> Start Add 구분 (IR / CT)
// (2023-04-19)
// NAK Packet 처리 수정 -> ACK = true 면 NAK = false
// (2023-04-20)
// Sensor Version Check(), Sensor App Start() 합치는 구조로 변경

// REV0.5 (2023-05-31)
// Portable Type Check 추가
// Portable Type Check -> Portable Type 인 경우 SHDN Pin On (Sensor Power Control)
// Charge LED -> Open Drain Mode & Set
// App Jump -> BackLight Off & SHDN Pin Off


// REV0.6 (2023-08-14)
// EXIT 및 Reset Key -> Sensor FW 상관없이 Main Start (Main FW 있을 경우만)
// Cartridge Sensor Memory 재설정
// BOOT 0x0000 - 0x3FFF (16K) / EEPROM 0x4000 - 0x7FFF (16K) / App 0x8000 - 1FFFF (96K)

// REV0.7 (2023-08-14)
// Cartridge Portable model 응답시간 늦음
// SHDN ON 이후 100ms 부터 응답 가능
// Delay 20ms -> 150ms로 수정 ( Cartridge & IR 응답대기 1초 )
            
#ifndef true  
  #define true						1
#endif

#ifndef false  
  #define false						0
#endif

#ifndef TRUE  
  #define TRUE						1
#endif

#ifndef FALSE  
  #define FALSE						0
#endif

#ifndef NULL
  #define NULL						(void *)0
#endif

#define BOOT_ADDR	0x08000000U
#define MAIN_ADDR	0x08020000U
#define PAGE_SIZE	(unsigned short)0x400
#define MAX_BUFF	0x200

#define IR_START_ADDR		0x08010000U
#define IR_END_ADDR			0x0803FFFFU
#define IR_FLASH_SIZE   (IR_END_ADDR - IR_START_ADDR)

#define CT_START_ADDR		0x00008000U
#define CT_END_ADDR			0x0001FFFFU
#define CT_FLASH_SIZE   (CT_END_ADDR - CT_START_ADDR)

#define EXIT		0x01
#define MAIN_UPLOAD 0x02
#define IR_UPLOAD	0x04
#define CT_UPLOAD	0x08 
#define BOOT_MODE	0x20

#define HI_BYTE(x)					((U8)( x >> 8 & 0xFF ))
#define LO_BYTE(x)					((U8)( x      & 0xFF ))

/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
typedef union {
    struct{
		U16		l: 16;
        U16		h: 16;
    }w;
	float		f;
}ufloat;

/* End Struct */

/* Start Function */ 
#ifdef _DEBUG_BUILD
	#define dprintf( ... )			printf( __VA_ARGS__ )
#else                                                    
	#define dprintf( ... )
#endif
/* End Function */

/* Start Variable */

/* End Variable */


#endif // __COMMMON_H__
