/**
 ******************************************************************************
 * @file    isr.c
 * @author  Alen Helac
 * @version V1.0.0
 * @brief   Implementation of all Interrupt Service Routines (callbacks)
 * RTC clock.
 ******************************************************************************
 */

#include "isr.h"

/**
 * @brief Occurs hourly on Sundays, manages daylight savings time and led
 * @param  None
 * @retval None
 */
void RTC_IRQHandler(void)
{
	if(RTC_GetITStatus(RTC_IT_ALRA) != RESET)
	{
		RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
		RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);

		/*
		 * Not using @ref RTC_Month_Date_Definitions defgroup as it has errors in defines
		 * for example, RTC_Month_October should be 0x0A, not 0x10
		 */
		if (!(RTC_DateStructure.RTC_Month == 3 || RTC_DateStructure.RTC_Month == 10)) {
			RTC_ClearITPendingBit(RTC_IT_ALRA);
			EXTI_ClearITPendingBit(EXTI_Line17); printf("fire");
			return;
		}

		//at this point it's either March or October

		if (RTC_DateStructure.RTC_Date <= 24) {
			RTC_ClearITPendingBit(RTC_IT_ALRA);
			EXTI_ClearITPendingBit(EXTI_Line17);
			return;
		}

		//at this point we know that it's last Sunday in either March or October

		//now we compare if hour is right. hours are subtracted by one, because this interrupt is masked for x:59:59
		if (RTC_DateStructure.RTC_Month == 3 && RTC_TimeStructure.RTC_Hours == 1){
			//we should switch to summer time
			RTC_DayLightSavingConfig(RTC_DayLightSaving_ADD1H, RTC_StoreOperation_Set);
			AH_Discovery_Led_Action(AH_Discovery_Led4, AH_Led_Action_On);
		}
		else if (RTC_DateStructure.RTC_Month == 10 && RTC_TimeStructure.RTC_Hours == 2){
			//we should switch to winter time, and miss next winter time interrupt, otherwise this would occur hourly
			if (!skipNextWT) {
				skipNextWT = true;
				RTC_DayLightSavingConfig(RTC_DayLightSaving_SUB1H, RTC_StoreOperation_Reset);
				AH_Discovery_Led_Action(AH_Discovery_Led4, AH_Led_Action_Off);
			}
		}

		RTC_ClearITPendingBit(RTC_IT_ALRA);
		EXTI_ClearITPendingBit(EXTI_Line17);
	}
}


/**
  * @brief  This function handles External line 0 to 1 interrupt request. In our case it will be called every second
  * and it's responsible of blinking led3
  * and updating uart clock
  * @param  None
  * @retval None
  */
void EXTI4_15_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line13) != RESET)
	{
		timeShow();
		RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);

		if (RTC_TimeStructure.RTC_Seconds % 2 == 0) {
			AH_Discovery_Led_Action(AH_Discovery_Led3, AH_Led_Action_Off);
		}
		else {
			AH_Discovery_Led_Action(AH_Discovery_Led3, AH_Led_Action_On);
		}

		EXTI_ClearITPendingBit(EXTI_Line13);
	}
}
