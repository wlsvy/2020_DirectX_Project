#include "Material.h"
#include "../Core/ObjectPool.h"
#include "imGui/imgui.h"
#include "Shaders.h"
#include "Texture.h"

std::shared_ptr<SharedMaterial> SharedMaterial::GetDefault() {
	return Core::Find<SharedMaterial>("Default");
}

void Material::OnGui()
{
	//DirectX::XMFLOAT4 col = Color;
	ImGui::Text(("Material : " + Name).c_str());
	if (Vshader) {
		ImGui::Text(("V shader : \n" + Vshader->Name).c_str());
	}
	if (Pshader) {
		ImGui::Text(("P shader : \n" + Pshader->Name).c_str());
	}
	if (MainTexture) {
		ImGui::Text(("main Texture : " + MainTexture->Name).c_str());
	}
	if (NormalMap) {
		ImGui::Text(("normal Map : \n" + NormalMap->Name).c_str());
	}
	for (auto & sub : SubTextures) {
		ImGui::Text(("sub Texture : " + sub->Name).c_str());
	}
	//ImGui::ColorEdit4("Color", &Color.x);	//Color Edit이 동시에 두 개가 활성화 되면 문제발생 주의할 것.

}