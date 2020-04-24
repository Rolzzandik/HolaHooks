#include "Resolver.h"
#include "..\Aimbot\Aimbot.h"
#include "..\Aimbot\Autowall.h"
#include "..\Aimbot\LagComp.h"
#include "..\..\Utils\Utils.h"
#include "..\..\SDK\IVEngineClient.h"
#include "..\..\SDK\Hitboxes.h"
#include "..\..\SDK\PlayerInfo.h"
#include "..\..\Utils\Math.h"
#include "..\..\Menu\Menu.h"
#include "..\..\Menu\config.h"
Resolver g_Resolver;


void Resolver::AnimationFix(C_BaseEntity* pEnt)
{
	const auto player_animation_state = pEnt->AnimState();


	if (!player_animation_state)
		return;

	if (Globals::MissedShots[pEnt->EntIndex()] > 2) {
		switch (Globals::MissedShots[pEnt->EntIndex()] % 4) {
		case 0: player_animation_state->m_flGoalFeetYaw = player_animation_state->m_flGoalFeetYaw + 60; break;
		case 1: player_animation_state->m_flGoalFeetYaw = player_animation_state->m_flGoalFeetYaw - 60; break;
		case 2: player_animation_state->m_flGoalFeetYaw = player_animation_state->m_flGoalFeetYaw - 60; break;
		case 3: player_animation_state->m_flGoalFeetYaw = player_animation_state->m_flGoalFeetYaw + 60; break;
		}
		float m_flLastClientSideAnimationUpdateTimeDelta = fabs(player_animation_state->m_iLastClientSideAnimationUpdateFramecount - player_animation_state->m_flLastClientSideAnimationUpdateTime);


		pEnt->UpdateClientAnimation();
		auto old_curtime = g_pGlobalVars->curtime;
		auto old_frametime = g_pGlobalVars->frametime;
		auto curtime = g_pGlobalVars->curtime;
		auto frametime = g_pGlobalVars->frametime;
		auto frametimne = (g_pGlobalVars->frametime);
		auto absoluteframtime = (g_pGlobalVars->absoluteframetime);
		auto interp_amount = (g_pGlobalVars->interpolationAmount);
		auto framecount = g_pGlobalVars->framecount;
		auto tickcount = g_pGlobalVars->tickcount;
		g_pGlobalVars->realtime = pEnt->GetSimulationTime();
		g_pGlobalVars->curtime = pEnt->GetSimulationTime();
		g_pGlobalVars->frametime = g_pGlobalVars->intervalPerTick;
		g_pGlobalVars->absoluteframetime = g_pGlobalVars->intervalPerTick;
		g_pGlobalVars->framecount = pEnt->GetSimulationTime();
		g_pGlobalVars->tickcount = pEnt->GetSimulationTime();
		g_pGlobalVars->interpolationAmount = 0.0f;
		g_pGlobalVars->curtime = pEnt->GetSimulationTime();
		g_pGlobalVars->frametime = g_pGlobalVars->intervalPerTick;

		player_animation_state->m_iLastClientSideAnimationUpdateFramecount >= pEnt->GetSimulationTime();
		player_animation_state->m_iLastClientSideAnimationUpdateFramecount = pEnt->GetSimulationTime() - 1;

		pEnt->UpdateClientAnimation();
		g_pGlobalVars->curtime = old_curtime;
		g_pGlobalVars->frametime = old_frametime;
		(g_pGlobalVars->realtime) = g_pGlobalVars->realtime;
		(g_pGlobalVars->curtime) = curtime;
		(g_pGlobalVars->frametime) = frametimne;
		(g_pGlobalVars->absoluteframetime) = absoluteframtime;
		g_pGlobalVars->framecount = framecount;
		g_pGlobalVars->tickcount = tickcount;
	}
}

inline float NormalizeYaw(float yaw)
{
	if (yaw > 180)
		yaw -= (round(yaw / 360) * 360.f);
	else if (yaw < -180)
		yaw += (round(yaw / 360) * -360.f);
	return yaw;
}
float GetLBYRotatedYaw(float lby, float yaw)
{
	float delta = NormalizeYaw(yaw - lby);
	if (fabs(delta) < 25.f)
		return lby;
	if (delta > 0.f)
		return yaw + 25.f;
	return yaw;
}
inline float NormalizePitch(float pitch)
{
	while (pitch > 90.f)
		pitch -= 180.f;
	while (pitch < -90.f)
		pitch += 180.f;
	return pitch;
}
float __fastcall ang_dif(float a1, float a2)
{
	float val = fmodf(a1 - a2, 360.0);
	while (val < -180.0f) val += 360.0f;
	while (val > 180.0f) val -= 360.0f;
	return val;
}
Vector CalcAngle69(Vector dst, Vector src)
{
	Vector angles;
	double delta[3] = { (src.x - dst.x), (src.y - dst.y), (src.z - dst.z) };
	double hyp = sqrt(delta[0] * delta[0] + delta[1] * delta[1]);
	angles.x = (float)(atan(delta[2] / hyp) * 180.0 / 3.14159265);
	angles.y = (float)(atanf(delta[1] / delta[0]) * 57.295779513082f);
	angles.z = 0.0f;

	if (delta[0] >= 0.0)
	{
		angles.y += 180.0f;
	}
	return angles;
}
template<class T, class U>
inline T clamp(T in, U low, U high)
{
	if (in <= low)
		return low;
	else if (in >= high)
		return high;
	else
		return in;
}


void AngleVectors(const Vector& angles, Vector* forward)
{
	Assert(s_bMathlibInitialized);
	Assert(forward);
	float	sp, sy, cp, cy;
	sy = sin(DEG2RAD(angles[1]));
	cy = cos(DEG2RAD(angles[1]));
	sp = sin(DEG2RAD(angles[0]));
	cp = cos(DEG2RAD(angles[0]));
	forward->x = cp * cy;
	forward->y = cp * sy;
	forward->z = -sp;
}


void NormalizeNumX(Vector& vIn, Vector& vOut)
{
	float flLen = vIn.Length();
	if (flLen == 0) {
		vOut.Init(0, 0, 1);
		return;
	}
	flLen = 1 / flLen;
	vOut.Init(vIn.x * flLen, vIn.y * flLen, vIn.z * flLen);
}
float flAngleMod(float flAngle)
{
	return((360.0f / 65536.0f) * ((int32_t)(flAngle * (65536.0f / 360.0f)) & 65535));
}
float ApproachAngle(float target, float value, float speed)
{
	target = flAngleMod(target);
	value = flAngleMod(value);
	float delta = target - value;
	if (speed < 0)
		speed = -speed;
	if (delta < -180)
		delta += 360;
	else if (delta > 180)
		delta -= 360;
	if (delta > speed)
		value += speed;
	else if (delta < -speed)
		value -= speed;
	else
		value = target;
	return value;
}

void update_state(C_AnimState* state, Vector angles) {
	using Fn = void(__vectorcall*)(void*, void*, float, float, float, void*);
	static auto fn = reinterpret_cast<Fn>(Utils::FindSignature("client_panorama.dll", "55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 F3 0F 11 54 24"));
	fn(state, nullptr, 0.0f, angles[1], angles[0], nullptr);
}

/*void HandleBackUpResolve(C_BaseEntity * pEnt)
{

	if (!c_config::get().aimbot_mode)
		return;
	if (pEnt->GetTeam() == Globals::LocalPlayer->GetTeam())
		return;
	const auto player_animation_state = pEnt->AnimState();
	if (!player_animation_state)
		return;
	if (Globals::MissedShots[pEnt->EntIndex()] > 2) {
		switch (Globals::MissedShots[pEnt->EntIndex()] % 4) {
		case 0: player_animation_state->m_flGoalFeetYaw = player_animation_state->m_flGoalFeetYaw + 45; break;
		case 1: player_animation_state->m_flGoalFeetYaw = player_animation_state->m_flGoalFeetYaw - 45; break;
		case 2: player_animation_state->m_flGoalFeetYaw = player_animation_state->m_flGoalFeetYaw - 30; break;
		case 3: player_animation_state->m_flGoalFeetYaw = player_animation_state->m_flGoalFeetYaw + 30; break;
		}
	}
	else {

		float m_flLastClientSideAnimationUpdateTimeDelta = fabs(player_animation_state->m_iLastClientSideAnimationUpdateFramecount - player_animation_state->m_flLastClientSideAnimationUpdateTime);
		auto v48 = 0.f;
		if (player_animation_state->m_flFeetSpeedForwardsOrSideWays >= 0.0f)
		{
			v48 = fminf(player_animation_state->m_flFeetSpeedForwardsOrSideWays, 1.0f);
		}
		else
		{
			v48 = 0.0f;
		}

		float v49 = ((player_animation_state->m_flStopToFullRunningFraction * -0.30000001) - 0.19999999) * v48;
		float flYawModifier = v49 + 1.0;
		if (player_animation_state->m_fDuckAmount > 0.0)
		{
			float v53 = 0.0f;

			if (player_animation_state->m_flFeetSpeedUnknownForwardOrSideways >= 0.0)
			{
				v53 = fminf(player_animation_state->m_flFeetSpeedUnknownForwardOrSideways, 1.0);
			}
			else
			{
				v53 = 0.0f;
			}
		}

		float flMaxYawModifier = player_animation_state->pad10[516] * flYawModifier;
		float flMinYawModifier = player_animation_state->pad10[512] * flYawModifier;
		float newFeetYaw = 0.f;
		auto eyeYaw = player_animation_state->m_flEyeYaw;
		auto lbyYaw = player_animation_state->m_flGoalFeetYaw;
		float eye_feet_delta = fabs(eyeYaw - lbyYaw);
		if (eye_feet_delta <= flMaxYawModifier)
		{
			if (flMinYawModifier > eye_feet_delta)
			{
				newFeetYaw = fabs(flMinYawModifier) + eyeYaw;
			}
		}
		else
		{
			newFeetYaw = eyeYaw - fabs(flMaxYawModifier);
		}

		float v136 = fmod(newFeetYaw, 180.0);
		if (v136 > 180.0)
		{
			v136 = v136 - 360.0;
		}
		if (v136 < 180.0)
		{
			v136 = v136 + 360.0;
		}
		player_animation_state->m_flGoalFeetYaw = v136;
	}
}*/

bool is_slow_walking(C_BaseEntity* entity) {
	float large = 0;
	float velocity_2D[64], old_velocity_2D[64];
	if (entity->GetVelocity().Length2D() != velocity_2D[entity->EntIndex()] && entity->GetVelocity().Length2D() != NULL) {
		old_velocity_2D[entity->EntIndex()] = velocity_2D[entity->EntIndex()];
		velocity_2D[entity->EntIndex()] = entity->GetVelocity().Length2D();
	}
	if (large == 0)return false;
	Vector velocity = entity->GetVelocity();
	Vector direction = entity->GetEyeAngles();

	float speed = velocity.Length();
	direction.y = entity->GetEyeAngles().y - direction.y;
	//method 1
	if (velocity_2D[entity->EntIndex()] > 1) {
		int tick_counter[64];
		if (velocity_2D[entity->EntIndex()] == old_velocity_2D[entity->EntIndex()])
			tick_counter[entity->EntIndex()] += 1;
		else
			tick_counter[entity->EntIndex()] = 0;

		while (tick_counter[entity->EntIndex()] > (1 / g_pGlobalVars->intervalPerTick)* fabsf(0.1f))
			return true;
	}


	return false;
}

float max_desync_angle(C_BaseEntity* entity) {

	auto animstate = uintptr_t(entity->AnimState());

	float duckammount = *(float*)(animstate + 0xA4);
	float speedfraction = max(0, min(*reinterpret_cast<float*>(animstate + 0xF8), 1));

	float speedfactor = max(0, min(1, *reinterpret_cast<float*> (animstate + 0xFC)));

	float unk1 = ((*reinterpret_cast<float*> (animstate + 0x11C) * -0.30000001) - 0.19999999)* speedfraction;
	float unk2 = unk1 + 1.f;
	float unk3;

	if (duckammount > 0)
		unk2 += ((duckammount * speedfactor) * (0.5f - unk2));

	unk3 = *(float*)(animstate + 0x334) * unk2;

	return unk3;
}


void HandleHits(C_BaseEntity* pEnt)
{
	auto NetChannel = g_pEngine->GetNetChannelInfo();
	if (!NetChannel)
		return;
	static float predTime[65];
	static bool init[65];
	if (Globals::Shot[pEnt->EntIndex()])
	{
		if (init[pEnt->EntIndex()])
		{
			g_Resolver.pitchHit[pEnt->EntIndex()] = pEnt->GetEyeAngles().x;
			predTime[pEnt->EntIndex()] = g_pGlobalVars->curtime + NetChannel->GetAvgLatency(FLOW_INCOMING) + NetChannel->GetAvgLatency(FLOW_OUTGOING) + TICKS_TO_TIME(1) + TICKS_TO_TIME(g_pEngine->GetNetChannel()->m_nChokedPackets);
			init[pEnt->EntIndex()] = false;
		}

		if (g_pGlobalVars->curtime > predTime[pEnt->EntIndex()] && !Globals::Hit[pEnt->EntIndex()])
		{
			Globals::MissedShots[pEnt->EntIndex()] += 1;
			Globals::Shot[pEnt->EntIndex()] = false;
		}
		else if (g_pGlobalVars->curtime <= predTime[pEnt->EntIndex()] && Globals::Hit[pEnt->EntIndex()])
			Globals::Shot[pEnt->EntIndex()] = false;
	}
	else
		init[pEnt->EntIndex()] = true;

	Globals::Hit[pEnt->EntIndex()] = false;
}
float stored_yaw;

bool CheckDesync(C_BaseEntity* pEnt)
{
	float simtime = pEnt->GetSimulationTime();
	float oldsimtime = pEnt->GetOldSimulationTime();
	bool check = g_pGlobalVars->curtime >= oldsimtime - simtime;
	float old_yaw = pEnt->AnimState()->m_flEyeYaw;
	float new_yaw;
	float difference;
	int check_count = 0;
	int jitter_check = 0;
	int bad_check = 0;
	int good_check = 0;
	if (check)
	{
		new_yaw = pEnt->AnimState()->m_flEyeYaw;
		difference = old_yaw - new_yaw;
		if (difference == -pEnt->DesyncValue() || difference == pEnt->DesyncValue())
		{
			check_count++;
		}
	}
	return check_count >= 3;
}
bool CheckLBY(C_BaseEntity* pEnt)
{

	float simtime = pEnt->GetSimulationTime();
	float oldsimtime = pEnt->GetOldSimulationTime();
	bool check = g_pGlobalVars->curtime >= oldsimtime - simtime;
	stored_yaw = pEnt->AnimState()->m_flGoalFeetYaw;
	float new_yaw;
	int check_count = 0;
	if (check)
	{
		if (stored_yaw != pEnt->GetLowerBodyYaw())
		{
			check_count++;
		}
	}
	return check_count >= 2;
}

int GetDesyncType(C_BaseEntity* pEnt)
{
	float simtime = pEnt->GetSimulationTime();
	float oldsimtime = pEnt->GetOldSimulationTime();
	int tickcount = Globals::pCmd->tick_count;
	Globals::pCmd->tick_count = (oldsimtime - simtime) + g_LagComp.LerpTime();
	float old_yaw = pEnt->AnimState()->m_flEyeYaw;
	Globals::pCmd->tick_count = tickcount;
	float new_yaw = pEnt->AnimState()->m_flEyeYaw;
	int desync = -1;
	int jitter_check = 0;
	float jitter_first;
	float jitter_second;
	float jitter_third;
	float jitter_fourty;
	float difference = old_yaw - new_yaw;

	if (difference == 58)
	{
		return 5;
	}
	else if (difference == -58)
	{
		return 7;
	}
	else if (difference == 35)
	{
		return 4;
	}
	else if (difference == 29)
	{
		return 3;
	}
	else if (difference == (90 - (1 + rand() % 20)))
	{
		return 2;
	}
	else if (difference == (70 - (1 + rand() % 10)))
	{
		return 1;
	}
	else if (difference == pEnt->DesyncValue())
	{
		return 9;
	}
	else if (difference == -pEnt->DesyncValue())
	{
		return 10;
	}
	return 0;
}
void Resolver::OnCreateMove()
{
	if (!c_config::get().aimbot_resolver == 1)
		return;

	if (!Globals::LocalPlayer->IsAlive())
		return;

	if (!Globals::LocalPlayer->GetActiveWeapon() || Globals::LocalPlayer->IsKnifeorNade())
		return;


	for (int i = 1; i < g_pEngine->GetMaxClients(); ++i)
	{
		C_BaseEntity* pPlayerEntity = g_pEntityList->GetClientEntity(i);

		if (!pPlayerEntity
			|| !pPlayerEntity->IsAlive()
			|| pPlayerEntity->IsDormant()
			|| pPlayerEntity == Globals::LocalPlayer
			|| pPlayerEntity->GetTeam() == Globals::LocalPlayer->GetTeam())
		{
			UseFreestandAngle[i] = false;
			continue;
		}
		bool lby_breaker = CheckLBY(pPlayerEntity);
		float current_yaw = pPlayerEntity->AnimState()->m_flGoalFeetYaw;
		float advance_check;
		if (lby_breaker && (current_yaw != stored_yaw))
		{
			pPlayerEntity->SetLowerBodyYaw(stored_yaw);
			Globals::pCmd->tick_count = g_LagComp.LerpTime() + pPlayerEntity->GetSimulationTime();
		}
		if (GetDesyncType(pPlayerEntity) == 10)
		{
			pPlayerEntity->AnimState()->m_flEyeYaw += pPlayerEntity->DesyncValue();
		}
		else if (GetDesyncType(pPlayerEntity) == 9)
		{
			pPlayerEntity->AnimState()->m_flEyeYaw -= pPlayerEntity->DesyncValue();
		}
		else if (GetDesyncType(pPlayerEntity) == 7)
		{
			pPlayerEntity->AnimState()->m_flEyeYaw += 58;
		}
		else if (GetDesyncType(pPlayerEntity) == 5)
		{
			pPlayerEntity->AnimState()->m_flEyeYaw -= 58;
		}
		else if (GetDesyncType(pPlayerEntity) == 4)
		{
			pPlayerEntity->AnimState()->m_flEyeYaw -= 35;
		}
		else if (GetDesyncType(pPlayerEntity) == 3)
		{
			pPlayerEntity->AnimState()->m_flEyeYaw -= 29;
		}
		else if (GetDesyncType(pPlayerEntity) == 2)
		{
			pPlayerEntity->AnimState()->m_flEyeYaw -= (90 - (1 + rand() % 20));
		}
		else if (GetDesyncType(pPlayerEntity) == 1)
		{
			pPlayerEntity->AnimState()->m_flEyeYaw -= (70 - (1 + rand() % 10));
		}
		pPlayerEntity->SetAbsAngles(Vector(0, pPlayerEntity->AnimState()->m_flEyeYaw, 0));

		if (abs(pPlayerEntity->GetVelocity().Length2D()) > 29.f)
			UseFreestandAngle[pPlayerEntity->EntIndex()] = false;

		if (abs(pPlayerEntity->GetVelocity().Length2D()) <= 29.f && !UseFreestandAngle[pPlayerEntity->EntIndex()])
		{
			static bool Autowalled = false;
			static bool HitSide1 = false;
			static bool HitSide2 = false;

			float angToLocal = g_Math.CalcAngle(Globals::LocalPlayer->GetOrigin(), pPlayerEntity->GetOrigin()).y;
			Vector ViewPoint = Globals::LocalPlayer->GetOrigin() + Vector(0, 0, 90);

			Vector2D Side1 = { (45 * sin(g_Math.GRD_TO_BOG(angToLocal))),(45 * cos(g_Math.GRD_TO_BOG(angToLocal))) };
			Vector2D Side2 = { (45 * sin(g_Math.GRD_TO_BOG(angToLocal + 180))) ,(45 * cos(g_Math.GRD_TO_BOG(angToLocal + 180))) };

			Vector2D Side3 = { (50 * sin(g_Math.GRD_TO_BOG(angToLocal))),(50 * cos(g_Math.GRD_TO_BOG(angToLocal))) };
			Vector2D Side4 = { (50 * sin(g_Math.GRD_TO_BOG(angToLocal + 180))) ,(50 * cos(g_Math.GRD_TO_BOG(angToLocal + 180))) };

			Vector Origin = pPlayerEntity->GetOrigin();

			Vector2D OriginLeftRight[] = { Vector2D(Side1.x, Side1.y), Vector2D(Side2.x, Side2.y) };

			Vector2D OriginLeftRightLocal[] = { Vector2D(Side3.x, Side3.y), Vector2D(Side4.x, Side4.y) };

			for (int side = 0; side < 2; side++)
			{
				Vector OriginAutowall = { Origin.x + OriginLeftRight[side].x,  Origin.y - OriginLeftRight[side].y , Origin.z + 90 };
				Vector OriginAutowall2 = { ViewPoint.x + OriginLeftRightLocal[side].x,  ViewPoint.y - OriginLeftRightLocal[side].y , ViewPoint.z };

				if (g_Autowall.CanHitFloatingPoint(OriginAutowall, ViewPoint))
				{
					switch (side)
					{
					case 0:
						HitSide1 = true;
						FreestandAngle[pPlayerEntity->EntIndex()] = 90;
						break;
					case 1:
						HitSide2 = true;
						FreestandAngle[pPlayerEntity->EntIndex()] = -90;
						break;
					}
					/*	if (side == 0)
						{
							HitSide1 = true;
							FreestandAngle[pPlayerEntity->EntIndex()] = 90;
						}
						if (side == 1)
						{
							HitSide2 = true;
							FreestandAngle[pPlayerEntity->EntIndex()] = -90;
						}*/

					Autowalled = true;
				}
				else
				{
					for (int side222 = 0; side222 < 2; side222++)
					{
						Vector OriginAutowall222 = { Origin.x + OriginLeftRight[side222].x,  Origin.y - OriginLeftRight[side222].y , Origin.z + 90 };

						if (g_Autowall.CanHitFloatingPoint(OriginAutowall222, OriginAutowall2))
						{
							switch (side222)
							{
							case 0:
								HitSide1 = true;
								FreestandAngle[pPlayerEntity->EntIndex()] = 90;
								break;
							case 1:
								HitSide2 = true;
								FreestandAngle[pPlayerEntity->EntIndex()] = -90;
								break;
							}
							/*	if (side222 == 0)
								{
									HitSide1 = true;
									FreestandAngle[pPlayerEntity->EntIndex()] = 90;
								}
								if (side222 == 1)
								{
									HitSide2 = true;
									FreestandAngle[pPlayerEntity->EntIndex()] = -90;
								}*/

							Autowalled = true;
						}
					}
				}
			}

			if (Autowalled)
			{
				if (HitSide1 && HitSide2)
					UseFreestandAngle[pPlayerEntity->EntIndex()] = false;
				else
					UseFreestandAngle[pPlayerEntity->EntIndex()] = true;
			}
		}
	}
}

void Resolver::FrameStage(ClientFrameStage_t stage)
{
	if (!Globals::LocalPlayer || !g_pEngine->IsInGame())
		return;

	static bool  wasDormant[65];

	for (int i = 1; i < g_pEngine->GetMaxClients(); ++i)
	{
		C_BaseEntity* pPlayerEntity = g_pEntityList->GetClientEntity(i);
		if (!pPlayerEntity
			|| !pPlayerEntity->IsAlive())
			continue;
		if (pPlayerEntity->IsDormant())
		{
			wasDormant[i] = true;
			continue;
		}
		if (stage == FRAME_RENDER_START)
		{
			HandleHits(pPlayerEntity);
			AnimationFix(pPlayerEntity);
			is_slow_walking(pPlayerEntity);
		}
		/*if (stage == FRAME_NET_UPDATE_POSTDATAUPDATE_START) {
			HandleBackUpResolve(pPlayerEntity);
		}*/

		if (stage == FRAME_NET_UPDATE_END && pPlayerEntity != Globals::LocalPlayer)
		{
			auto VarMap = reinterpret_cast<uintptr_t>(pPlayerEntity) + 36;
			auto VarMapSize = *reinterpret_cast<int*>(VarMap + 20);
			for (auto index = 0; index < VarMapSize; index++)
				*reinterpret_cast<uintptr_t*>(*reinterpret_cast<uintptr_t*>(VarMap) + index * 12) = 0;
		}
		wasDormant[i] = false;
	}
}
bool solve_desync_simple(C_BaseEntity* e) // 979
{
	if (!e || e->IsDormant() || !e->IsAlive())
		return false;

	for (size_t i = 0; i < e->NumOverlays(); i++)
	{
		auto layer = e->get_anim_overlay_index(i);
		if (!layer)
			continue;

		/*		if (e->get_sequence_act(layer->m_nSequence) == 979)
				{
					if (layer->m_flWeight == 0.0f && (layer->m_flCycle == 0.0f || layer->m_flCycle != layer->m_flPrevCycle))
						return true;
				}*/
	}
	return false;
}
float NormalizeYaw180(float yaw)
{
	if (yaw > 180)
		yaw -= (round(yaw / 360) * 360.f);
	else if (yaw < -180)
		yaw += (round(yaw / 360) * -360.f);

	return yaw;
}
float angle_difference(float a, float b) {
	auto diff = NormalizeYaw180(a - b);

	if (diff < 180)
		return diff;
	return diff - 360;
}
bool delta_58(float first, float second)
{
	if (first - second < 58.f && first - second > -58.f)
	{
		return true;
	}
	return false;
}
float Resolver::get_gun(C_BaseCombatWeapon* weapon)
{

	if (!weapon)
		return 0.f;

	if (weapon->isAuto())
		return 40.f;

	else if (weapon->is_scout())
		return 70.f;

	else if (weapon->is_awp())
		return 30.f;

	else
		return 34.f;
}
bool Resolver::enemy_is_slow_walking(C_BaseEntity* entity)
{
	C_BaseCombatWeapon* weapon = entity->GetActiveWeapon();
	if (!weapon)
		return false;

	float speed = entity->GetVelocity().Length2D();
	float get_gun = g_Resolver.get_gun(weapon);

	if (speed - get_gun < 34) // if it's more or less the same.
	{
		return true;
	}
	return false;
}
float approach(float cur, float target, float inc) {
	inc = abs(inc);

	if (cur < target)
		return min(cur + inc, target);
	if (cur > target)
		return max(cur - inc, target);

	return target;
}
float approach_angle(float cur, float target, float inc) {
	auto diff = angle_difference(target, cur);
	return approach(cur, cur + diff, inc);
}
void Beta(C_BaseEntity* pEnt)
{

	if (!c_config::get().aimbot_resolver == 1)
		return;
	if (pEnt->GetTeam() == Globals::LocalPlayer->GetTeam())
		return;
	const auto player_animation_state = pEnt->AnimState();
	if (!player_animation_state)
		return;
	if (Globals::MissedShots[pEnt->EntIndex()] > 2) {
		switch (Globals::MissedShots[pEnt->EntIndex()] % 4) {
		case 0: player_animation_state->m_flGoalFeetYaw = player_animation_state->m_flGoalFeetYaw + 45; break;
		case 1: player_animation_state->m_flGoalFeetYaw = player_animation_state->m_flGoalFeetYaw - 45; break;
		case 2: player_animation_state->m_flGoalFeetYaw = player_animation_state->m_flGoalFeetYaw - 30; break;
		case 3: player_animation_state->m_flGoalFeetYaw = player_animation_state->m_flGoalFeetYaw + 30; break;
		}
	}
	else {

		float m_flLastClientSideAnimationUpdateTimeDelta = fabs(player_animation_state->m_iLastClientSideAnimationUpdateFramecount - player_animation_state->m_flLastClientSideAnimationUpdateTime);
		auto v48 = 0.f;
		if (player_animation_state->m_flFeetSpeedForwardsOrSideWays >= 0.0f)
		{
			v48 = fminf(player_animation_state->m_flFeetSpeedForwardsOrSideWays, 1.0f);
		}
		else
		{
			v48 = 0.0f;
		}

		float v49 = ((player_animation_state->m_flStopToFullRunningFraction * -0.30000001) - 0.19999999) * v48;
		float flYawModifier = v49 + 1.0;
		if (player_animation_state->m_fDuckAmount > 0.0)
		{
			float v53 = 0.0f;

			if (player_animation_state->m_flFeetSpeedUnknownForwardOrSideways >= 0.0)
			{
				v53 = fminf(player_animation_state->m_flFeetSpeedUnknownForwardOrSideways, 1.0);
			}
			else
			{
				v53 = 0.0f;
			}
		}

		auto slow_walking = [&pEnt]() -> bool
		{
			bool s = g_Resolver.enemy_is_slow_walking(pEnt);

			if (s)
			{
				if (!(pEnt->GetFlags() & FL_DUCKING))
				{
					g_Resolver.enemy_slowwalk = true;
					return true;
				}
				else
				{
					g_Resolver.enemy_slowwalk = false;
					return false;
				}
			}
			else
				return false;
		};
		if (slow_walking() && Globals::MissedShots[pEnt->EntIndex()] >= 1)
		{
			Globals::enemyslow[pEnt->EntIndex()] = true;
		}
		auto max_rotate = max_desync_angle(pEnt);
		auto delta = angle_difference(pEnt->GetEyeAnglesXY()->y, pEnt->AnimState()->m_flCurrentFeetYaw);
		float flMaxYawModifier = player_animation_state->pad10[516] * flYawModifier;
		float flMinYawModifier = player_animation_state->pad10[512] * flYawModifier;
		float newFeetYaw = 0.f;
		auto eyeYaw = player_animation_state->m_flEyeYaw;
		auto lbyYaw = player_animation_state->m_flGoalFeetYaw;
		float eye_feet_delta = fabs(eyeYaw - lbyYaw);
		if (eye_feet_delta <= flMaxYawModifier)
		{
			if (flMinYawModifier > eye_feet_delta)
			{
				newFeetYaw = fabs(flMinYawModifier) + eyeYaw;
			}
		}
		else
		{
			newFeetYaw = eyeYaw - fabs(flMaxYawModifier);
		}

		float v136 = fmod(newFeetYaw, 180.0);
		if (v136 > 180.0)
		{
			v136 = v136 - 360.0;
		}
		if (v136 < 180.0)
		{
			v136 = v136 + 360.0;
		}
		player_animation_state->m_flGoalFeetYaw = v136;
	}
}