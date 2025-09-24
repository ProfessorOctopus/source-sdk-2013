//========= Copyright Valve Corporation, All rights reserved. ============//
// Purpose: 
//=============================================================================//
#include "cbase.h"
#include "ai_hint.h"
#include "ai_behavior.h"
#include "ai_baseactor.h"
#include "npc_playercompanion.h"
#include "npcevent.h"
#include "ai_behavior_functank.h"

ConVar	sk_hls_guard_health("sk_hls_guard_health", "0");

//=========================================================
// Activities
//=========================================================
class CMONSTER_BMSGUARD : public CNPC_PlayerCompanion
{
public:
	DECLARE_CLASS(CMONSTER_BMSGUARD, CNPC_PlayerCompanion);
	DECLARE_SERVERCLASS();
	DECLARE_DATADESC();

	virtual void Precache()
	{
		BaseClass::Precache();
		PrecacheModel(DefaultOrCustomModel("models/barney.mdl"));
		PrecacheScriptSound("NPC_Barney.FootstepLeft");
		PrecacheScriptSound("NPC_Barney.FootstepRight");
		PrecacheScriptSound("Munted.Itm.Mmm");
		PrecacheScriptSound("NPC_Barney.Die");
	}

	void Spawn(void);
	virtual Class_T Classify(void) { return CLASS_PLAYER_ALLY; }
	void Weapon_Equip(CBaseCombatWeapon* pWeapon);
	bool CreateBehaviors(void);
	void HandleAnimEvent(animevent_t* pEvent);
	bool ShouldLookForBetterWeapon() { return false; }
	void OnChangeRunningBehavior(CAI_BehaviorBase* pOldBehavior, CAI_BehaviorBase* pNewBehavior);
	void DeathSound(const CTakeDamageInfo& info);
	void GatherConditions();
	virtual bool SpeakIfAllowed(AIConcept_t concept, const char* modifiers = NULL, bool bRespondingToPlayer = false, char* pszOutResponseChosen = NULL, size_t bufsize = 0);
	void Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value);
	bool GetGameTextSpeechParams(hudtextparms_t& params) { params.r1 = 0; params.g1 = 200; params.b1 = 200; return BaseClass::GetGameTextSpeechParams(params); } //subtitle color

	CAI_FuncTankBehavior		m_FuncTankBehavior;
	COutputEvent				m_OnPlayerUse;

	DEFINE_CUSTOM_AI;
};

LINK_ENTITY_TO_CLASS(monster_bms_guard, CMONSTER_BMSGUARD);
IMPLEMENT_SERVERCLASS_ST(CMONSTER_BMSGUARD, DT_MONSTER_BMSGUARD)
END_SEND_TABLE()

//---------------------------------------------------------
// Save/Restore
//---------------------------------------------------------
BEGIN_DATADESC(CMONSTER_BMSGUARD)
DEFINE_OUTPUT(m_OnPlayerUse, "OnPlayerUse"),
DEFINE_USEFUNC(Use),
END_DATADESC()

void CMONSTER_BMSGUARD::Spawn(void)
{
	BaseClass::Spawn();
	Precache();

	SetModel(DefaultOrCustomModel("models/barney.mdl"));

	m_iHealth = sk_hls_guard_health.GetInt();

	NPCInit();
	SetUse(&CMONSTER_BMSGUARD::Use);
}

void CMONSTER_BMSGUARD::Weapon_Equip(CBaseCombatWeapon* pWeapon)
{
	BaseClass::Weapon_Equip(pWeapon);

	if (hl2_episodic.GetBool() && FClassnameIs(pWeapon, "weapon_ar2"))
	{
		// Allow Barney to defend himself at point-blank range in c17_05.
		pWeapon->m_fMinRange1 = 0.0f;
	}
}

void CMONSTER_BMSGUARD::HandleAnimEvent(animevent_t* pEvent)
{
	switch (pEvent->event)
	{
	case NPC_EVENT_LEFTFOOT:
	{
		EmitSound("NPC_Barney.FootstepLeft", pEvent->eventtime);
	}
	break;
	case NPC_EVENT_RIGHTFOOT:
	{
		EmitSound("NPC_Barney.FootstepRight", pEvent->eventtime);
	}
	break;
	default:
		BaseClass::HandleAnimEvent(pEvent);
		break;
	}
}

void CMONSTER_BMSGUARD::DeathSound(const CTakeDamageInfo& info)
{
	SentenceStop(); // Sentences don't play on dead NPCs
	EmitSound("npc_barney.die");
}

bool CMONSTER_BMSGUARD::CreateBehaviors(void)
{
	BaseClass::CreateBehaviors();
	AddBehavior(&m_FuncTankBehavior);
	return true;
}

void CMONSTER_BMSGUARD::OnChangeRunningBehavior(CAI_BehaviorBase* pOldBehavior, CAI_BehaviorBase* pNewBehavior)
{
	if (pNewBehavior == &m_FuncTankBehavior)
	{
		m_bReadinessCapable = false;
	}
	else if (pOldBehavior == &m_FuncTankBehavior)
	{
		m_bReadinessCapable = IsReadinessCapable();
	}
	BaseClass::OnChangeRunningBehavior(pOldBehavior, pNewBehavior);
}

bool CMONSTER_BMSGUARD::SpeakIfAllowed(AIConcept_t concept, const char* modifiers /*= NULL*/, bool bRespondingToPlayer /*= false*/, char* pszOutResponseChosen /*= NULL*/, size_t bufsize /* = 0 */)
{
	if (BaseClass::SpeakIfAllowed(concept, modifiers, bRespondingToPlayer, pszOutResponseChosen, bufsize))
	{
		return true;
	}
	return false;
}

void CMONSTER_BMSGUARD::GatherConditions()
{
	BaseClass::GatherConditions();
}

void CMONSTER_BMSGUARD::Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value)
{
	m_bDontUseSemaphore = true;
	SpeakIfAllowed(TLK_USE);
	m_bDontUseSemaphore = false;

	m_OnPlayerUse.FireOutput(pActivator, pCaller);
}

//-----------------------------------------------------------------------------
// Schedules
//-----------------------------------------------------------------------------
AI_BEGIN_CUSTOM_NPC(monster_bms_guard, CMONSTER_BMSGUARD)
AI_END_CUSTOM_NPC()