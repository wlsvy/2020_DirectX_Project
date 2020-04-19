#pragma once

#include "Component.h"

class Graphics;
class Model;
class SkinnedModel;
class VertexShader;
class PixelShader;
class Animator;

class Renderable : public Component {
	MANAGED_OBJECT(Renderable)
	COMPONENT_CONSTRUCTOR(Renderable, Component)
public:

	std::shared_ptr<VertexShader> Vshader;
	std::shared_ptr<PixelShader> Pshader;
	std::shared_ptr<Model> Model;
	std::shared_ptr<SkinnedModel> SkinnedModel;
	std::shared_ptr<Animator> Anim;
};