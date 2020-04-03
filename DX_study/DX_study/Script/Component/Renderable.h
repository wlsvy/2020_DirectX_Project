#pragma once

#include "Component.h"

class Graphics;
class Model;
class VertexShader;
class PixelShader;

class Renderable : public Component {
	MANAGED_OBJECT(Renderable)
	COMPONENT_CONSTRUCTOR(Renderable)
public:

	std::shared_ptr<VertexShader> Vshader;
	std::shared_ptr<PixelShader> Pshader;
	std::shared_ptr<Model> Model;
};