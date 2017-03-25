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



#define FLASH_SAVE_ADDR  0X08070000 //����FLASH �����ַ(����Ϊż��)
u8 TEXT_Buffer[4]={0};
#define SIZE sizeof(TEXT_Buffer)	 			  	//���鳤��

void frame_resent(void);//����λ�������ش�
u8 usart2_works=0;//����2����״ָ̬ʾ��0�����У�1����������֡��2���������ӷ���֡��3����������֡��4����������֡��
u8 usart1_works=0;//����1����״ָ̬ʾ��0�����У�1����������֡��2���������ӷ���֡��3������Ƶ��ɨ��֡��4������Ƶ��ɨ�跴��֡��
				  //5:���տ���֡;6:���Ϳ��Ʒ���֡;7:��������֡;8:�������ݷ���֡(�����ش�֡);9������֡���ߴ�����(���������)��

u8 index_frame_send=0;//���ڻظ���Ϣ֡�±�
u8 frame_send_buf[100]={0};//���ڻش�������

void relay(u8 index);//�̵������Ƹ�λ
u8 is_relaying=0;//���ڸ�λ��־λ��0�����У�1�����ڸ�λ��
int main(void)
{	 
	u16 t=0;
	u16 len,len1;	
	u8 xor_sum=0; 
	u8 temp=0;//�м���̱���
		
	pin_init();
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_Configuration(); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 	//�㲥���������1��ʼ��Ϊ115200
	uart2_init(115200);	 //ͨ�����������2��ʼ��Ϊ115200
//	KEY_Init();
 	LED_Init();			     //LED�˿ڳ�ʼ��
	TIM4_Int_Init(9999,7199);
	TIM3_Int_Init(9999,7199);//1S��һ��
//	TIM6_Int_Init(4,1199);//6K���ڷ���
//	TIM7_Int_Init(4,719);//10k���ڷ���
	 

//	STMFLASH_Read(FLASH_SAVE_ADDR,(u16*)flash_temp,SIZE);
//	fre_tmp=flash_temp[0]*10+FREQUENCY_MIN;	
//	if(flash_temp[2]>63){
//		flash_temp[2]=8;//Ĭ��ֵ����Ϊ8����Ӧ-13dbm
//	}
//
//	if((fre_tmp>FREQUENCY_MIN)&&(fre_tmp<FREQUENCY_MAX)){send_frequency=fre_tmp;}
//	else{send_frequency=FREQUENCY_MIN;}

 	while(1)
	{
/***********************************��ͨ�����������2��������************************************************************************************/
		if((USART2_RX_STA&0x8000)&&(usart2_works==0))
		{			   
			len1=USART2_RX_STA&0x3fff;//�õ��˴ν��յ������ݳ���

			if((USART2_RX_BUF[0]=='$')&&(len1>0)){
				xor_sum=XOR(USART2_RX_BUF,len1-1);
				if((xor_sum=='$')||(xor_sum==0x0d)){xor_sum++;}
				if(USART2_RX_BUF[len1-1]==xor_sum){
					index_frame_send=0;
 					/*****************************���Ӳ�ѯ֡*******************************************************************************/
					if((USART2_RX_BUF[1]=='r')&&(USART2_RX_BUF[2]=='e')&&(USART2_RX_BUF[3]=='y')&&(USART2_RX_BUF[4]=='_')){//���Ӳ�ѯ֡
						usart2_works=1;//���յ����Ӳ�ѯ֡				

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
						usart2_works=2;//�������Ӳ�ѯ����֡
						for(t=0;t<index_frame_send;t++)
						{
							USART_SendData(USART2, frame_send_buf[t]);//�򴮿ڷ�������
							while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
						}
						 USART2_RX_STA=0;//������ϣ����������һ֡
						 USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//���ж�
						 usart2_works=0;//�����꣬��־����
					}
 					/*****************************��λ֡***************************************************************************/
				 	else if((USART2_RX_BUF[1]=='r')&&(USART2_RX_BUF[2]=='s')&&(USART2_RX_BUF[3]=='e')&&(USART2_RX_BUF[4]=='_')){//��λ֡
						usart2_works=3;//���յ����Ӳ�ѯ֡				

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
						if(temp==1){//��λ�㲥�� 0x31�����ߵ绰��0x32�����ǵ绰��0x33��3Gģ�飻0x34������ģ�飻0x35���㲥�壻0x36��������
							//do something
							relay(1);//�����������������䣬�ɽ���ѭ���ϲ�Ϊrelay(temp);
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

						usart2_works=4;//�������Ӳ�ѯ����֡
						for(t=0;t<index_frame_send;t++)
						{
							USART_SendData(USART2, frame_send_buf[t]);//�򴮿ڷ�������
							while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
						}
						

						 USART2_RX_STA=0;//������ϣ����������һ֡
						 USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//���ж�
						 usart2_works=0;//�����꣬��־����
					}
 					/********************************�л���Ƶ��������֡***************************************************************/
				 	else if((USART2_RX_BUF[1]=='s')&&(USART2_RX_BUF[2]=='w')&&(USART2_RX_BUF[3]=='h')&&(USART2_RX_BUF[4]=='_')){//�л���Ƶ��������֡
						usart2_works=5;//���յ����Ӳ�ѯ֡				

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
						if(temp==1){//��Ƶ�л�
							//do something
							relay(5);//�����������������䣬�ɽ���ѭ���ϲ�Ϊrelay(temp);
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

						usart2_works=6;//�������Ӳ�ѯ����֡
						for(t=0;t<index_frame_send;t++)
						{
							USART_SendData(USART2, frame_send_buf[t]);//�򴮿ڷ�������
							while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
						}
						
						 USART2_RX_STA=0;//������ϣ����������һ֡
						 USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//���ж�
						 usart2_works=0;//�����꣬��־����
					}else {USART2_RX_STA=0;USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);usart2_works=0;}//֡ͷ����
				 }else{//end of XOR��XOR���������ش�
				 USART2_RX_STA=0;
				 USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
				 usart2_works=0;
				 }
				 }else {USART2_RX_STA=0;USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);usart2_works=0;}//end of check '$'


		}else {USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);}//�˴��ȴ�����2�ش����ݣ���usart2_works�������㣬��Ҫ��Ӷ���Ϊ0�������ж�
		
			
/**************************************���㲥���������1��������*******************************************************************************/
	   if((USART_RX_STA&0x8000)&&(usart1_works==0))
		{					   
			len=USART_RX_STA&0x3fff;//�õ��˴ν��յ������ݳ���

			if((USART_RX_BUF[0]=='$')&&(len>0)){
				xor_sum=XOR(USART_RX_BUF,len-1);
				if((xor_sum=='$')||(xor_sum==0x0d)){xor_sum++;}
				if(USART_RX_BUF[len-1]==xor_sum){
					index_frame_send=0;
 					/************************�Ӱ������ж�֡************************&&&&&&&&&*******************************************/
					if((USART_RX_BUF[1]=='r')&&(USART_RX_BUF[2]=='e')&&(USART_RX_BUF[3]=='d')&&(USART_RX_BUF[4]=='_')){//����֡
						usart1_works=1;	//�յ�����֡
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
			//			 frame_send_buf[index_frame_send]=flash_temp[0];//���Ƶ��
			//			 index_frame_send++;

			//			 if(flash_temp[2]>64)flash_temp[2]=8;//ֵ������Χ������ΪĬ��ֵ8����Ӧ-13dbm
			//			 RDA5820_TxPAG_Set(flash_temp[2]);

			//			 frame_send_buf[index_frame_send]=flash_temp[2];//���书��
			//			 index_frame_send++;
						 frame_send_buf[index_frame_send]=XOR(frame_send_buf,index_frame_send);
						 index_frame_send++;
						 usart1_works=2;//�������ӷ���֡
						 for(t=0;t<index_frame_send;t++)
						 {
							USART_SendData(USART1, frame_send_buf[t]);//�򴮿ڷ�������
							while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
						 }
						 USART_RX_STA=0;//������ϣ����������һ֡
						 USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//���ж�
						 usart1_works=0;//�����꣬��־����
					}
 					/*******************************************��λ֡*************************************************************/					
					else if((USART_RX_BUF[1]=='r')&&(USART_RX_BUF[2]=='s')&&(USART_RX_BUF[3]=='e')&&(USART_RX_BUF[4]=='_')&&(is_relaying==0)){//��λ֡
						usart1_works=3;//���յ���λ֡				

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
						if((USART_RX_BUF[6]-0x30)==5){//��λ�㲥�� 0x31�����ߵ绰��0x32�����ǵ绰��0x33��3Gģ�飻0x34������ģ�飻0x35���㲥�壻0x36��������
							//do something
								relay(5);
						}
						frame_send_buf[index_frame_send]=XOR(frame_send_buf,index_frame_send);
						index_frame_send++;
						usart1_works=4;//���͸�λ����֡
						for(t=0;t<index_frame_send;t++)
						{
							USART_SendData(USART1, frame_send_buf[t]);//�򴮿ڷ�������
							while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
						}
						 USART_RX_STA=0;//������ϣ����������һ֡
						 USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//���ж�
						 usart1_works=0;//�����꣬��־����
					}
 					/*******************************************Ƶ��ɨ�裬�̵�������֡*************************************************************/
					else if((USART_RX_BUF[1]=='s')&&(USART_RX_BUF[2]=='c')&&(USART_RX_BUF[3]=='a')&&(USART_RX_BUF[4]=='_')){//Ƶ��ɨ�����֡
						usart1_works=5;//���յ�Ƶ��ɨ�裬�̵�������֡
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

						 usart1_works=6;//����Ƶ��ɨ�裬�̵������Ʒ���֡
						 for(t=0;t<index_frame_send;t++)
						 {
							USART_SendData(USART1, frame_send_buf[t]);//�򴮿ڷ�������
							while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
						 }
						 USART_RX_STA=0;//������ϣ����������һ֡
						 USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//���ж�
						 usart1_works=0;//������ϣ���־����
					}
 					else {USART_RX_STA=0;USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);usart1_works=0;}//else{printf("Frame anomalous!");}
				}else {//֡��У��ͳ���ʱ��������֡��У��ͳ���ʱ�������ش�
					USART_RX_STA=0;
					USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//���ж�
					frame_resent();//���ش�
					usart1_works=0;		
				}//else{printf("Verify bits wrong!");}
		    }else {USART_RX_STA=0;USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);usart1_works=0;}//if(USART_RX_BUF[0]=='$')
						
		}else if((usart1_works==0)){USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);}//if(USART_RX_STA&0x8000)�����ж�ֻ�е�����֡�ֽ���δ����ʱʹ�ã�������֡�������֡�ڱ��ж����Ѵ������
			
	}//while(1)
}



void frame_resent(void){
	u8 t=0;
	
if((USART_RX_BUF[1]=='r')&&(USART_RX_BUF[2]=='s')&&(USART_RX_BUF[3]=='e')&&(USART_RX_BUF[4]=='_')||
	(USART_RX_BUF[1]=='s')&&(USART_RX_BUF[2]=='c')&&(USART_RX_BUF[3]=='a')&&(USART_RX_BUF[4]=='_')
	){//����֡����,֡У��ͳ�����ش�
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
	frame_send_buf[index_frame_send]=0;//һ��һ��ʽ����λ���Զ��ش��ղŷ�������֡���ش�����֡���͡�1���㲥����֡��2����������֡��3���鲥����֡��4������֡��5����֤֡��
	index_frame_send++;
	frame_send_buf[index_frame_send]=XOR(frame_send_buf,index_frame_send);
	index_frame_send++;

	for(t=0;t<index_frame_send;t++)
	{
		USART_SendData(USART1, frame_send_buf[t]);//�򴮿ڷ�������
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
	}
}
}

void relay(u8 index){
	is_relaying=1;//�������ִ�и�λ����
	LED0=0;//��ƣ���ʾ���ڸ�λ
	TIM4_Int_Init(9999,7199);//��λ����
	if(index==1){ //���ߵ绰
		RELAY1=1;
	}else if(index==2){//���ǵ绰
		RELAY2=1;
	}else if(index==3){//3G�绰
		RELAY3=1;
	}else if(index==4){//����
		RELAY4=1;
	}else if(index==5){//��̨�Ӱ�
		RELAY5=1;
	}else if(index==6){//��ά��һ������
	    RELAY6=1;
	}else if(index==7){//����1
		RELAY7=0;
	}else if(index==8){//����2
		RELAY8=0;
	}
	TIM_Cmd(TIM4, ENABLE);//��TIM4
}



