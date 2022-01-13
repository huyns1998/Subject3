/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Auto-generated by STM32CubeIDE
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
#include "system_stm32f4xx.h"
#include "eventman.h"
#include "timer.h"
#include "led.h"
#include "melody.h"
#include "lightsensor.h"
#include "temhumsensor.h"
#include "eventbutton.h"
#include "button.h"
#include "Ucglib.h"
#include "uartcmd.h"
#include "serial.h"
#include "stdio.h"
#include "string.h"
#include "stm32f401re_rcc.h"
#include "stm32f401re_gpio.h"
#include "stm32f401re_usart.h"
#include "misc.h"
#include "stm32f401re.h"
#include "buff.h"
#include "stm32f401re_i2c.h"
#include "stm32f401re_exti.h"
#include "stm32f401re_syscfg.h"

#define GPIO_PIN_SET				1
#define GPIO_PIN_RESET				0
#define GPIO_PIN_LOW				0
#define GPIO_PIN_HIGH				1

#define USART2_TX					GPIO_Pin_2  //PA2		 / PD5
#define USART2_RX					GPIO_Pin_3 //PA3         / PD6
#define USART2_GPIO					GPIOA
#define USART2_GPIO_CLOCK			RCC_AHB1Periph_GPIOA
#define USART2_CLOCK				RCC_APB1Periph_USART2

#define USARTx_Buad					9600


#define BUZZER_GPIO_PORT					GPIOC
#define BUZZER_GPIO_PIN						GPIO_Pin_9
#define BUZZER_PIN							9
#define BUZZERControl_SetClock				RCC_AHB1Periph_GPIOC

#define LED1_RED_ID							1
#define LED1_RED_GPIO_PORT					GPIOA
#define LED1_RED_GPIO_PIN					GPIO_Pin_1
#define LED1_RED_PIN						1
#define LED1_REDControl_SetClock			RCC_AHB1Periph_GPIOA

#define LED1_GREEN_ID						0
#define LED1_GREEN_GPIO_PORT				GPIOA
#define LED1_GREEN_GPIO_PIN					GPIO_Pin_0
#define LED1_GREEN_PIN						0
#define LED1_GREENControl_SetClock			RCC_AHB1Periph_GPIOA

#define LED1_BLUE_ID						0
#define LED1_BLUE_GPIO_PORT					GPIOA
#define LED1_BLUE_GPIO_PIN					GPIO_Pin_3
#define LED1_BLUE_PIN						3
#define LED1_BLUEControl_SetClock			RCC_AHB1Periph_GPIOA

#define LED2_BLUE_ID						10
#define LED2_BLUE_GPIO_PORT					GPIOA
#define LED2_BLUE_GPIO_PIN					GPIO_Pin_10
#define LED2_BLUE_PIN						10
#define LED2_BLUEControl_SetClock			RCC_AHB1Periph_GPIOA

#define LED2_RED_ID							13
#define LED2_RED_GPIO_PORT					GPIOB
#define LED2_RED_GPIO_PIN					GPIO_Pin_13
#define LED2_RED_PIN						13
#define LED2_REDControl_SetClock			RCC_AHB1Periph_GPIOB

#define LED2_GREEN_ID						0
#define LED2_GREEN_GPIO_PORT				GPIOA
#define LED2_GREEN_GPIO_PIN					GPIO_Pin_11
#define LED2_GREEN_PIN						11
#define LED2_GREENControl_SetClock			RCC_AHB1Periph_GPIOA

// Macro------------------------------------------------------------------
#define SIZE_QUEUE_DATA_RX 256
//Khởi tạo giá trị kích thước mảng nhận dữ liệu là 16 bytes.
//#define RX_BUFFER_SIZE     16
//----------------------------------------------------------------------------
//Khởi tạo các giá trị cho các byte Start, byte ACK, byte NACK, byte CheckXor
/* @brief Start of frame */
#define FRAME_SOF		0xB1

/* @brief Frame ack and nack */
#define FRAME_ACK		0x06
#define FRAME_NACK		0x15

/* @brief check xor init  */
#define CXOR_INIT_VAL	0xFF

/*
 * Gán các giá trị các phần tử trong mảng byRxBuffer với các giá trị kiểm tra
 * dữ liệu sau khi nhận được dữ liệu gồm các byte Command ID, Command Type, và Data.
 */
#define CMD_ID				byRxBuffer[2]
#define CMD_TYPE			byRxBuffer[3]
#define CMD_DATA_EPOINT		byRxBuffer[4]
#define CMD_DATA_STATE		byRxBuffer[5]
#define CMD_LAST_STATE		byRxBuffer[8]

#define LED_CONTROL			0x01
#define BUZZER_CONTROL		0x04

//macros for I2C
#define I2C_MASTER_RCC					RCC_APB1Periph_I2C1
#define I2C_MASTER_INSTANCE				I2C1

#define I2C_MASTER_GPIO_RCC				RCC_AHB1Periph_GPIOB
#define I2C_MASTER_PORT					GPIOB
#define SDA_MASTER_PIN					GPIO_Pin_9
#define SCL_MASTER_PIN					GPIO_Pin_8

#define I2C_SPEED						400000

//Other macros
#define SEND							1
#define RECEIVE							0
#define SLAVE_ADDR						0x40

#define TEMPERATURE						0
#define HUMIDITY						1

#define OPTION							0x00
#define CMDID_HUMI						0x85
#define CMDID_TEMP						0x84
#define CMDID_LED						0x01
#define CMDTYPE_RES						0x01
#define PAYLOAD1						0x00

#define BUTTONB2_GPIO_PORT				GPIOB
#define BUTTONB2_GPIO_PIN				GPIO_Pin_3
#define BUTTONB2_PIN					3
#define BUTTONB2Control_SetClock		RCC_AHB1Periph_GPIOB
#define SYSCFG_Clock					RCC_APB2Periph_SYSCFG

#define BUTTONB3_GPIO_PORT				GPIOA
#define BUTTONB3_GPIO_PIN				GPIO_Pin_4
#define BUTTONB3_PIN					4
#define BUTTONB3Control_SetClock		RCC_AHB1Periph_GPIOA

#define LED_ID_1						0x01
#define LED_ID_2						0x02
#define LED_COLOR_RED					0x00
#define LED_COLOR_GREEN					0x01
#define LED_COLOR_BLUE					0x02


static float _err_measure;
static float _err_estimate;
static float _q;
static float _current_estimate;
static float _last_estimate;
static float _kalman_gain;

/*	Private variables**************************************************/
static uint8_t byRxBufState;
static uint8_t byIndexRxBuf;
static uint8_t byCheckXorRxBuf;
static buffqueue_t serialQueueRx;
static uint8_t pBuffDataRx[SIZE_QUEUE_DATA_RX];
uint8_t byRxBuffer[RX_BUFFER_SIZE] = { 0 };
static buffqueue_t serialQueueRx;
//-----------------------------------------------------------------------
void Button_Init(void);
static void Interrupt_Init(void);
uint8_t PollRxBuff(void);
void USART2_Init(void);
void Serial_Init(void);
void LedBuzz_Init(void);
void delay(uint32_t millisecond);

void GPIO_I2CMaster_Inits(void);
void I2CMaster_Inits(void);
static void I2C_Start(void);
static void I2C_address_direction (uint8_t SlaveAddr, uint8_t direction);
static void I2C_Transmit(uint8_t Data);
static void I2C_Stop(void);
uint8_t I2C_receive_ack(void);
uint8_t I2C_receive_nack(void);
void delay(uint32_t millisecond);
float CalculateTemp(uint16_t data);
float CalculateHumi(uint16_t data);
float ReadHumidity(void);
float ReadTemperature(void);
void processGetValueSensor(void);
float TemHumSensor_readRegister(uint8_t slave_addr, uint8_t cmd_id, uint8_t tempOrhumi);
uint32_t CalculatorTime(uint32_t dwTimeInit, uint32_t dwTimeCurrent);
void InitSensor(void);
void SendDataToUsart(uint8_t option, uint8_t cmd_id, uint8_t cmd_type, float payload1, float payload2);
void HumiSensor_SendPacket(float humi);
void TempSensor_SendPacket(float temp);
void BuzzerControl_SendPacket(void);
void LedControl_SendPacket(uint8_t led_id, uint8_t led_color, uint8_t led_state);

//Struct for sensor
typedef struct
{
	float humi;
	float temp;
}Sensor;

GPIO_InitTypeDef 	GPIO_InitStructure;

//Init Button
void Button_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	//Init B2
	RCC_AHB1PeriphClockCmd(BUTTONB2Control_SetClock, ENABLE);

	GPIO_InitStruct.GPIO_Pin = BUTTONB2_GPIO_PIN;

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;

	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;

	GPIO_Init(BUTTONB2_GPIO_PORT, &GPIO_InitStruct);

	//Init B3

	RCC_AHB1PeriphClockCmd(BUTTONB3Control_SetClock, ENABLE);

	GPIO_InitStruct.GPIO_Pin = BUTTONB3_GPIO_PIN;

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;

	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;

	GPIO_Init(BUTTONB3_GPIO_PORT, &GPIO_InitStruct);

}

//B3:PA4
static void Interrupt_Init(void)
{
	//B2 interrupt config
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(SYSCFG_Clock, ENABLE);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource3);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource4);

	EXTI_InitStructure.EXTI_Line = EXTI_Line3;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//B3 interrupt config
	EXTI_InitStructure.EXTI_Line = EXTI_Line4;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

//Function ID for transmission frame
static uint8_t FunctionId = 1;
void BuzzerControl_SendPacket(void)
{
	//Init Checkxor byte
	uint8_t checkXor = CXOR_INIT_VAL;

	//option
	checkXor ^= OPTION; //0x00

	//CMD_ID
	checkXor ^= 0x04;

	//CMD_TYPE	RES(send data)
	checkXor ^= 0x01;//0x01

	//Payload
	checkXor ^= 0x01;//0x00

	//FUNC_ID
	checkXor ^= FunctionId;

	uint8_t data[] = {0xB1, 0x06, 0x00, 0x04, 0x01, 0x01, FunctionId, checkXor};

	for(uint8_t i = 0; i < 8; i++)
	{
		USART_SendData(USART2, (uint16_t)data[i]);
		while( !USART_GetFlagStatus(USART2,  USART_FLAG_TXE));
	}

	//Increase func_id max = 0xFE
	FunctionId++;
	if(FunctionId > 0xFE)
	{
		FunctionId = 1;
	}
}

//Update led state to PC
void LedControl_SendPacket(uint8_t led_id, uint8_t led_color, uint8_t led_state)
{
	//Init Checkxor byte
	uint8_t checkXor = CXOR_INIT_VAL;

	//OPT
	checkXor ^= OPTION; //0x00

	//CMD_ID
	checkXor ^= 0x01;

	//CMD_TYPE	RES(send data)
	checkXor ^= 0x02;//0x01

	//Payload
	checkXor ^= led_id;//0x01,0x02
	checkXor ^= led_color;//0x00->0x02

	//if led turn on
	if(led_state == 1)
	{
		//payload led state
		checkXor ^= 0x64;

		//FUNC_ID
		checkXor ^= FunctionId;

		uint8_t data[] = {0xB1, 0x08, 0x00, 0x01, 0x02, led_id, led_color, 0x64, FunctionId, checkXor};

		//send data
		for(uint8_t i = 0; i < 10; i++)
		{
			USART_SendData(USART2, (uint16_t)data[i]);
			while( !USART_GetFlagStatus(USART2,  USART_FLAG_TXE));
		}
	}
	//if led turn off
	else
	{
		//payload led state
		checkXor ^= 0x00;

		//FUNC_ID
		checkXor ^= FunctionId;

		uint8_t data[]= {0xB1, 0x08, 0x00, 0x01, 0x02, led_id, led_color, 0x00, FunctionId, checkXor};

		//send data
		for(uint8_t i = 0; i < 10; i++)
		{
			USART_SendData(USART2, (uint16_t)data[i]);
			while( !USART_GetFlagStatus(USART2,  USART_FLAG_TXE));
		}
	}

	//Increase func_id until func_id = 0xFE then reset
	FunctionId++;
	if(FunctionId > 0xFE)
	{
		FunctionId = 1;
	}
}

//B2 press interupt handler
void EXTI3_IRQHandler(void)
{
	if(EXTI_GetFlagStatus(EXTI_Line3) == SET)
	{
		//Send data for turn on buzzer to PC
		BuzzerControl_SendPacket();

		//Turn on buzzer
		GPIO_WriteBit(BUZZER_GPIO_PORT, BUZZER_GPIO_PIN, 1);
		delay(200);

		//Turn off buzzer
		GPIO_WriteBit(BUZZER_GPIO_PORT, BUZZER_GPIO_PIN, 0);
	}

	//Clear pending bit so that interrupt will not repeat
	EXTI_ClearITPendingBit(EXTI_Line3);
}

//Led state
//0 : LED1_RED
//1 : LED1_GREEN
//2 : LED1_BLUE
//3 : LED2_RED
//4 : LED2_GREEN
//5 : LED2_BLUE
uint8_t led_state[] = {0, 0, 0, 0, 0, 0};
void EXTI4_IRQHandler(void)
{
	if(EXTI_GetFlagStatus(EXTI_Line4) == SET)
	{
		//if led off then turn on
		if(led_state[4] == 0)
		{
			led_state[4] = 1;
			//Send data to PC
			LedControl_SendPacket(LED_ID_2, LED_COLOR_GREEN, 1);
			//Turn on led
			GPIO_WriteBit(LED1_GREEN_GPIO_PORT, LED1_GREEN_GPIO_PIN, 1);
		}
		//if led on then turn off
		else
		{
			led_state[4] = 0;
			//Send data to PC
			LedControl_SendPacket(LED_ID_2, LED_COLOR_GREEN, 0);
			//Turn off led
			GPIO_WriteBit(LED1_GREEN_GPIO_PORT, LED1_GREEN_GPIO_PIN, 0);
		}
	}
	//Clear pending bit so that interrupt will not repeat
	EXTI_ClearITPendingBit(EXTI_Line4);
}


//I2C1_SDA: PB9
//I2C1_SCL:	PB8
//Init pins for I2C1
void GPIO_I2CMaster_Inits(void)
{
	GPIO_InitTypeDef	GPIO_InitStruct;
	//Enable clock for I2C1
	RCC_APB1PeriphClockCmd(I2C_MASTER_RCC, ENABLE);

	//Enable clock for GPIOB
	RCC_AHB1PeriphClockCmd(I2C_MASTER_GPIO_RCC, ENABLE);

	//Alternate function mode
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;

	GPIO_InitStruct.GPIO_Pin = SCL_MASTER_PIN;
	GPIO_Init(I2C_MASTER_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin = SDA_MASTER_PIN;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	//config SCL and SDA for Alternate function mode
	GPIO_PinAFConfig(I2C_MASTER_PORT, GPIO_PinSource8, GPIO_AF_I2C1);
	GPIO_PinAFConfig(I2C_MASTER_PORT, GPIO_PinSource9, GPIO_AF_I2C1);
}

//Init I2C1 peripheral
void I2CMaster_Inits(void)
{
	I2C_InitTypeDef		I2C_InitStruct;
	I2C_InitStruct.I2C_ClockSpeed = I2C_SPEED;//FAST mode
	I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStruct.I2C_OwnAddress1 = 0x00;
	I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;
	//7 bits address
	I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;

	//Init I2C1 peripheral
	I2C_Init(I2C_MASTER_INSTANCE, &I2C_InitStruct);

	//Enable I2C1 peripheral
	I2C_Cmd(I2C_MASTER_INSTANCE, ENABLE);
}


//I2C_START condition
static void I2C_Start(void)
{
	//Wait until I2Cx is not busy
	while(I2C_GetFlagStatus(I2C_MASTER_INSTANCE, I2C_FLAG_BUSY));//??
    //Generate Start condition
	I2C_GenerateSTART(I2C_MASTER_INSTANCE, ENABLE);

	while(! I2C_CheckEvent(I2C_MASTER_INSTANCE, I2C_EVENT_MASTER_MODE_SELECT));


}

//I2C master send Address for Slave and r/w bit
//direction: SEND or RECEIVE
static void I2C_address_direction (uint8_t SlaveAddr, uint8_t direction)
{
	//left shift address for 1 bit r/w
	SlaveAddr = SlaveAddr << 1;
	if(direction == SEND)
	{
		//inform that master wants to write to slave
		I2C_Send7bitAddress(I2C_MASTER_INSTANCE, SlaveAddr, I2C_Direction_Transmitter);
		while(! I2C_CheckEvent(I2C_MASTER_INSTANCE, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	}
	else if (direction == RECEIVE)
	{
		//inform that master wants to read data from slave
		I2C_Send7bitAddress(I2C_MASTER_INSTANCE, SlaveAddr, I2C_Direction_Receiver);
		while(! I2C_CheckEvent(I2C_MASTER_INSTANCE, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
	}

}


//Send data to slave
static void I2C_Transmit(uint8_t Data)
{
	I2C_SendData(I2C_MASTER_INSTANCE, Data);

	while(! I2C_CheckEvent(I2C_MASTER_INSTANCE, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
}

//Generate stop condition
static void I2C_Stop(void)
{
	//Generate Start condition
	I2C_GenerateSTOP(I2C_MASTER_INSTANCE, ENABLE);

}


//receive data accomplish nack
uint8_t I2C_receive_nack(void)
{
	uint8_t data_receive = 0;

	I2C_AcknowledgeConfig(I2C_MASTER_INSTANCE, DISABLE);

	//Wait until I2Cx is not busy
//	while(I2C_GetFlagStatus(I2C_MASTER_INSTANCE, I2C_FLAG_BUSY));
	while(! I2C_CheckEvent(I2C_MASTER_INSTANCE, I2C_EVENT_MASTER_BYTE_RECEIVED));
	data_receive = I2C_ReceiveData(I2C_MASTER_INSTANCE);

	return data_receive;
}

//receive data accomplish ack
uint8_t I2C_receive_ack(void)
{
	uint8_t data_receive = 0;

	I2C_AcknowledgeConfig(I2C_MASTER_INSTANCE, ENABLE);

	//Wait until I2Cx is not busy
	while(! I2C_CheckEvent(I2C_MASTER_INSTANCE, I2C_EVENT_MASTER_BYTE_RECEIVED));

	data_receive = I2C_ReceiveData(I2C_MASTER_INSTANCE);

	return data_receive;
}


//Formula for calculating temperature
float CalculateTemp(uint16_t data)
{
	float result = (175.2*data)/65536-46.85;
	return result;
}

//Formula for calculating humidity
float CalculateHumi(uint16_t data)
{
	float result = 125.0*data/65536-6;
	return result;
}

//Read temperature or humidity from sensor
//slave address: 0x40
//cmi_id: 0xE3: get temperature,    0xE5: get humidity
//temOrhumi: TEMPERATURE or HUMIDITY
float TemHumSensor_readRegister(uint8_t slave_addr, uint8_t cmd_id, uint8_t tempOrhumi)//0xE3: get tempereature
{
	//data are got from sensor
	uint16_t data = 0;

	//result calculate from data
	float result = 0;

	//Genrate start condition
	I2C_Start();

	//send 7 bits address to slave and 1 bit write
	I2C_address_direction(slave_addr, SEND);

	//send cmd_id to slave
	I2C_Transmit(cmd_id);

	//Repeated start
	I2C_Stop();
	I2C_Start();

	//Send 7 bits address to slave and 1 bit read
	I2C_address_direction(slave_addr, RECEIVE);

	//Get MS byte
	data = (uint16_t)I2C_receive_ack();
	//Get LS byte
	data = (data << 8 ) | (uint16_t)I2C_receive_nack();

	//Indentify whether get humidity or temperature
	switch(tempOrhumi)
	{
		case TEMPERATURE:
			result = CalculateTemp(data);
			break;
		case HUMIDITY:
			result = CalculateHumi(data);
			break;
		default:
			break;
	}

	//Generate stop condition
	I2C_Stop();

	return result;
}

//Read Temperature
float ReadTemperature(void)
{
	return TemHumSensor_readRegister(0x40, 0xE3, TEMPERATURE);
}

//Read Humidity
float ReadHumidity(void)
{
	return TemHumSensor_readRegister(0x40, 0xE5, HUMIDITY);
}

//Calculate time
uint32_t CalculatorTime(uint32_t dwTimeInit, uint32_t dwTimeCurrent)
{
	uint32_t dwTimeTotal;

	if(dwTimeCurrent >= dwTimeInit)
	{
		dwTimeTotal = dwTimeCurrent - dwTimeInit;
	}
	else
	{
		dwTimeTotal = 0xFFFFFFFFU + dwTimeCurrent - dwTimeInit;
	}
	return dwTimeTotal;
}

uint32_t time_initial = 0;

//Buffer to convert float to string for LCD display
char bufTemp[5];
char bufHumi[5];

//including sensor information
Sensor sensorAfter;

//Init for Sensor
void InitSensor(void)
{

	sensorAfter.humi = 0;
	sensorAfter.temp = 0;
}

//Get sensor value every 5s period
//Change temperature display if the difference is more than 0.5oC
//Change humidity display if the difference is more than 1%
void processGetValueSensor(void)
{
	if(CalculatorTime(time_initial, GetMilSecTick()) >= 5000)//After 5s
	{
		//read Sensor data
		sensorAfter.humi = ReadHumidity();
		sensorAfter.temp = ReadTemperature();

		//humi max = 100%
		if(sensorAfter.humi > 100)
			sensorAfter.humi = 100;
		//humi min = 0%
		else if (sensorAfter.humi < 0) {
			sensorAfter.humi = 0;
		}

		//temperature max = 100 oC
		if(sensorAfter.temp > 100)
			sensorAfter.temp = 100;

		//Send humidity to PC
		HumiSensor_SendPacket(sensorAfter.humi);

		//Send Temperature to PC
		TempSensor_SendPacket(sensorAfter.temp);

		time_initial = GetMilSecTick();
	}
}

//Sendata to PC (for Tem_humiSensor use)
void SendDataToUsart(uint8_t option, uint8_t cmd_id, uint8_t cmd_type, float payload1, float payload2)
{
	uint8_t checkXor = CXOR_INIT_VAL;

	//option
	checkXor ^= option; //0x00

	//CMD_ID
	checkXor ^= cmd_id;//0x85

	//CMD_TYPE	RES(send data)
	checkXor ^= cmd_type;//0x01

	//Payload
	checkXor ^= (uint8_t)payload1;//0x00
	checkXor ^= (uint8_t)payload2;//data

	//FUNC_ID
	checkXor ^= FunctionId;

	uint8_t data[] = {0xB1, 0x07, option, cmd_id, cmd_type, payload1, payload2, FunctionId, checkXor};

	for(int i = 0; i <= 9; i++)
	{
		//send data
		USART_SendData(USART2, (uint16_t)data[i]);
		while( !USART_GetFlagStatus(USART2,  USART_FLAG_TXE));
	}
	//Increase func_id to 0xFE then reset
	FunctionId++;
	if(FunctionId > 0xFE)
	{
		FunctionId = 1;
	}
}

//Send Humidity value to PC
void HumiSensor_SendPacket(float humi)
{
	SendDataToUsart(OPTION, CMDID_HUMI, CMDTYPE_RES, PAYLOAD1, humi);
}

//Send Temperature value to PC
void TempSensor_SendPacket(float temp)
{
	SendDataToUsart(OPTION, CMDID_TEMP, CMDTYPE_RES, PAYLOAD1, temp);
}
//Init USART2
void USART2_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	USART_InitTypeDef USART_Initstruct;
	NVIC_InitTypeDef NVIC_InitStruct;

	/*
	 * Init TX for USART2
	 */

	//Enable GPIOClock for PA2
	RCC_AHB1PeriphClockCmd(USART2_GPIO_CLOCK, ENABLE);

	//Alternate function mode
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;

	//Clock speed
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

	//Output type push-pull
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;

	//Enable pull-up resistor
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;

	//PA2 TX pin
	GPIO_InitStruct.GPIO_Pin = USART2_TX | USART2_RX;
	GPIO_Init(USART2_GPIO, &GPIO_InitStruct);

	//Alternate function config
	GPIO_PinAFConfig(USART2_GPIO, GPIO_PinSource2, GPIO_AF_USART2);
	GPIO_PinAFConfig(USART2_GPIO, GPIO_PinSource3, GPIO_AF_USART2);

	//Enable clock for USART2 peripheral
	RCC_APB1PeriphClockCmd(USART2_CLOCK, ENABLE);

	//Enable USART2 peripheral
	USART_Cmd(USART2, ENABLE);

	USART_Initstruct.USART_BaudRate = USARTx_Buad;
	USART_Initstruct.USART_WordLength = USART_WordLength_8b;
	USART_Initstruct.USART_StopBits = USART_StopBits_1;
	USART_Initstruct.USART_Parity = USART_Parity_No;
	USART_Initstruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Initstruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2, &USART_Initstruct);

	USART_Cmd(USART2, ENABLE);

	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

	//enable interrupt
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
}

//Init usart and RX buffer
void Serial_Init(void)
{
	//Init buffer
	bufInit(pBuffDataRx, &serialQueueRx, sizeof(pBuffDataRx[0]), SIZE_QUEUE_DATA_RX);
	//Init uart2
	USART2_Init();
}

//Handle data from PC to device
uint8_t PollRxBuff(void)
{
	//Data get from buffer
	uint8_t byRxData;
	uint8_t byUartState = (uint8_t) UART_STATE_IDLE;

	while ((bufNumItems(&serialQueueRx) != 0)
			&& (byUartState == UART_STATE_IDLE))
	{
		//When receive data from PC

		//Get data in FIFO buffer
		bufDeDat(&serialQueueRx, &byRxData);
		switch (byRxBufState) {
		//if start byte
		case RX_STATE_START_BYTE:
			if (byRxData == FRAME_SOF)
			{
				byIndexRxBuf = 0;
				byCheckXorRxBuf = CXOR_INIT_VAL;
				byRxBufState = RX_STATE_DATA_BYTES;
			} else if (byRxData == FRAME_ACK) {
				byUartState = UART_STATE_ACK_RECEIVED;
			} else if (byRxData == FRAME_NACK)
			{
				byUartState = UART_STATE_NACK_RECEIVED;
			} else
			{
				byUartState = UART_STATE_ERROR;
			}
			break;
			//if data byte
		case RX_STATE_DATA_BYTES:
			if (byIndexRxBuf < RX_BUFFER_SIZE)
			{
				byRxBuffer[byIndexRxBuf] = byRxData;
				if (byIndexRxBuf > 0)
				{
					byCheckXorRxBuf ^= byRxData;
				}
				if (++byIndexRxBuf == *byRxBuffer)
				{
					byRxBufState = RX_STATE_CXOR_BYTE;
				}
			} else
			{
				byRxBufState = RX_STATE_START_BYTE;
				byUartState = UART_STATE_ERROR;
			}
			break;
			//if checkxor byte
		case RX_STATE_CXOR_BYTE: {
			if (byRxData == byCheckXorRxBuf)
			{
				byUartState = UART_STATE_DATA_RECEIVED;
			} else
			{
				byUartState = UART_STATE_ERROR;
			}
		default:
		byRxBufState = RX_STATE_START_BYTE;
		break;
		}

		}
	}
	return byUartState;
}

//Handle data get from PC
void processSerialReceiver(void)
{
	uint8_t stateRx = PollRxBuff();
	//if receive data
	if(stateRx == UART_STATE_DATA_RECEIVED)
	{
		switch(CMD_ID)
		{
			//0x01: LED control
			case LED_CONTROL:
				if(CMD_TYPE == 0x02)
				{
					//Left LED
					if(CMD_DATA_EPOINT == 0x02)
					{
						switch(CMD_DATA_STATE)
						{
							case 0x01:
								if(CMD_LAST_STATE != 0x04)
									{
										GPIO_WriteBit(LED1_GREEN_GPIO_PORT, LED1_GREEN_GPIO_PIN, 1);
									}
									else
									{
										GPIO_WriteBit(LED1_GREEN_GPIO_PORT, LED1_GREEN_GPIO_PIN, 0);
									}
								break;
							case 0x00:
								if(CMD_LAST_STATE != 0x04)
									{
										GPIO_WriteBit(LED1_RED_GPIO_PORT, LED1_RED_GPIO_PIN, 1);
									}
									else
									{
										GPIO_WriteBit(LED1_RED_GPIO_PORT, LED1_RED_GPIO_PIN, 0);
									}
								break;
							case 0x02:
								if(CMD_LAST_STATE != 0x04)
									{
										GPIO_WriteBit(LED1_BLUE_GPIO_PORT, LED1_BLUE_GPIO_PIN, 1);
									}
									else
									{
										GPIO_WriteBit(LED1_BLUE_GPIO_PORT, LED1_BLUE_GPIO_PIN, 0);
									}
								break;

						}
					}
					else
					//RIGRT LED
						if(CMD_DATA_EPOINT == 0x01)
						{

							switch(CMD_DATA_STATE)
							{
								case 0x01:
									if(CMD_LAST_STATE != 0x04)
										{
											GPIO_WriteBit(LED2_GREEN_GPIO_PORT, LED2_GREEN_GPIO_PIN, 1);
										}
										else
										{
											GPIO_WriteBit(LED2_GREEN_GPIO_PORT, LED2_GREEN_GPIO_PIN, 0);
										}
									break;
								case 0x00:
									if(CMD_LAST_STATE != 0x04)
										{
											GPIO_WriteBit(LED2_RED_GPIO_PORT, LED2_RED_GPIO_PIN, 1);
										}
										else
										{
											GPIO_WriteBit(LED2_RED_GPIO_PORT, LED2_RED_GPIO_PIN, 0);
										}
									break;
								case 0x02:
									if(CMD_LAST_STATE != 0x04)
										{
											GPIO_WriteBit(LED2_BLUE_GPIO_PORT, LED2_BLUE_GPIO_PIN, 1);
										}
										else
										{
											GPIO_WriteBit(LED2_BLUE_GPIO_PORT, LED2_BLUE_GPIO_PIN, 0);
										}
									break;

							}
					}
				}
				break;
			//Buzzer control
			case BUZZER_CONTROL:
				GPIO_WriteBit(BUZZER_GPIO_PORT, BUZZER_GPIO_PIN, 1);
				delay(200);
				GPIO_WriteBit(BUZZER_GPIO_PORT, BUZZER_GPIO_PIN, 0);
				break;

		}
		stateRx = 0x00;
	}
}

//Data get from PC
uint8_t receive_data = 0x00;
void USART2_IRQHandler(void)
{
	if(USART_GetITStatus(USART2, USART_IT_RXNE) == SET)
	{
		//take data
		receive_data = USART_ReceiveData(USART2);

		//push data into FIFO buffer
		bufEnDat(&serialQueueRx, (uint8_t*) &receive_data);
	}
}
void delay(uint32_t millisecond)
{
	for(uint32_t i = 0; i < millisecond; i++)
	{
		for(uint32_t j = 0; j < 5000; j++);
	}
}
//GPIO_Init for Buzzer and Led
void LedBuzz_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(BUZZERControl_SetClock, ENABLE);

	GPIO_InitStructure.GPIO_Pin = BUZZER_GPIO_PIN;

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_InitStructure.GPIO_OType =  GPIO_OType_PP;

	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;

	//Init Buzzer

	GPIO_Init(BUZZER_GPIO_PORT, &GPIO_InitStructure);

	//Init Led1 red

	GPIO_InitStructure.GPIO_Pin = LED1_RED_GPIO_PIN;

	RCC_AHB1PeriphClockCmd(LED1_REDControl_SetClock, ENABLE);

	GPIO_Init(LED1_RED_GPIO_PORT, &GPIO_InitStructure);

	//Init Led1 GREEN

	GPIO_InitStructure.GPIO_Pin = LED1_GREEN_GPIO_PIN;

	RCC_AHB1PeriphClockCmd(LED1_GREENControl_SetClock, ENABLE);

	GPIO_Init(LED1_GREEN_GPIO_PORT, &GPIO_InitStructure);

	//Init Led1 BLUE

	GPIO_InitStructure.GPIO_Pin = LED1_BLUE_GPIO_PIN;

	RCC_AHB1PeriphClockCmd(LED1_BLUEControl_SetClock, ENABLE);

	GPIO_Init(LED1_BLUE_GPIO_PORT, &GPIO_InitStructure);

	//Init Led2 BLUE

	GPIO_InitStructure.GPIO_Pin = LED2_BLUE_GPIO_PIN;

	RCC_AHB1PeriphClockCmd(LED2_BLUEControl_SetClock, ENABLE);

	GPIO_Init(LED2_BLUE_GPIO_PORT, &GPIO_InitStructure);

	//Init Led2 RED

	GPIO_InitStructure.GPIO_Pin = LED2_RED_GPIO_PIN;

	RCC_AHB1PeriphClockCmd(LED2_REDControl_SetClock, ENABLE);

	GPIO_Init(LED2_RED_GPIO_PORT, &GPIO_InitStructure);

	//Init Led2 RED

	GPIO_InitStructure.GPIO_Pin = LED2_GREEN_GPIO_PIN;

	RCC_AHB1PeriphClockCmd(LED2_GREENControl_SetClock, ENABLE);

	GPIO_Init(LED2_GREEN_GPIO_PORT, &GPIO_InitStructure);
}

int main(void)
{
	//Button Init
	Button_Init();
	Interrupt_Init();

	//Init sensor value
	InitSensor();

	//HSE clock 84 MHz
	SystemCoreClockUpdate();

	//Init timer
	TimerInit();

	//Init I2C
	GPIO_I2CMaster_Inits();
	I2CMaster_Inits();

	//Init time_initial
	time_initial = GetMilSecTick();

	//Init buzzer and Led
	LedBuzz_Init();

	//Init serial
	Serial_Init();
	while(1)
	{
		//Polling whether having data from PC send
		processSerialReceiver();

		//Polling data from temp-humi sensor
		processGetValueSensor();
	}
}











