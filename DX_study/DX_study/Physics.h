#pragma once

struct RaycastResult;
namespace DirectX {
	struct XMFLOAT3;
}

namespace Physics {
	bool Raycast(DirectX::XMFLOAT3 & from, DirectX::XMFLOAT3 & to);
	bool Raycast(DirectX::XMFLOAT3 & from, DirectX::XMFLOAT3 & to, RaycastResult & rayInfo);
}