#include "ErrorLogger.h"
#include "Engine.h"

#include "psapi.h"
#include "TCHAR.h"
#include "pdh.h"

#include <crtdbg.h>
#if _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define malloc(s) _malloc_dbg(s, _NORMAL_BLOCK, __FILE__, __LINE__)
#endif

void haveFun(HINSTANCE hInstance) {
	Engine engine;
	engine.Initialize(hInstance, "DirectX Study", "class", 1280, 768);

	while (engine.ProcessMessage() == true) {
		engine.Update();
		engine.RenderFrame();
	}
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_  HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nCmdShow) 
{
	HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr)) {
		ErrorLogger::Log(hr, "Failed to call CoInitialize.");
		return -1;
	}

	haveFun(hInstance);

#pragma region MEMORY CHECK

	//가상 메모리 체크
	MEMORYSTATUSEX memInfo;
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memInfo);
	DWORDLONG totalVirtualMem = memInfo.ullTotalPageFile;							//전체 사용가능한 메모리
	DWORDLONG virtualMemUsed = memInfo.ullTotalPageFile - memInfo.ullAvailPageFile;	//지금 사용중인 메모리

	//해당 프로세스가 쓰고 있는 메모리 체크
	PROCESS_MEMORY_COUNTERS pmc;
	GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
	//SIZE_T virtualMemUsedByMe = pmc.PrivateUsage;										//프로세스가 사용 중인 가상 메모리
	DWORDLONG totalPhysMem = memInfo.ullTotalPhys / 1000000;							//전체 RAM
	DWORDLONG physMemUsed = (memInfo.ullTotalPhys - memInfo.ullAvailPhys) / 1000000;	//사용 중인 RAM
	SIZE_T physMemUsedByMe = pmc.WorkingSetSize / 1000000;								//프로세스가 쓰고 있는 메모리

	_CrtDumpMemoryLeaks();	//메모리 누수 확인
#pragma endregion

	//system("pause");
	return 0;
}