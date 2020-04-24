#include "ResolverBETA.h"
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

Resolver11 g_Resolver11;


void Resolver11::AnimationFix11(C_BaseEntity* pEnt)
{
	static float oldSimtime[65];
	static float storedSimtime[65];
	static float ShotTime[65];
	static float SideTime[65][3];
	static int LastDesyncSide[65];
	static bool Delaying[65];
	static AnimationLayer StoredLayers[64][15];
	static C_AnimState* StoredAnimState[65];
	static float StoredPosParams[65][24];
	static Vector oldEyeAngles[65];
	static float oldGoalfeetYaw[65];
	float* PosParams = (float*)((uintptr_t)pEnt + 0x2774);
	bool update = false;
	bool shot = false;

	static bool jittering[65];

	auto* AnimState = pEnt->AnimState();
	if (!AnimState || !pEnt->AnimOverlays() || !PosParams)
		return;

	auto RemapVal = [](float val, float A, float B, float C, float D) -> float
	{
		if (A == B)
			return val >= B ? D : C;
		return C + (D - C) * (val - A) / (B - A);
	};

	if (storedSimtime[pEnt->EntIndex()] != pEnt->GetSimulationTime())
	{
		jittering[pEnt->EntIndex()] = false;
		pEnt->ClientAnimations(true);
		pEnt->UpdateClientAnimation();

		memcpy(StoredPosParams[pEnt->EntIndex()], PosParams, sizeof(float) * 24);
		memcpy(StoredLayers[pEnt->EntIndex()], pEnt->AnimOverlays(), (sizeof(AnimationLayer) * pEnt->NumOverlays()));

		oldGoalfeetYaw[pEnt->EntIndex()] = AnimState->m_flGoalFeetYaw;

		if (pEnt->GetActiveWeapon() && !pEnt->IsKnifeorNade())
		{
			if (ShotTime[pEnt->EntIndex()] != pEnt->GetActiveWeapon()->GetLastShotTime())
			{
				shot = true;
				ShotTime[pEnt->EntIndex()] = pEnt->GetActiveWeapon()->GetLastShotTime();
			}
			else
				shot = false;
		}
		else
		{
			shot = false;
			ShotTime[pEnt->EntIndex()] = 0.f;
		}

		float angToLocal = g_Math.NormalizeYaw(g_Math.CalcAngle(Globals::LocalPlayer->GetOrigin(), pEnt->GetOrigin()).y);

		float Back = g_Math.NormalizeYaw(angToLocal);
		float DesyncFix = 0;
		float Resim = g_Math.NormalizeYaw((0.24f / (pEnt->GetSimulationTime() - oldSimtime[pEnt->EntIndex()])) * (oldEyeAngles[pEnt->EntIndex()].y - pEnt->GetEyeAngles().y));

		if (Resim > 58.f)
			Resim = 58.f;
		if (Resim < -58.f)
			Resim = -58.f;

		if (pEnt->GetVelocity().Length2D() > 0.5f && !shot)
		{
			float Delta = g_Math.NormalizeYaw(g_Math.NormalizeYaw(g_Math.CalcAngle(Vector(0, 0, 0), pEnt->GetVelocity()).y) - g_Math.NormalizeYaw(g_Math.NormalizeYaw(AnimState->m_flGoalFeetYaw + RemapVal(PosParams[11], 0, 1, -60, 60)) + Resim));

			int CurrentSide = 0;

			if (Delta < 0)
			{
				CurrentSide = 1;
				SideTime[pEnt->EntIndex()][1] = g_pGlobalVars->curtime;
			}
			else if (Delta > 0)
			{
				CurrentSide = 2;
				SideTime[pEnt->EntIndex()][2] = g_pGlobalVars->curtime;
			}

			if (LastDesyncSide[pEnt->EntIndex()] == 1)
			{
				Resim += (58.f - Resim);
				DesyncFix += (58.f - Resim);
			}
			if (LastDesyncSide[pEnt->EntIndex()] == 2)
			{
				Resim += (-58.f - Resim);
				DesyncFix += (-58.f - Resim);
			}

			if (LastDesyncSide[pEnt->EntIndex()] != CurrentSide)
			{
				Delaying[pEnt->EntIndex()] = true;

				if (.5f < (g_pGlobalVars->curtime - SideTime[pEnt->EntIndex()][LastDesyncSide[pEnt->EntIndex()]]))
				{
					LastDesyncSide[pEnt->EntIndex()] = CurrentSide;
					Delaying[pEnt->EntIndex()] = false;
				}
			}

			if (!Delaying[pEnt->EntIndex()])
				LastDesyncSide[pEnt->EntIndex()] = CurrentSide;
		}
		else if (!shot)
		{
			float Brute = UseFreestandAngle11[pEnt->EntIndex()] ? g_Math.NormalizeYaw(Back + FreestandAngle11[pEnt->EntIndex()]) : pEnt->GetLowerBodyYaw();

			float Delta = g_Math.NormalizeYaw(g_Math.NormalizeYaw(Brute - g_Math.NormalizeYaw(g_Math.NormalizeYaw(AnimState->m_flGoalFeetYaw + RemapVal(PosParams[11], 0, 1, -60, 60))) + Resim));

			if (Delta > 58.f)
				Delta = 58.f;
			if (Delta < -58.f)
				Delta = -58.f;

			Resim += Delta;
			DesyncFix += Delta;

			if (Resim > 58.f)
				Resim = 58.f;
			if (Resim < -58.f)
				Resim = -58.f;
		}

		float Equalized = g_Math.NormalizeYaw(g_Math.NormalizeYaw(AnimState->m_flGoalFeetYaw + RemapVal(PosParams[11], 0, 1, -60, 60)) + Resim);

		float JitterDelta = fabs(g_Math.NormalizeYaw(oldEyeAngles[pEnt->EntIndex()].y - pEnt->GetEyeAngles().y));

		if (JitterDelta >= 70.f && !shot)
			jittering[pEnt->EntIndex()] = true;

		if (pEnt != Globals::LocalPlayer && pEnt->GetTeam() != Globals::LocalPlayer->GetTeam() && (pEnt->GetFlags() & FL_ONGROUND) && c_config::get().aimbot_resolver == 2)
		{
			if (jittering[pEnt->EntIndex()])
				AnimState->m_flGoalFeetYaw = g_Math.NormalizeYaw(pEnt->GetEyeAngles().y + DesyncFix);
			else
				AnimState->m_flGoalFeetYaw = Equalized;

			pEnt->SetLowerBodyYaw(AnimState->m_flGoalFeetYaw);
		}

		StoredAnimState[pEnt->EntIndex()] = AnimState;

		oldEyeAngles[pEnt->EntIndex()] = pEnt->GetEyeAngles();

		oldSimtime[pEnt->EntIndex()] = storedSimtime[pEnt->EntIndex()];

		storedSimtime[pEnt->EntIndex()] = pEnt->GetSimulationTime();

		update = true;
	}

	pEnt->ClientAnimations(false);

	if (pEnt != Globals::LocalPlayer && pEnt->GetTeam() != Globals::LocalPlayer->GetTeam() && (pEnt->GetFlags() & FL_ONGROUND) && c_config::get().aimbot_resolver == 2)
		pEnt->SetLowerBodyYaw(AnimState->m_flGoalFeetYaw);

	AnimState = StoredAnimState[pEnt->EntIndex()];

	memcpy((void*)PosParams, &StoredPosParams[pEnt->EntIndex()], (sizeof(float) * 24));
	memcpy(pEnt->AnimOverlays(), StoredLayers[pEnt->EntIndex()], (sizeof(AnimationLayer) * pEnt->NumOverlays()));

	if (pEnt != Globals::LocalPlayer && pEnt->GetTeam() != Globals::LocalPlayer->GetTeam() && (pEnt->GetFlags() & FL_ONGROUND) && c_config::get().aimbot_resolver == 2 && jittering[pEnt->EntIndex()])
		pEnt->SetAbsAngles(Vector(0, pEnt->GetEyeAngles().y, 0));
	else
		pEnt->SetAbsAngles(Vector(0, oldGoalfeetYaw[pEnt->EntIndex()], 0));

	*reinterpret_cast<int*>(uintptr_t(pEnt) + 0xA30) = g_pGlobalVars->framecount;
	*reinterpret_cast<int*>(uintptr_t(pEnt) + 0xA28) = 0;
}

float flAngleMod11(float flAngle)
{
	return((360.0f / 65536.0f) * ((int32_t)(flAngle * (65536.0f / 360.0f)) & 65535));
}

float ApproachAngle11(float target, float value, float speed)
{
	target = flAngleMod11(target);
	value = flAngleMod11(value);

	float delta = target - value;

	// Speed is assumed to be positive
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

void update_state11(C_AnimState* state, Vector angles) {
	using Fn = void(__vectorcall*)(void*, void*, float, float, float, void*);
	static auto fn = reinterpret_cast<Fn>(Utils::FindSignature("client_panorama.dll", "55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 F3 0F 11 54 24"));
	fn(state, nullptr, 0.0f, angles[1], angles[0], nullptr);
}

void HandleBackUpResolve(C_BaseEntity* pEnt) {

	if (!c_config::get().aimbot_resolver == 2)
		return;

	if (pEnt->GetTeam() == Globals::LocalPlayer->GetTeam())
		return;


	auto animstate = pEnt->AnimState();

	const auto player_animation_state = pEnt->AnimState();

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

	float flMaxYawModifier = player_animation_state->pad10[516] * flYawModifier;
	float flMinYawModifier = player_animation_state->pad10[512] * flYawModifier;

	float newFeetYaw = 1.f;

	auto eyeYaw = player_animation_state->m_flEyeYaw;

	auto lbyYaw = player_animation_state->m_flGoalFeetYaw;

	//auto eyeYaw = player_animation_state->m_flGoalFeetYaw; // б1г мем

	float eye_feet_delta = fabs(eyeYaw - lbyYaw);


	if (!player_animation_state)
		return;


	auto v28 = 0.f;

	if (player_animation_state->m_flFeetSpeedForwardsOrSideWays >= 0.0f)
	{
		v28 = fminf(player_animation_state->m_flFeetSpeedForwardsOrSideWays, 0.0f);
	}
	else
	{
		v28 = 0x3F800000;
	}

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





	/*if (eye_feet_delta <= flMaxYawModifier)
	{
		if (flMinYawModifier > eye_feet_delta)
		{
			newFeetYaw = fabs(flMinYawModifier) + eyeYaw;
		}
	}
	else
	{
		newFeetYaw = eyeYaw - fabs(flMaxYawModifier);
	}*/




	float v136 = fmod(newFeetYaw, 360.0);

	if (v136 > 180.0)
	{
		v136 = v136 - 360.0;
	}

	if (v136 < 180.0)
	{
		v136 = v136 + 360.0;
	}

	v28 = v49++;

	{                                             // inlined max_desync_delta
		float v9 = fabs(animstate->m_iLastClientSideAnimationUpdateFramecount - animstate->m_flLastClientSideAnimationUpdateTime);
		float speedfraction = 0.0;
		if (animstate->m_flFeetSpeedForwardsOrSideWays < 0.0)
			speedfraction = 0.0;
		else
			speedfraction = animstate->m_flFeetSpeedForwardsOrSideWays;

		float v2 = (animstate->m_flStopToFullRunningFraction * -0.30000001 - 0.19999999) * speedfraction;
		float v18 = v2;
		float v3 = v2 + 1.0;
		float v23 = v3;
		if (animstate->m_fDuckAmount > 0.0)
		{
			float v29 = 0.0;
			if (animstate->m_flFeetSpeedUnknownForwardOrSideways < 0.0)
				v29 = 0.0;
			else
				v29 = fminf((animstate->m_flFeetSpeedUnknownForwardOrSideways), 0x3F800000);
		}

		if (Globals::LocalPlayer)
		{
			for (int i = 1; i < g_pEngine->GetMaxClients(); ++i)
			{

				if (pEnt)// dormant
				{
					float v28 = pEnt->GetEyeAngles().y == 0.0 ? -45 : 60;
					if (v28)
						return;
					float v27 = pEnt->GetEyeAngles().y == 0.0 ? -30 : 30;
					if (v27)
						return;
					float v26 = pEnt->GetEyeAngles().y == 0.0 ? -90 : 90;
					if (v26)
						return;
					float v25 = pEnt->GetEyeAngles().y == 0.0 ? -180 : 90;
					if (v25)
						return;
					float v24 = pEnt->GetEyeAngles().y == 0.0 ? -360 : 360;
					if (v24)
						return;
				}
				float v8 = 0;
				float v7 = 0;
				float v6 = 0;
				float v5 = 0;
				float v4 = 0;
				for (size_t i = 0; i < pEnt->GetNumAnimOverlays(); i++)
				{
					auto layer = pEnt->GetNumAnimOverlays();
					if (!layer)
						continue;



					//if // (pEnt->GetSequenceActivity.layer->m_nSequence() == 979);
						//(pEnt->GetSequenceActivity.layer->m_nSequence() == 945);
					// лол мем кста annihilate
					v4 = pEnt->GetLowerBodyYaw();
				}
				float v20 = (animstate->m_vVelocityX) * v23;
				float a1 = (animstate->m_vVelocityY) * v23;
				float v30 = 0.0;
				float eye_angles_y = animstate->m_flEyeYaw;
				float goal_feet_yaw = animstate->m_flGoalFeetYaw;
				float v22 = fabs(eye_angles_y - goal_feet_yaw);
				if (v20 < v22)
				{
					float v11 = fabs(v20);
					v30 = eye_angles_y - v11;
				}
				else if (a1 > v22)
				{
					float v12 = fabs(a1);
					v30 = v12 + eye_angles_y;
				}
				float v36 = std::fmodf((v30), 360.0);
				if (v36 > 179.0)
					v36 = v36 - 360.0;
				if (v36 < 180.0)
					v36 = v36 + 360.0;
				animstate->m_flGoalFeetYaw = v36;
				if (Globals::MissedShots[pEnt->EntIndex()] > 4)
				{
					int v19 = Globals::MissedShots[pEnt->EntIndex()] % 3;
					switch (v19)
					{
					case 0:
						animstate->m_flGoalFeetYaw = animstate->m_flGoalFeetYaw + 55.0;
						break;
					case 1:
						animstate->m_flGoalFeetYaw = animstate->m_flGoalFeetYaw - 55.0;
						break;
					case 2:
						animstate->m_flGoalFeetYaw = animstate->m_flGoalFeetYaw - 70.0;
						break;
					case 3:
						animstate->m_flGoalFeetYaw = animstate->m_flGoalFeetYaw + 70.0;
						break;
					default:
						return;

						player_animation_state->m_flGoalFeetYaw = v136;
					}
				}
			}

		}
	}
}

void HandleHits11(C_BaseEntity* pEnt)
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
			g_Resolver11.pitchHit11[pEnt->EntIndex()] = pEnt->GetEyeAngles().x;
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

void Resolver11::OnCreateMove11()
{

	if (!c_config::get().aimbot_resolver == 2)
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
			UseFreestandAngle11[i] = false;
			continue;
		}

		if (abs(pPlayerEntity->GetVelocity().Length2D()) > 29.f)
			UseFreestandAngle11[pPlayerEntity->EntIndex()] = false;

		if (abs(pPlayerEntity->GetVelocity().Length2D()) <= 29.f && !UseFreestandAngle11[pPlayerEntity->EntIndex()])
		{
			bool Autowalled = false, HitSide1 = false, HitSide2 = false;

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
					if (side == 0)
					{
						HitSide1 = true;
						FreestandAngle11[pPlayerEntity->EntIndex()] = 90;
					}
					else if (side == 1)
					{
						HitSide2 = true;
						FreestandAngle11[pPlayerEntity->EntIndex()] = -90;
					}

					Autowalled = true;
				}
				else
				{
					for (int side222 = 0; side222 < 2; side222++)
					{
						Vector OriginAutowall222 = { Origin.x + OriginLeftRight[side222].x,  Origin.y - OriginLeftRight[side222].y , Origin.z + 90 };

						if (g_Autowall.CanHitFloatingPoint(OriginAutowall222, OriginAutowall2))
						{
							if (side222 == 0)
							{
								HitSide1 = true;
								FreestandAngle11[pPlayerEntity->EntIndex()] = 90;
							}
							else if (side222 == 1)
							{
								HitSide2 = true;
								FreestandAngle11[pPlayerEntity->EntIndex()] = -90;
							}

							Autowalled = true;
						}
					}
				}
			}

			if (Autowalled)
			{
				if (HitSide1 && HitSide2)
					UseFreestandAngle11[pPlayerEntity->EntIndex()] = false;
				else
					UseFreestandAngle11[pPlayerEntity->EntIndex()] = true;
			}
		}
	}
}

void Resolver11::FrameStage11(ClientFrameStage_t stage) {

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
			HandleHits11(pPlayerEntity);
			AnimationFix11(pPlayerEntity);
		}

		if (stage == FRAME_NET_UPDATE_POSTDATAUPDATE_START) {
			HandleBackUpResolve(pPlayerEntity);
		}

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