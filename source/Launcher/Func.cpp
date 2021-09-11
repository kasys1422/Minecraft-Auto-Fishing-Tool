#include "Func.h"
#include <windows.h>
#include <string>

void GetLocaleInfoChar(char *s)
{
	//‘ƒR[ƒhŽæ“¾(•ÏŠ· : WCHAR -> char)
	WCHAR lang_code_buffer_wchar[256];
	char lang_code_buffer_char[256];
	GetLocaleInfo(LOCALE_SYSTEM_DEFAULT, LOCALE_SNAME, lang_code_buffer_wchar, _countof(lang_code_buffer_wchar));
	size_t ReturnValue;
	wcstombs_s(&ReturnValue, lang_code_buffer_char, 256, lang_code_buffer_wchar, 256);
	sprintf_s(s, sizeof(s), "%s", lang_code_buffer_char);
}
