//========= Copyright Valve Corporation, All rights reserved. ============//
#include "cbase.h"
#include "ai_hint.h"
#include "ai_behavior.h"
#include "ai_baseactor.h"
#include "ai_behavior_functank.h"
#include "npc_playercompanion.h"
#include "npcevent.h"

ConVar	sk_hls_guard_health("sk_hls_guard_health", "300");

//---------------------------------------------------------
// Activities
class CNPC_BMSGUARD : public CNPC_PlayerCompanion
//---------------------------------------------------------
{
public:
	DECLARE_CLASS(CNPC_BMSGUARD, CNPC_PlayerCompanion);
	DECLARE_SERVERCLASS();
	DECLARE_DATADESC();

	virtual void Precache()
	{
		BaseClass::Precache();
		PrecacheModel(DefaultOrCustomModel("models/barney.mdl"));
		PrecacheScriptSound("NPC_Barney.FootstepLeft");
		PrecacheScriptSound("NPC_Barney.FootstepRight");
		PrecacheScriptSound("Munted.Itm.Mmm");
		PrecacheScriptSound("NPC_BMSGUARD.DIE");
	}

	void Spawn(void);
	virtual Class_T Classify(void) { return CLASS_PLAYER_ALLY; }
	void Weapon_Equip(CBaseCombatWeapon* pWeapon);
	void		GatherEnemyConditions(CBaseEntity* pEnemy);
	bool CreateBehaviors(void);
	void HandleAnimEvent(animevent_t* pEvent);
	bool ShouldLookForBetterWeapon() { return false; }
	void OnChangeRunningBehavior(CAI_BehaviorBase* pOldBehavior, CAI_BehaviorBase* pNewBehavior);
	void DeathSound(const CTakeDamageInfo& info);
	void GatherConditions(const CTakeDamageInfo &info);
	int  OnTakeDamage_Alive(const CTakeDamageInfo &inputInfo);
	void Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value);
	bool GetGameTextSpeechParams(hudtextparms_t& params) { params.r1 = 0; params.g1 = 200; params.b1 = 200; return BaseClass::GetGameTextSpeechParams(params); } //subtitle color

	CAI_FuncTankBehavior		m_FuncTankBehavior;
	COutputEvent				m_OnPlayerUse;
	COutputEvent				m_OnPlayerPDUse;

	DEFINE_CUSTOM_AI;
};

LINK_ENTITY_TO_CLASS(npc_bms_guard, CNPC_BMSGUARD);
LINK_ENTITY_TO_CLASS(monster_barney, CNPC_BMSGUARD);
IMPLEMENT_SERVERCLASS_ST(CNPC_BMSGUARD, DT_NPC_BMSGUARD)
END_SEND_TABLE()

// Save/Restore
BEGIN_DATADESC(CNPC_BMSGUARD)
DEFINE_OUTPUT(m_OnPlayerUse, "OnPlayerUse"),
DEFINE_OUTPUT(m_OnPlayerPDUse, "OnPlayerPDUse"),
DEFINE_USEFUNC(Use),
END_DATADESC()

void CNPC_BMSGUARD::Spawn(void)
{
	BaseClass::Spawn();
	Precache();

	SetModel(DefaultOrCustomModel("models/barney.mdl"));

	m_iHealth = sk_hls_guard_health.GetInt();

	NPCInit();
	SetUse(&CNPC_BMSGUARD::Use);
}

void CNPC_BMSGUARD::Weapon_Equip(CBaseCombatWeapon* pWeapon)
{
	BaseClass::Weapon_Equip(pWeapon);
	pWeapon->m_fMinRange1 = 0.0f; // Allow to defend themselves at point-blank range.
}

void CNPC_BMSGUARD::HandleAnimEvent(animevent_t* pEvent)
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

void CNPC_BMSGUARD::GatherEnemyConditions(CBaseEntity* pEnemy)
{
	BaseClass::GatherEnemyConditions(pEnemy);

	if (gpGlobals->curtime - GetLastEnemyTime() > 5)
	{
		if (HasCondition(COND_SEE_ENEMY) && (pEnemy->Classify() != CLASS_BULLSEYE))
		{
			SpeakIfAllowed(TLK_ENEMYSPOTTED);
		}
	}
}

void CNPC_BMSGUARD::DeathSound(const CTakeDamageInfo& info)
{
	SentenceStop();
	EmitSound("NPC_BMSGUARD.DIE"); // Sentences don't play on dead NPCs
}

bool CNPC_BMSGUARD::CreateBehaviors(void)
{
	BaseClass::CreateBehaviors();
	AddBehavior(&m_FuncTankBehavior);

	return true;
}

void CNPC_BMSGUARD::OnChangeRunningBehavior(CAI_BehaviorBase* pOldBehavior, CAI_BehaviorBase* pNewBehavior)
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

void CNPC_BMSGUARD::GatherConditions(const CTakeDamageInfo &info)
{
	BaseClass::GatherConditions();

	if ( GlobalEntity_GetState("gordon_precriminal") && m_bIsFollowing == true )
	{
		Speak(TLK_STOPFOLLOW);
		m_FollowBehavior.SetFollowTarget(NULL);
		m_bIsFollowing = false;
	}
}

int CNPC_BMSGUARD::OnTakeDamage_Alive(const CTakeDamageInfo &inputInfo)
{
	int ret = BaseClass::OnTakeDamage_Alive(inputInfo);

	if (!IsAlive() || m_lifeState == LIFE_DYING)
		return ret;

	if ((inputInfo.GetAttacker()->GetFlags() & FL_CLIENT))
	{
		if (GetEnemy() == NULL)
		{
			if (HasMemory(bits_MEMORY_SUSPICIOUS))
			{
				Msg("BA_MAD!\n" );
				Remember(bits_MEMORY_PROVOKED);
				m_FollowBehavior.SetFollowTarget(NULL);
				m_bIsFollowing = false;
			}
			else
			{
				Msg("BA_SHOT!\n" );
				Remember(bits_MEMORY_SUSPICIOUS);
			}
		}
		else
			Msg("BA_SHOT!\n" );
	}
	return ret;
}

void CNPC_BMSGUARD::Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value)
{
	m_bDontUseSemaphore = true;
	if (GlobalEntity_GetState("gordon_precriminal")){
		SpeakIfAllowed(TLK_PDUSE); // Pre-Criminal/Pre-Disaster Won't talk to you
		m_OnPlayerPDUse.FireOutput(pActivator, pCaller);
	}
	else if (m_afMemory & bits_MEMORY_PROVOKED)
	{
		Msg( "I'm not following you, you evil person!\n" );
		m_FollowBehavior.SetFollowTarget(NULL);
		m_bIsFollowing = false;
	}
	else 
	{
		if ( m_bIsFollowing == true ){
			Speak(TLK_STOPFOLLOW);
			m_FollowBehavior.SetFollowTarget( NULL );
			m_bIsFollowing = false;
		}
		else {
			Speak(TLK_STARTFOLLOW);
			m_FollowBehavior.SetFollowTarget( UTIL_GetLocalPlayer() );
			m_FollowBehavior.SetParameters( AIF_SIMPLE );
			m_bIsFollowing = true;
		}
		m_OnPlayerUse.FireOutput(pActivator, pCaller);
	}
	m_bDontUseSemaphore = false;
}

// Schedules
AI_BEGIN_CUSTOM_NPC(npc_bms_guard, CNPC_BMSGUARD)
AI_END_CUSTOM_NPC()