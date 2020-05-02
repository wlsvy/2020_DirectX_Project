#include "Mesh.h"

Mesh::Mesh(
	const std::vector<DWORD>& indices,
	const DirectX::XMMATRIX & transformMatrix,
	const std::string & name)
	:
	Object(name),
	transformMatrix(transformMatrix)
{
	try {
		ThrowIfFailed(
			this->indexbuffer.Initialize(indices.data(), indices.size())
			, "Failed to initialize index buffer for mesh.");
	}
	catch (CustomException e) {
		ErrorLogger::Log(e);
	}
}