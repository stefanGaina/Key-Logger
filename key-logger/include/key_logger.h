/******************************************************************************************************
 * @file key_logger.h                                                                                 *
 * @date:      @author:                   Reason for change:                                          *
 * 08.08.2023  Gaina Stefan               Initial version.                                            *
 * @details This file defines the type definitions and public interface of keylogger.                 *
 * @todo N/A.                                                                                         *
 * @bug No known bugs.                                                                                *
 *****************************************************************************************************/

#ifndef KEY_LOGGER_H_
#define KEY_LOGGER_H_

/******************************************************************************************************
 * HEADER FILE INCLUDES                                                                               *
 *****************************************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

/******************************************************************************************************
 * TYPE DEFINITIONS                                                                                   *
 *****************************************************************************************************/

/**
 * @brief The function prototype for the callback.
*/
typedef void (*key_logger_Callback_t)(void*);

/******************************************************************************************************
 * FUNCTION PROTOTYPES                                                                                *
 *****************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief This is a blocking function (until key_logger_stop() is called) logging into a file every 10
 * milliseconds what keys have been pressed. Only one logger can be ran at a time.
 * @param[in] file: File where the keys will be logged (function will return immediately if file is
 * invalid).
 * @return false - key logger failed to start | true - key logger ran successfully.
*/
extern bool key_logger_run(FILE* file);

/**
 * @brief This is a blocking function (until key_logger_stop() is called) logging into a file what keys
 * have been pressed. Only one logger can be ran at a time.
 * @param[in] file: File where the keys will be logged (function will return immediately if file is
 * invalid).
 * @param sleep_time: Minimum time that the querying of key states is paused (in milliseconds), might
 * be more due to OS scheduler. This is to reduce CPU load. A value too high might result in loss of
 * logged keys.
 * @param maximum_characters_per_line: The maximum amount of characters that will be written in file on
 * a line at a time (0 will deactivate this, not restricting the maximum length).
 * @param[in] callback: Callback after each iteration (can be NULL).
 * @param[in] callback_data: This will be passed as parameter for the callback.
 * @return false - key logger failed to start | true - key logger ran successfully.
*/
extern bool key_logger_run_custom(FILE* file, uint32_t sleep_time, uint8_t maximum_characters_per_line, key_logger_Callback_t callback, void* callback_data);

/**
 * @brief Makes key_logger_run() quit after the sleep time is finished. If logger is not running this
 * function has no effect.
 * @param void
 * @return void
*/
extern void key_logger_stop(void);

/**
 * @brief Queries the current state of the key logger.
 * @param void
 * @return true - key logger is running | false - key logger is not running.
*/
extern bool key_logger_is_running(void);

#ifdef __cplusplus
}
#endif

#endif /*< KEY_LOGGER_H_ */
