#ifndef __LED_H
#define __LED_H	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//LED��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/2
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 
#define LED0 PCout(14)// PC14
#define LED1 PCout(15)// PC15
#define RELAY1 PBout(1)
#define RELAY2 PBout(0)
#define RELAY3 PCout(5)
#define RELAY4 PCout(4)
#define RELAY5 PAout(7)
#define RELAY6 PAout(6)
#define RELAY7 PAout(5)//12V
#define RELAY8 PAout(4)//17V

#define SOUND_PC9 PCout(9)//  U1	
#define SOUND_PC8 PCout(8)   
#define SOUND_PC7 PCout(7)//  U1	
#define SOUND_PC6 PCout(6) 				  
#define SOUND_PB15 PBout(15)//  U2	
#define SOUND_PB14 PBout(14) 
#define SOUND_PB13 PBout(13)//  U10	
#define SOUND_PB12 PBout(12) 	

void LED_Init(void);//��ʼ��
unsigned char XOR(unsigned char *BUFF, u16 len);//���У���
void pin_init(void);
		 				    
#endif
