#include "hooks.hpp"
#include <intrin.h>
#include "../options.h"

#include "../helpers/input.hpp"
#include "../helpers/console.h"

void __stdcall hooks::hk_emit_sound1(IRecipientFilter& filter, int iEntIndex, int iChannel, const char* pSoundEntry, unsigned int nSoundEntryHash,
    const char* pSample, float flVolume, int nSeed, float flAttenuation, int iFlags, int iPitch, const Vector* pOrigin,
    const Vector* pDirection, void* pUtlVecOrigins, bool bUpdatePositions, float soundtime, int speakerentity, int unk)
{
    static auto ofunc = sound_hook.get_original< emit_sound1 >(index::emit_sound1);

    if (!strcmp(pSoundEntry, xor_str("UIPanorama.popup_accept_match_beep")))
    {
        static auto set_ready_fn = reinterpret_cast<bool(__stdcall*)(const char*)>(g_utils.pattern_scan(xor_str("client.dll"), xor_str("55 8B EC 83 E4 F8 8B 4D 08 BA ? ? ? ? E8 ? ? ? ? 85 C0 75 12")));

        HWND hwnd;

        if ((hwnd = FindWindow(NULL, "Counter-Strike: Global Offensive")) && GetForegroundWindow() == hwnd)
        {
            RECT lprect;
            GetClientRect(hwnd, &lprect);

            SendMessage(hwnd, WM_MOUSEMOVE, 0, MAKELPARAM(lprect.right / 2, lprect.bottom / 2));
            mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
        }

        if (set_ready_fn)
            set_ready_fn("");
    }

    if (g_options.ragebot_fakelag_enable)
    {
        if (strstr(pSample, xor_str("weapon")) && (strstr(pSample, xor_str("draw")) || strstr(pSample, xor_str("deploy"))))
        {
            static uint32_t prev_hash = 0;
            const uint32_t hash = (uint32_t)pSample;

            if (prev_hash == hash)
                return ofunc(interfaces::engine_sound, filter, iEntIndex, iChannel, pSoundEntry, nSoundEntryHash, pSample, flVolume, nSeed, flAttenuation, iFlags, iPitch,
                    pOrigin, pDirection, pUtlVecOrigins, bUpdatePositions, soundtime, speakerentity, unk);

            prev_hash = hash;
        }
    }

    if (strstr(pSample, xor_str("null")))
        pSample = "";

    ofunc(interfaces::engine_sound, filter, iEntIndex, iChannel, pSoundEntry, nSoundEntryHash, pSample, flVolume, nSeed, flAttenuation, iFlags, iPitch,
        pOrigin, pDirection, pUtlVecOrigins, bUpdatePositions, soundtime, speakerentity, unk);
}