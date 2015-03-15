#ifndef INC_MAIN_H
#define INC_MAIN_H

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
#include "misc.h"
#include <stdio.h>
#include <time.h>

extern RTC_InitTypeDef RTC_InitStructure;
extern RTC_AlarmTypeDef  RTC_AlarmStructure;
extern GPIO_InitTypeDef   GPIO_InitStructure;

extern NVIC_InitTypeDef  NVIC_InitStructure;
extern EXTI_InitTypeDef  EXTI_InitStructure;

extern volatile bool skipNextWT;

#endif /* INC_MAIN_H */
