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
	UINT ShaderResourcesBindingCount = 0;
};