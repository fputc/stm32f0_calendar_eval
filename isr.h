/**
 ******************************************************************************
 * @file    isr.h
 * @author  Alen Helac
 * @version V1.0.0
 * @brief   Declaration of all Interrupt Service Routines (callbacks)
 * RTC clock.
 ******************************************************************************
 */


#ifndef INC_ISR_H
#define INC_ISR_H

#include "main.h"

/**
 * @brief occurs hourly on Sundays, manages daylight savings time and led
 */
void RTC_IRQHandler(void);

/**
  * @brief  This function handles External line 0 to 1 interrupt request. In our case it's responsible of blinking led3
  * and updating uart clock
  * @param  None
  * @retval None
  */
void EXTI4_15_IRQHandler(void);

#endif /* INC_ISR_H */
