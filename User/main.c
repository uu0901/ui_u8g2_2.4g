#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "LED.h"
#include "Key.h"
#include "u8g2.h"
#include "iic_oled.h"
#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "timer2.h"
#include "usart.h"	 
#include "ui.h"
#include "mykey.h" 
#include "myled.h" 
#include "ad.h" 
#include "playmusic.h" 


#include "mpu6050.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"   
u8g2_t u8g2;


extern int8_t frame_len;
extern int8_t frame_len_trg;
extern int8_t ui_select;
extern int8_t ui_index;

uint8_t keynum;


//#include "ui_bmp_data.h"
int main(void)
{

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);     //设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	My_Key_Init();
	My_LED_Init();
    uart_init(115200);
    printf("Hello\r\n");
    OLED_Init();                         //IIC OLED 端口初始化
    TIM2_Int_Init(1000 - 1, 72 - 1);
    //u8g2初始化
    u8g2_Setup_ssd1306_i2c_128x64_noname_f(&u8g2, U8G2_R0, u8x8_byte_hw_i2c, u8x8_gpio_and_delay);
    u8g2_InitDisplay(&u8g2);

    //u8g2初始化


    frame_len = frame_len_trg = list[ui_select].len * 13;            //框框的宽度设置，默认ui_select是0，也就是一个菜单的len（长度）乘以每个字符的长度（6）才是框框真正的宽度
    ui_index = E_MENU;//设置UI默认进入菜单
    AD_Init();
    
    Video_init();
//	Music_init();
//	Play_song();
    
//    u8g2_ClearBuffer(&u8g2);         // 清除内部缓冲区
//    u8g2_DrawXBMP(&u8g2, 0, 0, 128, 64, cat0);
//    u8g2_SendBuffer(&u8g2);          // transfer internal memory to the 

    MPU_Init();					//初始化MPU6050
    
    while(mpu_dmp_init())
    {
        printf("MPU6050 Error\r\n");
    //        Delay_ms(200);
    }  
    
    u8g2_SetPowerSave(&u8g2, 0);
    while (1)                                     
    {
        
       keynum = Key_GetNum();
       ui_proc();
         


//        printf("ROCK1_AD_X=%d\r\n",ROCK1_AD_X);
//        printf("ROCK1_AD_Y=%d\r\n",ROCK1_AD_Y);
//        printf("ROCK1_AD_X_HighByte=%d\r\n",ROCK1_AD_X_HighByte);
//        printf("ROCK1_AD_X_LowByte=%d\r\n",ROCK1_AD_X_LowByte);
//        printf("ROCK1_AD_Y_HighByte=%d\r\n",ROCK1_AD_Y_HighByte);
//        printf("ROCK1_AD_Y_LowByte=%d\r\n",ROCK1_AD_Y_LowByte);
        // printf("\r\n");



        // printf("ROCK2_AD_X=%d\r\n",ROCK2_AD_X);
        // printf("ROCK2_AD_Y=%d\r\n",ROCK2_AD_Y);
        // printf("ROCK2_AD_X_HighByte=%d\r\n",ROCK2_AD_X_HighByte);
        // printf("ROCK2_AD_X_LowByte=%d\r\n",ROCK2_AD_X_LowByte);
        // printf("ROCK2_AD_Y_HighByte=%d\r\n",ROCK2_AD_Y_HighByte);
        // printf("ROCK2_AD_Y_LowByte=%d\r\n",ROCK2_AD_Y_LowByte);

    }
}

void TIM2_IRQHandler(void)   //TIM2中断
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)  //检查TIM3更新中断发生与否
    {
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update  );  //清除TIMx更新中断标志 

        Key_Tick();
        LED_Tick();

    }
}

