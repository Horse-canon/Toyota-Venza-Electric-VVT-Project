#ifndef _OnChipFlash_H__
#define _OnChipFlash_H__

#include <MyProject.h>

/*-------------------------------------------------------------------------------------------------
    Function Name : uint8 Flash_Sector_Erase(uint8 xdata *FlashAddress)
    Description   : 扇区自擦除: 指定将要擦除的Flash扇区，每个扇区128Byte，共128个扇区，
                    扇区0~127对应Flash地址0x0000~0x3fff，通过指定Flash地址来指定要擦除
                    的Flash地址所在扇区。一次只能擦除一个扇区，自擦除数据为任意值，一定
                    要在解锁后才给DPTR赋值。
    Input         : FlashAddress--Flash自擦除扇区内任意地址
    Output        : 0--Flash自擦除成功，1--Flash自擦除失败
-------------------------------------------------------------------------------------------------*/
uint8 Flash_Sector_Erase(uint8 xdata *FlashAddress);

/*-------------------------------------------------------------------------------------------------
    Function Name : uint8 Flash_Sector_Write(uint8 xdata *FlashAddress, uint8 FlashData)
    Description   : Flash自烧写: 对扇区预编程和自擦除后，可以对扇区内的地址进行Flash烧写，
                    一次烧写一个byte,一定要在解锁后才给DPTR赋值
    Input         : FlashAddress--Flash烧写地址
                    FlashData--Flash烧写数据
    Output        : 0--Flash自烧写成功，1--Flash自烧写失败
-------------------------------------------------------------------------------------------------*/
uint8 Flash_Sector_Write(uint8 xdata *FlashAddress, uint8 FlashData);

/*-------------------------------------------------------------------------------------------------
	Function Name :	void WriteData2Flash(uint8 xdata *BlockStartAddr,uint16 NewData2Flash)
	Description   :	дȫ2¸öז½ڵ½FLASH
	Input         :	uint8 xdata *BlockStartAddr£ºĿ±ꆌASHµؖ·  NewData2Flash£º±»дȫʽ¾ݍ
    Output		  :	1:ɈǸδº,дȫͪ³ɠ 0:ɈǸґº,дȫʧ°܍
-------------------------------------------------------------------------------------------------*/
uint8 Write2Byte2Flash(uint8 xdata *BlockStartAddr,uint16 NewData2Flash);

/*-------------------------------------------------------------------------------------------------
	Function Name :	uint16 GetLastDataFromFlash(uint8 xdata *BlockStartAddr)
	Description   :	´ӄ¿±ꆌASHɈǸ¶Áȡ2ז½ڗдȫµĊý¾ݍ
	Input         :	uint8 xdata *BlockStartAddr£ºĿ±ꆌASHɈǸ
	Output        :	¶Á³öµĊý¾ݍ
-------------------------------------------------------------------------------------------------*/
uint16 Get2ByteFromFlash(uint8 xdata *BlockStartAddr);

#endif