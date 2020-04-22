#pragma once
#include "../Core/Object.h"
#include <DirectXMath.h>
#include "../../Util/Color.h"

class VertexShader;
class PixelShader;
class Texture;

class Material : public Object {
	MANAGED_OBJECT(Material)
public:
	Material() {}
	Material(const std::string& name) : Object(name) {}

	std::shared_ptr<VertexShader> Vshader;
	std::shared_ptr<PixelShader> Pshader;
	std::shared_ptr<Texture> Texture;
	Color4Byte Color;
};