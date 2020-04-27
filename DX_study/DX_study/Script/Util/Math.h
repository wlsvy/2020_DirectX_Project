#pragma once
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <string>
#include <vector>
#include "../Internal/Graphics/Vertex.h"

class Model;
class SkinnedModel;
class Transform;
class RenderInfo;

namespace Math {
	using DWORD = unsigned long;

	const float POSITION_MAX = 10000.0f;
	const float POSITION_MIN = -10000.0f;
	const float Deg2Rad = 0.0174533f;	// pi / 180
	const float Rad2Deg = 57.2958f;	// 180 / pi

	float GetDistance(const Transform& src, const DirectX::XMFLOAT3 & target);
	float GetDistance(const Transform& src, const DirectX::XMVECTOR & target);
	float GetDistance(const Transform& src, const Transform & target);
	float GetDistance(const DirectX::XMVECTOR & src, const DirectX::XMVECTOR & target);
	DirectX::BoundingBox GetGlobalBoundingBox(const DirectX::BoundingBox & box, const Transform & tf);

	template<typename VertexTy>
	void ComputeVertexTangent(std::vector<VertexTy> & vertices, std::vector<DWORD> & indices);
	template<typename VertexTy>
	void ComputeVertexNormal(std::vector<VertexTy> & vertices, std::vector<DWORD> & indices);
	
}