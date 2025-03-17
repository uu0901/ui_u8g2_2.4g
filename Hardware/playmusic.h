#ifndef __PLAYMUSIC_H__
#define __PLAYMUSIC_H__
#include "stm32f10x.h" 
//国际标准音高频率对照表
//0
#define A0_SHARP   29
#define A0         27
#define B0         30
//1
#define C1         32
#define C1_        34
#define D1         36
#define D1_        38
#define E1         41
#define F1         43
#define F1_        46
#define G1         48
#define G1_        51
#define A1         55
#define A1_        58
#define B1         61
//2
#define C2         65
#define C2_        69
#define D2         73
#define D2_        77
#define E2         82
#define F2         87
#define F2_        92
#define G2         97
#define G2_        103
#define A2         110
#define A2_        116
#define B2         123
//3
#define C3         130
#define C3_        138
#define D3         146
#define D3_        155
#define E3         164
#define F3         174
#define F3_        184
#define G3         195
#define G3_        207
#define A3         220
#define A3_        233
#define B3         246
//4
#define C4         261
#define C4_        277
#define D4         293
#define D4_        311
#define E4         329
#define F4         349
#define F4_        369
#define G4         392
#define G4_        415
#define A4         440
#define A4_        466
#define B4         493
//5
#define C5         523
#define C5_        554
#define D5         587
#define D5_        622
#define E5         659
#define F5         698
#define F5_        739
#define G5         783
#define G5_        830
#define A5         880
#define A5_        932
#define B5         987
//6
#define C6         1046
#define C6_        1108
#define D6         1174
#define D6_        1244
#define E6         1318
#define F6         1396
#define F6_        1479
#define G6         1567
#define G6_        1661
#define A6         1760
#define A6_        1864
#define B6         1975
//7
#define C7         2093
#define C7_        2217
#define D7         2349
#define D7_        2499
#define E7         2637
#define F7         2793
#define F7_        2959
#define G7         3135
#define G7_        3322
#define A7         3520
#define A7_        3729
#define B7         3951
//8
#define C8         4186



void Music_init(void);
void Video_init(void);
void Sound_SetHZ(uint16_t pre);
void Play_Music(int pre,int tm,int tm0);
void Play_song(void);
void TIM3_IRQHandler(void);
void Video_init(void);
void Play_video(void);
#endif
