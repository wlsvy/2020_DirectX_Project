#pragma once
#include <memory>
#include <string>

class Engine;
class GraphicsManager;
class PhysicsModule;
class AnimationManager;
struct ID3D11Device;
struct ID3D11DeviceContext;

struct CB_VS_vertexshader;
struct CB_VS_boneData;
template<typename T>
class ConstantBuffer;
class Texture;
class AnimationClip;

namespace Module {
	Engine& GetEngine();
	GraphicsManager& GetGraphicsModule();
	AnimationManager& GetAnimationManager();

	ID3D11Device& GetDevice();
	ID3D11DeviceContext& GetDeviceContext();
	ConstantBuffer<CB_VS_vertexshader>& GetVertexCB();
	ConstantBuffer<CB_VS_boneData>& GetModelBoneCB();
	std::shared_ptr<Texture> GetTexture(const std::string & name);



	void RegisterTexture(const std::shared_ptr<Texture>& texture);
	void RegisterAnimClip(const std::shared_ptr<AnimationClip>& clip);
	std::shared_ptr<AnimationClip> GetAnimationClip(const std::string & name);
}