#include "hooks.hpp"
#include "listener_entity.h"
#include "..\features\features.h"
#include "../helpers/console.h"

int __fastcall hooks::hk_calc_view(void* ecx, double st0, int Angle, int a3, int a4, int a5, float* a6)
{
	const auto player = reinterpret_cast<C_BasePlayer*> (ecx);

	if (!player || !player->is_local())
		return g_listener_entity.m_track[player->ent_index()].vmt.get_original< hooks::calc_view_t >(index::calc_view) (ecx, st0, Angle, a3, a4, a5, a6);

	auto new_animstate = player->get_offset<bool>(0x3AB4);

	player->set_offset<int>(0x3AB4, false);

	g_listener_entity.m_track[player->ent_index()].vmt.get_original< hooks::calc_view_t >(index::calc_view) (ecx, st0, Angle, a3, a4, a5, a6);

	player->set_offset<int>(0x3AB4, new_animstate);
}

void __fastcall hooks::hk_do_extra_bones_processing(void* ecx, void*, CStudioHdr* hdr, Vector* pos, quaternion_t* q, matrix3x4_t* matrix, void* bone_list, void* context)
{
	const auto player = reinterpret_cast<C_BasePlayer*> (ecx);

	c_base_player_anim_state* animstate = player->get_base_player_anim_state();
	C_BasePlayer* backup{ nullptr };

	if (animstate)
	{
		backup = animstate->m_pBaseEntity;
		animstate->m_pBaseEntity = nullptr;
	}

	g_listener_entity.m_track[player->ent_index()].vmt.get_original< hooks::do_extra_bones_processing_t >
		(index::do_extra_bones_processing) (ecx, hdr, pos, q, matrix, bone_list, context);

	if (animstate && backup)
		animstate->m_pBaseEntity = backup;

	return;
}