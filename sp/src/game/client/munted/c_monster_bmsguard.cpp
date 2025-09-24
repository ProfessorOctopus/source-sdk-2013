//========= Copyright Valve Corporation, All rights reserved. ============//
#include "cbase.h"
#include "c_ai_basenpc.h"
#include "tier0/memdbgon.h"

class C_Monster_BMSGuard : public C_AI_BaseNPC
{
public:
	DECLARE_CLASS(C_Monster_BMSGuard, C_AI_BaseNPC);
	DECLARE_CLIENTCLASS();

	C_Monster_BMSGuard();
	virtual			~C_Monster_BMSGuard();
};

IMPLEMENT_CLIENTCLASS_DT(C_Monster_BMSGuard, DT_MONSTER_BMSGUARD, CMONSTER_BMSGUARD)
END_RECV_TABLE()

C_Monster_BMSGuard::C_Monster_BMSGuard(){}
C_Monster_BMSGuard ::~C_Monster_BMSGuard(){}