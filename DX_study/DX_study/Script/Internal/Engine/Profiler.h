#pragma once
#include <vector>
#include <memory>
#include <string>
#include <queue>
#include <chrono>
#include <unordered_map>
#include <array>
#include <time.h>

#include "../../Util/Math.h"
#include "../../Util/Singleton.h"

struct ProfilingSample {
	ProfilingSample(const std::string & name, float timeCost) : Name(name), TimeCost(timeCost) {}
	const std::string Name;
	const float TimeCost;
};

class Profiler : public Singleton<Profiler> {
public:
	using UINT = unsigned int;
	static const UINT MAX_TIME_SAMPLE_COUNT = 10;

	using SampleMap = std::unordered_map<std::string, std::array<float, MAX_TIME_SAMPLE_COUNT>>;

	void Initialize();
	void Update();
	void AddSample(const std::string & name, float timeCost);
	ProfilingSample PopSample();
	bool IsSampleEmpty() { return m_SampleQueue.empty(); }
	void Clear();

	const SampleMap GetSampleMap() const { return m_SampleMap; }

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
	std::string m_SampleDescription;
	SampleMap m_SampleMap;
	
};

struct ScopedProfilingSample {
	ScopedProfilingSample(const std::string & name) :
		BlockName(name),
		StartTime(std::chrono::high_resolution_clock::now()) {}

	~ScopedProfilingSample() {
		auto endTime = std::chrono::high_resolution_clock::now();

		float duration = std::chrono::high_resolution_clock::duration(endTime - StartTime).count() * 0.001; //milisecond
		Profiler::GetInstance().AddSample(BlockName, duration);
	}

	const std::string BlockName;
	const std::chrono::high_resolution_clock::time_point StartTime;
};