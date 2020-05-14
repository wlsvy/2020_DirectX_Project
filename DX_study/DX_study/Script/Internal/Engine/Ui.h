#pragma once
#include <Windows.h>
#include <memory>
#include <ImGui/imgui.h>

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11ShaderResourceView;
class Texture;
struct ImVec;

namespace GUI {
	void InitImGUI(HWND _hwnd);
	void NewFrame();
	void DrawEditorUI(ID3D11ShaderResourceView * image);
	void DrawDeferredChannelImage();
	void DrawTexture(const std::shared_ptr<Texture> & texture);
	void Render();
	ImVec2 GetDisplaySize();
};