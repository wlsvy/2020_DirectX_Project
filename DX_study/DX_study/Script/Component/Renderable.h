#pragma once
#include <vector>
#include <cassert>
#include "Component.h"

class Graphics;
class Model;
class SkinnedModel;
class VertexShader;
class PixelShader;
class Animator;
class MeshBase;
class SharedMaterial;
class Material;

class Renderable {
public:
	Renderable() {}
	Renderable(
		const std::shared_ptr<MeshBase> & mesh, 
		const std::shared_ptr<Material>& material) :
		m_Material(material), 
		m_Mesh(mesh) 
	{
		assert("Not Valid Parameters" && m_Material && m_Mesh);
	}

	const std::shared_ptr<Material> GetMaterial() const		{ return m_Material; }
	const std::shared_ptr<MeshBase> GetMesh() const			{ return m_Mesh; }

private:
	std::shared_ptr<Material> m_Material;
	std::shared_ptr<MeshBase> m_Mesh;
};

class RenderInfo : public Component {
	friend class Graphics;
	MANAGED_OBJECT(RenderInfo)
	COMPONENT_CONSTRUCTOR(RenderInfo, Component)
public:
	
	const std::shared_ptr<Model> GetModel() const			{ return m_Model; }
	const std::vector<Renderable> & GetRenerables() const	{ return m_Renderables; }
	bool IsVisible() const									{ return m_IsVisible; }

	void SetModel(const std::shared_ptr<Model>& model);
	void OnGui() override;

	std::shared_ptr<VertexShader> Vshader;
	std::shared_ptr<PixelShader> Pshader;
	std::shared_ptr<Animator> Anim;

private:
	std::shared_ptr<Model> m_Model;
	std::vector<Renderable> m_Renderables;

	bool m_IsVisible;
};
