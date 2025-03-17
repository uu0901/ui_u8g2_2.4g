#ifndef __UI_H
#define __UI_H
#include "stm32f10x.h" 
#include "u8g2.h"
extern u8g2_t u8g2;



//设置列表结构体
typedef struct
{
    char *str;
    uint8_t len;                            //菜单是英文可以用Strlen来获取长度，为了获取中文的长度这里要再添加一个len
} SETTING_LIST;
extern SETTING_LIST list[];
extern enum
{
    E_MENU,                  //菜单界面
    E_CAT,                   //图片界面
    E_PIC,                   //图片界面
    E_SETTING,               //设置界面
    E_LED,                   //LED界面
    E_ADVALUE,
    E_BAT,
    E_MPU6050,
    E_ABOUT,
    E_UI_MAX,                //界面最大值
} E_UI;
enum
{
    E_SELECT_SETTING,        //菜单界面的选择->进入设置     0
    E_SELECT_PIC,            //菜单界面的选择->进入图片     1
    E_SELECT_CAT,            //菜单界面的选择->进入猫猫     2
};

enum
{
    E_NONE,                 //运行状态机 无
    E_STATE_RUN_MENU,       //运行状态->跑向菜单
    E_STATE_RUN_CAT,        //运行状态->跑向图片
    E_STATE_RUN_SETTING,    //运行状态->跑向设置
    E_STATE_RUN_PIC,        //运行状态->跑向图片
    E_STATE_RUN_LED,        //运行状态->跑向图片

    E_STATE_ARRIVE,         //运行状态->到达
};

enum
{
    CAT_GERNER,
    CAT_QUIT,
    CAT_ENTER,
};
typedef struct
{
    uint8_t index;              //UI的索引
    void (*cb)(void);       //ui的执行函数
} UI_LIST;
uint8_t u8x8_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
uint8_t u8x8_byte_hw_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
void draw(u8g2_t *u8g2);
void PrintfVarFormat(u8g2_t *u8g2, u8g2_uint_t x, u8g2_uint_t y, const uint8_t *font, short num);
void MyTest(void);
void ui_proc(void);
void My_key_scan(void);
void key_proc(void);
void key_press_cb(void);
#endif
