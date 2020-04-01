#include "Util/ErrorLogger.h"
#include "Internal/Engine/Engine.h"
#include <memory>

void RunApplication(HINSTANCE& hInstance) {
	Engine engine;
	engine.Initialize(hInstance, "title", "class", 800, 600);
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
	RunApplication(hInstance);
	
	return 0;
}