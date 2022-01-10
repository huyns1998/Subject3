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

#include "stm32f401re_rcc.h"
#include "stm32f401re_gpio.h"
#include "stm32f401re_adc.h"
#include "stm32f401re_usart.h"
#include "stm32f401re_tim.h"
#include "timer.h"
//#include <math.h>

#define ADC_GPIO					GPIOC
#define ADC_GPIO_CLK				RCC_AHB1Periph_GPIOC
#define ADCx_SENSOR					ADC1
#define ADCx_CLK					RCC_APB2Periph_ADC1
#define ADC_GPIO_PIN				GPIO_Pin_5

#define LED1_RED_ID							1
#define LED1_RED_GPIO_PORT					GPIOA
#define LED1_RED_GPIO_PIN					GPIO_Pin_1
#define LED1_RED_PIN						1
#define LED1_REDControl_SetClock			RCC_AHB1Periph_GPIOA

#define TIM2_CLK							RCC_APB1Periph_TIM2
#define Tim_Period							8399

//Kalman filter------------------------------------------------------------

static float _err_measure;
static float _err_estimate;
static float _q;
static float _current_estimate;
static float _last_estimate;
static float _kalman_gain;
/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/
/******************************************************************************/
/*                            PRIVATE FUNCTIONS                               */
/******************************************************************************/
/******************************************************************************/
/*                            EXPORTED FUNCTIONS                              */
/******************************************************************************/

void KalmanFilterInit(float mea_e, float est_e, float q);
float KalmanFilter_updateEstimate(float mea);
void KalmanFilter_setMeasurementError(float mea_e);
void KalmanFilter_setEstimateError(float est_e);
void KalmanFilter_setProcessNoise(float q);
float KalmanFilter_getKalmanGain(void);
float KalmanFilter_getEstimateError(void);



//------------------------------------------------------------


void LightSensor_AdcInit(void);
void LedControl_TimerOCInit(void);
static void LedControl_PWM(uint16_t duty_cycle);
void delay(uint32_t ms);
uint16_t LightSensor_AdcPollingRead(void);
void ABL_StepBrightness(uint16_t light_value);
uint32_t CalculatorTime(uint32_t dwTimeInit, uint32_t dwTimeCurrent);

uint32_t time_initial = 0;

//Init timer2 as output comapre mode
void LedControl_TimerOCInit(void)
{
	GPIO_InitTypeDef 				GPIO_InitStruct;
	TIM_TimeBaseInitTypeDef			TIM_TimeBaseInitStruct;
	TIM_OCInitTypeDef				TIM_OCInitStruct;

	//Enable clock for GPOIA
	RCC_AHB1PeriphClockCmd(LED1_REDControl_SetClock, ENABLE);

	//Alternate function mode
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;

	//Medium speed
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

	//output type push-pull
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;

	//Config AP1 as alternate function of timer2
	GPIO_PinAFConfig(LED1_RED_GPIO_PORT, GPIO_PinSource1, GPIO_AF_TIM2);

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
	GPIO_Init(LED1_RED_GPIO_PORT, &GPIO_InitStruct);

	//Enable clock for timer2 peripheral
	RCC_APB1PeriphClockCmd(TIM2_CLK, ENABLE);

	// Clock div 1
	TIM_TimeBaseInitStruct.TIM_Prescaler = 0;

	//Counter up
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;

	//Period = clock / 8400
	TIM_TimeBaseInitStruct.TIM_Period = 8399;

	//Clock div 1
	TIM_TimeBaseInitStruct.TIM_ClockDivision = 0;

	//
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;

	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);

	//Output captrure mode2
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM2;
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStruct.TIM_Pulse = 0;
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_Low;

	TIM_OC2Init(TIM2, &TIM_OCInitStruct);

	TIM_Cmd(TIM2, ENABLE);
	TIM_CtrlPWMOutputs(TIM2, ENABLE);
}



//Control LED
static void LedControl_PWM(uint16_t duty_cycle)
{
	static uint16_t pulse_length = 0;

	pulse_length = (Tim_Period * duty_cycle)/100;

	TIM_SetCompare2(TIM2, pulse_length);
}

//Init ADC to read light sensor
void LightSensor_AdcInit(void)
{
	ADC_InitTypeDef			ADC_InitStruct;
	ADC_CommonInitTypeDef	ADC_CommonInitStruct;
	GPIO_InitTypeDef		GPIO_InitStruct;

	//-----------------GPIOC Init
	//Enable clock for GPIOC
	RCC_AHB1PeriphClockCmd(ADC_GPIO_CLK, ENABLE);

	//Pin 5
	GPIO_InitStruct.GPIO_Pin = ADC_GPIO_PIN;

	//Alternate function mode analog
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;

	//Not use pull-up resistor
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(ADC_GPIO, &GPIO_InitStruct);

	//reset ADC
	ADC_DeInit();

	//Enable clock for ADC1
	RCC_APB2PeriphClockCmd(ADCx_CLK, ENABLE);

	//-----------------ADC Common Init

	//Independent mode
	ADC_CommonInitStruct.ADC_Mode = ADC_Mode_Independent;

	//Clock /2
	ADC_CommonInitStruct.ADC_Prescaler = ADC_Prescaler_Div2;

	//Disable DMA
	ADC_CommonInitStruct.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;

	//5 cycle for 2 sampling time
	ADC_CommonInitStruct.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;

	//Update into corresponding register
	ADC_CommonInit(&ADC_CommonInitStruct);

	//----------------ADC advanced init

	//12 bits resolution
	ADC_InitStruct.ADC_Resolution = ADC_Resolution_12b;

	//1 channel conversion
	ADC_InitStruct.ADC_ScanConvMode = DISABLE;

	//Continuous conversion
	ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;

	ADC_InitStruct.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;


	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;

	//LSB bit is sorted right
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;

	//1 channel is used
	ADC_InitStruct.ADC_NbrOfConversion = 16;
	ADC_Init(ADCx_SENSOR, &ADC_InitStruct);


	ADC_EOCOnEachRegularChannelCmd(ADCx_SENSOR, DISABLE);
//	ADC_ContinuousModeCmd(ADCx_SENSOR, DISABLE);	//:D. Đoạn này thì lại disable continus
//	ADC_DiscModeChannelCountConfig(ADCx_SENSOR, 16);
//	ADC_DiscModeCmd(ADCx_SENSOR, ENABLE);
	//144 cycles each for each sample
	ADC_RegularChannelConfig(ADCx_SENSOR, ADC_Channel_15, 1, ADC_SampleTime_15Cycles);

	//Enalbe ADC1 peripheral
	ADC_Cmd(ADCx_SENSOR, ENABLE);


}


uint16_t LightSensor_AdcPollingRead(void)
{
	//read light value from sensor
	uint16_t mea = ADC_GetConversionValue(ADCx_SENSOR);

	//calculate brightness by kalman filter
//	uint16_t result = KalmanFilter_updateEstimate(mea);

	return mea;
}

//Control led brightness via light value
void ABL_StepBrightness(uint16_t light_value)
{
	if(light_value >= 800)
	{
		//turn off LED
		LedControl_PWM(0);
	}
	else if (light_value >= 600)
	{
		//led brightness = 20%
		LedControl_PWM(20);
	}
	else if (light_value >= 400) {

		//led brightness = 50%
		LedControl_PWM(50);
	}
	else if (light_value >= 200) {

		//led brightness = 70%
		LedControl_PWM(70);
	}
	else {

		//led brightness = max
		LedControl_PWM(100);
	}
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

void ABL_Process(void)
{
	if(CalculatorTime(time_initial, GetMilSecTick()) >= 100)//After 5s
	{
		//Control led brightness via light value
		ABL_StepBrightness(LightSensor_AdcPollingRead());

		//Update time_initial
		time_initial = GetMilSecTick();
	}

}

int main(void)
{
	//Init system clock 84MHz
	SystemCoreClockUpdate();

	//Init timer
	TimerInit();

	//Init timer2 s output compare mode
	LedControl_TimerOCInit();

	//init ADC1 channel 15
	LightSensor_AdcInit();

	//Start ADC conversion
	ADC_SoftwareStartConv(ADCx_SENSOR);

	//Init time_initial
	time_initial = GetMilSecTick();
	while(1)
	{
		processTimerScheduler();

		//Calculate led brightness
		ABL_Process();
	}
}


void KalmanFilterInit(float mea_e, float est_e, float q)
{
  _err_measure=mea_e;
  _err_estimate=est_e;
  _q = q;
}

float KalmanFilter_updateEstimate(float mea)
{
  _kalman_gain = _err_estimate/(_err_estimate + _err_measure);
  _current_estimate = _last_estimate + _kalman_gain * (mea - _last_estimate);
  _err_estimate =  (1.0 - _kalman_gain)*_err_estimate + fabs(_last_estimate-_current_estimate)*_q;
  _last_estimate=_current_estimate;

  return _current_estimate;
}

void KalmanFilter_setMeasurementError(float mea_e)
{
  _err_measure=mea_e;
}

void KalmanFilter_setEstimateError(float est_e)
{
  _err_estimate=est_e;
}

void KalmanFilter_setProcessNoise(float q)
{
  _q=q;
}

float KalmanFilter_getKalmanGain(void) {
  return _kalman_gain;
}

float KalmanFilter_getEstimateError(void) {
  return _err_estimate;
}


void delay(uint32_t ms)
{
	for(uint32_t i = 0; i < ms; i++)
	{
		for(uint32_t j = 0; j < 5000; j++);
	}
}











