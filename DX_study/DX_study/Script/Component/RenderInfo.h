#pragma once
#include <vector>
#include <cassert>
#include "Component.h"
#include "../Internal/Graphics/Renderable.h"

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



class RenderInfo : public Component {
	friend class Graphics;
	MANAGED_OBJECT(RenderInfo)
	COMPONENT_CONSTRUCTOR(RenderInfo, Component)
public:
	
	const std::shared_ptr<Model> GetModel() const				{ return m_Model; }
	const std::vector<Renderable> & GetRenerables() const		{ return m_Renderables; }
	bool IsVisible() const										{ return m_IsVisible; }
	size_t GetRenderableSize() const							{ return m_Renderables.size();}

	void SetModel(const std::shared_ptr<Model>& model);
	void OnGui(const char* option = "op") override;

	void SetMaterial(int index, const std::shared_ptr<Material> & mat) { m_Renderables[index].SetMaterial(mat);}
	void SetShaderState(int index, const std::shared_ptr<ShaderState> & shaderState) { m_Renderables[index].SetShaderState(shaderState); }

	std::shared_ptr<Animator> Anim;

private:
	std::shared_ptr<Model> m_Model;
	std::vector<Renderable> m_Renderables;

	bool m_IsVisible;
};
