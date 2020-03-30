#pragma once

class Engine;
class Timer;
struct ID3D11Device;
struct ID3D11DeviceContext;

namespace Core {
	Engine& GetEngine();
	Timer& GetTimer();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();

}