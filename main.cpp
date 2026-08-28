#include <windows.h>
#include <thread>
#include <chrono>
#include "lemon/hook.hpp"

[[maybe_unused]] class loader {
public:
    loader() {
        test.on_before += [](lemon::hook_cpu &cpu) {
            auto old_mode = *reinterpret_cast<std::uint8_t *>(cpu.EAX + cpu.ESI + 0x180);
            auto mode     = cpu.BP;

            if (old_mode == 53 && mode != 53) {
                auto ped = *reinterpret_cast<std::uint32_t *>(0xB6F5F0);
                if (!ped) return;

                auto weapon_data = *reinterpret_cast<std::uint32_t *>(ped + 0x480);
                if (!weapon_data) return;

                std::thread([weapon_data] {
                    std::this_thread::sleep_for(std::chrono::milliseconds(25));
                    *reinterpret_cast<bool *>(weapon_data + 0x84) = true;
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    *reinterpret_cast<bool *>(weapon_data + 0x84) = false;
                }).detach();
            }
        };
        test.install();
    }
private:
    lemon::hook<> test{ 0x0051565C };
} g_loader;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    return TRUE;
}
