#ifndef   __VERSION_H__
#define   __VERSION_H__


#define FirmwareVersion		(0.18f)


// REV0.00 (2023-07-26)
// UART, FLASH, GPIO, TIMER Driver 완료
// Modbus, Option 완료

// REV0.01 (2023-09-21)
// I2C Driver 추가
// HS300x 추가

// REV0.02 (2023-09-26 오전 10:49:39)
// ADC Driver 추가
// Hardware Version Sensing 추가
// Flash, ADC Error Check 추가
// Sensing Process & Measuring 추가

// REV0.03 (2023-10-13 오후 4:05:05)
// 측정데이터 MODBUS Mapping
// Sensing Process ADC Buff Size 추가 (Cavity 연동)
// MODBUS 40001 & 40002 쓰기 금지 적용
// Flash Task 및 DMA Write Delay 1msec 추가
// DAC, TIA Driver 추가
// Sensing Process Hardware Config 추가

// REV0.04 (2023-10-20 오전 10:56:12)
// Measuring Data Mapping 오류 수정
// Hardware Config 기본값 변경 (Sensing 범위 300 ~ 1900 mV 적용)
// SensingProcess Fifo 초기값 설정 (Hp, Tp)
// Sensor Type 변경 시 Hardware Config 적용
// ADC Debug I/O, 변수 삭제

// REV0.05 (2023-10-20 오후 5:43:19)
// RawDvMax Item Modbus Map 삭제 (TEMP, AVDD, DVDD, VCB, ICB0, ICB1)
// Watchdog 추가
// Hardware Config 삭제 (MODBUS Mapping으로 컨셉 변경)

// REV0.06 (2023-10-27 오전 8:46:28)
// Sensing Config Flash Address 변경

// REV0.07 (2023-10-30 오전 9:38:50)
// SYSTEM_FLASH_DEFAULT 수정

// REV0.08 (2023-11-15 오후 5:48:42)
// ADC Driver 수정 (Mux Setting Time 50usec 적용)
// SYSTEM_FLASH_DEFAULT 수정 (xZero: 0.4[V], xSpan: 1.8[V])

// REV0.09 (2023-11-21 오후 6:34:27)
// Measuring Data 변경 (RawDvMax -> RawDvSum)

// REV0.10 (2023-11-23 오전 9:23:14)
// RFILTER 초기값 변경 (LPTIA_RFILTER_1M -> LPTIA_RFILTER_20K)
// ADC Item 추가 (VCE, VRE)
// Measuring Data 추가 (PeakDv)
// Sensor Output Average 수정 (Min & Max 제외)
// Modbus Remap

// REV0.11 (2023-12-21 오후 6:46:34)
// Sensing Process DAC Reset 추가

// REV0.12 (2024-02-21 오후 2:30:12)
// Modbus Remap

// REV0.13 (2024-02-23 오후 1:24:32)
// Default TIA 수정 (RLOAD = 0, RFILTER = DISCONNECT, RGAIN = DISCONNECT, SWMODE = OFF)

// REV0.14 (2024-03-04 오후 6:21:21)
// VCE, VRE 삭제
// PeakDv 구조 변경 (ADC_RawType 적용)

// REV0.15 (2024-06-18 오후 7:15:09)
// Dual Mode Detection Pin 변경 (P0.1 -> P0.0)

// REV0.16 (2024-07-01 오후 2:04:12)
// Model Option 적용

// REV0.17 (2024-07-03 오후 4:28:01)
// Model Enabe Pin 초기화 오류 수정

// REV0.18 (2024-07-10 오후 1:34:52)
// 5200x Dual Mode Option 추가



#ifdef __VERSION_C__
	#define VERSION_EXT
#else
	#define VERSION_EXT extern
#endif

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
/* End Struct */

/* Start Function */
VERSION_EXT void System_Ver_Load( void );
/* End Function */

/* Start Variable */
VERSION_EXT float		HardwareVersion;
/* End Variable */


#endif // __VERSION_H__


