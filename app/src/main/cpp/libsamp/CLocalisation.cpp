#include "CLocalisation.h"

#include "main.h"
#include "game/game.h"
#include "net/netgame.h"
#include "game/RW/RenderWare.h"
#include "chatwindow.h"
#include "playertags.h"
#include "keyboard.h"
#include "CSettings.h"


char CLocalisation::m_szMessages[E_MSG::MSG_COUNT][MAX_LOCALISATION_LENGTH] = {
	"{bbbbbb}���������� � LIVE RUSSIA{ffffff}",
	"{bbbbbb}���������. ���������� � ����...{ffffff}",
	"{bbbbbb}������ ������� ����������{ffffff}",
	"{ff0000}��������� ����� ���� ��������������, �������������� ������{ffffff}",
	"{bbbbbb}unused{ffffff}",
	"{ff0000}��� ����������� �� �������. ����������� � ����{ffffff}",
	"{bbbbbb}������ �� ��������. ��������� ����������...{ffffff}",
	"{bbbbbb}�������� � �����, ���������������...{ffffff}",
	"{bbbbbb}������ �����{ffffff}"};

void CLocalisation::Initialise(const char* szFile)
{
	char buff[MAX_LOCALISATION_LENGTH];

	sprintf(&buff[0], "%sSAMP/%s", g_pszStorage, szFile);

	FILE* pFile = fopen(&buff[0], "r");
	if (!pFile)
	{
		Log("Localisation | Cannot initialise %s", szFile);
		return;
	}

	for (int i = 0; i < E_MSG::MSG_COUNT; i++)
	{
		memset(m_szMessages[i], 0, MAX_LOCALISATION_LENGTH);
	}
	uint32_t counter = 0;
	while (fgets(&buff[0], MAX_LOCALISATION_LENGTH, pFile) != NULL)
	{
		if (counter == E_MSG::MSG_COUNT)
		{
			break;
		}

		memcpy((void*)& m_szMessages[counter][0], (const void*)(&buff[0]), MAX_LOCALISATION_LENGTH);
		counter++;
	}
	fclose(pFile);
	Log("Localisation initialized");
}

char* CLocalisation::GetMessage(E_MSG msg)
{
	//Log("SYMBOLS debug %x %x %x %x %x %x %x %x %x", m_szMessages[msg][0], m_szMessages[msg][1], m_szMessages[msg][2], m_szMessages[msg][3], m_szMessages[msg][4], m_szMessages[msg][5],
		//m_szMessages[msg][6], m_szMessages[msg][7], m_szMessages[msg][8]);
	//Log("SYMBOLS debug %s", &m_szMessages[msg][0]);
	return &m_szMessages[msg][0];
}
