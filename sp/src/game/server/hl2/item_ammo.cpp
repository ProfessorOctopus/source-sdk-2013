//========= Copyright Valve Corporation, All rights reserved. ============//
// Purpose: The various Items	
//=============================================================================//
#include "cbase.h"
#include "player.h"
#include "gamerules.h"
#include "items.h"
#include "ammodef.h"
#include "eventlist.h"
#include "npcevent.h"
#include "tier0/memdbgon.h"
#include <props_shared.h>

// ========================================================================
//	>> CItemAmmo
// All ammo items now derive from this for multiplier purposes.
// ========================================================================
class CItemAmmo : public CItem
{
public:
	DECLARE_CLASS( CItemAmmo, CItem );
	DECLARE_DATADESC();

	int ITEM_GiveAmmo(CBasePlayer* pPlayer, float AmmoCount, const char* pszAmmoName)
	{
		int iAmmoType = GetAmmoDef()->Index(pszAmmoName);
		if (iAmmoType == -1)
		{
			Msg("ERROR: Attempting to give unknown ammo type (%s)\n", pszAmmoName);
			return 0;
		}
		return pPlayer->GiveAmmo(AmmoCount, iAmmoType);
	}
};

#define CItem CItemAmmo

extern int gEvilImpulse101;

//Convar
extern ConVar sk_battery;
extern ConVar sk_healthkit;
extern ConVar sk_healthvial;
extern ConVar sk_health_max;
extern ConVar sk_battery_max;
extern ConVar sk_pickup_pistol;
extern ConVar sk_max_pistol;
extern ConVar sk_pickup_smg1;
extern ConVar sk_max_smg1;
extern ConVar sk_pickup_ar2;
extern ConVar sk_max_ar2;
extern ConVar sk_pickup_357;
extern ConVar sk_max_357;
extern ConVar sk_pickup_xbow;
extern ConVar sk_max_xbow;
extern ConVar sk_pickup_flare;
extern ConVar sk_max_flare;
extern ConVar sk_pickup_rpg;
extern ConVar sk_max_rpg;
extern ConVar sk_pickup_smg1_gren;
extern ConVar sk_max_smg1_gren;
extern ConVar sk_pickup_sniper;
extern ConVar sk_max_sniper;
extern ConVar sk_pickup_buckshot;
extern ConVar sk_max_buckshot;
extern ConVar sk_pickup_ar2_gren;
extern ConVar sk_max_ar2_gren;

BEGIN_DATADESC( CItemAmmo )
	DEFINE_KEYFIELD(m_flPickUpAmount, FIELD_FLOAT, "itemval"),
END_DATADESC()

class CItemBattery : public CItem
{
public:
	DECLARE_CLASS(CItemBattery, CItem);

	void Spawn(void)
	{
		Precache();
		SetModel(DefaultOrCustomModel("models/items/battery.mdl"));
		BaseClass::Spawn();
	}

	void Precache(void)
	{
		PrecacheModel(DefaultOrCustomModel("models/items/battery.mdl"));
		PrecacheScriptSound("ItemBattery.Touch");
		if (m_PickupSnd != NULL_STRING) { PrecacheScriptSound(STRING(m_PickupSnd)); }
	}

	void Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value)
	{
		CBasePlayer* pPlayer = static_cast<CBasePlayer*>(pActivator);

		if (pPlayer->ArmorValue() < sk_battery_max.GetFloat() && pPlayer->IsSuitEquipped())// Player can pickup battery with suit.
		{
			CSingleUserRecipientFilter user(pPlayer);
			user.MakeReliable();

			// Set custom Armor Value in Hammer to whatever but if Value is 0 then use default Armor Value from Skill.cge
			if (m_flPickUpAmount == 0) { pPlayer->IncrementArmorValue(sk_battery.GetFloat(), sk_battery_max.GetFloat()); }
			else { pPlayer->IncrementArmorValue(m_flPickUpAmount, sk_battery_max.GetFloat()); }

			if (m_PickupSnd != NULL_STRING)
			{
				CPASAttenuationFilter filter(this);
				EmitSound_t ep;
				ep.m_pSoundName = (char*)STRING(m_PickupSnd);
				EmitSound(filter, pPlayer->entindex(), ep);
			}
			else
			{
				CPASAttenuationFilter filter(pPlayer, "ItemBattery.Touch");
				EmitSound(filter, pPlayer->entindex(), "ItemBattery.Touch");
			}

			if (g_pGameRules->ItemShouldRespawn(this) == GR_ITEM_RESPAWN_NO) { UTIL_Remove(this);}
			if (gEvilImpulse101){UTIL_Remove(this);}

			m_OnCollected.FireOutput(pActivator, this);
		}
		else if (sk_battery_max.GetFloat())// When suit is full, player can pickup item as physics prop.
		{
			pPlayer->PickupObject(this);
			m_OnPickedUp.FireOutput(pActivator, this);
			return;
		}
	}
	DECLARE_DATADESC();
};

LINK_ENTITY_TO_CLASS(item_battery, CItemBattery);
PRECACHE_REGISTER(item_battery);

BEGIN_DATADESC(CItemBattery)
	DEFINE_KEYFIELD(m_flPickUpAmount, FIELD_FLOAT, "itemval"),
	DEFINE_KEYFIELD(m_PickupSnd, FIELD_SOUNDNAME, "message"),

	//Output
	DEFINE_OUTPUT(m_OnCollected, "OnCollected"),
	DEFINE_OUTPUT(m_OnPickedUp, "OnPickedUp"),
END_DATADESC()

// ========================================================================
//	>> HealthVial
// ========================================================================
class CHealthVial : public CItem
{
public:
	DECLARE_CLASS(CHealthVial, CItem);

	void Spawn(void)
	{
		Precache();
		SetModel(DefaultOrCustomModel("models/healthvial.mdl"));
		BaseClass::Spawn();
	}

	void Precache(void)
	{
		PrecacheModel(DefaultOrCustomModel("models/healthvial.mdl"));
		PrecacheScriptSound("HealthVial.Touch");
		if (m_PickupSnd != NULL_STRING) { PrecacheScriptSound(STRING(m_PickupSnd)); }
	}

	void Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value)
	{
		CBasePlayer* pPlayer = static_cast<CBasePlayer*>(pActivator);

		if (pPlayer->GetHealth() < sk_health_max.GetFloat())
		{
			CSingleUserRecipientFilter user(pPlayer);
			user.MakeReliable();

			// Set custom Healthkit Value in Hammer to whatever but if Value is 0 then use default Health Value from Skill.cge
			if (m_flPickUpAmount == 0){pPlayer->TakeHealth(sk_healthvial.GetFloat(), DMG_GENERIC);}
			else{pPlayer->TakeHealth(m_flPickUpAmount, DMG_GENERIC);}

			if (m_PickupSnd != NULL_STRING)
			{
				CPASAttenuationFilter filter(this);
				EmitSound_t ep;
				ep.m_pSoundName = (char*)STRING(m_PickupSnd);
				EmitSound(filter, pPlayer->entindex(), ep);
			}
			else
			{
				CPASAttenuationFilter filter(pPlayer, "HealthVial.Touch");
				EmitSound(filter, pPlayer->entindex(), "HealthVial.Touch");
			}

			if (g_pGameRules->ItemShouldRespawn(this) == GR_ITEM_RESPAWN_NO) { UTIL_Remove(this); }
			if (gEvilImpulse101) { UTIL_Remove(this); }

			m_OnCollected.FireOutput(pActivator, this);
		}
		else if (sk_health_max.GetFloat())// When suit is full, player can pickup item as physics prop.
		{
			pPlayer->PickupObject(this);
			m_OnPickedUp.FireOutput(pActivator, this);
			return;
		}
	}
	DECLARE_DATADESC();
};

LINK_ENTITY_TO_CLASS(item_healthvial, CHealthVial);
PRECACHE_REGISTER(item_healthvial);

BEGIN_DATADESC(CHealthVial)
	DEFINE_KEYFIELD(m_flPickUpAmount, FIELD_FLOAT, "itemval"),
	DEFINE_KEYFIELD(m_PickupSnd, FIELD_SOUNDNAME, "message"),

	//Output
	DEFINE_OUTPUT(m_OnCollected, "OnCollected"),
	DEFINE_OUTPUT(m_OnPickedUp, "OnPickedUp"),
END_DATADESC()

// ========================================================================
//	>> HealthKit
// ========================================================================
class CHealthKit : public CItem
{
public:
	DECLARE_CLASS(CHealthKit, CItem);

	void Spawn(void)
	{
		Precache();
		SetModel(DefaultOrCustomModel("models/items/healthkit.mdl"));
		BaseClass::Spawn();
	}

	void Precache(void)
	{
		PrecacheModel(DefaultOrCustomModel("models/items/healthkit.mdl"));
		PrecacheScriptSound("HealthKit.Touch");
		if (m_PickupSnd != NULL_STRING) { PrecacheScriptSound(STRING(m_PickupSnd)); }
	}

	void Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value)
	{
		CBasePlayer* pPlayer = static_cast<CBasePlayer*>(pActivator);

		if (pPlayer->GetHealth() < sk_health_max.GetFloat())
		{
			CSingleUserRecipientFilter user(pPlayer);
			user.MakeReliable();

			// Set custom Healthkit Value in Hammer to whatever but if Value is 0 then use default Health Value from Skill.cge
			if (m_flPickUpAmount == 0) { pPlayer->TakeHealth(sk_healthkit.GetFloat(), DMG_GENERIC); }
			else { pPlayer->TakeHealth(m_flPickUpAmount, DMG_GENERIC); }

			if (m_PickupSnd != NULL_STRING)
			{
				CPASAttenuationFilter filter(this);
				EmitSound_t ep;
				ep.m_pSoundName = (char*)STRING(m_PickupSnd);
				EmitSound(filter, pPlayer->entindex(), ep);
			}
			else
			{
				CPASAttenuationFilter filter(pPlayer, "HealthKit.Touch");
				EmitSound(filter, pPlayer->entindex(), "HealthKit.Touch");
			}

			if (g_pGameRules->ItemShouldRespawn(this) == GR_ITEM_RESPAWN_NO) { UTIL_Remove(this); }
			if (gEvilImpulse101) { UTIL_Remove(this); }

			m_OnCollected.FireOutput(pActivator, this);
		}
		else if (sk_health_max.GetFloat())// When suit is full, player can pickup item as physics prop.
		{
			pPlayer->PickupObject(this);
			m_OnPickedUp.FireOutput(pActivator, this);
			return;
		}
	}
	DECLARE_DATADESC();
};

LINK_ENTITY_TO_CLASS(item_healthkit, CHealthKit);
PRECACHE_REGISTER(item_healthkit);

BEGIN_DATADESC(CHealthKit)
	DEFINE_KEYFIELD(m_flPickUpAmount, FIELD_FLOAT, "itemval"),
	DEFINE_KEYFIELD(m_PickupSnd, FIELD_SOUNDNAME, "message"),

	//Output
	DEFINE_OUTPUT(m_OnCollected, "OnCollected"),
	DEFINE_OUTPUT(m_OnPickedUp, "OnPickedUp"),
END_DATADESC()

// ========================================================================
//	>> BoxSRounds
// ========================================================================
class CItem_BoxSRounds : public CItem
{
public:
	DECLARE_CLASS( CItem_BoxSRounds, CItem );

	void Spawn( void )
	{ 
		Precache( );
		SetModel( "models/items/boxsrounds.mdl" );
		BaseClass::Spawn( );
	}

	void Precache( void )
	{
		PrecacheModel ("models/items/boxsrounds.mdl");
		PrecacheScriptSound("BaseCombatCharacter.AmmoPickup");
		if (m_PickupSnd != NULL_STRING) { PrecacheScriptSound(STRING(m_PickupSnd)); }
	}

	void Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value)
	{
		CBasePlayer* pPlayer = static_cast<CBasePlayer*>(pActivator);
		
		// Custom Ammo pickup Values for Hammer and Junk
		if ((pPlayer->GetAmmoCount("Pistol") < sk_max_pistol.GetFloat())) // If Ammo is under the max carry capacity for this Ammo type then allow collecting.
		{
			if (m_flPickUpAmount == 0)
			{
				ITEM_GiveAmmo(pPlayer, sk_pickup_pistol.GetFloat(), "Pistol");
			}
			else
			{
				ITEM_GiveAmmo(pPlayer, m_flPickUpAmount, "Pistol");
			}

			if (m_PickupSnd != NULL_STRING)
			{
				CPASAttenuationFilter filter(this);
				EmitSound_t ep;
				ep.m_pSoundName = (char*)STRING(m_PickupSnd);
				EmitSound(filter, pPlayer->entindex(), ep);
			}
			else
			{
				CPASAttenuationFilter filter(pPlayer, "BaseCombatCharacter.AmmoPickup");
				EmitSound(filter, pPlayer->entindex(), "BaseCombatCharacter.AmmoPickup");
			}

			if (g_pGameRules->ItemShouldRespawn(this) == GR_ITEM_RESPAWN_NO) { UTIL_Remove(this); }
			if (gEvilImpulse101) { UTIL_Remove(this); }

			m_OnCollected.FireOutput(pActivator, this);
		}
		else // If Ammo is full then player can pick this up like a normal prop.
		{
			pPlayer->PickupObject(this);
			m_OnPickedUp.FireOutput(pActivator, this);
			return;
		}
	}
	DECLARE_DATADESC();
};

LINK_ENTITY_TO_CLASS(item_box_srounds, CItem_BoxSRounds);
LINK_ENTITY_TO_CLASS(item_ammo_pistol, CItem_BoxSRounds);
LINK_ENTITY_TO_CLASS(item_large_box_srounds, CItem_BoxSRounds);
LINK_ENTITY_TO_CLASS(item_ammo_pistol_large, CItem_BoxSRounds);

BEGIN_DATADESC(CItem_BoxSRounds)
	DEFINE_KEYFIELD(m_flPickUpAmount, FIELD_FLOAT, "itemval"),
	DEFINE_KEYFIELD(m_PickupSnd, FIELD_SOUNDNAME, "message"),

	//Output
	DEFINE_OUTPUT(m_OnCollected, "OnCollected"),
	DEFINE_OUTPUT(m_OnPickedUp, "OnPickedUp"),
END_DATADESC()

// ========================================================================
//	>> BoxMRounds
// ========================================================================
class CItem_BoxMRounds : public CItem
{
public:
	DECLARE_CLASS( CItem_BoxMRounds, CItem );

	void Spawn( void )
	{ 
		Precache( );
		SetModel( "models/items/boxmrounds.mdl");
		BaseClass::Spawn( );
	}

	void Precache( void )
	{
		PrecacheModel ("models/items/boxmrounds.mdl");
		PrecacheScriptSound("BaseCombatCharacter.AmmoPickup");
		if (m_PickupSnd != NULL_STRING) { PrecacheScriptSound(STRING(m_PickupSnd)); }
	}

	void Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value)
	{
		CBasePlayer* pPlayer = static_cast<CBasePlayer*>(pActivator);

		// Custom Ammo pickup Values for Hammer and Junk
		if ((pPlayer->GetAmmoCount("SMG1") < sk_max_smg1.GetFloat())) // If Ammo is under the max carry capacity for this Ammo type then allow collecting.
		{
			if (m_flPickUpAmount == 0)
			{
				ITEM_GiveAmmo(pPlayer, sk_pickup_smg1.GetFloat(), "SMG1");
			}
			else
			{
				ITEM_GiveAmmo(pPlayer, m_flPickUpAmount, "SMG1");
			}

			if (m_PickupSnd != NULL_STRING)
			{
				CPASAttenuationFilter filter(this);
				EmitSound_t ep;
				ep.m_pSoundName = (char*)STRING(m_PickupSnd);
				EmitSound(filter, pPlayer->entindex(), ep);
			}
			else
			{
				CPASAttenuationFilter filter(pPlayer, "BaseCombatCharacter.AmmoPickup");
				EmitSound(filter, pPlayer->entindex(), "BaseCombatCharacter.AmmoPickup");
			}

			if (g_pGameRules->ItemShouldRespawn(this) == GR_ITEM_RESPAWN_NO) { UTIL_Remove(this); }
			if (gEvilImpulse101) { UTIL_Remove(this); }

			m_OnCollected.FireOutput(pActivator, this);
		}
		else // If Ammo is full then player can pick this up like a normal prop.
		{
			pPlayer->PickupObject(this);
			m_OnPickedUp.FireOutput(pActivator, this);
			return;
		}
	}
	DECLARE_DATADESC();
};

LINK_ENTITY_TO_CLASS(item_box_mrounds, CItem_BoxMRounds);
LINK_ENTITY_TO_CLASS(item_ammo_smg1, CItem_BoxMRounds);
LINK_ENTITY_TO_CLASS(item_large_box_mrounds, CItem_BoxMRounds);
LINK_ENTITY_TO_CLASS(item_ammo_smg1_large, CItem_BoxMRounds);

BEGIN_DATADESC(CItem_BoxMRounds)
	DEFINE_KEYFIELD(m_flPickUpAmount, FIELD_FLOAT, "itemval"),
	DEFINE_KEYFIELD(m_PickupSnd, FIELD_SOUNDNAME, "message"),

	//Output
	DEFINE_OUTPUT(m_OnCollected, "OnCollected"),
	DEFINE_OUTPUT(m_OnPickedUp, "OnPickedUp"),
END_DATADESC()

// ========================================================================
//	>> BoxLRounds
// ========================================================================
class CItem_BoxLRounds : public CItem
{
public:
	DECLARE_CLASS( CItem_BoxLRounds, CItem );

	void Spawn( void )
	{ 
		Precache( );
		SetModel( "models/items/combine_rifle_cartridge01.mdl");
		BaseClass::Spawn( );
	}

	void Precache( void )
	{
		PrecacheModel ("models/items/combine_rifle_cartridge01.mdl");
		PrecacheScriptSound("BaseCombatCharacter.AmmoPickup");
		if (m_PickupSnd != NULL_STRING) { PrecacheScriptSound(STRING(m_PickupSnd)); }
	}

	void Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value)
	{
		CBasePlayer* pPlayer = static_cast<CBasePlayer*>(pActivator);

		// Custom Ammo pickup Values for Hammer and Junk
		if ((pPlayer->GetAmmoCount("AR2") < sk_max_ar2.GetFloat())) // If Ammo is under the max carry capacity for this Ammo type then allow collecting.
		{
			if (m_flPickUpAmount == 0)
			{
				ITEM_GiveAmmo(pPlayer, sk_pickup_ar2.GetFloat(), "AR2");
			}
			else
			{
				ITEM_GiveAmmo(pPlayer, m_flPickUpAmount, "AR2");
			}

			if (m_PickupSnd != NULL_STRING)
			{
				CPASAttenuationFilter filter(this);
				EmitSound_t ep;
				ep.m_pSoundName = (char*)STRING(m_PickupSnd);
				EmitSound(filter, pPlayer->entindex(), ep);
			}
			else
			{
				CPASAttenuationFilter filter(pPlayer, "BaseCombatCharacter.AmmoPickup");
				EmitSound(filter, pPlayer->entindex(), "BaseCombatCharacter.AmmoPickup");
			}

			if (g_pGameRules->ItemShouldRespawn(this) == GR_ITEM_RESPAWN_NO) { UTIL_Remove(this); }
			if (gEvilImpulse101) { UTIL_Remove(this); }

			m_OnCollected.FireOutput(pActivator, this);
		}
		else // If Ammo is full then player can pick this up like a normal prop.
		{
			pPlayer->PickupObject(this);
			m_OnPickedUp.FireOutput(pActivator, this);
			return;
		}
	}
	DECLARE_DATADESC();
};

LINK_ENTITY_TO_CLASS(item_box_lrounds, CItem_BoxLRounds);
LINK_ENTITY_TO_CLASS(item_ammo_ar2, CItem_BoxLRounds);
LINK_ENTITY_TO_CLASS(item_large_box_lrounds, CItem_BoxLRounds);
LINK_ENTITY_TO_CLASS(item_ammo_ar2_large, CItem_BoxLRounds);

BEGIN_DATADESC(CItem_BoxLRounds)
	DEFINE_KEYFIELD(m_flPickUpAmount, FIELD_FLOAT, "itemval"),
	DEFINE_KEYFIELD(m_PickupSnd, FIELD_SOUNDNAME, "message"),

	//Output
	DEFINE_OUTPUT(m_OnCollected, "OnCollected"),
	DEFINE_OUTPUT(m_OnPickedUp, "OnPickedUp"),
END_DATADESC()

// ========================================================================
//	>> CItem_Box357Rounds
// ========================================================================
class CItem_Box357Rounds : public CItem
{
public:
	DECLARE_CLASS( CItem_Box357Rounds, CItem );

	void Spawn( void )
	{ 
		Precache( );
		SetModel( "models/items/357ammo.mdl");
		BaseClass::Spawn( );
	}

	void Precache(void)
	{
		PrecacheModel("models/items/357ammo.mdl");
		PrecacheScriptSound("BaseCombatCharacter.AmmoPickup");
		if (m_PickupSnd != NULL_STRING) { PrecacheScriptSound(STRING(m_PickupSnd)); }
	}

	void Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value)
	{
		CBasePlayer* pPlayer = static_cast<CBasePlayer*>(pActivator);

		// Custom Ammo pickup Values for Hammer and Junk
		if ((pPlayer->GetAmmoCount("357") < sk_max_357.GetFloat())) // If Ammo is under the max carry capacity for this Ammo type then allow collecting.
		{
			if (m_flPickUpAmount == 0)
			{
				ITEM_GiveAmmo(pPlayer, sk_pickup_357.GetFloat(), "357");
			}
			else
			{
				ITEM_GiveAmmo(pPlayer, m_flPickUpAmount, "357");
			}

			if (m_PickupSnd != NULL_STRING)
			{
				CPASAttenuationFilter filter(this);
				EmitSound_t ep;
				ep.m_pSoundName = (char*)STRING(m_PickupSnd);
				EmitSound(filter, pPlayer->entindex(), ep);
			}
			else
			{
				CPASAttenuationFilter filter(pPlayer, "BaseCombatCharacter.AmmoPickup");
				EmitSound(filter, pPlayer->entindex(), "BaseCombatCharacter.AmmoPickup");
			}

			if (g_pGameRules->ItemShouldRespawn(this) == GR_ITEM_RESPAWN_NO) { UTIL_Remove(this); }
			if (gEvilImpulse101) { UTIL_Remove(this); }

			m_OnCollected.FireOutput(pActivator, this);
		}
		else // If Ammo is full then player can pick this up like a normal prop.
		{
			pPlayer->PickupObject(this);
			m_OnPickedUp.FireOutput(pActivator, this);
			return;
		}
	}
	DECLARE_DATADESC();
};

LINK_ENTITY_TO_CLASS(item_ammo_357, CItem_Box357Rounds);
LINK_ENTITY_TO_CLASS(item_ammo_357_large, CItem_Box357Rounds);

BEGIN_DATADESC(CItem_Box357Rounds)
	DEFINE_KEYFIELD(m_flPickUpAmount, FIELD_FLOAT, "itemval"),
	DEFINE_KEYFIELD(m_PickupSnd, FIELD_SOUNDNAME, "message"),

	//Output
	DEFINE_OUTPUT(m_OnCollected, "OnCollected"),
	DEFINE_OUTPUT(m_OnPickedUp, "OnPickedUp"),
END_DATADESC()

// ========================================================================
//	>> CItem_BoxXBowRounds
// ========================================================================
class CItem_BoxXBowRounds : public CItem
{
public:
	DECLARE_CLASS( CItem_BoxXBowRounds, CItem );

	void Spawn( void )
	{ 
		Precache( );
		SetModel( "models/items/crossbowrounds.mdl");
		BaseClass::Spawn( );
	}

	void Precache(void)
	{
		PrecacheModel("models/items/crossbowrounds.mdl");
		PrecacheScriptSound("BaseCombatCharacter.AmmoPickup");
		if (m_PickupSnd != NULL_STRING) { PrecacheScriptSound(STRING(m_PickupSnd)); }
	}

	void Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value)
	{
		CBasePlayer* pPlayer = static_cast<CBasePlayer*>(pActivator);

		// Custom Ammo pickup Values for Hammer and Junk
		if ((pPlayer->GetAmmoCount("XBowBolt") < sk_max_xbow.GetFloat())) // If Ammo is under the max carry capacity for this Ammo type then allow collecting.
		{
			if (m_flPickUpAmount == 0)
			{
				ITEM_GiveAmmo(pPlayer, sk_pickup_xbow.GetFloat(), "XBowBolt");
			}
			else
			{
				ITEM_GiveAmmo(pPlayer, m_flPickUpAmount, "XBowBolt");
			}

			if (m_PickupSnd != NULL_STRING)
			{
				CPASAttenuationFilter filter(this);
				EmitSound_t ep;
				ep.m_pSoundName = (char*)STRING(m_PickupSnd);
				EmitSound(filter, pPlayer->entindex(), ep);
			}
			else
			{
				CPASAttenuationFilter filter(pPlayer, "BaseCombatCharacter.AmmoPickup");
				EmitSound(filter, pPlayer->entindex(), "BaseCombatCharacter.AmmoPickup");
			}

			if (g_pGameRules->ItemShouldRespawn(this) == GR_ITEM_RESPAWN_NO) { UTIL_Remove(this); }
			if (gEvilImpulse101) { UTIL_Remove(this); }

			m_OnCollected.FireOutput(pActivator, this);
		}
		else // If Ammo is full then player can pick this up like a normal prop.
		{
			pPlayer->PickupObject(this);
			m_OnPickedUp.FireOutput(pActivator, this);
			return;
		}
	}
	DECLARE_DATADESC();
};

LINK_ENTITY_TO_CLASS(item_ammo_crossbow, CItem_BoxXBowRounds);

BEGIN_DATADESC(CItem_BoxXBowRounds)
	DEFINE_KEYFIELD(m_flPickUpAmount, FIELD_FLOAT, "itemval"),
	DEFINE_KEYFIELD(m_PickupSnd, FIELD_SOUNDNAME, "message"),

	//Output
	DEFINE_OUTPUT(m_OnCollected, "OnCollected"),
	DEFINE_OUTPUT(m_OnPickedUp, "OnPickedUp"),
END_DATADESC()

// ========================================================================
//	>> FlareRound
// ========================================================================
class CItem_FlareRound : public CItem
{
public:
	DECLARE_CLASS( CItem_FlareRound, CItem );

	void Spawn( void )
	{ 
		Precache( );
		SetModel( "models/items/flare.mdl");
		BaseClass::Spawn( );
	}

	void Precache( void )
	{
		PrecacheModel ("models/items/flare.mdl");
		PrecacheScriptSound("BaseCombatCharacter.AmmoPickup");
		if (m_PickupSnd != NULL_STRING) { PrecacheScriptSound(STRING(m_PickupSnd)); }
	}

	void Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value)
	{
		CBasePlayer* pPlayer = static_cast<CBasePlayer*>(pActivator);

		// Custom Ammo pickup Values for Hammer and Junk
		if ((pPlayer->GetAmmoCount("Flare") < sk_max_flare.GetFloat())) // If Ammo is under the max carry capacity for this Ammo type then allow collecting.
		{
			if (m_flPickUpAmount == 0)
			{
				ITEM_GiveAmmo(pPlayer, sk_pickup_flare.GetFloat(), "Flare");
			}
			else
			{
				ITEM_GiveAmmo(pPlayer, m_flPickUpAmount, "Flare");
			}

			if (m_PickupSnd != NULL_STRING)
			{
				CPASAttenuationFilter filter(this);
				EmitSound_t ep;
				ep.m_pSoundName = (char*)STRING(m_PickupSnd);
				EmitSound(filter, pPlayer->entindex(), ep);
			}
			else
			{
				CPASAttenuationFilter filter(pPlayer, "BaseCombatCharacter.AmmoPickup");
				EmitSound(filter, pPlayer->entindex(), "BaseCombatCharacter.AmmoPickup");
			}

			if (g_pGameRules->ItemShouldRespawn(this) == GR_ITEM_RESPAWN_NO) { UTIL_Remove(this); }
			if (gEvilImpulse101) { UTIL_Remove(this); }

			m_OnCollected.FireOutput(pActivator, this);
		}
		else // If Ammo is full then player can pick this up like a normal prop.
		{
			pPlayer->PickupObject(this);
			m_OnPickedUp.FireOutput(pActivator, this);
			return;
		}
	}
	DECLARE_DATADESC();
};

LINK_ENTITY_TO_CLASS(item_flare_round, CItem_FlareRound);
LINK_ENTITY_TO_CLASS(item_box_flare_rounds, CItem_FlareRound);

BEGIN_DATADESC(CItem_FlareRound)
	DEFINE_KEYFIELD(m_flPickUpAmount, FIELD_FLOAT, "itemval"),
	DEFINE_KEYFIELD(m_PickupSnd, FIELD_SOUNDNAME, "message"),

	//Output
	DEFINE_OUTPUT(m_OnCollected, "OnCollected"),
	DEFINE_OUTPUT(m_OnPickedUp, "OnPickedUp"),
END_DATADESC()

// ========================================================================
// RPG Round
// ========================================================================
class CItem_RPG_Round : public CItem
{
public:
	DECLARE_CLASS( CItem_RPG_Round, CItem );

	void Spawn( void )
	{ 
		Precache( );
		SetModel( "models/weapons/w_missile_closed.mdl");
		BaseClass::Spawn( );
	}

	void Precache( void )
	{
		PrecacheModel ("models/weapons/w_missile_closed.mdl");
		PrecacheScriptSound("BaseCombatCharacter.AmmoPickup");
		if (m_PickupSnd != NULL_STRING) { PrecacheScriptSound(STRING(m_PickupSnd)); }
	}

	void Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value)
	{
		CBasePlayer* pPlayer = static_cast<CBasePlayer*>(pActivator);

		// Custom Ammo pickup Values for Hammer and Junk
		if ((pPlayer->GetAmmoCount("RPG_Round") < sk_max_rpg.GetFloat())) // If Ammo is under the max carry capacity for this Ammo type then allow collecting.
		{
			if (m_flPickUpAmount == 0)
			{
				ITEM_GiveAmmo(pPlayer, sk_pickup_rpg.GetFloat(), "RPG_Round");
			}
			else
			{
				ITEM_GiveAmmo(pPlayer, m_flPickUpAmount, "RPG_Round");
			}

			if (m_PickupSnd != NULL_STRING)
			{
				CPASAttenuationFilter filter(this);
				EmitSound_t ep;
				ep.m_pSoundName = (char*)STRING(m_PickupSnd);
				EmitSound(filter, pPlayer->entindex(), ep);
			}
			else
			{
				CPASAttenuationFilter filter(pPlayer, "BaseCombatCharacter.AmmoPickup");
				EmitSound(filter, pPlayer->entindex(), "BaseCombatCharacter.AmmoPickup");
			}

			if (g_pGameRules->ItemShouldRespawn(this) == GR_ITEM_RESPAWN_NO) { UTIL_Remove(this); }
			if (gEvilImpulse101) { UTIL_Remove(this); }

			m_OnCollected.FireOutput(pActivator, this);
		}
		else // If Ammo is full then player can pick this up like a normal prop.
		{
			pPlayer->PickupObject(this);
			m_OnPickedUp.FireOutput(pActivator, this);
			return;
		}
	}
	DECLARE_DATADESC();
};

LINK_ENTITY_TO_CLASS( item_ml_grenade, CItem_RPG_Round );
LINK_ENTITY_TO_CLASS( item_rpg_round, CItem_RPG_Round );

BEGIN_DATADESC(CItem_RPG_Round)
	DEFINE_KEYFIELD(m_flPickUpAmount, FIELD_FLOAT, "itemval"),
	DEFINE_KEYFIELD(m_PickupSnd, FIELD_SOUNDNAME, "message"),

	//Output
	DEFINE_OUTPUT(m_OnCollected, "OnCollected"),
	DEFINE_OUTPUT(m_OnPickedUp, "OnPickedUp"),
END_DATADESC()

// ========================================================================
//	>> CItem_SMG1AltFireRound
// ========================================================================
class CItem_SMG1AltFireRound : public CItem
{
public:
	DECLARE_CLASS(CItem_SMG1AltFireRound, CItem );

	void Spawn( void )
	{ 
		Precache( );
		SetModel( "models/items/ar2_grenade.mdl");
		BaseClass::Spawn( );
	}

	void Precache( void )
	{
		PrecacheModel ("models/items/ar2_grenade.mdl");
		PrecacheScriptSound("BaseCombatCharacter.AmmoPickup");
		if (m_PickupSnd != NULL_STRING) { PrecacheScriptSound(STRING(m_PickupSnd)); }
	}

	void Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value)
	{
		CBasePlayer* pPlayer = static_cast<CBasePlayer*>(pActivator);

		// Custom Ammo pickup Values for Hammer and Junk
		if ((pPlayer->GetAmmoCount("SMG1_Grenade") < sk_max_smg1_gren.GetFloat())) // If Ammo is under the max carry capacity for this Ammo type then allow collecting.
		{
			if (m_flPickUpAmount == 0)
			{
				ITEM_GiveAmmo(pPlayer, sk_pickup_smg1_gren.GetFloat(), "SMG1_Grenade");
			}
			else
			{
				ITEM_GiveAmmo(pPlayer, m_flPickUpAmount, "SMG1_Grenade");
			}

			if (m_PickupSnd != NULL_STRING)
			{
				CPASAttenuationFilter filter(this);
				EmitSound_t ep;
				ep.m_pSoundName = (char*)STRING(m_PickupSnd);
				EmitSound(filter, pPlayer->entindex(), ep);
			}
			else
			{
				CPASAttenuationFilter filter(pPlayer, "BaseCombatCharacter.AmmoPickup");
				EmitSound(filter, pPlayer->entindex(), "BaseCombatCharacter.AmmoPickup");
			}

			if (g_pGameRules->ItemShouldRespawn(this) == GR_ITEM_RESPAWN_NO) { UTIL_Remove(this); }
			if (gEvilImpulse101) { UTIL_Remove(this); }

			m_OnCollected.FireOutput(pActivator, this);
		}
		else // If Ammo is full then player can pick this up like a normal prop.
		{
			pPlayer->PickupObject(this);
			m_OnPickedUp.FireOutput(pActivator, this);
			return;
		}
	}
	DECLARE_DATADESC();
};

LINK_ENTITY_TO_CLASS(item_ar2_grenade, CItem_SMG1AltFireRound);
LINK_ENTITY_TO_CLASS(item_ammo_smg1_grenade, CItem_SMG1AltFireRound);

BEGIN_DATADESC(CItem_SMG1AltFireRound)
	DEFINE_KEYFIELD(m_flPickUpAmount, FIELD_FLOAT, "itemval"),
	DEFINE_KEYFIELD(m_PickupSnd, FIELD_SOUNDNAME, "message"),

	//Output
	DEFINE_OUTPUT(m_OnCollected, "OnCollected"),
	DEFINE_OUTPUT(m_OnPickedUp, "OnPickedUp"),
END_DATADESC()

// ========================================================================
//	>> BoxSniperRounds
// ========================================================================
class CItem_BoxSniperRounds : public CItem
{
public:
	DECLARE_CLASS( CItem_BoxSniperRounds, CItem );

	void Spawn( void )
	{ 
		Precache( );
		SetModel( "models/items/boxsniperrounds.mdl");
		BaseClass::Spawn( );
	}

	void Precache( void )
	{
		PrecacheModel ("models/items/boxsniperrounds.mdl");
		PrecacheScriptSound("BaseCombatCharacter.AmmoPickup");
		if (m_PickupSnd != NULL_STRING) { PrecacheScriptSound(STRING(m_PickupSnd)); }
	}

	void Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value)
	{
		CBasePlayer* pPlayer = static_cast<CBasePlayer*>(pActivator);

		// Custom Ammo pickup Values for Hammer and Junk
		if ((pPlayer->GetAmmoCount("Sniper") < sk_max_sniper.GetFloat())) // If Ammo is under the max carry capacity for this Ammo type then allow collecting.
		{
			if (m_flPickUpAmount == 0)
			{
				ITEM_GiveAmmo(pPlayer, sk_pickup_sniper.GetFloat(), "Sniper");
			}
			else
			{
				ITEM_GiveAmmo(pPlayer, m_flPickUpAmount, "Sniper");
			}

			if (m_PickupSnd != NULL_STRING)
			{
				CPASAttenuationFilter filter(this);
				EmitSound_t ep;
				ep.m_pSoundName = (char*)STRING(m_PickupSnd);
				EmitSound(filter, pPlayer->entindex(), ep);
			}
			else
			{
				CPASAttenuationFilter filter(pPlayer, "BaseCombatCharacter.AmmoPickup");
				EmitSound(filter, pPlayer->entindex(), "BaseCombatCharacter.AmmoPickup");
			}

			if (g_pGameRules->ItemShouldRespawn(this) == GR_ITEM_RESPAWN_NO) { UTIL_Remove(this); }
			if (gEvilImpulse101) { UTIL_Remove(this); }

			m_OnCollected.FireOutput(pActivator, this);
		}
		else // If Ammo is full then player can pick this up like a normal prop.
		{
			pPlayer->PickupObject(this);
			m_OnPickedUp.FireOutput(pActivator, this);
			return;
		}
	}
	DECLARE_DATADESC();
};

LINK_ENTITY_TO_CLASS(item_box_sniper_rounds, CItem_BoxSniperRounds);

BEGIN_DATADESC(CItem_BoxSniperRounds)
	DEFINE_KEYFIELD(m_flPickUpAmount, FIELD_FLOAT, "itemval"),
	DEFINE_KEYFIELD(m_PickupSnd, FIELD_SOUNDNAME, "message"),

	//Output
	DEFINE_OUTPUT(m_OnCollected, "OnCollected"),
	DEFINE_OUTPUT(m_OnPickedUp, "OnPickedUp"),
END_DATADESC()

// ========================================================================
//	>> BoxBuckshot
// ========================================================================
class CItem_BoxBuckshot : public CItem
{
public:
	DECLARE_CLASS( CItem_BoxBuckshot, CItem );

	void Spawn( void )
	{ 
		Precache( );
		SetModel( "models/items/boxbuckshot.mdl");
		BaseClass::Spawn( );
	}

	void Precache( void )
	{
		PrecacheModel ("models/items/boxbuckshot.mdl");
		PrecacheScriptSound("BaseCombatCharacter.AmmoPickup");
		if (m_PickupSnd != NULL_STRING) { PrecacheScriptSound(STRING(m_PickupSnd)); }
	}

	void Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value)
	{
		CBasePlayer* pPlayer = static_cast<CBasePlayer*>(pActivator);

		// Custom Ammo pickup Values for Hammer and Junk
		if ((pPlayer->GetAmmoCount("Buckshot") < sk_max_buckshot.GetFloat())) // If Ammo is under the max carry capacity for this Ammo type then allow collecting.
		{
			if (m_flPickUpAmount == 0)
			{
				ITEM_GiveAmmo(pPlayer, sk_pickup_buckshot.GetFloat(), "Buckshot");
			}
			else
			{
				ITEM_GiveAmmo(pPlayer, m_flPickUpAmount, "Buckshot");
			}

			if (m_PickupSnd != NULL_STRING)
			{
				CPASAttenuationFilter filter(this);
				EmitSound_t ep;
				ep.m_pSoundName = (char*)STRING(m_PickupSnd);
				EmitSound(filter, pPlayer->entindex(), ep);
			}
			else
			{
				CPASAttenuationFilter filter(pPlayer, "BaseCombatCharacter.AmmoPickup");
				EmitSound(filter, pPlayer->entindex(), "BaseCombatCharacter.AmmoPickup");
			}

			if (g_pGameRules->ItemShouldRespawn(this) == GR_ITEM_RESPAWN_NO) { UTIL_Remove(this); }
			if (gEvilImpulse101) { UTIL_Remove(this); }

			m_OnCollected.FireOutput(pActivator, this);
		}
		else // If Ammo is full then player can pick this up like a normal prop.
		{
			pPlayer->PickupObject(this);
			m_OnPickedUp.FireOutput(pActivator, this);
			return;
		}
	}
	DECLARE_DATADESC();
};

LINK_ENTITY_TO_CLASS(item_box_buckshot, CItem_BoxBuckshot);

BEGIN_DATADESC(CItem_BoxBuckshot)
	DEFINE_KEYFIELD(m_flPickUpAmount, FIELD_FLOAT, "itemval"),
	DEFINE_KEYFIELD(m_PickupSnd, FIELD_SOUNDNAME, "message"),

	//Output
	DEFINE_OUTPUT(m_OnCollected, "OnCollected"),
	DEFINE_OUTPUT(m_OnPickedUp, "OnPickedUp"),
END_DATADESC()

// ========================================================================
//	>> CItem_AR2AltFireRound
// ========================================================================
class CItem_AR2AltFireRound : public CItem
{
public:
	DECLARE_CLASS( CItem_AR2AltFireRound, CItem );

	void Spawn( void )
	{ 
		Precache( );
		SetModel( "models/items/combine_rifle_ammo01.mdl");
		BaseClass::Spawn( );
	}

	void Precache(void)
	{
		PrecacheParticleSystem("combineball");
		PrecacheModel("models/items/combine_rifle_ammo01.mdl");
		PrecacheScriptSound("BaseCombatCharacter.AmmoPickup");
		if (m_PickupSnd != NULL_STRING) { PrecacheScriptSound(STRING(m_PickupSnd)); }
	}

	void Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value)
	{
		CBasePlayer* pPlayer = static_cast<CBasePlayer*>(pActivator);

		// Custom Ammo pickup Values for Hammer and Junk
		if ((pPlayer->GetAmmoCount("AR2_Grenade") < sk_max_ar2_gren.GetFloat())) // If Ammo is under the max carry capacity for this Ammo type then allow collecting.
		{
			if (m_flPickUpAmount == 0)
			{
				ITEM_GiveAmmo(pPlayer, sk_pickup_ar2_gren.GetFloat(), "AR2_Grenade");
			}
			else
			{
				ITEM_GiveAmmo(pPlayer, m_flPickUpAmount, "AR2_Grenade");
			}

			if (m_PickupSnd != NULL_STRING)
			{
				CPASAttenuationFilter filter(this);
				EmitSound_t ep;
				ep.m_pSoundName = (char*)STRING(m_PickupSnd);
				EmitSound(filter, pPlayer->entindex(), ep);
			}
			else
			{
				CPASAttenuationFilter filter(pPlayer, "BaseCombatCharacter.AmmoPickup");
				EmitSound(filter, pPlayer->entindex(), "BaseCombatCharacter.AmmoPickup");
			}

			if (g_pGameRules->ItemShouldRespawn(this) == GR_ITEM_RESPAWN_NO) { UTIL_Remove(this); }
			if (gEvilImpulse101) { UTIL_Remove(this); }

			m_OnCollected.FireOutput(pActivator, this);
		}
		else // If Ammo is full then player can pick this up like a normal prop.
		{
			pPlayer->PickupObject(this);
			m_OnPickedUp.FireOutput(pActivator, this);
			return;
		}
	}
	DECLARE_DATADESC();
};

LINK_ENTITY_TO_CLASS( item_ammo_ar2_altfire, CItem_AR2AltFireRound );

BEGIN_DATADESC(CItem_AR2AltFireRound)
	DEFINE_KEYFIELD(m_flPickUpAmount, FIELD_FLOAT, "itemval"),
	DEFINE_KEYFIELD(m_PickupSnd, FIELD_SOUNDNAME, "message"),

	//Output
	DEFINE_OUTPUT(m_OnCollected, "OnCollected"),
	DEFINE_OUTPUT(m_OnPickedUp, "OnPickedUp"),
END_DATADESC()

// ==================================================================
// Ammo crate which will supply infinite ammo of the specified type
// ==================================================================

// Ammo types
enum
{
	AMMOCRATE_SMALL_ROUNDS,
	AMMOCRATE_MEDIUM_ROUNDS,
	AMMOCRATE_LARGE_ROUNDS,
	AMMOCRATE_RPG_ROUNDS,
	AMMOCRATE_BUCKSHOT,
	AMMOCRATE_GRENADES,
	AMMOCRATE_357,
	AMMOCRATE_CROSSBOW,
	AMMOCRATE_AR2_ALTFIRE,
	AMMOCRATE_SMG_ALTFIRE,
#ifdef MAPBASE
	AMMOCRATE_SLAM,
	AMMOCRATE_EMPTY,
#endif
	NUM_AMMO_CRATE_TYPES,
};

// Ammo crate

class CItem_AmmoCrate : public CBaseAnimating
{
public:
	DECLARE_CLASS( CItem_AmmoCrate, CBaseAnimating );

	void	Spawn( void );
	void	Precache( void );
	bool	CreateVPhysics( void );

	virtual void HandleAnimEvent( animevent_t *pEvent );

	void	SetupCrate( void );
	void	OnRestore( void );

	//FIXME: May not want to have this used in a radius
	int		ObjectCaps( void ) { return (BaseClass::ObjectCaps() | (FCAP_IMPULSE_USE|FCAP_USE_IN_RADIUS)); };
	void	Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );

	void	InputKill( inputdata_t &data );
	void	CrateThink( void );
	
	virtual int OnTakeDamage( const CTakeDamageInfo &info );

protected:

	int		m_nAmmoType;
	int		m_nAmmoIndex;

	static const char *m_lpzModelNames[NUM_AMMO_CRATE_TYPES];
	static const char *m_lpzAmmoNames[NUM_AMMO_CRATE_TYPES];
	static int m_nAmmoAmounts[NUM_AMMO_CRATE_TYPES];
	static const char *m_pGiveWeapon[NUM_AMMO_CRATE_TYPES];

	float	m_flCloseTime;
	COutputEvent	m_OnUsed;
	CHandle< CBasePlayer > m_hActivator;

#ifdef MAPBASE
	COutputEvent	m_OnAmmoTaken;
#endif

	DECLARE_DATADESC();
};

LINK_ENTITY_TO_CLASS( item_ammo_crate, CItem_AmmoCrate );

BEGIN_DATADESC( CItem_AmmoCrate )
	DEFINE_KEYFIELD( m_nAmmoType,	FIELD_INTEGER, "AmmoType" ),	
	DEFINE_FIELD( m_flCloseTime, FIELD_FLOAT ),
	DEFINE_FIELD( m_hActivator, FIELD_EHANDLE ),
	DEFINE_OUTPUT( m_OnUsed, "OnUsed" ),
#ifdef MAPBASE
	DEFINE_OUTPUT( m_OnAmmoTaken, "OnAmmoTaken" ),
#endif
	DEFINE_INPUTFUNC( FIELD_VOID, "Kill", InputKill ),
	DEFINE_THINKFUNC( CrateThink ),
END_DATADESC()

//-----------------------------------------------------------------------------
// Animation events.
//-----------------------------------------------------------------------------
// Models names
const char *CItem_AmmoCrate::m_lpzModelNames[NUM_AMMO_CRATE_TYPES] =
{
	"models/items/ammocrate_pistol.mdl",	// Small rounds
	"models/items/ammocrate_smg1.mdl",		// Medium rounds
	"models/items/ammocrate_ar2.mdl",		// Large rounds
	"models/items/ammocrate_rockets.mdl",	// RPG rounds
	"models/items/ammocrate_buckshot.mdl",	// Buckshot
	"models/items/ammocrate_grenade.mdl",	// Grenades
#ifdef MAPBASE
	"models/items/ammocrate_357.mdl",		// 357
	"models/items/ammocrate_xbow.mdl",		// Crossbow
	"models/items/ammocrate_ar2alt.mdl",	// Combine Ball 
#else
	"models/items/ammocrate_smg1.mdl",		// 357
	"models/items/ammocrate_smg1.mdl",	// Crossbow

	//FIXME: This model is incorrect!
	"models/items/ammocrate_ar2.mdl",		// Combine Ball 
#endif
	"models/items/ammocrate_smg2.mdl",	    // smg grenade
#ifdef MAPBASE
	"models/items/ammocrate_slam.mdl",	    // slam
	"models/items/ammocrate_empty.mdl",	    // empty
#endif
};

// Ammo type names
const char *CItem_AmmoCrate::m_lpzAmmoNames[NUM_AMMO_CRATE_TYPES] =
{
	"Pistol",		
	"SMG1",			
	"AR2",			
	"RPG_Round",	
	"Buckshot",		
	"Grenade",
	"357",
	"XBowBolt",
	"AR2AltFire",
	"SMG1_Grenade",
#ifdef MAPBASE
	"slam",
	NULL,
#endif
};

// Ammo amount given per +use
int CItem_AmmoCrate::m_nAmmoAmounts[NUM_AMMO_CRATE_TYPES] =
{
	300,	// Pistol
	300,	// SMG1
	300,	// AR2
	3,		// RPG rounds
	120,	// Buckshot
	5,		// Grenades
	50,		// 357
	50,		// Crossbow
	3,		// AR2 alt-fire
	5,
#ifdef MAPBASE
	5,		// SLAM
	NULL,	// Empty
#endif
};

const char *CItem_AmmoCrate::m_pGiveWeapon[NUM_AMMO_CRATE_TYPES] =
{
	NULL,	// Pistol
	NULL,	// SMG1
	NULL,	// AR2
	NULL,		// RPG rounds
	NULL,	// Buckshot
	"weapon_frag",		// Grenades
	NULL,		// 357
	NULL,		// Crossbow
	NULL,		// AR2 alt-fire
	NULL,		// SMG alt-fire
#ifdef MAPBASE
	"weapon_slam",		// SLAM
	NULL,	// Empty
#endif
};

#define	AMMO_CRATE_CLOSE_DELAY	1.5f

void CItem_AmmoCrate::Spawn( void )
{
	Precache();

	BaseClass::Spawn();

	SetModel( STRING( GetModelName() ) );
	SetMoveType( MOVETYPE_NONE );
	SetSolid( SOLID_VPHYSICS );
	CreateVPhysics();

	ResetSequence( LookupSequence( "Idle" ) );
	SetBodygroup( 1, true );

	m_flCloseTime = gpGlobals->curtime;
	m_flAnimTime = gpGlobals->curtime;
	m_flPlaybackRate = 0.0;
	SetCycle( 0 );

	m_takedamage = DAMAGE_EVENTS_ONLY;
}

bool CItem_AmmoCrate::CreateVPhysics( void )
{
	return ( VPhysicsInitStatic() != NULL );
}

void CItem_AmmoCrate::Precache( void )
{
	SetupCrate();
	PrecacheModel( STRING( GetModelName() ) );
	PrecacheScriptSound( "AmmoCrate.Open" );
	PrecacheScriptSound( "AmmoCrate.Close" );
}

void CItem_AmmoCrate::SetupCrate( void )
{
#ifdef MAPBASE
	// Custom models might be desired on, say, empty crates with custom textures
	if (GetModelName() == NULL_STRING)
#endif
	SetModelName( AllocPooledString( m_lpzModelNames[m_nAmmoType] ) );
	
	m_nAmmoIndex = GetAmmoDef()->Index( m_lpzAmmoNames[m_nAmmoType] );
}

void CItem_AmmoCrate::OnRestore( void )
{
	BaseClass::OnRestore();

	// Restore our internal state
	SetupCrate();
}

void CItem_AmmoCrate::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	CBasePlayer *pPlayer = ToBasePlayer( pActivator );

	if ( pPlayer == NULL )
		return;

	m_OnUsed.FireOutput( pActivator, this );

	int iSequence = LookupSequence( "Open" );

	// See if we're not opening already
	if ( GetSequence() != iSequence )
	{
		Vector mins, maxs;
		trace_t tr;

		CollisionProp()->WorldSpaceAABB( &mins, &maxs );

		Vector vOrigin = GetAbsOrigin();
		vOrigin.z += ( maxs.z - mins.z );
		mins = (mins - GetAbsOrigin()) * 0.2f;
		maxs = (maxs - GetAbsOrigin()) * 0.2f;
		mins.z = ( GetAbsOrigin().z - vOrigin.z );  
		
		UTIL_TraceHull( vOrigin, vOrigin, mins, maxs, MASK_SOLID, this, COLLISION_GROUP_NONE, &tr );

		if ( tr.startsolid || tr.allsolid )
			 return;
			
		m_hActivator = pPlayer;

		// Animate!
		ResetSequence( iSequence );

		// Make sound
		CPASAttenuationFilter sndFilter( this, "AmmoCrate.Open" );
		EmitSound( sndFilter, entindex(), "AmmoCrate.Open" );

		// Start thinking to make it return
		SetThink( &CItem_AmmoCrate::CrateThink );
		SetNextThink( gpGlobals->curtime + 0.1f );
	}

	// Don't close again for two seconds
	m_flCloseTime = gpGlobals->curtime + AMMO_CRATE_CLOSE_DELAY;
}

//-----------------------------------------------------------------------------
// Purpose: allows the crate to open up when hit by a crowbar
//-----------------------------------------------------------------------------
int CItem_AmmoCrate::OnTakeDamage( const CTakeDamageInfo &info )
{
	// if it's the player hitting us with a crowbar, open up
	CBasePlayer *player = ToBasePlayer(info.GetAttacker());
	if (player)
	{
		CBaseCombatWeapon *weapon = player->GetActiveWeapon();

		if (weapon && !stricmp(weapon->GetName(), "weapon_crowbar"))
		{
			// play the normal use sound
			player->EmitSound( "HL2Player.Use" );
			// open the crate
			Use(info.GetAttacker(), info.GetAttacker(), USE_TOGGLE, 0.0f);
		}
	}
	// don't actually take any damage
	return 0;
}

//-----------------------------------------------------------------------------
// Purpose: Catches the monster-specific messages that occur when tagged
//			animation frames are played.
// Input  : *pEvent - 
//-----------------------------------------------------------------------------
void CItem_AmmoCrate::HandleAnimEvent( animevent_t *pEvent )
{
	if ( pEvent->event == AE_AMMOCRATE_PICKUP_AMMO )
	{
		if ( m_hActivator )
		{
			if ( m_pGiveWeapon[m_nAmmoType] && !m_hActivator->Weapon_OwnsThisType( m_pGiveWeapon[m_nAmmoType] ) )
			{
				CBaseEntity *pEntity = CreateEntityByName( m_pGiveWeapon[m_nAmmoType] );
				CBaseCombatWeapon *pWeapon = dynamic_cast<CBaseCombatWeapon*>(pEntity);
				if ( pWeapon )
				{
					pWeapon->SetAbsOrigin( m_hActivator->GetAbsOrigin() );
					pWeapon->m_iPrimaryAmmoType = 0;
					pWeapon->m_iSecondaryAmmoType = 0;
					pWeapon->Spawn();
					if ( !m_hActivator->BumpWeapon( pWeapon ) )
					{
						UTIL_Remove( pEntity );
					}
					else
					{
#ifdef MAPBASE
						m_OnAmmoTaken.FireOutput(m_hActivator, this);
#endif
						SetBodygroup( 1, false );
					}
				}
			}

#ifdef MAPBASE
			// Empty ammo crates should still fire OnAmmoTaken
			if ( m_hActivator->GiveAmmo( m_nAmmoAmounts[m_nAmmoType], m_nAmmoIndex ) != 0 || m_nAmmoType == AMMOCRATE_EMPTY )
#else
			if ( m_hActivator->GiveAmmo( m_nAmmoAmounts[m_nAmmoType], m_nAmmoIndex ) != 0 )
#endif
			{
#ifdef MAPBASE
				m_OnAmmoTaken.FireOutput(m_hActivator, this);
#endif
				SetBodygroup( 1, false );
			}
			m_hActivator = NULL;
		}
		return;
	}
	BaseClass::HandleAnimEvent( pEvent );
}

void CItem_AmmoCrate::CrateThink( void )
{
	StudioFrameAdvance();
	DispatchAnimEvents( this );

	SetNextThink( gpGlobals->curtime + 0.1f );

	// Start closing if we're not already
	if ( GetSequence() != LookupSequence( "Close" ) )
	{
		// Not ready to close?
		if ( m_flCloseTime <= gpGlobals->curtime )
		{
			m_hActivator = NULL;

			ResetSequence( LookupSequence( "Close" ) );
		}
	}
	else
	{
		// See if we're fully closed
		if ( IsSequenceFinished() )
		{
			// Stop thinking
			SetThink( NULL );
			CPASAttenuationFilter sndFilter( this, "AmmoCrate.Close" );
			EmitSound( sndFilter, entindex(), "AmmoCrate.Close" );

			// FIXME: We're resetting the sequence here
			// but setting Think to NULL will cause this to never have
			// StudioFrameAdvance called. What are the consequences of that?
			ResetSequence( LookupSequence( "Idle" ) );
			SetBodygroup( 1, true );
		}
	}
}

void CItem_AmmoCrate::InputKill( inputdata_t &data )
{
#ifdef MAPBASE
	// Why is this its own function?
	// item_dynamic_resupply and item_item_crate are in the same boat.
	// I don't understand.
	m_OnKilled.FireOutput( data.pActivator, this );
#endif
	UTIL_Remove( this );
}