#include "GUI.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include <ImGui/imgui_internal.h>

#define ALPHA    ( ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoDragDrop | ImGuiColorEditFlags_PickerHueBar | ImGuiColorEditFlags_NoBorder )
#define NO_ALPHA ( ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoDragDrop | ImGuiColorEditFlags_PickerHueBar | ImGuiColorEditFlags_NoBorder )

void GUI::ReInit(HWND& hWnd)
{
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplOpenGL3_Init();

	p_style = &ImGui::GetStyle();
	p_style->WindowMinSize = ImVec2(800, 500);

	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromMemoryTTF(font_rubik, sizeof(font_rubik), 22.0f);

	static const ImWchar icon_range[]{ 0xf000, 0xf3ff, 0 };
	ImFontConfig icons_config;
	icons_config.MergeMode = true;
	icons_config.PixelSnapH = true;
	icons_config.OversampleH = 3;
	icons_config.OversampleV = 3;

	icons_font = io.Fonts->AddFontFromMemoryCompressedTTF(font_awesome_data, font_awesome_size, 19.5f, &icons_config, icon_range);

	
}

void GUI::Shutdown()
{
	draw = false;
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void GUI::Draw()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	if (draw)
	{
		static bool init{ false };
		if (!init)
		{
			//ImGui::SetNextWindowSize(ImVec2(800, 500), ImGuiCond_FirstUseEver);
			init = true;
		}

		static bool bools[50]{};
		static int ints[50]{}, combo = 0;
		std::vector < const char* > items = { "Option", "Option 1", "Option 2", "Option 3", "Option 4", "Option 5", ICON_FA_AXE_BATTLE " Option 6", "Option 7", "Option 8", "Option 9" };
		static char buf[64];

		static float color[4] = { 1.f, 1.f, 1.f, 1.f };

		PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

		ImGui::Begin("Arsenic", NULL, ImGuiWindowFlags_NoDecoration);
		{
			auto window = GetCurrentWindow();
			auto draw = window->DrawList;
			auto pos = window->Pos;
			auto size = window->Size;
			ImGuiIO& io = GetIO();
			

			gui.m_anim = ImLerp(gui.m_anim, 1.f, 0.045f);

			//GetBackgroundDrawList()->AddImage(bg, ImVec2(0, 0), io.DisplaySize);
			SetCursorPos(ImVec2(30, 20));

			ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
			ImGui::Text("Arsenic");
			SetCursorPos(ImVec2(10, 70));
			BeginChild("##tabs", ImVec2(150, size.y - 120));
			
			Spacing();
			Spacing();
			Spacing();

			gui.group_title("Catagories");
			if (gui.tab(ICON_FA_CROSSHAIRS, "Combat", gui.m_tab == 0) && gui.m_tab != 0)
				gui.m_tab = 0, gui.m_anim = 0.f;

			Spacing();
			Spacing();
			Spacing();

			if (gui.tab(ICON_FA_PENCIL, "Visuals", gui.m_tab == 1) && gui.m_tab != 1)
				gui.m_tab = 1, gui.m_anim = 0.f;

			Spacing();
			Spacing();
			Spacing();

			if (gui.tab(ICON_FA_USER, "Players", gui.m_tab == 2) && gui.m_tab != 2)
				gui.m_tab = 2, gui.m_anim = 0.f;

			Spacing();
			Spacing();
			Spacing();

			if (gui.tab(ICON_FA_LEAF, "World", gui.m_tab == 3) && gui.m_tab != 3)
				gui.m_tab = 3, gui.m_anim = 0.f;

			Spacing();
			Spacing();
			Spacing();

			if (gui.tab(ICON_FA_CODE, "Settings", gui.m_tab == 4) && gui.m_tab != 4)
				gui.m_tab = 4, gui.m_anim = 0.f;

			Spacing(), Spacing(), Spacing();

			EndChild();

			//SetCursorPos(ImVec2(190, 20));
			//Button(ICON_FA_SAVE " Save", ImVec2(100, 25));

			PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

			SetCursorPos(ImVec2(300, 20));
			BeginChild("##subtabs", ImVec2(240, 25));

			GetWindowDrawList()->AddRectFilled(GetWindowPos(), GetWindowPos() + GetWindowSize(), gui.button.to_im_color(), 4);
			GetWindowDrawList()->AddRect(GetWindowPos(), GetWindowPos() + GetWindowSize(), gui.border.to_im_color(), 4);

			for (int i = 0; i < gui.combat_subtabs.size(); ++i) {
				if (gui.subtab(gui.combat_subtabs.at(i), gui.m_combat_subtab == i, gui.combat_subtabs.size(), i == 0 ? ImDrawFlags_RoundCornersLeft : i == gui.combat_subtabs.size() ? ImDrawFlags_RoundCornersRight : 0) && gui.m_combat_subtab != i)
					gui.m_combat_subtab = i, gui.m_anim = 0.f;

				if (i != gui.combat_subtabs.size() - 1)
					SameLine();

			}

			EndChild();

			PopStyleVar();

			PushStyleVar(ImGuiStyleVar_Alpha, gui.m_anim);
			PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 8));

			SetCursorPos(ImVec2(185, 81 - (5 * gui.m_anim)));
			BeginChild("##childs", ImVec2(size.x - 200, size.y - 96));

			switch (gui.m_tab) {

			case 0:

				gui.group_box(ICON_FA_FEATHER " Feather", ImVec2(GetWindowWidth() / 2 - GetStyle().ItemSpacing.x / 2, 210)); {

					Checkbox("Skoro t'ma otstypit " ICON_FA_DAGGER, &bools[0]);
					SliderInt("Slider", &ints[0], 0, 100, "%d%%");
					Combo("Combo", &combo, items.data(), items.size());
					InputText("Input", buf, sizeof buf);
					ColorEdit4("Color", color, ALPHA);
					Button("Button", ImVec2(GetWindowWidth(), 25));

				} gui.end_group_box();

				SameLine();

				gui.group_box(ICON_FA_CROWN " Crown", ImVec2(GetWindowWidth() / 2 - GetStyle().ItemSpacing.x / 2, GetWindowHeight())); {

					for (int i = 1; i < 50; ++i) {
						Checkbox(std::to_string(i).c_str(), &bools[i]);

						if (i != 49)
							Separator();

					}

				} gui.end_group_box();

				break;

			case 1:

				gui.group_box(ICON_FA_BABY " Baby", ImVec2(GetWindowWidth() / 2 - GetStyle().ItemSpacing.x / 2, GetWindowHeight() / 2 - GetStyle().ItemSpacing.y / 2)); {

				} gui.end_group_box();

				gui.group_box(ICON_FA_AD " Ad", ImVec2(GetWindowWidth() / 2 - GetStyle().ItemSpacing.x / 2, GetWindowHeight() / 2 - GetStyle().ItemSpacing.y / 2)); {

				} gui.end_group_box();

				SameLine(), SetCursorPosY(0);

				gui.group_box("Non icon name", ImVec2(GetWindowWidth() / 2 - GetStyle().ItemSpacing.x / 2, GetWindowHeight())); {

				} gui.end_group_box();

				break;

			case 2:

				break;
			}

			EndChild();

			PopStyleVar(2);

		}
		ImGui::End();
	}
	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

//
// Singleton stuff (ignore)
//

GUI* GUI::p_instance;
std::mutex GUI::mutex;

void GUI::Init(HWND& hWnd)
{
	if (p_instance != nullptr)
		return;

	p_instance = new GUI(hWnd);
}

void GUI::Destroy()
{
	if (p_instance == nullptr)
		return;

	p_instance->Shutdown();

	delete p_instance;
	p_instance = nullptr;
}

GUI* GUI::Get()
{
	std::lock_guard<std::mutex> lock(mutex);

	if (p_instance == nullptr)
		return nullptr; // should init but for practical reasons does not

	return p_instance;
}

bool GUI::IsInit()
{
	return p_instance != nullptr;
}