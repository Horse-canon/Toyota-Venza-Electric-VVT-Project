/**
 * @copyright (C) COPYRIGHT 2022 Fortiortech Shenzhen
 * @file      main.c
 * @author    Fortiortech  Appliction Team
 * @since     Create:2022-12-29
 * @date      Last modify:2022-12-29
 * @note      Last modify author is Leo.li
 * @brief     Can Init/ Send /Receive/deal with
 */


#include <MyProject.h>
#include <CANIF.h>

CAN_Data xdata CAN_strRxPduTemp; 
uint8 xdata CAN_u8SoftErrorType;
uint8 xdata CAN_u8DrvState;
uint8 xdata CAN_u8RecoverCnt;
uint16 xdata CAN_u16StateTimCnt;

/**************************************************************************************************************/
/**
* @brief     Can初始化
* @date      2022-12-29
*/
void CAN_vidInit(void)
{
    uint8   IDE_FIL;
    uint16  BRP;
    uint8   SEG2;
    uint8   SEG1;
    uint8   CAN_IDE;
    uint8   CAN_RTR;
    uint8   u8LocCanBTR0;
    uint8   u8LocCanBTR1;
    uint8   u8LocCanBTR2;
    uint32  EXTID1;
    uint32  EXTID2;
#if (ID_MOD == 0)
    uint8   Data0;
    uint8   Data1;
    uint32  STDID1;
    uint32  STDID2;
#endif

    CAN_u8SoftErrorType = CAN_ERR_NONE;
    CAN_u8DrvState = CAN_SM_IDLE;
    CAN_u8RecoverCnt = 0;
    
#if (CAN_TRCV_SLEEP_ENA == STD_ON)
    /*****配置外部Can收发器使能引脚 *****/
	 SetBit(P0_OE, PIN7);
	 /*  收发器休眠管脚不上拉。     */
	 ClrBit(P0_PU, PIN7); 
	 /*  输出低有效，使能外部CAN收发器   */
	 GP07 = 0;
#endif

    /***************************清0避免写入bug**************************************/
    u8LocCanBTR0 = 0;
    u8LocCanBTR1 = 0;
    u8LocCanBTR2 = 0;
    CAN_BTR0 = 0;
    CAN_BTR1 = 0;
    CAN_BTR2 = 0;
    
    /*  该位域定义了为了重新同步，在每位中可以延长或缩短多少个时间单元的上限tRESYN */
    /*  tRESYN=tq*(RESYN[1:0]+1)                                                   */
    //u8LocCanBTR0 |= SJW1;
    //u8LocCanBTR0 |= SJW0;

    /*****************  时间触发采样模式        0：禁止     1：使能     **************************/
    //u8LocCanBTR2  |= TTCM;
    
    /****************** 功能：CAN模块波特率设置    *************************************/
    /*  1M ：BRP  = 3; SEG2 = 0; SEG1 = 3;      */
    /*  500K ：BRP  = 5; SEG2 = 0; SEG1 = 5;    */
    BRP  = 3;                     //范围0-0x3FF           时间单元tq=(BRP+1)/ 24MHz
    SEG2 = 0;                     //范围0-0x07            时间段2时间长度tSEG2=tq*(SEG2+1)
    SEG1 = 3;                     //范围0-0x0F            时间段1时间长度tSEG1=tq*(SEG1+1)

    u8LocCanBTR0 |= (BRP >> 8);
    u8LocCanBTR1  =  BRP;
    u8LocCanBTR2 |= (SEG2 << 4) | SEG1;

    CAN_BTR0 = u8LocCanBTR0;
    CAN_BTR1 = u8LocCanBTR1;
    CAN_BTR2 = u8LocCanBTR2;

    IDE_FIL = 0;                                                             //选择过滤模式：1：过滤扩展帧 0：过滤标准帧
    #if (FIL_SIN == 1)                                                       //单过滤模式
    {
        SetBit(CAN_CR0, FILMOD);
        #if (ID_MOD == 1)                                                    //过滤扩展帧
        {
            EXTID1   = 0x1FFFFFFF;                                           //0-0x1FFF
            CAN_IDE  = 1;                                                    //过滤扩展帧
            CAN_RTR  = 1;                                                    //过滤远程帧
            CAN_FIR0 = EXTID1 >> 21;
            CAN_FIR1 = ((EXTID1 >> 18) & 0xE0) | (CAN_IDE << 3) | (CAN_RTR << 4) | ((EXTID1 >> 15) & 0x07);
            CAN_FIR2 = EXTID1 >> 7;
            CAN_FIR3 = EXTID1 << 1;
            CAN_FMR0 = 0xFF;
            CAN_FMR1 = 0xFF;
            CAN_FMR2 = 0xFF;
            CAN_FMR3 = 0xFF;
        }
        #else                                                                //过滤标准帧
        {
            STDID1 = 0x7FF;                                                  //0-0x7FF
            CAN_IDE = 1;                                                     //过滤扩展帧
            CAN_RTR = 1;                                                     //过滤远程帧
            Data0   = 0xFF;                                                  //过滤第一字节
            Data1   = 0xFF;                                                  //过滤第二字节
            CAN_FIR0 = STDID1 >> 3;
            CAN_FIR1 = ((STDID1 << 5) & 0xE0) | (CAN_IDE << 3) | (CAN_RTR << 4);
            CAN_FIR2 = Data0;
            CAN_FIR3 = Data1;
            CAN_FMR0 = 0xFF;
            CAN_FMR1 = 0xFF;
            CAN_FMR2 = 0xFF;
            CAN_FMR3 = 0xFF;
        }
        #endif
    }
    #else                                                                    //双过滤模式
    {
        ClrBit(CAN_CR0, FILMOD);
        #if (ID_MOD == 1)                                                    //过滤扩展帧
        {
            EXTID1 = 0x1FFFFFFF;                                             //0-0x1FFF8000，只可更改EXTID1[15]-[28]
            EXTID2 = 0x1FFFFFFF;                                             //0-0x1FFF8000，只可更改EXTID1[15]-[28]
            CAN_IDE = 1;                                                     //过滤扩展帧
            CAN_RTR = 1;                                                     //过滤远程帧
            CAN_FIR0 = EXTID1 >> 21;
            CAN_FIR1 = ((EXTID1 >> 18) & 0xE0) | (CAN_IDE << 3) | (CAN_RTR << 4) | ((EXTID2 >> 15) & 0x07);
            CAN_FIR0 = EXTID2 >> 21;
            CAN_FIR1 = ((EXTID2 >> 18) & 0xE0) | (CAN_IDE << 3) | (CAN_RTR << 4) | ((EXTID2 >> 15) & 0x07);
            CAN_FMR0 = 0xFF;
            CAN_FMR1 = 0xFF;
            CAN_FMR2 = 0xFF;
            CAN_FMR3 = 0xFF;
        }
        #else                                                                 //过滤标准帧
        {
            STDID1   = 0x7FF;                                                 //0-0x7FF
            STDID2   = 0x7FF;                                                 //0-0x7FF
            CAN_IDE  = 1;                                                     //过滤扩展帧
            CAN_RTR  = 1;                                                     //过滤远程帧
            CAN_FIR0 = STDID1 >> 3;
            CAN_FIR1 = ((STDID1 << 5) & 0xE0) | (CAN_IDE << 3) | (CAN_RTR << 4) ;
            CAN_FIR2 = STDID2 >> 3;
            CAN_FIR3 = ((STDID2 << 5) & 0xE0) | (CAN_IDE << 3) | (CAN_RTR << 4) ;
            CAN_FMR0 = 0xFF;
            CAN_FMR1 = 0xFF;
            CAN_FMR2 = 0xFF;
            CAN_FMR3 = 0xFF;
        }
        #endif
    }
    #endif

    /*---------------------CAN协议版本-----------------------------------*/
    /*-------------------0：CAN2.0A/B，支持标准帧、扩展帧-----------------/
    /*------------------------1：CAN1.0，仅支持标准帧--------------------*/
    ClrBit(CAN_CR0, CANREV);
    /*当检测到总线活动后，硬件自动对CAN_CR1[SLEEP]清0来唤醒CAN*/
    ClrBit(CAN_CR0, WAKMOD);      

    /*----------------LISTEN置1 监听模式使能--------------*/
    /*----------------SELFTST置1 自测模式使能--------------*/
    ClrBit(CAN_CR0, SELFTST);
    ClrBit(CAN_CR0, LISTEN );
    /*在复位模式下，CAN控制器处于复位状态，不可以接收或发送报文。对CAN_CR0[RSTMOD]清‘0’以进入复位模式*/
    //  ClrBit(CAN_CR0, RSTMOD);
    
#if (CAN_ISR_ENABLE == STD_ON)
    /* 中断开启选择 */
    SetBit(CAN_IER, ETYIE | BOFIE | PERIE | ERWIE | OVIE | ARBIE | TXIE | RXIE); 
#endif

    /* 端口转移：0:txd→P00,rxd→P01 1:txd→P05,rxd→P06 */
    SetBit(CAN_CR0, CAN_CH);

    /* 1ms定时中断优先级别为1 */
    SetBit(IP2, PRTC1);
    SetBit(IP2, PRTC0);

    CAN_TEC = 0;
    CAN_REC = 0;

    CAN_u8DrvState = CAN_SM_NORMAL;
    /* CAN模块使能 */
    SetBit(CAN_CR0, CANEN); 
}

/* 函数名称：CAN_vidReadFrame
 * 函数功能：CAN报文接收
 * 参数说明：
 * 注意事项：若开启中断，需要注释掉SetBit(CAN_CR1, BUFRLS);
 */
void CAN_vidReadFrame(void)
{
    uint8 rdptr;
    uint8 u8LocValidFrmFlg;
    
    /*通过 CAN_CR2 的低 2 位（RDPTR1 和 RDPTR0）确定当前接收缓冲区索引*/
    rdptr = CAN_CR2 & (RDPTR1 | RDPTR0);
    u8LocValidFrmFlg = STD_TRUE;
    
    switch (rdptr)
    {
        /*	rdptr = 0b00，对应 RX0 缓冲区 */
        case (0 | 0):
            CAN_strRxPduTemp.CAN_IDE = ( CAN_RX0CR & 0x80 );            /*扩展 ID 标志（1 表示扩展帧）*/
            CAN_strRxPduTemp.CAN_RTR = ( CAN_RX0CR & 0x40 );            /*远程帧标志（1 表示远程帧）*/
            CAN_strRxPduTemp.CAN_DLC = ( CAN_RX0CR & 0x0F );            /*数据长度（0~8）*/
            
            if (CAN_strRxPduTemp.CAN_IDE)                              
            {
                 /*扩展帧 ID */
                CAN_strRxPduTemp.u32CanId = 0;
                /*扩展帧高8位*/
                CAN_strRxPduTemp.u32CanId |= (uint32)CAN_RX0ID0 << 21;
                CAN_strRxPduTemp.u32CanId |= (uint32)CAN_RX0ID1 << 13;
                CAN_strRxPduTemp.u32CanId |= (uint32)CAN_RX0ID2 << 5 ;
                CAN_strRxPduTemp.u32CanId |= (uint32)CAN_RX0ID3 >> 4 ;
            }
            else
            {
                /*标准帧 ID */
                CAN_strRxPduTemp.u32CanId = 0;
                CAN_strRxPduTemp.u32CanId |= CAN_RX0ID0 << 3;
                CAN_strRxPduTemp.u32CanId |= CAN_RX0ID1 >> 5;
            }
            
            CAN_strRxPduTemp.CanData[0] = CAN_RX0DR0;
            CAN_strRxPduTemp.CanData[1] = CAN_RX0DR1;
            CAN_strRxPduTemp.CanData[2] = CAN_RX0DR2;
            CAN_strRxPduTemp.CanData[3] = CAN_RX0DR3;
            CAN_strRxPduTemp.CanData[4] = CAN_RX0DR4;
            CAN_strRxPduTemp.CanData[5] = CAN_RX0DR5;
            CAN_strRxPduTemp.CanData[6] = CAN_RX0DR6;
            CAN_strRxPduTemp.CanData[7] = CAN_RX0DR7;
            break;
            
        case (0 | RDPTR0):
            CAN_strRxPduTemp.CAN_IDE = ( CAN_RX1CR & 0x80 );
            CAN_strRxPduTemp.CAN_RTR = ( CAN_RX1CR & 0x40 );
            CAN_strRxPduTemp.CAN_DLC = ( CAN_RX1CR & 0x0F );
            
            if (CAN_strRxPduTemp.CAN_IDE)
            {
                CAN_strRxPduTemp.u32CanId = 0;
                CAN_strRxPduTemp.u32CanId |= (uint32)CAN_RX1ID0 << 21;
                CAN_strRxPduTemp.u32CanId |= (uint32)CAN_RX1ID1 << 13;
                CAN_strRxPduTemp.u32CanId |= (uint32)CAN_RX1ID2 << 5 ;
                CAN_strRxPduTemp.u32CanId |= (uint32)CAN_RX1ID3 >> 4 ;
            }
            else
            {
                CAN_strRxPduTemp.u32CanId = 0;
                CAN_strRxPduTemp.u32CanId |= CAN_RX1ID0 << 3 ;
                CAN_strRxPduTemp.u32CanId |= CAN_RX1ID1 >> 5 ;
            }
            
            CAN_strRxPduTemp.CanData[0] = CAN_RX1DR0;
            CAN_strRxPduTemp.CanData[1] = CAN_RX1DR1;
            CAN_strRxPduTemp.CanData[2] = CAN_RX1DR2;
            CAN_strRxPduTemp.CanData[3] = CAN_RX1DR3;
            CAN_strRxPduTemp.CanData[4] = CAN_RX1DR4;
            CAN_strRxPduTemp.CanData[5] = CAN_RX1DR5;
            CAN_strRxPduTemp.CanData[6] = CAN_RX1DR6;
            CAN_strRxPduTemp.CanData[7] = CAN_RX1DR7;
            break;
            
        case (RDPTR1 | 0):
            CAN_strRxPduTemp.CAN_IDE = ( CAN_RX2CR & 0x80 );
            CAN_strRxPduTemp.CAN_RTR = ( CAN_RX2CR & 0x40 );
            CAN_strRxPduTemp.CAN_DLC = ( CAN_RX2CR & 0x0F );
            
            if (CAN_strRxPduTemp.CAN_IDE)
            {
                CAN_strRxPduTemp.u32CanId = 0;
                CAN_strRxPduTemp.u32CanId |= (uint32)CAN_RX2ID0 << 21;
                CAN_strRxPduTemp.u32CanId |= (uint32)CAN_RX2ID1 << 13;
                CAN_strRxPduTemp.u32CanId |= (uint32)CAN_RX2ID2 << 5 ;
                CAN_strRxPduTemp.u32CanId |= (uint32)CAN_RX2ID3 >> 4 ;
            }
            else
            {
                CAN_strRxPduTemp.u32CanId = 0;
                CAN_strRxPduTemp.u32CanId |= CAN_RX2ID0 << 3 ;
                CAN_strRxPduTemp.u32CanId |= CAN_RX2ID1 >> 5;
            }
            
            CAN_strRxPduTemp.CanData[0] = CAN_RX2DR0;
            CAN_strRxPduTemp.CanData[1] = CAN_RX2DR1;
            CAN_strRxPduTemp.CanData[2] = CAN_RX2DR2;
            CAN_strRxPduTemp.CanData[3] = CAN_RX2DR3;
            CAN_strRxPduTemp.CanData[4] = CAN_RX2DR4;
            CAN_strRxPduTemp.CanData[5] = CAN_RX2DR5;
            CAN_strRxPduTemp.CanData[6] = CAN_RX2DR6;
            CAN_strRxPduTemp.CanData[7] = CAN_RX2DR7;
            break;
            
        default:
            u8LocValidFrmFlg = STD_FALSE;
            break;
    }

    if (STD_TRUE == u8LocValidFrmFlg)
    {
       CANIF_vidReceiveProcess(CAN_strRxPduTemp.u32CanId, CAN_strRxPduTemp.CAN_DLC, &CAN_strRxPduTemp.CanData[0]);
    }
    
#if (CAN_ISR_ENABLE == STD_OFF)
    /* 如果开了中断请注释这句 */
    SetBit(CAN_CR1, BUFRLS); 
#endif

}


/* 函数名称：CAN_vidSendMsg
 * 函数功能：发送CAN数据
 * 参数说明：
 * 注意事项：无
 */
void CAN_vidSendMsg(uint8* pu8CanData, CAN_tstrSendHeadInfo* pstrCanSendHead)
{
    uint16 u16WaitTimer;

    u16WaitTimer = 0;
    while ( (0 == ReadBit(CAN_SR, TXDONE) ) && (u16WaitTimer < CAN_TXWAIT_TIMOUT) )
    {
        u16WaitTimer++;
    }
    
    CAN_TXCR  = pstrCanSendHead->u8CanFrameCfg;
    
    if (pstrCanSendHead->u8CanFrameCfg & IDE_EXT)
    {
        CAN_TXID0 = pstrCanSendHead->u32CanId >> 21;
        CAN_TXID1 = pstrCanSendHead->u32CanId >> 13;
        CAN_TXID2 = pstrCanSendHead->u32CanId >> 5;
        CAN_TXID3 = pstrCanSendHead->u32CanId << 4;
    }
    else
    {
        CAN_TXID0 = pstrCanSendHead->u32CanId >> 3;
        CAN_TXID1 = pstrCanSendHead->u32CanId << 5;
        CAN_TXID2 = 0;
        CAN_TXID3 = 0;
    }
    
    CAN_TXDR0 = pu8CanData[0];
    CAN_TXDR1 = pu8CanData[1];
    CAN_TXDR2 = pu8CanData[2];
    CAN_TXDR3 = pu8CanData[3];
    CAN_TXDR4 = pu8CanData[4];
    CAN_TXDR5 = pu8CanData[5];
    CAN_TXDR6 = pu8CanData[6];
    CAN_TXDR7 = pu8CanData[7];
    
    SetBit(CAN_CR1, TXREQ);
}

/* CAN BusOff状态读取 */
uint8 CAN_u8ErrState(void)
{
	if (0 != ReadBit(CAN_ESR, BOFF) )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}

}

/* 函数名称：CAN_vidSetCanError
 * 函数功能：CAN错误通过软件设置
 * 参数说明：CAN_ERR_NONE、CAN_ERR_REC_OV、CAN_ERR_BUSOFF等等
 * 注意事项：无
 */
void CAN_vidSetCanError(uint8 u8ErrorType)
{
   CAN_u8SoftErrorType = u8ErrorType;
}

/* 函数名称：CAN_vidStateMachine
 * 函数功能：CAN 状态管理状态机
 * 参数说明：无
 * 注意事项：调度周期10ms
 */
void CAN_vidStateMachine(void)
{
   switch(CAN_u8DrvState)
   {
      case CAN_SM_IDLE:
      {
         CAN_u8DrvState = CAN_SM_NORMAL;
      }
      break;

      case CAN_SM_NORMAL:
      {
         if (0 != ReadBit(CAN_ESR, BOFF) )
         {
            CAN_u8DrvState = CAN_SM_ERROR;
         }
         else if (0 != CAN_u8SoftErrorType)
         {
            CAN_u8DrvState = CAN_SM_ERROR;
         }
         else if (CAN_TEC >= 0x80)
         {
            CAN_u8DrvState = CAN_SM_ERROR;
         }
         else
         {}
      }
      break;

      case CAN_SM_ERROR:
      {
         /* CAN模块失能 */
         ClrBit(CAN_CR0, CANEN);
         CAN_u16StateTimCnt = 0;
         CAN_u8DrvState = CAN_SM_FAST_RECOVER;
      }
      break;

      case CAN_SM_FAST_RECOVER:
      {
         if (CAN_u16StateTimCnt < 10)
         {
            CAN_u16StateTimCnt++;
         }
         else
         {
            CAN_u16StateTimCnt = 0;
            CAN_TEC = 0;
            CAN_REC = 0;
            CAN_u8SoftErrorType = CAN_ERR_NONE;
            /* CAN模块使能 */
            SetBit(CAN_CR0, CANEN);
            CAN_u8DrvState = CAN_SM_NORMAL;
         }
      }
      break;

      case CAN_SM_SLOW_RECOVER:
      {
      }
      break;

      default:
         break;
   }
}

