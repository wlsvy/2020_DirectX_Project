#include "Time.h"

#include "../Internal/Core.h"
#include "../Internal/Timer.h"

double Time::GetTime()
{
	return Core::GetTimer().GetTime();
}

double Time::GetDeltaTime()
{
	return Core::GetTimer().GetDeltaTime();
}
