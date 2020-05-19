#include "Profiler.h"

#include <Windows.h>
#include <Psapi.h>
#include <algorithm>
#include <TCHAR.h>
#include <pdh.h>
#include "AdapterReader.h"
#include "../Core/InternalHelper.h"
#include "../../Util/StringHelper.h"
#include "../../Util/Time.h"

static std::unordered_map<std::string, std::chrono::high_resolution_clock::time_point> s_SampleTimeMap;

static PDH_HQUERY cpuQuery;
static PDH_HCOUNTER cpuTotal;
static ULARGE_INTEGER lastCPU, lastSysCPU, lastUserCPU;
static int numProcessors;
static HANDLE self;

double GetCurrentCpuUsageByProcess() {
	FILETIME ftime, fsys, fuser;
	ULARGE_INTEGER now, sys, user;
	double percent;

	GetSystemTimeAsFileTime(&ftime);
	memcpy(&now, &ftime, sizeof(FILETIME));


	GetProcessTimes(self, &ftime, &ftime, &fsys, &fuser);
	memcpy(&sys, &fsys, sizeof(FILETIME));
	memcpy(&user, &fuser, sizeof(FILETIME));
	percent = (sys.QuadPart - lastSysCPU.QuadPart) + (user.QuadPart - lastUserCPU.QuadPart);
	percent /= (now.QuadPart - lastCPU.QuadPart);
	percent /= numProcessors;
	lastCPU = now;
	lastUserCPU = user;
	lastSysCPU = sys;

	return percent * 100;
}

void Profiler::SampleBegin(const std::string & sampleName)
{
	auto it = s_SampleTimeMap.find(sampleName);
	if (it != s_SampleTimeMap.end()) {
		MessageBoxW(NULL, StringHelper::StringToWide("Already " + sampleName + " Sample begin!").c_str(), L"Error", MB_ICONERROR);
		return;
	}
	s_SampleTimeMap[sampleName] = std::chrono::high_resolution_clock::now();
}

void Profiler::SampleEnd(const std::string & sampleName)
{
	auto it = s_SampleTimeMap.find(sampleName);
	if (it == s_SampleTimeMap.end()) {
		MessageBoxW(NULL, StringHelper::StringToWide("No " + sampleName + " SampleBegin() Called!").c_str(), L"Error", MB_ICONERROR);
		return;
	}

	auto endTime = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - s_SampleTimeMap[sampleName]).count();

	s_SampleTimeMap.erase(it);

	GetInstance().AddSample(sampleName, duration);
}

void Profiler::Initialize()
{
	auto adapter = AdapterReader::GetAdapters();
	auto description = adapter[0].GetDescription();
	
	GPU_Name = std::to_string(description.Description);

	SYSTEM_INFO sysInfo;
	FILETIME ftime, fsys, fuser;
	GetSystemInfo(&sysInfo);
	numProcessors = sysInfo.dwNumberOfProcessors;

	GetSystemTimeAsFileTime(&ftime);
	memcpy(&lastCPU, &ftime, sizeof(FILETIME));


	self = GetCurrentProcess();
	GetProcessTimes(self, &ftime, &ftime, &fsys, &fuser);
	memcpy(&lastSysCPU, &fsys, sizeof(FILETIME));
	memcpy(&lastUserCPU, &fuser, sizeof(FILETIME));
}

void Profiler::Update()
{
	auto adapter = AdapterReader::GetAdapters();
	auto description = adapter[0].GetDescription();

	float MegaInverse = 1.0f / 1024.0f / 1024.0f; //Mega Byte

	GPU_MemoryUsed = description.DedicatedVideoMemory >> 20;

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
			auto pair = m_SampleMap.insert(std::make_pair(sample.Name, std::array<float, MAX_TIME_SAMPLE_COUNT>{ { 0, }}));
			it = pair.first;
		}

		auto & arr = it->second;
		std::rotate(arr.begin(), arr.begin() + 1, arr.end());
		arr[MAX_TIME_SAMPLE_COUNT - 1] = sample.TimeCost;
	}

	if (s_UpdateRemainTime < 0.0f) {
		s_UpdateRemainTime = SAMPLE_UPDATE_TERM;

		UpdateMemoryDescription();
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
	BindingCount_SamplerState = 0;
	BindingCount_ConstantBuffer = 0;
	BindingCount_VertexShader = 0;
	BindingCount_PixelShader = 0;
	BindingCount_GeometryShader = 0;
	BindingCount_ComputeShader = 0;
	BindingCount_ShaderResources = 0;
	BindingCount_UnorderedAccessView = 0;
	BindingCount_RenderTarget = 0;
	BindingCount_VertexBuffer = 0;
	BindingCount_IndexBuffer = 0;
	Call_DrawIndexed = 0;
	Call_Dispatch = 0;

	m_SampleDescription.clear();
}

void Profiler::UpdateMemoryDescription()
{
	MEMORYSTATUSEX memInfo;
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memInfo);
	CPU_TotalVirtualMemory = memInfo.ullTotalPageFile >> 20;
	CPU_VirtualMemoryUsed = (memInfo.ullTotalPageFile - memInfo.ullAvailPageFile) >> 20;

	PROCESS_MEMORY_COUNTERS pmc;
	GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));

	CPU_TotalPhysicsMemory = memInfo.ullTotalPhys >> 20;
	CPU_PhysicsMemoryUsed = (memInfo.ullTotalPhys - memInfo.ullAvailPhys) >> 20;
	CPU_PhysicsMemoryUsedByThis = pmc.WorkingSetSize >> 20;

	CPU_Usage = GetCurrentCpuUsageByProcess();
}
