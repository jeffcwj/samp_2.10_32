#include "../main.h"
#include "../game/game.h"
#include "netgame.h"

CPlayerPool::CPlayerPool()
{
	m_pLocalPlayer = new CLocalPlayer();

	for(auto & m_pPlayer : m_pPlayers)
	{
		m_pPlayer = nullptr;
	}
}

CPlayerPool::~CPlayerPool()
{
	delete m_pLocalPlayer;
	m_pLocalPlayer = nullptr;

	for(PLAYERID playerId = 0; playerId < MAX_PLAYERS; playerId++)
		Delete(playerId, 0);
}

#include "..//chatwindow.h"
#include "..//game/game.h"
#include "..//net/netgame.h"
extern CGame* pGame;
extern CNetGame* pNetGame;


void CPlayerPool::ApplyCollisionChecking()
{
	for(int i = 0; i < MAX_PLAYERS; i++)
	{
		CRemotePlayer *pPlayer = GetAt(i);
		if(pPlayer)
		{
			CPlayerPed *pPlayerPed = pPlayer->GetPlayerPed();
			if(pPlayerPed)
			{
				if(!pPlayerPed->IsInVehicle())
				{
					m_bCollisionChecking[i] = pPlayerPed->GetCollisionChecking();
					pPlayerPed->SetCollisionChecking(true);
				}
			}
		}
	}
}

void CPlayerPool::ResetCollisionChecking()
{
	for(int i = 0; i < MAX_PLAYERS; i++)
	{
		CRemotePlayer *pPlayer = GetAt(i);
		if(pPlayer)
		{
			CPlayerPed *pPlayerPed = pPlayer->GetPlayerPed();
			if(pPlayerPed)
			{
				if(!pPlayerPed->IsInVehicle())
					pPlayerPed->SetCollisionChecking(m_bCollisionChecking[i]);
			}
		}
	}
}

void CPlayerPool::UpdateScore(PLAYERID playerId, int iScore)
{
	if (playerId == m_LocalPlayerID)
	{
		m_iLocalPlayerScore = iScore;
	}
	else
	{
		if (playerId > MAX_PLAYERS - 1) { return; }
		m_iPlayerScores[playerId] = iScore;
	}
}

void CPlayerPool::UpdatePing(PLAYERID playerId, uint32_t dwPing)
{
	if (playerId == m_LocalPlayerID)
	{
		m_dwLocalPlayerPing = dwPing;
	}
	else
	{
		if (playerId > MAX_PLAYERS - 1) { return; }
		m_dwPlayerPings[playerId] = dwPing;
	}
}

void CPlayerPool::Process()
{
	for(const auto & player : m_pPlayers)
	{
		if(player == nullptr)
			continue;

		player->Process();
	}

	m_pLocalPlayer->Process();
}

PLAYERID CPlayerPool::GetCount()
{
	PLAYERID byteCount = 0;
	for (PLAYERID p = 0; p < MAX_PLAYERS; p++) {
		if (m_pPlayers[p]) byteCount++;
	}
	return byteCount;
}
bool CPlayerPool::New(PLAYERID playerId, char *szPlayerName, bool IsNPC)
{
	m_pPlayers[playerId] = new CRemotePlayer();

	if(m_pPlayers[playerId])
	{
		strcpy(m_szPlayerNames[playerId], szPlayerName);
		m_pPlayers[playerId]->SetID(playerId);
		m_pPlayers[playerId]->SetNPC(IsNPC);

		return true;
	}

	return false;
}

bool CPlayerPool::Delete(PLAYERID playerId, uint8_t byteReason)
{
	Log("CPlayerPool::Delete %d", playerId);
	if(!m_pPlayers[playerId])
		return false;

	if(GetLocalPlayer()->IsSpectating() && GetLocalPlayer()->m_SpectateID == playerId)
		GetLocalPlayer()->ToggleSpectating(false);

	delete m_pPlayers[playerId];
	m_pPlayers[playerId] = nullptr;

	return true;
}

PLAYERID CPlayerPool::FindRemotePlayerIDFromGtaPtr(CPedGta * pActor)
{
	CPlayerPed *pPlayerPed;

	for(auto & m_pPlayer : m_pPlayers)
	{
		if(m_pPlayer)
		{
			pPlayerPed = m_pPlayer->GetPlayerPed();

			if(pPlayerPed) {
				CPedGta *pTestActor = pPlayerPed->GetGtaActor();
				if((pTestActor != NULL) && (pActor == pTestActor)) // found it
					return m_pPlayer->GetID();
			}
		}
	}

	return INVALID_PLAYER_ID;	
}