#include "Profiler.h"

#include <algorithm>
#include "AdapterReader.h"
#include "../Core/InternalHelper.h"
#include "../../Util/StringHelper.h"
#include "../../Util/Time.h"

void Profiler::Initialize()
{
	auto adapter = AdapterReader::GetAdapters();
	auto description = adapter[0].GetDescription();

	auto d = Core::GetDevice();
	auto dc = Core::GetDeviceContext();
	
	GPU_Name = std::to_string(description.Description);
}

void Profiler::Update()
{
	auto adapter = AdapterReader::GetAdapters();
	auto description = adapter[0].GetDescription();

	GPU_MemoryUsed = description.DedicatedSystemMemory / 1024 / 1024; //Mega Byte

	static const float SAMPLE_UPDATE_TERM = 1.0f;
	static float s_UpdateRemainTime = SAMPLE_UPDATE_TERM;
	s_UpdateRemainTime -= Time::GetDeltaTime();

	while (!m_SampleQueue.empty()) {
		auto sample = m_SampleQueue.front();
		m_SampleQueue.pop();

		if (s_UpdateRemainTime > 0.0f) {
			continue;
		}

		auto it = m_SampleMap.find(sample.Name);
		if (it == m_SampleMap.end()) {
			auto pair = m_SampleMap.insert(std::make_pair(sample.Name, std::array<float, MAX_TIME_SAMPLE_COUNT>{ { 0.0, }}));
			it = pair.first;
		}

		auto & arr = it->second;
		std::rotate(arr.begin(), arr.begin() + 1, arr.end());
		arr[MAX_TIME_SAMPLE_COUNT - 1] = sample.TimeCost;
	}

	if (s_UpdateRemainTime < 0.0f) {
		s_UpdateRemainTime = SAMPLE_UPDATE_TERM;
	}
}

void Profiler::AddSample(const std::string & name, float timeCost)
{
	m_SampleQueue.emplace(name, timeCost);
}

ProfilingSample Profiler::PopSample()
{
	if (!m_SampleQueue.empty()) {
		auto f = m_SampleQueue.front();
		m_SampleQueue.pop();
		return f;
	}

	return ProfilingSample("", 0.0);
}

void Profiler::Clear()
{
	SamplerStateBindingCount = 0;
	ConstantBufferBindingCount = 0;
	VertexShaderBindingCount = 0;
	PixelShaderBindingCount = 0;
	GeometryShaderBindingCount = 0;
	ComputeShaderBindingCount = 0;
	ShaderResourcesBindingCount = 0;
	UnorderedAccessViewBindingCount = 0;
	RenderTargetBindingCount = 0;
	VertexBufferBindingCount = 0;
	IndexBufferBindingCount = 0;
	DrawCallCount = 0;
	DispatchCallCount = 0;

	CPU_MemoryUsed = 0;
	GPU_MemoryUsed = 0;
	m_SampleDescription.clear();
}
