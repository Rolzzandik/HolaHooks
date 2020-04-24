#pragma once
#include "..\..\Utils\GlobalVars.h"
#include "..\..\SDK\CGlobalVarsBase.h"
#include "..\..\SDK\IClientMode.h"
#include <deque>

class Resolver11
{
public:
	bool UseFreestandAngle11[65];
	float FreestandAngle11[65];

	float pitchHit11[65];

	void OnCreateMove11();
	void Yaw11(C_BaseEntity* ent);
	void FrameStage11(ClientFrameStage_t stage);
private:
	void AnimationFix11(C_BaseEntity* pEnt);
};
extern Resolver11 g_Resolver11;
