#include "Math.h"
#include "../Internal/Graphics/Model.h"
#include "../Internal/Graphics/Mesh.h"
#include "../Component/Transform.h"
#include "../Component/Renderable.h"


using DirectX::operator+;
using DirectX::operator-;
using DirectX::operator/=;

float Math::GetDistance(const Transform & src, const DirectX::XMFLOAT3 & target)
{
	DirectX::XMVECTOR vec = DirectX::XMLoadFloat3(&target);
	return GetDistance(src, vec);
}

float Math::GetDistance(const Transform & src, const DirectX::XMVECTOR & target)
{
	auto diff = target -src.positionVec;
	return DirectX::XMVector3Length(diff).m128_f32[0];
}

float Math::GetDistance(const Transform & src, const Transform & target)
{
	auto diff = target.positionVec - src.positionVec;
	return DirectX::XMVector3Length(diff).m128_f32[0];
}

float Math::GetDistance(const DirectX::XMVECTOR & src, const DirectX::XMVECTOR & target)
{
	return DirectX::XMVector3Length(target - src).m128_f32[0];
}

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
