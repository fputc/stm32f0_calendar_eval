#ifndef INC_MISC_H
#define INC_MISC_H

#include "main.h"

typedef enum {
	AH_Just_Before_Summer_Time,
	AH_Just_Before_Winter_Time,
	AH_Build_Time
}AH_Time_Init_Option_Typedef;

/**
  * @brief  Display the current date and time on the hyperterminal.
  * @param  None
  * @return None
  */
void timeShow(void);

/**
 * @brief configures alarm that is needed for daylight savings operation.
 * Fires interrupt hourly on Sunday.
 * @param None
 * @retval None
 */
void configureAlarm (void);

/**
 * @brief Configures and 1Hz interrupt. This is accomplished by configuring
 * RTC to produce 1Hz on RTC_OUT pin. Afterwards, we set configure interrupt on
 * that same pin.
 * @param None
 * @retval None
 */
void configure1HzInterupt(void);

/**
 * @brief sets RTC time to 5 secs before summer time
 * @note for test purposes only
 * @param none
 * @return bool success
 */
bool setRtcJustBeforeST(void);

/**
 * @brief sets RTC time to 5 secs before winter time
 * @note for test purposes only
 * @param none
 * @return bool success
 */
bool setRtcJustBeforeWT(void);

/**
 * @brief Performs number of init functions.
 * @param none
 * @return none
 */
void boardInit(void);

/**
 * @brief Prints custom header on hyperterminal.
 * @param None
 * @return None
 */
void printHeader(void);

/**
 * @brief Prints date and time header on hyperterminal.
 * @param None
 * @return None
 */
void printDateTimeHeader(void);

/**
 * @brief Display what time configuration options are available. Prompt user to input
 * desired option. Defaults to option 1 if timeout of 5 seconds is exceeded. Defaults
 * to option 1 if input character is invalid.
 * @param None
 * @return AH_Time_Init_Option_Typedef
 */
AH_Time_Init_Option_Typedef promptTimeOptions(void);


#endif /* INC_MISC_H */
