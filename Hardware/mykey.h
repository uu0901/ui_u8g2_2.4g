#ifndef __MYKEY_H
#define __MYKEY_H

#define PRESS_ROCK1_KEY     1
#define PRESS_ROCK2_KEY     2
#define PRESS_KEY_L         3
#define PRESS_KEY_R         4
#define PRESS_NONE          0

#define LongTime        1500
#define DoubleTime      500
#define KeyDelay        30

#define Key_None                  99

#define ROCK1_KEY_SingleClick     1
#define ROCK1_KEY_DobleClick      2
#define ROCK1_KEY_LongCick        3

#define ROCK2_KEY_SingleClick     4
#define ROCK2_KEY_DobleClick      5
#define ROCK2_KEY_LongCick        6

#define KEY_L_SingleClick         7
#define KEY_L_DobleClick          8
#define KEY_L_LongCick            9

#define KEY_R_SingleClick         10
#define KEY_R_DobleClick          11
#define KEY_R_LongCick            12

void My_Key_Init(void);
uint8_t Key_GetNum(void);
void Key_Tick(void);

#endif
