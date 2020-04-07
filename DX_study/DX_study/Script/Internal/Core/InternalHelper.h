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
struct CB_VS_vertexshader;
class Scene;
class Transform;
class Object;

namespace Core {
	Engine& GetEngine();
	Timer& GetTimer();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();

	Graphics& GetGraphics();
	ConstantBuffer<CB_VS_vertexshader>& GetCbVertexShader();

	Scene& GetCurrentScene();
	std::shared_ptr<Transform> GetWorldTransform();

	std::shared_ptr<Object> GetObjectById(int id);

	std::string GetBuildPath();
}