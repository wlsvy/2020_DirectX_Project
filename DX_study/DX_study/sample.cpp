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

	//���� �޸� üũ
	MEMORYSTATUSEX memInfo;
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memInfo);
	DWORDLONG totalVirtualMem = memInfo.ullTotalPageFile;							//��ü ��밡���� �޸�
	DWORDLONG virtualMemUsed = memInfo.ullTotalPageFile - memInfo.ullAvailPageFile;	//���� ������� �޸�

	//�ش� ���μ����� ���� �ִ� �޸� üũ
	PROCESS_MEMORY_COUNTERS pmc;
	GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
	//SIZE_T virtualMemUsedByMe = pmc.PrivateUsage;										//���μ����� ��� ���� ���� �޸�
	DWORDLONG totalPhysMem = memInfo.ullTotalPhys / 1000000;							//��ü RAM
	DWORDLONG physMemUsed = (memInfo.ullTotalPhys - memInfo.ullAvailPhys) / 1000000;	//��� ���� RAM
	SIZE_T physMemUsedByMe = pmc.WorkingSetSize / 1000000;								//���μ����� ���� �ִ� �޸�

	_CrtDumpMemoryLeaks();	//�޸� ���� Ȯ��
#pragma endregion

	//system("pause");
	return 0;
}