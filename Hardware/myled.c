#include "stm32f10x.h"                  // Device header
#include "myled.h" 
uint8_t LED0_Mode, LED1_Mode, LED2_Mode, LED3_Mode, LED4_Mode, LED5_Mode, LED6_Mode, LED7_Mode;
uint8_t BUZZER_Mode;

uint16_t LED0_Count, LED1_Count, LED2_Count, LED3_Count, LED4_Count, LED5_Count, LED6_Count, LED7_Count; //LED1计数值
uint16_t BUZZER_Count;

void My_LED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);     //开启AFIO时钟
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);//使能JTAGDisable，即禁用JTAG接口


    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = LED0_Pin | LED1_Pin| LED2_Pin | LED3_Pin | LED4_Pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = LED5_Pin | LED6_Pin | LED7_Pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_ResetBits(GPIOB, LED0_Pin | LED1_Pin | LED2_Pin | LED3_Pin | LED4_Pin);
    GPIO_ResetBits(GPIOA, LED5_Pin | LED6_Pin | LED7_Pin);
}
void My_Buzzer_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);


    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = BUZZER_Pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_ResetBits(GPIOC, BUZZER_Pin);
}
void LEDx_SetMode(uint8_t LEDx, uint8_t Mode)
{
    switch (LEDx)
    {
    case LED0:
        if (Mode != LED0_Mode)       //不加这个if那么主函数只能在需要改变模式时调用此函数，不能频繁调用，否则就频繁清count，led无法闪烁，加了这个判断就可以频繁调用这个函数了
        {
            LED0_Mode = Mode;
            LED0_Count = 0;          //每次切换LED1Mode，cout计数值清0，闪烁都从一个周期的最开始进行
        }
        break;
    case LED1:
        if (Mode != LED1_Mode)       //不加这个if那么主函数只能在需要改变模式时调用此函数，不能频繁调用，否则就频繁清count，led无法闪烁，加了这个判断就可以频繁调用这个函数了
        {
            LED1_Mode = Mode;
            LED1_Count = 0;          //每次切换LED1Mode，cout计数值清0，闪烁都从一个周期的最开始进行
        }
        break;
    case LED2:
        if (Mode != LED2_Mode)       //不加这个if那么主函数只能在需要改变模式时调用此函数，不能频繁调用，否则就频繁清count，led无法闪烁，加了这个判断就可以频繁调用这个函数了
        {
            LED2_Mode = Mode;
            LED2_Count = 0;          //每次切换LED1Mode，cout计数值清0，闪烁都从一个周期的最开始进行
        }
        break;
    case LED3:
        if (Mode != LED3_Mode)       //不加这个if那么主函数只能在需要改变模式时调用此函数，不能频繁调用，否则就频繁清count，led无法闪烁，加了这个判断就可以频繁调用这个函数了
        {
            LED3_Mode = Mode;
            LED3_Count = 0;          //每次切换LED1Mode，cout计数值清0，闪烁都从一个周期的最开始进行
        }
        break;  
    case LED4:
        if (Mode != LED4_Mode)       //不加这个if那么主函数只能在需要改变模式时调用此函数，不能频繁调用，否则就频繁清count，led无法闪烁，加了这个判断就可以频繁调用这个函数了
        {
            LED4_Mode = Mode;
            LED4_Count = 0;          //每次切换LED1Mode，cout计数值清0，闪烁都从一个周期的最开始进行
        }
        break;        
    case LED5:
        if (Mode != LED5_Mode)       //不加这个if那么主函数只能在需要改变模式时调用此函数，不能频繁调用，否则就频繁清count，led无法闪烁，加了这个判断就可以频繁调用这个函数了
        {
            LED5_Mode = Mode;
            LED5_Count = 0;          //每次切换LED1Mode，cout计数值清0，闪烁都从一个周期的最开始进行
        }
        break;
    case LED6:
        if (Mode != LED6_Mode)       //不加这个if那么主函数只能在需要改变模式时调用此函数，不能频繁调用，否则就频繁清count，led无法闪烁，加了这个判断就可以频繁调用这个函数了
        {
            LED6_Mode = Mode;
            LED6_Count = 0;          //每次切换LED1Mode，cout计数值清0，闪烁都从一个周期的最开始进行
        }
        break;
    case LED7:
        if (Mode != LED7_Mode)       //不加这个if那么主函数只能在需要改变模式时调用此函数，不能频繁调用，否则就频繁清count，led无法闪烁，加了这个判断就可以频繁调用这个函数了
        {
            LED7_Mode = Mode;
            LED7_Count = 0;          //每次切换LED1Mode，cout计数值清0，闪烁都从一个周期的最开始进行
        }
        break;
    default:
        break;
    }

}
void BUZZER_SetMode(uint8_t Mode)
{
    if (Mode != BUZZER_Mode)       //不加这个if那么主函数只能在需要改变模式时调用此函数，不能频繁调用，否则就频繁清count，led无法闪烁，加了这个判断就可以频繁调用这个函数了
    {
        BUZZER_Mode = Mode;
        BUZZER_Count = 0;          //每次切换LED1Mode，cout计数值清0，闪烁都从一个周期的最开始进行
    }
}
void LEDx_SetState(uint8_t LEDx, uint8_t State)
{
    switch (LEDx)
    {
    case LED0:
        if (State == ON)      
        {
            LED0_On();
        }
        else if (State == OFF)
        {
            LED0_Off();
        }
        break;
    case LED1:
        if (State == ON)      
        {
            LED1_On();
        }
        else if (State == OFF)
        {
            LED1_Off();
        }
        break;
    case LED2:
        if (State == ON)      
        {
            LED2_On();
        }
        else if (State == OFF)
        {
            LED2_Off();
        }
        break;
    case LED3:
        if (State == ON)      
        {
            LED3_On();
        }
        else if (State == OFF)
        {
            LED3_Off();
        }
        break; 
    case LED4:
        if (State == ON)      
        {
            LED4_On();
        }
        else if (State == OFF)
        {
            LED4_Off();
        }
        break;       
    case LED5:
        if (State == ON)      
        {
            LED5_On();
        }
        else if (State == OFF)
        {
            LED5_Off();
        }
        break;
    case LED6:
        if (State == ON)      
        {
            LED6_On();
        }
        else if (State == OFF)
        {
            LED6_Off();
        }
        break;
    case LED7:
        if (State == ON)      
        {
            LED7_On();
        }
        else if (State == OFF)
        {
            LED7_Off();
        }
        break;
    default:
        break;
    }

}
void BUZZER_SetState(uint8_t State)
{
    if (State == ON)      
    {
        BUZZER_On();
    }
    else if (State == OFF)
    {
        BUZZER_Off();
    }
}

void LED_Tick(void)//led动作函数1ms调用一次
{
/*LED0*/
    if (LED0_Mode == Mode_OFF)                 //mode=0关LED1
    {
       LEDx_SetState(LED0, OFF);
    }
    else if (LED0_Mode == Mode_ON)            //mode=1开LED1
    {
       LEDx_SetState(LED0, ON);
    }
    else if (LED0_Mode == Mode_SlowFlash)            //mode=2慢闪LED1
    {
        LED0_Count ++;
        LED0_Count %= 1000;             //if(LED1_Count > 999)  LED1_Count = 0;     count<1000时取余等于本身，count=1000时取余=0           =1000就请0     限幅0~999
        //LED1周期为1ms*1000=1000ms
        if (LED0_Count < 500)           //0~499     [(499-0)+1]*1ms=500ms
        {
            LEDx_SetState(LED0, ON);
        }
        else                              //500~999  [(999-500)+1]*1ms=500ms 
        {
            LEDx_SetState(LED0, OFF);
        }
    }
    else if (LED0_Mode == Mode_FastFlash)            //mode=3快闪LED1
    {
        LED0_Count ++;
        LED0_Count %= 100;              //计数到达100清0     =100就请0     限幅0~99
        //LED1周期为1ms*100=100ms
        if (LED0_Count < 50)            //0~49     [(49-0)+1]*1ms=50ms
        {
            LEDx_SetState(LED0, ON);
        }
        else                             //50~99    [(99-50)+1]*1ms=50ms
        {
            LEDx_SetState(LED0, OFF);
        }
    }
    else if (LED1_Mode == Mode_PotFlash)            //mode=4点闪LED1
    {
        LED0_Count ++;
        LED0_Count %= 1000;            //计数到达1000清0     =1000就请0     限幅0~999
        //LED1周期为1ms*1000=1000ms
        if (LED0_Count < 100)           //0~99     [(99-0)+1]*1ms=100ms
        {
            LEDx_SetState(LED0, ON);
        }
        else                             //100~999  [(999-100)+1]*1ms=900ms
        {
            LEDx_SetState(LED0, OFF);
        }
    }


/*LED1*/
    if (LED1_Mode == Mode_OFF)                 //mode=0关LED1
    {
        LEDx_SetState(LED1, OFF);
    }
    else if (LED1_Mode == Mode_ON)            //mode=1开LED1
    {
        LEDx_SetState(LED1, ON);
    }
    else if (LED1_Mode == Mode_SlowFlash)            //mode=2慢闪LED1
    {
        LED1_Count ++;
        LED1_Count %= 1000;             //if(LED1_Count > 999)  LED1_Count = 0;     count<1000时取余等于本身，count=1000时取余=0           =1000就请0     限幅0~999
        //LED1周期为1ms*1000=1000ms
        if (LED1_Count < 500)           //0~499     [(499-0)+1]*1ms=500ms
        {
            LEDx_SetState(LED1, ON);
        }
        else                              //500~999  [(999-500)+1]*1ms=500ms 
        {
            LEDx_SetState(LED1, OFF);
        }
    }
    else if (LED1_Mode == Mode_FastFlash)            //mode=3快闪LED1
    {
        LED1_Count ++;
        LED1_Count %= 100;              //计数到达100清0     =100就请0     限幅0~99
        //LED1周期为1ms*100=100ms
        if (LED1_Count < 50)            //0~49     [(49-0)+1]*1ms=50ms
        {
            LEDx_SetState(LED1, ON);
        }
        else                             //50~99    [(99-50)+1]*1ms=50ms
        {
            LEDx_SetState(LED1, OFF);
        }
    }
    else if (LED1_Mode == Mode_PotFlash)            //mode=4点闪LED1
    {
        LED1_Count ++;
        LED1_Count %= 1000;            //计数到达1000清0     =1000就请0     限幅0~999
        //LED1周期为1ms*1000=1000ms
        if (LED1_Count < 100)           //0~99     [(99-0)+1]*1ms=100ms
        {
            LEDx_SetState(LED1, ON);
        }
        else                             //100~999  [(999-100)+1]*1ms=900ms
        {
            LEDx_SetState(LED1, OFF);
        }
    }

/*LED2*/
    if (LED2_Mode == Mode_OFF)
    {
        LEDx_SetState(LED2, OFF);
    }
    else if (LED2_Mode == Mode_ON)
    {
        LEDx_SetState(LED2, ON);
    }
    else if (LED2_Mode == Mode_SlowFlash)
    {
        LED2_Count ++;
        LED2_Count %= 1000;

        if (LED2_Count < 500)
        {
            LEDx_SetState(LED2, ON);
        }
        else
        {
            LEDx_SetState(LED2, OFF);
        }
    }
    else if (LED2_Mode == Mode_FastFlash)
    {
        LED2_Count ++;
        LED2_Count %= 100;

        if (LED2_Count < 50)
        {
            LEDx_SetState(LED2, ON);
        }
        else
        {
            LEDx_SetState(LED2, OFF);
        }
    }
    else if (LED2_Mode == Mode_PotFlash)
    {
        LED2_Count ++;
        LED2_Count %= 1000;

        if (LED2_Count < 100)
        {
            LEDx_SetState(LED2, ON);
        }
        else
        {
            LEDx_SetState(LED2, OFF);
        }
    }

/*LED3*/
    if (LED3_Mode == Mode_OFF)
    {
        LEDx_SetState(LED3, OFF);
    }
    else if (LED3_Mode == Mode_ON)
    {
        LEDx_SetState(LED3, ON);
    }
    else if (LED3_Mode == Mode_SlowFlash)
    {
        LED3_Count ++;
        LED3_Count %= 1000;

        if (LED3_Count < 500)
        {
            LEDx_SetState(LED3, ON);
        }
        else
        {
            LEDx_SetState(LED3, OFF);
        }
    }
    else if (LED3_Mode == Mode_FastFlash)
    {
        LED3_Count ++;
        LED3_Count %= 100;

        if (LED3_Count < 50)
        {
            LEDx_SetState(LED3, ON);
        }
        else
        {
            LEDx_SetState(LED3, OFF);
        }
    }
    else if (LED3_Mode == Mode_PotFlash)
    {
        LED3_Count ++;
        LED3_Count %= 1000;

        if (LED3_Count < 100)
        {
            LEDx_SetState(LED3, ON);
        }
        else
        {
            LEDx_SetState(LED3, OFF);
        }
    }


/*LED4*/
    if (LED4_Mode == Mode_OFF)
    {
        LEDx_SetState(LED4, OFF);
    }
    else if (LED4_Mode == Mode_ON)
    {
        LEDx_SetState(LED4, ON);
    }
    else if (LED4_Mode == Mode_SlowFlash)
    {
        LED4_Count ++;
        LED4_Count %= 1000;

        if (LED4_Count < 500)
        {
            LEDx_SetState(LED4, ON);
        }
        else
        {
            LEDx_SetState(LED4, OFF);
        }
    }
    else if (LED4_Mode == Mode_FastFlash)
    {
        LED4_Count ++;
        LED4_Count %= 100;

        if (LED4_Count < 50)
        {
            LEDx_SetState(LED4, ON);
        }
        else
        {
            LEDx_SetState(LED4, OFF);
        }
    }
    else if (LED4_Mode == Mode_PotFlash)
    {
        LED4_Count ++;
        LED4_Count %= 1000;

        if (LED4_Count < 100)
        {
            LEDx_SetState(LED4, ON);
        }
        else
        {
            LEDx_SetState(LED4, OFF);
        }
    }
/*LED5*/
    if (LED5_Mode == Mode_OFF)
    {
        LEDx_SetState(LED5, OFF);
    }
    else if (LED5_Mode == Mode_ON)
    {
        LEDx_SetState(LED5, ON);
    }
    else if (LED5_Mode == Mode_SlowFlash)
    {
        LED5_Count ++;
        LED5_Count %= 1000;

        if (LED5_Count < 500)
        {
            LEDx_SetState(LED5, ON);
        }
        else
        {
            LEDx_SetState(LED5, OFF);
        }
    }
    else if (LED5_Mode == Mode_FastFlash)
    {
        LED5_Count ++;
        LED5_Count %= 100;

        if (LED5_Count < 50)
        {
            LEDx_SetState(LED5, ON);
        }
        else
        {
            LEDx_SetState(LED5, OFF);
        }
    }
    else if (LED5_Mode == Mode_PotFlash)
    {
        LED5_Count ++;
        LED5_Count %= 1000;

        if (LED5_Count < 100)
        {
            LEDx_SetState(LED5, ON);
        }
        else
        {
            LEDx_SetState(LED5, OFF);
        }
    }
/*LED6*/
    if (LED6_Mode == Mode_OFF)
    {
        LEDx_SetState(LED6, OFF);
    }
    else if (LED6_Mode == Mode_ON)
    {
        LEDx_SetState(LED6, ON);
    }
    else if (LED6_Mode == Mode_SlowFlash)
    {
        LED6_Count ++;
        LED6_Count %= 1000;

        if (LED6_Count < 500)
        {
            LEDx_SetState(LED6, ON);
        }
        else
        {
            LEDx_SetState(LED6, OFF);
        }
    }
    else if (LED6_Mode == Mode_FastFlash)
    {
        LED6_Count ++;
        LED6_Count %= 100;

        if (LED6_Count < 50)
        {
            LEDx_SetState(LED6, ON);
        }
        else
        {
            LEDx_SetState(LED6, OFF);
        }
    }
    else if (LED5_Mode == Mode_PotFlash)
    {
        LED6_Count ++;
        LED6_Count %= 1000;

        if (LED6_Count < 100)
        {
            LEDx_SetState(LED6, ON);
        }
        else
        {
            LEDx_SetState(LED6, OFF);
        }
    }   
/*LED7*/
    if (LED7_Mode == Mode_OFF)
    {
        LEDx_SetState(LED7, OFF);
    }
    else if (LED7_Mode == Mode_ON)
    {
        LEDx_SetState(LED7, ON);
    }
    else if (LED7_Mode == Mode_SlowFlash)
    {
        LED7_Count ++;
        LED7_Count %= 1000;

        if (LED7_Count < 500)
        {
            LEDx_SetState(LED7, ON);
        }
        else
        {
            LEDx_SetState(LED7, OFF);
        }
    }
    else if (LED7_Mode == Mode_FastFlash)
    {
        LED7_Count ++;
        LED7_Count %= 100;

        if (LED7_Count < 50)
        {
            LEDx_SetState(LED7, ON);
        }
        else
        {
            LEDx_SetState(LED7, OFF);
        }
    }
    else if (LED7_Mode == Mode_PotFlash)
    {
        LED7_Count ++;
        LED7_Count %= 1000;

        if (LED7_Count < 100)
        {
            LEDx_SetState(LED7, ON);
        }
        else
        {
            LEDx_SetState(LED7, OFF);
        }
    }   
/*BUZZER*/
    if (BUZZER_Mode == Mode_OFF)
    {
        BUZZER_SetState(OFF);
    }
    else if (BUZZER_Mode == Mode_ON)
    {
        BUZZER_SetState(ON);
    }
    else if (BUZZER_Mode == Mode_SlowFlash)
    {
        BUZZER_Count ++;
        BUZZER_Count %= 1000;

        if (BUZZER_Count < 500)
        {
            BUZZER_SetState(ON);
        }
        else
        {
            BUZZER_SetState(OFF);
        }
    }
    else if (BUZZER_Mode == Mode_FastFlash)
    {
        BUZZER_Count ++;
        BUZZER_Count %= 100;

        if (BUZZER_Count < 50)
        {
            BUZZER_SetState(ON);
        }
        else
        {
            BUZZER_SetState(OFF);
        }
    }
    else if (LED7_Mode == Mode_PotFlash)
    {
        BUZZER_Count ++;
        BUZZER_Count %= 1000;

        if (BUZZER_Count < 100)
        {
            BUZZER_SetState(ON);
        }
        else
        {
            BUZZER_SetState(OFF);
        }
    }          
}
