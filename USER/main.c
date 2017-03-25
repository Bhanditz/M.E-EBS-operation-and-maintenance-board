#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"		 	 
#include "audiosel.h"
#include "rda5820.h"
#include "timer.h"
//#include "usmart.h"
#include "gray.h"
#include "stmflash.h"



#define FLASH_SAVE_ADDR  0X08070000 //设置FLASH 保存地址(必须为偶数)
u8 TEXT_Buffer[4]={0};
#define SIZE sizeof(TEXT_Buffer)	 			  	//数组长度

void frame_resent(void);//向上位机请求重传
u8 usart2_works=0;//串口2工作状态指示。0：空闲；1：发送连接帧；2：接收连接反馈帧；3：发送数据帧；4：接收数据帧；
u8 usart1_works=0;//串口1工作状态指示。0：空闲；1：接收连接帧；2：发送连接反馈帧；3：接收频谱扫描帧；4：发送频谱扫描反馈帧；
				  //5:接收控制帧;6:发送控制反馈帧;7:接收数据帧;8:发送数据反馈帧(包括重传帧);9：数据帧无线传输中(不允许被打断)；

u8 index_frame_send=0;//串口回复信息帧下标
u8 frame_send_buf[100]={0};//串口回传缓冲区

void relay(u8 index);//继电器控制复位
u8 is_relaying=0;//正在复位标志位。0：空闲；1：正在复位；
int main(void)
{	 
	u16 t=0;
	u16 len,len1;	
	u8 xor_sum=0; 
	u8 temp=0;//中间过程变量
		
	pin_init();
	delay_init();	    	 //延时函数初始化	  
	NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(115200);	 	//广播软件：串口1初始化为115200
	uart2_init(115200);	 //通信软件：串口2初始化为115200
//	KEY_Init();
 	LED_Init();			     //LED端口初始化
	TIM4_Int_Init(9999,7199);
	TIM3_Int_Init(9999,7199);//1S闪一次
//	TIM6_Int_Init(4,1199);//6K周期方波
//	TIM7_Int_Init(4,719);//10k周期方波
	 

//	STMFLASH_Read(FLASH_SAVE_ADDR,(u16*)flash_temp,SIZE);
//	fre_tmp=flash_temp[0]*10+FREQUENCY_MIN;	
//	if(flash_temp[2]>63){
//		flash_temp[2]=8;//默认值这是为8，对应-13dbm
//	}
//
//	if((fre_tmp>FREQUENCY_MIN)&&(fre_tmp<FREQUENCY_MAX)){send_frequency=fre_tmp;}
//	else{send_frequency=FREQUENCY_MIN;}

 	while(1)
	{
/***********************************（通信软件）串口2接收数据************************************************************************************/
		if((USART2_RX_STA&0x8000)&&(usart2_works==0))
		{			   
			len1=USART2_RX_STA&0x3fff;//得到此次接收到的数据长度

			if((USART2_RX_BUF[0]=='$')&&(len1>0)){
				xor_sum=XOR(USART2_RX_BUF,len1-1);
				if((xor_sum=='$')||(xor_sum==0x0d)){xor_sum++;}
				if(USART2_RX_BUF[len1-1]==xor_sum){
					index_frame_send=0;
 					/*****************************连接查询帧*******************************************************************************/
					if((USART2_RX_BUF[1]=='r')&&(USART2_RX_BUF[2]=='e')&&(USART2_RX_BUF[3]=='y')&&(USART2_RX_BUF[4]=='_')){//链接查询帧
						usart2_works=1;//接收到连接查询帧				

						frame_send_buf[index_frame_send]='$';
						index_frame_send++;
						frame_send_buf[index_frame_send]='r';
						index_frame_send++;
						frame_send_buf[index_frame_send]='e';
						index_frame_send++;
						frame_send_buf[index_frame_send]='y';
						index_frame_send++;
						frame_send_buf[index_frame_send]='_';
						index_frame_send++;
						frame_send_buf[index_frame_send]='_';
						index_frame_send++;
						frame_send_buf[index_frame_send]=USART2_RX_BUF[5];
						index_frame_send++;

						frame_send_buf[index_frame_send]=XOR(frame_send_buf,index_frame_send);
						index_frame_send++;
						usart2_works=2;//发送连接查询反馈帧
						for(t=0;t<index_frame_send;t++)
						{
							USART_SendData(USART2, frame_send_buf[t]);//向串口发送数据
							while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);//等待发送结束
						}
						 USART2_RX_STA=0;//处理完毕，允许接收下一帧
						 USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//打开中断
						 usart2_works=0;//处理完，标志空闲
					}
 					/*****************************复位帧***************************************************************************/
				 	else if((USART2_RX_BUF[1]=='r')&&(USART2_RX_BUF[2]=='s')&&(USART2_RX_BUF[3]=='e')&&(USART2_RX_BUF[4]=='_')){//复位帧
						usart2_works=3;//接收到连接查询帧				

						frame_send_buf[index_frame_send]='$';
						index_frame_send++;
						frame_send_buf[index_frame_send]='r';
						index_frame_send++;
						frame_send_buf[index_frame_send]='s';
						index_frame_send++;
						frame_send_buf[index_frame_send]='e';
						index_frame_send++;
						frame_send_buf[index_frame_send]='_';
						index_frame_send++;
						frame_send_buf[index_frame_send]='_';
						index_frame_send++;
						frame_send_buf[index_frame_send]=USART2_RX_BUF[5];
						index_frame_send++;
						frame_send_buf[index_frame_send]=USART2_RX_BUF[6];
						index_frame_send++;
						
						frame_send_buf[index_frame_send]=XOR(frame_send_buf,index_frame_send);
						index_frame_send++;

						temp=USART2_RX_BUF[6]-0x30;
						if(temp==1){//复位广播板 0x31：有线电话；0x32：卫星电话；0x33：3G模块；0x34：北斗模块；0x35：广播板；0x36：其他；
							//do something
							relay(1);//将来若无其他语句填充，可将各循环合并为relay(temp);
						}else if(temp==2){
							//do something
							relay(2);
						}else if(temp==3){
							//do something
							relay(3);
						}else if(temp==4){
							//do something
							relay(4);
						}

						usart2_works=4;//发送连接查询反馈帧
						for(t=0;t<index_frame_send;t++)
						{
							USART_SendData(USART2, frame_send_buf[t]);//向串口发送数据
							while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);//等待发送结束
						}
						

						 USART2_RX_STA=0;//处理完毕，允许接收下一帧
						 USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//打开中断
						 usart2_works=0;//处理完，标志空闲
					}
 					/********************************切换音频开关申请帧***************************************************************/
				 	else if((USART2_RX_BUF[1]=='s')&&(USART2_RX_BUF[2]=='w')&&(USART2_RX_BUF[3]=='h')&&(USART2_RX_BUF[4]=='_')){//切换音频开关申请帧
						usart2_works=5;//接收到连接查询帧				

						frame_send_buf[index_frame_send]='$';
						index_frame_send++;
						frame_send_buf[index_frame_send]='s';
						index_frame_send++;
						frame_send_buf[index_frame_send]='w';
						index_frame_send++;
						frame_send_buf[index_frame_send]='h';
						index_frame_send++;
						frame_send_buf[index_frame_send]='_';
						index_frame_send++;
						frame_send_buf[index_frame_send]='_';
						index_frame_send++;
						frame_send_buf[index_frame_send]=USART2_RX_BUF[5];
						index_frame_send++;
						frame_send_buf[index_frame_send]=USART2_RX_BUF[6];
						index_frame_send++;

						frame_send_buf[index_frame_send]=XOR(frame_send_buf,index_frame_send);
						index_frame_send++;

						temp=USART2_RX_BUF[6]-0x30;
						if(temp==1){//音频切换
							//do something
							relay(5);//将来若无其他语句填充，可将各循环合并为relay(temp);
						}else if(temp==2){
							//do something
							relay(6);
						}else if(temp==3){
							//do something
							relay(7);
						}else if(temp==4){
							//do something
							relay(8);
						}

						usart2_works=6;//发送连接查询反馈帧
						for(t=0;t<index_frame_send;t++)
						{
							USART_SendData(USART2, frame_send_buf[t]);//向串口发送数据
							while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);//等待发送结束
						}
						
						 USART2_RX_STA=0;//处理完毕，允许接收下一帧
						 USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//打开中断
						 usart2_works=0;//处理完，标志空闲
					}else {USART2_RX_STA=0;USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);usart2_works=0;}//帧头错误
				 }else{//end of XOR，XOR出错请求重传
				 USART2_RX_STA=0;
				 USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
				 usart2_works=0;
				 }
				 }else {USART2_RX_STA=0;USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);usart2_works=0;}//end of check '$'


		}else {USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);}//此处等待串口2回传数据，故usart2_works不能清零，但要添加对其为0的条件判断
		
			
/**************************************（广播软件）串口1接收数据*******************************************************************************/
	   if((USART_RX_STA&0x8000)&&(usart1_works==0))
		{					   
			len=USART_RX_STA&0x3fff;//得到此次接收到的数据长度

			if((USART_RX_BUF[0]=='$')&&(len>0)){
				xor_sum=XOR(USART_RX_BUF,len-1);
				if((xor_sum=='$')||(xor_sum==0x0d)){xor_sum++;}
				if(USART_RX_BUF[len-1]==xor_sum){
					index_frame_send=0;
 					/************************子板链接判断帧************************&&&&&&&&&*******************************************/
					if((USART_RX_BUF[1]=='r')&&(USART_RX_BUF[2]=='e')&&(USART_RX_BUF[3]=='d')&&(USART_RX_BUF[4]=='_')){//连接帧
						usart1_works=1;	//收到连接帧
						 frame_send_buf[index_frame_send]='$';
						 index_frame_send++;
						 frame_send_buf[index_frame_send]='r';
						 index_frame_send++;
						 frame_send_buf[index_frame_send]='e';
						 index_frame_send++;
						 frame_send_buf[index_frame_send]='d';
						 index_frame_send++;
						 frame_send_buf[index_frame_send]='_';
						 index_frame_send++;
						 frame_send_buf[index_frame_send]='_';
						 index_frame_send++;
						 frame_send_buf[index_frame_send]=USART_RX_BUF[5];
						 index_frame_send++;
			//			 STMFLASH_Read(FLASH_SAVE_ADDR,(u16*)flash_temp,SIZE);
			//			 frame_send_buf[index_frame_send]=flash_temp[0];//相对频点
			//			 index_frame_send++;

			//			 if(flash_temp[2]>64)flash_temp[2]=8;//值超过范围，设置为默认值8，对应-13dbm
			//			 RDA5820_TxPAG_Set(flash_temp[2]);

			//			 frame_send_buf[index_frame_send]=flash_temp[2];//发射功率
			//			 index_frame_send++;
						 frame_send_buf[index_frame_send]=XOR(frame_send_buf,index_frame_send);
						 index_frame_send++;
						 usart1_works=2;//发送连接反馈帧
						 for(t=0;t<index_frame_send;t++)
						 {
							USART_SendData(USART1, frame_send_buf[t]);//向串口发送数据
							while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
						 }
						 USART_RX_STA=0;//处理完毕，允许接收下一帧
						 USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//打开中断
						 usart1_works=0;//处理完，标志空闲
					}
 					/*******************************************复位帧*************************************************************/					
					else if((USART_RX_BUF[1]=='r')&&(USART_RX_BUF[2]=='s')&&(USART_RX_BUF[3]=='e')&&(USART_RX_BUF[4]=='_')&&(is_relaying==0)){//复位帧
						usart1_works=3;//接收到复位帧				

						frame_send_buf[index_frame_send]='$';
						index_frame_send++;
						frame_send_buf[index_frame_send]='r';
						index_frame_send++;
						frame_send_buf[index_frame_send]='s';
						index_frame_send++;
						frame_send_buf[index_frame_send]='e';
						index_frame_send++;
						frame_send_buf[index_frame_send]='_';
						index_frame_send++;
						frame_send_buf[index_frame_send]='_';
						index_frame_send++;
						frame_send_buf[index_frame_send]=USART_RX_BUF[5];
						index_frame_send++;
						frame_send_buf[index_frame_send]=USART_RX_BUF[6];
						index_frame_send++;
						if((USART_RX_BUF[6]-0x30)==5){//复位广播板 0x31：有线电话；0x32：卫星电话；0x33：3G模块；0x34：北斗模块；0x35：广播板；0x36：其他；
							//do something
								relay(5);
						}
						frame_send_buf[index_frame_send]=XOR(frame_send_buf,index_frame_send);
						index_frame_send++;
						usart1_works=4;//发送复位反馈帧
						for(t=0;t<index_frame_send;t++)
						{
							USART_SendData(USART1, frame_send_buf[t]);//向串口发送数据
							while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
						}
						 USART_RX_STA=0;//处理完毕，允许接收下一帧
						 USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//打开中断
						 usart1_works=0;//处理完，标志空闲
					}
 					/*******************************************频谱扫描，继电器控制帧*************************************************************/
					else if((USART_RX_BUF[1]=='s')&&(USART_RX_BUF[2]=='c')&&(USART_RX_BUF[3]=='a')&&(USART_RX_BUF[4]=='_')){//频谱扫描控制帧
						usart1_works=5;//接收到频谱扫描，继电器控制帧
						frame_send_buf[index_frame_send]='$';
						 index_frame_send++;
						 frame_send_buf[index_frame_send]='s';
						 index_frame_send++;
						 frame_send_buf[index_frame_send]='c';
						 index_frame_send++;
						 frame_send_buf[index_frame_send]='a';
						 index_frame_send++;
						 frame_send_buf[index_frame_send]='_';
						 index_frame_send++;
						 frame_send_buf[index_frame_send]='_';
						 index_frame_send++;
						 frame_send_buf[index_frame_send]=USART_RX_BUF[5];
						 index_frame_send++;
						 frame_send_buf[index_frame_send]=XOR(frame_send_buf,index_frame_send);
						 index_frame_send++;

						 usart1_works=6;//发送频谱扫描，继电器控制反馈帧
						 for(t=0;t<index_frame_send;t++)
						 {
							USART_SendData(USART1, frame_send_buf[t]);//向串口发送数据
							while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
						 }
						 USART_RX_STA=0;//处理完毕，允许接收下一帧
						 USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//打开中断
						 usart1_works=0;//处理完毕，标志清零
					}
 					else {USART_RX_STA=0;USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);usart1_works=0;}//else{printf("Frame anomalous!");}
				}else {//帧的校验和出错时，是数据帧的校验和出错时，请求重传
					USART_RX_STA=0;
					USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//打开中断
					frame_resent();//求重传
					usart1_works=0;		
				}//else{printf("Verify bits wrong!");}
		    }else {USART_RX_STA=0;USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);usart1_works=0;}//if(USART_RX_BUF[0]=='$')
						
		}else if((usart1_works==0)){USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);}//if(USART_RX_STA&0x8000)，本判断只有当数据帧字节流未收满时使用；除数据帧外的其他帧在本判断中已处理完毕
			
	}//while(1)
}



void frame_resent(void){
	u8 t=0;
	
if((USART_RX_BUF[1]=='r')&&(USART_RX_BUF[2]=='s')&&(USART_RX_BUF[3]=='e')&&(USART_RX_BUF[4]=='_')||
	(USART_RX_BUF[1]=='s')&&(USART_RX_BUF[2]=='c')&&(USART_RX_BUF[3]=='a')&&(USART_RX_BUF[4]=='_')
	){//连接帧除外,帧校验和出错就重传
	index_frame_send=0;
	frame_send_buf[index_frame_send]='$';
	index_frame_send++;
	frame_send_buf[index_frame_send]='r';
	index_frame_send++;
	frame_send_buf[index_frame_send]='s';
	index_frame_send++;
	frame_send_buf[index_frame_send]='t';
	index_frame_send++;
	frame_send_buf[index_frame_send]='_';
	index_frame_send++;
	frame_send_buf[index_frame_send]='_';
	index_frame_send++;
	frame_send_buf[index_frame_send]=0;
	index_frame_send++;
	frame_send_buf[index_frame_send]=0;//一问一答方式，上位机自动重传刚才发的数据帧。重传数据帧类型。1：广播唤醒帧；2：单播唤醒帧；3：组播唤醒帧；4：控制帧；5：认证帧；
	index_frame_send++;
	frame_send_buf[index_frame_send]=XOR(frame_send_buf,index_frame_send);
	index_frame_send++;

	for(t=0;t<index_frame_send;t++)
	{
		USART_SendData(USART1, frame_send_buf[t]);//向串口发送数据
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
	}
}
}

void relay(u8 index){
	is_relaying=1;//标记正在执行复位动作
	LED0=0;//点灯，表示正在复位
	TIM4_Int_Init(9999,7199);//复位计数
	if(index==1){ //有线电话
		RELAY1=1;
	}else if(index==2){//卫星电话
		RELAY2=1;
	}else if(index==3){//3G电话
		RELAY3=1;
	}else if(index==4){//北斗
		RELAY4=1;
	}else if(index==5){//电台子板
		RELAY5=1;
	}else if(index==6){//运维另一个板子
	    RELAY6=1;
	}else if(index==7){//功放1
		RELAY7=0;
	}else if(index==8){//功放2
		RELAY8=0;
	}
	TIM_Cmd(TIM4, ENABLE);//打开TIM4
}



