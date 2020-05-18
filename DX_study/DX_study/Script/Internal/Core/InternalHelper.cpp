#include "InternalHelper.h"
#include "../Engine/Engine.h"
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
	return Engine::Get().GetGraphics().GetDevice();
}

ID3D11DeviceContext * Core::GetDeviceContext()
{
	return Engine::Get().GetGraphics().GetDeviceContext();
}

Graphics & Core::GetGraphics()
{
	return Engine::Get().GetGraphics();
}

ConstantBuffer<GpuObjectBuffer>& Core::GetCbVertexShader()
{
	return Engine::Get().GetGraphics().GetCbVertexShader();
}

ConstantBuffer<GpuBoneBuffer> & Core::GetCbBoneData()
{
	return Engine::Get().GetGraphics().GetCbBoneInfo();
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
	return Core::Find(id);
}

std::string Core::GetBuildPath()
{
#ifdef _DEBUG //Debug Mode
#ifdef _WIN64 //x64
	return "..\\x64\\Debug\\";
#else //x86(win32)
	return "..\\Debug\\";
#endif 
#else //Release Mode
#ifdef _WIN64 //x64
	return "..\\x64\\Release\\";
#else //x86(win32)
	return "..\\Release\\";
#endif
#endif
}

std::string Core::GetShaderCsoFilePath() {
	return "hlsl\\cso\\";
}


