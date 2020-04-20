#pragma once
#include <Windows.h>

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11ShaderResourceView;

namespace GUI {
	void InitImGUI(HWND _hwnd);
	void DrawEditorUI(ID3D11ShaderResourceView * image);
	void DrawDeferredChannelImage();
};