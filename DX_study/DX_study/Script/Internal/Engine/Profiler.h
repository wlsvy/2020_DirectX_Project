#pragma once
#include <vector>
#include <memory>
#include <string>
#include <queue>
#include <chrono>
#include <unordered_map>
#include <array>

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

	UINT CPU_MemoryUsed = 0;
	UINT GPU_MemoryUsed = 0;
	double CPU_Usage = 0.0f;
	unsigned long long CPU_TotalVirtualMemory = 0;
	unsigned long long CPU_VirtualMemoryUsed = 0;
	unsigned long long CPU_TotalPhysicsMemory = 0;
	unsigned long long CPU_PhysicsMemoryUsed = 0;
	unsigned long long CPU_PhysicsMemoryUsedByThis = 0;

	UINT BindingCount_SamplerState = 0;
	UINT BindingCount_ConstantBuffer = 0;
	UINT BindingCount_VertexShader = 0;
	UINT BindingCount_PixelShader = 0;
	UINT BindingCount_GeometryShader = 0;
	UINT BindingCount_ComputeShader = 0;
	UINT BindingCount_ShaderResources = 0;
	UINT BindingCount_UnorderedAccessView = 0;
	UINT BindingCount_RenderTarget = 0;
	UINT BindingCount_VertexBuffer = 0;
	UINT BindingCount_IndexBuffer = 0;

	UINT Call_DrawIndexed = 0;
	UINT Call_Dispatch = 0;

private:
	std::queue<ProfilingSample> m_SampleQueue;
	std::string m_SampleDescription;
	SampleMap m_SampleMap;
	
};

struct ScopedProfilingSample {
	ScopedProfilingSample(const std::string & name) :
		BlockName(name)
	{
		StartTime = std::chrono::high_resolution_clock::now();
	}

	~ScopedProfilingSample() {
		using MicroSec = std::chrono::microseconds;

		auto endTime = std::chrono::high_resolution_clock::now();

		auto du = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - StartTime).count();
		//ong long duration = std::chrono::high_resolution_clock::duration(endTime - StartTime).count() * 0.001; //milisecond
		Profiler::GetInstance().AddSample(BlockName, du);
	}

	const std::string BlockName;
	std::chrono::high_resolution_clock::time_point StartTime;
};