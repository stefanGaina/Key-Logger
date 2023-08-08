/******************************************************************************************************
 * @file key_logger_version.c                                                                         *
 * @date:      @author:                   Reason for change:                                          *
 * 08.08.2023  Gaina Stefan               Initial version.                                            *
 * @details This file implements the interface defined in key_logger_version.h.                       *
 * @todo N/A.                                                                                         *
 * @bug No known bugs.                                                                                *
 *****************************************************************************************************/

/******************************************************************************************************
 * HEADER FILE INCLUDES                                                                               *
 *****************************************************************************************************/

#include "key_logger_version.h"

/******************************************************************************************************
 * FUNCTION DEFINITIONS                                                                               *
 *****************************************************************************************************/

key_logger_Version_t key_logger_get_version(void)
{
	return (key_logger_Version_t){ KEY_LOGGER_VERSION_MAJOR, KEY_LOGGER_VERSION_MINOR, KEY_LOGGER_VERSION_PATCH };
}
