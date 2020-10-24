//#include "sys.h"
//#include "stm32f10x.h"
//#include "data_flash.h"
//u32 StartAddr;//0x0800 0000~0x800 3000作为bootloader使用，
//u16 size;
//u16 p_data; 
//u16 data_temp[256];//0x0800 C000~0x0800 FFFF作为数据存放区域。
//u8 STMFLASH_GetStatus(void)//查看FLASH状态
//{
//	u32 res;
//	res=FLASH->SR; 
//	if(res&(1<<0))
//		return 1; //忙
//	else if(res&(1<<2))return 2; //编程错误
//	else if(res&(1<<4))return 3; //写保护错误
//	return 0; //完成
//}
//u8 STMFLASH_WaitDone(u32 time)
//{
//	u8 res;
//	do
//	{
//		res=STMFLASH_GetStatus();
//		if(res!=1)
//			break; //
//		delay_us(1);
//		time--;
//	}
//	while(time);
//	if(time==0)
//		res=0xff; //TIMEOUT
//	return res;
//}
//void STMFLASH_Unlock(void)//解锁
//{
//	FLASH->KEYR=0X45670123;//写入解锁序列
//	FLASH->KEYR=0XCDEF89AB;
//}
//void STMFLASH_Lock(void)//上锁
//{
//	FLASH->CR|=1<<7; //上锁
//}
//u16 FLASH_read(u32 faddr)
//{
//	return *(vu16*)faddr; 
//}
//u8 FLASH_WIPE(u32 paddr)//擦除
//{
//	u8 res=0;
//	res=STMFLASH_WaitDone(0x000B0000);//等待上次操作结束,>20ms 
//	if(res==STMFLASH_GetStatus())
//	{
//		FLASH->CR|=0x00000002;
//		FLASH->AR=paddr;
//		FLASH->CR|=0x00000040;
//		res=STMFLASH_WaitDone(0x000B0000); //等待操作完成,>20ms
//		res=STMFLASH_GetStatus();//空闲
//	    FLASH->CR&=0x00001FFD; //清除页擦出标志
//	}
//	return res;
//}
//u8 flash_write(u32 ADDRESS,u16 p_data)//在指定位置写入半字
//{
//	u8 res=0; 
//	volatile unsigned int tmp=0;
//	if((ADDRESS>= 0x08000000) && ((ADDRESS) < 0x080FFFFF))
//	{
//		res=STMFLASH_WaitDone(0x00002000);//等待上次操作结束,
//		if(res==STMFLASH_GetStatus())//空闲
//		{
//			FLASH->CR|=0x00000001;//编程使能
//			*(vu16*)ADDRESS=p_data;
//			res=STMFLASH_WaitDone(0x00002000);//等待上次操作结束
//			if(res==STMFLASH_GetStatus())//空闲 写完成
//			{
//				tmp=ADDRESS+2;
//				*(vu16*)tmp=p_data>>16;
//				res=STMFLASH_WaitDone(0x00002000);//等待上次操作结束
//				FLASH->CR&=0x00001FFE; //清空对应位
//			}
//			else{
//				FLASH->CR&=0x00001FFE; //清空对应位
//			}
//		}
//	}
//	return res;
//}
//void STMFLASH_Write_NoCheck(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)   
//{ 			 		 
//	u16 i;
//	for(i=0;i<NumToWrite;i++)
//	{
//		flash_write(WriteAddr,pBuffer[i]);
//	    WriteAddr+=2;//地址增加2.
//	}  
//} 
////从指定地址开始读出指定长度的数据
////ReadAddr:起始地址
////pBuffer:数据指针
////NumToWrite:半字(16位)数
//void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead)   	
//{
//	u16 i;
//	for(i=0;i<NumToRead;i++)
//	{
//		pBuffer[i]=FLASH_read(ReadAddr);//读取2个字节.
//		ReadAddr+=2;//偏移2个字节.	
//	}
//}
//u8 STMFLASH_Write(u32 StartAddr,u16*p_data,u16 size)	
//{
//	volatile unsigned int FLASHstatus;
//	u32 EndAddr=StartAddr+size*4;
//	vu32 NbrOfPage = 0;//记录要擦除的页
//	u32 EraseCounter = 0x0, Address = 0x0;
//	int i;
//	int MemoryProgramStatus=1;
//	//为一是通过
//	STMFLASH_Unlock();//解锁
//	NbrOfPage=((EndAddr-StartAddr)>>10)+1;//有多少个页被擦除//清除所有已有标志
//	FLASH->SR=(0x00000001 | 0x00000020 | 0x00000004 | 0x00000010);//清除相关标志位
//	//擦页
//	FLASHstatus=STMFLASH_GetStatus(); 
//	for(EraseCounter=0;(EraseCounter<NbrOfPage)&&(STMFLASH_GetStatus()==0);EraseCounter++)
//	{ 
//	  FLASHstatus=FLASH_WIPE(StartAddr+(PageSize*EraseCounter));
//	}
//	//开始写数据
//	Address = StartAddr;
//	i=0;
//	while((Address<EndAddr)&&(STMFLASH_GetStatus()==0))
//	{ 
//		FLASHstatus=flash_write(Address,p_data[i++]);
//		Address=Address+4;
//	}
//	//检验数据是否出错
//	Address = StartAddr;
//	i=0;
//	while((Address < EndAddr) && (MemoryProgramStatus != 0))
//	{ 
//		if((*(vu32*) Address) != p_data[i++])
//	{ 
//		MemoryProgramStatus = 0;
//		return 1;
//	}
//		Address += 4;
//	}
////	STMFLASH_Lock();
//	return 0;
//}
//int flash_read(u32 StartAddr,u16*p_data,u16 size)
//{
//	u32 EndAddr=StartAddr+size*4;
//	int MemoryProgramStatus=1;
//	u32 Address = 0x0;
//	int i=0;
//	Address = StartAddr;
//	while((Address < EndAddr) && (MemoryProgramStatus != 0))
//	{
//		p_data[i++]=(*(vu32*) Address);
//		Address += 4; 
//	}
//	return 0; 
//}

