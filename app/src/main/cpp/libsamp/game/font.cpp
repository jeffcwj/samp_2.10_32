#include "../main.h"
#include "font.h"

void CFont::AsciiToGxtChar(const char* ascii, uint16_t* gxt)
{
	return ((void(*)(const char*, uint16_t*))(g_libGTASA + 0x005A83C0 + 1))(ascii, gxt);
}
