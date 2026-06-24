#include "OnChipFlash.h"

/*-------------------------------------------------------------------------------------------------
    Function Name : uint8 Flash_Sector_Erase(uint8 xdata *FlashAddress)
    Description   : 扇区自擦除: 指定将要擦除的Flash扇区，每个扇区128Byte，共128个扇区，
                    扇区0~127对应Flash地址0x0000~0x3fff，通过指定Flash地址来指定要擦除
                    的Flash地址所在扇区。一次只能擦除一个扇区，自擦除数据为任意值，一定
                    要在解锁后才给DPTR赋值。
    Input         : FlashAddress--Flash自擦除扇区内任意地址
    Output        : 0--Flash自擦除成功，1--Flash自擦除失败
-------------------------------------------------------------------------------------------------*/
uint8 Flash_Sector_Erase(uint8 xdata *FlashAddress)
{
    bool TempEA;

    TempEA = EA;
    EA = 0;                            //Flash自擦除前先关总中断

    if(FlashAddress < 0x7F00)          // 不擦除最后一个扇区,0X7F00~0X7FFF
    {
        FLA_CR = 0x03;                 //使能自擦除
        FLA_KEY = 0x5a;
        FLA_KEY = 0x1f;                //flash预编程解锁
        _nop_();
        *FlashAddress = 0xff;          //写任意数据
        FLA_CR = 0x08;                 //开始预编程，完成后Flash再次上锁
    }

    EA = TempEA;                       //Flash自擦除后总中断恢复 

    if(ReadBit(FLA_CR, FLAERR))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/*-------------------------------------------------------------------------------------------------
    Function Name : uint8 Flash_Sector_Write(uint8 xdata *FlashAddress, uint8 FlashData)
    Description   : Flash自烧写: 对扇区预编程和自擦除后，可以对扇区内的地址进行Flash烧写，
                    一次烧写一个byte,一定要在解锁后才给DPTR赋值
    Input         : FlashAddress--Flash烧写地址
                    FlashData--Flash烧写数据
    Output        : 0--Flash自烧写成功，1--Flash自烧写失败
-------------------------------------------------------------------------------------------------*/
uint8 Flash_Sector_Write(uint8 xdata *FlashAddress, uint8 FlashData)
{
    bool TempEA;

    TempEA = EA;                       //Flash自烧写前先关总中断
    EA = 0;

  if(FlashAddress < 0x7F00)            // 不编程最后一个扇区,0X7F00~0X7FFF
    {
        FLA_CR = 0x01;                 // 使能Flash编程
        FLA_KEY = 0x5a;
        FLA_KEY = 0x1f;                // flash预编程解锁
        _nop_();
        *FlashAddress = FlashData;     // 写编程数据
        FLA_CR = 0x08;                 // 开始预编程，完成后Flash再次上锁
    }

    EA = TempEA;                       //Flash自烧写后总中断恢复 

    if(ReadBit(FLA_CR, FLAERR))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/*-------------------------------------------------------------------------------------------------
	Function Name :	void WriteData2Flash(uint8 xdata *BlockStartAddr,uint16 NewData2Flash)
	Description   :	дȫ2¸öז½ڵ½FLASH
	Input         :	uint8 xdata *BlockStartAddr£ºĿ±ꆌASHµؖ·  NewData2Flash£º±»дȫʽ¾ݍ
    Output		  :	1:ɈǸδº,дȫͪ³ɠ 0:ɈǸґº,дȫʧ°܍
-------------------------------------------------------------------------------------------------*/
uint8 Write2Byte2Flash(uint8 xdata *BlockStartAddr,uint16 NewData2Flash)
{
	uint8 xdata *FlashStartAddr = BlockStartAddr;
	uint16 tempofFlashData=0;
	uint16 tempofNewFlashData=0;
	uint8 i;
	
	tempofNewFlashData = NewData2Flash;
	
	for(i=0;i<128;i++)
	{
		tempofFlashData = *(uint16 code *)(FlashStartAddr+2*i);
		if(tempofFlashData==0)
		{
			tempofFlashData = tempofNewFlashData>>8;                            //´ýдȫʽ¾݊חֽڍ
			Flash_Sector_Write((FlashStartAddr+2*i),(uint8)tempofFlashData);
			_nop_();
			tempofFlashData = tempofNewFlashData&0x00ff;                        //´ýдȫʽ¾݄©ז½ڍ
			Flash_Sector_Write((FlashStartAddr+2*i+1),(uint8)tempofFlashData);
			_nop_();		

			return 1;
		}
		else
		{
			if(i==127)
			{
				return 0;
			}
		}
	}
	return 0;
}

/*-------------------------------------------------------------------------------------------------
	Function Name :	uint16 GetLastDataFromFlash(uint8 xdata *BlockStartAddr)
	Description   :	´ӄ¿±ꆌASHɈǸ¶Áȡ2ז½ڗдȫµĊý¾ݍ
	Input         :	uint8 xdata *BlockStartAddr£ºĿ±ꆌASHɈǸ
	Output        :	¶Á³öµĊý¾ݍ
-------------------------------------------------------------------------------------------------*/
uint16 Get2ByteFromFlash(uint8 xdata *BlockStartAddr)
{
	uint8 xdata *FlashStartAddr = BlockStartAddr;
	uint8 i;
	uint16 tempofFlashData;
	
	for(i=0;i<128;i++)
	{
		tempofFlashData = *(uint16 code *)(FlashStartAddr+2*i);
		if(tempofFlashData==0)
		{
			if(i!=0)
			{
				tempofFlashData = *(uint16 code *)(FlashStartAddr+2*(i-1));
				return tempofFlashData;
			}
			else
			{
				return 0;
			}
		}
		else
		{
			if(i==127)
			{
				return tempofFlashData;
			}
		}	
	}
	return 0;
}