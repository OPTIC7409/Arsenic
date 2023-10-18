#include <fstream>
#include <thread>
#include <memory>
#include <cmath>

#include <Windows.h>
#include <RscLink.h>

#include "JNI.h"
#include "Link.h"
#include "Hooks.h"
#include "GUI.h"
#include "Exploits.h"

static std::shared_ptr<RscLink> p_rsc_link;
static std::atomic<bool> stop{ false };

void Init();
void Release();

/* Is called from the swapbuffers hook every frame */
void HookCallback()
{
    //if (!JNI::Get()->the_player.get() || JNI::Get()->the_player->GetInstance() == nullptr)
    //    return;
    /*
    int entity_list_size{ JNI::Get()->entity_list->methods["size"]->CallInt() };

    printf("%i entities\n", entity_list_size);
    */
}

/* Real entry point of the cheat */
void MainThread(HINSTANCE instance)
{
    Init();

    if (JNI::Get()->GetEnv() == nullptr)
    {
        Release();
        FreeLibrary(instance);
    }

    JNI::Get()->CreateInstances();

    while (!GetAsyncKeyState(VK_END))
    {
        static bool was_not_in_world{ true };

        if (!GUI::IsInit() || !Hooks::IsInit())
            continue;

        jobject world_client = JNI::Get()->p_mapper->classes["Minecraft"]->fields["theWorld"]->GetValueObject();

        /* Handle not in world */
        if (world_client == nullptr)
        {
            was_not_in_world = true;
            continue;
        }
        else if (was_not_in_world)
        {
            JNI::Get()->p_mapper->classes["WorldClient"]->SetInstance(world_client);
            JNI::Get()->entity_list->SetInstance(JNI::Get()->p_mapper->classes["WorldClient"]->fields["loadedEntityList"]->GetValueObject());
            JNI::Get()->p_mapper->classes["PlayerControllerMP"]->SetInstance(JNI::Get()->p_mapper->classes["Minecraft"]->fields["playerController"]->GetValueObject());
            JNI::Get()->curr_entity->SetInstance(nullptr);
            was_not_in_world = false;
        }

        /* In case the player was not loaded yet */
        if (JNI::Get()->the_player->GetInstance() == nullptr || JNI::Get()->the_player->methods["getHealth"]->CallFloat() == 0.f)
        {
            JNI::Get()->the_player->SetInstance(JNI::Get()->p_mapper->classes["Minecraft"]->fields["thePlayer"]->GetValueObject());
            continue;
        }

        Exploits::InstantRightClick();
        Exploits::AutoClick();

        std::this_thread::sleep_for(std::chrono::milliseconds(25));
    }

    Release();
    FreeLibrary(instance);
}

void Init()
{
    p_rsc_link = std::make_shared<RscLink>((std::uint8_t*)heading, LINK_HEADING_SIZE, (std::uint8_t*)data, LINK_DATA_SIZE);
    JNI::Init(p_rsc_link);

    Hooks::Init();
    Hooks::Get()->callback = HookCallback;
}

/* Manually release things here */
void Release()
{
    Hooks::Destroy();
    JNI::Destroy();
    stop = true;
}

bool __stdcall DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
    static FILE* p_file{ nullptr };
    static std::thread main_thread;

    if (reason == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(instance);

        AllocConsole();
        freopen_s(&p_file, "CONOUT$", "w", stdout);

        main_thread = std::thread([instance] { MainThread(instance); });
        if (main_thread.joinable())
            main_thread.detach();
    }
    else if (reason == DLL_PROCESS_DETACH)
    {
        fclose(p_file);
        FreeConsole();
    }

    return true;
}