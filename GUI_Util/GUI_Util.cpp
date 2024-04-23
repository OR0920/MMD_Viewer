// header
#include "GUI_Util.h"

// std
#include<cassert>

// windows
#include<windows.h>
#include<tchar.h>

// my lib
#include "System.h"

// ÉGÉâÅ[èoóÕ
#include"private/OutputLastError.h"

using namespace GUI_Util;

// ErrorBox
void GUI_Util::ErrorBox(const TCHAR* const message)
{
	MessageBox(NULL, message, _T("ERROR"), MB_ICONERROR);
}

