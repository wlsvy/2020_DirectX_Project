#include "Mesh.h"

Mesh::Mesh(
	const std::vector<DWORD>& indices,
	const DirectX::XMMATRIX & worldMatrix,
	const std::string & name)
	:
	Object(name),
	m_WorldMatrix(worldMatrix)
{
	try {
		ThrowIfFailed(
			m_IndexBuffer.Initialize(indices.data(), (UINT)indices.size())
			, "Failed to initialize index buffer for mesh.");
	}
	catch (CustomException e) {
		StringHelper::ErrorLog(e);
	}
}

void Mesh::OnGui(const char* option)
{
	ImGui::Text(("Mesh : " + Name).c_str());
}
