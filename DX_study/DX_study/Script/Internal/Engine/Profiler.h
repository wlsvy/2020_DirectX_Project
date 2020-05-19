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

using UINT = unsigned int;
using DWORDLONG = unsigned long long;

struct ProfilingSample {
	ProfilingSample(const std::string & name, float timeCost) : Name(name), TimeCost(timeCost) {}
	const std::string Name;
	const float TimeCost;
};

class Profiler : public Singleton<Profiler> {
public:
	static const UINT MAX_TIME_SAMPLE_COUNT = 10;
	using SampleMap = std::unordered_map<std::string, std::array<float, MAX_TIME_SAMPLE_COUNT>>;

	static void SampleBegin(const std::string & sampleName);
	static void SampleEnd(const std::string & sampleName);

	void Initialize();
	void Update();
	void UpdateMemoryDescription();

	void AddSample(const std::string & name, float timeCost);
	ProfilingSample PopSample();
	bool IsSampleEmpty() { return m_SampleQueue.empty(); }
	void Clear();

	const SampleMap GetSampleMap() const { return m_SampleMap; }

	std::string GPU_Name;

	UINT CPU_MemoryUsed = 0;
	UINT GPU_MemoryUsed = 0;
	double CPU_Usage = 0.0f;
	DWORDLONG CPU_TotalVirtualMemory = 0;
	DWORDLONG CPU_VirtualMemoryUsed = 0;
	DWORDLONG CPU_TotalPhysicsMemory = 0;
	DWORDLONG CPU_PhysicsMemoryUsed = 0;
	DWORDLONG CPU_PhysicsMemoryUsedByThis = 0;

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

/*
	사용 불가능
	이유는 모르겠지만, 소멸자에서 정상적인 현재 시간 값을 구할 수 없음
*/
struct ScopedProfilingSample {
	ScopedProfilingSample(const std::string & name) : SampleName(name)
	{
		Profiler::SampleBegin(SampleName);
	}

	~ScopedProfilingSample() 
	{
		Profiler::SampleEnd(SampleName);
	}

	const std::string SampleName;
};