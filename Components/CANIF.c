/**
 * @copyright (C) COPYRIGHT 2024 Fortiortech Shenzhen
 * @file      CANIF.c
 * @author    Brad Yang, R&D
 * @since     2024-08-27 15:24:44
 * @date      2024-09-10 15:31:23
 * @note      Last modify author is Brad Yang, R&D
 * @version
 * <table>
 * <tr><th>Date <th>Version <th>Author <th>Description </tr>
 * </table>
 * @brief
 */

/************************************Including Header Files***********************************/
#include <MyProject.h>


/************************************Define Macro*********************************************/
/************************************Define Global Symbols************************************/

/* 发送PDU头信息定义 */
CAN_tstrSendHeadInfo xdata CANIF_strS31Mcu1Head;
CAN_tstrSendHeadInfo xdata CANIF_strS31Mcu2Head;
CAN_tstrSendHeadInfo xdata CANIF_strS31Mcu3Head;

/* CAN发送数据缓冲区 */
uint8 xdata CAN_data_TX_1[8];
uint8 xdata CAN_data_TX_2[8];
uint8 xdata CAN_data_TX_3[8];

/* 接收PDU头信息定义 */
CANIF_tstrRxHeadInfo xdata CANIF_strS31Pdcu1Head;
CANIF_tstrRxHeadInfo xdata CANIF_strS31Pdcu2Head;
CANIF_tstrRxHeadInfo xdata CANIF_strS31Pdcu3Head;

/* CAN接收数据缓冲区 */
uint8 xdata CAN_data_RX_1[8];
uint8 xdata CAN_data_RX_2[8];
uint8 xdata CAN_data_RX_3[8];


#if (UDS_COMSTACK_ENABLE == STD_ON)
uint8 xdata CANIF_au8UdsPhyRxData[8];
uint8 xdata CANIF_au8UdsRespData[8];
#endif

/* CAN发送周期计数器 */
uint16 xdata CANIF_u16TxPeriodCnt = 0;

/***************************Function Subject***************************************************/

/******************************************************************************/
/*                                                                            */
/* !Description : CAN报文初始化                                                    */
/* !Reference   :                                                             */
/* !Trace_To    :                                                             */
/* !LastAuthor  :                                                             */
/******************************************************************************/
/* !Comment:                                                                  */
/* !Range   :                                                                 */
/******************************************************************************/
void CANIF_vidDataInit(void)
{
    CANIF_u16TxPeriodCnt = 0;

    /* !Comment: 发送帧头信息初始化. */
	CANIF_strS31Mcu1Head.u32CanId = 0xE1;               // CAN报文ID - 发送
	CANIF_strS31Mcu1Head.u8CanFrameCfg = IDE_STD | RTR_DAT | DLC_8;

	CANIF_strS31Mcu2Head.u32CanId = 0xE2;               // CAN报文ID - 发送
	CANIF_strS31Mcu2Head.u8CanFrameCfg = IDE_STD | RTR_DAT | DLC_8;

	CANIF_strS31Mcu3Head.u32CanId = 0xE3;               // CAN报文ID - 发送
	CANIF_strS31Mcu3Head.u8CanFrameCfg = IDE_STD | RTR_DAT | DLC_8;

    /* !Comment: 接收帧头信息初始化. */
	CANIF_strS31Pdcu1Head.RxAckFlg    = FALSE;
	CANIF_strS31Pdcu1Head.RxTimoutFlg = FALSE;
	CANIF_strS31Pdcu1Head.RxTimoutCnt = CANIF_COM_FIRST_TIMOUT;
	
	CANIF_strS31Pdcu2Head.RxAckFlg    = FALSE;
	CANIF_strS31Pdcu2Head.RxTimoutFlg = FALSE;
	CANIF_strS31Pdcu2Head.RxTimoutCnt = CANIF_COM_FIRST_TIMOUT;

	CANIF_strS31Pdcu3Head.RxAckFlg    = FALSE;
	CANIF_strS31Pdcu3Head.RxTimoutFlg = FALSE;
	CANIF_strS31Pdcu3Head.RxTimoutCnt = CANIF_COM_FIRST_TIMOUT;
}


/******************************************************************************/
/*                                                                            */
/* !Description : CAN接收处理函数                                                   */
/* !Reference   :                                                             */
/* !Trace_To    :                                                             */
/* !LastAuthor  :                                                             */
/******************************************************************************/
/* !Comment:                                                                  */
/* !Range   :                                                                 */
/******************************************************************************/
void CANIF_vidReceiveProcess(uint32 u32CanId, uint8 u8CanDlc, uint8* pu8CanData)
{    
    uint8 u8LocIdx;
    
    // CAN报文ID - 接收
    switch (u32CanId)
    {
        case 0XBD:
        {
            memcpy(CAN_data_RX_1, pu8CanData, sizeof(CAN_data_RX_1));
            CANIF_strS31Pdcu1Head.RxAckFlg = STD_TRUE;
					  memcpy(CAN_data_TX_1, pu8CanData, 8);
        }
        break;

        case 0XBE:
        {
            memcpy(CAN_data_RX_2, pu8CanData, sizeof(CAN_data_RX_2));
            CANIF_strS31Pdcu2Head.RxAckFlg = STD_TRUE;
        }
        break;

        case 0XBF:
        {
            memcpy(CAN_data_RX_3, pu8CanData, sizeof(CAN_data_RX_3));
            CANIF_strS31Pdcu3Head.RxAckFlg = STD_TRUE;
        }
        break;

#if (UDS_COMSTACK_ENABLE == STD_ON)
        case CANIF_UDS_PHYRX_ID:
            {
                pu8Dest = (uint8*)&CANIF_au8UdsPhyRxData;
                for (u8LocIdx = 0; u8LocIdx < 8; u8LocIdx++)
                {
                    pu8Dest[u8LocIdx] = pu8CanData[u8LocIdx];
                }
                
                CANIF_vidUdsRxIndication(CANIF_UDS_PHYRX_ID, 8, &CANIF_au8UdsPhyRxData);
            }
            break;
#endif

        default:
            break;
    }

}


/******************************************************************************/
/*                                                                            */
/* !Description : CAN应用报文发送处理函数                                               */
/* !Reference   :                                                             */
/* !Trace_To    :                                                             */
/* !LastAuthor  :                                                             */
/******************************************************************************/
/* !Comment:                                                                  */
/* !Range   :                                                                 */
/******************************************************************************/
void CANIF_vidTransProcess(void)
{
    if (CANIF_u16TxPeriodCnt < 10)
    {
       CANIF_u16TxPeriodCnt++;
    }
    else
    {
       CANIF_u16TxPeriodCnt = 0;
    }

    switch (CANIF_u16TxPeriodCnt)
    {
        case 0:
            CAN_vidSendMsg(&CAN_data_TX_1, &CANIF_strS31Mcu1Head);
            break;
           
        case 2:
            CAN_vidSendMsg(&CAN_data_TX_2, &CANIF_strS31Mcu2Head);
            break;
       
        case 4:
            CAN_vidSendMsg(&CAN_data_TX_3, &CANIF_strS31Mcu3Head);
            break;

        case 6:
            // CAN_vidSendMsg(&CAN_data_TX_4, &CANIF_strS31Mcu4Head);
            break;
           
        case 8:
            // CAN_vidSendMsg(&CAN_data_TX_5, &CANIF_strS31Mcu5Head);
            break;

        default:
            break;
    }

}

/******************************************************************************/
/*                                                                            */
/* !Description : CAN应用报文超时处理函数                                               */
/* !Reference   :                                                             */
/* !Trace_To    :                                                             */
/* !LastAuthor  :                                                             */
/******************************************************************************/
/* !Comment: 调度周期50ms                                                         */
/* !Range   :                                                                 */
/******************************************************************************/
void CANIF_vidComLostDiag(void)
{
    if (CANIF_strS31Pdcu1Head.RxAckFlg)
    {
        CANIF_strS31Pdcu1Head.RxAckFlg    = FALSE;
        CANIF_strS31Pdcu1Head.RxTimoutFlg = FALSE;
        CANIF_strS31Pdcu1Head.RxTimoutCnt = CANIF_COM_PDCU1_TIMOUT;
    }
    else
    {
        if (CANIF_strS31Pdcu1Head.RxTimoutCnt > 0)
        {
            CANIF_strS31Pdcu1Head.RxTimoutCnt--;
        }
        else
        {
            CANIF_strS31Pdcu1Head.RxTimoutFlg = TRUE;
        }
    }

    if (CANIF_strS31Pdcu2Head.RxAckFlg)
    {
        CANIF_strS31Pdcu2Head.RxAckFlg    = FALSE;
        CANIF_strS31Pdcu2Head.RxTimoutFlg = FALSE;
        CANIF_strS31Pdcu2Head.RxTimoutCnt = CANIF_COM_PDCU2_TIMOUT;
    }
    else
    {
        if (CANIF_strS31Pdcu2Head.RxTimoutCnt > 0)
        {
            CANIF_strS31Pdcu2Head.RxTimoutCnt--;
        }
        else
        {
            CANIF_strS31Pdcu2Head.RxTimoutFlg = TRUE;
        }
    }

    if (CANIF_strS31Pdcu3Head.RxAckFlg)
    {
        CANIF_strS31Pdcu3Head.RxAckFlg    = FALSE;
        CANIF_strS31Pdcu3Head.RxTimoutFlg = FALSE;
        CANIF_strS31Pdcu3Head.RxTimoutCnt = CANIF_COM_PDCU3_TIMOUT;
    }
    else
    {
        if (CANIF_strS31Pdcu3Head.RxTimoutCnt > 0)
        {
            CANIF_strS31Pdcu3Head.RxTimoutCnt--;
        }
        else
        {
            CANIF_strS31Pdcu3Head.RxTimoutFlg = TRUE;
        }
    }

    if ( (TRUE == CANIF_strS31Pdcu1Head.RxTimoutFlg) || 
         (TRUE == CANIF_strS31Pdcu2Head.RxTimoutFlg) ||
         (TRUE == CANIF_strS31Pdcu3Head.RxTimoutFlg) )
    {
        if (mcRun == mcState)
        {
        /* 若需要采取报文超时保护，可以在这里添加保护代码。 */
        #if 0
            mcFaultSource = FaultCANTimout;
            mcState = mcFault;
            FaultProcess();
        #endif
        }
    }
}


/**
 * @brief      CAN测试函数
 *
 * @param[in]  MCU_Data_Number  The mcu data number
 */
void CANIF_vidTestCanSend(void)
{
    CAN_data_TX_1[0] = 0X11;
    CAN_data_TX_1[1] = 0X22;
    CAN_data_TX_1[2] = 0X33;
    CAN_data_TX_1[3] = 0X44;
    CAN_data_TX_1[4] = 0X55;
    CAN_data_TX_1[5] = 0X66;
    CAN_data_TX_1[6] = 0X77;
    CAN_data_TX_1[7] = 0X88;
}
