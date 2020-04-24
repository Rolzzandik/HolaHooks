#include "AntiAim.h"
#include "..\Aimbot\Autowall.h"
#include "..\..\Utils\Utils.h"
#include "..\..\SDK\IVEngineClient.h"
#include "..\..\SDK\PlayerInfo.h"
#include "..\..\Utils\Math.h"
#include "..\..\Menu\Menu.h"
#include "..\..\Menu\config.h"
#include "..\..\Features\Aimbot\Aimbot.h"

bool Swtich = false;
static float SentYaw;
static bool FreestandingSide = false;

AntiAim g_AntiAim;

void FreeStanding() // should have rewritten
{
	static float FinalAngle;
	bool bside1 = false;
	bool bside2 = false;
	bool autowalld = false;

	float tempdis = 999999999.f;
	Vector2D LocalOrg = Vector2D(Globals::LocalPlayer->GetOrigin().x, Globals::LocalPlayer->GetOrigin().y);

	float closeYaw = 999.f;

	for (int i = 1; i <= g_pEngine->GetMaxClients(); ++i)
	{
		C_BaseEntity* pPlayerEntity = g_pEntityList->GetClientEntity(i);

		if (!pPlayerEntity
			|| !pPlayerEntity->IsAlive()
			|| pPlayerEntity->IsDormant()
			|| pPlayerEntity == Globals::LocalPlayer
			|| pPlayerEntity->GetTeam() == Globals::LocalPlayer->GetTeam())
			continue;

		Vector2D EnemyOrg = Vector2D(pPlayerEntity->GetOrigin().x, pPlayerEntity->GetOrigin().y);


		if (tempdis > fabs(g_Math.Distance(EnemyOrg, LocalOrg)))
		{
			closeYaw = g_Math.NormalizeYaw(g_Math.CalcAngle(Globals::LocalPlayer->GetOrigin(), pPlayerEntity->GetOrigin()).y);
			tempdis = fabs(g_Math.Distance(EnemyOrg, LocalOrg));
		}

		if (c_config::get().Freestanding)
		{
			float angToLocal = g_Math.CalcAngle(Globals::LocalPlayer->GetOrigin(), pPlayerEntity->GetOrigin()).y;
			Vector ViewPoint = pPlayerEntity->GetOrigin() + Vector(0, 0, 90);

			Vector2D Side1 = { (45 * sin(g_Math.GRD_TO_BOG(angToLocal))),(45 * cos(g_Math.GRD_TO_BOG(angToLocal))) };
			Vector2D Side2 = { (45 * sin(g_Math.GRD_TO_BOG(angToLocal + 180))) ,(45 * cos(g_Math.GRD_TO_BOG(angToLocal + 180))) };

			Vector2D Side3 = { (50 * sin(g_Math.GRD_TO_BOG(angToLocal))),(50 * cos(g_Math.GRD_TO_BOG(angToLocal))) };
			Vector2D Side4 = { (50 * sin(g_Math.GRD_TO_BOG(angToLocal + 180))) ,(50 * cos(g_Math.GRD_TO_BOG(angToLocal + 180))) };

			Vector Origin = Globals::LocalPlayer->GetOrigin();

			Vector2D OriginLeftRight[] = { Vector2D(Side1.x, Side1.y), Vector2D(Side2.x, Side2.y) };

			Vector2D OriginLeftRightLocal[] = { Vector2D(Side3.x, Side3.y), Vector2D(Side4.x, Side4.y) };

			for (int side = 0; side < 2; side++)
			{
				Vector OriginAutowall = { Origin.x + OriginLeftRight[side].x,  Origin.y - OriginLeftRight[side].y , Origin.z + 80 };
				Vector OriginAutowall2 = { ViewPoint.x + OriginLeftRightLocal[side].x,  ViewPoint.y - OriginLeftRightLocal[side].y , ViewPoint.z };

				if (g_Autowall.CanHitFloatingPoint(OriginAutowall, ViewPoint))
				{
					if (side == 0)
					{
						FreestandingSide = true;
						bside1 = true;
						FinalAngle = angToLocal + 90;
					}
					else if (side == 1)
					{
						FreestandingSide = false;
						bside2 = true;
						FinalAngle = angToLocal - 90;
					}
					autowalld = true;
				}
				else
				{
					for (int side222 = 0; side222 < 2; side222++)
					{
						Vector OriginAutowall222 = { Origin.x + OriginLeftRight[side222].x,  Origin.y - OriginLeftRight[side222].y , Origin.z + 80 };

						if (g_Autowall.CanHitFloatingPoint(OriginAutowall222, OriginAutowall2))
						{
							if (side222 == 0)
							{
								FreestandingSide = true;
								bside1 = true;
								FinalAngle = angToLocal + 90;
							}
							else if (side222 == 1)
							{
								FreestandingSide = false;
								bside2 = true;
								FinalAngle = angToLocal - 90;
							}
							autowalld = true;
						}
					}
				}
			}
		}
	}

	if (!autowalld || (bside1 && bside2))
	{
		if (closeYaw != 999.f && c_config::get().AtTarget)
			Globals::pCmd->viewangles.y = g_Math.NormalizeYaw(closeYaw + SentYaw);
		else
			Globals::pCmd->viewangles.y += SentYaw;
	}
	else
		Globals::pCmd->viewangles.y = FinalAngle;
}

bool LBYUpdate()
{
	static float NextUpdate = 0;
	float Curtime = g_pGlobalVars->curtime;
	auto* AnimState = Globals::LocalPlayer->AnimState();

	if (!AnimState || !(Globals::LocalPlayer->GetFlags() & FL_ONGROUND))
		return false;

	if (AnimState->speed_2d > 0.1f)
		NextUpdate = Curtime + 0.22f;

	if (NextUpdate < Curtime)
	{
		NextUpdate = Curtime + 1.1f;
		return true;
	}

	return false;
}

void Real()
{
	float Yaw = 0.f;
	if (Globals::bSendPacket)
	{
		switch (c_config::get().yawAntiAim)
		{
		case 0:
			Globals::pCmd->viewangles.y;
			break;
		case 1:
			Yaw += 180.f;
			break;
		case 2:
			Yaw += 0.f;//lol
			break;
		case 3:
			Yaw += Swtich ? -90.f : 90.f;
			break;
		case 4:
			Yaw = g_Math.NormalizeYaw(g_pGlobalVars->curtime * 360.f);
			break;
		}

		SentYaw = Yaw;

		FreeStanding();

		if (c_config::get().JitterRange != 0)
		{
			float Offset = c_config::get().JitterRange / 2.f;

			if (!c_config::get().RandJitterInRange)
			{
				Swtich ? Globals::pCmd->viewangles.y -= Offset : Globals::pCmd->viewangles.y += Offset;
			}
			else
			{
				static bool oldSwtich = Swtich;

				Globals::pCmd->viewangles.y -= Offset;

				static int Add = 0;

				if (oldSwtich != Swtich)
				{
					Add = rand() % c_config::get().JitterRange;
					oldSwtich = Swtich;
				}

				Globals::pCmd->viewangles.y += Add;
			}
		}

		SentYaw = Globals::pCmd->viewangles.y;
	}

	if (!Globals::bSendPacket && Globals::LocalPlayer->AnimState() && g_pEngine->GetNetChannel() && c_config::get().DesyncAngle) // yea i have absolutly no clue on how todo desync just pasted in someting stacker gave me
	{
		auto NetChannel = g_pEngine->GetNetChannel();
		float Delta = Globals::LocalPlayer->GetMaxDesyncDelta();
		bool LBYUpdateb = LBYUpdate();

		if (!c_config::get().Freestanding)
			if (LBYUpdateb)
				FreestandingSide = !FreestandingSide;

		if (FreestandingSide)
		{
			if (LBYUpdateb)
				Globals::pCmd->viewangles.y = SentYaw - 120.f;
			else
				Globals::pCmd->viewangles.y = SentYaw - Globals::LocalPlayer->AnimState()->speed_2d <= 0.1 ? (Delta + 30.f) : Delta;
		}
		else
		{
			if (LBYUpdateb)
				Globals::pCmd->viewangles.y = SentYaw + 120.f;
			else
				Globals::pCmd->viewangles.y = SentYaw + Globals::LocalPlayer->AnimState()->speed_2d <= 0.1 ? (Delta + 30.f) : Delta;
		}
	}
	else
		Globals::pCmd->viewangles.y = SentYaw;
}

void Pitch()
{
	switch (c_config::get().pitchAntiAim)
	{
	case 0:
		Globals::pCmd->viewangles.x;
		break;
	case 1:
		Globals::pCmd->viewangles.x = 89.9f;
		break;
	case 2:
		Globals::pCmd->viewangles.x = Swtich ? 991.f : 1080.f;
		break;
	case 3:
		Globals::pCmd->viewangles.x = 1080.f;
		break;
	case 4:
	{
		static bool chjang = false;
		static float spin = 0;
		if (chjang != Swtich)
		{
			spin += 10.f;
			chjang = Swtich;
		}

		if (spin > 80.f)
			spin = 0.f;

		Globals::pCmd->viewangles.x = 1080.f - spin;
	}

	break;

	case 5: Globals::pCmd->viewangles.x = -89.9f; break;
	case 6: Globals::pCmd->viewangles.x = 991.f; break;
	}
}

void AntiAim::OnCreateMove()
{
	if (!g_pEngine->IsInGame() || !g_pEngine->IsConnected() || !Globals::LocalPlayer)
		return;

	if (!c_config::get().antiaim_enabled)
		return;

	if (!Globals::LocalPlayer->IsAlive())
		return;

	if (!Globals::LocalPlayer->GetActiveWeapon() || Globals::LocalPlayer->IsNade())
		return;

	if (Globals::LocalPlayer->GetMoveType() == MoveType_t::MOVETYPE_LADDER)
		return;

	C_BaseCombatWeapon* weapon = Globals::LocalPlayer->GetActiveWeapon();
	if (weapon && weapon->GetItemDefinitionIndex() != ItemDefinitionIndex::WEAPON_REVOLVER)
	{
		float flServerTime = Globals::LocalPlayer->GetTickBase() * g_pGlobalVars->intervalPerTick;
		bool canShoot = (weapon->GetNextPrimaryAttack() <= flServerTime);
		if (canShoot && (Globals::pCmd->buttons & IN_ATTACK))
			return;
	}

	float flServerTime = Globals::LocalPlayer->GetTickBase() * g_pGlobalVars->intervalPerTick;
	bool canShoot = (Globals::LocalPlayer->GetActiveWeapon()->GetNextPrimaryAttack() <= flServerTime);

	//if (canShoot && (Globals::pCmd->buttons & IN_ATTACK))
	//	return;

	if (Globals::pCmd->buttons & IN_USE)
	{
		Globals::bSendPacket = true;
		return;
	}

	if (Globals::bSendPacket)
		Swtich = !Swtich;

	Pitch();
	Real();
}