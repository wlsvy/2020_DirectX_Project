#pragma once
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <string>

class Model;
class SkinnedModel;
class Transform;
class Renderable;

namespace Math {

	const float POSITION_MAX = 10000.0f;
	const float POSITION_MIN = -10000.0f;
	const float Deg2Rad = 0.0174533f;	// pi / 180
	const float Rad2Deg = 57.2958f;	// 180 / pi

	bool CheckFrustumCull(const DirectX::BoundingFrustum & f, const Model & model, const Transform & tf);
	bool CheckFrustumCull(const DirectX::BoundingFrustum & f, const SkinnedModel & model, const Transform & tf);
	bool CheckFrustumCull(const DirectX::BoundingFrustum & f, const Renderable & renderable, const Transform & tf);
	DirectX::BoundingBox GetGlobalBoundingBox(const DirectX::BoundingBox & box, const Transform & tf);
}