#include "stm32f0xx.h"
#include "stm32f0xx_pwr.h"
#include "stm32f0xx_gpio.h"
#include "stm32f0xx_exti.h"
#include "stm32f0xx_misc.h"
#include "stm32f0xx_rcc.h"
#include "stm32f0xx_rtc.h"
#include "stm32f0_ah_discovery.h"
#include "stm32f0xx_ah_delay.h"

#include <stdio.h>



RTC_TimeTypeDef RTC_TimeStructure;
RTC_InitTypeDef RTC_InitStructure;
RTC_AlarmTypeDef  RTC_AlarmStructure;
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

  /* Enable the LSE OSC */
  RCC_LSICmd(ENABLE);

  /* Wait till LSE is ready */
  while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
  {
  }

  /* Select the RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);


  AsynchPrediv = 124;
  SynchPrediv = 319;

  /* Enable the RTC Clock */
  RCC_RTCCLKCmd(ENABLE);

  /* Wait for RTC APB registers synchronisation */
  RTC_WaitForSynchro();
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
  printf("\n\r  The current time is :  %d:%d:%d \n\r", RTC_TimeStructure.RTC_Hours, RTC_TimeStructure.RTC_Minutes, RTC_TimeStructure.RTC_Seconds);
}


void sanityTest (void)
{
	static const char string[] = "We'll set the clock on \n\r";
	uint32_t hours = 0;
	uint32_t minutes = 0;
	uint32_t seconds = 5;
	printf("%s", string);
	printf("Hours: %d \n\r", hours);
	printf("Minutes: %d \n\r", minutes);
	printf("Seconds: %d \n\r", seconds);


	RTC_InitStructure.RTC_AsynchPrediv = AsynchPrediv;
	RTC_InitStructure.RTC_SynchPrediv = SynchPrediv;
	RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;

	if (RTC_Init(&RTC_InitStructure) == ERROR)
	{
	  printf("Prescaler configuration failed \n\r");
	}

	RTC_TimeStructInit(&RTC_TimeStructure);

	//Configure the RTC time register
	if(RTC_SetTime(RTC_Format_BIN, &RTC_TimeStructure) == ERROR)
	{
		printf("\n\r>> !! RTC Set Time failed. !! <<\n\r");
	}
	else
	{
		printf("\n\r>> !! RTC Set Time success. !! <<\n\r");/*
		RTC_TimeShow();
		// Indicator for the RTC configuration
		//RTC_WriteBackupRegister(RTC_BKP_DR0, BKP_VALUE); */
	}


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

	    /* Set the alarm X+5s */
	  RTC_AlarmStructure.RTC_AlarmTime.RTC_H12     = RTC_H12_AM;
	  RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours   = 0x00;
	  RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes = 0x00;
	  RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds = 0x01;
	  RTC_AlarmStructure.RTC_AlarmDateWeekDay = 0x31;
	  RTC_AlarmStructure.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_Date;
	  RTC_AlarmStructure.RTC_AlarmMask = RTC_AlarmMask_DateWeekDay;
	  RTC_SetAlarm(RTC_Format_BCD, RTC_Alarm_A, &RTC_AlarmStructure);

	  RTC_ITConfig(RTC_IT_ALRA, ENABLE);

	  /* Enable the alarm */
	  RTC_AlarmCmd(RTC_Alarm_A, ENABLE);

}


/**
  * @brief  This function handles RTC interrupt request.
  * @param  None
  * @retval None
  */
void RTC_IRQHandler(void)
{
	RTC_TimeShow();
  if(RTC_GetITStatus(RTC_IT_ALRA) != RESET)
  {
  	RTC_TimeShow();
    RTC_ClearITPendingBit(RTC_IT_ALRA);
    EXTI_ClearITPendingBit(EXTI_Line17);
  }
}

int main(void)
{
	static const char bdt[] = "\n\rBuild date and time: " __DATE__ " " __TIME__ "\n\r";
	AH_DELAY_SysTickInit();
	AH_Discovery_Debug_Init(9600);
	printf("%s", bdt);

	RTC_Config();
	sanityTest();



    while(1)
    {
    }
}
