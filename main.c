#include "stm32f0xx.h"
#include "stm32f0xx_gpio.h"
#include "stm32f0xx_rcc.h"
#include "stm32f0xx_ah_delay.h"
#include <stdio.h>

int main(void)
{
	AH_DELAY_SysTickInit();

    while(1)
    {
    	AH_DELAY_DelayMs(1000);
    }
}
