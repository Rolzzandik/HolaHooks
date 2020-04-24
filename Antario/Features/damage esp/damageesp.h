#pragma once




#include <array>

class IGameEvent;
#include "../../SDK/Vector.h"


namespace DamageESP
{
	class FloatingText
	{
	public:
		FloatingText() :
			valid(false)
		{ }

		bool valid;
		float startTime;
		int damage;
		int hitgroup;
		Vector hitPosition;
		int randomIdx;
	};

	const int MAX_FLOATING_TEXTS = 50;
	extern std::array<FloatingText, MAX_FLOATING_TEXTS> floatingTexts;

	void HandleGameEvent(IGameEvent* pEvent);
	void Draw();
};