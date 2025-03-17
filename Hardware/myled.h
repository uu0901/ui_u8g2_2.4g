#ifndef __MYLED_H
#define __MYLED_H

#define LED0_Pin     GPIO_Pin_7
#define LED1_Pin     GPIO_Pin_6
#define LED2_Pin     GPIO_Pin_5
#define LED3_Pin     GPIO_Pin_4     //PB4   JNTRST
#define LED4_Pin     GPIO_Pin_3     //PB3   JTDO
#define LED5_Pin     GPIO_Pin_15    //PA15  JTDI
#define LED6_Pin     GPIO_Pin_12
#define LED7_Pin     GPIO_Pin_11    

#define BUZZER_Pin   GPIO_Pin_14

#define LED0         0
#define LED1         1
#define LED2         2
#define LED3         3
#define LED4         4
#define LED5         5
#define LED6         6
#define LED7         7

#define ON           1
#define OFF          0
/* LED0 ~ LED4 GPIOB
   LED5 ~ LED7 GPIOA
*/
#define LED0_On()          GPIO_ResetBits(GPIOB, LED0_Pin);
#define LED0_Off()         GPIO_SetBits(GPIOB, LED0_Pin);
#define LED1_On()          GPIO_ResetBits(GPIOB, LED1_Pin);
#define LED1_Off()         GPIO_SetBits(GPIOB, LED1_Pin);
#define LED2_On()          GPIO_ResetBits(GPIOB, LED2_Pin);
#define LED2_Off()         GPIO_SetBits(GPIOB, LED2_Pin);
#define LED3_On()          GPIO_ResetBits(GPIOB, LED3_Pin);
#define LED3_Off()         GPIO_SetBits(GPIOB, LED3_Pin);
#define LED4_On()          GPIO_ResetBits(GPIOB, LED4_Pin);
#define LED4_Off()         GPIO_SetBits(GPIOB, LED4_Pin);
#define LED5_On()          GPIO_ResetBits(GPIOA, LED5_Pin);
#define LED5_Off()         GPIO_SetBits(GPIOA, LED5_Pin);
#define LED6_On()          GPIO_ResetBits(GPIOA, LED6_Pin);
#define LED6_Off()         GPIO_SetBits(GPIOA, LED6_Pin);
#define LED7_On()          GPIO_ResetBits(GPIOA, LED7_Pin);
#define LED7_Off()         GPIO_SetBits(GPIOA, LED7_Pin);
/*  
   BUZZER GPIOC
*/
#define BUZZER_On()        GPIO_SetBits(GPIOC, BUZZER_Pin);
#define BUZZER_Off()       GPIO_ResetBits(GPIOC, BUZZER_Pin);

#define Mode_OFF            0
#define Mode_ON             1
#define Mode_SlowFlash      2
#define Mode_FastFlash      3
#define Mode_PotFlash       4
void My_LED_Init(void);
void My_Buzzer_Init(void);
void LEDx_SetMode(uint8_t LEDx, uint8_t Mode);
void BUZZER_SetMode(uint8_t Mode);
void LED_Tick(void);

#endif
