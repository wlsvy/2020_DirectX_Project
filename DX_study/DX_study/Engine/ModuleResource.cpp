#include "ModuleResource.h"
#include "Engine.h"
#include "../Graphics/Graphics.h"
#include "AnimationManager.h"

Engine & Module::GetEngine()
{
	return Engine::GetInstance();
}

GraphicsManager & Module::GetGraphicsModule()
{
	return Engine::GetInstance().GetGraphicsModule();
}

AnimationManager & Module::GetAnimationManager()
{
	return Engine::GetInstance().GetAnimationManager();
}

ID3D11Device & Module::GetDevice()
{
	return Engine::GetInstance().GetGraphicsModule().GetDevice();
}

ID3D11DeviceContext & Module::GetDeviceContext()
{
	return Engine::GetInstance().GetGraphicsModule().GetDeviceContext();
}

ConstantBuffer<CB_VS_vertexshader>& Module::GetVertexCB()
{
	return Engine::GetInstance().GetGraphicsModule().GetCbVertexShader();
}

ConstantBuffer<CB_VS_boneData>& Module::GetModelBoneCB()
{
	return Engine::GetInstance().GetGraphicsModule().GetCbBoneInfo();
}

std::shared_ptr<Texture> Module::GetTexture(const std::string & name)
{
	return Engine::GetInstance().GetGraphicsModule().GetTexture(name);
}

void Module::RegisterTexture(const std::shared_ptr<Texture>& texture)
{
	Engine::GetInstance().GetGraphicsModule().RegisterTexture(texture);
}

void Module::RegisterAnimClip(const std::shared_ptr<AnimationClip>& clip)
{
	Engine::GetInstance().GetAnimationManager().RegisterClip(clip);
}

std::shared_ptr<AnimationClip> Module::GetAnimationClip(const std::string & name)
{
	return GetAnimationManager().GetClip(name);
}
