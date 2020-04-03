#pragma once

#include "Component.h"

class Graphics;
class Model;
class VertexShader;
class PixelShader;

class Renderer : public Component {
	MANAGED_OBJECT(Renderer)
public:
	Renderer(GameObject* gameObj) : Component(gameObj) {}

	std::shared_ptr<VertexShader> Vshader;
	std::shared_ptr<PixelShader> Pshader;
	std::shared_ptr<Model> Model;
};