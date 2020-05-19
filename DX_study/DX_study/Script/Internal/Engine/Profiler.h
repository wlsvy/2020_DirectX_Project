#pragma once
#include <vector>
#include <memory>
#include <string>
#include <queue>
#include "../../Util/Math.h"
#include "../../Util/Singleton.h"

struct ScopedProfilingBlock {

};

struct ProfilingSample {
	ProfilingSample(const std::string & name, double timeCost) : SampleName(name), TimeCost(timeCost) {}
	std::string SampleName;
	double TimeCost;
};

class Profiler : public Singleton<Profiler> {
public:
	using UINT = unsigned int;

	void Initialize();
	void Update();
	void AddProfilingInfo(const std::string & name, double timeCost);
	ProfilingSample PopProfilingInfo();

	std::string GPU_Name;

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

	UINT CPU_MemoryUsed = 0;
	UINT GPU_MemoryUsed = 0;

private:
	std::queue<ProfilingSample> m_SampleQueue;
};