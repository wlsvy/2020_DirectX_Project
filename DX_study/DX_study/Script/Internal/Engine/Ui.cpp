#include "Ui.h"
#include <d3d11.h>
#include <ImGui/imgui_impl_dx11.h>
#include <ImGui/imgui_impl_win32.h>
#include <array>
#include <unordered_map>
#include <algorithm>

#include "Engine.h"

#include "../Graphics/Graphics.h"
#include "../Graphics/Texture.h"
#include "../Graphics/Model.h"
#include "../Graphics/Material.h"
#include "../Graphics/AnimationClip.h"
#include "../Graphics/Shaders.h"

#include "../Core/Scene.h"
#include "../Core/InternalHelper.h"
#include "../Core//GameObject.h"
#include "../Engine/Profiler.h"
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

	s_ImGuiIO->Fonts->AddFontFromFileTTF("Data/Fonts/malgun.ttf", 17.0f, NULL, s_ImGuiIO->Fonts->GetGlyphRangesKorean());
}

void GUI::NewFrame()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void GUI::DrawEditorUI(ID3D11ShaderResourceView * image)
{
	static bool s_ShowEditor = true;
	static bool s_ShowStat = false;
	static bool s_ShowResources = false;
	static bool s_ShowImGuiDemo = false;

	ImGuiIO& io = ImGui::GetIO();
	auto& graphics = Core::GetGraphics();

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

	//Menu Bar
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Menu"))
		{
			ImGui::MenuItem("Show Hierarchy", "", &s_ShowEditor);
			ImGui::MenuItem("Show Profiler", "", &s_ShowStat);
			ImGui::MenuItem("Show Resources", "", &s_ShowResources);
			ImGui::MenuItem("Show ImGuiDemo", "", &s_ShowImGuiDemo);

			if (ImGui::MenuItem("Exit", "Alt+F4"))
			{
				Engine::Get().CloseWindow();
			}
			ImGui::EndMenu();
		}
		ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 330);
		ImGui::Text("Delta Time : %.3f, Frame Rate : %.1f FPS", 1000.0f / io.Framerate, io.Framerate);

		ImGui::EndMenuBar();
	}
	//Scene
	{
		ImGui::Spacing();
		ImGui::Image(image, { io.DisplaySize.x, io.DisplaySize.y });

		ImGui::End();
		ImGui::PopStyleVar();
	}
	
	//Editor
	if (s_ShowEditor) {
		ImGui::Begin("Editor", &s_ShowEditor, ImGuiWindowFlags_NoCollapse);
		ImGui::BeginChild("Editor##Hierarchy", ImVec2(io.DisplaySize.x * 0.15f, 0), true);
		ImGui::Text("Hierarchy");
		ImGui::Separator();
		ImGui::Spacing();

		auto& scene = Engine::Get().GetCurrentScene();
		scene.OnGui();

		ImGui::EndChild();
		ImGui::SameLine();

		ImGui::BeginChild("Editor##Inspector", ImVec2(0, 0), true);
		ImGui::Text("Inspector");
		ImGui::Separator();
		ImGui::Spacing();


		if (auto selected = scene.GetGuiSelected().lock())
		{
			selected->GetGameObject()->OnGui();
		}

		ImGui::EndChild();
		ImGui::End();
	}

	//Profiler
	if (s_ShowStat ) {
		ImGui::Begin("Profiler", &s_ShowStat, ImGuiWindowFlags_NoCollapse);
		if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_FittingPolicyDefault_))
		{
			auto & p = Profiler::GetInstance();
			if (ImGui::BeginTabItem("Status", nullptr, ImGuiTabItemFlags_NoCloseWithMiddleMouseButton)) {
				ImGui::Text("Device");
				ImGui::Separator();

				ImGui::Text(
					" GPU Device : %s \
				\n GPU Memory : %u MB\
				\n CPU Usage : %f %% \
				\n CPU TotalVirtualMemory : %llu MB\
				\n CPU VirtualMemoryUsed : %llu MB\
				\n CPU TotalPhysicsMemory : %llu MB\
				\n CPU PhysicsMemoryUsed : %llu MB\
				\n CPU PhysicsMemoryUsedByThis : %llu MB",
					p.GPU_Name.c_str(),
					p.GPU_MemoryUsed,
					p.CPU_Usage,
					p.CPU_TotalVirtualMemory,
					p.CPU_VirtualMemoryUsed,
					p.CPU_TotalPhysicsMemory,
					p.CPU_PhysicsMemoryUsed,
					p.CPU_PhysicsMemoryUsedByThis
				);

				ImGui::Spacing();
				ImGui::Text("Binding Count");
				ImGui::Separator();

				ImGui::Text(
					" SamplerState : %u	\
				\n ConstantBuffer : %u \
				\n VertexShader : %u \
				\n PixelShader : %u \
				\n GeometryShader : %u \
				\n ComputeShader : %u \
				\n ShaderResources : %u \
				\n UnorderedAccessView : %u \
				\n RenderTarget : %u \
				\n VertexBuffer : %u \
				\n IndexBuffer : %u ",
					p.BindingCount_SamplerState,
					p.BindingCount_ConstantBuffer,
					p.BindingCount_VertexShader,
					p.BindingCount_PixelShader,
					p.BindingCount_GeometryShader,
					p.BindingCount_ComputeShader,
					p.BindingCount_ShaderResources,
					p.BindingCount_UnorderedAccessView,
					p.BindingCount_RenderTarget,
					p.BindingCount_VertexBuffer,
					p.BindingCount_IndexBuffer);

				ImGui::Spacing();
				ImGui::Text("Call");
				ImGui::Separator();

				ImGui::Text(
					" DrawCall : %u \
				\n DispatchCall : %u",
					p.Call_DrawIndexed,
					p.Call_Dispatch);
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Performance", nullptr, ImGuiTabItemFlags_NoCloseWithMiddleMouseButton)) {
				for (auto & pair : p.GetSampleMap()) {
					ImGui::PlotLines(pair.first.c_str(), pair.second.data(), Profiler::MAX_TIME_SAMPLE_COUNT);
					ImGui::SameLine();
					ImGui::Text(" %f MicroSec", pair.second[Profiler::MAX_TIME_SAMPLE_COUNT - 1]);
				}
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Render Targets", nullptr, ImGuiTabItemFlags_NoCloseWithMiddleMouseButton)) {
				ImVec2 scene_size = ImVec2(s_ImGuiIO->DisplaySize.x * 0.4f, s_ImGuiIO->DisplaySize.y * 0.4f);
				for (UINT i = 0; i < RenderTargetTypes::Max; i++) {
					ImGui::Image(graphics.GetRenderTargetSrv(i), scene_size);
				}
				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}
		ImGui::End();
	}

	//Resources
	if (s_ShowResources) {
		ImGui::Begin("Resources", &s_ShowResources, ImGuiWindowFlags_NoCollapse);
		if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_FittingPolicyDefault_))
		{
			if (ImGui::BeginTabItem("Model", nullptr, ImGuiTabItemFlags_NoCloseWithMiddleMouseButton)) {
				for (auto & item : Core::Pool<Model>::GetInstance().GetItems()) {
					ImGui::Text(item->Name.c_str());
				}
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Texture", nullptr, ImGuiTabItemFlags_NoCloseWithMiddleMouseButton)) {
				ImVec2 scene_size = ImVec2(s_ImGuiIO->DisplaySize.x * 0.1f, s_ImGuiIO->DisplaySize.y * 0.1f);
				for (auto & item : Core::Pool<Texture>::GetInstance().GetItems()) {
					ImGui::Image(item->GetTextureResourceView(), scene_size);
					ImGui::SameLine();
					ImGui::Text(item->Name.c_str());
				}
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Material", nullptr, ImGuiTabItemFlags_NoCloseWithMiddleMouseButton)) {
				for (auto & item : Core::Pool<SharedMaterial>::GetInstance().GetItems()) {
					ImGui::Text(item->Name.c_str());
				}
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Shader", nullptr, ImGuiTabItemFlags_NoCloseWithMiddleMouseButton)) {
				ImGui::Text("Vertex Shader");
				ImGui::Separator();
				for (auto & item : Core::Pool<VertexShader>::GetInstance().GetItems()) {
					ImGui::Text(item->Name.c_str());
				}
				ImGui::Spacing();
				ImGui::Text("Geometry Shader");
				ImGui::Separator();
				for (auto & item : Core::Pool<GeometryShader>::GetInstance().GetItems()) {
					ImGui::Text(item->Name.c_str());
				}
				ImGui::Spacing();
				ImGui::Text("Pixel Shader");
				ImGui::Separator();
				for (auto & item : Core::Pool<PixelShader>::GetInstance().GetItems()) {
					ImGui::Text(item->Name.c_str());
				}
				ImGui::Spacing();
				ImGui::Text("Compute Shader");
				ImGui::Separator();
				for (auto & item : Core::Pool<ComputeShader>::GetInstance().GetItems()) {
					ImGui::Text(item->Name.c_str());
				}
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Animation Clip", nullptr, ImGuiTabItemFlags_NoCloseWithMiddleMouseButton)) {
				for (auto & item : Core::Pool<AnimationClip>::GetInstance().GetItems()) {
					ImGui::Text(item->Name.c_str());
				}
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
		ImGui::End();
	}

	//ImGuiDemo
	if (s_ShowImGuiDemo) {
		ImGui::ShowDemoWindow(&s_ShowImGuiDemo);
	}
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

void GUI::Render()
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

ImVec2 GUI::GetDisplaySize()
{
	return s_ImGuiIO->DisplaySize;
}
