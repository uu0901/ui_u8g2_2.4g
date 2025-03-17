#include "stm32f10x.h"                  // Device header
#include "mykey.h"                  // Device header
uint8_t Key_Num;

void My_Key_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC, ENABLE);


    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;      //ROCK1_KEY
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;      //ROCK2_KEY
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;     //KEY_L
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;     //KEY_R
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure); 
}

uint8_t Key_GetNum(void)
{
    /* 
    �˴��벻��������жϷ�����Temp = Key_Num; Key_Num = 0; ������֮�䣬
    �����ж������˼����־λ����ô�ж��˳��󣬻�����ִ��key_Nm=0��
    �����ٴε��ô˺������Ƿ���0�ˣ�Ҳ���Ǵ˴ΰ����¼��ͻᱻ���ԡ�
    ����Ҫ���һ��if�жϣ����key_num��Ϊ0����ִ�ж�����0�Ĳ��� 
    */
    //uint8_t Temp;
    //Temp = Key_Num;
    //Key_Num = 0;
    //return Temp;
    

    uint8_t Temp;
    if (Key_Num)
    {
        Temp = Key_Num;
        Key_Num = 0;

        return Temp;
    }
    return 0;
}

uint8_t Key_GetState(void)//��ȡ����״̬
{
    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15) == 0) //ROCK1_KEY����
    {
        return PRESS_ROCK1_KEY;
    }
    if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8) == 0) //ROCK2_KEY����
    {
        return PRESS_ROCK2_KEY;
    }
    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == 0)//KEY_L����
    {
        return PRESS_KEY_L;
    }
    if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_15) == 0)//KEY_R����
    {
        return PRESS_KEY_R;
    }    
    return PRESS_NONE;//�ް�������
}


void Key_Tick(void)//����ɨ��1ms����һ��
{
    //��̬����Ĭ�ϳ�ֵΪ0
    static uint8_t Count;                                    //����ֵ
    static uint8_t CurrentKeyVal, PreviousKeyVal;           //����״̬����һ��״̬
    static uint16_t PressDownCnt;
    static uint16_t RePressDownCnt;
    static uint16_t PressUpCnt;
    static uint8_t step;  
    static uint8_t ROCK1_KEY_FLAG, ROCK2_KEY_FLAG, KEY_L_FLAG, KEY_R_FLAG;//������־λ
    Count ++;
    if (Count >= 1)                                //20ms����һ��Key_GetState
    {
        Count = 0;

        PreviousKeyVal = CurrentKeyVal;                     //�ϴ�״̬��Ҳ������һ�εı���
        CurrentKeyVal = Key_GetState();                      //����״̬
    }
    if(step == 0)
    {
        PressDownCnt = 0; 
        RePressDownCnt = 0;
        PressUpCnt = 0;
        if (CurrentKeyVal !=0  && PreviousKeyVal == 0)      //����Ϊ0 �ϴη�0��˵���а������²�����  ����Ϊ1 �ɿ�Ϊ0
        {
            step = 1;
            if(CurrentKeyVal == PRESS_ROCK1_KEY) ROCK1_KEY_FLAG = 1;
            if(CurrentKeyVal == PRESS_ROCK2_KEY) ROCK2_KEY_FLAG = 1;
            if(CurrentKeyVal == PRESS_KEY_L) KEY_L_FLAG = 1;
            if(CurrentKeyVal == PRESS_KEY_R) KEY_R_FLAG = 1;
        }
    }
    else if (step == 1)
    {
        if(CurrentKeyVal != PRESS_NONE)//����
        {
            PressDownCnt ++;
            if(PressDownCnt >= LongTime)//����
            {
                if(CurrentKeyVal == PRESS_ROCK1_KEY)
                {
                    Key_Num = ROCK1_KEY_LongCick;
                    ROCK1_KEY_FLAG = 0;
                }
                else if(CurrentKeyVal == PRESS_ROCK2_KEY)
                {
                    Key_Num = ROCK2_KEY_LongCick;
                    ROCK2_KEY_FLAG = 0;
                }
                else if(CurrentKeyVal == PRESS_KEY_L)
                {
                    Key_Num = KEY_L_LongCick;
                    KEY_L_FLAG = 0;
                }
                else if(CurrentKeyVal == PRESS_KEY_R)
                {
                    Key_Num = KEY_R_LongCick;
                    KEY_R_FLAG = 0;
                }
                step = 0;
            }
        }
        else //����
        {

            step = 2;
        }
    }  
    else if (step == 2)//����
    {
        PressUpCnt ++;
        if(PressUpCnt <= DoubleTime)
        {
            if(CurrentKeyVal != PRESS_NONE)//����
            {
                RePressDownCnt ++;
            }
            if(RePressDownCnt > KeyDelay)//˫��
            {
                if(CurrentKeyVal == PRESS_ROCK1_KEY)
                {
                    ROCK1_KEY_FLAG = 0;
                    Key_Num = ROCK1_KEY_DobleClick;
                }
                else if(CurrentKeyVal == PRESS_ROCK2_KEY)
                {
                    ROCK2_KEY_FLAG = 0;
                    Key_Num = ROCK2_KEY_DobleClick;
                }
                else if(CurrentKeyVal == PRESS_KEY_L)
                {
                    KEY_L_FLAG = 0;
                    Key_Num = KEY_L_DobleClick;
                }
                else if(CurrentKeyVal == PRESS_KEY_R)
                {
                    KEY_R_FLAG = 0;
                    Key_Num = KEY_R_DobleClick;
                }
                step = 0;
            }
        }
        else//����
        {
 
            if(PressDownCnt > KeyDelay)//����
            {
                if(ROCK1_KEY_FLAG == 1)
                {
                    ROCK1_KEY_FLAG = 0;
                    Key_Num = ROCK1_KEY_SingleClick;
                }
                else if(ROCK2_KEY_FLAG == 1)
                {
                    ROCK2_KEY_FLAG = 0;
                    Key_Num = ROCK2_KEY_SingleClick;
                }
                else if(KEY_L_FLAG == 1)
                {
                    KEY_L_FLAG = 0;
                    Key_Num = KEY_L_SingleClick;
                }
                else if(KEY_R_FLAG == 1)
                {
                    KEY_R_FLAG = 0;
                    Key_Num = KEY_R_SingleClick;
                }
                step = 0;
            }
        }
    }
}
        

