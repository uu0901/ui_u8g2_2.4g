#include "IIC_OLED.h"

#define OLED_ADDRESS		0x78		//MPU6050的I2C从机地址
void Oled_I2C_WaitEvent(I2C_TypeDef* I2Cx, uint32_t I2C_EVENT)
{
	uint32_t Timeout;
	Timeout = 10000;									//给定超时计数时间
	while (I2C_CheckEvent(I2Cx, I2C_EVENT) != SUCCESS)	//循环等待指定事件
	{
		Timeout --;										//等待时，计数值自减
		if (Timeout == 0)								//自减到0后，等待超时
		{
			/*超时的错误处理代码，可以添加到此处*/
			break;										//跳出等待，不等了
		}
	}
}
void I2C_WriteByte(uint8_t RegAddress, uint8_t Data)
{
	I2C_GenerateSTART(I2C1, ENABLE);										//硬件I2C生成起始条件
	Oled_I2C_WaitEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT);					//等待EV5
	
	I2C_Send7bitAddress(I2C1, OLED_ADDRESS, I2C_Direction_Transmitter);	//硬件I2C发送从机地址，方向为发送
	Oled_I2C_WaitEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);	//等待EV6
	
	I2C_SendData(I2C1, RegAddress);											//硬件I2C发送寄存器地址
	Oled_I2C_WaitEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING);			//等待EV8
	
	I2C_SendData(I2C1, Data);												//硬件I2C发送数据
	Oled_I2C_WaitEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED);				//等待EV8_2
	
	I2C_GenerateSTOP(I2C1, ENABLE);											//硬件I2C生成终止条件
}
void WriteCmd(unsigned char I2C_Command) // 写命令
{
	I2C_WriteByte(0x00, I2C_Command); //
}

void WriteDat(unsigned char I2C_Data) // 写数据
{
	I2C_WriteByte(0x40, I2C_Data);
}
// 反显函数
void OLED_ColorTurn(u8 i)
{
	if (i == 0)
	{
		WriteCmd(0xA6);
	}
	if (i == 1)
	{
		WriteCmd(0xA7);
	}
}
// 屏幕旋转180度
void OLED_DisplayTurn(u8 i)
{
	if (i == 0)
	{
		WriteCmd(0xC8);
		WriteCmd(0xA1);
	}
	if (i == 1)
	{
		WriteCmd(0xC0);
		WriteCmd(0xA0);
	}
}
// 开启OLED显示
void OLED_DisPlay_On(void)
{
	WriteCmd(0x8D);
	WriteCmd(0x14);
	WriteCmd(0xAF);
}

// 关闭OLED显示
void OLED_DisPlay_Off(void)
{
	WriteCmd(0x8D);
	WriteCmd(0x10);
	WriteCmd(0xAE);
}
void OLED_Init(void)
{
	/*开启时钟*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);		//开启I2C2的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);		//开启GPIOB的时钟
	GPIO_PinRemapConfig(GPIO_Remap_I2C1, ENABLE);
	/*GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);					//将PB10和PB11引脚初始化为复用开漏输出
	
	/*I2C初始化*/
	I2C_InitTypeDef I2C_InitStructure;						//定义结构体变量
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;				//模式，选择为I2C模式
	I2C_InitStructure.I2C_ClockSpeed = 800000;				//时钟速度，选择为50KHz
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;		//时钟占空比，选择Tlow/Thigh = 2
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;				//应答，选择使能
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;	//应答地址，选择7位，从机模式下才有效
	I2C_InitStructure.I2C_OwnAddress1 = 0x00;				//自身地址，从机模式下才有效
	I2C_Init(I2C1, &I2C_InitStructure);						//将结构体变量交给I2C_Init，配置I2C2
	
	/*I2C使能*/
	I2C_Cmd(I2C1, ENABLE);									//使能I2C2，开始运行
}

