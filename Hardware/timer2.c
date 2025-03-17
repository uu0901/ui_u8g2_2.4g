#include "timer2.h"
#include "stm32f10x.h"                  // Device header

//arr=1000.psc=72�����ʱ��Ϊ1ms
void TIM2_Int_Init(uint16_t arr,uint16_t psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);               //ʱ��ʹ��
    
    //��ʱ��TIM3��ʼ��
    TIM_TimeBaseStructure.TIM_Period = arr;                             //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ   
    TIM_TimeBaseStructure.TIM_Prescaler =psc;                           //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;           //����ʱ�ӷָ�:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;      //TIM���ϼ���ģʽ
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);                   //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
    
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);
    TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE );                          //ʹ��ָ����TIM3�ж�,��������ж�
    
    //�ж����ȼ�NVIC��
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;                   //TIM3�ж�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;        //��ռ���ȼ�0��
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;                //�����ȼ�3��
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                   //IRQͨ����ʹ��
    NVIC_Init(&NVIC_InitStructure);                                    //��ʼ��NVIC�Ĵ���
    
    TIM_Cmd(TIM2, ENABLE);                                              //ʹ��TIMx    
}












