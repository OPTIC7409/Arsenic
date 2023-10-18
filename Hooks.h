#ifndef HOOKS_H_
#define HOOKS_H_

#include <mutex>
#include <vector>
#include <atomic>

#include <MinHook.h>
#include <Windows.h>
#include <gl/GL.h>

#pragma comment(lib, "opengl32.lib")

typedef BOOL(__stdcall* twglSwapBuffers) (HDC hDc);

inline std::atomic<bool> in_world{ false };

class Hooks final
{
public:
	Hooks(const Hooks&) = delete;
	void operator=(const Hooks&) = delete;

	static void Init();
	static void Destroy();
	static Hooks* Get();
	static bool IsInit();

private:
	Hooks()
	{
		IntallHooks();
	}

	void PushHook(LPVOID pTarget, LPVOID pDetour, LPVOID* ppOriginal);
	void IntallHooks();
	void RemoveHooks();
	// Hooked functions definitions
	static LRESULT __stdcall WndProc(HWND, UINT, WPARAM, LPARAM);
	static bool __stdcall wglSwapBuffers(HDC);

public:
	twglSwapBuffers o_swapbuffers{ nullptr };
	HWND hWnd;
	void(*callback)();

private:
	static Hooks* p_instance;
	static std::mutex mutex;
	void* p_swapbuffers;

	std::vector<void*> hooks;
};

#endif