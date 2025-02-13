#include "CDebugInfo.h"

#include "main.h"
#include "gui/gui.h"
#include "game/game.h"
#include "net/netgame.h"
#include "game/RW/RenderWare.h"
#include "chatwindow.h"
#include "playertags.h"
#include "keyboard.h"
#include "CSettings.h"
#include "util/patch.h"
#include "game/Timer.h"

extern CGUI* pGUI;
extern CGame *pGame;

uint32_t CDebugInfo::uiStreamedPeds = 0;
uint32_t CDebugInfo::uiStreamedVehicles = 0;
uint32_t CDebugInfo::m_uiDrawDebug = 0;

void CDebugInfo::SetDrawFPS(bool toggle)
{
	m_uiDrawDebug = toggle;
	//*(bool*)(g_libGTASA + 0x00959907) = toggle;
}

void CDebugInfo::Draw()
{
//	return;
	//if (!pGame->IsToggledHUDElement(HUD_ELEMENT_FPS)) return;

	char szStr[30];
	char szStrMem[256];
	char szStrPos[256];
	ImVec2 pos;
	//if (m_uiDrawDebug)
	{
		snprintf(&szStr[0], 30, "FPS: %.0f / %d", CTimer::game_FPS, *(uint32_t*)(g_libGTASA + 0x009FC8FC + 0x0C));
		pos = ImVec2(pGUI->ScaleX(40.0f), pGUI->ScaleY(1080.0f - pGUI->GetFontSize() * 10));

		pGUI->RenderText(pos, (ImU32)0xFFFFFFFF, true, &szStr[0]);

		uint32_t msUsed = *(uint32_t*)(g_libGTASA + 0x00792B7C);
		uint32_t msAvailable = *(uint32_t*)(g_libGTASA + 0x00685FA0);

		//	CStreaming::ms_memoryAvailable = 50 * 1024 * 1024; // 50 MB
		float percentUsed = ((float)msUsed/(float)msAvailable)*100;

		struct mallinfo memInfo = mallinfo();
		int totalAllocatedMB  = memInfo.uordblks / (1024 * 1024);

		snprintf(&szStrMem[0], 256, "MEM: %d mb ( stream %d/%d)",
				 totalAllocatedMB,
				 msUsed / (1024 * 1024),
				 msAvailable / (1024 * 1024)
		);
		//Log("%s", szStrMem);
		pos = ImVec2(pGUI->ScaleX(40.0f), pGUI->ScaleY(1080.0f - pGUI->GetFontSize() * 9));

		pGUI->RenderText(pos, (ImU32)0xFFFFFFFF, true, &szStrMem[0]);

		if (pGame->FindPlayerPed()->m_pPed)
		{
			snprintf(&szStrPos[0], 256, "POS: %.2f, %.2f, %.2f",
					 pGame->FindPlayerPed()->m_pPed->mat->pos.x,
					 pGame->FindPlayerPed()->m_pPed->mat->pos.y,
					 pGame->FindPlayerPed()->m_pPed->mat->pos.z);
			pos = ImVec2(pGUI->ScaleX(40.0f), pGUI->ScaleY(1080.0f - pGUI->GetFontSize() * 8));
			pGUI->RenderText(pos, (ImU32)0xFFFFFFFF, true, &szStrPos[0]);
		}
	}
}