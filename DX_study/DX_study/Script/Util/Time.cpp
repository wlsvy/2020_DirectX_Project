#include "Time.h"

#include "../Internal/Core/InternalHelper.h"
#include "../Internal/Engine/Timer.h"
#include "../Internal//Engine/Engine.h"

double Time::GetTime()
{
	return Core::GetTimer().GetTime();
}

double Time::GetDeltaTime()
{
	return Core::GetTimer().GetDeltaTime();
}

float Time::GetFixedDeltaTime()
{
	return Engine::s_FixedFrameRate;
}
