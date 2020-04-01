#include "InternalHelper.h"
#include "../Engine/Engine.h"
#include "../Engine/DeviceResources.h"

Engine & Core::GetEngine()
{
	return Engine::Get();
}

Timer & Core::GetTimer()
{
	return GetEngine().GetTimer();
}

ID3D11Device * Core::GetDevice()
{
	return DeviceResources::GetInstance().GetDevice();
}

ID3D11DeviceContext * Core::GetDeviceContext()
{
	return DeviceResources::GetInstance().GetDeviceContext();
}


