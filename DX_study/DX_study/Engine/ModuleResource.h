#pragma once

class Engine;
class GraphicsManager;
class PhysicsModule;
struct ID3D11Device;
struct ID3D11DeviceContext;

namespace Module {
	Engine& GetEngine();
	GraphicsManager& GetGraphicsModule();
	ID3D11Device& GetDevice();
	ID3D11DeviceContext& GetDeviceContext();
}