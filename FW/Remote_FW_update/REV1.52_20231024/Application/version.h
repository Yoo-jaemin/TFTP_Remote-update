#ifndef   __VERSION_H__
#define   __VERSION_H__


#define FirmwareVersion		(1.52f)

// REV0.0 (2018-11-12 오후 5:48:05)
// 시작

// REV0.1 (2018-11-20 오후 8:48:05)
// MODBUS 적용
// 5Hz, 50% Duty 시험용   

// REV0.2 (2018-11-23 오후 3:40:55)
// HOLDING REGISTER MAP 정리중...    
// IR Configuration 적용  

// REV0.3 (2018-11-29 오후 3:39:51)
// Sensor Offset Drift 보상 로직 추가
// EEP_SYS_MODBUS_TIMEOUT_TICK_100uS 추가 (0.1[%] 단위로 설정) 
// EEP_CRC, EEP_PREFIX 접근 시 Fault 처리 하지 않고 값만 변경하지 않도록 수정
// USART3_RX_BUFF_SIZE 변경 (50 ->255)
// MODBUS Input Register에 IR Raw Data 추가 및 채널 선택을 위한 Holding Register 추가
// MODBUS Input Register Size 변경 (500->320)
// MODBUS Address 설정 추가
// MODBUS Input Register Sequence Number Count Error 수정
// Event Log 추가 

// REV0.4 (2018-11-30 오후 8:42:40)
// MODBUS Timeout 처리 추가 

// REV0.5 (2018-11-30 오후 8:42:40)
// Filtering Logic 제거
// Event Log 제거
// Amp. Calibration Offset & Gain Margin 설정 가능하도록 변경 
// Internal ADC interrup disable
// MODBUS Timeout Process 수정
// MODBUS Wave Output Logic 추가

// REV0.6 (2018-12-10 오후 2:37:38)
// MODBUS 음수데이터 버그 수정    
// Amp. Calibraion Offset Compensation Logic 추가
// MODBUS Input Register Map 변경 (Amp. Calibration Status 추가)
// EEPROM_Task() 추가 

// REV0.7 (2018-12-13 오후 10:37:38)
// 내부 ADC Conversion 보상로직 추가

// REV0.8 (2019-01-24 오후 5:34:29)
// EEPROM REMAP
// Amp Cal Logic 수정

// REV0.9 (2019-04-01 오전 10:42:21)
// Temperature Resolution 변경 (0.0 -> 0.00)
// EEPROM REMAP (EEP_IRn_AMP_CAL_REFERENCE_100uV)
// Warm-up 시간 변경(20sec -> 60sec)
// Warm-up 시 Peak to Peak는 Zero Cal 값으로 전송되도록 변경

// REV1.0 (2019-05-17 오전 9:04:05)
// Warm-up 시 Peak to Peak 측정값으로 전송되도록 변경

// REV1.1 (2019-06-08 오전 10:34:48)
// Data Type 변경(word -> float, EEPROM Remap)
// Offset Drift Check Disable
// 2019-06-18 오후 1:22:31 이후 온도분석

// REV1.2 (2019-06-25 오후 4:24:49)
// Offset Drift Check Logic 삭제
// Default Offset Value '128' ( Amp Cal 시작 시.. )

// REV1.30 (2019-11-25 오전 8:32:25)
// Version Format 변경
// Eep Fifo 확장
// AT24C32 Write Fault 체크 되도록 수정
// AT24C32 Read, Write Function 수정
// Amp Cat Target 초기값 3.8[V]로 변경

// REV1.31 (2019-11-26 오후 12:01:04)
// H/W Version Format 변경
// System Error Debug Bit 추가
// EEPROM CRC Write 횟수 축소
// Application Offset 적용

// REV1.32 (2019-12-16 오후 4:17:38)
// I2C Driver 수정

// REV1.33 (2019-12-18 오전 9:45:57)
// Offset Potentiometer Limit 변경
// System Clock Config 수정
// 삼성 데모 버전 (2019-12-19, PoE: 4SET, Standard: 2SET)

// REV1.34 (2019-12-19 오후 4:07:50)
// I2C Clock Speed 변경 (300KHz -> 100KHz)

// REV1.35 (2019-12-27 오전 9:04:46)
// AT24C32_WriteWord Logic 수정
// IR Source PWM Frequency(10Hz) & Duty(50%) Fix
// IR Processing Logic 수정
// LNG, COS P.P. 버전 (2019-12-31 배포)

// REV1.36 (2020-01-07 오후 7:40:25)
// I2C ReInit 추가

// REV1.37 (2020-01-09 오전 9:23:31)
// EEPROM Trouble 수정 (AT24C32_WriteWord)

// REV1.38 (2020-02-05 오전 11:14:39)
// I2C Driver 수정
// TLP0102 Driver 수정
// AT24C32 Driver 수정
// Amp Cal Error 개선

// REV1.39 (2020-02-06 오후 2:30:26)
// Sensor 추가 (LMM-274-X027)

// REV1.40 (2020-03-02 오전 11:43:23)
// EEPROM Write delay 변경 (5 -> 7msec)
// Wave Viewer 용 데이터 추가
// Modbus Wave Map 채널별 분리

// REV1.41 (2020-07-14 오후 1:07:05)
// Amp Cal Target Voltage 기본값 변경
// Amp Cal Target Voltage EEPROM 적용
// Offset PTM Wiper Limit 변경

// REV1.42 (2020-09-10 오전 10:37:36)
// Sensor Type 확장 및 추가 (Rotary 16 position 적용)

// REV1.43 (2020-11-19 오후 12:17:06)
// Amp Cal Handler PTM Write Logic 개선

// REV1.44 (2020-12-08 오후 2:42:16)
// Dual Mode 적용

// REV1.45 (2021-02-03 오후 1:52:53)
// Sensor Type 변경 및 추가(LIM-272-X022 -> LIM-272-X054, LMM-274-X033)

// REV1.46 (2021-08-06 오후 4:59:49)
// Amp Cal Logic 개선
// - GAIN 설정 시 Saturation 방지
// - Gain Tick 변경 (Target 80% 이하에서 2씩 증가)

// REV1.47 (2021-11-18 오전 10:39:01)
// Sensor Type 추가(LMM-274-X040, LMM-274-X042)

// REV1.48 (2022-04-13 오전 9:25:06)
// ADC 오차 개선 (Sampling Time 변경: 28 -> 239 Cycles)

// REV1.49 (2022-10-31 오전 9:19:39)
// ADS868x Channel Range Select Register 초기화 감지 채널 확장 (CH0 -> CH0~7)
// System State Mapping 분리
// Detector Type 추가 (GSA-972)
// 모델별 IR Source Frequency 가변 적용
// IR 신호 Hi, Lo 판별 기준값 변경 (Offset 전압 -> A/D값)

// REV1.50 (2022-12-15 오후 1:20:12)
// Option Load Data 초기값 적용

// REV1.51 (2023-03-20 오전 10:54:23)
// Sensor 추가 (LMM-274-X044)

// REV1.52 (2023-10-24 오후 1:42:57)
// Sensor 추가 (LMM-272-X127)



#endif // __VERSION_H__


