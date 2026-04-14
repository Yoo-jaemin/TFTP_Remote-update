#ifndef   __VERSION_H__
#define   __VERSION_H__


#define FirmwareVersion		(3.93f)


// REV0.0 (2018-11-16 오후 5:48:05)
// LED, DEBUG UART, RTC, TFT, NAND 검증 완료 

// REV0.1 (2018-11-26 오후 11:10:28)
// Cavity Interface 추가 

// REV0.2 (2018-12-02 오후 1:18:40)
// EEPROM, KEY 추가
// Ratary Switch Option 추가 

// REV0.3 (2018-12-04 오후 9:57:25)
// Project Tree Renewal
// Internal ADC Sensing 추가
// Pump Control 추가
// Sensing Data Processing 추가

// REV0.4 (2018-12-08 오후 7:23:12)
// Sensing Data Processing Logic 수정
// Data Logging Process 수정
// Reset Long-Key 동작 시 Cavity Amp. Calibration 수행
// Function Long-Key 동작 시 Log Mode 순차적으로 변경

// REV0.5 (2018-12-13 오후 10:37:38)
// Nand Flash 추가
// Sensing Data Processing Logic 수정   

// REV0.6 (2018-12-16 오전 11:27:10)
// EEPROM Pump Frequency Map 추가
// Pump Control Logic 수정
// I2C2 추가
// USART Handle 구조 변경 및 USART6 External MODBUS Interface 추가
// Current Output 추가    

// REV0.7 (2018-12-18 오후 1:14:40)
// MODBUS Process 수정     

// REV0.8 (2018-12-19 오후 12:48:44)
// H/W Revision Read Logic 추가
// I2C 구조 변경
// Base Timer 구조 변경 (Timer Interrupt Vector 여유시간 확보)
// DAC8571 Power Up Delay Nack Fault 방지를 위한 Delay(1msec.) 추가
// Task Process Normal과 Time Synchronization 영역으로 분리
// Cavity Init. Fuction Sensing Process에서 처리 

// REV0.9 (2018-12-22 오전 9:43:43)
// Digital(I2C) Pressure Sensor 적용 
// Key Task - Reed Switch Port 분리 적용
// GDI, SDIO, FatFs 추가
// System Mode 적용 

// REV1.0 (2019-01-03 오전 11:26:37)
// Boot Logic 설계
// Pump Flow Control 추가
// Function Mode 추가 

// REV1.1
// GUI Normal View Design 2Ch 완료 등..... 

// REV1.2 (2019-02-14 오전 10:59:15)
// LCD Drive Logic 수정
// Function Task 추가
// Flow Calibration 추가 

// REV1.3 (2019-02-28 오후 1:56:33)
// Mesurement Setting 추가 

// REV1.4 (2019-03-11 오전 9:17:22)
// Gas Calibration Logic 추가
// Alarm Logic 추가 

// REV1.5 (2019-03-26 오후 12:49:17)
// Sensig Logic 수정
// Key Short Check 추가
// COS Trend 분석용

// REV1.6 (2019-03-27 오전 10:48:45)
// Sensig Logic 수정
// Cavity Sensor Temperature Lpf 제외

// REV1.7 (2019-04-01 오전 10:42:51)
// Temperaure Resolution 변경 (0.0 -> 0.00)
// Cavity EEPROM Remap (SENSOR_EEP_CHn_AMP_CAL_TARGET_100uV)
// Flow Rate Set Logic 수정
// Calibration Logic 수정

// REV1.8 (2019-05-20 오후 3:14:08)
// Calibration Logic 수정
// Power Sensing Scale 변경 ( 59[V] Max )

// REV1.9 (2019-05-31 오후 5:56:19)
// CH4 Demo용 긴급프로젝트 시작...
// Cavity 입고일정 6/10.. 데모 10SET 조립완료 일정 6/12..
// 18일 안에 측정정확도, 측정가스와 간섭조건, 온도보상을 위한 모든 데이터를 수집하고 알고리즘 개발을 완료한다...

// REV2.0 (2019-06-07 오후 1:44:44)
// LwIP 추가
// POE & System Configuration Function 추가
// Boot Process 수정
// LNG 100%LEL & Auxi 9.440μm & 8.000μm 동일 a, n 적용
// 2019-06-18 오후 1:22:31 이후 온도분석

// REV2.1 (2019-06-18 오후 3:21:17)
// Measuring Logic 수정

// REV2.2 (2019-06-21 오후 1:10:13)
// Filtering Logic 추가
// Voltage per Temperature 기본값 설정 (LNG 100%LEL = 7.5mV, COS 100PPM = 13mV)

// REV2.3 (2019-06-25 오전 11:20:50)
// Filtering Parameter 추가
// 간섭가스 필터링 4.85μm 제거

// REV2.4 (2019-06-26 오후 7:38:12)
// MODBUS TCP 추가

// REV2.51 (2019-06-28 오전 11:50:49)
// Measuring Fault 조건 수정
// 온도보상 로직 제거 (vpt = 0.f)
// Sensing Process Time Sync Task로 변경
// IR 계산 실시간으로 변경
// Output Gas 적용

// REV2.52 (2019-07-01 오후 12:38:01)
// Base Zero 추가 (Volume Deviation 조건 검토 필요)
// IR Source Power Check 추가
// Gain Calibration Reference Voltage 변경 (4.2[V] ->3.8[V])

// REV2.53 (2019-07-03 오전 10:14:11)
// 온도보상 적용
// LNG DEMO 최종점검

// REV2.54 (2019-07-05 오후 2:33:31)
// MODBUS Remap (Rev0.2)
// 4.85μm vpt 변경 (8.5mV -> 10.9mV)

// REV2.55 (2019-07-08 오전 8:34:27)
// LNG 긴급데모 완료
// Filtering 시정 수 최종 값 반영

// REV2.56 (2019-07-08 오후 2:40:46)
// 485버전 재시작
// Current Output DAC 16-BIT 적용 (DAC7571 -> DAC8571, H/W Revision 후 재진행 예정)
// Engineer Mode -> Factory Mode
// Alarm Setting 추가
// Flow Calibration Set Frequency Limit 적용 (50 ~ 250 Hz)
// Function mode에서 watchdog 10초 적용
// Remote Test Mode 추가
// Engineer Mode 추가

// REV2.57 (2019-08-06 오후 3:58:14)
// RS-485 Function Menu 추가

// REV2.58 (2019-08-09 오전 11:39:34)
// Changed a lot...

// REV2.59 (2019-09-11 오후 4:54:38)
// Modbus Status Icon 추가
// Flow Icon Speed 적용

// REV2.60 (2019-09-18 오전 10:54:22)
// Factory Settings...
// Measuring Option 추가
// Down Long Key Maintenance Mode 진입 추가
// Calibration Logic 수정

// REV2.61 (2019-09-20 오후 3:05:08)
// Factory Settings...
// Maintenance Mode 추가

// REV2.62 (2019-09-24 오전 9:17:20)
// Base Zero 조건 변경

// REV2.63 (2019-09-24 오후 4:19:00)
// Flow Control Logic 수정 (Freq -> Freq & Duty)
// Gain Setting Factory Cali Menu로 이동

// REV2.64 (2019-10-17 오후 5:30:24)
// Measuring 구조 변경
// Function - Flow Trouble Check Time 추가
// Remote Test Set True만 처리 되도록 수정
// Auto Calibration 제거
// Factory Calibration 추가
// mA Adjustment Resolution 변경 (10uA -> 1uA)
// Measuring Config 수정 (IR Dual 적용)
// Cross Sensitivity 추가

// REV2.65 (2019-11-07 오후 8:44:55)
// Span Calibration Menu에 Cross Sensitivity 적용

// REV2.66 (2019-11-08 오후 8:40:26)
// Measuring Gas Channel Configuration Logic 변경
// Measuring Fault Logic 추가
// Modbus Remap REV0.6
// Relay Test Mode & Energized Set Logic 수정
// mA Set & Test 모드에서 최대설정치 적용되도록 수정

// REV2.67 (2019-11-13 오후 12:51:55)
// Error Scan & Decoding Logic 추가
// Cal Mode에서 Measuring Filter 제거
// SD Icon -> Status Icon으로 변경
// Key Lock Mode 추가

// REV2.68 (2019-11-21 오후 2:00:46)
// Current Output Init Value Maintenance Gas 적용
// Measuring Filter 조건 변경 (Random Noise & ROC Great)
// Maintenance Level Update Length 오류 수정

// REV2.69 (2019-11-22 오전 10:26:00)
// AT24C32 Write Fail Check 되도록 수정
// Sensing Process 수정

// REV2.70 (2019-11-25 오후 8:39:31)
// Sensing Process 수정
// Status Message 추가
// Peripheral Error Code 1로 정의
// Factory System Init 추가
// Factory System Info 추가
// Function Quit Timer 추가
// P.P. 예정

// REV2.71 (2019-12-13 오전 11:40:43)
// Status Message 수정
// Max Flow 수정
// Canlendar Setting & View Logic 수정
// Backlight Control 변경
// Gain Setting Status 적용

// REV2.72 (2019-12-16 오후 4:18:32)
// vMeasuring Color Set Logic 수정
// 간섭가스 아이콘 추가

// REV2.73 (2019-12-19 오전 11:41:17)
// Measuring Config 진입 시 기본값 로드
// Measuring Config Update Logic 수정
// Maintenance Logic 수정 (Relay Output Disable, Test Mode 진입불가, mA Adjustment 불가)
// mA Maintenance Level 적용
// 1채널 모드에서 mA Output 채널2 0 mA 출력되도록 수정

// REV2.74 (2019-12-19 오후 5:28:11)
// MODBUS Write Holding Logic 추가 (Alarm Set Point)
// 삼성 데모 버전 (2019-12-20, Standard: 1SET)

// REV2.75 (2019-12-20 오전 9:48:05)
// I2C Clock Speed 변경 (300KHz -> 100KHz)
// Measuring Option 관련 버그 수정
// Sensor Trouble 추가 (Not Stable)

// REV2.76 (2019-12-23 오후 7:33:07)
// Measuring Filter 조건 수정
// Self-Test 조건 수정
// Boot..... 수정
// Measuring Status 추가

// REV2.77 (2019-12-26 오후 7:30:05)
// Measuring Status 수정
// AT24C32_WriteWord Logic 수정
// LNG, COS P.P. 버전 (2019-12-31 배포)

// REV2.78 (2019-12-31 오후 12:33:33)
// Factory_Sensor_Set_Init() 수정

// REV2.79 (2020-01-06 오전 9:46:58)
// I2C ReInit 추가
// Modbus 1xxxx 번지 채널 3, 4 Reseved 추가
// FlowSet Gain Lower Limit 적용 (x21)

// REV2.80 (2020-01-09 오전 9:18:44)
// AT24C32_WriteWord Logic 수정
// Measuring Config Update 시 Output Channel 반영안되는 문제 수정

// REV2.81 (2020-01-09 오후 2:39:35)
// Engineering Mode에서 Base Zero 수행되도록 수정
// High Scale 변경 시 Span Gas, Alarm Level도 비율 유지하여 같이 변경되도록 수정
// Alarm Level 기본값 변경
// Manual Base Zero 추가

// REV2.82 (2020-01-15 오후 8:15:58)
// 간섭가스 아이콘 제거
// Warm-up Time 변경 (900 sec.)
// Enter_Self_Test_Mode() 실행 시 Test Event True
// Flow Cal Logic 수정
// Flow Cal Threshold 변경 (Target Voltage, Zero Reference = 300mV, Target Gain Max = 201, Zero Margin = 50mV)
// Max Flow 변경 (1000 mL/min)
// Flow Control Threshold 변경 (Freq = 180 - 60Hz, Duty = 20 - 30% )
// Flow Rate 기울기 실시간 계산으로 변경
// Flow Rate 80mL/min 이하 Low 표시, 1080 이상 High 표시
// Base Zero Default 변경 (Time = COS만 60초 나머지는 120초)
// a, n 변경 (LNG, COS)

// REV2.83 (2020-01-30 오후 5:30:08)
// Flow Set Exceed Ptm Range 추가
// Flow Filtering 기준 변경 (<80% of Target, >120% of Target)
// Flow Trouble   기준 변경 (<80, >1200)
// 간섭가스 검출 시 Filter Icon 표시

// REV2.84 (2020-02-03 오후 1:29:26)
// Key Lock 표시방법 변경 (Status Icon -> Status Message)
// Under Gas mA Output Level 변경 (2mA -> 0mA)
// I2C Driver 수정
// DAC8571 Driver 수정
// MCP46x1 Driver 수정
// AT24C32 Driver 수정
// bZero Logic 수정 (범위 내 1도 차이 날 경우 실행)

// REV2.85 (2020-02-06 오후 5:14:17)
// Sensor 추가 (LMM-274-X027)
// Filter 추가 (8,925nm)
// Gas 추가 (C4F8 2000ppm, C5F8 2000ppm, C4F6 2000ppm, CH2F2 2000ppm, TEOS 30ppm, CH3F 2000ppm)
// Gas Cal Low Zero to Sppn 기준 변경
// Cal Status 변경 (실제 수행 모드에서만 SET, 가스선택, Info Viewer 모드 등 RESET)
// 활성화 센서만 가스값 계산
// Log Mode 추가 (DATA_LOG_MODE_ZERO_STABILITY)
// Normal 모드 최초 진입시 Key Lock 해제, Gas Percentage Min, Max Log 초기화
// Normal 모드에서 Up Long Key로 Gas Percentage Min, Max Log, bZeroCnt 초기화
// 4850nm VPT 수정 (7.3mV)
// Modbus Heart Top 65535로 수정
// Gas Simulation에서 Modbus Trouble Bit 연동
// Gas Simulation Range 변경 (0 ~ HIgh Scale * 1.2)
// Gas Simulation Under Set 제거
// Gas Simulation Trouble Set 추가
// Gas 6종 Curve Data 적용 (C4F8 2000ppm, C5F8 2000ppm, C4F6 2000ppm, CH2F2 2000ppm, TEOS 30ppm, CH3F 2000ppm)
// Default Span Gas 적용

// REV2.86 (2020-02-14 오후 5:28:52)
// Base Zero Off 설정 추가
// FlowSet Logic 수정 (PTM 공용화, Zero Span Decision 기준)

// REV2.87 (2020-02-26 오전 10:46:39)
// Output Gas List 추가 및 적용
// 간섭 검출용 백분율 가스 추가
// Base Zero Logic 수정
// - 범위 벗어난 채널 있을 경우 수행안함 (+Percentage Only)
// - 안정화 시간 적용 (10분)
// - 채널별 Warm-up 모드 적용 (최초 1시간)
// - Warm-up 모드에서 기준 변경 (범위 2배, 시간 20초 고정)
// - 온도편차 1도 이내일 경우 Warm-up 모드 해제
// EEPROM Write delay 변경 (5 -> 7msec)
// Manual Zero Update 시 Reference Channel SpanGas Update되지 않도록 수정

// REV2.88 (2020-03-03 오후 4:06:18)
// mA Output Test Mode에서 0mA 출력되는 문제 수정
// 간섭필터링 Zero Skip 이상에서 실행

// REV2.89 (2020-03-03 오후 9:11:15)
// CARTRIDGE_READ_REGISTER_SIZE 변경 (192 -> 128)
// CAVITY_READ_REGISTER_SIZE 변경 (256 -> 128)
// Factory Measuring - View Status 수정
// - Measuring Status Color 수정
// - Interference Gas Monitor 추가
// - Wave Monitor 추가

// REV2.90 (2020-03-10 오전 11:12:15)
// vpt 수정
// Peak Monitor 추가
// All Base Mode 추가

// REV2.91 (2020-03-11 오후 4:37:29호)
// Measuring & Status View 위치 수정
// 간섭채널간 필터링 로직 제거
// Scope mode에서 이전 메뉴 이동 시 잔상 제거

// REV2.92 (2020-03-16 오후 2:41:55)
// Measuring Filter
// - Sensor Trouble Logic 수정
// - Offset Adjustment 제거
// - Source Power 판정 시간 수정
// - LNG Mode 부탄 필터 추가
// Self-test
// - Sensor Level Test 조건 추가
// Gain Set 온도 기준 변경 (30 ~ 50)

// REV2.94 (2020-03-19 오후 1:43:39)
// TEOS Range 변경 (30.0 -> 100)

// REV2.95 (2020-03-23 오후 4:22:52)
// Base Zero Warm-up 상태 해제 조건 변경
// - 온도조건 삭제
// - 수동제로 진행 시 해제
// Base Zero Warm-up 상태 수행 시간 변경 (20초 -> 10초)
// LNG Mode 부탄 필터 제거

// REV2.96 (2020-03-24 오전 9:12:53)
// Factory Measuring (View Status)
// - Output Channel 간섭 검출 데이터 추가
// - Raw Deviation 데이터 추가
// - 반응률 표시 적용
// - 필터 컬러 가변 적용
// - i-Gas Mornitor 삭제
// - i-Peak & t-Peak Monitor 통합
// Factory Cal
// - Base Set Update Error 수정
// Factory System Init
// - Sensor Set (Skip Percentage 가스별 기본값 적용)
// - PoE / 485 삭제
// - 1채널 모드에서 mA CH-2 Disable 적용
// iPer 계산 오류 수정 (bSpan 산출)
// Base Zero And 조건 삭제

// REV2.97 (2020-03-30 오전 9:40:34)
// Base Zero 조건 변경
// - Timer 전체 기본값 600 적용
// - 온도편차 조건 변경 (1 -> 0.5dec)
// Vpt 제거
// 센서 온도 정상 범위 수정 (-10 to 85 -> -20 to 85)

// REV2.98 (2020-04-06 오전 9:13:09)
// 온도보상 제거
// Measuring Status OcAvrg 추가
// Measuring Status Up & Down Key Repeat Disable
// Manual Cal Update 시 Base Cal Data 업데이트
// Factory Cal -> View Info
// - Channel Rotation Key 변경 (Func -> Up & Down)
// - Base Zero Counts Clear (Func Long Key)
// Function Menu 선택 시 Rotation 적용 (Top->Bot & Bot->Top)
// Nand ID 추가 (CYPRESS, S34ML04G2)

// REV2.99 (2020-04-14 오후 5:07:29)
// VBAT Scan Logic 수정 (Factory -> System Info에서만 활성화)
// Function Menu Rotation 삭제
// Maintenance Mode에서 Interface -> Relay Setting Disable
// TEOS 100ppm Curve Data 적용
// CAL Logic 수정
// - All Base & Span 승인 추가
// - Span 반응률 표시
// - Span 반응률 판정 기준 수정 (Zero to Span ADC < Span Gas Resolution * 2 = failure)
// - Zero 반응률 판정 삭제
// - View Info 반응률 추가

// REV3.00 (2020-04-20 오후 2:54:06)
// 간섭판정 기준 수정 (한 채널 이상 3% 이상이면 판정)

// REV3.01 (2020-04-21 오후 2:14:48)
// IR Sensor Number 변경 (2.84 Down Grade Mapping)

// REV3.02 (2020-04-21 오후 3:14:35)
// High Scale 홀수 설정 시 Gas Simulation 농도설정 오차 수정

// REV3.03 (2020-05-11 오후 4:42:54)
// Base Zero 전압변경 (기존값 - 현재값) * 0.3 + 현재값

// REV3.04 (2020-05-14 오후 2:36:49)
// Factory Measuring Status Parameter 변경

// REV3.05 (2020-05-20 오전 9:10:29)
// Modbus RTU Unit ID 설정범위 수정
// Modbus TCP Unit ID 삭제
// Webpage 적용 (Status View만 적용)
// Flow Control 시정수 변경 (자체진동 노이즈 개선)
// - 기존: 180 ~ 60Hz, 20 ~ 30% Duty
// - 변경:       60Hz,  1 ~ 31% Duty

// REV3.06 (2020-05-28 오후 2:31:16)
// System EEPROM 태그 영역분리 및 추가
// 시리얼넘버 11자리 적용
// Flow Control 초기값 변경
// 시리얼넘버 업데이트 페이지 분리 (factory.htm)

// REV3.07 (2020-06-02 오후 1:48:40)
// Serial Number 기본값 변경

// REV3.08 (2020-06-02 오후 2:58:34)
// URL Decoding 적용
// TAG Prefix 변경
// TAG 기본값 변경
// TEOS SPAN 기본값 변경 (50 -> 30ppm)

// REV3.09 (2020-06-03 오전 8:55:14)
// URL Decoding Character 추가 및 수정

// REV3.10 (2020-06-03 오후 7:02:48)
// Measuring Config 마지막번호 설정 시 설정값 오류 현상 수정
// Com Status Icon 표시방식 변경 (토글 -> 점멸)

// REV3.11 (2020-06-04 오후 4:56:00)
// IR Signal Not Stable 판정시간 변경 (10분 -> 15분)
// Waepage 0.1 적용 (Parameter, Alarm 추가)
// LNG KFI 인증버전

// REV3.12 (2020-06-10 오전 8:58:26)
// Waepage 0.1 완료
// Flow Trouble 기준 변경 ( 80mL 이하 또는 1200mL 이상 -> Target -20% 이하 또는 Target +20% 이상)
// Flow Trouble 판정 시간 최소 설정값 변경 (1초 -> 5초)
// Offset Drift 보상 제거
// Error-50 추가 (측정불능상태 2시간 이상 지속될 경우)

// REV3.13 (2020-06-22 오후 3:50:39)
// Error Decoding 우선순위 변경 (기존: E-50 -> 채널n E-41,42,47, 변경: 채널n E-41,42,47 -> E-50)

// REV3.14 (2020-07-14 오전 10:27:52)
// Amp Cal Target Voltage 설정 추가
// Sensor Cal 초기값 변경 (Zero = 2.5[V], Span = 2.215[V])
// Not Stable Monitor 추가

// REV3.15 (2020-07-20 오전 9:57:52)
// Not Stable Mointor -> Filtering Status 수정
// Measuring Filter Set 추가

// REV3.16 (2020-07-27 오후 5:03:06)
// Filtering Status Target 채널 컬러 지정

// REV3.17 (2020-09-14 오후 5:18:45)
// Gas Simulation Mode 가스명 표시 스페이스 적용
// Measuring Task 활성 채널만 적용
// LMM-274-X025 C4H10 삭제
// Dual IR Logic 추가
// Sensor Type 추가
// - LIM-272-CH
// - LIM-272-DH
// - LIM-272-IH
// - LIM-272-X018
// - LIM-272-X021
// - LIM-272-XXXX (Reserved)
// - LIM-272-X044
// - LIM-272-X040

// REV3.18 (2020-11-02 오전 11:23:24)
// CO2 High Scale 변경 (3% -> 5%)
// CO2 Alarm 농도 수정 (CO2)
// Span Cal 전압오류 수정

// REV3.19 (2020-11-12 오전 11:23:18)
// LIM-272-XXXX -> LIM-272-X054 적용
// CO2 5% 추가

// REV3.20 (2020-11-19 오전 10:33:00)
// 반응률 데이터 추가
// Gain 설정범위 수정 (Offset: 3.0[V] -> 3.5[V], Target: 2.5[V] -> 2.0[V])
// Gain 기본설정 수정 (Offset: 3.0[V] -> 3.3[V], Target: 2.5[V] -> 2.2[V])
// Sensor Set Init - Gain 기본설정값 포함
// IR Offset Drift Check Margin 변경 (200mV -> 100mV)
// IR Reference 미포함 iPer로 적용

// REV3.21 (2020-12-07 오전 10:04:36)
// Base Zero Off 시 Manual Base Zero 비활성화
// Span Gas 및 설정 범위 Cross Scale 연동되도록 수정
// Cross Scale 적용 위치 변경 (결과값 -> 스판가스값)
// CO2 온도보상 적용
// IR Curve Data 수정 (CH2F2 QUAD)
// 가스추가 (COS DUAL, C4F8 DUAL, ECH QUAD, SFA-1 QUAD, C4F6S QUAD)

// REV3.22 (2021-02-03 오후 4:56:39)
// 채널맵핑 수정 (ECH, SFA-1)

// REV3.23 (2021-02-04 오후 2:47:03)
// Span Gas 표시값 변경 (Cross Scale 미적용)
// Span Max 계산식 변경 (SpanMax = HighScale * (100.f / CrossScale) + 0.5f, 최대값 = 9999)

// REV3.24 (2021-02-05 오후 1:52:13)
// Measuring Config Update 버그 수정 (Cross Scale)
// SFA-1 간섭필터링 기준채널 변경 (3.330(Target) -> 8.925)
// ECH, SFA-1 측정상수 변경 (a, n, iSpanScale, CrossScale)

// REV3.25 (2021-02-09 오후 5:05:09)
// SFA-1 측정상수 변경 (a, n, CrossScale)
// Span Skip Cross Scale 적용

// REV3.26 (2021-03-02 오후 3:43:14)
// LIM-272-X054 표시값 수정 (DUAL-XXX -> DUAL-054)
// IR Gas List 추가 (10.100)
// iGas 계산시 Cross Scale 적용되지 않도록 수정
// Gas Name 설정기능 추가
// Maintenance Auto Quit Timer 기본값 수정 (60분 -> OFF)
// SFA-1 Range 변경 (2000ppm -> 100%LEL)
// IR Gas List 삭제 (IR_GAS_03_330_HC_CH4_100_LEL, IR_GAS_03_330_HC_C4H10_100_LEL)

// REV3.27 (2021-03-18 오전 11:42:46)
// SFA-1 측정상수 변경 (a, n, iSpanScale, CrossScale)
// System Error Hex Code 표시 제거

// REV3.28 (2021-03-29 오후 2:41:02)
// 가스추가 (IPA QUAD)
// Measuring Status 항목 추가 (Error Status)

// REV3.29 (2021-04-08 오후 4:38:45)
// Measuring Config 수정 (LIM-272-CH TYPE-004 & 005 삭제)

// REV3.30 (2021-05-13 오후 3:24:43)
// 광학 경로 오염 진단 추가(E-07)
// 가스추가 (3.33, X027_4MS_100_LEL)

// REV3.31 (2021-06-01 오후 12:59:03)
// COS 간섭필터링 시정수 변경 (IPA & Ethanol 저농도 검출 요청 - 하이닉스)

// REV3.32 (2021-06-02 오후 5:43:31)
// COS 간섭필터링 시정수 변경 (FC-3283 저농도 추가 검출 요청 - 하이닉스)

// REV3.33 (2021-10-14 오전 11:14:32)
// Buzzer Drive 추가
// Pump PWM 계산식 변경
// brownout reset 추가 (LEVEL1 Falling Edge 2.13 ~ 2.24[V] Typ. 2.19[V])
// Sensor Type 추가 (LMM-274-X040(Rotary-B), LMM-274-X042(Rotary-C))
// 가스추가 (ECH, NF3)
// SFA-1, 4MS Span Gas & Cross Scale 기본값 변경

// REV3.34 (2021-12-15 오전 10:20:58)
// Measuring Mode Item의 Test 항목 분리 (EEPROM 미적용, 전원 OFF 시 해제)
// NF3 간섭 필터링 시정수 변경

// REV3.35 (2021-12-20 오후 2:42:49)
// NDIR 계산식 Kelvin 온도 적용

// REV3.36 (2022-03-25 오후 2:42:49)
// 간섭필터링 상수 변경 (iSpanScale: CH2F2)

// REV3.37 (2022-04-05 오전 9:47:47)
// 간섭필터링 상수 변경
// - iSpanScale: CH4, LNG, CH3F, C5F8, TEOS, C4F6, SFA-1, IPA, 4MS, C4F6S, ECH
// - n: CH4, LNG, CH3F, SFA-1, IPA

// REV3.38 (2022-05-30 오후 8:34:04)
// MODBUS Map 변경 (시스템그룹 요청사항)
// 간섭필터링 개선 (SFA-1, CH3F, C5F8, TEOS, IPA)
// 간섭필터링 상태에서 Base Zero 실행되지 않도록 수정

// REV3.39 (2022-06-14 오후 3:01:03)
// 간섭필터링 개선 (C5F8, IPA)

// REV3.40 (2022-09-06 오후 3:23:17)
// 신규가스 추가 (GMP-02, C4H2F6, OMCTS, Toluene)
// LMM-274-X042 Channel-2 Filter 변경 (9.000 -> 9.150)

// REV3.41 (2022-10-13 오후 2:45:16)
// IPA RANGE 추가 (IPA 100 %LEL)

// REV3.42 (2022-10-31 오후 7:24:13)
// System Error Scan 항목 추가 (Cartridge & Cavity eADC)
// Sensor State Mapping 분리
// Modbus RTU & TCP 쓰기 금지 주소 적용
// Modbus RTU & TCP Write Multiple Registers 쓰기 에러 수정
// Scope Mode 8Hz 추가 적용
// Modbus Master 상태추가 (MODBUS_RTU_MASTER_UNUSED_FUNCTION_CODE)
// Timer Frequency 제어 정확도 향상 (TIM_Base_InitTypeDef -> AutoReloadPreload 추가)
// Flow Init Item 추가 (SYSTEM_EEP_FLOW_CTRL_P_CALCULATION_SCALE)
// FlowSet Target 전압 Scale 적용(972 = 0.5)
// FlowSet Span Decision 조건 삭제 (Result_Full < (FlowSet.Cal.Span_mL * 0.5f))

// REV3.43 (2022-11-16 오후 5:34:33)
// LCD 밝기 조절 추가

// REV3.44 (2022-11-23 오전 9:20:18)
// LCD Brightness EEP Prefix 적용

// REV3.45 (2022-11-25 오전 9:54:41)
// Measuring Timer IR Source Frequency 연동
// 모델별 RawMaxSum Limit 적용
// N2O 1000 PPM 추가
// COS 75 PPM 추가 (신규센서 GSA-972 적용)

// REV3.46 (2022-12-06 오후 5:45:05)
// GTD-5100FN-H COS 75 PPM -> 50 PPM 변경
// Maximum Power Test Mode 추가
// Dual Sensor CH4 Range 추가 (100 %VOL)

// REV3.47 (2022-12-09 오전 10:30:06)
// COS 75ppm a, n, 1st, 2nd Alarm 변경

// REV3.48 (2022-12-12 오후 4:21:08)
// Pump & Buzzer Drive Tree 변경 (App -> BSP)
// Power Control 추가 (Battery Mode)
// Time Deviation Limit 변경 (>1 -> >3)
// Modbus RTU & TCP Write Multiple Registers 참조 주소 오류 수정
// 알람설정 하한치 수정 (Zero Skip + 1)

// REV3.49 (2023-01-26 오전 8:50:57)
// BQ2579x, TPS25750 Driver 추가

// REV3.50 (2023-03-15 오전 9:23:47)
// 신규물질 추가 (HMDS, Silitane, Ethanol, Methanol, CTFE, C3F6 DUAL)
// Flow Set GTD-5100FN-H 기준 Taget Full & Zero 전압 변경 (센서 오차에 따른 불량률 개선)

// REV3.51 (2023-03-16 오전 11:34:14)
// 배터리 미사용 모드에서 BatCharger Sync Set 되도록 수정
// CTFE Unit 수정 (%VOL -> PPM)
// BatMode Hardware Version Port 적용
// Flow Set GTD-5100FN 기준 Taget Full & Zero 전압 변경 (센서 오차에 따른 불량률 개선)

// REV3.52 (2023-03-20 오전 11:29:24)
// Methanol iSpanScale & ZeroSkip 수정

// REV3.53 (2023-03-20 오후 3:45:53)
// 신규물질 추가 (Ethylene, C3F6 QUAD)

// REV3.54 (2023-04-19 오후 5:45:49)
// TPS25750 Dead Battery Flag Clear Logic 추가

// REV3.55 (2023-04-20 오후 3:35:13)
// Battery Icon 추가
// Low Battery Message & Auto Shut Dwon 적용
// Buzzer 설정 추가
// Portable & Buzzer Present Mode UI 적용
// Relay_BlinkType 수정

// REV3.56 (2023-05-16 오후 4:27:43)
// Fully Charged LED 추가
// Power S/W GPIO Port 변경
// H/W Version AD Port 변경

// REV3.57 (2023-05-30 오전 9:21:33)
// Power Off Mode Log 추가

// REV3.58 (2023-05-30 오후 1:54:00)
// USB PD Configuration 변경 (호환 충전기 확장)
// Power On 시 Fully Charged LED Off
// Protable NF3 최초 데모 버전 (영남지사 엄삼욱 06/01 출고)
// Battery Icon 색상변경 및 Fully Charged 상태 추가

// REV3.59 (2023-06-01 오후 2:56:32)
// Battery Charger PFM 비활성화
// Battery 잔량 판정 로직 수정
// System Option Detection Time 수정 (1msec * 1000)
// PowerKey PUPDR Register Init 수정 (PULLUP -> NOPULL)
// 신규물질 추가 (FL300)
// Portable Mode System Info 적용

// REV3.60 (2023-06-29 오후 2:58:44)
// 통신상태 아이콘 위치 오류 수정
// FL300 측정상수 변경 (SpanGas, CrossScale, a, n)

// REV3.61 (2023-07-05 오후 2:56:13)
// mA 세분화 적용

// REV3.62 (2023-07-11 오전 10:39:31)
// E-11 mA 출력 오류 수정

// REV3.63 (2023-08-25 오후 2:28:43)
// Cartridge 추가
// - Measuring Config 수정
// - Gas Channel 추가 5 -> 6 (Cartridge 2, Cavity 4)
// - Sensing_Process Fifo 삭제
// - Sensing_Process Task 이동 (TmSync_Common_Task -> Measuring_Task)
// - Sensor Sequence Number Check Logic 수정
// - Factory Menu UI 수정
// - Sensor EEPROM Write Logic 수정
// Cartridge Sensor & Gas List 추가
// MODBUS RTU & TCP Slave Write Register 마지막 주소 금지 오류 수정
// Dual Channel UI 적용
// - Font 수정 (29x48 -> 29x46)
// - 측정값 Dot 처리 좌표 수정
// - System Error 채널 분리
// Output Gas Decision 실행조건 추가 (Sensor Warm Up 완료 시 진행)

// REV3.64 (2023-09-25 오후 7:04:14)
// EC_GAS_EC_DC_HCL_15_CH2CL2_500_PPM Dp 수정 (1 -> 0)
// CT_SEN_EC_DC_HF_9 Config 오류 수정
// Default Gas Info 항목추가 (Maintenance Gas)
// Cross Scale 상한값 수정 (5.00 -> 50.00)
// Maintenance Set CH-2 Decimal Point Mapping 오류 수정
// Cartridge Error Item 추가 (HS300x: Humidity & Temperature Sensor, iADC, Flash)
// Cavity 미사용 Error Reserved 처리 (IR Sensor, IR Source, iADC, I2C Driver)
// System_Error_Peripheral 추가 (Cavity: Driver, Init, Cartridge: HS300x, iADC, Flash)
// Interference Gas Filtering Channel Remap (1, 2, 3, 4 -> 2, 3, 4, 5)
// RandomNoise, Deviation 판정 참조값 변경 (2 or 1 -> IR Sensor 채널 수 / 2)

// REV3.65 (2023-10-13 오후 4:07:41)
// Cartridge Data Mapping
// Scope Mode 표시우선순위 변경 (전압표시 -> 측정값)
// Sequence Number Check 오류 수정 (TOP -> BOT)
// Factory Cal Channel 선택 오류 수정 (Gain Target Voltage, Zero, Base Zero Set)
// Cavity Gain Target Voltage Lower Limit 수정 (2.000[V] -> 1.800[V])
// System Config 순서 변경 (Cavity -> Cartridge)
// Cartridge Config 수정 (BuffSize & Period Cavity 연동)
// Cartridge nRat, nPer 계산식 적용
// Debug Print Data 수정 (Cartridge Data 추가)

// REV3.66 (2023-10-20 오전 10:54:18)
// CT_RAW_DV_ITEM 삭제 (TEMP, AVDD, DVDD, VCB, ICB0, ICB1)
// IR Calculation Beta 보상 기준온도 변경 (bzTemp -> mzTemp)
// Zero Cal Reverse 판정 삭제
// IPA 100 %LEL a, n, SpanGas 변경
// Sensor & Gas 추가 (LIM-272-X127, NF3 200 PPM Dual)
// Gas Info 항목추가 (mGain)
// Default Gas Info & Sensor Set Init 항목추가 (mGain, TargetOffs, TargetGain)
// 신규물질 추가 (HMDS 100 %LEL, OMCTS 100 %LEL, TEOS_HT200)
// TEOS iSpanScale Ch1 수정 (IPA Filtering Margin 확보)

// REV3.67 (2023-10-25 오후 1:43:42)
// Measuring_Gas_Info_Update(), Factory_Sensor_Set_Init() AmpCalTarget Cartridge 공통 적용
// Measuring_Gas_Info_Update(), Factory_Sensor_Set_Init() Cartridge DAC, TIA Data 추가
// Cartridge_TiaType 추가
// AmpCalTarget Cartridge 설정 범위 적용 (300mV ~ 1900mV)
// Cartridge Gain Cal 추가

// REV3.68 (2023-10-27 오전 8:44:54)
// Cartridge Sensing Config Flash Address 변경
// Function State Calibration Bit Menu 진입 시 활성화 되도록 수정
// Cartridge Gain 변경 시 즉시 적용되도록 수정
// Cartridge Gain Cal C-RATE 계산 bZero 값 적용

// REV3.69 (2023-10-27 오전 10:54:04)
// Factroy Cal Info, Measuring Status 수정 (Catridge Data 적용)
// Channel Config Load Combined 모드 Cavity Ref Channel Mapping 오류 수정
// Peak Monitor 초기화 오류 수정
// Cartridge Rgain Return Logic 수정 (설정값과 다를 경우만 실행)
// DEBUG PRINT Data 수정
// Cartridge Gain Cross Scale 적용
// Alpha 보상 IR 계산식 수정 (Absorbance -> Transmittance)
// PUMP_CTRL_DUTY_MAX 수정 (31% -> 36%)
// Self test <-> Warm up 순서 변경
// Normal Mode 최초 진입 시 Measuring Lpf 초기화
// Normal Mode 진입 시 Flow Trouble Timer 초기화 & System Error Clear
// Warm up zero calibration 추가

// REV3.70 (2023-11-14 오전 9:33:46)
// Warm up zero 활성화 조건 변경 (Portable & Cavity Dual & NF3 200ppm일 경우만 활성화)

// REV3.71 (2023-11-14 오후 1:30:14)
// Cartridge 기준전압 변경 (Zero to Span: 1.6[V] -> 1.4[V])
// CT_SEN_EC_DC_O3_1 RGAIN 기본값 추가 (512K)

// REV3.72 (2023-11-14 오후 5:05:24)
// Warm up zero 실행 시 Base zero NotSable 해제
// View Cal Info Offs, Gain 채널 Mapping 오류 수정
// SENSOR_EEP_CT_CHn_DEFAULT 변경 (Zero: 0.4[V], Span: 1.8[V])
// Cartridge Target Offs 변경 시 Bias 전압도 변경되도록 수정
// Factory Sensor Set Init 초기값 변경 (Maintenance Gas)
// Gas Percentage 초기값 변경 (0 -> Maintenence Gas)
// Warm up zero 활성화 시 Flow Self Test 기준 변경 (50% -> 90%)
// Self Test Sensor Check 항목추가 (CtgRawDvSumSum)
// Cartridge Data 변경 (RawDvMax -> RawDvSum)

// REV3.73 (2023-11-23 오전 9:22:44)
// UNI Type 추가
// 신규물질 추가 (C3F6O 5000 PPM)
// Cal Decision Logic 수정 (Low Response Factor: 1.25 -> 0.7)
// TEOS & HT200 Gas Name 수정 (TEOS -> TEOS-HT200)
// HMDS 100 %LEL a, n 추가

// REV3.74 (2023-11-24 오후 2:59:02)
// Measuring Deviation Data 삭제 (CtgRawDvSum)
// Self test 조건 수정 (Cartridge Channels 적용)
// CARTRIDGE_BUF_SIZE 수정 (180 -> 200)
// Scope Mode 표시오류 수정
// UNI Type 수정 (Power Sensing & Trouble 판정)
// Measuring Filter Item 변경 (Temp -> CtgTemp & CvtTemp)
// Log Print Data 추가 (BatSenTemp, CtgSenTemp, CtgSenHumi)
// CSTACK, HEAP Size 변경 (16K -> 8K, 32K -> 16K)
// Cartridge Filtering Item 추가 (VCE, VRE, LPTIA_LPF_PeakDv)

// REV3.75 (2023-12-11 오전 11:09:36)
// Cartridge R-Filter 기본값 변경 (1M -> 20K)
// Cartridge R-Gain 기본값 적용
// Cartridge DAC Reset & Error(Code-8, 2mA) 추가
// Cavity 미사용 시 Source Frequency 10으로 설정 (Measuring Timer 기본값)
// Unified Type Detection Pin 변경 (GPIOA.12 -> GPIOA.6)

// REV3.76 (2024-01-12 오전 9:30:31)
// Cartridge DAC Error 시정수 변경 (15mV, 5초, 3회 -> 30mV, 10초, 5회)
// Cartridge Default Gas Info REV0.1 적용
// Default Gas Info Item 추가 (OutDelayPer, OutDelaySec)
// Cavity Gain Target Voltage Lower Limit 수정 (1.800[V] -> 1.400[V])
// O2 Linearity & Temperature 보상 적용
// 측정방식별 Span Cal Response 적용 (반응률, 전류, 전압)

// REV3.77 (2024-01-30 오후 2:14:35)
// CB Type Span Cal Response 계산 및 표시 오류 수정
// Cartridge TIA R-FILTER 변경 (20Kohm -> 1Mohm)
// Measuring Filter Type 변경 View Status 적용 (U16 -> U32)
// Self-test Sensor Check Timeout 분리 적용 (60초 -> 900초)
// System Init 채널선택표시 오류 수정 (Sensor Set, Sensor Cal)
// Cartridge RandomNoise 판정 조건 수정 (Zero & Vias -> Zero | Bias)
// IR NotStable 조건 추가 (offset unstable)
// IR NotStable Common Error 추가 (필터링 이후 안정화 되지 않을때 발생)
// Measuring Config Set Number Memory 변경 (System EEP -> Sensor EEP)
// Cartridge Response 적용
// Interference Gas Filtering Channel Mapping
// O2 25%VOL 추가
// Cal Limit 추가 (IR High 제외)
// NF3 Portable WarmUpSec 변경 (150 -> 120)

// REV3.78 (2024-02-19 오전 9:31:05)
// O2 25%VOL Alarm Type 수정 (Increas -> Decrease)
// Self-test Sensor Level Check 조건 추가 (Cavity Offset)
// Cartridge Sensor Lpf 가중치 분리 (16%)
// Cartridge TIA R-FILTER 변경 (1Mohm -> 20Kohm)
// Cartridge Modbus Remap
// Cartridge Random Noise 시정수 시험용

// REV3.79 (2024-02-23 오전 9:53:15)
// Default TIA 수정 (RLOAD = 0, RFILTER = DISCONNECT, RGAIN = DISCONNECT, SWMODE = OFF)
// Cartridge Undefined Sensor 추가
// Cal Response Limit 완료
// CT_SEN_SIH4_CD_50 Gain & Cal Limit 수정
// Measuring Filter Decision 오타 수정 (CvtNotStable -> CtgNotStable)
// System Error Remap
// Sys Error Decoding (Cartridge & Dual Mode 적용)
// Cartridge Error 추가 (Exceed current)
// IR Not Stable Common Error 삭제
// System Error 발생 시 Maintenance Gas 출력
// 신규물질 추가 (C3H6 100 %LEL)

// REV3.80 (2024-03-04 오후 5:11:21)
// VCE, VRE 삭제
// Cartridge PeakDvType 및 Item 추가 (Zero, Bias)

// REV3.81 (2024-03-05 오후 4:20:35)
// VZERO, VBIAS PeakDv Limit 적용
// nPerDv Limit 수정 (SiH4, HF, HCl, C2H4, NO)

// REV3.82 (2024-03-11 오후 5:12:57)
// Self-test 조건변경 (CtgRawDvSum_LPTIA_LPF: 1000 -> 2000)
// CtgExcCurrent Error Check 조건 수정 (EC Type 삭제)
// CtgRandomNoise 참조 오류 수정 (RawDvSumLmt_VZB -> PeakDvLmt_VZB)

// REV3.83 (2024-03-14 오전 9:07:08) T-PJT 사전생산 버전
// EC_DC_HCL_15 전체 측정가스 Zero Skip 및 Calibration Low Limit 변경 (Skip: 7% -> 9%, Limit: 5% -> 7%)

// REV3.84 (2024-04-02 오전 11:23:03)
// IR Error 진단 추가 (Offset Unstable)

// REV3.85 (2024-04-23 오후 1:40:14)
// AvDvSumLmt 100으로 수정 (로직 미완성 임시 적용)
// Portable C4F8 Dual Warm-up zero 및 Beta 보상 적용
// TPS25750 Patch Bundle Reload 추가 (Patch Mode 진입 시 PBM Process 실행)
// IR optical path contamination check Dual X127 조건 추가

// REV3.86 (2024-05-23 오후 5:49:42)
// MODEL_CVT_QUAD_X025_ETHYLENE_100_LEL 삭제
// 신규물질 추가 (C4F6 300 ppm, ETHYLENE 100% LEL)
// AvDvSumLmt 임시 변경 (100 -> 500)

// REV3.87 (2024-06-18 오후 4:28:49)
// C3F6O SpnaGas & CrossScale 변경 (3000 & 1.00 -> 2600 & 1.15)
// Cartridge Filtering 기준치 변경 (RawDvSumLmt_TIA, PeakDvLmt_TIA)
// Dual Cartridge Filtering & Error 추가 (Gas Deviation - nPer >6.0%)

// REV3.88 (2024-07-08 오전 10:55:31)
// Cartridge 모델 분리 적용
// Cartridge 모델별 필터링 기준치 적용
// Dual Cartridge 불량 진단 및 출력 로직 추가
// ETHYLENE 100 %LEL 가스명 변경 (ETHYLENE > C2H4)

// REV3.89 (2024-07-10 오후 7:47:54)
// Cartridge 5200 모델 LPTIA_LPF RandomNoise 즉시 적용

// REV3.90 (2024-07-12 오후 2:27:17)
// MODBUS Map REV0.9 적용 (Bit Name 변경: Maintenance Mode -> Function Mode, Bit 추가: Maintenance Mode)
// 신규물질 추가 (CF4 500 PPM)

// REV3.91 (2024-08-30 오후 2:50:21)
// Measuring Config 실행 시 Cavity Manual Offset & Gain Wiper 업데이트 되지 않도록 수정

// REV3.92 (2024-09-26 오후 4:46:00)
// Dual Cartridge Gas Deviation Error 판정시간 변경 (15분 -> 30초)
// Dual Cartridge Gas Deviation Error 추가 (E-51: 채널1이 높을 경우 , E-52: 채널2가 높을 경우)

// REV3.93 (2024-10-25 오전 10:46:49)
// 신규물질 추가 (X042 BTBAS 1000 PPM, 긴급설치용, 삭제 예정)



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
VERSION_EXT bool System_Ver_Load( void );
/* End Function */

/* Start Variable */
VERSION_EXT float		HardwareVersion;
/* End Variable */


#endif // __VERSION_H__


