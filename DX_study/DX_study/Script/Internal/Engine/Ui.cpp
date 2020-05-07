#include "Ui.h"
#include <d3d11.h>

#include "../Graphics/Graphics.h"
#include "../Graphics/Texture.h"
#include <ImGui/imgui_impl_dx11.h>
#include <ImGui/imgui_impl_win32.h>
//#include "../Graphics/imGui/imgui_impl_win32.h"
//#include "../Graphics/imGui/imgui_impl_dx11.h"
#include "Engine.h"
#include "../Core/Scene.h"
#include "../Core/InternalHelper.h"
#include "../Core//GameObject.h"
#include "../../Component/Transform.h"

ImGuiIO * s_ImGuiIO;

void GUI::InitImGUI(HWND _hwnd)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	s_ImGuiIO = &ImGui::GetIO();
	ImGui_ImplWin32_Init(_hwnd);
	ImGui_ImplDX11_Init(Core::GetDevice(), Core::GetDeviceContext());
	//ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();
	ImGui::StyleColorsClassic();

	s_ImGuiIO->Fonts->AddFontFromFileTTF("Data/Fonts/malgun.ttf", 15.0f, NULL, s_ImGuiIO->Fonts->GetGlyphRangesKorean());
}

void GUI::DrawEditorUI(ID3D11ShaderResourceView * image)
{
	static bool show_editor = true;
	static bool show_statistics = false;
	static bool show_texture_viewer = false;

	ImGuiIO& io = ImGui::GetIO();

	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(io.DisplaySize);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	if (!ImGui::Begin("MAIN UI", 0, ImGuiWindowFlags_MenuBar
		| ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoScrollbar
		| ImGuiWindowFlags_NoScrollWithMouse
		| ImGuiWindowFlags_NoCollapse
		| ImGuiWindowFlags_AlwaysAutoResize
		| ImGuiWindowFlags_NoBringToFrontOnFocus)) {
		ImGui::End();
		ImGui::PopStyleVar();
		return;
	}

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Menu"))
		{
			ImGui::MenuItem("Show Statistics", "", &show_statistics);
			ImGui::MenuItem("Show Editor", "", &show_editor);
			ImGui::MenuItem("Show Texture Viewer", "", &show_texture_viewer);

			if (ImGui::MenuItem("Exit", "Alt+F4"))
			{
				//Engine::ref().window()->forceExit();
			}
			ImGui::EndMenu();
		}
		ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 330);
		ImGui::Text("Delta Time : %.3f, Frame Rate : %.1f FPS", 1000.0f / io.Framerate, io.Framerate);

		ImGui::EndMenuBar();
	}

	if (!show_editor)
	{
		ImGui::Image(image, { io.DisplaySize.x, io.DisplaySize.y });

		ImGui::End();
		ImGui::PopStyleVar();

		return;
	}

	ImGui::Spacing();

	ImGui::BeginGroup();

	ImGui::BeginChild("Scene##Editor", ImVec2(io.DisplaySize.x * 0.5, io.DisplaySize.y * 0.5), true, ImGuiWindowFlags_NoScrollbar);
	ImGui::Text("Scene");
	ImGui::Separator();
	ImGui::Spacing();

	ImVec2 scene_size = ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f);
	ImGui::Image(image, scene_size);

	ImGui::EndChild();

	ImGui::BeginChild("Project##Editor", ImVec2(io.DisplaySize.x * 0.25f, 0), true);
	ImGui::Text("Project");
	ImGui::Separator();
	ImGui::Spacing();
	ImGui::EndChild();

	ImGui::SameLine();

	ImGui::BeginChild("Console##Editor", ImVec2(io.DisplaySize.x * 0.25f, 0), true);
	ImGui::Text("Console");
	ImGui::Separator();
	ImGui::Spacing();
	ImGui::EndChild();

	ImGui::EndGroup();
	ImGui::SameLine();

	ImGui::BeginChild("Hierarchy##Editor", ImVec2(io.DisplaySize.x * 0.15f, 0), true);
	ImGui::Text("Hierarchy");
	ImGui::Separator();
	ImGui::Spacing();

	auto& scene = Engine::Get().GetCurrentScene();
	scene.OnGui();

	ImGui::EndChild();
	ImGui::SameLine();

	ImGui::BeginChild("Inspector##Editor", ImVec2(0, 0), true);
	ImGui::Text("Inspector");
	ImGui::Separator();
	ImGui::Spacing();

	
	if (auto selected = scene.GetGuiSelected().lock())
	{
		selected->GetGameObject()->OnGui();
	}

	ImGui::EndChild();
	ImGui::PopStyleVar();
	ImGui::End();
}

void GUI::DrawDeferredChannelImage()
{
	auto& graphics = Core::GetGraphics();

	ImGui::Begin("Deferred Rendering Debug");
	ImVec2 scene_size = ImVec2(s_ImGuiIO->DisplaySize.x * 0.2f, s_ImGuiIO->DisplaySize.y * 0.2f);
	ImGui::Image(graphics.GetRenderTargetSrv(0), scene_size);
	ImGui::Image(graphics.GetRenderTargetSrv(1), scene_size);
	ImGui::Image(graphics.GetRenderTargetSrv(2), scene_size);
	ImGui::Image(graphics.GetRenderTargetSrv(3), scene_size);
	ImGui::Image(graphics.GetRenderTargetSrv(4), scene_size);
	ImGui::Image(graphics.GetRenderTargetSrv(5), scene_size);

	ImGui::End();
}

void GUI::DrawTexture(const std::shared_ptr<Texture>& texture)
{
	ImGui::Text(texture->Name.c_str());
	ImVec2 scene_size = ImVec2(s_ImGuiIO->DisplaySize.x * 0.15f, s_ImGuiIO->DisplaySize.y * 0.15f);
	ImGui::Image(texture->GetTextureResourceView(), scene_size);
}

ImVec2 GUI::GetDisplaySize()
{
	return s_ImGuiIO->DisplaySize;
}
