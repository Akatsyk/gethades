#include "hooks.hpp"

void __fastcall hooks::hk_check_file_crcs_with_server(void* ecx, void* edx)
{

}

bool __fastcall hooks::hk_loose_file_allowed(void* ecx, void* edx)
{
    return true;
}