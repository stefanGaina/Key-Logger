#ifndef KEY_LOGGER_INSTALLER_H_
#define KEY_LOGGER_INSTALLER_H_

/******************************************************************************************************
 * HEADER FILE INCLUDES                                                                               *
 *****************************************************************************************************/

#include <stdbool.h>

/******************************************************************************************************
 * FUNCTION PROTOTYPES                                                                                *
 *****************************************************************************************************/

/**
 * @brief
 * @param void
 * @return true - key logger ran successfully | false - key logger failed to start.
*/
extern bool key_logger_installer_install(void);

#endif /*< KEY_LOGGER_INSTALLER_H_ */
