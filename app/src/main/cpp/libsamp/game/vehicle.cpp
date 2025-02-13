#include "../main.h"
#include "game.h"

extern CGame* pGame;
#include "..//CDebugInfo.h"
#include "..//net/netgame.h"
#include "game/Core/Vector.h"
#include "game/Models/ModelInfo.h"
#include "StreamingInfo.h"

extern CNetGame* pNetGame;

CVehicle::CVehicle(int iType, float fPosX, float fPosY, float fPosZ, float fRotation, bool bSiren)
{
	Log("CVehicle(%d, %4.f, %4.f, %4.f, %4.f)", iType, fPosX, fPosY, fPosZ, fRotation);

	CDebugInfo::uiStreamedVehicles++;
	RwMatrix mat;
	uint32_t dwRetID = 0;

	m_pCustomHandling = nullptr;

	m_pLeftFrontTurnLighter = nullptr;
	m_pRightFrontTurnLighter = nullptr;
	m_pLeftRearTurnLighter = nullptr;
	m_pRightRearTurnLighter = nullptr;

	m_pLeftReverseLight = nullptr;
	m_pRightReverseLight = nullptr;

	m_pVehicle = nullptr;
	m_dwGTAId = 0;
	m_pTrailer = nullptr;

	// normal vehicle
	if (!pGame->IsModelLoaded(iType)) {
		CStreaming::RequestModel(iType, STREAMING_GAME_REQUIRED);
		CStreaming::LoadAllRequestedModels(false);
		while (!pGame->IsModelLoaded(iType)) usleep(10);
	}
	m_bHasSiren = false;

	m_pVehicle = CHook::CallFunction<CVehicleGta*>(g_libGTASA + 0x002EBE04 + 1, iType, CVector(fPosX, fPosY, fPosZ), false);
	dwRetID = GamePool_Vehicle_GetIndex(m_pVehicle);
	//ScriptCommand(&create_car, iType, fPosX, fPosY, fPosZ, &dwRetID);
	ScriptCommand(&set_car_z_angle, dwRetID, fRotation);
	ScriptCommand(&car_gas_tank_explosion, dwRetID, 0);
	ScriptCommand(&set_car_hydraulics, dwRetID, 0);
	ScriptCommand(&toggle_car_tires_vulnerable, dwRetID, 1);
	ScriptCommand(&set_car_immunities, dwRetID, 0, 0, 0, 0, 0);
//	m_pVehicle = (CVehicleGta*)GamePool_Vehicle_G
//	etAt(dwRetID);
	m_pEntity = m_pVehicle;
	m_dwGTAId = dwRetID;

	if (m_pVehicle) {
		//m_pVehicle->m_nOverrideLights = eVehicleOverrideLightsState::NO_CAR_LIGHT_OVERRIDE;
		m_pVehicle->dwDoorsLocked = 0;
		m_pVehicle->fHealth = 1000.0;
		m_bIsLocked = false;

		GetMatrix(&mat);
		mat.pos.x = fPosX;
		mat.pos.y = fPosY;
		mat.pos.z = fPosZ;

		if (GetVehicleSubtype() != VEHICLE_SUBTYPE_BIKE &&
			GetVehicleSubtype() != VEHICLE_SUBTYPE_PUSHBIKE)
			mat.pos.z += 0.25f;

		SetMatrix(mat);
	}


	m_byteObjectiveVehicle = 0;
	m_bSpecialMarkerEnabled = false;
	m_bIsLocked = false;
	m_dwMarkerID = 0;
	m_bIsInvulnerable = false;

	bHasSuspensionLines = false;
	m_pSuspensionLines = nullptr;
	if (GetVehicleSubtype() == VEHICLE_SUBTYPE_CAR)
	{
		CopyGlobalSuspensionLinesToPrivate();
	}

//
//	RwFrame* pWheelLF = ((RwFrame * (*)(RpClump*, const char*))(g_libGTASA + 0x003856F4 + 1))(m_pVehicle->m_pRpClump, "wheel_lf_dummy"); // GetFrameFromname
//	RwFrame* pWheelRF = ((RwFrame * (*)(RpClump*, const char*))(g_libGTASA + 0x003856F4 + 1))(m_pVehicle->m_pRpClump, "wheel_rf_dummy"); // GetFrameFromname
//	RwFrame* pWheelRB = ((RwFrame * (*)(RpClump*, const char*))(g_libGTASA + 0x003856F4 + 1))(m_pVehicle->m_pRpClump, "wheel_rb_dummy"); // GetFrameFromname
//	RwFrame* pWheelLB = ((RwFrame * (*)(RpClump*, const char*))(g_libGTASA + 0x003856F4 + 1))(m_pVehicle->m_pRpClump, "wheel_lb_dummy"); // GetFrameFromname
//
//	if (pWheelLF && pWheelRF && pWheelRB && pWheelLB)
//	{
//		memcpy(&m_vInitialWheelMatrix[0], (const void*)&(pWheelLF->modelling), sizeof(RwMatrix));
//		memcpy(&m_vInitialWheelMatrix[1], (const void*)&(pWheelRF->modelling), sizeof(RwMatrix));
//		memcpy(&m_vInitialWheelMatrix[2], (const void*)&(pWheelRB->modelling), sizeof(RwMatrix));
//		memcpy(&m_vInitialWheelMatrix[3], (const void*)&(pWheelLB->modelling), sizeof(RwMatrix));
//	}
}



CVehicle::~CVehicle()
{
	if(!m_dwGTAId)return;

	CDebugInfo::uiStreamedVehicles--;
	m_pVehicle = GamePool_Vehicle_GetAt(m_dwGTAId);

	if(!m_pVehicle)return;

	if(IsTrailer()){
		CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
		CVehicle *tmpVeh = pVehiclePool->GetVehicleFromTrailer(this);
		if(tmpVeh)
		{
			ScriptCommand(&detach_trailer_from_cab, m_dwGTAId, tmpVeh->m_dwGTAId);
			tmpVeh->m_pTrailer = nullptr;
		}
	}

	if (m_dwMarkerID) {
		ScriptCommand(&disable_marker, m_dwMarkerID);
		m_dwMarkerID = 0;
	}
	RemoveEveryoneFromVehicle();

	if(m_pTrailer) {
		ScriptCommand(&detach_trailer_from_cab, m_pTrailer->m_dwGTAId, m_dwGTAId);
		m_pTrailer = nullptr;
	}

	if (m_pVehicle->nModelIndex == TRAIN_PASSENGER_LOCO ||
		m_pVehicle->nModelIndex == TRAIN_FREIGHT_LOCO) {
		ScriptCommand(&destroy_train, m_dwGTAId);
	}
	else {
		ScriptCommand(&destroy_car, m_dwGTAId);
	}

	delete m_pCustomHandling;
	m_pCustomHandling = nullptr;

	if (bHasSuspensionLines && m_pSuspensionLines) {
		delete[] m_pSuspensionLines;
		m_pSuspensionLines = nullptr;
		bHasSuspensionLines = false;
	}

	//
	delete m_pLeftFrontTurnLighter;
	m_pLeftFrontTurnLighter = nullptr;

	delete m_pLeftRearTurnLighter;
	m_pLeftRearTurnLighter = nullptr;

	delete m_pRightFrontTurnLighter;
	m_pRightFrontTurnLighter = nullptr;

	delete m_pRightRearTurnLighter;
	m_pRightRearTurnLighter = nullptr;

	//
	if(m_pLeftReverseLight != nullptr)
	{
		delete m_pLeftReverseLight;
		m_pLeftReverseLight = nullptr;
	}
	if(m_pRightReverseLight != nullptr)
	{
		delete m_pRightReverseLight;
		m_pRightReverseLight = nullptr;
	}
}

void CVehicle::toggleRightTurnLight(bool toggle)
{
    m_bIsOnRightTurnLight = toggle;

	CVehicleModelInfo* pModelInfoStart = static_cast<CVehicleModelInfo *>(CModelInfo::GetModelInfo(
			m_pVehicle->nModelIndex));

	CVector* m_avDummyPos = pModelInfoStart->m_pVehicleStruct->m_avDummyPos;

	CVector vecFront;
	// 0 - front light
	vecFront.x = m_avDummyPos[0].x + 0.1f;
	vecFront.y = m_avDummyPos[0].y;
	vecFront.z = m_avDummyPos[0].z;

	CVector vecRear;
	vecRear.x = m_avDummyPos[1].x + 0.1f;
	vecRear.y = m_avDummyPos[1].y;
	vecRear.z = m_avDummyPos[1].z;

	CVector vec;
	vec.x = vec.y = vec.z = 0;

	if(m_pRightFrontTurnLighter != nullptr)
	{
		delete m_pRightFrontTurnLighter;
		m_pRightFrontTurnLighter = nullptr;
	}
	if(m_pRightRearTurnLighter != nullptr)
	{
		delete m_pRightRearTurnLighter;
		m_pRightRearTurnLighter = nullptr;
	}

	if(!toggle) return;

	m_pRightFrontTurnLighter = pGame->NewObject(19294, 0.0, 0.0, 0.0, vec, 300.0);
    m_pRightFrontTurnLighter->AttachToVehicle(getSampId(), &vecFront, &vecFront);

	m_pRightRearTurnLighter = pGame->NewObject(19294, 0.0, 0.0, 0.0, vec, 300.0);
	m_pRightRearTurnLighter->AttachToVehicle(getSampId(), &vecRear, &vecRear);

	m_pRightFrontTurnLighter->ProcessAttachToVehicle(this);
	m_pRightRearTurnLighter->ProcessAttachToVehicle(this);
}

void CVehicle::toggleReverseLight(bool toggle)
{
	CVehicleModelInfo* pModelInfoStart = static_cast<CVehicleModelInfo *>(CModelInfo::GetModelInfo(
			m_pVehicle->nModelIndex));

	CVector* m_avDummyPos = pModelInfoStart->m_pVehicleStruct->m_avDummyPos;

	CVector vecRight;
	vecRight.x = m_avDummyPos[1].x;
	vecRight.y = m_avDummyPos[1].y;
	vecRight.z = m_avDummyPos[1].z;

	CVector vecLeft;
	vecLeft.x = -m_avDummyPos[1].x;
	vecLeft.y = m_avDummyPos[1].y;
	vecLeft.z = m_avDummyPos[1].z;

	CVector vec;
	vec.x = vec.y = vec.z = 0;

	if(m_pLeftReverseLight != nullptr)
	{
		delete m_pLeftReverseLight;
		m_pLeftReverseLight = nullptr;
	}
	if(m_pRightReverseLight != nullptr)
	{
		delete m_pRightReverseLight;
		m_pRightReverseLight = nullptr;
	}

	if(!toggle) return;

	m_pLeftReverseLight = pGame->NewObject(19281, 0.0, 0.0, 0.0, vec, 300.0);
	m_pLeftReverseLight->AttachToVehicle(getSampId(), &vecLeft, &vecLeft);

	m_pRightReverseLight = pGame->NewObject(19281, 0.0, 0.0, 0.0, vec, 300.0);
	m_pRightReverseLight->AttachToVehicle(getSampId(), &vecRight, &vecRight);

	m_pRightReverseLight->ProcessAttachToVehicle(this);
	m_pLeftReverseLight->ProcessAttachToVehicle(this);
}

void CVehicle::toggleLeftTurnLight(bool toggle)
{
    m_bIsOnLeftTurnLight = toggle;

	CVehicleModelInfo* pModelInfoStart = static_cast<CVehicleModelInfo *>(CModelInfo::GetModelInfo(
			m_pVehicle->nModelIndex));

	CVector* m_avDummyPos = pModelInfoStart->m_pVehicleStruct->m_avDummyPos;

    CVector vecFront;
    // 0 - front light
    vecFront.x = -(m_avDummyPos[0].x + 0.1f);
    vecFront.y = m_avDummyPos[0].y;
    vecFront.z = m_avDummyPos[0].z;

    CVector vecRear;
    vecRear.x = -(m_avDummyPos[1].x + 0.1f);
    vecRear.y = m_avDummyPos[1].y;
    vecRear.z = m_avDummyPos[1].z;

    CVector vec;
    vec.x = vec.y = vec.z = 0;

    if(m_pLeftFrontTurnLighter != nullptr)
    {
        delete m_pLeftFrontTurnLighter;
        m_pLeftFrontTurnLighter = nullptr;
    }
    if(m_pLeftRearTurnLighter != nullptr)
    {
        delete m_pLeftRearTurnLighter;
        m_pLeftRearTurnLighter = nullptr;
    }

    if(!toggle) return;

    m_pLeftFrontTurnLighter = pGame->NewObject(19294, 0.0, 0.0, 0.0, vec, 300.0);
    m_pLeftFrontTurnLighter->AttachToVehicle(getSampId(), &vecFront, &vecFront);

    m_pLeftRearTurnLighter = pGame->NewObject(19294, 0.0, 0.0, 0.0, vec, 300.0);
    m_pLeftRearTurnLighter->AttachToVehicle(getSampId(), &vecRear, &vecRear);

    m_pLeftFrontTurnLighter->ProcessAttachToVehicle(this);
    m_pLeftRearTurnLighter->ProcessAttachToVehicle(this);
}

VEHICLEID CVehicle::getSampId()
{
	return pNetGame->GetVehiclePool()->findSampIdFromGtaPtr(m_pVehicle);
}

void CVehicle::LinkToInterior(int iInterior)
{
	if (GamePool_Vehicle_GetAt(m_dwGTAId))
	{
		ScriptCommand(&link_vehicle_to_interior, m_dwGTAId, iInterior);
	}
}

int32_t CVehicle::AddVehicleUpgrade(int32_t modelId)
{
	Log("AddVehicleUpgrade");
	CStreaming::RequestModel(modelId, STREAMING_GAME_REQUIRED);
	CStreaming::LoadAllRequestedModels(false);

	ScriptCommand(&request_car_component, modelId);

	while (!ScriptCommand(&is_component_available, modelId))
	{
		usleep(5);
	}

	return ( ( int32_t(*)(CVehicleGta*, int32_t) )(g_libGTASA + 0x0058C66C + 1) )(m_pVehicle, modelId);
}

void CVehicle::SetColor(int iColor1, int iColor2)
{
//	if (iColor1 >= 256 || iColor1 < 0)
//	{
//		iColor1 = 0;
//	}
//	if (iColor2 >= 256 || iColor2 < 0)
//	{
//		iColor2 = 0;
//	}
	if (m_pVehicle)
	{
		if (GamePool_Vehicle_GetAt(m_dwGTAId))
		{
			m_pVehicle->m_nPrimaryColor = (uint8_t)iColor1;
			m_pVehicle->m_nSecondaryColor = (uint8_t)iColor2;
		}
	}

    //m_byteColor1 = iColor1;
	color1.Set(iColor1);
	m_byteColor2 = (uint8_t)iColor2;
	m_bColorChanged = true;
}

void CVehicle::AttachTrailer()
{
	if (m_pTrailer && GamePool_Vehicle_GetAt(m_pTrailer->m_dwGTAId) )
	{
		ScriptCommand(&put_trailer_on_cab, m_pTrailer->m_dwGTAId, m_dwGTAId);
	}
}

//-----------------------------------------------------------

void CVehicle::DetachTrailer()
{
	if (m_pTrailer && GamePool_Vehicle_GetAt(m_pTrailer->m_dwGTAId))
	{
		ScriptCommand(&detach_trailer_from_cab, m_pTrailer->m_dwGTAId, m_dwGTAId);
	}
	m_pTrailer = nullptr;
}

//-----------------------------------------------------------

void CVehicle::SetTrailer(CVehicle* pTrailer)
{
	m_pTrailer = pTrailer;
}

//-----------------------------------------------------------

CVehicle* CVehicle::GetTrailer()
{
	if (!m_pVehicle) return nullptr;

	return m_pTrailer;
}

void CVehicle::SetHealth(float fHealth)
{
	if (m_pVehicle)
	{
		m_pVehicle->fHealth = fHealth;
	}
}

float CVehicle::GetHealth()
{
	if (m_pVehicle) return m_pVehicle->fHealth;
	else return 0.0f;
}

// 0.3.7
void CVehicle::SetInvulnerable(bool bInv)
{
	if (!m_pVehicle) return;
	if (!GamePool_Vehicle_GetAt(m_dwGTAId)) return;

	if (bInv)
	{
		ScriptCommand(&set_car_immunities, m_dwGTAId, 1, 1, 1, 1, 1);
		ScriptCommand(&toggle_car_tires_vulnerable, m_dwGTAId, 0);
		m_bIsInvulnerable = true;
	}
	else
	{
		ScriptCommand(&set_car_immunities, m_dwGTAId, 0, 0, 0, 0, 0);
		ScriptCommand(&toggle_car_tires_vulnerable, m_dwGTAId, 1);
		m_bIsInvulnerable = false;
	}
}

// 0.3.7
bool CVehicle::IsDriverLocalPlayer()
{
	if (m_pVehicle)
	{
		if ((CPedGta*)m_pVehicle->pDriver == GamePool_FindPlayerPed())
			return true;
	}

	return false;
}

// 0.3.7
bool CVehicle::HasSunk()
{
	if (!m_pVehicle) return false;
	return ScriptCommand(&has_car_sunk, m_dwGTAId);
}

bool IsValidGamePed(CPedGta* pPed);

void CVehicle::RemovePassenger(CPedGta *pPed)
{
	((bool (*)(CVehicleGta*, CPedGta*))(g_libGTASA + 0x00584548 + 1))(m_pVehicle, pPed);
}

void CVehicle::RemoveDriver(bool bDontTurnOffEngine)
{
	((bool (*)(CVehicleGta*, bool))(g_libGTASA + 0x005847CC + 1))(m_pVehicle, bDontTurnOffEngine);
}

void CVehicle::RemoveEveryoneFromVehicle()
{
	Log("RemoveEveryoneFromVehicle");
	if (!m_pVehicle) return;

	if (m_pVehicle->pDriver)
	{
		RemoveDriver(true);
	}

	for(const auto & pPassenger : m_pVehicle->pPassengers)
	{
		RemovePassenger(pPassenger);
	}
}

// 0.3.7
bool CVehicle::IsOccupied()
{
	if (m_pVehicle)
	{
		if (m_pVehicle->pDriver) return true;
		if (m_pVehicle->pPassengers[0]) return true;
		if (m_pVehicle->pPassengers[1]) return true;
		if (m_pVehicle->pPassengers[2]) return true;
		if (m_pVehicle->pPassengers[3]) return true;
		if (m_pVehicle->pPassengers[4]) return true;
		if (m_pVehicle->pPassengers[5]) return true;
		if (m_pVehicle->pPassengers[6]) return true;
	}

	return false;
}

void CVehicle::ProcessMarkers()
{
	if (!m_pVehicle) return;

	if (m_byteObjectiveVehicle)
	{
		if (!m_bSpecialMarkerEnabled)
		{
			if (m_dwMarkerID)
			{
				ScriptCommand(&disable_marker, m_dwMarkerID);
				m_dwMarkerID = 0;
			}

			ScriptCommand(&tie_marker_to_car, m_dwGTAId, 1, 3, &m_dwMarkerID);
			ScriptCommand(&set_marker_color, m_dwMarkerID, 1006);
			ScriptCommand(&show_on_radar, m_dwMarkerID, 3);
			m_bSpecialMarkerEnabled = true;
		}

		return;
	}

	if (!m_byteObjectiveVehicle && m_bSpecialMarkerEnabled)
	{
		if (m_dwMarkerID)
		{
			ScriptCommand(&disable_marker, m_dwMarkerID);
			m_bSpecialMarkerEnabled = false;
			m_dwMarkerID = 0;
		}
	}

	if (GetDistanceFromLocalPlayerPed() < 200.0f && !IsOccupied())
	{
		if (!m_dwMarkerID)
		{
			// show
			ScriptCommand(&tie_marker_to_car, m_dwGTAId, 1, 2, &m_dwMarkerID);
			ScriptCommand(&set_marker_color, m_dwMarkerID, 1004);
		}
	}

	else if (IsOccupied() || GetDistanceFromLocalPlayerPed() >= 200.0f)
	{
		// remove
		if (m_dwMarkerID)
		{
			ScriptCommand(&disable_marker, m_dwMarkerID);
			m_dwMarkerID = 0;
		}
	}
}

void CVehicle::SetDoorState(int iState)
{
	if (!m_pVehicle) return;
	if (iState)
	{
		m_pVehicle->dwDoorsLocked = 2;
		m_bIsLocked = true;
		CVehicle::fDoorState = 1;
	}
	else
	{
		m_pVehicle->dwDoorsLocked = 0;
		m_bIsLocked = false;
		CVehicle::fDoorState = 0;
	}
}

int CVehicle::GetDoorState(){
	return CVehicle::fDoorState;
}

void CVehicle::SetLightsState(bool iState)
{
	if(!m_dwGTAId)return;
	if(!m_pVehicle)return;

	//if (GamePool_Vehicle_GetAt(m_dwGTAId))
	//{


		m_pVehicle->m_nOverrideLights = 0;
		m_pVehicle->m_nVehicleFlags.bLightsOn = iState;
//		ScriptCommand(&FORCE_CAR_LIGHTS, m_dwGTAId, iState ? 2 : 1);
	//}
	m_bLightsOn = iState;
}

bool CVehicle::GetLightsState(){
	return m_bLightsOn;
}

void CVehicle::SetBootAndBonnetState(int iBoot, int iBonnet)
{
	if (GamePool_Vehicle_GetAt(m_dwGTAId) && m_pVehicle)
	{
		if (iBoot == 1)
		{
			SetComponentAngle(1, 17, 1.0f);
		}
		else
		{
			SetComponentAngle(1, 17, 0.0f);
		}

		if (iBonnet == 1)
		{
			SetComponentAngle(0, 16, 1.0f);
		}
		else
		{
			SetComponentAngle(0, 16, 0.0f);
		}
	}
}

void CVehicle::RemoveComponent(uint16_t uiComponent)
{

	int component = (uint16_t)uiComponent;

	if (!m_dwGTAId || !m_pVehicle)
	{
		return;
	}

	if (GamePool_Vehicle_GetAt(m_dwGTAId))
	{
		ScriptCommand(&remove_component, m_dwGTAId, component);
	}
}

void* GetSuspensionLinesFromModel(int nModelIndex, int& numWheels)
{
	uint8_t* pCollisionData = GetCollisionDataFromModel(nModelIndex);

	if (!pCollisionData)
	{
		return nullptr;
	}

	void* pLines = *(void**)(pCollisionData + 16);

	numWheels = *(uint8_t*)(pCollisionData + 6);

	return pLines;
}

uint8_t* GetCollisionDataFromModel(int nModelIndex)
{
	auto* pModelInfoStart = static_cast<CVehicleModelInfo *>(CModelInfo::GetModelInfo(nModelIndex));

	if (!pModelInfoStart)
	{
		return nullptr;
	}

	if (!pModelInfoStart->m_pColModel)
	{
		return nullptr;
	}

	uint8_t* pCollisionData = *(uint8_t * *)(pModelInfoStart->m_pColModel + 44);

	return pCollisionData;
}
void CVehicle::SetHandlingData(std::vector<SHandlingData>& vHandlingData)
{
	if (!m_pVehicle || !m_dwGTAId)
	{
		return;
	}
	if (!GamePool_Vehicle_GetAt(m_dwGTAId))
	{
		return;
	}

	if (GetVehicleSubtype() != VEHICLE_SUBTYPE_CAR)
	{
		return;
	}


	if (!m_pCustomHandling)
	{
		m_pCustomHandling = new tHandlingData;
	}

	auto pModelInfoStart = static_cast<CVehicleModelInfo *>(CModelInfo::GetModelInfo(m_pVehicle->nModelIndex));

	if (!pModelInfoStart)
	{
		return;
	}

	//CChatWindow::AddDebugMessage("handling id %d", *(uint16_t*)(pModelInfoStart + 98));

	CHandlingDefault::GetDefaultHandling(pModelInfoStart->m_nHandlingId, m_pCustomHandling);

	/*CChatWindow::AddDebugMessage("mass %f", m_pCustomHandling->m_fMass);
	CChatWindow::AddDebugMessage("turn %f", m_pCustomHandling->m_fTurnMass);
	CChatWindow::AddDebugMessage("m_fEngineAcceleration %f", m_pCustomHandling->m_transmissionData.m_fEngineAcceleration);
	CChatWindow::AddDebugMessage("m_fMaxGearVelocity %f", m_pCustomHandling->m_transmissionData.m_fMaxGearVelocity);
	CChatWindow::AddDebugMessage("flags 0x%x", m_pCustomHandling->m_nHandlingFlags);*/

	bool bNeedRecalculate = false;

	for (auto& i : vHandlingData)
	{
		switch (i.flag)
		{
			case E_HANDLING_PARAMS::hpMaxSpeed:
				m_pCustomHandling->m_transmissionData.m_fMaxGearVelocity = i.fValue * 0.84;
				break;
			case E_HANDLING_PARAMS::hpAcceleration: {
				//float sampSpeed = m_pCustomHandling->m_transmissionData.m_fMaxGearVelocity * 1.2f;
				//m_pCustomHandling->m_transmissionData.m_fEngineAcceleration = sampSpeed / 12.0f;
				m_pCustomHandling->m_transmissionData.m_fEngineAcceleration =  i.fValue;
				break;
			}
			case E_HANDLING_PARAMS::hpEngineInertion:
				m_pCustomHandling->m_transmissionData.m_fEngineInertia = i.fValue;
				break;
			case E_HANDLING_PARAMS::hpGear:

				if (i.fValue == 1)
				{
					m_pCustomHandling->m_transmissionData.m_nDriveType = 'R';
				}

				if (i.fValue == 2)
				{
					m_pCustomHandling->m_transmissionData.m_nDriveType = 'F';
				}

				if (i.fValue == 3)
				{
					m_pCustomHandling->m_transmissionData.m_nDriveType = '4';
				}

				break;
			case E_HANDLING_PARAMS::hpMass:
				m_pCustomHandling->m_fMass = i.fValue;
				break;
			case E_HANDLING_PARAMS::hpMassTurn:
				m_pCustomHandling->m_fTurnMass = i.fValue;
				break;
			case E_HANDLING_PARAMS::hpBrakeDeceleration:
			{
				m_pCustomHandling->m_fBrakeDeceleration = i.fValue;
				break;
			}
			case E_HANDLING_PARAMS::hpTractionMultiplier:
			{
				m_pCustomHandling->m_fTractionMultiplier = i.fValue;
				break;
			}
			case E_HANDLING_PARAMS::hpTractionLoss:
			{
				m_pCustomHandling->m_fTractionLoss = i.fValue;
				break;
			}
			case E_HANDLING_PARAMS::hpTractionBias:
			{
				m_pCustomHandling->m_fTractionBias = i.fValue;
				break;
			}
			case E_HANDLING_PARAMS::hpSuspensionLowerLimit:
			{
				m_pCustomHandling->m_fSuspensionLowerLimit = i.fValue;
				bNeedRecalculate = true;
				break;
			}
			case E_HANDLING_PARAMS::hpSuspensionBias:
			{
				m_pCustomHandling->m_fSuspensionBiasBetweenFrontAndRear = i.fValue;
				bNeedRecalculate = true;
				break;
			}
			case E_HANDLING_PARAMS::hpWheelSize:
			{
				bNeedRecalculate = true;
				break;
			}
		}
	}

	float fOldFrontWheelSize = 0.0f;
	float fOldRearWheelSize = 0.0f;

	/*CChatWindow::AddDebugMessage("AFTER");
	CChatWindow::AddDebugMessage("mass %f", m_pCustomHandling->m_fMass);
	CChatWindow::AddDebugMessage("turn %f", m_pCustomHandling->m_fTurnMass);
	CChatWindow::AddDebugMessage("m_fEngineAcceleration %f", m_pCustomHandling->m_transmissionData.m_fEngineAcceleration);
	CChatWindow::AddDebugMessage("m_fMaxGearVelocity %f", m_pCustomHandling->m_transmissionData.m_fMaxGearVelocity);
	CChatWindow::AddDebugMessage("flags 0x%x", m_pCustomHandling->m_nHandlingFlags);*/

	((void (*)(int, tHandlingData*))(g_libGTASA + 0x00570DC8 + 1))(0, m_pCustomHandling);
	m_pVehicle->pHandling = m_pCustomHandling;

	if (bNeedRecalculate)
	{
		((void (*)(CVehicleGta*))(g_libGTASA + 0x0054EC38 + 1))(m_pVehicle); // CAutomobile::SetupSuspensionLines

		CopyGlobalSuspensionLinesToPrivate();
	}

	if (bNeedRecalculate)
	{
		((void (*)(CVehicleGta*))(g_libGTASA + 0x0055F430 + 1))(m_pVehicle); // process suspension
	}
	//ScriptCommand(&set_car_heavy, m_dwGTAId, 1);
}

void CVehicle::ResetVehicleHandling()
{
	Log("ResetVehicleHandling");
	if (!m_pVehicle || !m_dwGTAId)
	{
		return;
	}
	if (!GamePool_Vehicle_GetAt(m_dwGTAId))
	{
		return;
	}

	if (GetVehicleSubtype() != VEHICLE_SUBTYPE_CAR)
	{
		return;
	}

	if (!m_pCustomHandling)
	{
		m_pCustomHandling = new tHandlingData;
	}

	auto pModelInfoStart = CModelInfo::GetModelInfo(m_pVehicle->nModelIndex);

	if (!pModelInfoStart)
	{
		return;
	}

	CHandlingDefault::GetDefaultHandling(*(uint16_t*)(pModelInfoStart + 98), m_pCustomHandling);

	((void (*)(int, tHandlingData*))(g_libGTASA + 0x00570DC8 + 1))(0, m_pCustomHandling);

	m_pVehicle->pHandling = m_pCustomHandling;

	((void (*)(CVehicleGta*))(g_libGTASA + 0x0054EC38 + 1))(m_pVehicle); // CAutomobile::SetupSuspensionLines
	CopyGlobalSuspensionLinesToPrivate();

	Log("Reseted to defaults");
}

RwObject* GetAllAtomicObjectCB(RwObject* object, void* data)
{

	std::vector<RwObject*>& result = *((std::vector<RwObject*>*) data);
	result.push_back(object);
	return object;
}

// Get all atomics for this frame (even if they are invisible)
void GetAllAtomicObjects(RwFrame* frame, std::vector<RwObject*>& result)
{

	((uintptr_t(*)(RwFrame*, void*, uintptr_t))(g_libGTASA + 0x001D8858 + 1))(frame, (void*)GetAllAtomicObjectCB, (uintptr_t)& result);
}


RwMatrix* RwMatrixMultiplyByVector(CVector* out, RwMatrix* a2, CVector* in);

void CVehicle::SetComponentAngle(bool bUnk, int iID, float angle)
{
	if (GetVehicleSubtype() == VEHICLE_SUBTYPE_CAR)
	{
		((void(*)(CVehicleGta*, int a2, int a3, int a4, float a5, uint8_t a6))(g_libGTASA + 0x005507F4 + 1))(m_pVehicle, 0, iID, bUnk, angle, 1); // CAutomobile::OpenDoor
	}
}


void CVehicle::SetComponentVisibleInternal(const char* szComponent, bool bVisible)
{
	if (!m_pVehicle || !m_dwGTAId)
	{
		return;
	}

	if (!GamePool_Vehicle_GetAt(m_dwGTAId))
	{
		return;
	}

	if (!m_pVehicle->m_pRwObject)
	{
		return;
	}


	RwFrame* pFrame = ((RwFrame * (*)(RpClump*, const char*))(g_libGTASA + 0x003856F4 + 1))(m_pVehicle->m_pRpClump, szComponent); // GetFrameFromname
	if (pFrame != nullptr)
	{
		// Get all atomics for this component - Usually one, or two if there is a damaged version
		std::vector<RwObject*> atomicList;
		GetAllAtomicObjects(pFrame, atomicList);

		// Count number currently visible
		uint uiNumAtomicsCurrentlyVisible = 0;
		for (uint i = 0; i < atomicList.size(); i++)
		{
			if (!atomicList[i])
			{
				continue;
			}
			if (atomicList[i]->flags & 0x04)
			{
				uiNumAtomicsCurrentlyVisible++;
			}
		}

		if (bVisible && uiNumAtomicsCurrentlyVisible == 0)
		{
			// Make atomic (undamaged version) visible. TODO - Check if damaged version should be made visible instead
			for (uint i = 0; i < atomicList.size(); i++)
			{
				RwObject* pAtomic = atomicList[i];
				if (!pAtomic)
				{
					continue;
				}
				int       AtomicId = ((int(*)(RwObject*))(g_libGTASA + 0x005D4B54 + 1))(pAtomic); // CVisibilityPlugins::GetAtomicId

				if (!(AtomicId & ATOMIC_ID_FLAG_TWO_VERSIONS_DAMAGED))
				{
					// Either only one version, or two versions and this is the undamaged one
					pAtomic->flags |= 0x04;
				}
			}
		}
		else if (!bVisible && uiNumAtomicsCurrentlyVisible > 0)
		{
			// Make all atomics invisible
			for (uint i = 0; i < atomicList.size(); i++)
			{
				if (!atomicList[i])
				{
					continue;
				}
				atomicList[i]->flags &= ~0x05;            // Mimic what GTA seems to do - Not sure what the bottom bit is for
			}
		}
	}
}

void CVehicle::CopyGlobalSuspensionLinesToPrivate()
{
	if (GetVehicleSubtype() != VEHICLE_SUBTYPE_CAR)
	{
		return;
	}

	if (!bHasSuspensionLines)
	{
		int numWheels;
		void* pOrigSuspension = GetSuspensionLinesFromModel(m_pVehicle->nModelIndex, numWheels);

		if (pOrigSuspension && numWheels)
		{
			bHasSuspensionLines = true;
			m_pSuspensionLines = new uint8_t[0x20 * numWheels];
		}
	}

	int numWheels;
	void* pOrigSuspension = GetSuspensionLinesFromModel(m_pVehicle->nModelIndex, numWheels);

	if (pOrigSuspension && numWheels)
	{
		memcpy(m_pSuspensionLines, pOrigSuspension, 0x20 * numWheels);
	}
}

void CVehicle::SetEngineState(bool bEnable)
{
	if(!m_dwGTAId)return;
	if(!m_pVehicle)return;
	if (!GamePool_Vehicle_GetAt(m_dwGTAId)) {
		return;
	}
	//m_pVehicle->m_nVehicleFlags.bEngineBroken = 1;
	//m_bEngineOn = bEnable;
	m_pVehicle->m_nVehicleFlags.bEngineOn = m_bEngineOn = bEnable;;
}

bool CVehicle::HasDamageModel()
{
	if (GetVehicleSubtype() == VEHICLE_SUBTYPE_CAR)
		return true;
	return false;
}

uint8_t CVehicle::GetPanelStatus(uint8_t bPanel)
{
	if (m_pVehicle && bPanel < MAX_PANELS)
	{
		uintptr_t* pDamageManager = (uintptr_t*)((uintptr_t)m_pVehicle + 1456);
		return ((uint8_t(*)(uintptr_t, uint8_t))(g_libGTASA + 0x0056E78A + 1))(((uintptr_t)m_pVehicle + 1456), bPanel);
	}
	return 0;
}

void CVehicle::SetPanelStatus(uint8_t bPanel, uint8_t bPanelStatus)
{
	if (m_pVehicle && bPanel < MAX_PANELS && bPanelStatus <= 3)
	{
		if (GetPanelStatus(bPanel) != bPanelStatus)
		{
			uintptr_t* pDamageManager = (uintptr_t*)((uintptr_t)m_pVehicle + 1456);
			((uint8_t(*)(uintptr_t, uint8_t, uint8_t))(g_libGTASA + 0x0056E770 + 1))(((uintptr_t)m_pVehicle + 1456), bPanel, bPanelStatus);

			if (bPanelStatus == DT_PANEL_INTACT)
			{
				// Grab the car node index for the given panel
				static int s_iCarNodeIndexes[7] = { 0x0F, 0x0E, 0x00 /*?*/, 0x00 /*?*/, 0x12, 0x0C, 0x0D };
				int iCarNodeIndex = s_iCarNodeIndexes[bPanel];

				// CAutomobile::FixPanel
				((uint8_t(*)(uintptr_t, uint32_t, uint32_t))(g_libGTASA + 0x0055D7A6 + 1))((uintptr_t)m_pVehicle, iCarNodeIndex, static_cast<uint32_t>(bPanel));
			}
			else
			{
				((uint8_t(*)(uintptr_t, uint32_t, bool))(g_libGTASA + 0x00552CDC + 1))((uintptr_t)m_pVehicle, static_cast<uint32_t>(bPanel), false);
			}
		}
	}
}

uint8_t CVehicle::GetDoorStatus(eDoors bDoor)
{
	if (m_pVehicle && bDoor < MAX_DOORS)
	{
		DAMAGE_MANAGER_INTERFACE* pDamageManager = (DAMAGE_MANAGER_INTERFACE*)((uintptr_t)m_pVehicle + 1456);
		if (pDamageManager) return pDamageManager->Door[bDoor];
	}
	return 0;
}

void CVehicle::SetDoorStatus(eDoors bDoor, uint8_t bDoorStatus, bool spawnFlyingComponen)
{
	if (m_pVehicle && bDoor < MAX_DOORS)
	{
		if (GetDoorStatus(bDoor) != bDoorStatus)
		{
			uintptr_t* pDamageManager = (uintptr_t*)((uintptr_t)m_pVehicle + 1456);
			((uint8_t(*)(uintptr_t, uint8_t, uint8_t, bool))(g_libGTASA + 0x0056E7B0 + 1))(((uintptr_t)m_pVehicle + 1456), bDoor, bDoorStatus, spawnFlyingComponen);

			if (bDoorStatus == DT_DOOR_INTACT || bDoorStatus == DT_DOOR_SWINGING_FREE)
			{
				// Grab the car node index for the given door id
				static int s_iCarNodeIndexes[6] = { 0x10, 0x11, 0x0A, 0x08, 0x0B, 0x09 };
				int iCarNodeIndex = s_iCarNodeIndexes[bDoor];

				// CAutomobile::FixDoor
				((uint8_t(*)(uintptr_t, uint32_t, uint32_t))(g_libGTASA + 0x0055D6AA + 1))((uintptr_t)m_pVehicle, iCarNodeIndex, static_cast<uint32_t>(bDoor));
			}
			else
			{
				bool bQuiet = !spawnFlyingComponen;
				((uint8_t(*)(uintptr_t, uint32_t, bool))(g_libGTASA + 0x00552E3C + 1))((uintptr_t)m_pVehicle, static_cast<uint32_t>(bDoor), bQuiet);
			}
		}
	}
}

void CVehicle::SetDoorStatus(uint32_t dwDoorStatus, bool spawnFlyingComponen)
{
	if (m_pVehicle)
	{
		for (uint8_t uiIndex = 0; uiIndex < MAX_DOORS; uiIndex++)
		{
			SetDoorStatus(static_cast<eDoors>(uiIndex), static_cast<uint8_t>(dwDoorStatus), spawnFlyingComponen);
			dwDoorStatus >>= 8;
		}
	}
}

void CVehicle::SetPanelStatus(uint32_t ulPanelStatus)
{
	if (m_pVehicle)
	{
		for (uint8_t uiIndex = 0; uiIndex < MAX_PANELS; uiIndex++)
		{
			SetPanelStatus(uiIndex, static_cast<uint8_t>(ulPanelStatus));
			ulPanelStatus >>= 4;
		}
	}
}

void CVehicle::SetLightStatus(uint8_t bLight, uint8_t bLightStatus)
{
	if (m_pVehicle && bLight < MAX_LIGHTS)
	{
		uintptr_t* pDamageManager = (uintptr_t*)((uintptr_t)m_pVehicle + 1456);
		((uint8_t(*)(uintptr_t, uint8_t, uint8_t))(g_libGTASA + 0x0056E748 + 1))(((uintptr_t)m_pVehicle + 1456), bLight, bLightStatus);
	}
}

void CVehicle::SetLightStatus(uint8_t ucStatus)
{
	if (m_pVehicle)
	{
		DAMAGE_MANAGER_INTERFACE* pDamageManager = (DAMAGE_MANAGER_INTERFACE*)((uintptr_t)m_pVehicle + 1456);
		if (pDamageManager) pDamageManager->Lights = static_cast<uint32_t>(ucStatus);
	}
}

uint8_t CVehicle::GetLightStatus(uint8_t bLight)
{
	if (m_pVehicle && bLight < MAX_LIGHTS)
	{
		uintptr_t* pDamageManager = (uintptr_t*)((uintptr_t)m_pVehicle + 1456);
		return ((uint8_t(*)(uintptr_t, uint8_t))(g_libGTASA + 0x0056E762 + 1))(((uintptr_t)m_pVehicle + 1456), bLight);
	}
	return 0;
}

uint8_t CVehicle::GetWheelStatus(eWheelPosition bWheel)
{
	if (m_pVehicle && bWheel < MAX_WHEELS)
	{
		return ((uint8_t(*)(uintptr_t, uint8_t))(g_libGTASA + 0x0056E79E + 1))(((uintptr_t)m_pVehicle + 1456), bWheel);
	}
	return 0;
}

void CVehicle::SetWheelStatus(eWheelPosition bWheel, uint8_t bTireStatus)
{
	if (m_pVehicle && bWheel < MAX_WHEELS)
	{
		uintptr_t* pDamageManager = (uintptr_t*)((uintptr_t)m_pVehicle + 1456);
		((uint8_t(*)(uintptr_t, uint8_t, uint8_t))(g_libGTASA + 0x0056E798 + 1))(((uintptr_t)m_pVehicle + 1456), bWheel, bTireStatus);
	}
}

void CVehicle::SetBikeWheelStatus(uint8_t bWheel, uint8_t bTireStatus)
{
	if (m_pVehicle && bWheel < 2)
	{
		if (bWheel == 0)
		{
			*(uint8_t*)((uintptr_t)m_pVehicle + 1644) = bTireStatus;
		}
		else
		{
			*(uint8_t*)((uintptr_t)m_pVehicle + 1645) = bTireStatus;
		}
	}
}

uint8_t CVehicle::GetBikeWheelStatus(uint8_t bWheel)
{
	if (m_pVehicle && bWheel < 2)
	{
		if (bWheel == 0)
		{
			return *(uint8_t*)((uintptr_t)m_pVehicle + 1644);
		}
		else
		{
			return *(uint8_t*)((uintptr_t)m_pVehicle + 1645);
		}
	}
	return 0;
}

void CVehicle::UpdateDamageStatus(uint32_t dwPanelDamage, uint32_t dwDoorDamage, uint8_t byteLightDamage, uint8_t byteTireDamage)
{
	if (HasDamageModel())
	{
		SetPanelStatus(dwPanelDamage);
		SetDoorStatus(dwDoorDamage, false);

		SetLightStatus(eLights::LEFT_HEADLIGHT, byteLightDamage & 1);
		SetLightStatus(eLights::RIGHT_HEADLIGHT, (byteLightDamage >> 2) & 1);
		if ((byteLightDamage >> 6) & 1)
		{
			SetLightStatus(eLights::LEFT_TAIL_LIGHT, 1);
			SetLightStatus(eLights::RIGHT_TAIL_LIGHT, 1);
		}

		SetWheelStatus(eWheelPosition::REAR_RIGHT_WHEEL, byteTireDamage & 1);
		SetWheelStatus(eWheelPosition::FRONT_RIGHT_WHEEL, (byteTireDamage >> 1) & 1);
		SetWheelStatus(eWheelPosition::REAR_LEFT_WHEEL, (byteTireDamage >> 2) & 1);
		SetWheelStatus(eWheelPosition::FRONT_LEFT_WHEEL, (byteTireDamage >> 3) & 1);
	}
	else if (GetVehicleSubtype() == VEHICLE_SUBTYPE_BIKE)
	{
		SetBikeWheelStatus(1, byteTireDamage & 1);
		SetBikeWheelStatus(0, (byteTireDamage >> 1) & 1);
	}
}

unsigned int CVehicle::GetVehicleSubtype()
{
	if (m_pVehicle)
	{
		if (m_pVehicle->vtable == g_libGTASA + 0x0066D678) // 0x871120
		{
			return VEHICLE_SUBTYPE_CAR;
		}
		else if (m_pVehicle->vtable == g_libGTASA + 0x0066DA20) // 0x8721A0
		{
			return VEHICLE_SUBTYPE_BOAT;
		}
		else if (m_pVehicle->vtable == g_libGTASA + 0x0066D7F0) // 0x871360
		{
			return VEHICLE_SUBTYPE_BIKE;
		}
		else if (m_pVehicle->vtable == g_libGTASA + 0x0066DD84) // 0x871948
		{
			return VEHICLE_SUBTYPE_PLANE;
		}
		else if (m_pVehicle->vtable == g_libGTASA + 0x0066DB34) // 0x871680
		{
			return VEHICLE_SUBTYPE_HELI;
		}
		else if (m_pVehicle->vtable == g_libGTASA + 0x0066D908) // 0x871528
		{ // bmx?
			return VEHICLE_SUBTYPE_PUSHBIKE;
		}
		else if (m_pVehicle->vtable == g_libGTASA + 0x0066E0FC) // 0x872370
		{
			return VEHICLE_SUBTYPE_TRAIN;
		}
//		else if (m_pVehicle->vtable == g_libGTASA + 0x0066DFD4) // 0x872370
//		{
//			return VEHICLE_SUBTYPE_TRAILER;
//		}
		//
	}

	return 0;
}

bool CVehicle::IsTrailer()
{
	if(!m_pVehicle)return false;
	if(!m_pVehicle->nModelIndex)return false;

	return ((bool (*)(int)) (g_libGTASA + 0x003863B0 + 1))(m_pVehicle->nModelIndex);
}

void CVehicle::GetDamageStatusEncoded(uint8_t* byteTyreFlags, uint8_t* byteLightFlags, uint32_t* dwDoorFlags, uint32_t* dwPanelFlags)
{
	if (byteTyreFlags) *byteTyreFlags = GetWheelStatus(eWheelPosition::REAR_RIGHT_WHEEL) | (GetWheelStatus(eWheelPosition::FRONT_RIGHT_WHEEL) << 1)
										| (GetWheelStatus(eWheelPosition::REAR_LEFT_WHEEL) << 2) | (GetWheelStatus(eWheelPosition::FRONT_LEFT_WHEEL) << 3);

	if (byteLightFlags) *byteLightFlags = GetLightStatus(eLights::LEFT_HEADLIGHT) | (GetLightStatus(eLights::RIGHT_HEADLIGHT) << 2);
	if (GetLightStatus(eLights::LEFT_TAIL_LIGHT) && GetLightStatus(eLights::RIGHT_TAIL_LIGHT))
		*byteLightFlags |= (1 << 6);

	if (dwDoorFlags) *dwDoorFlags = GetDoorStatus(eDoors::BONNET) | (GetDoorStatus(eDoors::BOOT) << 8) |
									(GetDoorStatus(eDoors::FRONT_LEFT_DOOR) << 16) | (GetDoorStatus(eDoors::FRONT_RIGHT_DOOR) << 24);

	if (dwPanelFlags) *dwPanelFlags = GetPanelStatus(ePanels::FRONT_LEFT_PANEL) | (GetPanelStatus(ePanels::FRONT_RIGHT_PANEL) << 4)
									  | (GetPanelStatus(ePanels::REAR_LEFT_PANEL) << 8) | (GetPanelStatus(ePanels::REAR_RIGHT_PANEL) << 12)
									  | (GetPanelStatus(ePanels::WINDSCREEN_PANEL) << 16) | (GetPanelStatus(ePanels::FRONT_BUMPER) << 20)
									  | (GetPanelStatus(ePanels::REAR_BUMPER) << 24);
}

void CVehicle::ProcessDamage()
{
	if (pNetGame)
	{
		VEHICLEID vehId = pNetGame->GetVehiclePool()->findSampIdFromGtaPtr(m_pVehicle);
		if (vehId != INVALID_VEHICLE_ID)
		{
			if (HasDamageModel())
			{
				uint8_t byteTyreFlags, byteLightFlags;
				uint32_t dwDoorFlags, dwPanelFlags;

				GetDamageStatusEncoded(&byteTyreFlags, &byteLightFlags, &dwDoorFlags, &dwPanelFlags);
				if (byteTyreFlags != m_byteTyreStatus || byteLightFlags != m_byteLightStatus ||
					dwDoorFlags != m_dwDoorStatus || dwPanelFlags != m_dwPanelStatus)
				{
					m_byteLightStatus = byteLightFlags;
					m_byteTyreStatus = byteTyreFlags;
					m_dwDoorStatus = dwDoorFlags;
					m_dwPanelStatus = dwPanelFlags;

					RakNet::BitStream bsDamage;

					bsDamage.Write(vehId);
					bsDamage.Write(dwPanelFlags);
					bsDamage.Write(dwDoorFlags);
					bsDamage.Write(byteLightFlags);
					bsDamage.Write(byteTyreFlags);

					pNetGame->GetRakClient()->RPC(&RPC_VehicleDamage, &bsDamage, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0, false, UNASSIGNED_NETWORK_ID, nullptr);
				}
			}
			else if (GetVehicleSubtype() == VEHICLE_SUBTYPE_BIKE)
			{
				uint8_t byteTyreFlags = GetBikeWheelStatus(1) | (GetBikeWheelStatus(0) << 1);
				if (m_byteTyreStatus != byteTyreFlags)
				{
					m_byteTyreStatus = byteTyreFlags;

					RakNet::BitStream bsDamage;
					bsDamage.Write(vehId);
					bsDamage.Write((uint32_t)0);
					bsDamage.Write((uint32_t)0);
					bsDamage.Write((uint8_t)0);
					bsDamage.Write(byteTyreFlags);

					pNetGame->GetRakClient()->RPC(&RPC_VehicleDamage, &bsDamage, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0, false, UNASSIGNED_NETWORK_ID, nullptr);
				}
			}
		}
	}
}