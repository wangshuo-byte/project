//#include "oled.h"
//#include "data_upper.h"
//#include "usart.h"
//#include "mpu6050.h"
//#include "data_flash.h"
//#include "oled.h"
//#include "allcontrol.h"	
///////////////////////////////////////////////////////////////////////////////////////
////数据拆分宏定义，在发送大于1字节的数据类型时，比如int16、float等，需要把数据拆分成单独字节进行发送
//#define BYTE0(dwTemp)       ( *( (char *)(&dwTemp)      ) )
//#define BYTE1(dwTemp)       ( *( (char *)(&dwTemp) + 1) )
//#define BYTE2(dwTemp)       ( *( (char *)(&dwTemp) + 2) )
//#define BYTE3(dwTemp)       ( *( (char *)(&dwTemp) + 3) )
//dt_flag_t f;                    //需要发送数据的标志
//u8 data_to_send[50];//发送数据缓存
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
////移植时，用户应根据自身应用的情况，根据使用的通信方式，实现此函数
//void usart2_send_char(u8 c)
//{   	
//	while(!(USART1->SR & (1 << 7)));//数据已经被转移到移位寄存器 //循环发送,直到发送完毕   
//	USART_SendData(c);  
//}
//void USART_CHAR(char *str)
//{
//	while(*str!='\0')
//	usart2_send_char(*str++);
//}
//void u32tostr(unsigned long dat,char *str) //串口发送浮点型数据 ，高精度不可用。
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
//	if(len>29)return;//最多28字节数据 
//	data_to_send[len+4]=0;//校验数置零
//	data_to_send[0]=0XAA;//帧头
//	data_to_send[1]=0XAA;//帧头
//	data_to_send[2]=fun;//功能字
//	data_to_send[3]=len;//数据长度
//	for(i=0;i<len;i++)data_to_send[4+i]=data[i];//复制数据
//	for(i=0;i<len+4;i++)data_to_send[len+4]+=data_to_send[i];//计算校验和	
//	for(i=0;i<len+5;i++)usart2_send_char(data_to_send[i]);//发送数据到串口1 
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
////Data_Receive_Prepare函数是协议预解析，根据协议的格式，将收到的数据进行一次格式性解析，格式正确的话再进行数据解析
////移植时，此函数应由用户根据自身使用的通信方式自行调用，比如串口每收到一字节数据，则调用此函数一次
////此函数解析出符合格式的数据帧后，会自行调用数据解析函数
//void ANO_DT_Data_Receive_Prepare(u8 data)
//{
//    static u8 RxBuffer[50];
//    static u8 _data_len = 0,_data_cnt = 0;
//    static u8 state = 0;
//    
//    if(state==0&&data==0xAA)//第一个放帧头
//    {
//        state=1;
//        RxBuffer[0]=data;
//    }
//    else if(state==1&&data==0xAF)//第二个放帧头
//    {
//        state=2;
//        RxBuffer[1]=data;
//    }
//    else if(state==2&&data<0XF1)//第三个放功能字
//    {
//        state=3;
//        RxBuffer[2]=data;
//    }
//    else if(state==3&&data<50)//第四个数据长度
//    {
//        state = 4;
//        RxBuffer[3]=data;
//        _data_len = data;
//        _data_cnt = 0;
//    }
//    else if(state==4&&_data_len>0)//第四个开始是数据
//    {
//        _data_len--;
//        RxBuffer[4+_data_cnt++]=data;
//        if(_data_len==0)
//        state = 5;
//    }
//    else if(state==5)//最后的数据放最后一位
//    {
//        state = 0;
//        RxBuffer[4+_data_cnt]=data;
//        ANO_DT_Data_Receive_Anl(RxBuffer,_data_cnt+5);
//    }
//    else
//        state = 0;
//}
/////////////////////////////////////////////////////////////////////////////////////
////Data_Receive_Anl函数是协议数据解析函数，函数参数是符合协议格式的一个数据帧，该函数会首先对协议数据进行校验
////校验通过后对数据进行解析，实现相应功能
////此函数可以不用用户自行调用，由函数Data_Receive_Prepare自动调用
//void ANO_DT_Data_Receive_Anl(u8 *data_buf,u8 num)
//{
//    u8 sum = 0;
//	u8 i;
//    for(i=0;i<(num-1);i++)
//    sum += *(data_buf+i);
//    if(!(sum==*(data_buf+num-1))) return;     //判断sum
//    if(!(*(data_buf)==0xAA && *(data_buf+1)==0xAF))     return; //判断帧头 
//    if(*(data_buf+2)==0X02)
//    {
//        if(*(data_buf+4)==0X01)//读取PID请求 返回0XAA    
//        {
//					ANO_DT_Send_PID();
//        }
//        if(*(data_buf+4)==0X02)//读取飞行模式设置 返回0XAA 数据帧OA
//        {
//        }
//				
//        if(*(data_buf+4)==0XA0)     //读取版本信息
//        {
//					
//        }
//        if(*(data_buf+4)==0XA1)     //恢复默认参数
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
//		 OLED_ShowNum(0,2,Balance_Kp,3,16);		// 直立环p
//		 OLED_ShowNum(50,2,Balance_Kd*100,3,16);	// 直立环d
//		 OLED_ShowNum(0,4,Velocity_Kp,3,16);	//速度环P
//		 OLED_ShowNum(50,4,Velocity_Ki*100,3,16);	//速度环i
//		 p = 0.001*( (vs16)(*(data_buf+16)<<8)|*(data_buf+17) );
//		 i = 0.001*( (vs16)(*(data_buf+18)<<8)|*(data_buf+19) );
//		 d = 0.001*((vs16)(*(data_buf+20)<<8)|*(data_buf+21));
//		 ANO_DT_Send_Check(*(data_buf+2),sum);
//    }
//}



