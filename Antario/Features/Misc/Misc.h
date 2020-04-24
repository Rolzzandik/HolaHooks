#pragma once

#include "..\Aimbot\Autowall.h"
#include "..\Aimbot\Aimbot.h"
#include "..\..\Utils\GlobalVars.h"
#include "..\..\Utils\Math.h"
#include "..\..\SDK\ICvar.h"
#include "..\..\SDK\CPrediction.h"
#include "..\..\Menu\Menu.h"
#include <iostream>
#include <algorithm>
#include "..\..\Menu\config.h"
#include "../../SDK/IClientMode.h"
#include "../AntiAim/AntiAim.h"
#define _SOLVEY(a, b, c, d, e, f) ((c * b - d * a) / (c * f - d * e))
#define SOLVEY(...) _SOLVEY(?, ?, ?, ?, ?, ?)
#define SOLVEX(y, world, forward, right) ((world.x - right.x * y) / forward.x)
#define Square(x) ((x)*(x))
auto LoadNamedSky = reinterpret_cast<void(__fastcall*)(const char*)>(Utils::FindSignature("engine.dll", "55 8B EC 81 EC ? ? ? ? 56 57 8B F9 C7 45"));
//Find out how many ticks player is choking

float GetNetworkLatency()
{
	INetChannelInfo *nci = g_pEngine->GetNetChannelInfo();
	if (nci)
	{
		return nci->GetAvgLatency(FLOW_INCOMING);
	}
	return 0.0f;
}


int GetNumberOfTicksChoked(C_BaseEntity* pEntity)
{
	float flSimulationTime = pEntity->GetSimulationTime();
	float flSimDiff = g_pGlobalVars->curtime - flSimulationTime;
	float latency = GetNetworkLatency();
	return TIME_TO_TICKS(max(0.0f, flSimDiff - latency));
}

void GetChoked() {
	auto NetChannel = g_pEngine->GetNetChannel();

	if (!NetChannel)
		return;

}

class chat_translator : public singleton<chat_translator> {
public:
	auto client_cmd(const char* cmd, std::string& buf) -> int;
	auto dispatch_user_message(int entity_id, const std::string& msg) -> void;
	auto thread_tick() -> void; // call from another thread, for prevent game freezes.
public:
	auto translate(const std::string& text, const std::string& lang)->std::string;

private:
	std::string lang_me_{ "" };
	std::string lang_other_{ "" };

	struct msg_t {
		int entity_id;
		std::string msg;
	};
	std::deque<msg_t> msgs_;
};

/*
 __       __      ____________       _                  ___
|  |     |  |    |  _________ |     | |               _|   |_
|  |     |  |    | |        | |     | |             _|  _ _  |_
|  |_____|  |    | |        | |     | |            |  _|___|_  |
|   _____   |    | |        | |     | |            | |       | |
|  |     |  |    | |        | |     | |            | |       | |
|  |     |  |    | |________| |     | |_______     | |       | |
|__|     |__|    |____________|     |_________|    |_|       |_|

________________________________________________________________
|                                                               |
|    H O L A H O O K S                                          |
|                                                               |
|_______________________________________________________________|
*/



enum EChatTranslatorResult {
	ECTR_None,
	ECTR_SetLang,
	ECTR_Translate
};

class Misc
{
public:
	int choke;
    void OnCreateMove()
    {
        this->pCmd   = Globals::pCmd;
        this->pLocal = Globals::LocalPlayer;

		this->DoAutostrafe();
		this->DoBhop();
		this->AutoRevolver();
		if (!g_pEngine->IsVoiceRecording()) {
			this->DoFakeLag();
		}
		//this->FakeDuck();

		if (c_config::get().local_chams)
			choke = 1;
		else
			choke = std::min<int>(c_config::get().fakelag, 12);
		
		this->DoSlowWalk();
		this->Animated_Clantag();
		this->DoDisablePostProcess();
		FakeDuck();
    };

	void Choker(int ticks)
	{
		static int choked = 0;
		if (choked >= ticks)
		{
			Globals::bSendPacket = true;
			choked = 0;
		}
		else
		{
			Globals::bSendPacket = false;
			choked++;
		}
	}

	void FakeDuck()
	{
		if (!GetAsyncKeyState(c_config::get().fakeduck_bind))
			return;

		Choker(7);
		bool should_crouch = g_pEngine->GetNetChannel()->m_nChokedPackets >= (7 / 2);
		if (should_crouch)
		{
			Globals::pCmd->buttons |= IN_DUCK;
		}
		else
		{
			Globals::pCmd->buttons &= ~IN_DUCK;
		}
	}


	void MovementFix(Vector& oldang)
	{
		Vector vMovements(Globals::pCmd->forwardmove, Globals::pCmd->sidemove, 0.f);

		if (vMovements.Length2D() == 0)
			return;

		Vector vRealF, vRealR;
		Vector aRealDir = Globals::pCmd->viewangles;
		aRealDir.Clamp();

		g_Math.AngleVectors(aRealDir, &vRealF, &vRealR, nullptr);
		vRealF[2] = 0;
		vRealR[2] = 0;

		VectorNormalize(vRealF);
		VectorNormalize(vRealR);

		Vector aWishDir = oldang;
		aWishDir.Clamp();

		Vector vWishF, vWishR;
		g_Math.AngleVectors(aWishDir, &vWishF, &vWishR, nullptr);

		vWishF[2] = 0;
		vWishR[2] = 0;

		VectorNormalize(vWishF);
		VectorNormalize(vWishR);

		Vector vWishVel;
		vWishVel[0] = vWishF[0] * Globals::pCmd->forwardmove + vWishR[0] * Globals::pCmd->sidemove;
		vWishVel[1] = vWishF[1] * Globals::pCmd->forwardmove + vWishR[1] * Globals::pCmd->sidemove;
		vWishVel[2] = 0;

		float a = vRealF[0], b = vRealR[0], c = vRealF[1], d = vRealR[1];
		float v = vWishVel[0], w = vWishVel[1];

		float flDivide = (a * d - b * c);
		float x = (d * v - b * w) / flDivide;
		float y = (a * w - c * v) / flDivide;

		Globals::pCmd->forwardmove = x;
		Globals::pCmd->sidemove = y;
	}
	template<class T, class U>
	T fine(T in, U low, U high)
	{
		if (in <= low)
			return low;

		if (in >= high)
			return high;

		return in;
	}


	void LinearExtrapolations()
	{
		if (g_Menu.Config.Aimbot && c_config::get().fakelag_prediction)
		{
			auto m_local = Globals::LocalPlayer;
			if (m_local && m_local->IsAlive()) {
				for (int i = 1; i < g_pEngine->GetMaxClients(); i++)
				{
					auto m_entity = g_pEntityList->GetClientEntity(i);
					if (m_entity) {

						float simtime_delta = m_entity->GetSimulationTime() - m_entity->GetOldSimulationTime();
						int choked_ticks = fine(TIME_TO_TICKS(simtime_delta), 1, 15);
						Vector lastOrig;

						if (lastOrig.Length() != m_entity->GetOrigin().Length())
							lastOrig = m_entity->GetOrigin();

						float delta_distance = (m_entity->GetOrigin() - lastOrig).LengthSqr();
						if (delta_distance > 4096.f)
						{
							Vector velocity_per_tick = m_entity->GetVelocity() * g_pGlobalVars->intervalPerTick;
							auto new_origin = m_entity->GetOrigin() + (velocity_per_tick * choked_ticks);
							m_entity->SetAbsOrigin(new_origin);
						}
					}

				}
			}
		}
	}

	void NightMode() {
		static std::string old_Skyname = "";
		static bool OldNightmode;
		static int OldSky;
		if (!g_pEngine->IsConnected() || !g_pEngine->IsInGame() || !Globals::LocalPlayer || !Globals::LocalPlayer->IsAlive())
		{
			old_Skyname = "";
			OldNightmode = false;
			OldSky = 0;
			return;
		}

		static ConVar*r_DrawSpecificStaticProp;
		if (OldNightmode != c_config::get().nightmode)
		{

			r_DrawSpecificStaticProp = g_pCvar->FindVar("r_DrawSpecificStaticProp");
			r_DrawSpecificStaticProp->SetValue(0);

			for (MaterialHandle_t i = g_pMaterialSys->FirstMaterial(); i != g_pMaterialSys->InvalidMaterial(); i = g_pMaterialSys->NextMaterial(i))
			{
				IMaterial* pMaterial = g_pMaterialSys->GetMaterial(i);
				if (!pMaterial)
					continue;
				if (strstr(pMaterial->GetTextureGroupName(), "World") || strstr(pMaterial->GetTextureGroupName(), "StaticProp"))
				{
					if (c_config::get().nightmode) {
						LoadNamedSky("sky_csgo_night02");

						if (strstr(pMaterial->GetTextureGroupName(), "StaticProp"))
							pMaterial->ColorModulate(0.11f, 0.11f, 0.11f);
						else
							pMaterial->ColorModulate(0.05f, 0.05f, 0.05f);
					}
					else {
						LoadNamedSky("sky_cs15_daylight04_hdr");
						pMaterial->ColorModulate(1.0f, 1.0f, 1.0f);
					}
				}
			}
			OldNightmode = c_config::get().nightmode;
		}
	}

	void AsusProps() {
		static std::string old_Skyname = "";
		static bool OldNightmode;
		static int OldSky;

		if (!g_pEngine->IsConnected() || !g_pEngine->IsInGame() || !Globals::LocalPlayer || !Globals::LocalPlayer->IsAlive())
		{
			old_Skyname = "";
			OldNightmode = false;
			OldSky = 0;
			return;
		}

		if (OldNightmode != c_config::get().transparent_props)
		{
			for (MaterialHandle_t i = g_pMaterialSys->FirstMaterial(); i != g_pMaterialSys->InvalidMaterial(); i = g_pMaterialSys->NextMaterial(i))
			{
				IMaterial* pMaterial = g_pMaterialSys->GetMaterial(i);
				if (!pMaterial)
					continue;
				if (strstr(pMaterial->GetTextureGroupName(), "StaticProp textures"))
				{
					if (c_config::get().transparent_props) {
						pMaterial->AlphaModulate(0.7f);
					}
					else {
						pMaterial->AlphaModulate(1.f);
					}
						
				}
			}
			OldNightmode = c_config::get().transparent_props;
		}

	}
	void SwapManual()
	{
		if (GetKeyState(c_config::get().manual_swap_bind))
		{
			Globals::Manual_Side = true;
		}
		else {
			Globals::Manual_Side = false;
		}
	}

	//bool isThirdperson = false;
	void ThirdPerson()
	{
		if (!g_pEngine->IsInGame() || !g_pEngine->IsConnected() || !Globals::LocalPlayer)
			return;

		static bool init = false;
		static bool set_angle = false;
		auto pLocalEntity = Globals::LocalPlayer;

		static int stored_thirdperson_distance;

		if (stored_thirdperson_distance != c_config::get().thirdperson_distance) {
			std::string command; command += "cam_idealdist "; command += std::to_string(c_config::get().thirdperson_distance + 30);
			g_pEngine->ExecuteClientCmd(command.c_str());

			stored_thirdperson_distance = c_config::get().thirdperson_distance;
		}
		static Vector vecAngles;
		g_pEngine->GetViewAngles(vecAngles);

		if (GetKeyState(c_config::get().thirdperson_bind) && Globals::LocalPlayer->IsAlive())
		{
			if (init)
			{
				ConVar* sv_cheats = g_pCvar->FindVar("sv_cheats");
				*(int*)((DWORD)& sv_cheats->fnChangeCallback + 0xC) = 0; // ew
				sv_cheats->SetValue(1);
				g_pEngine->ExecuteClientCmd("thirdperson");
				//isThirdperson = true;
				g_Menu.Config.IsThirdperson = true;

				std::string command; command += "cam_idealdist "; command += std::to_string(c_config::get().thirdperson_distance + 30);
				g_pEngine->ExecuteClientCmd(command.c_str());
			}
			init = false;
		}
		else
		{
			if (!init)
			{
				ConVar* sv_cheats = g_pCvar->FindVar("sv_cheats");
				*(int*)((DWORD)& sv_cheats->fnChangeCallback + 0xC) = 0; // ew
				sv_cheats->SetValue(1);
				g_pEngine->ExecuteClientCmd("firstperson");
				//isThirdperson = false;
				g_Menu.Config.IsThirdperson = false;


			}
			init = true;
		}


	}

	void Thirdperson_FSN(ClientFrameStage_t curStage) {
		if (curStage == FRAME_RENDER_START && g_GameInput->m_fCameraInThirdPerson && Globals::LocalPlayer && Globals::LocalPlayer->IsAlive())
		{
				g_pPrediction->SetLocalViewAngles(Vector(Globals::RealAngle.x, Globals::RealAngle.y, 0));
		}


		if (curStage == FRAME_RENDER_START && g_Menu.Config.IsThirdperson)
		{
			g_pPrediction->SetLocalViewAngles(Vector(Globals::RealAngle.x, Globals::RealAngle.y, 0));
			if (c_config::get().antiaim_enabled)
			{
				if (Globals::LocalPlayer->AnimState()->m_bInHitGroundAnimation && !GetAsyncKeyState(VK_SPACE))
				{
					if (Globals::LocalPlayer->AnimState()->m_flHeadHeightOrOffsetFromHittingGroundAnimation)
					{
						g_pPrediction->SetLocalViewAngles(Vector(0, Globals::RealAngle.y, 0));
					}
				}
			}
			Globals::LocalPlayer->UpdateClientAnimation();
			g_pPrediction->SetLocalViewAngles(Vector(0, Globals::FakeAngle.y, 0));
		}
	}
	void NormalWalk()
	{

		Globals::pCmd->buttons &= ~IN_MOVERIGHT;
		Globals::pCmd->buttons &= ~IN_MOVELEFT;
		Globals::pCmd->buttons &= ~IN_FORWARD;
		Globals::pCmd->buttons &= ~IN_BACK;

		if (Globals::pCmd->forwardmove > 0.f)
			Globals::pCmd->buttons |= IN_FORWARD;
		else if (Globals::pCmd->forwardmove < 0.f)
			Globals::pCmd->buttons |= IN_BACK;
		if (Globals::pCmd->sidemove > 0.f)
		{
			Globals::pCmd->buttons |= IN_MOVERIGHT;
		}
		else if (Globals::pCmd->sidemove < 0.f)
		{
			Globals::pCmd->buttons |= IN_MOVELEFT;
		}

	}


	void NoRecoil(CUserCmd* cmd)
	{
		if (!c_config::get().aimbot_norecoil)
			return;

		auto local_player = g_pEntityList->GetClientEntity(g_pEngine->GetLocalPlayer());
		if (!local_player)
			return;

		auto weapon = local_player->GetActiveWeapon();
		if (weapon)
			weapon->GetAccuracyPenalty();

		cmd->viewangles -= local_player->GetAimPunchAngle() * 2;
	}

	Color getHealthColor(C_BaseEntity* Entity)
	{
		int red, green, blue, alpha;
		int health = Entity->GetHealth();
		red = 255 - (health * 2.55);
		green = health * 2.55;
		blue = 0;
		return Color(red, green, blue, 255);
	}

	void SetClan(const char* tag, const char* name) {
		static auto pSetClanTag = reinterpret_cast<void(__fastcall*)(const char*, const char*)>(((DWORD)Utils::FindPattern("engine.dll", (PBYTE)"\x53\x56\x57\x8B\xDA\x8B\xF9\xFF\x15\x00\x00\x00\x00\x6A\x24\x8B\xC8\x8B\x30", "xxxxxxxxx????xxxxxx")));
		pSetClanTag(tag, name);
	}

	std::string clantag = "";
	std::string old = "";

	void Animated_Clantag() {

		if (!c_config::get().misc_clantag)
			return;

		if (clantag != old) {
			char const* c = clantag.data();
			SetClan(c, c);
			old = clantag;
		}

		switch (int(g_pGlobalVars->curtime) % 15)
		{
		case 0: clantag = " [H] "; break;
		case 1: clantag = " [HO] "; break;
		case 2: clantag = " [HOL] "; break;
		case 3: clantag = " [HOLA] "; break;
		case 4: clantag = " [HOLAH] "; break;
		case 5: clantag = " [HOLAHO] "; break;
		case 6: clantag = " [HOLAHOO] "; break;
		case 7: clantag = " [HOLAHOOK] "; break;
		case 8: clantag = " [HOLAHOOKS] "; break;
		case 9: clantag = " [HOLAHOOKS] "; break;
		case 10:clantag = " [HOLAHOOK] "; break;
		case 11:clantag = " [HOLAHOO] "; break;
		case 12:clantag = " [HOLAHO] "; break;
		case 13:clantag = " [HOLAH] "; break;
		case 14:clantag = " [HOLA] "; break;
		case 15:clantag = " [HOL] "; break;
		case 16:clantag = " [HO] "; break;
		case 17:clantag = " [H] "; break;
		case 18:clantag = " [] "; break;

		}

		/*switch (int(g_pGlobalVars->curtime * 2.4) % 27)
		{
		case 0: clantag = " "; break;
		case 1: clantag = " ga"; break;
		case 2: clantag = " gam"; break;
		case 3: clantag = " game"; break;
		case 4: clantag = " games"; break;
		case 5: clantag = " gamese"; break;
		case 6: clantag = " gamesen"; break;
		case 7: clantag = " gamesens"; break;
		case 8: clantag = " gamesense"; break;
		case 9:clantag =  " gamesense "; break;
		case 10:clantag = " gamesense "; break;
		case 11:clantag = " gamesense "; break;
		case 12:clantag = " gamesense "; break;
		case 13:clantag = " gamesense "; break;
		case 14:clantag = " gamesense "; break;
		case 15:clantag = " gamesense "; break;
		case 16:clantag = " gamesense "; break;
		case 17:clantag = "gamesense "; break;
		case 18:clantag = "amesense "; break;
		case 19:clantag = "mesense "; break;
		case 20:clantag = "esense "; break;
		case 21:clantag = "sense "; break;
		case 22:clantag = "sens "; break;
		case 23:clantag ="sen "; break;
		case 24:clantag = "se "; break;
		case 25:clantag ="s "; break;

	}*/
#if _DEBUG
		switch (int(g_pGlobalVars->curtime) % 15)
		{
		case 0: clantag = "Debugging..."; break;
		
		}
#endif

#if _BETA
		switch (int(g_pGlobalVars->curtime) % 15)
		{
		case 0: clantag = "HOLA"; break;
		case 1: clantag = "HOOKS"; break;
		case 2: clantag = "BETA"; break;
		}
#endif
	}

	void AutoRevolver()
	{
		auto me = Globals::LocalPlayer;
		auto cmd = Globals::pCmd;
		auto weapon = me->GetActiveWeapon();

		if (!c_config::get().autorevolver)
			return;

		if (!me || !me->IsAlive() || !weapon)
			return;

		if (weapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_REVOLVER)
		{
			static int delay = 0; /// pasted delay meme from uc so we'll stop shooting on high ping
			delay++;

			if (delay <= 15)
				Globals::pCmd->buttons |= IN_ATTACK;
			else
				delay = 0;
		}
	}

	void slow_walk(CUserCmd *cmd)
	{
		if (!GetAsyncKeyState(VK_SHIFT))
			return;

		if (!Globals::LocalPlayer)
			return;

		auto weapon_handle = Globals::LocalPlayer->GetActiveWeapon();

		if (!weapon_handle)
			return;

		Vector velocity = Globals::LocalPlayer->GetVelocity();
		Vector direction = velocity.Angle();
		float speed = velocity.Length();

		direction.y = cmd->viewangles.y - direction.y;

		Vector negated_direction = direction * -speed;
		if (velocity.Length() >= (weapon_handle->GetCSWpnData()->max_speed * .34f))
		{
			cmd->forwardmove = negated_direction.x;
			cmd->sidemove = negated_direction.y;
		}
	}

	void MinWalk(CUserCmd* get_cmd, float get_speed) const
	{
		if (get_speed <= 0.f)
			return;

		float min_speed = (float)(FastSqrt(Square(get_cmd->forwardmove) + Square(get_cmd->sidemove) + Square(get_cmd->upmove)));
		if (min_speed <= 0.f)
			return;

		if (get_cmd->buttons & IN_DUCK)
			get_speed *= 2.94117647f;

		if (min_speed <= get_speed)
			return;

		float kys = get_speed / min_speed;

		get_cmd->forwardmove *= kys;
		get_cmd->sidemove *= kys;
		get_cmd->upmove *= kys;
	}
	void DoSlowWalk() const
	{
		if (!c_config::get().slowwalk)
			return;

		if (c_config::get().slowwalk_bind > 0 && !GetAsyncKeyState(c_config::get().slowwalk_bind) || c_config::get().slowwalk_bind <= 0)
		{
			return;
		}

		MinWalk(pCmd, c_config::get().slowwalk_speed);
	}

	bool DisablePostProcess2 = true;

	void DoDisablePostProcess()
	{
		ConVar* post_process = g_pCvar->FindVar("mat_postprocess_enable");
		*(int*)((DWORD)& post_process->fnChangeCallback) = 1;
		if (!DisablePostProcess2)
			post_process->SetValue(1);
		else
			post_process->SetValue(0);
	}


private:
    CUserCmd*     pCmd;
    C_BaseEntity* pLocal;

    void DoBhop() const
    {
        if (!c_config::get().misc_bhop)
            return;

        static bool bLastJumped = false;
        static bool bShouldFake = false;

        if (!bLastJumped && bShouldFake)
        {
            bShouldFake = false;
            pCmd->buttons |= IN_JUMP;
        }
        else if (pCmd->buttons & IN_JUMP)
        {
            if (pLocal->GetFlags() & FL_ONGROUND)
                bShouldFake = bLastJumped = true;
            else 
            {
                pCmd->buttons &= ~IN_JUMP;
                bLastJumped = false;
            }
        }
        else
            bShouldFake = bLastJumped = false;
    }

	void DoAutostrafe() const
	{
		if (!Globals::LocalPlayer || !g_pEngine->IsConnected() || !g_pEngine->IsInGame() || !c_config::get().misc_autostrafe)
			return;

		if (!Globals::LocalPlayer->IsAlive())
			return;

		if (!(Globals::LocalPlayer->GetFlags() & FL_ONGROUND) && GetAsyncKeyState(VK_SPACE))
		{
			pCmd->forwardmove = (10000.f / Globals::LocalPlayer->GetVelocity().Length2D() > 450.f) ? 450.f : 10000.f / Globals::LocalPlayer->GetVelocity().Length2D();
			pCmd->sidemove = (pCmd->mousedx != 0) ? (pCmd->mousedx < 0.0f) ? -450.f : 450.f : (pCmd->command_number % 2) == 0 ? -450.f : 450.f;	
		}
	}

	const wchar_t* to_wchar(const char* string) {
		va_list va_alist;
		char buf[1024];
		va_start(va_alist, string);
		_vsnprintf_s(buf, sizeof(buf), string, va_alist);
		va_end(va_alist);
		wchar_t wbuf[1024];
		MultiByteToWideChar(CP_UTF8, 0, buf, 256, wbuf, 256);

		return wbuf;
	}



	void DoFakeLag() const
	{
		if (!g_pEngine->IsConnected() || !g_pEngine->IsInGame() || c_config::get().fakelag == 0 || c_config::get().legit_aimbot_backtrack)
			return;
		//voice
		if (g_pEngine->IsVoiceRecording())
			return;

		if (!Globals::LocalPlayer->IsAlive())
			return;

		auto NetChannel = g_pEngine->GetNetChannel();

		if (!NetChannel)
			return;

		//fakelag fixed
		{
			static float maxSpeed = 320.f;
			static float Acceleration = 5.5f;
			float maxAccelspeed = Acceleration * maxSpeed * g_pGlobalVars->intervalPerTick;

			float TicksToStop = Globals::LocalPlayer->GetVelocity().Length() / maxAccelspeed;

			bool canHit = false;
			static bool init = false;
			static bool stop = true;
			static bool stop2 = true;
			bool skip = false;
			if (!(Globals::pCmd->buttons & IN_ATTACK))
				if (c_config::get().fakelagwhileshooting)
				{
					for (int i = 1; i < g_pEngine->GetMaxClients(); ++i)
					{
						C_BaseEntity* pPlayerEntity = g_pEntityList->GetClientEntity(i);

						if (!pPlayerEntity
							|| !pPlayerEntity->IsAlive()
							|| pPlayerEntity->IsDormant()
							|| pPlayerEntity == Globals::LocalPlayer
							|| pPlayerEntity->GetTeam() == Globals::LocalPlayer->GetTeam())
							continue;

						Vector EnemyHead = { pPlayerEntity->GetOrigin().x, pPlayerEntity->GetOrigin().y, (pPlayerEntity->GetHitboxPosition(0, g_Aimbot.Matrix[pPlayerEntity->EntIndex()]).z + 10.f) };

						Vector Head = { Globals::LocalPlayer->GetOrigin().x, Globals::LocalPlayer->GetOrigin().y, (Globals::LocalPlayer->GetHitboxPosition(0, g_Aimbot.Matrix[pPlayerEntity->EntIndex()]).z + 10.f) };
						Vector HeadExtr = (Head + (Globals::LocalPlayer->GetVelocity() * 0.203125f));
						Vector OriginExtr = ((Globals::LocalPlayer->GetOrigin() + (Globals::LocalPlayer->GetVelocity() * 0.21875f)) + Vector(0, 0, 8));

						float dmg;

						if (abs(Globals::LocalPlayer->GetVelocity().Length2D()) > 50.f && (g_Autowall.CanHitFloatingPoint(HeadExtr, EnemyHead) || g_Autowall.CanHitFloatingPoint(OriginExtr, EnemyHead)))
						{
							canHit = true;
						}
					}

					if (canHit)
					{
						if (stop2)
						{
							init = true;
							stop2 = false;
						}
					}
					else
						stop2 = true;

					if (init)
					{
						if (!stop)
						{
							Globals::bSendPacket = true;
							Globals::LagPeek = true;
							stop = true;
							skip = true;
						}
					}
					static int ticks = 0;
					if (!skip)
					{
						if (Globals::LagPeek)
						{
							Globals::bSendPacket = ticks < 13 - c_config::get().fakelag;

							if (NetChannel->m_nChokedPackets < 13)
								Globals::bSendPacket = false;
							else
								Globals::LagPeek = false;
						}
						else
						{
							Globals::bSendPacket = (NetChannel->m_nChokedPackets >= c_config::get().fakelag);
							stop = false;
							init = false;


							{
								if (pLocal && pLocal->IsAlive() && !(Globals::pCmd->buttons & IN_ATTACK))
								{
									if (ticks >= 15)
									{
										Globals::bSendPacket = true;
										ticks = 0;
									}
									else
									{
										Globals::bSendPacket = ticks < 15 - c_config::get().fakelag;
									}
									ticks++;
								}
							}

						}
					}
				}
				else
				{

					init = false;
					stop = true;
					stop2 = true;
					Globals::LagPeek = false;

					Globals::bSendPacket = (NetChannel->m_nChokedPackets >= c_config::get().fakelag);

					if (GetAsyncKeyState(VK_SHIFT))
						Globals::bSendPacket = (NetChannel->m_nChokedPackets >= 13);





				}
		}
	}
};

extern Misc g_Misc;

