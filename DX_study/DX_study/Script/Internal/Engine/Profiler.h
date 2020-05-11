#pragma once
#include <vector>
#include <memory>
#include <string>
#include "../../Util/Math.h"
#include "../../Util/Singleton.h"

class Profiler : public Singleton<Profiler> {
public:

	using UINT = unsigned int;

	UINT SamplerStateBindingCount = 0;
	UINT ConstantBufferBindingCount = 0;
	UINT VertexShaderBindingCount = 0;
	UINT PixelShaderBindingCount = 0;
	UINT GeometryShaderBindingCount = 0;
	UINT ComputeShaderBindingCount = 0;
	UINT ShaderResourcesBindingCount = 0;
	UINT UnorderedAccessViewBindingCount = 0;
	UINT RenderTargetBindingCount = 0;
	UINT VertexBufferBindingCount = 0;
	UINT IndexBufferBindingCount = 0;
	UINT DrawCallCount = 0;
	UINT DispatchCallCount = 0;
};