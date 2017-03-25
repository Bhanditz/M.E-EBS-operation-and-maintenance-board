#include "led.h"

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//LED驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/2
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	   

//初始化PB5和PE5为输出口.并使能这两个口的时钟		    
//LED IO初始化
void LED_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 //使能PB,PE端口时钟
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15|GPIO_Pin_14;				 //LED0-->PC14,15 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOC, &GPIO_InitStructure);					 //根据设定参数初始化
 GPIO_SetBits(GPIOC,GPIO_Pin_14);						 //PC14,15 输出高
 GPIO_SetBits(GPIOC,GPIO_Pin_15);

}

void pin_init(void){
     GPIO_InitTypeDef  GPIO_InitStructure;
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOA, ENABLE);//使能PC,PB,PA,端口时钟
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;				 
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	 GPIO_Init(GPIOB, &GPIO_InitStructure);
	 GPIO_ResetBits(GPIOB,GPIO_Pin_0);//PB(0)
	 GPIO_ResetBits(GPIOB,GPIO_Pin_1);//PB(1)

	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_4;
	 GPIO_Init(GPIOC, &GPIO_InitStructure);
	 GPIO_ResetBits(GPIOC,GPIO_Pin_4);//PC(4)
	 GPIO_ResetBits(GPIOC,GPIO_Pin_5);//PC(5)

	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_4|GPIO_Pin_6|GPIO_Pin_7;
	 GPIO_Init(GPIOA, &GPIO_InitStructure);
	 GPIO_SetBits(GPIOA,GPIO_Pin_4);//PA(4) 功放1
	 GPIO_SetBits(GPIOA,GPIO_Pin_5);//PA(5) 功放2
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
 

