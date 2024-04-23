#pragma once

namespace GUI_Util
{
	// メッセージ　兼　結果
	// SUCCESSとFAILはHRESULTと統一
	enum Result
	{
		SUCCESS = 0,
		FAIL = 1,
		CONTINUE,
		QUIT,
	};
}