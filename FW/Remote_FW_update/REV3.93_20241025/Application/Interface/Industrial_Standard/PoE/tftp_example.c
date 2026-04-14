/*
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Dirk Ziegelmeier <dziegel@gmx.de>
 *
 */

#include <stdio.h>

//#include "lwip/apps/tftp_client.h"
#include "tftp_client.h" 

//#include "lwip/apps/tftp_server.h"
#include "tftp_example.h"
#include "main.h"
   
#include <string.h>
#include "nand_flash.h"
#include "sensing_process.h"

#if LWIP_UDP

/* Define a base directory for TFTP access
 * ATTENTION: This code does NOT check for sandboxing,
 * i.e. '..' in paths is not checked!  */
#ifndef LWIP_TFTP_EXAMPLE_BASE_DIR
#define LWIP_TFTP_EXAMPLE_BASE_DIR ""
#endif

/* Define this to a file to get via tftp client */
//#ifndef LWIP_TFTP_EXAMPLE_CLIENT_FILENAME
//#define LWIP_TFTP_EXAMPLE_CLIENT_FILENAME "GTD-5x00(F)N_REV.bin"        //"GTD-5x00(F)N_REV.bin"//"test.bin"
//#endif
   
char *LWIP_TFTP_EXAMPLE_CLIENT_FILENAME;
s_FWupdate FWupdate; 
extern s_FWupdate_flag tftp_flag;


/* Define this to a server IP string */
#ifndef LWIP_TFTP_EXAMPLE_CLIENT_REMOTEIP
#define LWIP_TFTP_EXAMPLE_CLIENT_REMOTEIP "192.168.4.145"
#endif

static void boot_jump_select(void);
static char full_filename[256];

static void *
tftp_open_file(const char* fname, u8_t is_write)
{
  snprintf(full_filename, sizeof(full_filename), "%s%s", LWIP_TFTP_EXAMPLE_BASE_DIR, fname);
  
  printf("Open_File:%s %s\n\n",  LWIP_TFTP_EXAMPLE_BASE_DIR, fname);
  
  full_filename[sizeof(full_filename)-1] = 0;
  
  if (is_write) {
    printf(" fo\r\n");
    return (void*)fopen(full_filename, "wb");
  } else {
    return (void*)fopen(full_filename, "rb");
  }
}

static void*
tftp_open(const char* fname, const char* mode, u8_t is_write)
{
  LWIP_UNUSED_ARG(mode);
  return tftp_open_file(fname, is_write);
}

static void
tftp_close(void* handle)
{
  fclose((FILE*)handle);
}

static int
tftp_read(void* handle, void* buf, int bytes)
{
  int ret = fread(buf, 1, bytes, (FILE*)handle);
  if (ret <= 0) {
    return -1;
  }
  return ret;
}

static int
tftp_write(void* handle, struct pbuf* p)
{
  while (p != NULL) {
    if (fwrite(p->payload, 1, p->len, (FILE*)handle) != (size_t)p->len) {
      return -1;
    }
    p = p->next;
  }

  return 0;
}

/* For TFTP client only */
static void
tftp_error(void* handle, int err, const char* msg, int size)
{
  char message[100];

  LWIP_UNUSED_ARG(handle);

  memset(message, 0, sizeof(message));
  MEMCPY(message, msg, LWIP_MIN(sizeof(message)-1, (size_t)size));

  printf("TFTP error: %d (%s)", err, message);
}

static const struct tftp_context tftp = {
  tftp_open,
  tftp_close,
  tftp_read,
  tftp_write,
  tftp_error
};

void
tftp_example_init_server(void)
{
  tftp_init_server(&tftp);
}

void
tftp_example_init_client(void)
{
  void *f;
  err_t err;
  
  ip_addr_t srv;
  int ret = ipaddr_aton(LWIP_TFTP_EXAMPLE_CLIENT_REMOTEIP, &srv);
  LWIP_ASSERT("ipaddr_aton failed", ret == 1);
  
  err = tftp_init_client(&tftp);
  if (err == ERR_OK)
      printf("*** Tftp_init_client Done ***\r\n\r\n");
  LWIP_ASSERT("tftp_init_client failed", err == ERR_OK);  
  
  //f = tftp_open_file(LWIP_TFTP_EXAMPLE_CLIENT_FILENAME, 1);     // 0:read    
  //if (f != NULL)
  //    printf("*** Tftp Open file***\r\n\r\n");
  //LWIP_ASSERT("failed to create file", f != NULL);
   
  err = tftp_get(f, &srv, TFTP_PORT, LWIP_TFTP_EXAMPLE_CLIENT_FILENAME, TFTP_MODE_OCTET);
  if (err == ERR_OK)
      printf("*** Tftp_RRQ_Send OK ***\r\n\r\n");
  LWIP_ASSERT("tftp_get failed", err == ERR_OK);
}


void FW_Update_task(void)
{
    switch (FWupdate.seq)
    {
        case START_UPDATE :
             for (uint16_t i = 0; i < 14; i++) { 
                  NAND_Erase_Block(0, i, 0);                               
                  printf("\r\n P: %d, B: %d, P: %d\r\n\r\n", NandAddr.Page, NandAddr.Block, NandAddr.Plane);
             }
             FWupdate.seq = MAIN_UPDATE;   
             break; 
        case MAIN_UPDATE :
             printf(" Seq : MAIN_UPDATE \r\n");
             if (tftp_flag.bMain_Update) {
                 printf(" Main_update ~ \r\n"); 
                 
                 LWIP_TFTP_EXAMPLE_CLIENT_FILENAME = "GTD-5x00(F)N_REV.bin";
                 NandAddr.Page  = 0;
                 NandAddr.Block = 0;
                 NandAddr.Plane = 0;  
                 tftp_example_init_client(); 
                 FWupdate.seq = MAIN_UPDATING;   
             } else 
                 FWupdate.seq = CT_UPDATE;
             break;
        case MAIN_UPDATING :
        case CT_UPDATING :
        case IR_UPDATING:
             //printf(" Update Progressing ~ \r\n");
             break;
        case CT_UPDATE :
             printf(" Seq : CT_UPDATE\r\n");
             if (tftp_flag.bCt_Update) { 
                 printf(" CT_update ~ \r\n");  
                 
                 LWIP_TFTP_EXAMPLE_CLIENT_FILENAME = "GTD-5x00(F)N_Cartridge_REV.bin";
                 NandAddr.Page  = 0;
                 NandAddr.Block = 10;
                 NandAddr.Plane = 0;
                 tftp_example_init_client(); 
                 FWupdate.seq = CT_UPDATING;
             } else 
                 FWupdate.seq = IR_UPDATE;
             break;
        case IR_UPDATE :
             if (tftp_flag.bIr_Update) {
                 printf(" IR_update ~ \r\n");  
                 
                 LWIP_TFTP_EXAMPLE_CLIENT_FILENAME = "GSA-970_REV.bin";
                 NandAddr.Page  = 0;
                 NandAddr.Block = 12;
                 NandAddr.Plane = 0;
                 tftp_example_init_client();
                 FWupdate.seq = IR_UPDATING;
             } else
                FWupdate.seq = END_UPDATE;
             break;
        case END_UPDATE :
             //printf(" Boot_Jump ~ \r\n"); 
             boot_jump_select();
             break;
    }
}

static void boot_jump_select(void)
{
    if (tftp_flag.bCt_Update) {
        SENSOR_USART_PORT_SELECTION( CARTRIDGE_USART_PORT );
        Cartridge_BootJump_Write_Packet_Send( &SensingProcess.Modbus );
        HAL_Delay(80);
    } else if (tftp_flag.bIr_Update) {
        SENSOR_USART_PORT_SELECTION( CAVITY_PORT );
        Cavity_BootJump_Write_Packet_Send( &SensingProcess.Modbus );
        HAL_Delay(80);
    }
    HAL_NVIC_SystemReset();
}


#endif /* LWIP_UDP */
