#pragma once
#include <vector>
#include "Component.h"

class Graphics;
class Model;
class SkinnedModel;
class VertexShader;
class PixelShader;
class Animator;
class MeshBase;
class SharedMaterial;

class Renderable {
public:
	Renderable() {}
	Renderable(const std::shared_ptr<SharedMaterial>& material, const std::shared_ptr<MeshBase> & mesh) : Material(material), Mesh(mesh) {}

	std::shared_ptr<SharedMaterial> Material;
	std::shared_ptr<MeshBase> Mesh;
};

class RenderInfo : public Component {
	friend class Graphics;
	MANAGED_OBJECT(RenderInfo)
	COMPONENT_CONSTRUCTOR(RenderInfo, Component)
public:
	
	//void SetModel(const std::shared_ptr<Model>& model);
	bool IsVisible() const { return m_IsVisible; }
	void OnGui() override;

	std::shared_ptr<VertexShader> Vshader;
	std::shared_ptr<PixelShader> Pshader;
	std::shared_ptr<Model> Model;
	std::shared_ptr<Animator> Anim;

	

private:
	bool m_IsVisible;

	std::vector<Renderable> m_Renderables;
};
