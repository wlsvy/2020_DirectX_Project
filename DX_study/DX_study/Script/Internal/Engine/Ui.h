#pragma once
#include <Windows.h>
#include <memory>

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11ShaderResourceView;
class Texture;

namespace GUI {
	void InitImGUI(HWND _hwnd);
	void DrawEditorUI(ID3D11ShaderResourceView * image);
	void DrawDeferredChannelImage();
	void DrawTexture(const std::shared_ptr<Texture> & texture);
	
};