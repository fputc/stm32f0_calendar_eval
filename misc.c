/**
 ******************************************************************************
 * @file    misc.h
 * @author  Alen Helac
 * @version V1.0.0
 * @brief   implementation of miscellaneous functions used across this
 * application.
 * RTC clock.
 ******************************************************************************
 */

#include "misc.h"

/**
  * @brief  Display the current date and time on the hyperterminal.
  * @param  None
  * @return None
  */
void timeShow(void)
{
	RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
	RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);
	printf("\r%d.%d.%d %s ",RTC_DateStructure.RTC_Date, RTC_DateStructure.RTC_Month, RTC_DateStructure.RTC_Year, weekdayEnName[RTC_DateStructure.RTC_WeekDay]);
	printf("%d:%d:%d   ",RTC_TimeStructure.RTC_Hours, RTC_TimeStructure.RTC_Minutes, RTC_TimeStructure.RTC_Seconds);
}

/**
 * @brief configures alarm that is needed for daylight savings operation.
 * Fires interrupt hourly on Sunday.
 * @param None
 * @retval None
 */
void configureAlarm (void)
{
	// EXTI configuration
	EXTI_ClearITPendingBit(EXTI_Line17);
	EXTI_InitStructure.EXTI_Line = EXTI_Line17;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	//Enable the RTC Wakeup Interrupt
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//activate alarm hourly on Sunday, every x:59:59
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

/**
 * @brief Configures and 1Hz interrupt. This is accomplished by configuring
 * RTC to produce 1Hz on RTC_OUT pin. Afterwards, we set configure interrupt on
 * that same pin.
 * @param None
 * @retval None
 */
void configure1HzInterupt(void) {
	//Configure RTC to produce 1Hz on RTC_OUT (PC13) pin
	RTC_CalibOutputConfig(RTC_CalibOutput_1Hz);
	RTC_CalibOutputCmd(ENABLE);

	//Enable GPIOC clock
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);

	//Configure PC13 pin as input
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	//Enable SYSCFG clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	//Connect EXTI13 Line to PC13 pin
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource13);

	//Configure EXTI13 line
	EXTI_InitStructure.EXTI_Line = EXTI_Line13;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	//Enable and set EXTI13 Interrupt
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_15_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/**
 * @brief sets RTC time to 5 secs before summer time
 * @note for test purposes only
 * @param none
 * @return bool success
 */
bool setRtcJustBeforeST(void) {

	RTC_TimeStructInit(&RTC_TimeStructure);
	RTC_DateStructInit(&RTC_DateStructure);

	RTC_TimeStructure.RTC_Hours = 1;
	RTC_TimeStructure.RTC_Minutes = 59;
	RTC_TimeStructure.RTC_Seconds = 55;

	RTC_DateStructure.RTC_Date = 29;
	RTC_DateStructure.RTC_Month = 3;
	RTC_DateStructure.RTC_Year = 15;
	RTC_DateStructure.RTC_WeekDay = AH_RTC_GetWeekdayForDate(29, 3, 15);

	return RTC_SetTime(RTC_Format_BIN, &RTC_TimeStructure) && RTC_SetDate(RTC_Format_BIN, &RTC_DateStructure);
}

/**
 * @brief sets RTC time to 5 secs before winter time
 * @note for test purposes only
 * @param none
 * @return bool success
 */
bool setRtcJustBeforeWT(void) {

	RTC_TimeStructInit(&RTC_TimeStructure);
	RTC_DateStructInit(&RTC_DateStructure);

	RTC_TimeStructure.RTC_Hours = 2;
	RTC_TimeStructure.RTC_Minutes = 59;
	RTC_TimeStructure.RTC_Seconds = 55;

	RTC_DateStructure.RTC_Date = 25;
	RTC_DateStructure.RTC_Month = 10;
	RTC_DateStructure.RTC_Year = 15;
	RTC_DateStructure.RTC_WeekDay = AH_RTC_GetWeekdayForDate(25, 10, 15);

	return RTC_SetTime(RTC_Format_BIN, &RTC_TimeStructure) && RTC_SetDate(RTC_Format_BIN, &RTC_DateStructure);
}

/**
 * @brief Performs number of init functions.
 * @param none
 * @return none
 */
void boardInit(void) {
	AH_DELAY_SysTickInit();
	AH_Discovery_Led_Init(AH_Discovery_Led3);
	AH_Discovery_Led_Init(AH_Discovery_Led4);
	AH_Discovery_Debug_Init(9600);
	AH_RTC_Init();
}

/**
 * @brief Prints custom header on hyperterminal.
 * @param None
 * @return None
 */
void printHeader(void) {
	printf("\n\r\n=================================================");
	printf("\n\r>> STM32F0-DISCOVERY RTC Evaluation by Alen H. <<");
	printf("\n\r=================================================\n\r");
};

/**
 * @brief Prints date and time header on hyperterminal.
 * @param None
 * @return None
 */
void printDateTimeHeader(void) {
	printf("\n\r----- Date and time -----------------------------\n\r");
}

/**
 * @brief Display what time configuration options are available. Prompt user to input
 * desired option. Defaults to option 1 if timeout of 5 seconds is exceeded. Defaults
 * to option 1 if input character is invalid.
 * @param None
 * @return AH_Time_Init_Option_Typedef
 */
AH_Time_Init_Option_Typedef promptTimeOptions(void) {
	char chrReceived;

	printf("Available time setting preferences:\n\r");
	printf("1. Set to time upon project is built \n\r");
	printf("2. Five seconds before summer time begins\n\r");
	printf("3. Five seconds before winter time begins\n\r");
	printf("\n\rSelect 1, 2 or 3 [default: 1, timeout 5s]: ");

	AH_DELAY_StartTick();

	while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET && AH_DELAY_GetTickTime() < 5000);
	chrReceived = (char)USART_ReceiveData(USART1);

	AH_DELAY_StopTick();


	printf("%c \n\r", chrReceived);

	switch (chrReceived) {
		case '1':
			printf("You've chosen option 1...");
			return AH_Build_Time;

		case '2':
			printf("You've chosen option 2...");
			return AH_Just_Before_Summer_Time;

		case '3':
			printf("You've chosen option 3...");
			return AH_Just_Before_Winter_Time;

		case '\0':
			printf("\r\nTime out, defaulted to option 1...");
			return AH_Build_Time;

		default:
			printf("Invalid input, defaulted to option 1...\n\r");
			return AH_Build_Time;
	}
}
