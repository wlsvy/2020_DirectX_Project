#include "Core.h"
#include "Engine.h"

Engine & Core::GetEngine()
{
	return Engine::GetInstance();
}

Timer & Core::GetTimer()
{
	return GetEngine().GetTimer();
}
