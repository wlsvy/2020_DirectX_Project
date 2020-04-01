#pragma once

class Engine;
class Timer;
struct ID3D11Device;
struct ID3D11DeviceContext;
class Graphics;
template<typename T>
class ConstantBuffer;
struct CB_VS_vertexshader;

namespace Core {
	Engine& GetEngine();
	Timer& GetTimer();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();

	Graphics& GetGraphics();
	ConstantBuffer<CB_VS_vertexshader>& GetCbVertexShader();

}