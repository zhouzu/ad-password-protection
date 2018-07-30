#include "stdafx.h"
#include "hasher.h"
#include "filter.h"
#include "utils.h"
#include "stringnormalization.h"
#include "hashevaluator.h"
#include "registry.h"
#include "eventlog.h"
#include "messages.h"
#include "passwordevaluator.h"

extern "C" __declspec(dllexport)  BOOLEAN __stdcall InitializeChangeNotify(void)
{
	return TRUE;
}

extern "C" __declspec(dllexport) NTSTATUS __stdcall PasswordChangeNotify(
	PUNICODE_STRING UserName,
	ULONG RelativeId,
	PUNICODE_STRING NewPassword
)
{
	return 0L;
}

extern "C" __declspec(dllexport) BOOLEAN __stdcall PasswordFilter(
	PUNICODE_STRING AccountName,
	PUNICODE_STRING FullName,
	PUNICODE_STRING Password,
	BOOLEAN SetOperation
)
{
	bool simulate = false;

	try {

		if (GetRegValue(L"Disabled", 0) != 0)
		{
			eventlog::getInstance().logw(EVENTLOG_WARNING_TYPE, MSG_AGENT_DISABLED, 0);
			return TRUE;
		}

		simulate = GetRegValue(L"Simulate", 0) != 0;

		std::wstring password(Password->Buffer, Password->Length / sizeof(WCHAR));
		std::wstring accountName(AccountName->Buffer, AccountName->Length / sizeof(WCHAR));
		std::wstring fullName(FullName->Buffer, FullName->Length / sizeof(WCHAR));
		
		BOOLEAN result = ProcessPassword(password, accountName, fullName, SetOperation);

		if (simulate)
		{
			return TRUE;
		}
		else 
		{
			return result;
		}
	}
	catch (std::system_error const& e)
	{
		OutputDebugString(L"Win32 error caught");

		eventlog::getInstance().log(EVENTLOG_ERROR_TYPE, MSG_WIN32ERROR, 2, std::to_string(e.code().value()).c_str(), e.what());

		if ((SetOperation && GetRegValue(L"AllowPasswordSetOnError", 1) == 0) || (!SetOperation && GetRegValue(L"AllowPasswordChangeOnError", 1) == 0))
		{
			OutputDebugString(L"Rejected password because AllowPasswordSetOnError or AllowPasswordChangeOnError was non-zero");
			eventlog::getInstance().logw(EVENTLOG_WARNING_TYPE, MSG_PASSWORD_REJECTED_ON_ERROR, 1, SetOperation ? L"set" : L"change");
			return simulate ? TRUE : FALSE;
		}
	}
	catch (std::exception const& e)
	{
		OutputDebugString(L"Other error caught");

		eventlog::getInstance().log(EVENTLOG_ERROR_TYPE, MSG_UNEXPECTEDERROR, 1, e.what());

		if ((SetOperation && GetRegValue(L"AllowPasswordSetOnError", 1) == 0) || (!SetOperation && GetRegValue(L"AllowPasswordChangeOnError", 1) == 0))
		{
			OutputDebugString(L"Rejected password because AllowPasswordSetOnError or AllowPasswordChangeOnError was non-zero");
			eventlog::getInstance().logw(EVENTLOG_WARNING_TYPE, MSG_PASSWORD_REJECTED_ON_ERROR, 1, SetOperation ? L"set" : L"change");
			return simulate ? TRUE : FALSE;
		}
	}
	catch (...)
	{
		OutputDebugString(L"Unexpected error caught");

		eventlog::getInstance().logw(EVENTLOG_ERROR_TYPE, MSG_UNEXPECTEDERROR, 1, L"No exception information was available");

		if ((SetOperation && GetRegValue(L"AllowPasswordSetOnError", 1) == 0) || (!SetOperation && GetRegValue(L"AllowPasswordChangeOnError", 1) == 0))
		{
			OutputDebugString(L"Rejected password because AllowPasswordSetOnError or AllowPasswordChangeOnError was non-zero");
			eventlog::getInstance().logw(EVENTLOG_WARNING_TYPE, MSG_PASSWORD_REJECTED_ON_ERROR, 1, SetOperation ? L"set" : L"change");
			return simulate ? TRUE : FALSE;
		}
	}

	OutputDebugString(L"Approved password");
	return TRUE;
}