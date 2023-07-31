/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2019,逐飞科技
* All rights reserved.
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file             main
* @company          成都逐飞科技有限公司
* @author           逐飞科技(QQ3184284598)
* @version          查看doc内version文件 版本说明
* @Software         IAR 8.3 or MDK 5.24
* @Target core      MM32F3277
* @Taobao           https://seekfree.taobao.com/
* @date             2021-11-11
********************************************************************************************************************/

#include "zf_common_headfile.h"

#define MIC_1						B10
#define MIC_2						B11
#define MIC_3						B12

#define MIC_1_EXIT					EXTI15_10_IRQn
#define MIC_2_EXIT					EXTI15_10_IRQn
#define MIC_3_EXIT					EXTI15_10_IRQn

#define SERVO_PWM_BOTTOM			TIM2_PWM_CH1_A15							//下面舵机的PWM控制为A15
#define SERVO_PWM_TOP				TIM2_PWM_CH2_A1								//上面舵机的PWM控制为A1

static unsigned int MIC_TEMP_1 = 0;
static unsigned int MIC_TEMP_2 = 0;
static unsigned int MIC_TEMP_3 = 0;

static unsigned int MIC_FLAG = 0;

int main(void)
{	
// MM32系统时钟初始化
    clock_init(SYSTEM_CLOCK_120M);                                              // 初始化芯片时钟 工作频率为 120MHz	
    debug_init();                                                               // 初始化默认 Debug UART
	printf("\n\n\nClock&Debug OK!\n");
	
// 舵机初始化	    
	pwm_init(SERVO_PWM_BOTTOM,50,700);											// 初始化 SERVO 频率 50Hz 初始占空比 7.5 %	mid 700	left 580	right 820	
	pwm_init(SERVO_PWM_TOP,50,800);												// 初始化 SERVO 频率 50Hz 初始占空比 7.5 %	mid 715	left 715	right 715
	printf("SERVO OK!\n");
	system_delay_ms(1000);
	
// GPIO初始化
	gpio_init(B9,GPO,GPIO_HIGH,GPO_PUSH_PULL);
	gpio_set_level(B9,0);
	system_delay_ms(1000);
	
// 中断初始化
	exti_init(MIC_1, EXTI_TRIGGER_FALLING);										//外部中断输入 下降沿触发
	exti_init(MIC_2, EXTI_TRIGGER_FALLING);										//外部中断输入 下降沿触发
	exti_init(MIC_3, EXTI_TRIGGER_FALLING);										//外部中断输入 下降沿触发
	
	interrupt_set_priority(MIC_1_EXIT,0);
	interrupt_set_priority(MIC_2_EXIT,1);
	interrupt_set_priority(MIC_3_EXIT,1);
	while(1)
	{
		if( MIC_TEMP_2 != 0 && MIC_TEMP_3 != 0)
		{
			interrupt_disable(EXTI15_10_IRQn);
			timer_stop(TIM_1);
			timer_clear(TIM_1);
			if(MIC_TEMP_2 > MIC_TEMP_3)
				pwm_init(SERVO_PWM_BOTTOM,50,580);
			if(MIC_TEMP_2 < MIC_TEMP_3)
				pwm_init(SERVO_PWM_BOTTOM,50,820);
		}
		printf("MIC:%d,%d,%d\n",MIC_TEMP_1,MIC_TEMP_2,MIC_TEMP_3);
		MIC_TEMP_1 = 0;
		MIC_TEMP_2 = 0;
		MIC_TEMP_3 = 0;
		system_delay_ms(100);
		interrupt_enable(EXTI15_10_IRQn);
	}
}

void mic_1_exti_handler(void)
{
	timer_start(TIM_1,TIMER_SYSTEM_CLOCK);
	MIC_FLAG++;
}

void mic_2_exti_handler(void)
{	
	if(MIC_FLAG)
		MIC_TEMP_2 = timer_get(TIM_1);
}
void mic_3_exti_handler(void)
{
	if(MIC_FLAG)
		MIC_TEMP_3 = timer_get(TIM_1);
}

