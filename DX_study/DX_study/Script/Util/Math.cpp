#include "Math.h"
#include "../Internal/Graphics/Model.h"
#include "../Internal/Graphics/Mesh.h"
#include "../Component/Transform.h"
#include "../Component/Renderable.h"


using DirectX::operator+;
using DirectX::operator-;
using DirectX::operator/=;

DirectX::BoundingBox Math::GetGlobalBoundingBox(const DirectX::BoundingBox & box, const Transform & tf)
{
	auto centerVec = DirectX::XMLoadFloat3(&box.Center);
	auto newCenterVec = DirectX::XMVector3Transform(centerVec, tf.GetWorldMatrix());
	DirectX::XMFLOAT3 newCenter;
	DirectX::XMStoreFloat3(&newCenter, newCenterVec);

	auto globalScale = tf.GetLossyScale();

	return DirectX::BoundingBox(
		newCenter,
		DirectX::XMFLOAT3(
			box.Extents.x * globalScale.m128_f32[0],
			box.Extents.y * globalScale.m128_f32[1],
			box.Extents.z * globalScale.m128_f32[2]
		)
	);
}
