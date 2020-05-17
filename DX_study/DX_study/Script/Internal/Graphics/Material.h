#pragma once
#include <DirectXMath.h>
#include <vector>
#include "../Core/Object.h"
#include "../../Util/Color.h"

class VertexShader;
class PixelShader;
class GeometryShader;
class Texture;

class Material : public Object {
public:
	Material() {}
	Material(const std::string& name) : Object(name) {}
	void OnGui(const char* option = "-1") override;

	DirectX::XMFLOAT4 Color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	std::shared_ptr<VertexShader> Vshader;
	std::shared_ptr<PixelShader> Pshader;
	std::shared_ptr<GeometryShader> Gshader;

	std::shared_ptr<Texture> Albedo;

	std::shared_ptr<Texture> Normal;
	float NormalIntensity = 1.0f;

	std::shared_ptr<Texture> Metal;
	float MetalIntensity = 1.0f;

	std::shared_ptr<Texture> Roughness;
	float RoughnessIntensity = 1.0f;

	std::shared_ptr<Texture> Specular;
	float SpecularIntensity = 1.0f;

	std::vector<std::shared_ptr<Texture>> SubTextures;
};

class SharedMaterial : public Material {
	MANAGED_OBJECT(SharedMaterial)
public:
	static std::shared_ptr<SharedMaterial> GetDefault();

	SharedMaterial() {}
	SharedMaterial(const std::string& name) : Material(name) {}
};