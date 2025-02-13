#include "../main.h"
#include "game.h"
#include "../net/netgame.h"
#include "common.h"
#include "vehicle.h"

#include "..//CDebugInfo.h"
#include "util/patch.h"
#include "game/Enums/PedState.h"
#include "CWorld.h"
#include "game/RW/rtanim.h"
#include "game/Enums/WeaponType.h"
#include "StreamingInfo.h"
#include "game/Enums/AnimationEnums.h"
#include "game/Models/ModelInfo.h"

extern CGame* pGame;
extern CNetGame *pNetGame;
CPlayerPed* g_pCurrentFiredPed;
BULLET_DATA* g_pCurrentBulletData;

CPlayerPed::CPlayerPed()
{
	m_dwGTAId = 1;
	m_pPed = (CPedGta*)GamePool_FindPlayerPed();
	m_pEntity = m_pPed;

	m_bHaveBulletData = false;

	m_bytePlayerNumber = 0;
	SetPlayerPedPtrRecord(m_bytePlayerNumber,(uintptr_t)m_pPed);
	ScriptCommand(&set_actor_weapon_droppable, m_dwGTAId, 1);
	ScriptCommand(&set_char_never_targeted, m_dwGTAId, 1);
	ScriptCommand(&set_actor_can_be_decapitated, m_dwGTAId, 0);
	m_dwArrow = 0;

	for (int i = 0; i < MAX_ATTACHED_OBJECTS; i++)
	{
		m_aAttachedObjects[i].bState = false;
	}
}

CPlayerPed::CPlayerPed(uint8_t bytePlayerNumber, int iSkin, float fX, float fY, float fZ, float fRotation)
{
	CDebugInfo::uiStreamedPeds++;

	uint32_t dwPlayerActorID = 0;
	int iPlayerNum = bytePlayerNumber;

	m_pPed = nullptr;
	m_dwGTAId = 0;
	m_dwArrow = 0;
	m_bHaveBulletData = false;

	ScriptCommand(&create_player, &iPlayerNum, fX, fY, fZ, &dwPlayerActorID);
	ScriptCommand(&create_actor_from_player, &iPlayerNum, &dwPlayerActorID);

	m_dwGTAId = dwPlayerActorID;
	m_pPed = CUtil::GetPoolPed(m_dwGTAId);

	m_pEntity = CUtil::GetPoolPed(m_dwGTAId);

	m_bytePlayerNumber = bytePlayerNumber;

	SetPlayerPedPtrRecord(m_bytePlayerNumber, (uintptr_t)m_pPed);
	ScriptCommand(&set_actor_weapon_droppable, m_dwGTAId, 1);
	ScriptCommand(&set_actor_immunities, m_dwGTAId, 0, 0, 0, 0, 0);
	ScriptCommand(&set_actor_can_be_decapitated, m_dwGTAId, 0); // ����� �����
	ScriptCommand(&set_char_never_targeted, m_dwGTAId, 1);

	ScriptCommand(&set_actor_money, m_dwGTAId, 0); // ������ ������ ��� ������

	SetModelIndex(iSkin);
	ForceTargetRotation(fRotation);
	RwMatrix mat;
	GetMatrix(&mat);
	mat.pos.x = fX;
	mat.pos.y = fY;
	mat.pos.z = fZ+ 0.15f;
	SetMatrix(mat);
	
	for (int i = 0; i < MAX_ATTACHED_OBJECTS; i++)
	{
		m_aAttachedObjects[i].bState = false;
	}

	memset(&RemotePlayerKeys[m_bytePlayerNumber], 0, sizeof(PAD_KEYS));

}

bool IsValidGamePed(CPedGta* pPed);

CPlayerPed::~CPlayerPed()
{
	CDebugInfo::uiStreamedPeds--;

	memset(&RemotePlayerKeys[m_bytePlayerNumber], 0, sizeof(PAD_KEYS));

	SetPlayerPedPtrRecord(m_bytePlayerNumber, 0);

	if(!m_dwGTAId)return;
	if(!CUtil::GetPoolPed(m_dwGTAId))return;

	if (m_pPed && IsValidGamePed(m_pPed))
	{
		FlushAttach();
		if (IsInVehicle()) {
			RemoveFromVehicleAndPutAt(100.0f, 100.0f, 20.0f);

		//	ClearAllTasks();
		}
        ClearAllTasks();

		uintptr_t dwPedPtr = (uintptr_t)m_pPed;
		*(uint32_t*)(*(uintptr_t*)(dwPedPtr + 1088) + 76) = 0;
		// CPlayerPed::Destructor

		(( void (*)(CPedGta*))(*(void**)(m_pPed->vtable+0x4)))(m_pPed);
		//((void (*)(uintptr_t))(g_libGTASA + 0x004CE6A0 + 1))((uintptr_t)m_pEntity);
		//ScriptCommand(&DELETE_CHAR, m_dwGTAId);

		m_pPed = nullptr;
		m_pEntity = nullptr;
		m_dwGTAId = 0;
	}
	else
	{
		m_pPed = nullptr;
		m_pEntity = nullptr;
		m_dwGTAId = 0;
	}
}

CAMERA_AIM * CPlayerPed::GetCurrentAim()
{
	return GameGetInternalAim();
}

void CPlayerPed::SetCurrentAim(CAMERA_AIM * pAim)
{
	GameStoreRemotePlayerAim(m_bytePlayerNumber, pAim);
}

uint16_t CPlayerPed::GetCameraMode()
{
	return GameGetLocalPlayerCameraMode();
}

void CPlayerPed::SetCameraMode(uint16_t byteCamMode)
{
	GameSetPlayerCameraMode(byteCamMode, m_bytePlayerNumber);
}

float CPlayerPed::GetCameraExtendedZoom()
{
	return GameGetLocalPlayerCameraExtZoom();
}

void CPlayerPed::ApplyCrouch()
{
	
	if (!m_pPed || !m_dwGTAId)
	{
		return;
	}
	if (!CUtil::GetPoolPed(m_dwGTAId))
	{
		return;
	}

	if (!(m_pPed->bIsDucking))
	{
		if (!IsCrouching())
		{
			if (m_pPed->pPedIntelligence)
			{
				((void (*)(CPedIntelligence*, uint16_t))(g_libGTASA + 0x004C07B0 + 1))(m_pPed->pPedIntelligence, 0);
			}
		}
	}
}

void CPlayerPed::ResetCrouch()
{
	
	if (!m_pPed || !m_dwGTAId)
	{
		return;
	}
	if (!CUtil::GetPoolPed(m_dwGTAId))
	{
		return;
	}
	m_pPed->bIsDucking = 0;
	if (m_pPed->pPedIntelligence)
	{
		((int (*)(CPedIntelligence *))(g_libGTASA + 0x004C08A8 + 1))(m_pPed->pPedIntelligence);
	}
	//bKeepTasksAfterCleanUp
}

bool CPlayerPed::IsCrouching()
{
	
	if (!m_pPed || !m_dwGTAId)
	{
		return false;
	}
	if (!CUtil::GetPoolPed(m_dwGTAId))
	{
		return false;
	}
	return m_pPed->bIsDucking;
}

void CPlayerPed::SetCameraExtendedZoom(float fZoom)
{
	GameSetPlayerCameraExtZoom(m_bytePlayerNumber, fZoom);
}

void CPlayerPed::SetCameraExtendedZoom(float fZoom, float fAspectRatio)
{
	GameSetPlayerCameraExtZoom(m_bytePlayerNumber, fZoom, fAspectRatio);
}


void CPlayerPed::SetDead()
{
	
	if (!m_dwGTAId || !m_pPed)
	{
		return;
	}
	if (!CUtil::GetPoolPed(m_dwGTAId))
	{
		return;
	}
	
	RwMatrix mat;
	GetMatrix(&mat);
	// will reset the tasks
	TeleportTo(mat.pos.x, mat.pos.y, mat.pos.z);
	m_pPed->fHealth = 0.5f;



//	uint8_t old = CWorld::PlayerInFocus; // CWorld::PlayerInFocus - 0x008E864C
//	CWorld::PlayerInFocus = m_bytePlayerNumber;
//	ScriptCommand(&kill_actor, m_dwGTAId);
//	CWorld::PlayerInFocus = old;
}

// 0.3.7
bool CPlayerPed::IsInVehicle()
{
	return m_pPed->bInVehicle;
}
int GameGetWeaponModelIDFromWeaponID(int iWeaponID)
{
	switch (iWeaponID)
	{
	case WEAPON_BRASSKNUCKLE:
		return WEAPON_MODEL_BRASSKNUCKLE;

	case WEAPON_GOLFCLUB:
		return WEAPON_MODEL_GOLFCLUB;

	case WEAPON_NIGHTSTICK:
		return WEAPON_MODEL_NITESTICK;

	case WEAPON_KNIFE:
		return WEAPON_MODEL_KNIFE;

	case WEAPON_BASEBALLBAT:
		return WEAPON_MODEL_BAT;

	case WEAPON_SHOVEL:
		return WEAPON_MODEL_SHOVEL;

	case WEAPON_POOL_CUE:
		return WEAPON_MODEL_POOLSTICK;

	case WEAPON_KATANA:
		return WEAPON_MODEL_KATANA;

	case WEAPON_CHAINSAW:
		return WEAPON_MODEL_CHAINSAW;

	case WEAPON_DILDO1:
		return WEAPON_MODEL_DILDO;

	case WEAPON_DILDO2:
		return WEAPON_MODEL_DILDO2;

	case WEAPON_VIBE1:
		return WEAPON_MODEL_VIBRATOR;

	case WEAPON_VIBE2:
		return WEAPON_MODEL_VIBRATOR2;

	case WEAPON_FLOWERS:
		return WEAPON_MODEL_FLOWER;

	case WEAPON_CANE:
		return WEAPON_MODEL_CANE;

	case WEAPON_GRENADE:
		return WEAPON_MODEL_GRENADE;

	case WEAPON_TEARGAS:
		return WEAPON_MODEL_TEARGAS;

	case WEAPON_MOLOTOV:
		return -1;

	case WEAPON_PISTOL:
		return WEAPON_MODEL_COLT45;

	case WEAPON_PISTOL_SILENCED:
		return WEAPON_MODEL_SILENCED;

	case WEAPON_DESERT_EAGLE:
		return WEAPON_MODEL_DEAGLE;

	case WEAPON_SHOTGUN:
		return WEAPON_MODEL_SHOTGUN;

	case WEAPON_SAWNOFF_SHOTGUN:
		return WEAPON_MODEL_SAWEDOFF;

	case WEAPON_SPAS12_SHOTGUN:
		return WEAPON_MODEL_SHOTGSPA;

	case WEAPON_MICRO_UZI:
		return WEAPON_MODEL_UZI;

	case WEAPON_MP5:
		return WEAPON_MODEL_MP5;

	case WEAPON_AK47:
		return WEAPON_MODEL_AK47;

	case WEAPON_M4:
		return WEAPON_MODEL_M4;

	case WEAPON_TEC9:
		return WEAPON_MODEL_TEC9;

	case WEAPON_COUNTRYRIFLE:
		return WEAPON_MODEL_RIFLE;

	case WEAPON_SNIPERRIFLE:
		return WEAPON_MODEL_SNIPER;

	case WEAPON_RLAUNCHER:
		return WEAPON_MODEL_ROCKETLAUNCHER;

	case WEAPON_RLAUNCHER_HS:
		return WEAPON_MODEL_HEATSEEKER;

	case WEAPON_FLAMETHROWER:
		return WEAPON_MODEL_FLAMETHROWER;

	case WEAPON_MINIGUN:
		return WEAPON_MODEL_MINIGUN;

	case WEAPON_REMOTE_SATCHEL_CHARGE:
		return WEAPON_MODEL_SATCHEL;

	case WEAPON_DETONATOR:
		return WEAPON_MODEL_BOMB;

	case WEAPON_SPRAYCAN:
		return WEAPON_MODEL_SPRAYCAN;

	case WEAPON_EXTINGUISHER:
		return WEAPON_MODEL_FIREEXTINGUISHER;

	case WEAPON_CAMERA:
		return WEAPON_MODEL_CAMERA;

	case -1:
		return WEAPON_MODEL_NIGHTVISION;

	case -2:
		return WEAPON_MODEL_INFRARED;

	case WEAPON_PARACHUTE:
		return WEAPON_MODEL_PARACHUTE;

	}

	return -1;
}


void CPlayerPed::SetWeaponAmmo(int iWeaponID, int iAmmo)
{
	if (!m_pPed || !m_dwGTAId)
	{
		return;
	}

	if (!CUtil::GetPoolPed(m_dwGTAId))
	{
		return;
	}

	int iModelID = 0;
	iModelID = GameGetWeaponModelIDFromWeaponID(iWeaponID);

	if (iModelID == -1) return;

	ScriptCommand(&SET_CHAR_AMMO, m_dwGTAId, iWeaponID, iAmmo);
	SetCurrentWeapon((eWeaponType)iWeaponID);
}
void CPlayerPed::GiveWeapon(int iWeaponID, int iAmmo)
{
	if (!m_pPed || !m_dwGTAId)
	{
		return;
	}

	if (!CUtil::GetPoolPed(m_dwGTAId))
	{
		return;
	}

	int iModelID = 0;
	iModelID = GameGetWeaponModelIDFromWeaponID(iWeaponID);
	
	if (iModelID == -1) return;
	
	if (!pGame->IsModelLoaded(iModelID)) 
	{
		CStreaming::RequestModel(iModelID, STREAMING_GAME_REQUIRED);
		CStreaming::LoadAllRequestedModels(false);
		while (!pGame->IsModelLoaded(iModelID)) sleep(1);
	}
	//ScriptCommand(&give_actor_weapon, m_dwGTAId, iWeaponID, iAmmo);

	GiveWeapon((eWeaponType)iWeaponID, iAmmo, false);
	SetCurrentWeapon((eWeaponType)iWeaponID);
}

void CPlayerPed::GiveWeapon(eWeaponType weaponType, uint32_t ammoQuantity, bool GenerateOldWeaponPickup)
{
	((int(*)(CPedGta*, eWeaponType, uint32_t, bool))(g_libGTASA + 0x0049F588 + 1))(m_pPed, weaponType, ammoQuantity, GenerateOldWeaponPickup);
}

void CPlayerPed::SetCurrentWeapon(eWeaponType weaponType)
{
	((int(*)(uintptr_t, eWeaponType))(g_libGTASA + 0x004A521C + 1))((uintptr_t)m_pPed, weaponType);
}
//
//void CPlayerPed::SetPlayerAimState()
//{
//	if (!m_pPed || !m_dwGTAId)
//	{
//		return;
//	}
//
//	if (!CUtil::GetPoolPed(m_dwGTAId))
//	{
//		return;
//	}
//
//	uintptr_t ped = (uintptr_t)m_pPed;
//	uint8_t old = *CWorld::PlayerInFocus; // CWorld::PlayerInFocus - 0x008E864C
//	*CWorld::PlayerInFocus = m_bytePlayerNumber;
//
//	((uint32_t(*)(uintptr_t, int, int, int))(g_libGTASA + 0x00454A6C + 1))(ped, 1, 1, 1); // CPlayerPed::ClearWeaponTarget
//	*(uint8_t *)(*(uint32_t *)(ped + 1088) + 52) = *(uint8_t *)(*(uint32_t *)(ped + 1088) + 52) & 0xF7 | 8 * (1 & 1); // magic
//
//	*CWorld::PlayerInFocus = old;
//}

void CPlayerPed::ApplyCommandTask(char* a2, int a4, int a5, int a6, CVector* a7, char a8, float a9, int a10, int a11, char a12)
{
	uint32_t dwPed = (uint32_t)m_pPed;
	if (!dwPed) return;
	// 00958484 - g_ikChainManager
	// 00463188 addr
	((int(*)(uintptr_t a1, char* a2, uint32_t a3, int a4, int a5, int a6, CVector* a7, char a8, float a9, int a10, int a11, char a12))(g_libGTASA + 0x004D36E2 + 1))
		(g_libGTASA + 0x009F9D4C, a2, dwPed, a4, a5, a6, a7, a8, a9, a10, a11, a12);

}

void CPlayerPed::ClearPlayerAimState()
{
	if (!m_pPed || !m_dwGTAId)
	{
		return;
	}

	if (!CUtil::GetPoolPed(m_dwGTAId))
	{
		return;
	}

	uintptr_t ped = (uintptr_t)m_pPed;
	uint8_t old = CWorld::PlayerInFocus;	// CWorld::PlayerInFocus - 0x008E864C
	CWorld::PlayerInFocus = m_bytePlayerNumber;

	*(uint32_t *)(ped + 1432) = 0;	// unk
	((uint32_t(*)(uintptr_t, int, int, int))(g_libGTASA + 0x004C58E4 + 1))(ped, 0, 0, 0);	// CPlayerPed::ClearWeaponTarget
	*(uint8_t *)(*(uint32_t *)(ped + 1088) + 52) = *(uint8_t *)(*(uint32_t *)(ped + 1088) + 52) & 0xF7 | 8 * (0 & 1);	// magic...

	CWorld::PlayerInFocus = old;
}

BYTE CPlayerPed::GetCurrentWeapon()
{
	if(!m_pPed) return 0;
	if(CUtil::GetPoolPed(m_dwGTAId) == 0) return 0;

	DWORD dwRetVal;
	ScriptCommand(&get_actor_armed_weapon,m_dwGTAId,&dwRetVal);
	return (BYTE)dwRetVal;
}

// 0.3.7
bool CPlayerPed::IsAPassenger()
{
	if(m_pPed->pVehicle && m_pPed->bInVehicle)
	{
		auto pVehicle = m_pPed->pVehicle;

		if(	pVehicle->pDriver != m_pPed ||
			   pVehicle->nModelIndex == TRAIN_PASSENGER ||
			   pVehicle->nModelIndex == TRAIN_FREIGHT )
			return true;
	}

	return false;
}

// 0.3.7
CVehicleGta* CPlayerPed::GetGtaVehicle()
{
	return m_pPed->pVehicle;
}

// 0.3.7
void CPlayerPed::RemoveFromVehicleAndPutAt(float fX, float fY, float fZ)
{
	if (!m_pPed) return;
	if(!m_dwGTAId)return;
	if (!IsValidGamePed(m_pPed) || !CUtil::GetPoolPed(m_dwGTAId)) {
		return;
	}

	if (CUtil::IsGameEntityArePlaceable(reinterpret_cast<CEntityGta *>(&m_pPed))) {
		return;
	}

	if(m_pPed && m_pPed->bInVehicle)
		ScriptCommand(&remove_actor_from_car_and_put_at, m_dwGTAId, fX, fY, fZ);
}

// 0.3.7
void CPlayerPed::SetInitialState()
{
	((void (*)(CPedGta*, bool bReplay))(g_libGTASA + 0x004C37B4 + 1))(m_pPed, false);
}

// 0.3.7
void CPlayerPed::SetHealth(float fHealth)
{
	if(!m_pPed) return;
	if (IsValidGamePed(m_pPed))
	{
		m_pPed->fHealth = fHealth;
	}

}

// 0.3.7
float CPlayerPed::GetHealth()
{
	if(!m_pPed) return 0.0f;
	return m_pPed->fHealth;
}

// 0.3.7
void CPlayerPed::SetArmour(float fArmour)
{
	if(!m_pPed) return;
	m_pPed->fArmour = fArmour;
}

float CPlayerPed::GetArmour()
{
	if(!m_pPed) return 0.0f;
	return m_pPed->fArmour;
}

void CPlayerPed::SetInterior(uint8_t byteID, bool refresh)
{
	if(!m_pPed) return;

	if (!CUtil::GetPoolPed(m_dwGTAId))
	{
		return;
	}

	if(m_pPed && m_bytePlayerNumber != 0) {
		ScriptCommand(&link_actor_to_interior, m_dwGTAId, byteID);
	}
	else {
		ScriptCommand(&select_interior, byteID);
		ScriptCommand(&link_actor_to_interior, m_dwGTAId, byteID);

		if(refresh) {
			RwMatrix mat;
			this->GetMatrix(&mat);
			ScriptCommand(&refresh_streaming_at, mat.pos.x, mat.pos.y);
		}
	}
}

void CPlayerPed::PutDirectlyInVehicle(CVehicle *pVehicle, int iSeat)
{

	if(!pVehicle) return;
	if (!GamePool_Vehicle_GetAt(pVehicle->m_dwGTAId)) return;

	if (!m_pPed) return;
	if(!m_dwGTAId)return;
	if (!IsValidGamePed(m_pPed) || !CUtil::GetPoolPed(m_dwGTAId)) {
		return;
	}
	if (CUtil::IsGameEntityArePlaceable(reinterpret_cast<CEntityGta *>(&m_pPed))) {
		return;
	}

    CVehicleGta *gtaVehicle = pVehicle->m_pVehicle;
    if(!gtaVehicle)return;
    if(gtaVehicle->fHealth == 0.0f) return;

	/*
	if(GetCurrentWeapon() == WEAPON_PARACHUTE) {
		SetCurrentWeapon(0);
	}*/

	// check seatid

	Log("PutDirectlyInVehicle");

//	RwMatrix mat;
//	pVehicle->GetMatrix(&mat);
//
////	GetMatrix(&mat);
////	mat.pos.x = pVehicle->entity.mat->pos.x;
////	mat.pos.y = pVehicle->entity.mat->pos.y;
////	mat.pos.z = pVehicle->entity.mat->pos.z;
//	SetMatrix(mat);

//	CHook::CallFunction<bool>(g_libGTASA + 0x0050AAC8 + 1, m_pPed, pVehicle->m_pVehicle, iSeat, iSeat==0);
	if(iSeat == 0)
	{
		if(pVehicle->m_pVehicle->pDriver) return;
		ScriptCommand(&TASK_WARP_CHAR_INTO_CAR_AS_DRIVER, m_dwGTAId, pVehicle->m_dwGTAId);
	}
	else
	{
		//iSeat--;
		ScriptCommand(&put_actor_in_car2, m_dwGTAId, pVehicle->m_dwGTAId, iSeat);
	}
}

void CPlayerPed::EnterVehicle(int iVehicleID, bool bPassenger)
{
	if (!m_pPed) return;
	if(!m_dwGTAId)return;
	if (!IsValidGamePed(m_pPed) || !CUtil::GetPoolPed(m_dwGTAId)) {
		return;
	}

	if (CUtil::IsGameEntityArePlaceable(m_pPed)) {
		return;
	}

	CVehicleGta* ThisVehicleType;
	if((ThisVehicleType = GamePool_Vehicle_GetAt(iVehicleID)) == 0) return;
	if (ThisVehicleType->fHealth == 0.0f) return;

	if(bPassenger)
	{
		if(ThisVehicleType->nModelIndex == TRAIN_PASSENGER &&
			(m_pPed == GamePool_FindPlayerPed()))
		{
			ScriptCommand(&put_actor_in_car2, m_dwGTAId, iVehicleID, -1);
		}
		else
		{
			ScriptCommand(&send_actor_to_car_passenger,m_dwGTAId,iVehicleID, 3000, -1);
		}
	}
	else{
		ScriptCommand(&TASK_ENTER_CAR_AS_DRIVER, m_dwGTAId, iVehicleID, -1);
	}

}

// 0.3.7
void CPlayerPed::ExitCurrentVehicle()
{
    Log("ExitCurrentVehicle");
	if (!m_pPed) return;
	if(!m_dwGTAId)return;
	if (!IsValidGamePed(m_pPed) || !CUtil::GetPoolPed(m_dwGTAId)) {
		return;
	}

	if (CUtil::IsGameEntityArePlaceable(m_pPed)) {
		return;
	}

	if(m_pPed->bInVehicle)
	{
		ScriptCommand(&TASK_LEAVE_ANY_CAR, m_dwGTAId);

	}
}

// 0.3.7
VEHICLEID CPlayerPed::GetCurrentSampVehicleID()
{
	if(!m_pPed) return INVALID_VEHICLE_ID;
	if(!pNetGame)return INVALID_VEHICLE_ID;

	CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
	if(!pVehiclePool)return INVALID_VEHICLE_ID;
	if(!m_pPed->pVehicle)return INVALID_VEHICLE_ID;

	return pVehiclePool->findSampIdFromGtaPtr((CVehicleGta *) m_pPed->pVehicle);
}
CVehicle* CPlayerPed::GetCurrentVehicle()
{
	if(!m_pPed) return nullptr;

	CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();

	for(const auto & pVehicle : pVehiclePool->m_pVehicles)
	{
		if(pVehicle == nullptr || pVehicle->m_pVehicle == nullptr) continue;

		if(pVehicle->m_pVehicle == m_pPed->pVehicle)
			return pVehicle;
	}
	return nullptr;
}

int CPlayerPed::GetVehicleSeatID()
{
	auto *pVehicle = (CVehicleGta *)m_pPed->pVehicle;

	if( pVehicle->pDriver == m_pPed) return 0;

	if(pVehicle->pPassengers[0] == m_pPed) return 1;
	if(pVehicle->pPassengers[1] == m_pPed) return 2;
	if(pVehicle->pPassengers[2] == m_pPed) return 3;
	if(pVehicle->pPassengers[3] == m_pPed) return 4;
	if(pVehicle->pPassengers[4] == m_pPed) return 5;
	if(pVehicle->pPassengers[5] == m_pPed) return 6;
	if(pVehicle->pPassengers[6] == m_pPed) return 7;

	return (-1);
}

// 0.3.7
void CPlayerPed::TogglePlayerControllable(bool bToggle, bool isTemp)
{
	if(!isTemp) lToggle = bToggle;

	if (!m_pPed) return;
	if(!m_dwGTAId)return;
	if (!IsValidGamePed(m_pPed) || !CUtil::GetPoolPed(m_dwGTAId)) {
		return;
	}

	if(!bToggle)
	{
		ScriptCommand(&toggle_player_controllable, m_bytePlayerNumber, 0);
		ScriptCommand(&lock_actor, m_dwGTAId, 1);
	}
	else if(lToggle)
	{
		ScriptCommand(&toggle_player_controllable, m_bytePlayerNumber, 1);
		ScriptCommand(&lock_actor, m_dwGTAId, 0);
	}

}

// 0.3.7
void CPlayerPed::SetModelIndex(unsigned int uiModel)
{
	if(!CUtil::GetPoolPed(m_dwGTAId)) return;

	if(m_pPed)
	{
		DestroyFollowPedTask();
		CEntity::SetModelIndex(uiModel);

		// reset the Ped Audio Attributes
	//	(( void (*)(uintptr_t, uintptr_t))(g_libGTASA+0x34B2A8+1))(((uintptr_t)m_pPed+660), (uintptr_t)m_pPed);
	}
}

bool CPlayerPed::IsAnimationPlaying(char* szAnimName)
{
	if (!m_pPed) return false;
	if (!CUtil::GetPoolPed(m_dwGTAId)) return false;
	if (!szAnimName || !strlen(szAnimName)) return false;

	if (ScriptCommand(&is_char_playing_anim, m_dwGTAId, szAnimName)) {
		return true;
	}

	return false;
}

void CPlayerPed::ClearAllTasks()
{
	if (!CUtil::GetPoolPed(m_dwGTAId) || !m_pPed) {
		return;
	}

	ScriptCommand(&clear_char_tasks, m_dwGTAId);
}

void CPlayerPed::ClearAnimations()
{
	ApplyAnimation("crry_prtial", "CARRY", 4.0, 0, 0, 0, 0, 1);
	//ClearAllTasks();
	RwMatrix mat;
	GetMatrix(&mat);
	TeleportTo(mat.pos.x,mat.pos.y,mat.pos.z);

	Log("ClearAnimations");
}

void CPlayerPed::DestroyFollowPedTask()
{

}

int Weapon_FireSniper(CWeaponGta* pWeaponSlot, CPedGta* pPed)
{
	return ((int (*)(CWeaponGta*, CPedGta*))(g_libGTASA + 0x005DD740 + 1))(pWeaponSlot, pPed);
}

void CPlayerPed::ClearAllWeapons()
{
    Log("ClearAllWeapons %x", m_pPed);

    ((void (*)(uintptr_t))(g_libGTASA + 0x0049F836 + 1))((uintptr_t)m_pPed);
   // CHook::CallFunction<void>(g_libGTASA + 0x0049F836, m_pPed);
}

uintptr_t* GetWeaponInfo(int iWeapon, int iSkill)
{
	return CHook::CallFunction<uintptr_t*>(g_libGTASA + 0x0018F48C, iWeapon, iSkill);
}

CVector* CPlayerPed::GetCurrentWeaponFireOffset()
{
	if (!IsValidGamePed(m_pPed) || !CUtil::GetPoolPed(m_dwGTAId)) {
		return nullptr;
	}

	CWeaponGta* pSlot = GetCurrentWeaponSlot();

	if (pSlot)
		return (CVector*)(GetWeaponInfo(pSlot->dwType, 1) + 0x24);

	return nullptr;
}

void CPlayerPed::GetWeaponInfoForFire(int bLeft, CVector* vecBone, CVector* vecOut) {
	if (!IsValidGamePed(m_pPed) || !CUtil::GetPoolPed(m_dwGTAId)) {
		return;
	}

	if (CUtil::IsGameEntityArePlaceable(m_pPed)) {
		return;
	}

	CVector* pFireOffset = GetCurrentWeaponFireOffset();
	if (pFireOffset && vecBone && vecOut) {
		vecOut->x = pFireOffset->x;
		vecOut->y = pFireOffset->y;
		vecOut->z = pFireOffset->z;

		int bone_id = 24;
		if (bLeft) {
			bone_id = 34;
		}

		// CPed::GetBonePosition
		((void (*)(CPedGta*, CVector*, int, bool))(g_libGTASA + 0x004A4B0C + 1))(m_pPed, vecBone, bone_id, false);

		vecBone->z += pFireOffset->z + 0.15f;

		// CPed::GetTransformedBonePosition
		((void (*)(CPedGta*, CVector*, int, bool))(g_libGTASA + 0x004A24A8 + 1))(m_pPed, vecOut, bone_id, false);
	}
}

extern uint32_t (*CWeapon__FireInstantHit)(CWeaponGta* thiz, CPedGta* pFiringEntity, CVector* vecOrigin, CVector* muzzlePosn, CEntityGta* targetEntity, CVector *target, CVector* originForDriveBy, int arg6, int muzzle);
extern uint32_t (*CWeapon__FireSniper)(CWeaponGta *pWeaponSlot, CPedGta *pFiringEntity, CEntityGta *a3, CVector *vecOrigin);
bool g_customFire = false;
void CPlayerPed::FireInstant() {
	if(!m_pPed || !CUtil::GetPoolPed(m_dwGTAId)) {
		return;
	}

	uint8_t byteCameraMode;
	if(m_bytePlayerNumber != 0) {
		byteCameraMode = *pbyteCameraMode;
		*pbyteCameraMode = GameGetPlayerCameraMode(m_bytePlayerNumber);

		// wCameraMode2
		GameStoreLocalPlayerCameraExtZoom();
		GameSetRemotePlayerCameraExtZoom(m_bytePlayerNumber);

		GameStoreLocalPlayerAim();
		GameSetRemotePlayerAim(m_bytePlayerNumber);
	}

	g_pCurrentFiredPed = this;

	if(m_bHaveBulletData) {
		g_pCurrentBulletData = &m_bulletData;
	} else {
		g_pCurrentBulletData = nullptr;
	}

	CWeaponGta *pSlot = GetCurrentWeaponSlot();
	if(pSlot) {
		if(GetCurrentWeapon() == WEAPON_SNIPERRIFLE) {
			if(pSlot) {
				Weapon_FireSniper(pSlot, m_pPed);
			} else {
				Weapon_FireSniper(nullptr, nullptr);
			}
		} else {
			CVector vecBonePos;
			CVector vecOut;
			g_customFire = true;
			GetWeaponInfoForFire(false, &vecBonePos, &vecOut);
			CWeapon__FireInstantHit(pSlot, m_pPed, &vecBonePos, &vecOut, nullptr, nullptr, nullptr, 0, 1);
		}
	}

	g_pCurrentFiredPed = nullptr;
	g_pCurrentBulletData = nullptr;

	if(m_bytePlayerNumber != 0) {
		*pbyteCameraMode = byteCameraMode;

		// wCamera2
		GameSetLocalPlayerCameraExtZoom();
		GameSetLocalPlayerAim();
	}
}

void CPlayerPed::ResetDamageEntity()
{
	Log("ResetDamageEntity");
	m_pPed->m_pDamageEntity = 0;
}

// 0.3.7
void CPlayerPed::RestartIfWastedAt(CVector *vecRestart, float fRotation)
{	
	ScriptCommand(&restart_if_wasted_at, vecRestart->x, vecRestart->y, vecRestart->z, fRotation, 0);
}

// 0.3.7
void CPlayerPed::ForceTargetRotation(float fRotation)
{
	if(!m_pPed) return;
	if(!CUtil::GetPoolPed(m_dwGTAId)) return;

	m_pPed->m_fCurrentRotation = CUtil::DegToRad(fRotation);
	m_pPed->m_fAimingRotation = CUtil::DegToRad(fRotation);

	ScriptCommand(&set_actor_z_angle,m_dwGTAId,fRotation);
}

void CPlayerPed::SetRotation(float fRotation)
{
	if(!m_pPed) return;
	if(!CUtil::GetPoolPed(m_dwGTAId)) return;

	m_pPed->m_fCurrentRotation = CUtil::DegToRad(fRotation);
	m_pPed->m_fAimingRotation = CUtil::DegToRad(fRotation);
}

// 0.3.7
uint8_t CPlayerPed::GetActionTrigger()
{
	return (uint8_t)m_pPed->m_nPedState;
}

void CPlayerPed::SetActionTrigger(ePedState action)
{
	m_pPed->m_nPedState = action;
}

int GetInternalBoneIDFromSampID(int sampid);

void CPlayerPed::AttachObject(ATTACHED_OBJECT_INFO* pInfo, int iSlot)
{
	if (m_aAttachedObjects[iSlot].bState)
	{
		DeattachObject(iSlot);
	}
	memcpy((void*)& m_aAttachedObjects[iSlot], (const void*)pInfo, sizeof(ATTACHED_OBJECT_INFO));
	RwMatrix matPos;
	GetMatrix(&matPos);
	CVector vecRot{ 0.0f, 0.0f, 0.0f };
	m_aAttachedObjects[iSlot].pObject = new CObject(pInfo->dwModelId, matPos.pos.x, matPos.pos.y, matPos.pos.z, vecRot, 200.0f);
	*(uint32_t*)((uintptr_t)m_aAttachedObjects[iSlot].pObject->m_pEntity + 28) &= 0xFFFFFFFE; // disable collision
	m_aAttachedObjects[iSlot].bState = true;

}

void CPlayerPed::SetAttachOffset(int iSlot, CVector pos, CVector rot)
{
	if (iSlot < 0 || iSlot >= MAX_ATTACHED_OBJECTS)
	{
		return;
	}
	m_aAttachedObjects[iSlot].vecOffset = pos;
	m_aAttachedObjects[iSlot].vecRotation = rot;
}

void CPlayerPed::DeattachObject(int iSlot)
{
	if (m_aAttachedObjects[iSlot].bState)
	{
		delete m_aAttachedObjects[iSlot].pObject;
	}
	m_aAttachedObjects[iSlot].bState = false;
}

bool CPlayerPed::IsHasAttach()
{
	for (int i = 0; i < MAX_ATTACHED_OBJECTS; i++)
	{
		if (m_aAttachedObjects[i].bState) return true;
	}
	return false;
}

bool CPlayerPed::IsValidAttach(int iSlot)
{
	if(m_aAttachedObjects[iSlot].bState)return true;
	return false;
}
void CPlayerPed::FlushAttach()
{
	for (int i = 0; i < MAX_ATTACHED_OBJECTS; i++)
	{
		DeattachObject(i);
	}
}

RwMatrix* RwMatrixMultiplyByVector(CVector* out, RwMatrix* a2, CVector* in)
{
	RwMatrix* result;
	CVector* v4;

	result = a2;
	v4 = in;
	out->x = a2->at.x * in->z + a2->up.x * in->y + a2->right.x * in->x + a2->pos.x;
	out->y = result->at.y * v4->z + result->up.y * v4->y + result->right.y * v4->x + result->pos.y;
	out->z = result->at.z * v4->z + result->up.z * v4->y + a2->right.z * in->x + result->pos.z;
	return result;
}

void RwMatrixRotate(RwMatrix* pMat, CVector* axis, float angle)
{
	((int(*)(RwMatrix*, CVector*, float, int))(g_libGTASA + 0x001E38F4 + 1))(pMat, axis, angle, 1);
}

void CPlayerPed::ProcessAttach()
{
	if (!m_pPed) return;
	if(!m_dwGTAId)return;
	if (!IsValidGamePed(m_pPed) || !CUtil::GetPoolPed(m_dwGTAId)) {
		return;
	}

	((int(*)(CPedGta*))(g_libGTASA + 0x003EC046 + 1))(m_pPed); // UpdateRPHAnim

	if (IsAdded())
	{
		ProcessHeadMatrix();
	}
	for (int i = 0; i < MAX_ATTACHED_OBJECTS; i++)
	{
		if (!m_aAttachedObjects[i].bState) continue;
		CObject* pObject = m_aAttachedObjects[i].pObject;
		if (IsAdded())
		{
			RpHAnimHierarchy* hierarchy = ((RpHAnimHierarchy * (*)(uintptr_t*))(g_libGTASA + 0x005D1070 + 1))((uintptr_t*)m_pPed->m_pRwObject); // GetAnimHierarchyFromSkinClump
			int iID;
			uint32_t bone = m_aAttachedObjects[i].dwBone;
			if (hierarchy)
			{
				iID = ((int(*)(RpHAnimHierarchy*, int))(g_libGTASA + 0x001C2C10 + 1))(hierarchy, bone); // RpHAnimIDGetIndex
			}
			else
			{
				continue;
			}
			if (iID == -1)
			{
				continue;
			}
			pObject->RemovePhysical();

			RwMatrix outMat;
			memcpy(&outMat, &hierarchy->pMatrixArray[iID], sizeof(RwMatrix));

			CVector vecOut;
			RwMatrixMultiplyByVector(&vecOut, &outMat, &m_aAttachedObjects[i].vecOffset);

			outMat.pos.x = vecOut.x;
			outMat.pos.y = vecOut.y;
			outMat.pos.z = vecOut.z;

			CVector axis = { 1.0f, 0.0f, 0.0f };
			if (m_aAttachedObjects[i].vecRotation.x != 0.0f)
			{
				RwMatrixRotate(&outMat, &axis, m_aAttachedObjects[i].vecRotation.x);
			}
			axis.x = 0.0f; axis.y = 1.0f; axis.z = 0.0f;

			if (m_aAttachedObjects[i].vecRotation.y != 0.0f)
			{
				RwMatrixRotate(&outMat, &axis, m_aAttachedObjects[i].vecRotation.y);
			}
			axis.x = 0.0f; axis.y = 0.0f; axis.z = 1.0f;
			if (m_aAttachedObjects[i].vecRotation.z != 0.0f)
			{
				RwMatrixRotate(&outMat, &axis, m_aAttachedObjects[i].vecRotation.z);
			}

			RwMatrixScale(&outMat, &m_aAttachedObjects[i].vecScale);
			*(uint32_t*)((uintptr_t)pObject->m_pEntity + 28) &= 0xFFFFFFFE; // disable collision

			if (outMat.pos.x >= 10000.0f || outMat.pos.x <= -10000.0f ||
				outMat.pos.y >= 10000.0f || outMat.pos.y <= -10000.0f ||
				outMat.pos.z >= 10000.0f || outMat.pos.z <= -10000.0f)
			{
				continue;
			}

			pObject->SetMatrix(outMat); // copy to CMatrix

			pObject->UpdateRwMatrixAndFrame();

			pObject->AddPhysical();
		}
		else
		{
			pObject->TeleportTo(0.0f, 0.0f, 0.0f);
		}
	}
}

void CPlayerPed::ProcessHeadMatrix()
{
	RpHAnimHierarchy* hierarchy = ((RpHAnimHierarchy * (*)(uintptr_t*))(g_libGTASA + 0x005D1070 + 1))((uintptr_t*)m_pPed->m_pRwObject); // GetAnimHierarchyFromSkinClump
	int iID;
	uint32_t bone = 4;
	if (hierarchy)
	{
		iID = ((int(*)(RpHAnimHierarchy*, int))(g_libGTASA + 0x001C2C10 + 1))(hierarchy, bone); // RpHAnimIDGetIndex
	}
	else
	{
		return;
	}
	if (iID == -1)
	{
		return;
	}

	memcpy(&m_HeadBoneMatrix, &hierarchy->pMatrixArray[iID], sizeof(RwMatrix));
}

bool IsTaskRunNamedOrSlideToCoord(void* pTask)
{
	
	uintptr_t dwVTable = *(uintptr_t*)(pTask);
	if (dwVTable == (g_libGTASA + 0x0066C4E0) || dwVTable == (g_libGTASA + 0x006694F0)) // CTaskSimpleSlideToCoord CTaskSimpleRunNamedAnim
	{
		return true;
	}
	return false;
}

void* GetSubTaskFromTask(void* pTask)
{
	
	uintptr_t pVTableTask = *((uintptr_t*)pTask);
	return ((void* (*)(void*))(*(void**)(pVTableTask + 12)))(pTask);
}


bool CPlayerPed::IsPlayingAnim(int idx)
{
	
	if (!m_pPed || !m_dwGTAId || (idx == 0) )
	{
		return 0;
	}
	if (!CUtil::GetPoolPed(m_dwGTAId))
	{
		return 0;
	}
	if (!m_pPed->m_pRwObject)
	{
		return 0;
	}

	const char* pAnim = GetAnimByIdx(idx - 1);
	if (!pAnim)
	{
		return false;
	}
	const char* pNameAnim = strchr(pAnim, ':') + 1;

	uintptr_t blendAssoc = ((uintptr_t(*)(RpClump* clump, const char* szName))(g_libGTASA + 0x00390A24 + 1))
		(m_pPed->m_pRpClump, pNameAnim);	// RpAnimBlendClumpGetAssociation

	if (blendAssoc)
	{
		return true;
	}
	else
	{
		return false;
	}
}

int CPlayerPed::GetCurrentAnimationIndex(float& blendData)
{
	
	blendData = 4.0f;

	if (!m_pPed || !m_dwGTAId)
	{
		return 0;
	}

	if (!CUtil::GetPoolPed(m_dwGTAId))
	{
		return 0;
	}

	if (!m_pPed->m_pRwObject)
	{
		return 0;
	}
	sizeof(CPedGta);
	CPedIntelligence* pIntelligence = m_pPed->pPedIntelligence;

	if (pIntelligence)
	{
		void* pTask = pIntelligence->m_TaskMgr.m_aPrimaryTasks[TASK_PRIMARY_PRIMARY];

		if (pTask)
		{
			while (!IsTaskRunNamedOrSlideToCoord(pTask))
			{
				pTask = GetSubTaskFromTask(pTask);
				if (!pTask)
				{
					return 0;
				}
			}

			const char* szName = (const char*)((uintptr_t)pTask + 13);
			const char* szGroupName = (const char*)((uintptr_t)pTask + 37);

			std::string szStr = std::string(szGroupName);
			szStr += ":";
			szStr += szName;

			int idx = GetAnimIdxByName(szStr.c_str());
			if (idx == -1)
			{
				return 0;
			}
			else
			{
				return idx + 1;
			}
		}
	}
	return 0;
}

void CPlayerPed::PlayAnimByIdx(int idx, float BlendData, bool loop, bool freeze, uint8_t time)
{
	
	if (!idx)
	{
		RwMatrix mat;
		GetMatrix(&mat);
		TeleportTo(mat.pos.x, mat.pos.y, mat.pos.z);
		return;
	}
	std::string szAnim;
	std::string szBlock;

	char pszAnim[40];
	char pszBlock[40];

	memset(&pszAnim[0], 0, 40);
	memset(&pszBlock[0], 0, 40);

	bool bTest = false;
	const char* pBegin = GetAnimByIdx(idx - 1);
	if (!pBegin)
	{
		return;
	}
	while (*pBegin)
	{
		if (*pBegin == ':')
		{
			pBegin++;
			bTest = true;
			continue;
		}
		if (!bTest)
		{
			szBlock += *pBegin;
		}
		if (bTest)
		{
			szAnim += *pBegin;
		}
		pBegin++;
	}

	strcpy(&pszAnim[0], szAnim.c_str());
	strcpy(&pszBlock[0], szBlock.c_str());
	ApplyAnimation(&pszAnim[0], &pszBlock[0], BlendData, loop, 1, 1, freeze, time);
}

bool IsBlendAssocGroupLoaded(int iGroup)
{
	uintptr_t* pBlendAssocGroup = *(uintptr_t * *)(g_libGTASA + 0x00942184); // CAnimManager::ms_aAnimAssocGroups
	uintptr_t blendAssoc = (uintptr_t)pBlendAssocGroup;
	blendAssoc += (iGroup * 20);
	pBlendAssocGroup = (uintptr_t*)blendAssoc;
	return *(pBlendAssocGroup) != NULL;
}

void CPlayerPed::SetMoveAnim(int iAnimGroup)
{
	Log("SetMoveAnim %d", iAnimGroup);

	if(iAnimGroup == ANIM_GROUP_DEFAULT)
	{
		auto pModel = reinterpret_cast<CPedModelInfo*>(CModelInfo::GetModelInfo(m_pPed->nModelIndex));
		iAnimGroup = pModel->m_nAnimType;
	}

	// Find which anim block to load
	const char* strBlockName = nullptr;
	switch (iAnimGroup)
	{
	case 55:
	case 58:
	case 61:
	case 64:
	case 67:
		strBlockName = "fat";
		break;

	case 56:
	case 59:
	case 62:
	case 65:
	case 68:
		strBlockName = "muscular";
		break;

	case 138:
		strBlockName = "skate";
		break;

	default:
		strBlockName = "ped";
		break;
	}
	if (!strBlockName)
	{
		return;
	}

	if (!IsBlendAssocGroupLoaded(iAnimGroup))
	{
		Log("Animgrp %d not loaded", iAnimGroup);
		uintptr_t pAnimBlock = ((uintptr_t(*)(const char*))(g_libGTASA + 0x0038DEA4 + 1))(strBlockName);
		if (!pAnimBlock)
		{
			return;
		}
		uint8_t bLoaded = *((uint8_t*)pAnimBlock + 16);
		if (!bLoaded)
		{
			uintptr_t animBlocks = (uintptr_t)(g_libGTASA + 0x00940AFC);
			uintptr_t idx = (pAnimBlock - animBlocks) / 32;

			uintptr_t modelId = idx + 25575;
			Log("trying to load modelid %u", modelId);
			if (!pGame->IsModelLoaded(modelId))
			{
				CStreaming::RequestModel(modelId, STREAMING_GAME_REQUIRED);
				CStreaming::LoadAllRequestedModels(false);
				int tries = 0;
				while (!pGame->IsModelLoaded(modelId) && tries <= 10)
				{
					usleep(10);
					tries++;
				}
			}
		}
		if (!IsBlendAssocGroupLoaded(iAnimGroup))
		{
			Log("not loaded %d", iAnimGroup);
			return;
		}
		Log("animgrp %d loaded", iAnimGroup);
	}
	m_pPed->m_motionAnimGroup = static_cast<AssocGroupId>(iAnimGroup);

//	uintptr_t ped = (uintptr_t)m_pPed;
//	*(int*)(ped + 1244) = iAnimGroup;

	((void(*)(CPedGta* thiz))(g_libGTASA + 0x004C668C + 1))(m_pPed); // ReApplyMoveAnims
}



// 0.3.7
bool CPlayerPed::IsDead()
{
	
	if(!m_pPed) return true;
	if(m_pPed->fHealth > 0.0f) return false;
	return true;
}

// 0.3.7
void CPlayerPed::ShowMarker(uint32_t iMarkerColorID)
{
	if(m_dwArrow) HideMarker();
	ScriptCommand(&create_arrow_above_actor, m_dwGTAId, &m_dwArrow);
	ScriptCommand(&set_marker_color, m_dwArrow, iMarkerColorID);
	ScriptCommand(&show_on_radar2, m_dwArrow, 2);
}

// 0.3.7
void CPlayerPed::HideMarker()
{
	if(m_dwArrow) ScriptCommand(&disable_marker, m_dwArrow);
	m_dwArrow = 0;
}

void CPlayerPed::SetFightingStyle(int iStyle)
{
	
	if(!m_pPed || !m_dwGTAId) return;
	if (!CUtil::GetPoolPed(m_dwGTAId))
	{
		return;
	}
	//CChatWindow::AddDebugMessage("set fighting style %d", iStyle);
	ScriptCommand( &set_fighting_style, m_dwGTAId, iStyle, 6 );
}

// 0.3.7

void CPlayerPed::ApplyAnimation(char* szAnimName, char* szAnimFile, float fDelta, bool bLoop, bool bLockX, bool bLockY, bool bFreeze, int uiTime)
{

    if (!m_pPed) return;
    if (!CUtil::GetPoolPed(m_dwGTAId)) return;

    if (!strcasecmp(szAnimFile, "SEX")) return;

	if(!pGame->IsAnimationLoaded(szAnimFile)) {
		CGame::RequestAnimation(szAnimFile);

		ScriptCommand(&apply_animation, m_dwGTAId, szAnimName, szAnimFile, fDelta, bLoop, bLockX, bLockY, bFreeze, uiTime);
	//	usleep(100000);
	}

//	animFlagTime = (uint8_t)uiTime;
//	animFlagFreeze = bFreeze;
//	animFlagLoop = bLoop;

	ScriptCommand(&apply_animation, m_dwGTAId, szAnimName, szAnimFile, fDelta, bLoop, bLockX, bLockY, bFreeze, uiTime);
}

PLAYERID CPlayerPed::FindDeathResponsiblePlayer()
{
	CPlayerPool *pPlayerPool;
	CVehiclePool *pVehiclePool;
	PLAYERID PlayerIDWhoKilled = INVALID_PLAYER_ID;

	if(pNetGame)
	{
		pVehiclePool = pNetGame->GetVehiclePool();
		pPlayerPool = pNetGame->GetPlayerPool();
	}
	else
	{ // just leave if there's no netgame.
		return INVALID_PLAYER_ID;
	}

	if(m_pPed)
	{
		if(m_pPed->m_pDamageEntity)
		{
			PlayerIDWhoKilled = pPlayerPool->FindRemotePlayerIDFromGtaPtr((CPedGta *)m_pPed->m_pDamageEntity);
			if(PlayerIDWhoKilled != INVALID_PLAYER_ID)
			{
				// killed by another player with a weapon, this is all easy.
				return PlayerIDWhoKilled;
			}
			else
			{
				if(pVehiclePool->findSampIdFromGtaPtr((CVehicleGta *) m_pPed->m_pDamageEntity) != INVALID_VEHICLE_ID)
				{
					CVehicleGta *pGtaVehicle = (CVehicleGta *)m_pPed->m_pDamageEntity;
					PlayerIDWhoKilled = pPlayerPool->FindRemotePlayerIDFromGtaPtr((CPedGta *)pGtaVehicle->pDriver);

					if(PlayerIDWhoKilled != INVALID_PLAYER_ID)
					{
						return PlayerIDWhoKilled;
					}
				}
			}
		}
	}
	return INVALID_PLAYER_ID;
}

// 0.3.7
void CPlayerPed::GetBonePosition(int iBoneID, CVector* vecOut)
{
	if(!m_pPed) return;

	(( void (*)(CPedGta*, CVector*, int, int))(g_libGTASA + 0x004A4B0C + 1))(m_pPed, vecOut, iBoneID, 0);
}

CEntityGta* CPlayerPed::GetEntityUnderPlayer()
{
	uintptr_t entity;
	CVector vecStart;
	CVector vecEnd;
	char buf[100];

	if(!m_pPed) return nullptr;
	if( m_pPed->bInVehicle || !CUtil::GetPoolPed(m_dwGTAId))
		return 0;

	vecStart.x = m_pPed->mat->pos.x;
	vecStart.y = m_pPed->mat->pos.y;
	vecStart.z = m_pPed->mat->pos.z - 0.25f;

	vecEnd.x = m_pPed->mat->pos.x;
	vecEnd.y = m_pPed->mat->pos.y;
	vecEnd.z = vecStart.z - 1.75f;

	LineOfSight(&vecStart, &vecEnd, (void*)buf, (uintptr_t)&entity,
		0, 1, 0, 1, 0, 0, 0, 0);

	return (CEntityGta*)entity;
}
void CPlayerPed::ClumpUpdateAnimations(float step, int flag)
{
	if (m_pPed)
	{
		auto pRpClump = m_pEntity->m_pRpClump;
		if (pRpClump)
		{
			((void (*)(RpClump*, float, int))(g_libGTASA + 0x0038BF50 + 1))(pRpClump, step, flag);
		}
	}
}

uint8_t CPlayerPed::GetExtendedKeys()
{
	uint8_t result = 0;
	if (LocalPlayerKeys.bKeys[ePadKeys::KEY_YES])
	{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_YES] = false;
		result = 1;
	}
	if (LocalPlayerKeys.bKeys[ePadKeys::KEY_NO])
	{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_NO] = false;
		result = 2;
	}
	if (LocalPlayerKeys.bKeys[ePadKeys::KEY_CTRL_BACK])
	{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_CTRL_BACK] = false;
		result = 3;
	}

	return result;
}

uint16_t CPlayerPed::GetKeys(uint16_t *lrAnalog, uint16_t *udAnalog)
{
	*lrAnalog = LocalPlayerKeys.wKeyLR;
	*udAnalog = LocalPlayerKeys.wKeyUD;
	uint16_t wRet = 0;

	// KEY_ANALOG_RIGHT
	if (LocalPlayerKeys.bKeys[ePadKeys::KEY_ANALOG_RIGHT]) wRet |= 1;
	wRet <<= 1;
	// KEY_ANALOG_LEFT
	if (LocalPlayerKeys.bKeys[ePadKeys::KEY_ANALOG_LEFT]) wRet |= 1;
	wRet <<= 1;
	// KEY_ANALOG_DOWN
	if (LocalPlayerKeys.bKeys[ePadKeys::KEY_ANALOG_DOWN]) wRet |= 1;
	wRet <<= 1;
	// KEY_ANALOG_UP
	if (LocalPlayerKeys.bKeys[ePadKeys::KEY_ANALOG_UP]) wRet |= 1;
	wRet <<= 1;
	// KEY_WALK
	if (LocalPlayerKeys.bKeys[ePadKeys::KEY_WALK]) wRet |= 1;
	wRet <<= 1;
	// KEY_SUBMISSION
	if (LocalPlayerKeys.bKeys[ePadKeys::KEY_SUBMISSION]) wRet |= 1;
	wRet <<= 1;
	// KEY_LOOK_LEFT
	if (LocalPlayerKeys.bKeys[ePadKeys::KEY_LOOK_LEFT]) wRet |= 1;
	wRet <<= 1;

	if (GetCameraMode() == 0x35)
	{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_HANDBRAKE] = 1;
	}
	else
	{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_HANDBRAKE] = 0;
	}

	// KEY_HANDBRAKE
	if(LocalPlayerKeys.bKeys[ePadKeys::KEY_HANDBRAKE]/*true*/) wRet |= 1;
	wRet <<= 1;
	// KEY_LOOK_RIGHT
	if(LocalPlayerKeys.bKeys[ePadKeys::KEY_LOOK_RIGHT]) wRet |= 1;
	wRet <<= 1;
	// KEY_JUMP
	if(LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP]) wRet |= 1;
	wRet <<= 1;
	// KEY_SECONDARY_ATTACK
	if(LocalPlayerKeys.bKeys[ePadKeys::KEY_SECONDARY_ATTACK]) wRet |= 1;
	wRet <<= 1;
	// KEY_SPRINT
	if(LocalPlayerKeys.bKeys[ePadKeys::KEY_SPRINT]) wRet |= 1;
	wRet <<= 1;
	// KEY_FIRE
	if(LocalPlayerKeys.bKeys[ePadKeys::KEY_FIRE]) wRet |= 1;
	wRet <<= 1;
	// KEY_CROUCH
	if(LocalPlayerKeys.bKeys[ePadKeys::KEY_CROUCH]) wRet |= 1;
	wRet <<= 1;
	// KEY_ACTION
	if(LocalPlayerKeys.bKeys[ePadKeys::KEY_ACTION]) wRet |= 1;

	memset(LocalPlayerKeys.bKeys, 0, ePadKeys::SIZE);

	return wRet;
}

CWeaponGta* CPlayerPed::GetCurrentWeaponSlot()
{
	if (m_pPed) 
	{
		return &m_pPed->WeaponSlots[m_pPed->byteCurWeaponSlot];
	}
	return nullptr;
}


void CPlayerPed::SetKeys(uint16_t wKeys, uint16_t lrAnalog, uint16_t udAnalog)
{
	PAD_KEYS *pad = &RemotePlayerKeys[m_bytePlayerNumber];

	// LEFT/RIGHT
	pad->wKeyLR = lrAnalog;
	// UP/DOWN
	pad->wKeyUD = udAnalog;

	// KEY_ACTION
	pad->bKeys[ePadKeys::KEY_ACTION] = (wKeys & 1);
	wKeys >>= 1;
	// KEY_CROUCH
	pad->bKeys[ePadKeys::KEY_CROUCH] = (wKeys & 1);
	wKeys >>= 1;
	// KEY_FIRE
	pad->bKeys[ePadKeys::KEY_FIRE] = (wKeys & 1);
	wKeys >>= 1;
	// KEY_SPRINT
	pad->bKeys[ePadKeys::KEY_SPRINT] = (wKeys & 1);
	wKeys >>= 1;
	// KEY_SECONDARY_ATTACK
	pad->bKeys[ePadKeys::KEY_SECONDARY_ATTACK] = (wKeys & 1);
	wKeys >>= 1;
	// KEY_JUMP
	pad->bKeys[ePadKeys::KEY_JUMP] = (wKeys & 1);
	if(!pad->bKeys[ePadKeys::KEY_JUMP]) pad->bIgnoreJump = false;
	wKeys >>= 1;
	// KEY_LOOK_RIGHT
	pad->bKeys[ePadKeys::KEY_LOOK_RIGHT] = (wKeys & 1);
	wKeys >>= 1;
	// KEY_HANDBRAKE
	pad->bKeys[ePadKeys::KEY_HANDBRAKE] = (wKeys & 1);
	wKeys >>= 1;
	// KEY_LOOK_LEFT
	pad->bKeys[ePadKeys::KEY_LOOK_LEFT] = (wKeys & 1);
	wKeys >>= 1;
	// KEY_SUBMISSION
	pad->bKeys[ePadKeys::KEY_SUBMISSION] = (wKeys & 1);
	wKeys >>= 1;
	// KEY_WALK
	pad->bKeys[ePadKeys::KEY_WALK] = (wKeys & 1);
	wKeys >>= 1;
	// KEY_ANALOG_UP
	pad->bKeys[ePadKeys::KEY_ANALOG_UP] = (wKeys & 1);
	wKeys >>= 1;
	// KEY_ANALOG_DOWN
	pad->bKeys[ePadKeys::KEY_ANALOG_DOWN] = (wKeys & 1);
	wKeys >>= 1;
	// KEY_ANALOG_LEFT
	pad->bKeys[ePadKeys::KEY_ANALOG_LEFT] = (wKeys & 1);
	wKeys >>= 1;
	// KEY_ANALOG_RIGHT
	pad->bKeys[ePadKeys::KEY_ANALOG_RIGHT] = (wKeys & 1);

	return;
}

void CPlayerPed::SetAimZ(float fAimZ)
{
	if (!m_pPed)
	{
		return;
	}
	*(float*)(*((uintptr_t*)m_pPed + 272) + 84) = fAimZ;
	//m_pPed + 272 - dwPlayerInfo
}

float CPlayerPed::GetAimZ()
{
	if (!m_pPed)
	{
		return 0.0f;
	}
	return *(float*)(*((uintptr_t*)m_pPed + 272) + 84);
}

void CPlayerPed::ProcessSpecialAction(BYTE byteSpecialAction) {

	if (byteSpecialAction == SPECIAL_ACTION_CARRY && !IsAnimationPlaying("CRRY_PRTIAL"))
	{
		ApplyAnimation("CRRY_PRTIAL", "CARRY", 4.1, 0, 0, 0, 1, 1);
		IsActionCarry = true;
	}
	if(IsActionCarry && byteSpecialAction != SPECIAL_ACTION_CARRY)
	{
		ApplyAnimation("crry_prtial", "CARRY", 4.00, false, false, false, false, 1);
		//ClearAnimations();
		//ApplyAnimation("CRRY_PRTIAL", "CARRY", 4.0, 0, 0, 0, 0, 0);
		IsActionCarry = false;
	}

    // pissing:start
    if(!m_iPissingState && byteSpecialAction == SPECIAL_ACTION_PISSING) {
        ApplyAnimation("PISS_LOOP", "PAULNMAC", 4.0f, 1, true, true, true, -1);

        //opcode_066a('PETROLCAN', lhActor0, 0.0, 0.58, -0.08, 0.0, 0.01, 0.0, 1, l000f);

        ScriptCommand(&attach_particle_to_actor2,"PETROLCAN",m_dwGTAId,
                      0.0f, 0.58f, -0.08f, 0.0f, 0.01f, 0.0f, 1, &m_dwPissParticlesHandle);

        ScriptCommand(&make_particle_visible,m_dwPissParticlesHandle);

        m_iPissingState = 1;
    }

    // pissing:stop
    if(m_iPissingState && byteSpecialAction != SPECIAL_ACTION_PISSING) {
        if(m_dwPissParticlesHandle) {
            ScriptCommand(&destroy_particle,m_dwPissParticlesHandle);
            m_dwPissParticlesHandle = 0;
        }
        ClearAnimations();
        m_iPissingState = 0;
    }
}

void CPlayerPed::ProcessBulletData(BULLET_DATA* btData)
{
	if (!m_pPed || !CUtil::GetPoolPed(m_dwGTAId)) {
		return;
	}

	BULLET_SYNC bulletSyncData;

	if (btData) {
		m_bHaveBulletData = true;
		m_bulletData.pEntity = btData->pEntity;
		m_bulletData.vecOrigin.x = btData->vecOrigin.x;
		m_bulletData.vecOrigin.y = btData->vecOrigin.y;
		m_bulletData.vecOrigin.z = btData->vecOrigin.z;

		m_bulletData.vecPos.x = btData->vecPos.x;
		m_bulletData.vecPos.y = btData->vecPos.y;
		m_bulletData.vecPos.z = btData->vecPos.z;

		m_bulletData.vecOffset.x = btData->vecOffset.x;
		m_bulletData.vecOffset.y = btData->vecOffset.y;
		m_bulletData.vecOffset.z = btData->vecOffset.z;

		uint8_t byteHitType = 0;
		unsigned short InstanceID = 0xFFFF;

		if (m_bytePlayerNumber == 0)
		{
			if (pNetGame)
			{
				CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
				if (pPlayerPool)
				{
					CPlayerPed* pPlayerPed = pPlayerPool->GetLocalPlayer()->GetPlayerPed();
					if (pPlayerPed)
					{
						memset(&bulletSyncData, 0, sizeof(BULLET_SYNC));
						if (pPlayerPed->GetCurrentWeapon() != WEAPON_SNIPERRIFLE || btData->pEntity)
						{
							if (btData->pEntity)
							{
								CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
								CObjectPool* pObjectPool = pNetGame->GetObjectPool();

								uint16_t PlayerID;
								uint16_t VehicleID;
								uint16_t ObjectID;

								if (pVehiclePool && pObjectPool)
								{
									PlayerID = pPlayerPool->FindRemotePlayerIDFromGtaPtr((CPedGta*)btData->pEntity);
									if (PlayerID == INVALID_PLAYER_ID)
									{
										VehicleID = pVehiclePool->findSampIdFromGtaPtr(
												(CVehicleGta *) btData->pEntity);
										if (VehicleID == INVALID_VEHICLE_ID)
										{
											ObjectID = pObjectPool->FindIDFromGtaPtr(btData->pEntity);
											if (ObjectID == INVALID_OBJECT_ID)
											{
												CVector vecOut;
												vecOut.x = 0.0f;
												vecOut.y = 0.0f;
												vecOut.z = 0.0f;

												if (btData->pEntity->mat)
												{
													ProjectMatrix(&vecOut, btData->pEntity->mat->ToRwMatrix(), &btData->vecOffset);
													btData->vecOffset.x = vecOut.x;
													btData->vecOffset.y = vecOut.y;
													btData->vecOffset.z = vecOut.z;
												}
												else
												{
													btData->vecOffset.x = btData->pEntity->mat->pos.x + btData->vecOffset.x;
													btData->vecOffset.y = btData->pEntity->mat->pos.y + btData->vecOffset.y;
													btData->vecOffset.z = btData->pEntity->mat->pos.z + btData->vecOffset.z;
												}
											}
											else
											{
												// object
												byteHitType = BULLET_HIT_TYPE_OBJECT;
												InstanceID = ObjectID;
											}
										}
										else
										{
											// vehicle
											byteHitType = BULLET_HIT_TYPE_VEHICLE;
											InstanceID = VehicleID;
										}
									}
									else
									{
										// player
										byteHitType = BULLET_HIT_TYPE_PLAYER;
										InstanceID = PlayerID;
									}
								}
							}

							bulletSyncData.vecOrigin.x = btData->vecOrigin.x;
							bulletSyncData.vecOrigin.y = btData->vecOrigin.y;
							bulletSyncData.vecOrigin.z = btData->vecOrigin.z;

							bulletSyncData.vecPos.x = btData->vecPos.x;
							bulletSyncData.vecPos.y = btData->vecPos.y;
							bulletSyncData.vecPos.z = btData->vecPos.z;

							bulletSyncData.vecOffset.x = btData->vecOffset.x;
							bulletSyncData.vecOffset.y = btData->vecOffset.y;
							bulletSyncData.vecOffset.z = btData->vecOffset.z;

							bulletSyncData.byteHitType = byteHitType;
							bulletSyncData.PlayerID = InstanceID;
							bulletSyncData.byteWeaponID = pPlayerPed->GetCurrentWeapon();

							RakNet::BitStream bsBullet;
							bsBullet.Write((char)ID_BULLET_SYNC);
							bsBullet.Write((char*)&bulletSyncData, sizeof(BULLET_SYNC));


							pNetGame->GetRakClient()->Send(&bsBullet, HIGH_PRIORITY, RELIABLE, 0);
						}
					}
				}
			}
		}
	}
	else
	{
		m_bHaveBulletData = false;
		memset(&m_bulletData, 0, sizeof(BULLET_DATA));
	}
}

