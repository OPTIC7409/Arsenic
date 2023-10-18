#include "Hooks.h"
#include "GUI.h"

static WNDPROC oWndProc{ nullptr };

void Hooks::PushHook(LPVOID pTarget, LPVOID pDetour, LPVOID* ppOriginal)
{
	hooks.push_back(pTarget);
	MH_CreateHook(pTarget, pDetour, ppOriginal);
}

void Hooks::IntallHooks()
{
	MH_Initialize();

	// To get window handle
	hWnd = FindWindowA("LWJGL", nullptr); // older versions
	if (!hWnd)
		hWnd = FindWindowA("GLFW30", nullptr); // more recent versions of mc

	if (!hWnd)
	{
		printf("[-] Failed to get window handle\n");
		return;
	}

	oWndProc = (WNDPROC)SetWindowLongPtrW(hWnd, GWLP_WNDPROC, (LONG_PTR)WndProc);

	p_swapbuffers = GetProcAddress(GetModuleHandleA("opengl32.dll"), "wglSwapBuffers");

	if (p_swapbuffers != nullptr)
		PushHook(p_swapbuffers, &wglSwapBuffers, (LPVOID*)&o_swapbuffers);
	else
	{
		printf("[-] Failed to get wglSwapBuffers addr\n");
		return;
	}

	MH_EnableHook(MH_ALL_HOOKS);
}

void Hooks::RemoveHooks()
{
	SetWindowLongPtrW(hWnd, GWLP_WNDPROC, (LONG_PTR)oWndProc);

	MH_DisableHook(MH_ALL_HOOKS);

	GUI::Destroy();

	MH_RemoveHook(p_swapbuffers);
}

//
// Hooked functions
//

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT __stdcall Hooks::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (GUI::Get())
	{
		// Change menu state
		if (msg == WM_KEYDOWN && wParam == VK_INSERT)
			GUI::Get()->draw = !GUI::Get()->draw;

		if (GUI::Get()->draw && ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
			return true;
	}

	return CallWindowProcA(oWndProc, hWnd, msg, wParam, lParam);
}

bool __stdcall Hooks::wglSwapBuffers(HDC hdc)
{
	static HGLRC o_context = wglGetCurrentContext();
	static HGLRC new_context{};
	static GLint last_viewport[4];

	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	static bool init = false;
	if (!init || viewport[2] != last_viewport[2] || viewport[3] != last_viewport[3])
	{
		if (new_context)
		{
			wglMakeCurrent(hdc, o_context);
			GUI::Get()->Shutdown();
			wglDeleteContext(new_context);
		}

		new_context = wglCreateContext(hdc);
		wglMakeCurrent(hdc, new_context);

		glViewport(0, 0, viewport[2], viewport[3]);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, viewport[2], viewport[3], 0, -1, 1);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glDisable(GL_DEPTH_TEST);

		if (!GUI::IsInit())
			GUI::Init(Hooks::Get()->hWnd);
		else
			GUI::Get()->ReInit(Hooks::Get()->hWnd);

		memcpy(last_viewport, viewport, sizeof(GLint) * 4);
		init = true;
	}
	else
	{
		wglMakeCurrent(hdc, new_context);
		GUI::Get()->Draw();
	}

	if (Hooks::Get()->callback != nullptr && GUI::IsInit())
		Hooks::Get()->callback();

	wglMakeCurrent(hdc, o_context);

	return Hooks::Get()->o_swapbuffers(hdc);
}

//
// Singleton stuff (ignore)
//

Hooks* Hooks::p_instance;
std::mutex Hooks::mutex;

void Hooks::Init()
{
	if (p_instance != nullptr)
		return;

	p_instance = new Hooks();
}

void Hooks::Destroy()
{
	if (p_instance == nullptr)
		return;

	p_instance->RemoveHooks();

	delete p_instance;
	p_instance = nullptr;
}

Hooks* Hooks::Get()
{
	std::lock_guard<std::mutex> lock(mutex);

	if (p_instance == nullptr)
		return nullptr; // should init but for practical reasons does not

	return p_instance;
}

bool Hooks::IsInit()
{
	return p_instance != nullptr;
}