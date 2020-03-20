#include "ui.h"
#include "../Engine.h"

void UIspace::InitImGUI(ID3D11Device * _device, ID3D11DeviceContext * _deviceContext, HWND _hwnd)
{
	//Setup ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplWin32_Init(_hwnd);
	ImGui_ImplDX11_Init(_device, _deviceContext);
	//ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();
	ImGui::StyleColorsClassic();
}

void UIspace::EditorUI(ID3D11ShaderResourceView * image)
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
		if (ImGui::BeginMenu("What the??"))
		{
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Viewer Window"))
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
		//cam->composer()->set_render_size({ io.DisplaySize.x, io.DisplaySize.y });
		//cam->set_aspect(io.DisplaySize.x / io.DisplaySize.y);
		//Engine::ref().camera()->set_render_to_screen(false);

		ImGui::End();
		ImGui::PopStyleVar();

		//if (show_statistics) Statistics(&show_statistics, !show_editor);
		//if (show_texture_viewer) TextureViewer(&show_texture_viewer, !show_editor);
		return;
	}

	ImGui::Spacing();

	ImGui::BeginGroup();

	ImGui::BeginChild("Scene##Editor", ImVec2(io.DisplaySize.x * 0.6, io.DisplaySize.y * 0.6), true, ImGuiWindowFlags_NoScrollbar);
	ImGui::Text("Scene");
	ImGui::Separator();
	ImGui::Spacing();

	ImVec2 scene_size = ImVec2(io.DisplaySize.x * 0.6f, io.DisplaySize.y * 0.6f);
	ImGui::Image(image, scene_size);

	ImGui::EndChild();

	ImGui::BeginChild("Project##Editor", ImVec2(io.DisplaySize.x * 0.25f, 0), true);
	ImGui::Text("Project");
	ImGui::Separator();
	ImGui::Spacing();
	ImGui::EndChild();

	ImGui::SameLine();

	ImGui::BeginChild("Console##Editor", ImVec2(io.DisplaySize.x * 0.35f, 0), true);
	ImGui::Text("Console");
	ImGui::Separator();
	ImGui::Spacing();
	//Engine::ref().Log.Draw();
	ImGui::EndChild();

	ImGui::EndGroup();
	ImGui::SameLine();

	ImGui::BeginChild("Hierarchy##Editor", ImVec2(io.DisplaySize.x * 0.15f, io.DisplaySize.y), true);
	ImGui::Text("Hierarchy");
	ImGui::Separator();
	ImGui::Spacing();

	Engine::GetInstance().getSceneManager()->OnGui();

	ImGui::EndChild();
	ImGui::SameLine();

	ImGui::BeginChild("Inspector##Editor", ImVec2(0, 0), true);
	ImGui::Text("Inspector");
	ImGui::Separator();
	ImGui::Spacing();

	GameObject_v2 * selectedObj = Engine::GetInstance().getSceneManager()->getUIselectedObj();
	if (selectedObj != nullptr)
	{
		selectedObj->OnGui();
	}

	ImGui::EndChild();
	ImGui::PopStyleVar();
	ImGui::End();
}
