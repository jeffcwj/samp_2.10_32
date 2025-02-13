#pragma once

#include "entity.h"

enum E_HANDLING_PARAMS
{
	hpMaxSpeed,
	hpAcceleration,
	hpGear,
	hpEngineInertion,
	hpMass,
	hpMassTurn,

	hpBrakeDeceleration,
	hpTractionMultiplier,
	hpTractionLoss,
	hpTractionBias,
	hpSuspensionLowerLimit,
	hpSuspensionBias,

	hpWheelSize,

	hpMax
};

#define EXTRA_COMPONENT_BOOT			10
#define EXTRA_COMPONENT_BONNET			11
#define EXTRA_COMPONENT_BUMP_REAR		12
#define EXTRA_COMPONENT_DEFAULT_DOOR 	13
#define EXTRA_COMPONENT_WHEEL			14
#define EXTRA_COMPONENT_BUMP_FRONT		15

struct SCustomCarShadow
{
	RwTexture* pTexture;
	uint8_t r;
	uint8_t g;
	uint8_t b;
	float fSizeX;
	float fSizeY;
};

void* GetSuspensionLinesFromModel(int nModelIndex, int& numWheels);
uint8_t* GetCollisionDataFromModel(int nModelIndex);

class CVehicle : public CEntity
{
public:
	CVehicle(int iType, float fPosX, float fPosY, float fPosZ, float fRotation = 0.0f, bool bSiren = false);
	~CVehicle();

	void LinkToInterior(int iInterior);
	void SetColor(int iColor1, int iColor2);

	void SetHealth(float fHealth);
	float GetHealth();

	// 0.3.7
	bool IsOccupied();

	// 0.3.7
	void SetInvulnerable(bool bInv);
	// 0.3.7
	bool IsDriverLocalPlayer();
	// 0.3.7
	bool HasSunk();

	void ProcessMarkers();

	void RemoveEveryoneFromVehicle();
	void RemovePassenger(CPedGta *pPed);
	void RemoveDriver(bool bDontTurnOffEngine);
	void SetDoorState(int iState);
	int GetDoorState();

	void UpdateDamageStatus(uint32_t dwPanelDamage, uint32_t dwDoorDamage, uint8_t byteLightDamage, uint8_t byteTireDamage);

	void AttachTrailer();
	void DetachTrailer();
	void SetTrailer(CVehicle* pTrailer);
	CVehicle* GetTrailer();

	unsigned int GetVehicleSubtype();

	void SetEngineState(bool bEnable);
	void SetLightsState(bool iState);
	bool GetLightsState();
	void SetBootAndBonnetState(int iBoot, int iBonnet);

	void RemoveComponent(uint16_t uiComponent);

	void SetComponentVisible(uint8_t group, uint16_t components);
	void SetHandlingData(std::vector<SHandlingData>& vHandlingData);
	void ResetVehicleHandling();

	void SetHeadlightsColor(uint8_t r, uint8_t g, uint8_t b);
	void ProcessHeadlightsColor(uint8_t& r, uint8_t& g, uint8_t& b);
	void SetWheelAlignment(int iWheel, float angle);
	void SetWheelOffset(int iWheel, float offset);
	void SetWheelWidth(float fValue);
	void ProcessWheelsOffset();

	void SetCustomShadow(uint8_t r, uint8_t g, uint8_t b, float fSizeX, float fSizeY, const char* szTex);

private:
	void ProcessWheelOffset(RwFrame* pFrame, bool bLeft, float fValue, int iID);
	void SetComponentAngle(bool bUnk, int iID, float angle);

	void SetComponentVisibleInternal(const char* szComponent, bool bVisible);
	std::string GetComponentNameByIDs(uint8_t group, int subgroup);

	void CopyGlobalSuspensionLinesToPrivate();
public:
	enum eTurnState
	{
		TURN_OFF,
		TURN_LEFT,
		TURN_RIGHT,
		TURN_ALL
	};
	CVehicleGta* 	m_pVehicle;

	// �����������
	CObject*		m_pLeftFrontTurnLighter;
	CObject*		m_pRightFrontTurnLighter;
	CObject*		m_pLeftRearTurnLighter;
	CObject*		m_pRightRearTurnLighter;
	eTurnState 		m_iTurnState;
	bool 			m_bIsOnRightTurnLight;
	bool  			m_bIsOnAllurnLight;
	bool 			m_bIsOnLeftTurnLight;
	bool 			m_bIsOnAllTurnLight;

	// ������ ���
	CObject*		m_pLeftReverseLight;
	CObject*		m_pRightReverseLight;

	bool 			m_bHasSiren;
	bool 			m_bIsSirenOn;
	bool 			m_bIsLocked;
	CVehicle* 		m_pTrailer;
	uint32_t		m_dwMarkerID;
	bool 			m_bIsInvulnerable;
	uint8_t			m_byteObjectiveVehicle; // Is this a special objective vehicle? 0/1
	uint8_t			m_bSpecialMarkerEnabled;

	union
	{
		int32_t m_byteColor1;
		CRGBA color1;
	};
	//uint8_t			m_byteColor1;
	uint8_t			m_byteColor2;
	bool 			m_bColorChanged;

	tHandlingData* m_pCustomHandling;

	void* m_pSuspensionLines;
	bool bHasSuspensionLines;

	//int bEngine;
	int fDoorState;
	//int bLights;

	bool IsTrailer();

	bool m_bLightsOn;
	bool m_bEngineOn;

	// Damage status
	uint8_t			m_byteTyreStatus;
	uint8_t			m_byteLightStatus;
	uint32_t		m_dwDoorStatus;
	uint32_t		m_dwPanelStatus;

	bool HasDamageModel();

	void SetPanelStatus(uint8_t bPanel, uint8_t bPanelStatus);

	uint8_t GetPanelStatus(uint8_t bPanel);

	void SetDoorStatus(uint32_t dwDoorStatus, bool spawnFlyingComponen);

	void SetPanelStatus(uint32_t ulPanelStatus);

	void SetLightStatus(uint8_t bLight, uint8_t bLightStatus);

	uint8_t GetLightStatus(uint8_t bLight);

	void SetLightStatus(uint8_t ucStatus);

	void SetBikeWheelStatus(uint8_t bWheel, uint8_t bTireStatus);

	void SetWheelStatus(eWheelPosition bWheel, uint8_t bTireStatus);

	uint8_t GetBikeWheelStatus(uint8_t bWheel);

	void SetDoorStatus(eDoors bDoor, uint8_t bDoorStatus, bool spawnFlyingComponen);

	uint8_t GetDoorStatus(eDoors bDoor);

	void ProcessDamage();

	void
	GetDamageStatusEncoded(uint8_t *byteTyreFlags, uint8_t *byteLightFlags, uint32_t *dwDoorFlags,
						   uint32_t *dwPanelFlags);

	uint8_t GetWheelStatus(eWheelPosition bWheel);

	bool IsValidGameVehicle();

	void toggleRightTurnLight(bool toggle);

	VEHICLEID getSampId();

	void toggleLeftTurnLight(bool toggle);

	void toggleAllTurnLight(bool toggle);

	void toggleReverseLight(bool toggle);

    int32_t AddVehicleUpgrade(int32_t modelId);
};

enum eVehicleOverrideLightsState {
	NO_CAR_LIGHT_OVERRIDE = 0,
	FORCE_CAR_LIGHTS_OFF = 1,
	FORCE_CAR_LIGHTS_ON = 2
};