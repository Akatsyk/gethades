#include "hooks.hpp"
#include "listener_entity.h"
#include "../helpers/math.hpp"
#include "../options.h"
#include "../features/features.h"

void __fastcall hooks::hk_update_client_side_animation(void* ecx)
{
    const auto e = reinterpret_cast<C_BasePlayer*> (ecx);
    const auto index = e->ent_index();

    if (!e || e->is_dormant() || g_unload)
        return;

    if (e->is_enemy())
    {
        float simtimes[64]{ };
        const auto old_cur_time = interfaces::global_vars->curtime;
        const auto old_real_time = interfaces::global_vars->realtime;
        const auto old_frame_time = interfaces::global_vars->frametime;
        const auto old_abs_frame_time = interfaces::global_vars->absoluteframetime;
        const auto old_interp_amount = interfaces::global_vars->interpolation_amount;
        const auto old_tick_count = interfaces::global_vars->tickcount;

        interfaces::global_vars->curtime = e->simulation_time();
        interfaces::global_vars->realtime = e->simulation_time();
        interfaces::global_vars->interpolation_amount = 0;
        interfaces::global_vars->tickcount = time_to_ticks(e->simulation_time());
        e->set_abs_original(e->vec_origin());

        const auto state = e->get_base_player_anim_state();

        g_listener_entity.m_track[index].vmt.get_original< hooks::update_client_side_animation_t >(index::update_client_side_animation) (ecx);

        simtimes[e->ent_index()] = e->simulation_time();

        interfaces::global_vars->curtime = old_cur_time;
        interfaces::global_vars->realtime = old_real_time;
        interfaces::global_vars->frametime = old_frame_time;
        interfaces::global_vars->absoluteframetime = old_abs_frame_time;
        interfaces::global_vars->tickcount = old_tick_count;
        interfaces::global_vars->interpolation_amount = old_interp_amount;
    }
    else
        g_listener_entity.m_track[index].vmt.get_original< hooks::update_client_side_animation_t >(index::update_client_side_animation) (ecx);
}