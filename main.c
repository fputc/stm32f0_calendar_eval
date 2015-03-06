#include "stm32f0xx.h"
#include "stm32f0xx_pwr.h"
#include "stm32f0xx_gpio.h"
#include "stm32f0xx_exti.h"
#include "stm32f0xx_misc.h"
#include "stm32f0xx_rcc.h"
#include "stm32f0xx_rtc.h"
#include "stm32f0xx_syscfg.h"
#include "stm32f0_ah_discovery.h"
#include "stm32f0xx_ah_delay.h"
#include "stm32f0xx_ah_rtc.h"
#include "datetime_build_defs.h"
#include <stdio.h>
#include <time.h>


RTC_InitTypeDef RTC_InitStructure;
RTC_AlarmTypeDef  RTC_AlarmStructure;
GPIO_InitTypeDef   GPIO_InitStructure;
uint32_t AsynchPrediv = 0, SynchPrediv = 0;

NVIC_InitTypeDef  NVIC_InitStructure;
EXTI_InitTypeDef  EXTI_InitStructure;

/**
  * @brief  Configure the RTC peripheral by selecting the clock source.
  * @param  None
  * @retval None
  */
void RTC_Config(void)
{
  /* Enable the PWR clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

  /* Allow access to RTC */
  PWR_BackupAccessCmd(ENABLE);

  /* Enable the LSI OSC */
  RCC_LSICmd(ENABLE);

  /* Wait till LSI is ready */
  while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET) {}

  /* Select the RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);

  /*Dividers calculated for 40KHz*/
  AsynchPrediv = 124;
  SynchPrediv = 319;

  /* Enable the RTC Clock */
  RCC_RTCCLKCmd(ENABLE);

  /* Wait for RTC APB registers synchronization */
  RTC_WaitForSynchro();

  /* Configure RTC to produce 1Hz on RTC_OUT (PC13) pin */
  RTC_CalibOutputConfig(RTC_CalibOutput_1Hz);
  RTC_CalibOutputCmd(ENABLE);

  RTC_InitStructure.RTC_AsynchPrediv = AsynchPrediv;
	RTC_InitStructure.RTC_SynchPrediv = SynchPrediv;
	RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;

  	if (RTC_Init(&RTC_InitStructure) == ERROR)
  	{
  	  printf("Prescaler configuration failed \n\r");
  	}
}


/**
  * @brief  Display the current time on the Hyperterminal.
  * @param  None
  * @retval None
  */
void RTC_TimeShow(void)
{
  /* Get the current Time */
  RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
  RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);
  printf("\n\r  %d.%d.%d Weekday: %d  ",RTC_DateStructure.RTC_Date, RTC_DateStructure.RTC_Month, RTC_DateStructure.RTC_Year, RTC_DateStructure.RTC_WeekDay);
  printf(" %d:%d:%d \n\r",RTC_TimeStructure.RTC_Hours, RTC_TimeStructure.RTC_Minutes, RTC_TimeStructure.RTC_Seconds);
}



void AH_RTC_SetBuildTime(void) {

	bool success = true;
	RTC_TimeStructInit(&RTC_TimeStructure);
	RTC_DateStructInit(&RTC_DateStructure);

	RTC_GetBuildTime(&RTC_TimeStructure);
	RTC_GetBuildDate(&RTC_DateStructure);

	//Configure the RTC time register
	if(!(RTC_SetTime(RTC_Format_BIN, &RTC_TimeStructure)))
	{
		success = false;
	}
	else
	{
		printf("\n\r>> !! RTC Set Time success. !! <<\n\r");/*
		RTC_TimeShow();
		// Indicator for the RTC configuration
		//RTC_WriteBackupRegister(RTC_BKP_DR0, BKP_VALUE); */
	}

	//Configure the RTC time register
	if(!(RTC_SetDate(RTC_Format_BIN, &RTC_DateStructure)))
	{
		printf("\n\r>> !! RTC Set date failed. !! <<\n\r");
	}
	else
	{
		printf("\n\r>> !! RTC Set date success. !! <<\n\r");/*
		RTC_TimeShow();
		// Indicator for the RTC configuration
		//RTC_WriteBackupRegister(RTC_BKP_DR0, BKP_VALUE); */
	}
}

void configureAlarm (void)
{
	/* EXTI configuration *******************************************************/
	EXTI_ClearITPendingBit(EXTI_Line17);
	EXTI_InitStructure.EXTI_Line = EXTI_Line17;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* Enable the RTC Wakeup Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//activate alarm hourly on Sunday
	RTC_TimeStructInit(&RTC_TimeStructure);
	RTC_TimeStructure.RTC_Minutes = 59;
	RTC_TimeStructure.RTC_Seconds = 59;
	RTC_AlarmStructure.RTC_AlarmTime = RTC_TimeStructure;
	RTC_AlarmStructure.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_WeekDay;
	RTC_AlarmStructure.RTC_AlarmDateWeekDay = RTC_Weekday_Sunday;
	RTC_AlarmStructure.RTC_AlarmMask = RTC_AlarmMask_Hours;

	RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmStructure);

	RTC_ITConfig(RTC_IT_ALRA, ENABLE);
	RTC_AlarmCmd(RTC_Alarm_A, ENABLE);
}







void RTC_IRQHandler(void)
{printf("\n\rALAAAAAAARM\n\r");
	if(RTC_GetITStatus(RTC_IT_ALRA) != RESET)
	{

		AH_Discovery_Led_Action(AH_Discovery_Led4, AH_Led_Action_Toggle);

		RTC_ClearITPendingBit(RTC_IT_ALRA);
		EXTI_ClearITPendingBit(EXTI_Line17);
	}
}



/**
  * @brief  This function handles External line 0 to 1 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI4_15_IRQHandler(void)
{  RTC_TimeShow();
  if(EXTI_GetITStatus(EXTI_Line13) != RESET)
  {
		RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);

		if (RTC_TimeStructure.RTC_Seconds % 2 == 0) {
			AH_Discovery_Led_Action(AH_Discovery_Led3, AH_Led_Action_Off);
		}
		else {
			AH_Discovery_Led_Action(AH_Discovery_Led3, AH_Led_Action_On);
		}

    /* Clear the EXTI line 0 pending bit */
    EXTI_ClearITPendingBit(EXTI_Line13);
  }
}

void configure1HzInterupt(void)
{
  /* Enable GPIOc clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);

  /* Configure PA0 pin as input floating */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  /* Enable SYSCFG clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  /* Connect EXTI13 Line to PA13 pin */
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource13);

  /* Configure EXTI13 line */
  EXTI_InitStructure.EXTI_Line = EXTI_Line13;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Enable and set EXTI0 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI4_15_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}


int main(void)
{
	//board init
	AH_DELAY_SysTickInit();
	AH_Discovery_Led_Init(AH_Discovery_Led3);
	AH_Discovery_Led_Init(AH_Discovery_Led4);
	AH_Discovery_Debug_Init(9600);

	//output header
	printf("=================================================\n\r");
	printf(">> STM32F0-DISCOVERY RTC Evaluation by Alen H. <<\n\r");
	printf("=================================================\n\r");

	RTC_Config();
	AH_RTC_SetBuildTime();

	//options
	printf("Time set to build time.");

	configureAlarm();
	configure1HzInterupt();

	if (AH_RTC_IsDaylightSavingActive() == true) {
		AH_Discovery_Led_Action(AH_Discovery_Led4, AH_Led_Action_On);
	}


	/*
	AH_DELAY_DelayMs(5000);
	RTC_DayLightSavingConfig(RTC_DayLightSaving_ADD1H, RTC_StoreOperation_Set);

	AH_DELAY_DelayMs(5000);
		RTC_DayLightSavingConfig(RTC_DayLightSaving_SUB1H, RTC_StoreOperation_Reset);
	*/

	RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
	RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);

	const time_t ts = AH_RTC_GetDaylightSavingsStart(15);
	const time_t te = AH_RTC_GetDaylightSavingsEnd(15);

	printf("Daylight savings start at %s", ctime(&ts));
	printf("\n\r Daylight savings end at %s", ctime(&te));
    while(1)
    {
    	if (AH_RTC_IsDaylightSavingActive() == true) {
    			AH_Discovery_Led_Action(AH_Discovery_Led4, AH_Led_Action_On);
    		}
    }
}
