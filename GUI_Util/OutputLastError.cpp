#include"OutputLastError.h"

#include<Windows.h>

#include"System.h"

void OutputLastError()
{
	auto le = GetLastError();
	wchar_t* messageBuff = nullptr;
	FormatMessage
	(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		le,
		0,
		(LPWSTR)&messageBuff,
		0,
		NULL
	);
	DebugMessageWide(messageBuff);
}

