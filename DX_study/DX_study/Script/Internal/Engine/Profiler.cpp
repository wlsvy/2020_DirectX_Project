#include "Profiler.h"

#include <chrono>

#include "AdapterReader.h"
#include "../Core/InternalHelper.h"



void Profiler::Initialize()
{
	auto & adapter = AdapterReader::GetAdapters()[0];
	auto device = Core::GetDevice();

	
	//adapter.description.
	//GPU_Name = device->
}

void Profiler::Update()
{

}

void Profiler::AddProfilingInfo(const std::string & name, double timeCost)
{
	m_SampleQueue.emplace(name, timeCost);
}

ProfilingSample Profiler::PopProfilingInfo()
{
	if (!m_SampleQueue.empty()) {
		auto f = m_SampleQueue.front();
		m_SampleQueue.pop();
		return f;
	}

	return ProfilingSample("", 0.0);
}
