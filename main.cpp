#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <thread>
#include <chrono>
#include <cstdint>

// Dirección exacta donde se intercepta el cambio de modo de cámara
constexpr uintptr_t HOOK_ADDR = 0x0051565C;
constexpr uintptr_t RETURN_ADDR = 0x00515661;

void ResetWeaponFlag(uintptr_t weaponData) {
    std::this_thread::sleep_for(std::chrono::milliseconds(25));
    *reinterpret_cast<bool*>(weaponData + 0x84) = true;
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    *reinterpret_cast<bool*>(weaponData + 0x84) = false;
}

// Procesa los registros pasados desde el desensamblado del motor del juego
void __cdecl ProcessCameraMode(uint8_t oldMode, uint8_t newMode) {
    if (oldMode == 53 && newMode != 53) {
        uint32_t ped = *reinterpret_cast<uint32_t*>(0xB6F5F0);
        if (ped) {
            uint32_t weaponData = *reinterpret_cast<uint32_t*>(ped + 0x480);
            if (weaponData) {
                std::thread(ResetWeaponFlag, weaponData).detach();
            }
        }
    }
}

// Subrutina naked: captura EAX+ESI+0x180 (oldMode) y BP (newMode) sin corromper el stack
void __declspec(naked) CameraHookProxy() {
    __asm {
        pushad
        
        // Extrae oldMode de (EAX + ESI + 0x180) y newMode de BP (EBP)
        movzx eax, byte ptr [eax + esi + 0x180]
        movzx ebx, bp
        
        push ebx        // newMode
        push eax        // oldMode
        call ProcessCameraMode
        add esp, 8      // Limpia los argumentos del stack
        
        popad

        // Ejecuta las instrucciones originales que fueron sobrescritas: push ebx; push esi; push edi
        push ebx
        push esi
        push edi

        // Regresa a la ejecución normal del juego
        jmp RETURN_ADDR
    }
}

void InstallHook() {
    DWORD oldProtect;
    VirtualProtect(reinterpret_cast<void*>(HOOK_ADDR), 5, PAGE_EXECUTE_READWRITE, &oldProtect);

    // Escribe el salto (JMP) a nuestra subrutina
    *reinterpret_cast<uint8_t*>(HOOK_ADDR) = 0xE9;
    *reinterpret_cast<uint32_t*>(HOOK_ADDR + 1) = reinterpret_cast<uint32_t>(CameraHookProxy) - HOOK_ADDR - 5;

    VirtualProtect(reinterpret_cast<void*>(HOOK_ADDR), 5, oldProtect, &oldProtect);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);
        InstallHook();
    }
    return TRUE;
}
