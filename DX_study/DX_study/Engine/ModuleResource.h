#pragma once

class Engine;
class GraphicsManager;
class PhysicsModule;
struct ID3D11Device;
struct ID3D11DeviceContext;

struct CB_VS_vertexshader;
struct CB_VS_boneData;
template<typename T>
class ConstantBuffer;

namespace Module {
	Engine& GetEngine();
	GraphicsManager& GetGraphicsModule();

	ID3D11Device& GetDevice();
	ID3D11DeviceContext& GetDeviceContext();
	ConstantBuffer<CB_VS_vertexshader>& GetVertexCB();
	ConstantBuffer< CB_VS_boneData>& GetModelBoneCB();

}