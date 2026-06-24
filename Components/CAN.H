#ifndef CAN_H
#define CAN_H

#include <MyProject.h>
#include <CANIF.h>

#ifndef STD_ON
#define STD_ON     (1U)
#endif
#ifndef STD_OFF
#define STD_OFF    (0U)
#endif
#ifndef STD_TRUE
#define STD_TRUE     (1U)
#endif
#ifndef STD_FALSE
#define STD_FALSE    (0U)
#endif
#ifndef TRUE
#define TRUE     (STD_TRUE)
#endif
#ifndef FALSE
#define FALSE    (STD_FALSE)
#endif

#define IDNum  0x01

/* IDE */
#define IDE_STD 0
#define IDE_EXT 0x80
/* RTR */
#define RTR_DAT 0
#define RTR_RMT 0x40

/* DLCs */
#define DLC_0                   (0)
#define DLC_1                   (1)
#define DLC_2                   (2)
#define DLC_3                   (3)
#define DLC_4                   (4)
#define DLC_5                   (5)
#define DLC_6                   (6)
#define DLC_7                   (7)
#define DLC_8                   (8)

/******************************过滤模式*********************************/
/*****变量说明：FIL_SIN：0：双过滤器模式            1：单过滤器模式*****************/
/*****变量说明：ID_MOD ：0：过滤标准帧              1：过滤扩展帧*******************/
#define     FIL_SIN    1
#define     ID_MOD     1


/* CAN中断使能开关 */
#define CAN_ISR_ENABLE       (STD_ON)
/* 外部CAN收发器的睡眠模式支持开关 */
#define CAN_TRCV_SLEEP_ENA   (STD_ON)
/* CAN发送超时计数极限，软延时 */
#define CAN_TXWAIT_TIMOUT      (24000)

typedef struct
{
    uint8 CAN_IDE;
    uint8 CAN_RTR;
    uint8 CAN_DLC;                                                     
    uint32 u32CanId;       //ID号范围：0-0x1FFFFFFF
    uint8 CanData[8];
}CAN_Data;

typedef struct
{
    uint8  u8CanFrameCfg;
    uint32 u32CanId;
}CAN_tstrSendHeadInfo;

typedef struct
{
    uint8  RxAckFlg;
    uint8  RxTimoutFlg;
    uint16 RxTimoutCnt;
}CANIF_tstrRxHeadInfo;

/* 软件CAN错误类型 */
enum
{
   CAN_ERR_NONE = 0,
   CAN_ERR_REC_OV = 0x01,
   CAN_ERR_BUSOFF = 0x02,
};

/* CAN驱动状态机 */
enum
{
   CAN_SM_IDLE = 0,
   CAN_SM_NORMAL,
   CAN_SM_ERROR,
   CAN_SM_FAST_RECOVER,
   CAN_SM_SLOW_RECOVER
};

extern CAN_Data xdata CAN_strRxPduTemp; 
extern void CAN_vidReadFrame(void);
extern void CAN_vidSendMsg(uint8 * pu8CanData, CAN_tstrSendHeadInfo * pstrCanSendHead);
extern void CAN_vidInit(void);
extern uint8 CAN_u8ErrState(void);
extern void CAN_vidSetCanError(uint8 u8ErrorType);
extern void CAN_vidStateMachine(void);
/**************************************************************************************************************/

#endif