/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   tftp_example.h
 * Author: dziegel
 *
 * Created on February 17, 2018, 3:43 PM
 */

#ifndef TFTP_EXAMPLE_H
#define TFTP_EXAMPLE_H

#include "common.h"


typedef enum {
  
    START_UPDATE = 1,
    MAIN_UPDATE,
    MAIN_UPDATING,
    CT_UPDATE,
    CT_UPDATING,
    IR_UPDATE,
    IR_UPDATING,
    END_UPDATE,
    
} e_FWupdate_status;


typedef struct {
  
  bool bMain_Update;
  bool bCt_Update;
  bool bIr_Update;
  
  bool bMain_bootJump;
  bool bCt_bootJump;
  bool bIr_bootJump;
 
  uint8_t Update_totCnt; 
  
} s_FWupdate_flag;


typedef struct {
  
    e_FWupdate_status seq;
    s_FWupdate_flag flg;

} s_FWupdate;



#ifdef __cplusplus
extern "C" {
#endif

void tftp_example_init_server(void);
void tftp_example_init_client(void);

void FW_Update_task(void);

#ifdef __cplusplus
}
#endif

#endif /* TFTP_EXAMPLE_H */

