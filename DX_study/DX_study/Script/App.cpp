#include "Util/ErrorLogger.h"
#include "Internal/Engine/Engine.h"
#include <memory>

void RunApplication(HINSTANCE& hInstance) 
{
	auto& engine = Engine::Get();
	engine.Initialize(hInstance, "title", "class", 1024, 768);

	engine.Run();

	engine.Release();
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_  HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nCmdShow) 
{
	HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr)) {
		StringHelper::ErrorLog(hr, "Failed to call CoInitialize.");
		return -1;
	}
	RunApplication(hInstance);
	
	int a = 0;

	return 0;
}