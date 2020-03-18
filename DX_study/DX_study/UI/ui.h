#pragma once
#include <windows.h>
#include <d3d11.h>
#include "imGui/imgui.h"
#include "imGui/imgui_impl_win32.h"
#include "imGui/imgui_impl_dx11.h"

class Engine;

struct UI_EDITOR_DESC {
	ID3D11ShaderResourceView * _image;

};

namespace UIspace {
	void InitImGUI(ID3D11Device * _device, ID3D11DeviceContext * _deviceContext, HWND _hwnd);
	void EditorUI(ID3D11ShaderResourceView * _image, Engine * _engine);
}