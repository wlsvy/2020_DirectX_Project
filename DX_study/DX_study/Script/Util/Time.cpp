#include "Time.h"

#include "../Internal/Core/InternalHelper.h"
#include "../Internal/Engine/Timer.h"

double Time::GetTime()
{
	return Core::GetTimer().GetTime();
}

double Time::GetDeltaTime()
{
	return Core::GetTimer().GetDeltaTime();
}
