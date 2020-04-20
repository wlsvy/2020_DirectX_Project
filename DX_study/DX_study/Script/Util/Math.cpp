#include "Math.h"
#include "../Internal/Graphics/Model.h"
#include "../Internal/Graphics/Mesh.h"
#include "../Component/Transform.h"


using DirectX::operator+;
using DirectX::operator-;
using DirectX::operator/=;

bool Math::CheckFrustumCull(const DirectX::BoundingFrustum & f, const Model & model, const Transform & tf)
{
	for (auto & mesh : model.GetMeshes()) {
		if (f.Contains(Math::GetGlobalBoundingBox(mesh.GetLocalAABB(), tf)) != DirectX::DISJOINT) {
			return false;
		}
	}
	return true;
}

bool Math::CheckFrustumCull(const DirectX::BoundingFrustum & f, const SkinnedModel & model, const Transform & tf)
{
	for (auto & mesh : model.GetMeshes()) {
		if (f.Contains(Math::GetGlobalBoundingBox(mesh.GetLocalAABB(), tf)) != DirectX::DISJOINT) {
			return false;
		}
	}
	return true;
}


DirectX::BoundingBox Math::GetGlobalBoundingBox(const DirectX::BoundingBox & box, const Transform & tf)
{
	return DirectX::BoundingBox(
		DirectX::XMFLOAT3(
			box.Center.x * tf.scale.x + tf.position.x,
			box.Center.y * tf.scale.y + tf.position.y,
			box.Center.z * tf.scale.z + tf.position.z
		),
		DirectX::XMFLOAT3(
			box.Extents.x * tf.scale.x,
			box.Extents.y * tf.scale.y,
			box.Extents.z * tf.scale.z
		)
	);
}
