#pragma once
#include <memory>
#include <string>

class Engine;
class Timer;
struct ID3D11Device;
struct ID3D11DeviceContext;
class Graphics;
template<typename T>
class ConstantBuffer;
struct GpuObjectBuffer;
struct GpuBoneBuffer;
class Scene;
class Transform;
class Object;

namespace Core {
	Engine& GetEngine();
	Timer& GetTimer();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();

	Graphics& GetGraphics();
	ConstantBuffer<GpuObjectBuffer> & GetCbVertexShader();
	ConstantBuffer<GpuBoneBuffer> & GetCbBoneData();

	Scene& GetCurrentScene();
	std::shared_ptr<Transform> GetWorldTransform();

	std::string GetBuildPath();
	std::string GetShaderCsoFilePath();
}