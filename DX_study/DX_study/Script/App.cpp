#include "Util/ErrorLogger.h"
#include "Internal/Engine/Engine.h"
#include "Util/Time.h"
#include <memory>

void RunApplication(HINSTANCE& hInstance) {
	static float val = 0.0f;

	Engine engine;
	engine.Initialize(hInstance, "title", "class", 1024, 768);
	while (engine.ProcessMessage()) {
		val += Time::GetDeltaTime();
		if (val > Engine::s_FixedFrameRate) {
			engine.FixedUpdate();
			val -= Engine::s_FixedFrameRate;
		}

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