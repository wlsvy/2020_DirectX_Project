#pragma once

#include "Component.h"

class Graphics;
class Model;
class SkinnedModel;
class VertexShader;
class PixelShader;
class Animator;
class MeshBase;

class Renderable : public Component {
	friend class Graphics;
	MANAGED_OBJECT(Renderable)
	COMPONENT_CONSTRUCTOR(Renderable, Component)
public:
	
	bool IsVisible() const { return m_IsVisible; }
	void OnGui() override;

	std::shared_ptr<VertexShader> Vshader;
	std::shared_ptr<PixelShader> Pshader;
	std::shared_ptr<Model> Model;
	std::shared_ptr<SkinnedModel> SkinnedModel;
	std::shared_ptr<Animator> Anim;
	std::weak_ptr<MeshBase> Mesh;

private:
	void Draw() const;

	bool m_IsVisible;
};