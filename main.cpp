#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <thread>
#include <chrono>
#include <cstdint>

// Dirección de memoria de GTA SA (Hook en la cámara 0x51565C)
uintptr_t hookAddress = 0x0051565C;
typedef void(__cdecl* CameraUpdate_t)();
CameraUpdate_t TrampolineCamera = nullptr;

// Byte de respaldo para restaurar instrucciones
BYTE originalBytes[5];

void ResetWeaponFlag(uintptr_t weaponData) {
    // Reducido a 25ms / 10ms para C-Bug y Litefoot ultra rápido
    std::this_thread::sleep_for(std::chrono::milliseconds(25));
    *reinterpret_cast<bool*>(weaponData + 0x84) = true;
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    *reinterpret_cast<bool*>(weaponData + 0x84) = false;
}

void CheckAimMode() {
    static uint8_t prevMode = 0;
    
    // Puntero local del jugador y la cámara
    uint32_t ped = *reinterpret_cast<uint32_t*>(0xB6F5F0);
    if (ped) {
        // En GTA SA el modo de cámara actual se obtiene en la estructura de la cámara
        uint8_t currentMode = *reinterpret_cast<uint8_t*>(0xB6F1A8 + 0x59);
        
        if (prevMode == 53 && currentMode != 53) {
            uint32_t weaponData = *reinterpret_cast<uint32_t*>(ped + 0x480);
            if (weaponData) {
                std::thread(ResetWeaponFlag, weaponData).detach();
            }
        }
        prevMode = currentMode;
    }
}

// Subrutina de inyección en ensamblador / hook
void __declspec(naked) CameraHookProxy() {
    __asm {
        pushad
        call CheckAimMode
        popad
        
        // Ejecuta los bytes originales pisados
        push ebx
        push esi
        push edi
        
        // Salta de regreso a la ejecución normal de GTA SA
        mov eax, hookAddress
        add eax, 5
        jmp eax
    }
}

void InstallHook() {
    DWORD oldProtect;
    VirtualProtect((void*)hookAddress, 5, PAGE_EXECUTE_READWRITE, &oldProtect);
    
    // Guardar bytes originales
    memcpy(originalBytes, (void*)hookAddress, 5);
    
    // Escribir JMP (0xE9) hacia nuestro Proxy
    *(BYTE*)hookAddress = 0xE9;
    *(DWORD*)(hookAddress + 1) = ((DWORD)CameraHookProxy - hookAddress - 5);
    
    VirtualProtect((void*)hookAddress, 5, oldProtect, &oldProtect);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);
        InstallHook();
    }
    return TRUE;
}
