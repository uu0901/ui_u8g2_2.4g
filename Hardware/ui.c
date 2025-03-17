#include "ui.h"
#include "delay.h"
#include "ui_bmp_data.h"
#include "stdio.h"
#include "stdlib.h"
#include "led.h"
#include "iic_oled.h"
#include "iic_oled.h"
#include "mykey.h" 
#include "myled.h" 
#include "ad.h" 


#include "mpu6050.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h" 
float pitch,roll,yaw; 		//欧拉角
short aacx,aacy,aacz;		//加速度传感器原始数据
short gyrox,gyroy,gyroz;	//陀螺仪原始数据
short temp;					//温度	

#define SCL_Pin GPIO_Pin_8
#define SDA_Pin GPIO_Pin_9
#define IIC_GPIO_Port GPIOB

uint8_t  ROCK1_ADC_SendFlag, ROCK2_ADC_SendFlag;
uint16_t ROCK1_AD_X, ROCK1_AD_Y, ROCK2_AD_X, ROCK2_AD_Y;
uint8_t ROCK1_AD_X_HighByte, ROCK1_AD_X_LowByte, ROCK1_AD_Y_HighByte, ROCK1_AD_Y_LowByte, \
        ROCK2_AD_X_HighByte, ROCK2_AD_X_LowByte, ROCK2_AD_Y_HighByte, ROCK2_AD_Y_LowByte;
int16_t BAT_AD_VALUE;
uint8_t u8x8_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    switch (msg)
    {
    case U8X8_MSG_DELAY_100NANO: // delay arg_int * 100 nano seconds
        __NOP();
        break;
    case U8X8_MSG_DELAY_10MICRO: // delay arg_int * 10 micro seconds
        for (uint16_t n = 0; n < 320; n++)
        {
            __NOP();
        }
        break;
    case U8X8_MSG_DELAY_MILLI: // delay arg_int * 1 milli second
        Delay_ms(1);
        break;
    case U8X8_MSG_DELAY_I2C: // arg_int is the I2C speed in 100KHz, e.g. 4 = 400 KHz
        Delay_us(5);
        break;                    // arg_int=1: delay by 5us, arg_int = 4: delay by 1.25us
    case U8X8_MSG_GPIO_I2C_CLOCK: // arg_int=0: Output low at I2C clock pin
        if (arg_int == 1)
        {
            GPIO_SetBits(IIC_GPIO_Port, SCL_Pin);
        }
        else if (arg_int == 0)
        {
            GPIO_ResetBits(IIC_GPIO_Port, SCL_Pin);
        }
        break;                   // arg_int=1: Input dir with pullup high for I2C clock pin
    case U8X8_MSG_GPIO_I2C_DATA: // arg_int=0: Output low at I2C data pin
        if (arg_int == 1)
        {
            GPIO_SetBits(IIC_GPIO_Port, SDA_Pin);
        }
        else if (arg_int == 0)
        {
            GPIO_ResetBits(IIC_GPIO_Port, SDA_Pin);
        }
        break; // arg_int=1: Input dir with pullup high for I2C data pin
    case U8X8_MSG_GPIO_MENU_SELECT:
        u8x8_SetGPIOResult(u8x8, /* get menu select pin state */ 0);
        break;
    case U8X8_MSG_GPIO_MENU_NEXT:
        u8x8_SetGPIOResult(u8x8, /* get menu next pin state */ 0);
        break;
    case U8X8_MSG_GPIO_MENU_PREV:
        u8x8_SetGPIOResult(u8x8, /* get menu prev pin state */ 0);
        break;
    case U8X8_MSG_GPIO_MENU_HOME:
        u8x8_SetGPIOResult(u8x8, /* get menu home pin state */ 0);
        break;
    default:
        u8x8_SetGPIOResult(u8x8, 1); // default return value
        break;
    }
    return 1;
}
uint8_t u8x8_byte_hw_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    static uint8_t buffer[32]; /* u8g2/u8x8 will never send more than 32 bytes between START_TRANSFER and END_TRANSFER */
    static uint8_t buf_idx;
    uint8_t *data;

    switch (msg)
    {

    case U8X8_MSG_BYTE_SEND:
        data = (uint8_t *)arg_ptr;
        while (arg_int > 0)
        {
            buffer[buf_idx++] = *data;
            data++;
            arg_int--;
        }
        break;

    case U8X8_MSG_BYTE_INIT:
        /* add your custom code to init i2c subsystem */
        break;

    case U8X8_MSG_BYTE_START_TRANSFER:
        buf_idx = 0;
        break;

    case U8X8_MSG_BYTE_END_TRANSFER:

        //		HW_I2cWrite(u8x8,buffer,buf_idx);
        // 居然给我蒙对了！
        if (buf_idx <= 0)
            break;

        /* wait for the busy falg to be reset */
        // while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));

        /* start transfer */
        I2C_GenerateSTART(I2C1, ENABLE);
        Oled_I2C_WaitEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT);

        I2C_Send7bitAddress(I2C1, u8x8_GetI2CAddress(u8x8), I2C_Direction_Transmitter);
        Oled_I2C_WaitEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);

        for (uint8_t i = 0; i < buf_idx; i++)
        {
            I2C_SendData(I2C1, buffer[i]);

            Oled_I2C_WaitEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED);
        }

        I2C_GenerateSTOP(I2C1, ENABLE);

        break;

    default:
        return 0;
    }
    return 1;
}
void draw(u8g2_t *u8g2)
{
    u8g2_ClearBuffer(u8g2); 
    u8g2_SetFontMode(u8g2, 1); 
    u8g2_SetFontDirection(u8g2, 0); 
    u8g2_SetFont(u8g2, u8g2_font_inb24_mf);
    u8g2_DrawStr(u8g2, 0, 20, "U");
    
    u8g2_SetFontDirection(u8g2, 1);
    u8g2_SetFont(u8g2, u8g2_font_inb30_mn);
    u8g2_DrawStr(u8g2, 21,8,"8");
        
    u8g2_SetFontDirection(u8g2, 0);
    u8g2_SetFont(u8g2, u8g2_font_inb24_mf);
    u8g2_DrawStr(u8g2, 51,30,"g");
    u8g2_DrawStr(u8g2, 67,30,"\xb2");
    
    u8g2_DrawHLine(u8g2, 2, 35, 47);
    u8g2_DrawHLine(u8g2, 3, 36, 47);
    u8g2_DrawVLine(u8g2, 45, 32, 12);
    u8g2_DrawVLine(u8g2, 46, 33, 12);
  
    u8g2_SetFont(u8g2, u8g2_font_4x6_tr);
    u8g2_DrawStr(u8g2, 1,54,"github.com/olikraus/u8g2");
    
    u8g2_SendBuffer(u8g2);
    Delay_ms(1000);
}

void PrintfVarFormat(u8g2_t *u8g2, u8g2_uint_t x, u8g2_uint_t y,const uint8_t *font, short num)
{
    char buff[10];
    u8g2_SetFont(u8g2,font);//设定字体
    sprintf(buff,"%4d",num);//4是长度的限制 0是不足4位前面补0 如num为100 打印出来是0100 此函数的返回值是buff的长度
    u8g2_DrawStr(u8g2,x,y,buff);
}
void PrintfVarFormatFloat(u8g2_t *u8g2, u8g2_uint_t x, u8g2_uint_t y,const uint8_t *font, float num)
{
    char buff[10];
    u8g2_SetFont(u8g2,font);//设定字体
    sprintf(buff,"%.2f",num);//4是长度的限制 0是不足4位前面补0 如num为100 打印出来是0100 此函数的返回值是buff的长度
    u8g2_DrawStr(u8g2,x,y,buff);
}
void MyTest(void)
{
    u8g2_ClearBuffer(&u8g2);                                              //清空缓冲区
    
                                                                          //画线
    Delay_ms(1000);
    u8g2_DrawLine(&u8g2, 0,0, 127, 63);         //起点0 0终点127 63
    u8g2_DrawLine(&u8g2, 127,0, 0, 63);         //起点127 0终点0 63
    u8g2_SendBuffer(&u8g2);                     //发送缓冲区
    Delay_ms(1000);
                                                                          //画线

    u8g2_ClearBuffer(&u8g2);                                             //清空缓冲区 
    
    draw(&u8g2);                                                          //官方测试函数
    
    u8g2_DrawXBMP(&u8g2, 0, 0, 25, 25, u8g_logo_bits);                    //绘制图像
    
    u8g2_DrawBox(&u8g2, 30, 0, 25, 25);                                    //绘制实心方框
  
    u8g2_SetFont(&u8g2,u8g2_font_helvR08_tr);                             //设置字体
    u8g2_DrawButtonUTF8(&u8g2,72, 20, U8G2_BTN_BW2, 0, 2, 2, "Btn" );     //绘制方框+文本
    
    u8g2_DrawCircle(&u8g2,10, 35, 10, U8G2_DRAW_ALL);                   //绘制空心圆

    u8g2_DrawDisc(&u8g2,30, 35, 10, U8G2_DRAW_ALL);                     //绘制实心圆

    u8g2_DrawEllipse(&u8g2,50, 45, 15, 10, U8G2_DRAW_ALL);              //绘制空心椭圆

    u8g2_DrawFilledEllipse(&u8g2,80, 45, 15, 10, U8G2_DRAW_ALL);        //绘制实心椭圆

    u8g2_DrawFrame(&u8g2,0,50,25,13);                                   //绘制空心矩形
    
    u8g2_SetFont(&u8g2,u8g2_font_unifont_t_symbols);                   //设置字体

    u8g2_DrawGlyph(&u8g2,108, 20, 0x2603);                             //绘制官方符号
   u8g2_DrawGlyphX2(&u8g2,96,60, 0x2603);                             //2倍绘制官方符号
    u8g2_DrawGlyphX2(&u8g2,96,60, 0x2614);                             //绘制官方符号

    u8g2_DrawHLine(&u8g2, 0, 10, 50);                                  //绘制水平线

    u8g2_DrawLine(&u8g2, 0,0, 127, 63);                                //绘制两点线

    u8g2_DrawRBox(&u8g2,  0, 0, 50, 40, 15);                            //绘制圆角实心方框

    u8g2_DrawRFrame(&u8g2,  0, 45, 50, 15, 5);                          //绘制圆角空心方框
    
    u8g2_SetFont(&u8g2,u8g2_font_ncenB14_tr);                          //设置字体
    u8g2_DrawStr(&u8g2,0,15,"Hello World!");                            //显示字符串

    u8g2_DrawTriangle(&u8g2, 0, 0, 50, 50,50, 0);                       //绘制三角形
    
   u8g2_DrawPixel(&u8g2, 100, 30);                                     //绘制一个像素点

    u8g2_SetFont(&u8g2,u8g2_font_unifont_t_symbols);                   //设置字体
   u8g2_DrawUTF8(&u8g2, 5, 20, "Snowman: 鈽?");                  //显示字符串+官方图形

    u8g2_DrawVLine(&u8g2, 50, 10, 30);                                  //绘制垂直线


    u8g2_SetFont(&u8g2,u8g2_font_ncenB14_tr);                         //设置字体
    u8g2_DrawStr(&u8g2,5,20,"Hello World!");                            //显示字符串，方便下面的获取基准线以上和以下的测试
    uint8_t i = 0;
    //获取基准线以上的高度
    i = u8g2_GetAscent(&u8g2);//传递i
    PrintfVarFormat(&u8g2, 0,40,u8g2_font_unifont_t_symbols, i);//显示i
    //获取基准线以上的高度
    
    //获取基准线以下的高度
    i = u8g2_GetDescent(&u8g2);//传递i
    PrintfVarFormat(&u8g2, 0,60,u8g2_font_unifont_t_symbols, i);//显示i
    //获取基准线以下的高度
    
    //获取屏幕高度
    i = u8g2_GetDisplayHeight(&u8g2);
    PrintfVarFormat(&u8g2, 0,20,u8g2_font_ncenB14_tr, i);//显示i
    //获取屏幕宽度
    i = u8g2_GetDisplayWidth(&u8g2);
    PrintfVarFormat(&u8g2, 0,40,u8g2_font_ncenB14_tr, i);//显示i

    
    u8g2_SendBuffer(&u8g2);                 //发送缓冲区
    
    
                                                    //绘制进度条
    char buff[20];
    for( i=0;i<=100;i=i+1)
    {
        u8g2_ClearBuffer(&u8g2); 
        u8g2_SetFont(&u8g2,u8g2_font_ncenB08_tf);//字体
        sprintf(buff,"%d%%",(int)(i/100.0*100));
        u8g2_DrawStr(&u8g2,105,30,buff);        //当前进度显示
        u8g2_DrawBox(&u8g2,2,22,i,10);          //填充框实心矩形框
        u8g2_DrawFrame(&u8g2,0,20,103,14);      //空心矩形框
        u8g2_SendBuffer(&u8g2);
    }
                                                    //绘制进度条
//        LED1_ON();
//        Delay_ms(500);
//        LED1_OFF();
//        Delay_ms(500);

        uint8_t keynum1;//全局变量
        uint8_t keynum2;//全局变量
        u8g2_ClearBuffer(&u8g2);                                           //清空缓冲区       
//        keynum1 = get_io_val(0);
//       keynum2 = get_io_val(1);
        PrintfVarFormat(&u8g2, 0,20,u8g2_font_ncenB14_tr, keynum1);//显示i
      PrintfVarFormat(&u8g2, 0,40,u8g2_font_ncenB14_tr, keynum2);//显示i
       u8g2_SendBuffer(&u8g2);                                             //发送缓冲区
}


//****************************菜单部分****************************//
int8_t list_len;                             //菜单列表数组中元素的数量
uint8_t i;                                   //遍历菜单列表数组中元素的数量的变量


int8_t ui_select = 2;                        //当前UI的选项
int8_t y_select = 0;                         //当前设置的文本的选项
int8_t ui_index = 0;                         //当前UI在哪个界面
int8_t ui_state = 0;                         //UI状态机

int8_t menu_x, menu_x_trg;                  //菜单x 和菜单的x目标
int8_t menu_y, menu_y_trg;                  //菜单y 和菜单的y目标

int8_t cat_run_x, cat_run_x_trg;                    //图片页面x 和图片的x目标
int8_t cat_run_y, cat_run_y_trg;                    //图片页面y 和图片的y目标

int8_t pic_x, pic_x_trg;                    //图片页面x 和图片的x目标
int32_t pic_y, pic_y_trg;                    //图片页面y 和图片的y目标

int8_t setting_x = 0, setting_x_trg = 0;    //设置界面x 和设置界面x目标
int8_t setting_y = 18, setting_y_trg = 18;   //设置界面y 和设置界面y目标

int8_t frame_len, frame_len_trg;            //框框的宽度 和框框的宽度目标
int8_t frame_y, frame_y_trg;                //框框的y 和框框的y目标


int8_t Indic_y = 0, Indic_y_trg = 0;                           //指示器的y 和指示器y目标
int8_t Indic_x = 116, Indic_x_trg = 116;                           //指示器的y 和指示器y目标

int8_t Indic_vertical_x = 120, Indic_vertical_x_trg = 120;     //指示器垂线的x和指示器垂线的x目标
int8_t Indic_vertical_y = 0, Indic_vertical_y_trg = 0;     //指示器垂线的x和指示器垂线的x目标

int8_t Indic_Horizental_x = 118, Indic_Horizental_x_trg = 118;     //指示器垂线的x和指示器垂线的x目标
int8_t Indic_Horizental_y = 4, Indic_Horizental_y_trg = 4;     //指示器垂线的x和指示器垂线的x目标




uint8_t cat_flag = CAT_GERNER;
//设置列表
SETTING_LIST list[] = 
{
    {"Led", 3},
    {"ADValue ", 5},
    {"BAT ", 3},
    {"+ MPU6050 ", 7},
    {"+ NRF24L01 ", 7},
    {"+ About   ", 6},
};

//****************************菜单部分****************************//

enum
{
    None = 0,
    Up,
    down,
    left,
    righ,
};
uint8_t flag = 0;
void checkRock1AdX_add(int rock1_ad_x)//检测X轴增，摇杆向上拨动为增
{
    uint8_t newFlag = flag;         //默认flag是0也就是松手
    static uint8_t prevFlag;         //默认prevFlag是0也就是松手， 用于保存上次的 flag 状态
    // 检查是否达到3600左右，向上拨动检测
    if (rock1_ad_x >= 3590 && rock1_ad_x <= 4500 && flag == None) //上一次是松手，这一次拨动才有效
    {
        newFlag = Up;//向上拨动标志
        printf("Flag set to 1 at ROCK1_AD_X value: %d\n", rock1_ad_x);
    }
    // 检查是否回到2000~2100范围，松手居中检测
    else if (rock1_ad_x >= 2000 && rock1_ad_x <= 2100 && flag == Up)//上一次是向上拨动，这一次松手才有效
    {
        newFlag = None;//松手标志
        printf("Flag reset to 0 at ROCK1_AD_X value: %d\n", rock1_ad_x);
    }
    if (newFlag != prevFlag) //拨动了
    {
        if (newFlag == 0) 
        {
            // 当 flag 从1变为0时，输出一个标志位
            printf("Transition detected: Flag changed from 1 to 0.\n");
        }
        // 更新 flag 和 prevFlag
        flag = newFlag; //到这里，拨动了就是返回1，没拨动也就是松手返回0，程序检测flag就可以
        prevFlag = newFlag;
    }
}
void checkRock1AdX_dec(int rock1_ad_x)//检测X轴减，摇杆向下拨动为减
{
 uint8_t newFlag = flag;         //默认flag是0也就是松手
    static uint8_t prevFlag;        //默认prevFlag是0也就是松手， 用于保存上次的 flag 状态
    // 检查是否达到1800左右，向下拨动检测
    if (rock1_ad_x >= 0 && rock1_ad_x <= 1800 && flag == None) //上一次是松手，这一次拨动才有效
    {
        newFlag = down;//向下拨动标志
        printf("Flag set to 1 at ROCK1_AD_X value: %d\n", rock1_ad_x);
    }
    // 检查是否回到2000~2100范围，松手居中检测
    else if (rock1_ad_x >= 2000 && rock1_ad_x <= 2100 && flag == down)//上一次是向下拨动，这一次松手才有效
    {
        newFlag = None;//松手标志
        printf("Flag reset to 0 at ROCK1_AD_X value: %d\n", rock1_ad_x);
    }

    if (newFlag != prevFlag) //拨动了
    {
        if (newFlag == 0) 
        {
            // 当 flag 从1变为0时，输出一个标志位
            printf("Transition detected: Flag changed from 1 to 0.\n");
        }
        // 更新 flag 和 prevFlag
        flag = newFlag;   //到这里，拨动了就是返回2，没拨动也就是松手返回0，程序检测flag就可以
        prevFlag = newFlag;
    }
}
void checkRock1AdY_add(int rock1_ad_y)//检测X轴增，摇杆向上拨动为增
{
    uint8_t newFlag = flag;         //默认flag是0也就是松手
    static uint8_t prevFlag;         //默认prevFlag是0也就是松手， 用于保存上次的 flag 状态
    // 检查是否达到3600左右，向上拨动检测
    if (rock1_ad_y >= 3590 && rock1_ad_y <= 4500 && flag == None) //上一次是松手，这一次拨动才有效
    {
        newFlag = righ;//向上拨动标志
        printf("Flag set to 1 at ROCK1_AD_X value: %d\n", rock1_ad_y);
    }
    // 检查是否回到2000~2100范围，松手居中检测
    else if (rock1_ad_y >= 2000 && rock1_ad_y <= 2100 && flag == righ)//上一次是向上拨动，这一次松手才有效
    {
        newFlag = None;//松手标志
        printf("Flag reset to 0 at ROCK1_AD_X value: %d\n", rock1_ad_y);
    }
    if (newFlag != prevFlag) //拨动了
    {
        if (newFlag == 0) 
        {
            // 当 flag 从1变为0时，输出一个标志位
            printf("Transition detected: Flag changed from 1 to 0.\n");
        }
        // 更新 flag 和 prevFlag
        flag = newFlag; //到这里，拨动了就是返回1，没拨动也就是松手返回0，程序检测flag就可以
        prevFlag = newFlag;
    }
}
void checkRock1AdY_dec(int rock1_ad_y)//检测X轴减，摇杆向下拨动为减
{
 uint8_t newFlag = flag;         //默认flag是0也就是松手
    static uint8_t prevFlag;        //默认prevFlag是0也就是松手， 用于保存上次的 flag 状态
    // 检查是否达到1800左右，向下拨动检测
    if (rock1_ad_y >= 0 && rock1_ad_y <= 1800 && flag == None) //上一次是松手，这一次拨动才有效
    {
        newFlag = left;//向下拨动标志
        printf("Flag set to 1 at ROCK1_AD_X value: %d\n", rock1_ad_y);
    }
    // 检查是否回到2000~2100范围，松手居中检测
    else if (rock1_ad_y >= 2000 && rock1_ad_y <= 2100 && flag == left)//上一次是向下拨动，这一次松手才有效
    {
        newFlag = None;//松手标志
        printf("Flag reset to 0 at ROCK1_AD_X value: %d\n", rock1_ad_y);
    }

    if (newFlag != prevFlag) //拨动了
    {
        if (newFlag == 0) 
        {
            // 当 flag 从1变为0时，输出一个标志位
            printf("Transition detected: Flag changed from 1 to 0.\n");
        }
        // 更新 flag 和 prevFlag
        flag = newFlag;   //到这里，拨动了就是返回2，没拨动也就是松手返回0，程序检测flag就可以
        prevFlag = newFlag;
    }
}

//******************************************************ui部分******************************************************//
//运动逼近函数，到达目标值返回0，否则返回1
int8_t ui_run(int8_t *a, int8_t *a_trg, int8_t step, int8_t slow_cnt)
{
    uint8_t temp;
    temp = abs(*a_trg - *a) > slow_cnt ? step : 1;//目标值和当前值的选择步进
    if (*a < *a_trg) 
    {
        *a += temp;
    }
    else if (*a > *a_trg) 
    {
        *a -= temp;
    }
    else    
    {
        return 0;
    }
    return 1;
}
int8_t ui_run_pic(int32_t *a, int32_t *a_trg, int8_t step, int8_t slow_cnt)
{
    uint8_t temp;
    temp = abs(*a_trg - *a) > slow_cnt ? step : 1;//目标值和当前值的选择步进
    if (*a < *a_trg) 
    {
        *a += temp;
    }
    else if (*a > *a_trg) 
    {
        *a -= temp;
    }
    else    
    {
        return 0;
    }
    return 1;
}
void menu_ui_show(short offset_y)       //菜单ui显示
{
    u8g2_DrawXBMP(&u8g2, menu_x - 75, offset_y + 10, 36, 36, cat_logo);
    u8g2_DrawUTF8(&u8g2, menu_x - 75, offset_y + 60, "点赞");//menu的x轴是menu_x是0，此时menu的x轴和第一个文本的x轴重复了
    u8g2_DrawXBMP(&u8g2, menu_x - 15, offset_y + 10, 36, 36, picture_logo);
    u8g2_DrawUTF8(&u8g2, menu_x - 15, offset_y + 60, "投币");//menu的x轴是menu_x是0，此时menu的x轴和第一个文本的x轴重复了
    u8g2_DrawXBMP(&u8g2, menu_x + 45, offset_y + 10, 36, 36, setting_logo);
    u8g2_DrawUTF8(&u8g2, menu_x + 45, offset_y + 60, "收藏");//menu的x轴是menu_x是0，此时menu的x轴和第一个文本的x轴重复了
}
uint8_t backtoseeting;
void setting_ui_show(int8_t offset_y, int8_t offset_Indic_y, int8_t offset_Indic_vertical_x)   //设置UI显示
{
    int list_len = sizeof(list) / sizeof(SETTING_LIST);//list_len=6

    for (int i = 0 ; i < list_len; i++)//0到5 list[0].str~list[5].str
    {
        //drawstr的xy坐标是左下角，所以y轴要偏移，这里默认的y轴offset_y形参传入的是seeting_y也就是18，也就是这个字符串的高度是18，这里默认+18是因为第一个字符的i是0
        
        u8g2_DrawStr(&u8g2, setting_x + 6, offset_y + i * 20 + 18, list[i].str); // 第一段输出位置
    }
    
    u8g2_SetDrawColor(&u8g2,2);//异或显示，让框框反色，注意这里是要画RBox，Fram是不行的
    u8g2_DrawRBox(&u8g2, setting_x, offset_y + frame_y, frame_len, 22, 2);//初始化框框的y轴是offset_y是18，此时框框的y轴和第一个文本的y轴重复了
    u8g2_SetDrawColor(&u8g2,1);//异或显示，让框框反色，注意这里是要画RBox，Fram是不行的

    if(backtoseeting == 1)//回到设置标志位，目的是让框框回弹到list[0]
    {
        backtoseeting = 0;//清除该标志位
        ui_run(&frame_y, &frame_y_trg, 10, 10);       //框框y轴ui运动函数 
    }
    else
    {
        ui_run(&frame_y, &frame_y_trg, 5, 4);       //框框y轴ui运动函数 
    }
    ui_run(&frame_len, &frame_len_trg, 10, 5);  //框框宽度ui运动函数

    u8g2_DrawVLine(&u8g2, offset_Indic_vertical_x, Indic_vertical_y, 64);//垂线 左下角xy
    for(int i = 0; i < list_len; i++)//画list条水平线
    {
        //y 8~120
        //x 118                  y = 8                      len = （120-x)*2 = 4                 
        //box_x= x - 2=116       box_y = y+2 = 10          boxlen = (120 - box_x)* 2 = 8   box_ = h/2 = 4   
        u8g2_DrawHLine(&u8g2, Indic_Horizental_x, Indic_Horizental_y + i*8, 4);//左下角xy
    }
    u8g2_DrawRBox(&u8g2, Indic_x, offset_Indic_y + Indic_y , 8, 8, 4);//左上角xy   // offset_Indic_y = 4  Indic_y 
    ui_run(&Indic_y, &Indic_y_trg, 5, 4);                      //框框宽度ui运动函数   Indic_y_trg 没运动一次偏移一个指示器
    ui_run(&Indic_x, &Indic_x_trg, 5, 4);  
    ui_run(&Indic_Horizental_y, &Indic_Horizental_y_trg, 5, 4);                      //框框宽度ui运动函数   Indic_y_trg 没运动一次偏移一个指示器
    ui_run(&Indic_vertical_y, &Indic_vertical_y_trg, 5, 4);  

}


void pic_ui_show(short offset_y)        //图片UI显示
{
    u8g2_DrawXBMP(&u8g2, 40, offset_y + 20, 36, 36, dianzan);
    u8g2_DrawXBMP(&u8g2, 40, offset_y + 20 + 64, 36, 36, toubi);
    u8g2_DrawXBMP(&u8g2, 40, offset_y + 20 + 128, 36, 36, shoucang);

}
extern uint8_t keynum;
uint8_t MenuprocFirstEnterFlag = 0;
void menu_proc(void)            //菜单处理函数
{

    /* 保存第一次进入时候不会触发按键动作 t=0这个赋值动作只有退出菜单时候才会调用，如rock1keylongclik*/
    if(MenuprocFirstEnterFlag == 0) 
    {
        keynum = Key_None;
        MenuprocFirstEnterFlag = 1;
        printf("key_none!\r\n");
    }

    ROCK1_AD_Y = AD_GetValue(ROCK1_Y_ADC_Channel);//ADC_Y
    checkRock1AdY_add(ROCK1_AD_Y);//检测左摇杆
    checkRock1AdY_dec(ROCK1_AD_Y);//检测左摇杆

    u8g2_SetFont(&u8g2,u8g2_myfont_chinese);                         //设置字体
    int list_len = 3;                  //菜单的元素个数
    if(flag == left)
    {
        printf("Rock1_left\r\n");
        flag = None;
        ui_select++;//增加
        if (ui_select >= list_len)//ui_select=2 --->ui_selec=1      限幅最大值为1
        {
            ui_select = list_len - 1;
        }

    }
    else if(flag == righ)
    {
        printf("Rock1_righ\r\n");   
        flag = None;
        ui_select--;//减少
        if (ui_select < 0)       //ui_select=-1 --->ui_selec=0      限幅最小值为0
        {
            ui_select = 0;
        }
    }
           else if (keynum == ROCK1_KEY_SingleClick)          
           {
                printf("ROCK1_KEY_SingleClick\r\n");
            // ui_state = E_STATE_MENU;    //状态机设置为菜单运行状态
               ui_select++;//增加
               if (ui_select >= list_len)//ui_select=2 --->ui_selec=1      限幅最大值为1
               {
                   ui_select = list_len - 1;
               }
           }
           else if(keynum == ROCK2_KEY_SingleClick)                          //id = 0 是keyup
           {
               printf("ROCK2_KEY_SingleClick\r\n");
            //   ui_state = E_STATE_MENU;    //状态机设置为菜单运行状态
               ui_select--;//减少
               if (ui_select < 0)       //ui_select=-1 --->ui_selec=0      限幅最小值为0
               {
                   ui_select = 0;
               }
           }
           /*
           此处通过调试menu_x_trg最大值只能是120，因此进行反推，我有3个菜单，
           要使菜单右移，此处我设置初始菜单是最后一个，因此最大右移此处为3-1也就是list-1,
           最大是120，我们又限制右移次数为2，因此单次步距就是120/2=60了，再通过极限值120推算菜单的x轴位置
           我们设置为中心位置45，
            ····..  图1x          图2x          图3x   
           因此120- 75 = 45， 60 - 15 = 45 , 0 + 45 = 45
            */
           menu_x_trg =   ui_select * 60;
            if (keynum == ROCK2_KEY_LongCick)           //id = 1 是key0
            {
                printf("ROCK2_KEY_LongCick\r\n");
                if (ui_select == E_SELECT_SETTING)//如果目标ui为设置 ui_select=E_SELECT_SETTING=0
                {
                    ui_index = E_SETTING;           //把当前的UI设为设置页面，一会会在他的处理函数中运算

                    ui_state = E_STATE_RUN_SETTING; //UI状态设置为 跑向设置

                    menu_y_trg = -64;               //设置菜单界面的目标

                    setting_y_trg = 0;              //设置设置界面的目标
                    setting_y = 64;                 //设置设置界面的初始值
                    
                    ui_select = 0;
                    Indic_y_trg  = 0;
                    Indic_vertical_y_trg = 0;
                    Indic_Horizental_y_trg = 4;

                    MenuprocFirstEnterFlag = 0;
                }
                else if (ui_select == E_SELECT_PIC)//如果目标ui为设置 ui_select=E_SELECT_SETTING=1
                {
                    
                    ui_index = E_PIC;               //把当前的UI设为图片页面，一会会在他的处理函数中运算

                    ui_state = E_STATE_RUN_PIC;     //UI状态设置为 跑向图片

                    menu_y_trg = -64;               //设置菜单界面的目标

                    pic_y_trg = 0;                  //设置图片界面的目标
                    pic_y = 64;                     //设置图片界面的初始值

                    MenuprocFirstEnterFlag = 0;
                }
                else if (ui_select == E_SELECT_CAT)//如果目标ui为设置 ui_select=E_SELECT_SETTING=1
                {
                    ui_index = E_CAT;               //把当前的UI设为图片页面，一会会在他的处理函数中运算

                    ui_state = E_STATE_RUN_CAT;     //UI状态设置为 跑向图片

                    menu_y_trg = -64;               //设置菜单界面的目标
                    cat_run_y_trg = 0;                  //设置图片界面的目标
                    cat_run_x_trg = 0;                  //设置图片界面的目标
                    cat_run_x = -128;                     //设置图片界面的初始值

                    MenuprocFirstEnterFlag = 0;
                }
            }
    switch (ui_state)                                //菜单界面UI状态机
    {
        case E_STATE_RUN_SETTING:                   //运行跑向设置，key0按键长按时，E_STATE_RUN_SETTING=3
        {
            static uint8_t flag = 0;                //定义一个缓存变量用以判断是否到位
            if (ui_run(&setting_y, &setting_y_trg, 10, 4) == 0)//设置的y轴运动
            {
                flag |= 0xf0;                       //如果设置到位了 或上0xF0
                printf("%x\r\n", flag);
            }
            if (ui_run(&menu_y, &menu_y_trg, 10, 4) == 0)//菜单的y轴运动
            {
                flag |= 0x0f;                       //如果菜单到位了 或上0x0F
                printf("%x\r\n", flag);
            }
            if (flag == 0xff)                       // 0 | 0xf0 | 0x0f = 0xff
            {
                //如果到位了 状态置为到位
                flag = 0;                           //清除标志
                ui_state = E_STATE_ARRIVE;          //菜单状态为到位，E_STATE_ARRIVE=5
            }
            setting_ui_show(setting_y,Indic_y, Indic_vertical_x);             //显示设置ui
            break;
        }
        case E_STATE_RUN_PIC:                       //运行跑向设置，key0按键长按时，E_STATE_RUN_PIC=4
        {
            static uint8_t flag = 0;                //定义一个缓存变量用以判断是否到位
            if (ui_run_pic(&pic_y, &pic_y_trg, 10, 4) == 0)//图片的y轴运动
            {
                flag |= 0xf0;                      //如果图片到位了 或上0xF0     
            }
            if (ui_run(&menu_y, &menu_y_trg, 10, 4) == 0)//菜单的y轴运动
            {
                flag |= 0x0f;                      //如果菜单到位了 或上0x0F
            }
            if (flag == 0xff)                       // 0 | 0xf0 | 0x0f = 0xff
            {
                //如果到位了 状态置为到位
                flag = 0;                           //清除标志
                ui_state = E_STATE_ARRIVE;          //菜单状态为到位，E_STATE_ARRIVE=5
            }
            pic_ui_show(pic_y);                     //显示图片ui
            break;
            
        }
        case E_STATE_RUN_CAT:                       //运行跑向设置，key0按键长按时，E_STATE_RUN_PIC=4
        {
            static uint8_t flag = 0;                //定义一个缓存变量用以判断是否到位
            // if (ui_run(&cat_run_y, &cat_run_y_trg, 10, 4) == 0)//图片的y轴运动
            // {
            //     flag |= 0xf0;                      //如果图片到位了 或上0xF0     
            // }
            if (ui_run(&menu_y, &menu_y_trg, 10, 4) == 0)//菜单的y轴运动
            {
                flag |= 0x0f;                      //如果菜单到位了 或上0x0F
            }
            // if (flag == 0xff)                       // 0 | 0xf0 | 0x0f = 0xff
            // {
            //     //如果到位了 状态置为到位
            //     flag = 0;                           //清除标志
            //     ui_state = E_STATE_ARRIVE;          //菜单状态为到位，E_STATE_ARRIVE=5
            // }
            // cat_flag = CAT_ENTER;
            // TIM_Cmd(TIM3,ENABLE);
            // break;
            
        }
    }  
    ui_run(&menu_x, &menu_x_trg, 15, 2);     //只运算x轴
    menu_ui_show(menu_y);
}

uint8_t PicprocFirstEnterFlag = 0;
void pic_proc(void)                         //图片处理函数
{
    /* 保存第一次进入时候不会触发按键动作 t=0这个赋值动作只有退出菜单时候才会调用，如rock1keylongclik*/
    if(PicprocFirstEnterFlag == 0) 
    {
        keynum = Key_None;
        PicprocFirstEnterFlag = 1;
        printf("key_none!\r\n");
    }
    static int8_t num;
    ROCK1_AD_X = AD_GetValue(ROCK1_X_ADC_Channel);//ADC_X
    checkRock1AdX_add(ROCK1_AD_X);//检测左摇杆
    checkRock1AdX_dec(ROCK1_AD_X);//检测左摇杆
    if(flag == down)//左摇杆上移动
    {
        printf("Rock1_down!\r\n");
        flag = None;
        num ++;
        if(num == 3) num = 0;
        pic_y_trg = num * (-64);
    }
    else if(flag == Up)
    {  
        printf("Rock1_up!\r\n");
        flag = None;
        num --;
        if(num == -1) num = 2;
        pic_y_trg = num * (-64);
    }

    if (keynum == ROCK2_KEY_LongCick)                       //id = 1是key0
    {

    }
    else  if(keynum == ROCK1_KEY_LongCick)                                  //id =0 是keyup
    {
        printf("ROCK1_KEY_LongCick!\r\n");
        ui_index = E_MENU;                  //把当前的UI设为菜单页面，一会会在他的处理函数中运算    
        ui_state = E_STATE_RUN_PIC;         //UI状态设置为 跑向图片【其实是跑向菜单】

        menu_y_trg = 0;                     //设置菜单界面的目标   

        if(num == 0)pic_y_trg = 64;                           //设置图片界面的目标
        else if(num == 1)pic_y_trg = 128;                     //设置图片界面的目标
        else if(num == 2)pic_y_trg = 192;                     //设置图片界面的目标

        PicprocFirstEnterFlag = 0;
    }
    else if(keynum == ROCK1_KEY_SingleClick)
    {
        printf("ROCK1_KEY_SingleClick!\r\n");
        num ++;
        if(num == 3) num = 0;
        pic_y_trg = num * (-64);
    }
    else if(keynum == ROCK2_KEY_SingleClick)
    {
        printf("ROCK2_KEY_SingleClick!\r\n");
        num --;
        if(num == -1) num = 2;
        pic_y_trg = num * (-64);
    }

    switch (ui_state)
    {
        case E_STATE_RUN_PIC:                               //运行跑向菜单，keyup按键长按时，E_STATE_RUN_PIC=4
        {
            static uint8_t pic_flag = 0;                    //定义一个缓存变量用以判断是否到位
            if (ui_run_pic(&pic_y, &pic_y_trg, 10, 4) == 0)     //图片的y轴运动
            {
                pic_flag |= 0xf0;                           //如果图片到位了 或上0xF0     
            }
            if (ui_run(&menu_y, &menu_y_trg, 10, 4) == 0)   //菜单的y轴运动
            {
                pic_flag |= 0x0f;                            //如果菜单到位了 或上0x0F                        
            }
            if (pic_flag == 0xff)                            // 0 | 0xf0 | 0x0f = 0xff
            {
                 //如果到位了 状态置为到位
                pic_flag = 0;                               //清除标志
                ui_state = E_STATE_ARRIVE;                  //菜单状态为到位，E_STATE_ARRIVE=5
            }
            menu_ui_show(menu_y);            //显示菜单UI
            break;
        }
    }

    ui_run_pic(&pic_y, &pic_y_trg, 8, 2);     //只运算x轴
    pic_ui_show(pic_y);              //显示菜单UI
}

/*
ui_select的情况
            ui_select
    list_1      0
    list_2      1
    list_3      2
    list_4      3 
    list_5      4
    list_6      5
*/
uint8_t SettingFirstEnterFlag = 0;
void setting_proc(void)                              //设置处理函数
{
    /* 保存第一次进入时候不会触发按键动作 t=0这个赋值动作只有退出菜单时候才会调用，如rock1keylongclik*/
    if(SettingFirstEnterFlag == 0) 
    {
        keynum = Key_None;
        SettingFirstEnterFlag = 1;
        printf("key_none!\r\n");
    }
    u8g2_SetFont(&u8g2,u8g2_font_wqy15_t_chinese1);                         //设置字体
    ROCK1_AD_X = AD_GetValue(ROCK1_X_ADC_Channel);//ADC_X
    ROCK1_AD_Y = AD_GetValue(ROCK1_Y_ADC_Channel);//ADC_Y
    ROCK2_AD_X = AD_GetValue(ROCK2_X_ADC_Channel);//ADC_X
    ROCK2_AD_Y = AD_GetValue(ROCK2_Y_ADC_Channel);//ADC_Y
    checkRock1AdX_add(ROCK1_AD_X);//检测左摇杆
    checkRock1AdX_dec(ROCK1_AD_X);//检测左摇杆

    int list_len = sizeof(list) / sizeof(SETTING_LIST);     //计算出列表的菜单个数

    if(flag == Up)//左摇杆上移动
    {
        printf("Rock1_Up!\r\n");
        flag = 0;

        ui_select --;                               //选项-1
        if (ui_select < 0)                          //list_len=6  意味着框框要偏移6-1次
        {
            ui_select = 0;                          //极限值为0
        }


        if (ui_select < list_len / 2)               //光标小于3时，开始进行菜单偏移list_len/2=6/2=3             
        {
            y_select --;                            //递减2 1 0
        }
        if (y_select <= 0)                          //限幅菜单偏移3次
        {
            y_select = 0;
        }
        
    }
    else if(flag == down)//左摇杆下移动
    {
        printf("Rock1_down!\r\n");
        flag = 0;
        ui_select ++;                                //选项+1
        if (ui_select >= list_len)                  //list_len=6  意味着框框要偏移6-1次，因为框框默认框选第一个菜单，因此限幅在list_len-1
        {
            ui_select = list_len - 1;               //极限值为5
        }

        if (ui_select >= list_len / 2)             //光标大于3时，开始进行菜单偏移list_len/2=6/2=3
        {
            y_select ++;                           //递增1 2 3
        }
        if (y_select >= list_len - 3)              //限幅菜单偏移3次
        {
            y_select = list_len - 3;                
        }
    }

    if (keynum == ROCK1_KEY_SingleClick) //左摇杆单击
    {
        printf("ROCK1_KEY_SingleClick!\r\n");
        ui_select ++;                                //选项+1
        if (ui_select >= list_len)                  //list_len=6  意味着框框要偏移6-1次，因为框框默认框选第一个菜单，因此限幅在list_len-1
        {
            ui_select = list_len - 1;               //极限值为5
        }


        if (ui_select >= list_len / 2)             //光标大于3时，开始进行菜单偏移list_len/2=6/2=3
        {
            y_select ++;                           //递增1 2 3
        }
        if (y_select >= list_len - 3)              //限幅菜单偏移3次
        {
            y_select = list_len - 3;                
        }
    }
    else if(keynum == ROCK1_KEY_DobleClick) //左摇杆双击
    {
        printf("ROCK1_KEY_DobleClick!\r\n");
        ui_select --;                               //选项-1
        if (ui_select < 0)                          //list_len=6  意味着框框要偏移6-1次
        {
            ui_select = 0;                          //极限值为0
        }

        if (ui_select < list_len / 2)               //光标小于3时，开始进行菜单偏移list_len/2=6/2=3             
        {
            y_select --;                            //递减2 1 0
        }
        if (y_select <= 0)                          //限幅菜单偏移3次
        {
            y_select = 0;
        }

    }
    setting_y_trg = -y_select * 20;                   //设置设置列表的文本偏移  光标向下和向上移动时，y_select都为正数，为了让菜单列表向上偏移，因此这里加了负号

    frame_y_trg = ui_select * 20;                    //设置框框的y轴偏移
    Indic_y_trg = ui_select * 4;                    //设置框框的y轴偏移
    frame_len_trg = list[ui_select].len * 13;        //设置框框的宽度偏移ui_select 0~5 对应list数组元素
    if (keynum == ROCK2_KEY_LongCick) //右摇杆双击
    {
        printf("ROCK2_KEY_LongCick!\r\n");
        if(ui_select == 0)                          //选中了LED
        {
            ui_index = E_LED;                       //把当前的UI设为LED页面，一会会在他的处理函数中运算   

            ui_state = E_STATE_RUN_LED;             //UI状态设置为 跑向LED

            // menu_y_trg = 0;                     //设置LED界面的目标,暂未使用

            setting_x_trg = -100;                   //设置设置界面的目标，向左偏移  
            Indic_vertical_x_trg = -64;
            Indic_Horizental_x_trg = -64;
            Indic_x_trg = -64;
            ui_select = 0;                         //光标默认设置为0    

            SettingFirstEnterFlag = 0;
        }
        if(ui_select == 1)                          //选中了ADVALUE
        {
            ui_index = E_ADVALUE;                       //把当前的UI设为LED页面，一会会在他的处理函数中运算   

            ui_state = E_STATE_RUN_LED;             //UI状态设置为 跑向LED

            // menu_y_trg = 0;                     //设置LED界面的目标,暂未使用

            setting_x_trg = -100;                   //设置设置界面的目标，向左偏移  
            Indic_vertical_x_trg = -64;
            Indic_Horizental_x_trg = -64;
            Indic_x_trg = -64;
            ui_select = 1;                         //光标默认设置为0    

            SettingFirstEnterFlag = 0;
        }
        if(ui_select == 2)                          //选中了BAT
        {
            ui_index = E_BAT;                       //把当前的UI设为LED页面，一会会在他的处理函数中运算
            ui_state = E_STATE_RUN_LED;             //UI状态设置为 跑向LED

            // menu_y_trg = 0;                     //设置LED界面的目标,暂未使用

            setting_x_trg = -100;                   //设置设置界面的目标，向左偏移  
            Indic_vertical_x_trg = -64;
            Indic_Horizental_x_trg = -64;
            Indic_x_trg = -64; 
            ui_select = 2;                         //光标默认设置为0     

            SettingFirstEnterFlag = 0; 
        }
        if(ui_select == 3)                          //选中了MPU6050
        {
            ui_index = E_MPU6050;                       //把当前的UI设为LED页面，一会会在他的处理函数中运算
            ui_state = E_STATE_RUN_LED;             //UI状态设置为 跑向LED

            // menu_y_trg = 0;                     //设置LED界面的目标,暂未使用

            setting_x_trg = -100;                   //设置设置界面的目标，向左偏移  
            Indic_vertical_x_trg = -64;
            Indic_Horizental_x_trg = -64;
            Indic_x_trg = -64; 
            ui_select = 3;                         //光标默认设置为0     

            SettingFirstEnterFlag = 0; 
        }
        if(ui_select == 5)                          //选中了ABOUT
        {
            ui_index = E_ABOUT;                       //把当前的UI设为LED页面，一会会在他的处理函数中运算
            ui_state = E_STATE_RUN_LED;             //UI状态设置为 跑向LED

            // menu_y_trg = 0;                     //设置LED界面的目标,暂未使用

            setting_x_trg = -100;                   //设置设置界面的目标，向左偏移  
            Indic_vertical_x_trg = -64;
            Indic_Horizental_x_trg = -64;
            Indic_x_trg = -64; 
            ui_select = 5;                         //光标默认设置为0     

            SettingFirstEnterFlag = 0; 
        }
    }
    else if(keynum == ROCK1_KEY_LongCick) //左摇杆长按
    {
        printf("ROCK1_KEY_LongCick!\r\n");
        ui_index = E_MENU;                          //把当前的UI设为菜单页面，一会会在他的处理函数中运算   

        ui_state = E_STATE_RUN_SETTING;             //UI状态设置为 跑向设置【其实是跑向菜单】

        menu_y_trg = 0;                             //设置菜单界面的目标  

        setting_y_trg = 64;                         //设置设置界面的目标  
        Indic_y_trg  = 64;
        Indic_vertical_y_trg  = 64;
        Indic_Horizental_y_trg  = 64;

                
        frame_y =  ui_select * 20 ;                  //框框的宽度设置，默认ui_select是0，也就是一个菜单的len（长度）乘以每个字符的长度（6）才是框框真正的宽度    
        ui_select = 0;                              //光标默认设置为0         

        y_select = 0;                               //页面偏移设置为默认
        frame_y_trg = 0;                            //框框y轴设置为0，显示函数那的函数会默认再次偏移18

        backtoseeting = 1;                          //回到设置标志位

        //框框宽度设置为第一个list,即list[0]
        frame_len_trg = list[ui_select].len * 13;        //设置框框的宽度偏移ui_select 0~5 对应list数组元素

        SettingFirstEnterFlag = 0;
    }

    switch (ui_state)
    {
        case E_STATE_RUN_SETTING:                                   //运行跑向菜单，keyup按键长按时，E_STATE_RUN_SETTING=3             
        {   
            static uint8_t flag = 0;                                //定义一个缓存变量用以判断是否到位
            
            if (ui_run(&setting_y, &setting_y_trg, 10, 4) == 0 )     //设置的y轴运动
            {
                flag |= 0xf0;                                      //如果图片到位了 或上0xF0                                       
            }
            if (ui_run(&Indic_Horizental_y ,&Indic_Horizental_y_trg, 10, 4) == 0 )     //设置的y轴运动
            {
                flag |= 0xf0;                                      //如果图片到位了 或上0xF0                                       
            }
            if (ui_run(&Indic_vertical_y, &Indic_vertical_y_trg, 10, 4) == 0 )     //设置的y轴运动
            {
                flag |= 0xf0;                                      //如果图片到位了 或上0xF0                                       
            }
            if (ui_run(&Indic_y, &Indic_y_trg, 10, 4) == 0 )     //设置的y轴运动
            {
                flag |= 0xf0;                                      //如果图片到位了 或上0xF0                                       
            }
            if (ui_run(&menu_y, &menu_y_trg, 10, 4) == 0)          //菜单的y轴运动
            {
                flag |= 0x0f;                                       //如果菜单到位了 或上0x0F       
            }
            if (flag == 0xff)                                       // 0 | 0xf0 | 0x0f = 0xff
            {
                 //如果到位了 状态置为到位
                flag = 0;                                           //清除标志
                ui_state = E_STATE_ARRIVE;                          //菜单状态为到位，E_STATE_ARRIVE=5
            }
            menu_ui_show(menu_y);                               //显示菜单UI
            break;
        }
        case E_STATE_RUN_LED:                                   //运行跑向菜单，keyup按键长按时，E_STATE_RUN_SETTING=3             
        {   
            static uint8_t flag = 0;                                //定义一个缓存变量用以判断是否到位
            if (ui_run(&setting_x, &setting_x_trg, 10, 4) == 0)     //设置的y轴运动
            {
                flag |= 0xf0;                                      //如果图片到位了 或上0xF0                                       
            }
            if (ui_run(&Indic_vertical_x, &Indic_vertical_x_trg, 10, 4))       //菜单的y轴运动
            {
                flag |= 0x0f;                                       //如果菜单到位了 或上0x0F       
            }     
            if (ui_run(&Indic_Horizental_x, &Indic_Horizental_x_trg, 10, 4) == 0)     //设置的y轴运动
            {
                flag |= 0xf0;                                      //如果图片到位了 或上0xF0                                       
            }   
            if (ui_run(&Indic_x, &Indic_x_trg, 10, 4) == 0)     //设置的y轴运动
            {
                flag |= 0xf0;                                      //如果图片到位了 或上0xF0                                       
            }

            if (flag == 0xf0)                                       // 0 | 0xf0 | 0x0f = 0xff
            {
                 //如果到位了 状态置为到位
                flag = 0;                                           //清除标志
                ui_state = E_STATE_ARRIVE;                          //菜单状态为到位，E_STATE_ARRIVE=5
            }
            break;
        }
    }
    ui_run(&setting_y, &setting_y_trg, 5, 4);           //设置界面的ui动作函数
    setting_ui_show(setting_y,Indic_y, Indic_vertical_x);                         //显示设置UI
}
enum
{
    led0flagon = 0,
    led0flagoff,
    led1flagon,
    led1flagoff,
    led2flagon,
    led2flagoff,
    led3flagon,
    led3flagoff,
    led4flagon,
    led4flagoff,
    led5flagon,
    led5flagoff,
    led6flagon,
    led6flagoff,
    led7flagon,
    led7flagoff,
};
uint8_t led0flag = led0flagoff;
uint8_t led1flag = led1flagoff;
uint8_t led2flag = led2flagoff;
uint8_t led3flag = led3flagoff;
uint8_t led4flag = led4flagoff;
uint8_t led5flag = led5flagoff;
uint8_t led6flag = led6flagoff;
uint8_t led7flag = led7flagoff;

uint8_t LedprocFirstEnterFlag = 0;
void led_proc(void)                         //LED处理函数
{
    /* 保存第一次进入时候不会触发按键动作 t=0这个赋值动作只有退出菜单时候才会调用，如rock1keylongclik*/
    if(LedprocFirstEnterFlag == 0) 
    {
        keynum = Key_None;
        LedprocFirstEnterFlag = 1;
        printf("key_none!\r\n");
    }
    

    ROCK1_AD_X = AD_GetValue(ROCK1_X_ADC_Channel);//ADC_X
    ROCK1_AD_Y = AD_GetValue(ROCK1_Y_ADC_Channel);//ADC_Y
    ROCK2_AD_X = AD_GetValue(ROCK2_X_ADC_Channel);//ADC_X
    ROCK2_AD_Y = AD_GetValue(ROCK2_Y_ADC_Channel);//ADC_Y
    checkRock1AdX_add(ROCK1_AD_X);//检测摇杆
    checkRock1AdX_dec(ROCK1_AD_X);

    u8g2_SetFont(&u8g2,u8g2_font_t0_15_mf);                         //设置字体

    u8g2_DrawStr(&u8g2, 0, 15, "LED0:");
    u8g2_SetFont(&u8g2,u8g2_font_unifont_t_symbols);
    u8g2_DrawGlyph(&u8g2,120,  15, 0x25c5);/* dec 9731/hex 2603 Snowman */
    u8g2_DrawStr(&u8g2, 0, 32, "LED1:");
    u8g2_SetFont(&u8g2,u8g2_font_unifont_t_symbols);
    u8g2_DrawGlyph(&u8g2,120,  32, 0x25c5);/* dec 9731/hex 2603 Snowman */
    u8g2_DrawStr(&u8g2, 0, 47, "LED2:");
    u8g2_SetFont(&u8g2,u8g2_font_unifont_t_symbols);
    u8g2_DrawGlyph(&u8g2,120,  47, 0x25c5);/* dec 9731/hex 2603 Snowman */
    u8g2_DrawStr(&u8g2, 0, 62, "LED3:");
    u8g2_SetFont(&u8g2,u8g2_font_unifont_t_symbols);
    u8g2_DrawGlyph(&u8g2,120,  62, 0x25c5);/* dec 9731/hex 2603 Snowman */
    if(led0flag == led0flagon)    
    {
        u8g2_DrawStr(&u8g2, 42, 18, "   ");
        // u8g2_DrawStr(&u8g2, 38, 12, "on");
        u8g2_SetFont(&u8g2,u8g2_font_unifont_t_symbols);
        u8g2_DrawGlyph(&u8g2,42,  15, 0x2713);/* dec 9731/hex 2603 Snowman */
    }
    else if(led0flag == led0flagoff)                
    {
        u8g2_DrawStr(&u8g2, 42, 16, "   ");
        // u8g2_DrawStr(&u8g2, 38, 12, "off");
        u8g2_SetFont(&u8g2,u8g2_font_unifont_t_symbols);
        u8g2_DrawGlyph(&u8g2,42, 16, 0x2715);/* dec 9731/hex 2603 Snowman */
    }
    if(led1flag == led1flagon)    
    {
        u8g2_DrawStr(&u8g2, 42, 30, "   ");
        // u8g2_DrawStr(&u8g2, 38, 12, "on");
        u8g2_SetFont(&u8g2,u8g2_font_unifont_t_symbols);
        u8g2_DrawGlyph(&u8g2,42,  30, 0x2713);/* dec 9731/hex 2603 Snowman */
    }
    else if(led1flag == led1flagoff)                
    {
        u8g2_DrawStr(&u8g2, 42, 34, "   ");
        // u8g2_DrawStr(&u8g2, 38, 12, "off");
        u8g2_SetFont(&u8g2,u8g2_font_unifont_t_symbols);
        u8g2_DrawGlyph(&u8g2,42, 34, 0x2715);/* dec 9731/hex 2603 Snowman */
    }
    if(led2flag == led2flagon)    
    {
        u8g2_DrawStr(&u8g2, 42, 45, "   ");
        // u8g2_DrawStr(&u8g2, 38, 12, "on");
        u8g2_SetFont(&u8g2,u8g2_font_unifont_t_symbols);
        u8g2_DrawGlyph(&u8g2,42,  45, 0x2713);/* dec 9731/hex 2603 Snowman */
    }
    else if(led2flag == led2flagoff)                
    {
        u8g2_DrawStr(&u8g2, 42, 48, "   ");
        // u8g2_DrawStr(&u8g2, 38, 12, "off");
        u8g2_SetFont(&u8g2,u8g2_font_unifont_t_symbols);
        u8g2_DrawGlyph(&u8g2,42, 48, 0x2715);/* dec 9731/hex 2603 Snowman */
    }
    if(led3flag == led3flagon)    
    {
        u8g2_DrawStr(&u8g2, 42, 60, "   ");
        // u8g2_DrawStr(&u8g2, 38, 12, "on");
        u8g2_SetFont(&u8g2,u8g2_font_unifont_t_symbols);
        u8g2_DrawGlyph(&u8g2,42,  60, 0x2713);/* dec 9731/hex 2603 Snowman */
    }
    else if(led3flag == led3flagoff)                
    {
        u8g2_DrawStr(&u8g2, 42, 63, "   ");
        // u8g2_DrawStr(&u8g2, 38, 12, "off");
        u8g2_SetFont(&u8g2,u8g2_font_unifont_t_symbols);
        u8g2_DrawGlyph(&u8g2,42, 63, 0x2715);/* dec 9731/hex 2603 Snowman */
    }
    if (keynum == ROCK1_KEY_SingleClick)                                   
    {

    }
    else  if (keynum == ROCK1_KEY_DobleClick)                                    
    {

    }
    if (keynum == ROCK2_KEY_LongCick)                      
    {

    }
    else if(keynum == ROCK1_KEY_LongCick)                                
    {
        printf("ROCK1_KEY_LongCick!\r\n");
        u8g2_SetFont(&u8g2,u8g2_font_wqy15_t_chinese1);                         //设置字体

        ui_index = E_SETTING;               //把当前的UI设为设置页面，一会会在他的处理函数中运算    
        ui_state = E_STATE_RUN_LED;         //UI状态设置为 跑向LED【其实是跑向设置】

        // menu_y_trg = 0;                  //设置LED界面的目标   
        Indic_vertical_x_trg = 120;
        Indic_Horizental_x_trg = 118;
        Indic_x_trg = 116;
        setting_x_trg = 0;                  //设置设置界面的目标

        LedprocFirstEnterFlag = 0;
    }
    if (keynum == ROCK2_KEY_SingleClick)                
    {
        printf("ROCK2_KEY_SingleClick!\r\n");
        LEDx_SetMode(LED0, Mode_ON);  
        LEDx_SetMode(LED1, Mode_ON);   
        LEDx_SetMode(LED2, Mode_ON);   
        LEDx_SetMode(LED3, Mode_ON);  
        led0flag = led0flagon;
        led1flag = led1flagon;
        led2flag = led2flagon;
        led3flag = led3flagon;
    }
    else if(keynum == ROCK2_KEY_DobleClick)
    {
        printf("ROCK2_KEY_DobleClick!\r\n");
        LEDx_SetMode(LED0, Mode_OFF);  
        LEDx_SetMode(LED1, Mode_OFF);   
        LEDx_SetMode(LED2, Mode_OFF);   
        LEDx_SetMode(LED3, Mode_OFF);  
        led0flag = led0flagoff;
        led1flag = led1flagoff;
        led2flag = led2flagoff;
        led3flag = led3flagoff;

    }

    switch (ui_state)
    {
        case E_STATE_RUN_LED:                               //运行跑向菜单，keyup按键长按时，E_STATE_RUN_PIC=4
        {
            static uint8_t pic_flag = 0;                    //定义一个缓存变量用以判断是否到位

            if (ui_run(&setting_x, &setting_x_trg, 10, 4) == 0)     //图片的y轴运动
            {
                pic_flag |= 0xf0;                           //如果图片到位了 或上0xF0     
            }
            if (ui_run(&Indic_vertical_x, &Indic_vertical_x_trg, 10 , 4))   //菜单的y轴运动
            {
                pic_flag |= 0x0f;                            //如果菜单到位了 或上0x0F                        
            }
            if (ui_run(&Indic_Horizental_x, &Indic_Horizental_x_trg, 10, 4) == 0)     //设置的y轴运动
            {
                pic_flag |= 0xf0;                                      //如果图片到位了 或上0xF0                                       
            } 
            if (ui_run(&Indic_x, &Indic_x_trg, 10, 4) == 0)     //设置的y轴运动
            {
                pic_flag |= 0xf0;                                      //如果图片到位了 或上0xF0                                       
            }
            if (pic_flag == 0xf0)                            // 0 | 0xf0 | 0x0f = 0xff
            {
                 //如果到位了 状态置为到位
                pic_flag = 0;                               //清除标志
                ui_state = E_STATE_ARRIVE;                  //菜单状态为到位，E_STATE_ARRIVE=5
            }

            break;
        }
    }
    setting_ui_show(setting_y, Indic_y, Indic_vertical_x);                         //显示设置UI

}
uint8_t AdvalueprocFirstEnterFlag = 0;
void advalue_proc(void)                         //LED处理函数
{
        /* 保存第一次进入时候不会触发按键动作 t=0这个赋值动作只有退出菜单时候才会调用，如rock1keylongclik*/
        if(AdvalueprocFirstEnterFlag == 0) 
        {
            keynum = Key_None;
            AdvalueprocFirstEnterFlag = 1;
            printf("key_none!\r\n");
        }
        u8g2_SetFont(&u8g2,u8g2_font_t0_11_mf);                         //设置字体

        if(led2flag == led2flagoff)    u8g2_DrawStr(&u8g2, 80, 12, "off");               
        else if(led2flag == led2flagon)                u8g2_DrawStr(&u8g2, 80, 24, "on");
        u8g2_DrawStr(&u8g2, 0, 12, "AD1_X:");
        u8g2_DrawStr(&u8g2, 0, 24, "AD1_y:");

        u8g2_DrawStr(&u8g2, 0, 36, "AD2_X:");
        u8g2_DrawStr(&u8g2, 0, 48, "AD2_y:");
        ROCK1_AD_X = AD_GetValue(ROCK1_X_ADC_Channel);//ADC_X
		ROCK1_AD_Y = AD_GetValue(ROCK1_Y_ADC_Channel);//ADC_Y

		ROCK1_AD_X_HighByte = (ROCK1_AD_X >> 8) & 0xFF;//取出ADC_X的高8位
		ROCK1_AD_X_LowByte  =  ROCK1_AD_X & 0xFF;	   //取出ADC_X的低8位
		ROCK1_AD_Y_HighByte = (ROCK1_AD_Y >> 8) & 0xFF;//取出ADC_Y的高8位
		ROCK1_AD_Y_LowByte  =  ROCK1_AD_Y & 0xFF;	   //取出ADC_Y的低8位

        PrintfVarFormat(&u8g2,40, 12,u8g2_font_t0_11_mf, ROCK1_AD_X);
        // PrintfVarFormat(&u8g2,40, 24,u8g2_font_t0_11_mf, ROCK1_AD_X_HighByte);
        // PrintfVarFormat(&u8g2,80, 24,u8g2_font_t0_11_mf, ROCK1_AD_X_LowByte);
        PrintfVarFormat(&u8g2,40, 24,u8g2_font_t0_11_mf, ROCK1_AD_Y);

        ROCK2_AD_X = AD_GetValue(ROCK2_X_ADC_Channel);//ADC_X
		ROCK2_AD_Y = AD_GetValue(ROCK2_Y_ADC_Channel);//ADC_Y
		ROCK2_AD_X_HighByte = (ROCK2_AD_X >> 8) & 0xFF;//取出ADC_X的高8位
		ROCK2_AD_X_LowByte  =  ROCK2_AD_X & 0xFF;	   //取出ADC_X的低8位
		ROCK2_AD_Y_HighByte = (ROCK2_AD_Y >> 8) & 0xFF;//取出ADC_Y的高8位
		ROCK2_AD_Y_LowByte  =  ROCK2_AD_Y & 0xFF;	   //取出ADC_Y的低8位
        PrintfVarFormat(&u8g2,40, 36,u8g2_font_t0_11_mf, ROCK2_AD_X);
        PrintfVarFormat(&u8g2,40, 48,u8g2_font_t0_11_mf, ROCK2_AD_Y);
        if (keynum == ROCK2_KEY_LongCick)                      //id = 1是key0
        {

        }
        else if(keynum == ROCK1_KEY_LongCick)                                   //id =0 是keyup
        {
            printf("ROCK1_KEY_LongCick!\r\n");
            u8g2_SetFont(&u8g2,u8g2_font_wqy15_t_chinese1);                         //设置字体

            ui_index = E_SETTING;               //把当前的UI设为设置页面，一会会在他的处理函数中运算    
            ui_state = E_STATE_RUN_LED;         //UI状态设置为 跑向LED【其实是跑向设置】

            // menu_y_trg = 0;                  //设置LED界面的目标   
            Indic_vertical_x_trg = 120;
            Indic_Horizental_x_trg = 118;
            Indic_x_trg = 116;
            setting_x_trg = 0;                  //设置设置界面的目标

            AdvalueprocFirstEnterFlag = 0;
        }
        else if(keynum == ROCK2_KEY_SingleClick)                       //key0
        {
            printf("ROCK2_KEY_SingleClick!\r\n");
            LEDx_SetMode(LED1,Mode_ON);  
            led2flag = led2flagon;
        }
        else if(keynum == ROCK2_KEY_DobleClick)
        {
            printf("ROCK2_KEY_DobleClick!\r\n");
            LEDx_SetMode(LED1, Mode_OFF);  
            led2flag = led2flagoff;
        }

    switch (ui_state)
    {
        case E_STATE_RUN_LED:                               //运行跑向菜单，keyup按键长按时，E_STATE_RUN_PIC=4
        {
            static uint8_t pic_flag = 0;                    //定义一个缓存变量用以判断是否到位

            if (ui_run(&setting_x, &setting_x_trg, 10, 4) == 0)     //图片的y轴运动
            {
                pic_flag |= 0xf0;                           //如果图片到位了 或上0xF0     
            }
            if (ui_run(&Indic_vertical_x, &Indic_vertical_x_trg, 10 , 4))   //菜单的y轴运动
            {
                pic_flag |= 0x0f;                            //如果菜单到位了 或上0x0F                        
            }
            if (ui_run(&Indic_Horizental_x, &Indic_Horizental_x_trg, 10, 4) == 0)     //设置的y轴运动
            {
                pic_flag |= 0xf0;                                      //如果图片到位了 或上0xF0                                       
            } 
            if (ui_run(&Indic_x, &Indic_x_trg, 10, 4) == 0)     //设置的y轴运动
            {
                pic_flag |= 0xf0;                                      //如果图片到位了 或上0xF0                                       
            }
            if (pic_flag == 0xf0)                            // 0 | 0xf0 | 0x0f = 0xff
            {
                 //如果到位了 状态置为到位
                pic_flag = 0;                               //清除标志
                ui_state = E_STATE_ARRIVE;                  //菜单状态为到位，E_STATE_ARRIVE=5
            }
            break;
        }
    }
    // menu_ui_show(menu_y);            //显示菜单UI
    setting_ui_show(setting_y, Indic_y, Indic_vertical_x);                         //显示设置UI
}
uint8_t CatprocFirstEnterFlag = 0;
void cat_proc(void)
{
    /* 保存第一次进入时候不会触发按键动作 t=0这个赋值动作只有退出菜单时候才会调用，如rock1keylongclik*/
    if(CatprocFirstEnterFlag == 0) 
    {
        keynum = Key_None;
        CatprocFirstEnterFlag = 1;
        printf("key_none!\r\n");
    }
    static uint8_t run_flag = 0;
    if (ui_run(&menu_y, &menu_y_trg, 10, 4) == 0)
    {
        run_flag = 1;
    }


    static uint8_t bmp = 0;

    if (bmp==6)bmp=0;
    if(run_flag == 1)
    {
        if(ui_index != E_MENU) Delay_ms(50);
        switch(bmp) 
            {
            case 0:
                u8g2_DrawXBMP(&u8g2, cat_run_x, cat_run_y, 128, 64, cat0);
                printf("1111\r\n");
                break;
            case 1:
                u8g2_DrawXBMP(&u8g2, cat_run_x, cat_run_y, 128, 64, cat1);
                    printf("2222\r\n");
                break;
            case 2:
                u8g2_DrawXBMP(&u8g2, cat_run_x, cat_run_y, 128, 64, cat2);
                    printf("3333\r\n");
                break;
            case 3:
                u8g2_DrawXBMP(&u8g2, cat_run_x, cat_run_y, 128, 64, cat3);
                    printf("4444\r\n");
                break;
            case 4:
                u8g2_DrawXBMP(&u8g2, cat_run_x, cat_run_y, 128, 64, cat4);
                printf("5555\r\n");
                break;
            case 5:
                u8g2_DrawXBMP(&u8g2, cat_run_x, cat_run_y, 128, 64, cat5);
                printf("6666\r\n");
                break;
            default:
                break;
            }
            bmp++;
    }
  
        if (keynum == ROCK2_KEY_LongCick)                       //id = 1是key0
        {

        }
        else  if(keynum == ROCK1_KEY_LongCick)                                  //id =0 是keyup
        {
            printf("ROCK1_KEY_LongCick!\r\n");
            ui_index = E_MENU;                  //把当前的UI设为菜单页面，一会会在他的处理函数中运算    
            ui_state = E_STATE_RUN_CAT;         //UI状态设置为 跑向图片【其实是跑向菜单】

            menu_y_trg = 0;                     //设置菜单界面的目标   

            cat_run_y_trg = 64;                     //设置图片界面的目标
            CatprocFirstEnterFlag = 0;
        }

    switch (ui_state)
    {
     case E_STATE_RUN_CAT:                               //运行跑向菜单，keyup按键长按时，E_STATE_RUN_PIC=4
    {
           static uint8_t pic_flag = 0;                    //定义一个缓存变量用以判断是否到位
             if (ui_run(&cat_run_y, &cat_run_y_trg, 10, 4) == 0)     //图片的y轴运动
            {
                 pic_flag |= 0xf0;                           //如果图片到位了 或上0xF0     
            }
             if (ui_run(&menu_y, &menu_y_trg, 10, 4) == 0)   //菜单的y轴运动
             {
                 pic_flag |= 0x0f;                            //如果菜单到位了 或上0x0F                        
             }
             if (pic_flag == 0xff)                            // 0 | 0xf0 | 0x0f = 0xff
             {
                  //如果到位了 状态置为到位
                 pic_flag = 0;                               //清除标志
                 ui_state = E_STATE_ARRIVE;                  //菜单状态为到位，E_STATE_ARRIVE=5
             }
             menu_ui_show(menu_y);            //显示菜单UI
             run_flag = 0;
            break;
         }
    }

    ui_run(&cat_run_x, &cat_run_x_trg, 10, 4);	
}
void bat_proc(void)
{  
    BAT_AD_VALUE = AD_GetValue(BAT_ADC_Channel);
    printf("%.2f\r\n\r\n", (BAT_AD_VALUE * 2 *3.3 / 4095));
    PrintfVarFormatFloat(&u8g2,40, 36,u8g2_font_wqy15_t_chinese1, (BAT_AD_VALUE * 2 *3.3 / 4095));
    if(keynum == ROCK1_KEY_LongCick)                                   //id =0 是keyup
    {
        u8g2_SetFont(&u8g2,u8g2_font_wqy15_t_chinese1);                         //设置字体

        ui_index = E_SETTING;               //把当前的UI设为设置页面，一会会在他的处理函数中运算    
        ui_state = E_STATE_RUN_LED;         //UI状态设置为 跑向LED【其实是跑向设置】

        // menu_y_trg = 0;                  //设置LED界面的目标   
        Indic_vertical_x_trg = 120;
        Indic_Horizental_x_trg = 118;
        Indic_x_trg = 116;
        setting_x_trg = 0;                  //设置设置界面的目标
    }
    switch (ui_state)
    {
        case E_STATE_RUN_LED:                               //运行跑向菜单，keyup按键长按时，E_STATE_RUN_PIC=4
        {
            static uint8_t pic_flag = 0;                    //定义一个缓存变量用以判断是否到位

            if (ui_run(&setting_x, &setting_x_trg, 10, 4) == 0)     //图片的y轴运动
            {
                pic_flag |= 0xf0;                           //如果图片到位了 或上0xF0     
            }
            if (ui_run(&Indic_vertical_x, &Indic_vertical_x_trg, 10 , 4))   //菜单的y轴运动
            {
                pic_flag |= 0x0f;                            //如果菜单到位了 或上0x0F                        
            }
            if (ui_run(&Indic_Horizental_x, &Indic_Horizental_x_trg, 10, 4) == 0)     //设置的y轴运动
            {
                pic_flag |= 0xf0;                                      //如果图片到位了 或上0xF0                                       
            } 
            if (ui_run(&Indic_x, &Indic_x_trg, 10, 4) == 0)     //设置的y轴运动
            {
                pic_flag |= 0xf0;                                      //如果图片到位了 或上0xF0                                       
            }
            if (pic_flag == 0xf0)                            // 0 | 0xf0 | 0x0f = 0xff
            {
                 //如果到位了 状态置为到位
                pic_flag = 0;                               //清除标志
                ui_state = E_STATE_ARRIVE;                  //菜单状态为到位，E_STATE_ARRIVE=5
            }
            setting_ui_show(setting_y, Indic_y, Indic_vertical_x);                         //显示设置UI
            break;
        }
    }
    // menu_ui_show(menu_y);            //显示菜单UI

}
void mpu6050_proc(void)
{  

    if(mpu_dmp_get_data(&pitch,&roll,&yaw)==0)
    { 
        temp=MPU_Get_Temperature();	//得到温度值
        printf("\n\r pitch=%.2f      roll=%.2f      yaw=%.2f      temp=%2d.%d\n\r",pitch,roll,yaw, temp/100, temp%10); 
        MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	//得到加速度传感器数据
        MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);	//得到陀螺仪数据
    }
    u8g2_SetFont(&u8g2,u8g2_font_t0_12_mf);                         //设置字体
    u8g2_DrawStr(&u8g2, 0, 12, "pitch");
    u8g2_DrawStr(&u8g2, 0, 24, "roll");
    u8g2_DrawStr(&u8g2, 0, 36, "yaw");
    u8g2_DrawStr(&u8g2, 0, 48, "temp");
    PrintfVarFormatFloat(&u8g2, 80, 12, u8g2_font_t0_12_mf, pitch);
    PrintfVarFormatFloat(&u8g2, 80, 24, u8g2_font_t0_12_mf, roll);
    PrintfVarFormatFloat(&u8g2, 80, 36, u8g2_font_t0_12_mf, yaw);
    PrintfVarFormat(&u8g2, 72, 48, u8g2_font_t0_12_mf, temp/100);  

    char buff[10];
    u8g2_SetFont(&u8g2,u8g2_font_t0_12_mf);//设定字体
    u8g2_DrawStr(&u8g2, 96, 48, ".");
    sprintf(buff,"%d",temp%10);//4是长度的限制 0是不足4位前面补0 如num为100 打印出来是0100 此函数的                                                                                                            返回值是buff的长度
    u8g2_DrawStr(&u8g2,104,48,buff);

    if(keynum == ROCK1_KEY_LongCick)                                   //id =0 是keyup
    {
        u8g2_SetFont(&u8g2,u8g2_font_wqy15_t_chinese1);                         //设置字体

        ui_index = E_SETTING;               //把当前的UI设为设置页面，一会会在他的处理函数中运算    
        ui_state = E_STATE_RUN_LED;         //UI状态设置为 跑向LED【其实是跑向设置】

        // menu_y_trg = 0;                  //设置LED界面的目标   
        Indic_vertical_x_trg = 120;
        Indic_Horizental_x_trg = 118;
        Indic_x_trg = 116;
        setting_x_trg = 0;                  //设置设置界面的目标
    }
    switch (ui_state)
    {
        case E_STATE_RUN_LED:                               //运行跑向菜单，keyup按键长按时，E_STATE_RUN_PIC=4
        {
            static uint8_t pic_flag = 0;                    //定义一个缓存变量用以判断是否到位

            if (ui_run(&setting_x, &setting_x_trg, 10, 4) == 0)     //图片的y轴运动
            {
                pic_flag |= 0xf0;                           //如果图片到位了 或上0xF0     
            }
            if (ui_run(&Indic_vertical_x, &Indic_vertical_x_trg, 10 , 4))   //菜单的y轴运动
            {
                pic_flag |= 0x0f;                            //如果菜单到位了 或上0x0F                        
            }
            if (ui_run(&Indic_Horizental_x, &Indic_Horizental_x_trg, 10, 4) == 0)     //设置的y轴运动
            {
                pic_flag |= 0xf0;                                      //如果图片到位了 或上0xF0                                       
            } 
            if (ui_run(&Indic_x, &Indic_x_trg, 10, 4) == 0)     //设置的y轴运动
            {
                pic_flag |= 0xf0;                                      //如果图片到位了 或上0xF0                                       
            }
            if (pic_flag == 0xf0)                            // 0 | 0xf0 | 0x0f = 0xff
            {
                 //如果到位了 状态置为到位
                pic_flag = 0;                               //清除标志
                ui_state = E_STATE_ARRIVE;                  //菜单状态为到位，E_STATE_ARRIVE=5
            }
            setting_ui_show(setting_y, Indic_y, Indic_vertical_x);                         //显示设置UI
            break;
        }
    }

}
void about_proc(void)
{  
    u8g2_SetFont(&u8g2,u8g2_font_helvB12_tf);                         //设置字体
    u8g2_DrawStr(&u8g2, 0, 18, "LDQ");
    u8g2_DrawStr(&u8g2, 0, 36, "2025.2.19");
    u8g2_DrawStr(&u8g2, 0, 54, "U8g2");

    
    if(keynum == ROCK1_KEY_LongCick)                                   //id =0 是keyup
    {
        u8g2_SetFont(&u8g2,u8g2_font_wqy15_t_chinese1);                         //设置字体

        ui_index = E_SETTING;               //把当前的UI设为设置页面，一会会在他的处理函数中运算    
        ui_state = E_STATE_RUN_LED;         //UI状态设置为 跑向LED【其实是跑向设置】

        // menu_y_trg = 0;                  //设置LED界面的目标   
        Indic_vertical_x_trg = 120;
        Indic_Horizental_x_trg = 118;
        Indic_x_trg = 116;
        setting_x_trg = 0;                  //设置设置界面的目标
    }
    switch (ui_state)
    {
        case E_STATE_RUN_LED:                               //运行跑向菜单，keyup按键长按时，E_STATE_RUN_PIC=4
        {
            static uint8_t pic_flag = 0;                    //定义一个缓存变量用以判断是否到位

            if (ui_run(&setting_x, &setting_x_trg, 10, 4) == 0)     //图片的y轴运动
            {
                pic_flag |= 0xf0;                           //如果图片到位了 或上0xF0     
            }
            if (ui_run(&Indic_vertical_x, &Indic_vertical_x_trg, 10 , 4))   //菜单的y轴运动
            {
                pic_flag |= 0x0f;                            //如果菜单到位了 或上0x0F                        
            }
            if (ui_run(&Indic_Horizental_x, &Indic_Horizental_x_trg, 10, 4) == 0)     //设置的y轴运动
            {
                pic_flag |= 0xf0;                                      //如果图片到位了 或上0xF0                                       
            } 
            if (ui_run(&Indic_x, &Indic_x_trg, 10, 4) == 0)     //设置的y轴运动
            {
                pic_flag |= 0xf0;                                      //如果图片到位了 或上0xF0                                       
            }
            if (pic_flag == 0xf0)                            // 0 | 0xf0 | 0x0f = 0xff
            {
                 //如果到位了 状态置为到位
                pic_flag = 0;                               //清除标志
                ui_state = E_STATE_ARRIVE;                  //菜单状态为到位，E_STATE_ARRIVE=5
            }
            setting_ui_show(setting_y, Indic_y, Indic_vertical_x);                         //显示设置UI
            break;
        }
    }

}
UI_LIST ui_list[] = //UI表
{
    {E_MENU,    NULL},
    {E_CAT,     cat_proc  },
    {E_PIC,     pic_proc  },
    {E_SETTING, setting_proc},
    {E_LED,     led_proc},
    {E_ADVALUE, advalue_proc},
    {E_BAT,     bat_proc},
    {E_MPU6050, mpu6050_proc},
    {E_ABOUT,   about_proc},
};
void ui_proc(void)
{
    int i;

    for (i = 0 ; i < E_UI_MAX ; i++)            //E_UI_MAX=3        E_MENU=0    E_PIC=1      E_SETTING=2
    {
        if (ui_index == ui_list[i].index)       //如果当前索引等于UI表中的索引
        {
            // if (ui_list[i].cb)                  //执行UI对应的回调函数
            // {
                u8g2_ClearBuffer(&u8g2);         // 清除内部缓冲区
                ui_list[i].cb();              //执行对应的回调函数，并传入按键信息作为形参
                u8g2_SendBuffer(&u8g2);          // transfer internal memory to the
            // }
        }
    }
}
