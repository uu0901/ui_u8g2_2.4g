//#include "stm32f10x.h"                  // Device header
//#include "Delay.h"

//void Key_Init(void)
//{
// 	GPIO_InitTypeDef GPIO_InitStructure;
// 
// 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB,ENABLE);//ʹ��PORTA,PORTEʱ��

//	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_13|GPIO_Pin_6;//KEY0-KEY2
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó���������
// 	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOE2,3,4

//	//��ʼ�� WK_UP-->GPIOA.0	  ��������
//	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PA0���ó����룬Ĭ������	  
//	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.0
//}

//uint8_t Key_GetNum(void)
//{
//	uint8_t KeyNum = 0;
//	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) == 0)
//	{
//		Delay_ms(20);
//		while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) == 0);
//		Delay_ms(20);
//		KeyNum = 1;
//	}
//	if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 1)
//	{
//		Delay_ms(20);
//		while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 1);
//		Delay_ms(20);
//		KeyNum = 2;
//	}
//	
//	return KeyNum;
//}
