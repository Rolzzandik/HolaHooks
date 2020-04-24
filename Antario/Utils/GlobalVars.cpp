#include "GlobalVars.h"

namespace Globals
{
    CUserCmd*      pCmd         = nullptr;
    C_BaseEntity*  LocalPlayer = nullptr;
    std::uintptr_t uRandomSeed  = NULL;
	Vector         OriginalView;
	HMODULE	       Module = NULL;
	bool           bSendPacket  = true;
	bool		   LagPeek      = false;
	int            TargetIndex  = -1;
	Vector         EnemyEyeAngs[65];
	Vector         AimbotHitbox[65][27];
	Vector         RealAngle;
	Vector         FakeAngle;
	Vector         TpAngle;
	Vector         stored_angles;
	int            choked_commands;
	bool           fakeducked = false;
	bool           isfakeducking = false;
	bool           Shot[65];
	bool		   MissedShot;
	bool           Hit[65];
	int            MissedShots[65];
	bool enemyslow[65];
	bool			Manual_Side = true;
	bool           FreezeTime = false;
	bool			FakeDuck[65];
	float          GoalLby = 0;

	int kills = 0; 
	float killtime = 0.0f;
	int globalAlpha = 0;
	bool           newround;


	bool HitPlayer;
	bool DesyncEnabledSpecialChecks = false;
	std::string Username;
	float          w2s_matrix[4][4];
	DWORD CourierNew;
	DWORD SmallText;
	DWORD WeaponESP;
	DWORD WeaponIcon;
	DWORD TabFont;
	DWORD IndicatorFont;
	DWORD KillCounterFont;
}
