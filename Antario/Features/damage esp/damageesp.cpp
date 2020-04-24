#include "damageesp.h"
#include "../../Utils/GlobalVars.h"
#include "../../SDK/ISurface.h"
#include "../../Utils/Math.h"
#include "../../SDK/IEngineTrace.h"
#include "../../SDK/IClientMode.h"
#include "../Visuals/ESP.h"
#include "../../SDK/CEntity.h"
//#include "../otheresp/otheresp.h"
#include "../../SDK/CGlobalVarsBase.h"
#include "../../SDK/IGameEvent.h"
#include "../../Menu/config.h"

#include <d3dx9.h>
#pragma comment (lib, "d3dx9.lib")


namespace DamageESP
{
	std::array<FloatingText, MAX_FLOATING_TEXTS> floatingTexts;
	int floatingTextsIdx = 0;

	void HandleGameEvent(IGameEvent* pEvent)
	{

		if (!c_config::get().KillCounter || !(g_pEngine->IsInGame() && g_pEngine->IsConnected() && Globals::LocalPlayer))
			return;

		const char *name = pEvent->GetName();

		static Vector lastImpactPos = Vector(0, 0, 0);

		if (!strcmp(name, ("player_hurt")))
		{
			float curTime = g_pGlobalVars->curtime;

			int userid = pEvent->GetInt(("userid"));
			int attackerid = pEvent->GetInt(("attacker"));
			int dmg_health = pEvent->GetInt(("dmg_health"));
			int hitgroup = pEvent->GetInt(("hitgroup"));

			auto *entity = (C_BaseEntity*)g_pEntityList->GetClientEntity(g_pEngine->GetPlayerForUserID(userid));
			auto *attacker = (C_BaseEntity*)g_pEntityList->GetClientEntity(g_pEngine->GetPlayerForUserID(attackerid));

			if (!entity || attacker != Globals::LocalPlayer)
				return;

			FloatingText txt;
			txt.startTime = curTime;
			txt.hitgroup = hitgroup;
			txt.hitPosition = lastImpactPos;
			txt.damage = dmg_health;
			txt.randomIdx = rand() % 5;
			txt.valid = true;

			floatingTexts[floatingTextsIdx++ % MAX_FLOATING_TEXTS] = txt;
		}
		else if (!strcmp(name, ("bullet_impact")))
		{
			int userid = pEvent->GetInt(("userid"));
			float x = pEvent->GetFloat(("x"));
			float y = pEvent->GetFloat(("y"));
			float z = pEvent->GetFloat(("z"));

			auto *entity = (C_BaseEntity*)g_pEntityList->GetClientEntity(g_pEngine->GetPlayerForUserID(userid));

			if (!entity || entity != Globals::LocalPlayer)
				return;

			lastImpactPos = Vector(x, y, z);
		}
	}

	
};