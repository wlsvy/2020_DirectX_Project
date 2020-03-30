#include "Core.h"
#include "Engine.h"
#include "DeviceResources.h"

Engine & Core::GetEngine()
{
	return Engine::GetInstance();
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

