#pragma once

#include <memory>
#include <cassert>

class Mesh;
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

	const std::shared_ptr<Material> GetMaterial()							const { return m_Material; }
	const std::shared_ptr<Mesh> GetMesh()									const { return m_Mesh; }
	const std::shared_ptr<ShaderState> GetShaderState()						const { return m_ShaderState; }

private:
	std::shared_ptr<Material> m_Material;
	std::shared_ptr<Mesh> m_Mesh;
	std::shared_ptr<ShaderState> m_ShaderState;
};