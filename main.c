#include "main.h"

RTC_InitTypeDef RTC_InitStructure;
RTC_AlarmTypeDef RTC_AlarmStructure;
GPIO_InitTypeDef GPIO_InitStructure;

NVIC_InitTypeDef NVIC_InitStructure;
EXTI_InitTypeDef EXTI_InitStructure;

volatile bool skipNextWT = false;

int main(void)
{
	bool timeInitSuccess = false;
	AH_Time_Init_Option_Typedef option;

	boardInit();
	printHeader();

	//ask user what option for time source he'd prefer
	option = promptTimeOptions();

	//set time accordingly
	switch (option) {
		case AH_Just_Before_Summer_Time:
			timeInitSuccess = setRtcJustBeforeST();
			break;

		case AH_Just_Before_Winter_Time:
			timeInitSuccess = setRtcJustBeforeWT();
			break;

		case AH_Build_Time:
			timeInitSuccess = AH_RTC_SetBuildTime();
			break;
	}

	//display init status, loop forever if failed
	if (timeInitSuccess) {
		printf("success.");
		printDateTimeHeader();
	}
	else {
		printf("failed.");
		while(1);
	}

	// once time is set, determine if daylight savings is active, set led accordingly
	if (AH_RTC_IsDaylightSavingActive() == true) {
		AH_Discovery_Led_Action(AH_Discovery_Led4, AH_Led_Action_On);
	}

	//configure interrupts
	configureAlarm();
	configure1HzInterupt();

	//rest of the process is handled by ISRs, implemented in isr.c
	while(1);
}
