#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "playmusic.h"
#include "u8g2.h"
#include "ui_bmp_data.h"
#include "ui.h"
#include "mykey.h"
void Music_init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	TIM_InternalClockConfig(TIM4);//内部时钟源
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 99;//100	
	TIM_TimeBaseInitStructure.TIM_Prescaler = 1439;
//	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 50;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStructure);
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);//初始化结构体为默认值
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 50;		//CCR占空比，据说可调音色
	TIM_OC2Init(TIM4, &TIM_OCInitStructure);
	TIM_Cmd(TIM4, ENABLE);
}

void Video_init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitTypeStruct;
	NVIC_InitTypeDef NVIC_InitTypeStruct;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	TIM_TimeBaseInitTypeStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitTypeStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitTypeStruct.TIM_Period = 7199;
	TIM_TimeBaseInitTypeStruct.TIM_Prescaler = 1439;
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitTypeStruct);
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);//使能定时器的中断
    TIM_ClearFlag(TIM3, TIM_FLAG_Update);
	NVIC_InitTypeStruct.NVIC_IRQChannel = TIM3_IRQn;//中断通道 
	NVIC_InitTypeStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitTypeStruct.NVIC_IRQChannelPreemptionPriority = 0;//优先级
	NVIC_InitTypeStruct.NVIC_IRQChannelSubPriority = 3;
	NVIC_Init(&NVIC_InitTypeStruct);
	TIM_Cmd(TIM3,DISABLE);
}

void Sound_SetHZ(uint16_t pre)
{
	TIM_PrescalerConfig(TIM4,pre,TIM_PSCReloadMode_Immediate);//修改预分频值改变频率，72M/(pre*100)=hz
}
void Play_Music(int pre,int tm,int tm0)
{
//	Music_init();
	Sound_SetHZ(pre);Delay_ms(tm);Sound_SetHZ(20);Delay_ms(tm0);
}


int song1[]={  //第一段音乐                                             
	        E5,10,F5,10,G5,20,  C6,10,F5,10,E5,10,F5,10,  G5,10,C6,10,E6,10,F6,10,  E6,10,A5,10,C6,20,  G5,20,E5,10,F5,10,  G5,20,C6,20,  D6,10,B5,10,C6,10,D6,10,  F6,10,E6,10,F6,10,D6,10  //27
           };
int song2[]={  //第二段音乐（持续循环）
          G4,20,A4,20,  D4_,10,E4,20,D4,10,  D4_,10,D4,10,C4,20,  C4,20,D4,20,  D4_,20,D4,10,D4,10,  C4,10,D4,10,E4,10,G4,10,  A4,10,E4,10,G4,10,D4,10,  E4,10,C4,10,D4,10,C4,10,	  //25
	        E4,20,G4,20,  A4,10,E4,10,G4,10,D4,10,  E4,10,C4,10,D4_,10,E4,10,  D4,10,D4,10,C4,10,D4,10,  D4_,20,C4,10,D4,10,  E4,10,G4,10,D4,10,D4_,10,  D4,10,C4,10,D4,20,  C4,20,D4,20, //26
	        G4,20,A4,20,  D4_,10,E4,20,D4,10,  D4_,10,D4,10,C4,20,  C4,20,D4,20,  D4_,20,D4,10,D4,10,  C4,10,D4,10,E4,10,G4,10,  A4,10,E4,10,G4,10,D4,10,  E4,10,C4,10,D4,10,C4,10,
	        E4,20,G4,20,  A4,10,E4,10,G4,10,D4,10,  E4,10,C4,10,D4_,10,E4,10,  D4,10,D4,10,C4,10,D4,10,  D4_,20,C4,10,D4,10,  E4,10,G4,10,D4,10,D4_,10,  D4,10,C4,10,D4,20,  C4,20,D4,20,
	        C4,20,G3,10,A3,10,  C4,20,G3,10,A3,10,  C4,10,D4,10,E4,10,C4,10,  F4,10,E4,10,F4,10,G4,10,  C4,20,C4,20,  G3,10,A3,10,C4,10,G3,10,  F4,10,E4,10,D4,10,C4,10,  F3,10,E3,10,F3,10,G3,10, //28
	        C4,20,G3,10,A3,10,  C4,20,G3,10,A3,10,  C4,10,C4,10,D4,10,E4,10,  C4,10,G3,10,A3,10,G3,10,  C4,20,C4,10,B3,10,  C4,10,G3,10,A3,10,C4,10,  F4,10,E4,10,F4,10,G4,10,  C4,20,B3,20,  //27
	        C4,20,G3,10,A3,10,  C4,20,G3,10,A3,10,  C4,10,D4,10,E4,10,C4,10,  F4,10,E4,10,F4,10,G4,10,  C4,20,C4,20,  G3,10,A3,10,C4,10,G3,10,  F4,10,E4,10,D4,10,C4,10,  F3,10,E3,10,F3,10,G3,10,  
          C4,20,G3,10,A3,10,  C4,20,G3,10,A3,10,  C4,10,C4,10,D4,10,E4,10,  C4,10,G3,10,A3,10,G3,10,  C4,20,C4,10,B3,10,  C4,10,G3,10,A3,10,C4,10,  F4,10,E4,10,F4,10,G4,10,  C4,20,B3,20	
           };
void Play_song(void)
{
	int i,tim,tm0,pre;
	for(i=0;i<54;i+=2)//音符数
			{
        pre=720000/song1[i];//根据频率计算预分频值
				tim=song1[i+1]*8;	//控速
				tm0=0;
				Play_Music(pre,tim,tm0);
			}
	while (1)
		{
			for(i=0;i<424;i+=2)//音符数
			{
        pre=720000/song2[i];//根据频率计算预分频值
				tim=song2[i+1]*8;	//控速
				tm0=0;
				Play_Music(pre,tim,tm0);
			}
		}
}

extern u8g2_t u8g2;
extern int8_t cat_run_x, cat_run_x_trg;
extern int8_t cat_run_y, cat_run_y_trg;
extern uint8_t cat_flag;
extern int8_t ui_run(int8_t *a, int8_t *a_trg, int8_t step, int8_t slow_cnt);
uint8_t bmp = 0;
extern uint8_t keynum;
extern int8_t ui_index;                         //当前UI在哪个界面
extern int8_t ui_state;                         //UI状态机
extern int8_t menu_y_trg;                  //菜单y 和菜单的y目标
void TIM3_IRQHandler(void)	//中断函数
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET)
	{
        TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
//		bmp++;
//		if (bmp==6)bmp=0;
//        u8g2_ClearBuffer(&u8g2);         // 清除内部缓冲区
//		ui_run(&cat_run_x, &cat_run_x_trg, 10, 4);	
//		// if(cat_flag == CAT_ENTER) ui_run(&cat_run_x, &cat_run_x_trg, 10, 4);
//		// else if(cat_flag == CAT_QUIT) ui_run(&cat_run_y, &cat_run_y_trg, 10, 4);
////		printf("2222\r\n");	

//		if(keynum == ROCK1_KEY_LongCick)                                  //id =0 是keyup
//		{
//			 ui_index = E_MENU;                  //把当前的UI设为菜单页面，一会会在他的处理函数中运算    
//			 ui_state = E_STATE_RUN_CAT;         //UI状态设置为 跑向图片【其实是跑向菜单】

//			 menu_y_trg = 0;                     //设置菜单界面的目标   

//			 cat_run_y_trg = 64;                     //设置图片界面的目标

//			 cat_flag = CAT_QUIT;
//		 }

//        switch(bmp) 
//			{
//            case 0:
//                u8g2_DrawXBMP(&u8g2, cat_run_x, cat_run_y, 128, 64, cat0);
//                break;
//            case 1:
//                u8g2_DrawXBMP(&u8g2, cat_run_x, cat_run_y, 128, 64, cat1);
//                break;
//            case 2:
//                u8g2_DrawXBMP(&u8g2, cat_run_x, cat_run_y, 128, 64, cat2);
//                break;
//            case 3:
//                u8g2_DrawXBMP(&u8g2, cat_run_x, cat_run_y, 128, 64, cat3);
//                break;
//            case 4:
//                u8g2_DrawXBMP(&u8g2, cat_run_x, cat_run_y, 128, 64, cat4);
//                break;
//            case 5:
//                u8g2_DrawXBMP(&u8g2, cat_run_x, cat_run_y, 128, 64, cat5);
//                break;
//            default:
//                break;
//			}
//            u8g2_SendBuffer(&u8g2);          // transfer internal memory to the   
			// if(ui_run(&cat_run_y, &cat_run_y_trg, 10, 4) == 0 )
			// {
			// 	TIM_Cmd(TIM3,DISABLE);
			// }         
		}
}

