//Header File
#ifndef __CANIF_H__
#define __CANIF_H__

/*******************************Including Header Files*************************************/
#include <MyProject.h>
#include "string.h"

#define UDS_COMSTACK_ENABLE             STD_OFF

/*******************************Define Macro***********************************************/
/* !Comment: UDS 响应ID */
#define CANIF_UDS_RESP_ID               (0x7EB)
/* !Comment: UDS 物理帧接收ID */
#define CANIF_UDS_PHYRX_ID              (0x7E3)
/* !Comment: UDS 功能帧接收ID */
#define CANIF_UDS_FUNRX_ID              (0x7DF)

/* !Comment: 报文超时时间, 时间 = Value*50ms. */
#define CANIF_COM_FIRST_TIMOUT          (60)
#define CANIF_COM_PDCU1_TIMOUT          (20)
#define CANIF_COM_PDCU2_TIMOUT          (20)
#define CANIF_COM_PDCU3_TIMOUT          (20)

/***************************External Symbols***************************************************/
/***************************External Function**************************************************/
extern void CANIF_vidDataInit(void);
extern void CANIF_vidReceiveProcess(uint32 u32CanId, uint8 u8CanDlc, uint8* pu8CanData);
extern void CANIF_vidTransProcess(void);
extern void CANIF_vidComLostDiag(void);
extern uint8 CANIF_u8EcuInBusOff(void);
extern void CANIF_vidTestCanSend(void);
#endif

