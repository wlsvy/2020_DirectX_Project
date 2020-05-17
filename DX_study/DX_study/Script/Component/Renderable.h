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
class Mesh;
class SharedMaterial;
class Material;
class ShaderState;

class Renderable {
public:
	Renderable() {}
	Renderable(
		const std::shared_ptr<Mesh> & mesh,
		const std::shared_ptr<Material>& material,
		const std::shared_ptr<ShaderState>& shaderStat) :
		m_Material(material),
		m_Mesh(mesh),
		m_ShaderState(shaderStat)
	{
		assert("Not Valid Parameters" && m_Material && m_Mesh && m_ShaderState);
	}

	void SetMaterial(const std::shared_ptr<Material> & mat)					{ m_Material = mat; }
	void SetShaderState(const std::shared_ptr<ShaderState> & shaderState)	{ m_ShaderState = shaderState; }

	const std::shared_ptr<Material> GetMaterial() const						{ return m_Material; }
	const std::shared_ptr<Mesh> GetMesh() const								{ return m_Mesh; }
	const std::shared_ptr<ShaderState> GetShaderState() const				{ return m_ShaderState; }

private:
	std::shared_ptr<Material> m_Material;
	std::shared_ptr<Mesh> m_Mesh;
	std::shared_ptr<ShaderState> m_ShaderState;
};

class RenderInfo : public Component {
	friend class Graphics;
	MANAGED_OBJECT(RenderInfo)
	COMPONENT_CONSTRUCTOR(RenderInfo, Component)
public:
	
	const std::shared_ptr<Model> GetModel() const				{ return m_Model; }
	const std::vector<Renderable> & GetRenerables() const		{ return m_Renderables; }
	bool IsVisible() const										{ return m_IsVisible; }

	void SetModel(const std::shared_ptr<Model>& model);
	void OnGui(const char* option = "-1") override;

	void SetMaterial(int index, const std::shared_ptr<Material> & mat) { m_Renderables[index].SetMaterial(mat);}
	void SetShaderState(int index, const std::shared_ptr<ShaderState> & shaderState) { m_Renderables[index].SetShaderState(shaderState); }

	std::shared_ptr<VertexShader> Vshader;
	std::shared_ptr<PixelShader> Pshader;
	std::shared_ptr<Animator> Anim;

private:
	std::shared_ptr<Model> m_Model;
	std::vector<Renderable> m_Renderables;

	bool m_IsVisible;
};
