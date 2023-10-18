#ifndef GUI_H_
#define GUI_H_

#include <memory>
#include <thread>
#include <chrono>
#include <mutex>

#include <Windows.h>
#include <ImGui/imgui.h>
#include <ImGui/backends/imgui_impl_win32.h>
#include <ImGui/backends/imgui_impl_opengl3.h>
#include "fonts.hpp"
#include "font_awesome.h"
#include "font_awesome.cpp"
#include "newgui.hpp"

class GUI final
{
public:
	GUI(const GUI&) = delete;
	void operator=(const GUI&) = delete;

	static void Init(HWND& hWnd);
	static void Destroy();
	static GUI* Get();
	static bool IsInit();

	void Draw();
	void ReInit(HWND& hWnd);
	void Shutdown();

private:

	GUI(HWND& hWnd)
	{
		ReInit(hWnd);
	}

public:
	bool draw = true;

	bool no_right_click_delay{ false };
	bool autoclick{ false };
	int autoclick_delay{ 200 };
	int max_autoclick_delay_randomizer{ 20 };
	bool aimbot{ false };

	ImFont* icons_font = nullptr;
	ImGuiStyle* p_style;

	ImColor winBg = ImColor(20, 20, 20);

	ImColor primary_color = ImColor(112, 145, 245);
	ImColor primary_color_hovered = ImColor(112, 145, 229);

	ImColor bg = ImColor(30, 30, 30);
	ImColor bg_hovered = ImColor(40, 40, 40);

	ImColor* primary_text_color = new ImColor(255, 255, 255);
	ImColor* secondary_text_color = new ImColor(200, 200, 200);
	
	

private:
	static GUI* p_instance;
	static std::mutex mutex;
};

#endif