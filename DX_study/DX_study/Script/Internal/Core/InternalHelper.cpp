#include "InternalHelper.h"
#include "../Engine/Engine.h"
#include "../Engine/DeviceResources.h"
#include "../Graphics/Graphics.h"
#include "ObjectPool.h"
#include "Scene.h"

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
	return Engine::Get().GetGraphics().GetDeviceResources().GetDevice();
}

ID3D11DeviceContext * Core::GetDeviceContext()
{
	return Engine::Get().GetGraphics().GetDeviceResources().GetDeviceContext();
}

Graphics & Core::GetGraphics()
{
	return Engine::Get().GetGraphics();
}

ConstantBuffer<CB_VS_vertexshader>& Core::GetCbVertexShader()
{
	return Engine::Get().GetGraphics().GetCbVertexShader();
}

Scene & Core::GetCurrentScene()
{
	return Engine::Get().GetCurrentScene();
}

std::shared_ptr<Transform> Core::GetWorldTransform()
{
	return GetCurrentScene().GetWorldTransform();
}

std::shared_ptr<Object> Core::GetObjectById(int id)
{
	return Pool::Find(id);
}


