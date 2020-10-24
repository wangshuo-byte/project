//#include "sys.h"
//#include "stm32f10x.h"
//#include "data_flash.h"
//u32 StartAddr;//0x0800 0000~0x800 3000��Ϊbootloaderʹ�ã�
//u16 size;
//u16 p_data; 
//u16 data_temp[256];//0x0800 C000~0x0800 FFFF��Ϊ���ݴ������
//u8 STMFLASH_GetStatus(void)//�鿴FLASH״̬
//{
//	u32 res;
//	res=FLASH->SR; 
//	if(res&(1<<0))
//		return 1; //æ
//	else if(res&(1<<2))return 2; //��̴���
//	else if(res&(1<<4))return 3; //д��������
//	return 0; //���
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
//void STMFLASH_Unlock(void)//����
//{
//	FLASH->KEYR=0X45670123;//д���������
//	FLASH->KEYR=0XCDEF89AB;
//}
//void STMFLASH_Lock(void)//����
//{
//	FLASH->CR|=1<<7; //����
//}
//u16 FLASH_read(u32 faddr)
//{
//	return *(vu16*)faddr; 
//}
//u8 FLASH_WIPE(u32 paddr)//����
//{
//	u8 res=0;
//	res=STMFLASH_WaitDone(0x000B0000);//�ȴ��ϴβ�������,>20ms 
//	if(res==STMFLASH_GetStatus())
//	{
//		FLASH->CR|=0x00000002;
//		FLASH->AR=paddr;
//		FLASH->CR|=0x00000040;
//		res=STMFLASH_WaitDone(0x000B0000); //�ȴ��������,>20ms
//		res=STMFLASH_GetStatus();//����
//	    FLASH->CR&=0x00001FFD; //���ҳ������־
//	}
//	return res;
//}
//u8 flash_write(u32 ADDRESS,u16 p_data)//��ָ��λ��д�����
//{
//	u8 res=0; 
//	volatile unsigned int tmp=0;
//	if((ADDRESS>= 0x08000000) && ((ADDRESS) < 0x080FFFFF))
//	{
//		res=STMFLASH_WaitDone(0x00002000);//�ȴ��ϴβ�������,
//		if(res==STMFLASH_GetStatus())//����
//		{
//			FLASH->CR|=0x00000001;//���ʹ��
//			*(vu16*)ADDRESS=p_data;
//			res=STMFLASH_WaitDone(0x00002000);//�ȴ��ϴβ�������
//			if(res==STMFLASH_GetStatus())//���� д���
//			{
//				tmp=ADDRESS+2;
//				*(vu16*)tmp=p_data>>16;
//				res=STMFLASH_WaitDone(0x00002000);//�ȴ��ϴβ�������
//				FLASH->CR&=0x00001FFE; //��ն�Ӧλ
//			}
//			else{
//				FLASH->CR&=0x00001FFE; //��ն�Ӧλ
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
//	    WriteAddr+=2;//��ַ����2.
//	}  
//} 
////��ָ����ַ��ʼ����ָ�����ȵ�����
////ReadAddr:��ʼ��ַ
////pBuffer:����ָ��
////NumToWrite:����(16λ)��
//void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead)   	
//{
//	u16 i;
//	for(i=0;i<NumToRead;i++)
//	{
//		pBuffer[i]=FLASH_read(ReadAddr);//��ȡ2���ֽ�.
//		ReadAddr+=2;//ƫ��2���ֽ�.	
//	}
//}
//u8 STMFLASH_Write(u32 StartAddr,u16*p_data,u16 size)	
//{
//	volatile unsigned int FLASHstatus;
//	u32 EndAddr=StartAddr+size*4;
//	vu32 NbrOfPage = 0;//��¼Ҫ������ҳ
//	u32 EraseCounter = 0x0, Address = 0x0;
//	int i;
//	int MemoryProgramStatus=1;
//	//Ϊһ��ͨ��
//	STMFLASH_Unlock();//����
//	NbrOfPage=((EndAddr-StartAddr)>>10)+1;//�ж��ٸ�ҳ������//����������б�־
//	FLASH->SR=(0x00000001 | 0x00000020 | 0x00000004 | 0x00000010);//�����ر�־λ
//	//��ҳ
//	FLASHstatus=STMFLASH_GetStatus(); 
//	for(EraseCounter=0;(EraseCounter<NbrOfPage)&&(STMFLASH_GetStatus()==0);EraseCounter++)
//	{ 
//	  FLASHstatus=FLASH_WIPE(StartAddr+(PageSize*EraseCounter));
//	}
//	//��ʼд����
//	Address = StartAddr;
//	i=0;
//	while((Address<EndAddr)&&(STMFLASH_GetStatus()==0))
//	{ 
//		FLASHstatus=flash_write(Address,p_data[i++]);
//		Address=Address+4;
//	}
//	//���������Ƿ����
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

