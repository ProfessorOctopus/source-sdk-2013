//========= Copyright Valve Corporation, All rights reserved. ============//
#include "cbase.h"
#include "c_ai_basenpc.h"
#include "tier0/memdbgon.h"

class C_NPC_BMSGUARD : public C_AI_BaseNPC
{
public:
	DECLARE_CLASS(C_NPC_BMSGUARD, C_AI_BaseNPC);
	DECLARE_CLIENTCLASS();

	C_NPC_BMSGUARD();
	virtual			~C_NPC_BMSGUARD();
};

IMPLEMENT_CLIENTCLASS_DT(C_NPC_BMSGUARD, DT_NPC_BMSGUARD, CNPC_BMSGUARD)
END_RECV_TABLE()

C_NPC_BMSGUARD::C_NPC_BMSGUARD(){}
C_NPC_BMSGUARD ::~C_NPC_BMSGUARD(){}