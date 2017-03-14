#include "led.h"

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

//��ʼ��PB5��PE5Ϊ�����.��ʹ���������ڵ�ʱ��		    
//LED IO��ʼ��
void LED_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 //ʹ��PB,PE�˿�ʱ��
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15|GPIO_Pin_14;				 //LED0-->PC14,15 �˿�����
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
 GPIO_Init(GPIOC, &GPIO_InitStructure);					 //�����趨������ʼ��
 GPIO_SetBits(GPIOC,GPIO_Pin_14);						 //PC14,15 �����
 GPIO_SetBits(GPIOC,GPIO_Pin_15);

}

void pin_init(void){
     GPIO_InitTypeDef  GPIO_InitStructure;
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOA, ENABLE);//ʹ��PC,PB,PA,�˿�ʱ��
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;				 
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	 GPIO_Init(GPIOB, &GPIO_InitStructure);
	 GPIO_ResetBits(GPIOB,GPIO_Pin_0);//PB(0)//�̵���
	 GPIO_ResetBits(GPIOB,GPIO_Pin_1);//PB(1)

	 GPIO_SetBits(GPIOB,GPIO_Pin_12);//PB(12)//��Ƶ����
	 GPIO_SetBits(GPIOB,GPIO_Pin_13);//PB(13)
	 GPIO_SetBits(GPIOB,GPIO_Pin_14);//PB(14)
	 GPIO_SetBits(GPIOB,GPIO_Pin_15);//PB(15)
	 

	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_4|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;
	 GPIO_Init(GPIOC, &GPIO_InitStructure);
	 GPIO_ResetBits(GPIOC,GPIO_Pin_4);//PC(4)//�̵���
	 GPIO_ResetBits(GPIOC,GPIO_Pin_5);//PC(5)

	 GPIO_SetBits(GPIOC,GPIO_Pin_6);//PC(6)//��Ƶ����
	 GPIO_SetBits(GPIOC,GPIO_Pin_7);//PC(7)
	 GPIO_SetBits(GPIOC,GPIO_Pin_8);//PC(8)
	 GPIO_SetBits(GPIOC,GPIO_Pin_9);//PC(9)

	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_4|GPIO_Pin_6|GPIO_Pin_7;
	 GPIO_Init(GPIOA, &GPIO_InitStructure);
	 GPIO_ResetBits(GPIOA,GPIO_Pin_4);//PA(4) 48V����	 ���ţ�1�򿪣�0ֹͣ��������0�򿪣�1�رգ�//�̵���
	 GPIO_ResetBits(GPIOA,GPIO_Pin_5);//PA(5) 12V����
	 GPIO_ResetBits(GPIOA,GPIO_Pin_6);//PA(6)
	 GPIO_ResetBits(GPIOA,GPIO_Pin_7);//PA(7)
	 
}

unsigned char XOR(unsigned char *BUFF, u16 len)
{
	unsigned char result=0;
	u16 i;
	for(result=BUFF[0],i=1;i<len;i++)
	{
		result ^= BUFF[i];
	}
	return result;
}
 

