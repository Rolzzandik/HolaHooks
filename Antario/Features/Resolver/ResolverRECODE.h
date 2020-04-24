#pragma once
#include "..\..\Utils\GlobalVars.h"
#include "..\..\SDK\CGlobalVarsBase.h"
#include "..\..\SDK\IClientMode.h"
#include <deque>

class Resolver3 {
public:
	bool UseFreestandAngle3[65];
	float FreestandAngle3[65];
	Vector absOriginBackup3;
	float pitchHit3[65];

	void FrameStage3(ClientFrameStage_t stage);
	void OnCreateMove3();
private:
	void Resolve(C_BaseEntity* pEnt);
};
extern Resolver3 g_Resolver3;