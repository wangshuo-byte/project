//#include "oled.h"
//#include "data_upper.h"
//#include "usart.h"
//#include "mpu6050.h"
//#include "data_flash.h"
//#include "oled.h"
//#include "allcontrol.h"	
///////////////////////////////////////////////////////////////////////////////////////
////���ݲ�ֺ궨�壬�ڷ��ʹ���1�ֽڵ���������ʱ������int16��float�ȣ���Ҫ�����ݲ�ֳɵ����ֽڽ��з���
//#define BYTE0(dwTemp)       ( *( (char *)(&dwTemp)      ) )
//#define BYTE1(dwTemp)       ( *( (char *)(&dwTemp) + 1) )
//#define BYTE2(dwTemp)       ( *( (char *)(&dwTemp) + 2) )
//#define BYTE3(dwTemp)       ( *( (char *)(&dwTemp) + 3) )
//dt_flag_t f;                    //��Ҫ�������ݵı�־
//u8 data_to_send[50];//�������ݻ���
////u8 TEXT_Buffer[5];
//u8 p;
//u8 i;
//u8 d;
//u8 a;
//u8 b;
//short gy;
//extern float data1[10];
//u8 data2[10];
//void USART_SendData(uint16_t Data)
//{
//    USART1->DR = (Data & (uint16_t)0x01FF);
//	
//}
////��ֲʱ���û�Ӧ��������Ӧ�õ����������ʹ�õ�ͨ�ŷ�ʽ��ʵ�ִ˺���
//void usart2_send_char(u8 c)
//{   	
//	while(!(USART1->SR & (1 << 7)));//�����Ѿ���ת�Ƶ���λ�Ĵ��� //ѭ������,ֱ���������   
//	USART_SendData(c);  
//}
//void USART_CHAR(char *str)
//{
//	while(*str!='\0')
//	usart2_send_char(*str++);
//}
//void u32tostr(unsigned long dat,char *str) //���ڷ��͸��������� ���߾��Ȳ����á�
//{
//    char temp[20];
//    unsigned char i=0,j=0;
//    i=0;
//    while(dat)
//    {
//        temp[i]=dat%10+0x30;
//        i++;
//        dat/=10;
//    }
//    j=i;
//    for(i=0;i<j+1;i++)
//    {
//       str[i]=temp[j-i-1];
//		if(j-i-1==0)
//		{
//			i=i+1;
//			str[i]='\r';
//			i=i+1;
//			str[i]='\n';
//		}
//    }
//    if(!i) 
//	{
//		str[i++]='0';
//	}
//    str[i]=0;
//	USART_CHAR(str);
//}

//void usart2_niming_report(u8 fun,u8*data,u8 len)
//{
//	u8 data_to_send[33];
//	u8 i;
//	if(len>29)return;//���28�ֽ����� 
//	data_to_send[len+4]=0;//У��������
//	data_to_send[0]=0XAA;//֡ͷ
//	data_to_send[1]=0XAA;//֡ͷ
//	data_to_send[2]=fun;//������
//	data_to_send[3]=len;//���ݳ���
//	for(i=0;i<len;i++)data_to_send[4+i]=data[i];//��������
//	for(i=0;i<len+4;i++)data_to_send[len+4]+=data_to_send[i];//����У���	
//	for(i=0;i<len+5;i++)usart2_send_char(data_to_send[i]);//�������ݵ�����1 
//}

//static void ANO_DT_Send_Check(u8 head, u8 check_sum)
//{

//	data_to_send[0]=0xAA;
//    data_to_send[1]=0xAA;
//    data_to_send[2]=0xEF;
//    data_to_send[3]=2;
//    data_to_send[4]=head;
//    data_to_send[5]=check_sum;
//	
//    usart2_niming_report(0xEF,data_to_send,7);
//}
//void usart1_report_imu(float roll,float pitch,float yaw)
//{
//	u8 data_to_send[12];
//    vs16 _temp;
//    _temp = (int)(roll*100);
//    data_to_send[0]=BYTE1(_temp);
//    data_to_send[1]=BYTE0(_temp);
//    _temp = (int)(pitch*100);
//    data_to_send[2]=BYTE1(_temp);
//    data_to_send[3]=BYTE0(_temp);
//    _temp = (int)(yaw*100);
//    data_to_send[4]=BYTE1(_temp);
//    data_to_send[5]=BYTE0(_temp);
//		
//	_temp=0;
//	data_to_send[6]=BYTE1(_temp);
//    data_to_send[7]=BYTE0(_temp);
//	data_to_send[8]=BYTE1(_temp);
//    data_to_send[9]=BYTE0(_temp);
//		
//	_temp=0;
//	data_to_send[10]=BYTE1(_temp);
//		
//	_temp=0;
//	data_to_send[11]=BYTE1(_temp);
//    
//    usart2_niming_report(0x01,data_to_send,12);
//}

//void mpu6050_send_data(short aacx,short aacy,short aacz,short gyrox,short gyroy,short gyroz)
//{	
//	u8 data_to_send[18];
//    vs16 _temp;
//	u8 i;
//    gy=aacy;
//    for(i = 0;i< 18; i++) data_to_send[i] = 0;
//	_temp=aacx;
//	data_to_send[0]=BYTE1(_temp);
//    data_to_send[1]=BYTE0(_temp);
//    _temp = aacy;
//    data_to_send[2]=BYTE1(_temp);
//    data_to_send[3]=BYTE0(_temp);
//    _temp = aacz;    
//    data_to_send[4]=BYTE1(_temp);
//    data_to_send[5]=BYTE0(_temp);
//    
//    _temp = gyrox;    
//    data_to_send[6]=BYTE1(_temp);
//    data_to_send[7]=BYTE0(_temp);
//    _temp = gyroy;    
//    data_to_send[8]=BYTE1(_temp);
//    data_to_send[9]=BYTE0(_temp);
//    _temp = gyroz;
//    data_to_send[10]=BYTE1(_temp);
//    data_to_send[11]=BYTE0(_temp);
//	_temp=0;
//	data_to_send[12]=BYTE1(_temp);
//    data_to_send[13]=BYTE0(_temp);
//	_temp=0;
//	data_to_send[14]=BYTE1(_temp);
//    data_to_send[15]=BYTE0(_temp);
//	_temp=0;
//	data_to_send[16]=BYTE1(_temp);
//    data_to_send[17]=BYTE0(_temp);
//		
//	usart2_niming_report(0x02,data_to_send,18);
//}
//void ANO_DT_Send_PID()
//{
//		u8 data_to_send[18];
//    vs16 _temp;
//		u8 i;
//    
//		for(i = 0;i< 18; i++) data_to_send[i] = 0;
//    _temp = 1;
//    data_to_send[0]=BYTE1(_temp);
//    data_to_send[1]=BYTE0(_temp);
//    _temp = 1;
//    data_to_send[2]=BYTE1(_temp);
//    data_to_send[3]=BYTE0(_temp);
//    _temp =1;
//    data_to_send[4]=BYTE1(_temp);
//    data_to_send[5]=BYTE0(_temp);
//		
//		_temp =1;
//    data_to_send[6]=BYTE1(_temp);
//    data_to_send[7]=BYTE0(_temp);
//    _temp = 1;
//    data_to_send[8]=BYTE1(_temp);
//    data_to_send[9]=BYTE0(_temp);
//    _temp = 1;
//    data_to_send[10]=BYTE1(_temp);
//    data_to_send[11]=BYTE0(_temp);
//		
//		_temp =p;
//    data_to_send[12]=BYTE1(_temp);
//    data_to_send[13]=BYTE0(_temp);
//    _temp =i;
//    data_to_send[14]=BYTE1(_temp);
//    data_to_send[15]=BYTE0(_temp);
//    _temp =d;
//    data_to_send[16]=BYTE1(_temp);
//    data_to_send[17]=BYTE0(_temp);
//		
//   usart2_niming_report(0x10,data_to_send,18);
//}

/////////////////////////////////////////////////////////////////////////////////////
////Data_Receive_Prepare������Э��Ԥ����������Э��ĸ�ʽ�����յ������ݽ���һ�θ�ʽ�Խ�������ʽ��ȷ�Ļ��ٽ������ݽ���
////��ֲʱ���˺���Ӧ���û���������ʹ�õ�ͨ�ŷ�ʽ���е��ã����紮��ÿ�յ�һ�ֽ����ݣ�����ô˺���һ��
////�˺������������ϸ�ʽ������֡�󣬻����е������ݽ�������
//void ANO_DT_Data_Receive_Prepare(u8 data)
//{
//    static u8 RxBuffer[50];
//    static u8 _data_len = 0,_data_cnt = 0;
//    static u8 state = 0;
//    
//    if(state==0&&data==0xAA)//��һ����֡ͷ
//    {
//        state=1;
//        RxBuffer[0]=data;
//    }
//    else if(state==1&&data==0xAF)//�ڶ�����֡ͷ
//    {
//        state=2;
//        RxBuffer[1]=data;
//    }
//    else if(state==2&&data<0XF1)//�������Ź�����
//    {
//        state=3;
//        RxBuffer[2]=data;
//    }
//    else if(state==3&&data<50)//���ĸ����ݳ���
//    {
//        state = 4;
//        RxBuffer[3]=data;
//        _data_len = data;
//        _data_cnt = 0;
//    }
//    else if(state==4&&_data_len>0)//���ĸ���ʼ������
//    {
//        _data_len--;
//        RxBuffer[4+_data_cnt++]=data;
//        if(_data_len==0)
//        state = 5;
//    }
//    else if(state==5)//�������ݷ����һλ
//    {
//        state = 0;
//        RxBuffer[4+_data_cnt]=data;
//        ANO_DT_Data_Receive_Anl(RxBuffer,_data_cnt+5);
//    }
//    else
//        state = 0;
//}
/////////////////////////////////////////////////////////////////////////////////////
////Data_Receive_Anl������Э�����ݽ������������������Ƿ���Э���ʽ��һ������֡���ú��������ȶ�Э�����ݽ���У��
////У��ͨ��������ݽ��н�����ʵ����Ӧ����
////�˺������Բ����û����е��ã��ɺ���Data_Receive_Prepare�Զ�����
//void ANO_DT_Data_Receive_Anl(u8 *data_buf,u8 num)
//{
//    u8 sum = 0;
//	u8 i;
//    for(i=0;i<(num-1);i++)
//    sum += *(data_buf+i);
//    if(!(sum==*(data_buf+num-1))) return;     //�ж�sum
//    if(!(*(data_buf)==0xAA && *(data_buf+1)==0xAF))     return; //�ж�֡ͷ 
//    if(*(data_buf+2)==0X02)
//    {
//        if(*(data_buf+4)==0X01)//��ȡPID���� ����0XAA    
//        {
//					ANO_DT_Send_PID();
//        }
//        if(*(data_buf+4)==0X02)//��ȡ����ģʽ���� ����0XAA ����֡OA
//        {
//        }
//				
//        if(*(data_buf+4)==0XA0)     //��ȡ�汾��Ϣ
//        {
//					
//        }
//        if(*(data_buf+4)==0XA1)     //�ָ�Ĭ�ϲ���
//        {
//					
//        }
//    }
//    if(*(data_buf+2)==0X10)//PID1
//    {
//         Velocity_Kp = ( (vs16)(*(data_buf+4)<<8)|*(data_buf+5) );
//         Velocity_Ki =0.01* ( (vs16)(*(data_buf+6)<<8)|*(data_buf+7) );
//         b = 0.1*( (vs16)(*(data_buf+8)<<8)|*(data_buf+9) );
//         Balance_Kp = ( (vs16)(*(data_buf+10)<<8)|*(data_buf+11) );
//         a = 0.1*( (vs16)(*(data_buf+12)<<8)|*(data_buf+13) );
//         Balance_Kd = 0.01*( (vs16)(*(data_buf+14)<<8)|*(data_buf+15) );
////		 data1[0]=Velocity_Kp;
////	     data1[1]=Velocity_Ki;
////		 data1[2]=Balance_Kp;
////		 data1[3]=Balance_Kd;
////		 STMFLASH_Write(0X0800C000,(u16*)&data1[0],3);
////		 STMFLASH_Write(0X0800D000,(u16*)&data1[1],3);
////		 STMFLASH_Write(0X0800E000,(u16*)&data1[2],3);
////		 STMFLASH_Write(0X0800EC00,(u16*)&data1[3],3);
//		 OLED_ShowNum(0,2,Balance_Kp,3,16);		// ֱ����p
//		 OLED_ShowNum(50,2,Balance_Kd*100,3,16);	// ֱ����d
//		 OLED_ShowNum(0,4,Velocity_Kp,3,16);	//�ٶȻ�P
//		 OLED_ShowNum(50,4,Velocity_Ki*100,3,16);	//�ٶȻ�i
//		 p = 0.001*( (vs16)(*(data_buf+16)<<8)|*(data_buf+17) );
//		 i = 0.001*( (vs16)(*(data_buf+18)<<8)|*(data_buf+19) );
//		 d = 0.001*((vs16)(*(data_buf+20)<<8)|*(data_buf+21));
//		 ANO_DT_Send_Check(*(data_buf+2),sum);
//    }
//}



