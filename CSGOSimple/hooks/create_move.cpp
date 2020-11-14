#include "hooks.hpp"
#include "../options.h"
#include "../helpers/math.hpp"
#include "../helpers/console.h"

#include "../menu/menu.hpp"
#include "../features/features.h"
#include <mutex>

void patch(PVOID address, const int type, const int bytes)
{
	DWORD d, ds; //declared for future use.
	VirtualProtect(address, bytes, PAGE_EXECUTE_READWRITE, &d); //remove write protection!
	memset(address, type, bytes); //patch the data
	VirtualProtect(address, bytes, d, &ds); //set the write protection back to its normal state
}

void setMoveChokeClampLimit() {

	auto clMoveChokeClamp = g_utils.pattern_scan(xor_str("engine.dll"), xor_str("B8 ? ? ? ? 3B F0 0F 4F F0 89 5D FC")) + 1;

	unsigned long protect = 0;

	VirtualProtect((void*)clMoveChokeClamp, 4, PAGE_EXECUTE_READWRITE, &protect);
	*(std::uint32_t*)clMoveChokeClamp = 62;
	VirtualProtect((void*)clMoveChokeClamp, 4, protect, &protect);

}

template<class T, class U>
T fine(T in, U low, U high)
{
	if (in <= low)
		return low;

	if (in >= high)
		return high;

	return in;
}

void extrapolation()
{
	auto m_local = g_local;
	if (m_local && m_local->is_alive()) {
		for (int i = 0; i < interfaces::engine_client->get_max_clients(); i++)
		{
			auto m_entity = dynamic_cast<C_BasePlayer*>(interfaces::entity_list->get_client_entity(i));
			auto simtime = *reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(ctx::client.local) + 0x268);
			auto old_simtime = simtime + 4;
			float simtime_delta = simtime - old_simtime;
			int choked_ticks = fine(time_to_ticks(simtime_delta), 1, 15);
			Vector lastOrig;

			if (lastOrig.length() != m_entity->get_abs_origin().length())
				lastOrig = m_entity->get_abs_origin();

			float delta_distance = (m_entity->get_abs_origin() - lastOrig).length_sqr();
			if (delta_distance > 4096.f)
			{
				Vector velocity_per_tick = m_entity->vec_velocity() * interfaces::global_vars->interval_per_tick;
				auto new_origin = m_entity->get_abs_origin() + (velocity_per_tick * choked_ticks);
				m_entity->set_abs_original(new_origin);
			}
		}
	}
}

void compensate_recoil() noexcept
{
	auto local_player = reinterpret_cast<C_BasePlayer*>(interfaces::entity_list->get_client_entity(interfaces::engine_client->get_localplayer()));
	if (!local_player)
		return;

	auto weapon = local_player->active_weapon();
	if (weapon)
		weapon->get_inaccuracy();

	ctx::client.cmd->viewangles -= local_player->aim_punch_angle() * 2;
}

void __stdcall hooks::hk_create_move(int sequence_number, float input_sample_frametime, bool b_active, bool& send_packet)
{
	const auto ofunc = hlclient_hook.get_original< create_move >(index::create_move);
	ofunc(interfaces::chl_client, sequence_number, input_sample_frametime, b_active);

	auto cmd = interfaces::input->get_user_cmd(sequence_number);
	auto verified = interfaces::input->get_verified_cmd(sequence_number);

	g_features.event_logger.on_create_move();
	g_features.animations.player_animations();

	if (!g_local || !g_local->is_alive())
	{
		g_features.rage_backtrack.on_create_move();
		send_packet = true;
		return;
	}

	if (!cmd || !cmd->command_number || g_unload)
		return;

	ctx::client.cmd = cmd;
	ctx::client.local = g_local;
	ctx::client.should_choke = false;

	cmd->buttons |= IN_BULLRUSH;
	g_features.anti_aim.fake_crouch(cmd, send_packet);

	if (menu.is_visible())
		cmd->buttons &= ~IN_ATTACK;

	const auto old_viewangles = cmd->viewangles;
	globals.old_forwardmove = cmd->forwardmove;
	globals.old_sidemove = cmd->sidemove;

	globals.max_desync_delta = g_local->get_max_desync_delta();

	static std::once_flag fl;
	std::call_once(fl, []()
	{
		static auto max_choke_ticks = reinterpret_cast<int*> (g_utils.pattern_scan(xor_str("engine.dll"), xor_str("0F 4F F0 89 5D FC")) - 0x6);
		patch(static_cast<PVOID> (max_choke_ticks), 17, 1);
	});

	static std::once_flag fl2;
	std::call_once(fl2, []()
	{
		setMoveChokeClampLimit();
	});

	//console::write_line ( *max_choke_ticks );

	g_features.resolver.on_create_move();

	compensate_recoil();

	//test = 32;

	//console::write_line ( test );

	/* features before (outside) prediction */
	c_misc::bhop(cmd);

	g_features.engine_prediction.run_engine_pred(cmd);
	/* features inside prediction */
	//rbot, etc...
	if (g_options.ragebot_antiaim_enable)
		g_features.anti_aim.run_lby_prediction();

	g_features.fakelag.on_create_move(cmd, send_packet);

	if (g_local->move_type() != MOVETYPE_LADDER && g_local->move_type() != MOVETYPE_FLY)
		g_features.anti_aim.run_antiaim(cmd, send_packet);
	else
		globals.real_yaw = cmd->viewangles.yaw;

	g_features.ragebot.on_create_move(cmd, send_packet);
	g_features.rage_backtrack.on_create_move();

	if (g_options.ragebot_antiaim_enable)
		g_features.anti_aim.lby_breaker ( cmd, send_packet );
	
	c_misc::airduck(cmd);

	//g_features.legitbot.on_create_move ( cmd );

	g_features.engine_prediction.end_engine_pred();

	/* features after prediction */

	g_math.normalize_angles(cmd->viewangles);
	g_math.clamp_angles(cmd->viewangles);

	if (g_local->move_type() != MOVETYPE_LADDER && g_local->move_type() != MOVETYPE_FLY)
		c_misc::auto_strafe(cmd, old_viewangles);

	g_features.anti_aim.slowwalk(cmd);

	if (interfaces::client_state->iChokedCommands >= 15)
		send_packet = true;

	if (g_options.ragebot_shooting_mode == static_cast<int> (shooting_modes_t::supress) && !globals.in_fakeduck && !g_options.
		ragebot_fakelag_while_shooting)
	{
		static auto last_yaw = 0.f;
		const auto supressed_tick_delta = interfaces::global_vars->tickcount - globals.prev_supressed_tick;

		static auto sw = false;

		if (supressed_tick_delta == 1)
		{
			send_packet = true;
			cmd->buttons &= ~IN_ATTACK;
			//sw = !sw;
			cmd->viewangles.yaw = last_yaw + (sw ? -(g_local->get_max_desync_delta() - 5.f) : (g_local->get_max_desync_delta() - 5.f));
		}

		if (g_features.ragebot.should_supress)
		{
			last_yaw = cmd->viewangles.yaw;
			g_features.ragebot.should_supress = false;
			send_packet = false;
			globals.prev_supressed_tick = interfaces::global_vars->tickcount;
		}
	}

	static auto was_attack_in_first_package = false;

	if ((was_attack_in_first_package || cmd->buttons & IN_ATTACK) && interfaces::client_state->iChokedCommands >= 14)
		send_packet = true;

	if (globals.in_fakeduck)
		send_packet = globals.fackeduck_send_packet_overwrite;

	was_attack_in_first_package = interfaces::client_state->iChokedCommands == 0 && (cmd->buttons & IN_ATTACK);

	g_features.anti_aim.finish_antiaim(cmd, send_packet);

	if (g_options.misc_slidewalk && (g_local->move_type() != MOVETYPE_LADDER && g_local->move_type() != MOVETYPE_FLY))
	{
		if (cmd->forwardmove > 0)
		{
			cmd->buttons |= IN_BACK;
			cmd->buttons &= ~IN_FORWARD;
		}

		if (cmd->forwardmove < 0)
		{
			cmd->buttons |= IN_FORWARD;
			cmd->buttons &= ~IN_BACK;
		}

		if (cmd->sidemove < 0)
		{
			cmd->buttons |= IN_MOVERIGHT;
			cmd->buttons &= ~IN_MOVELEFT;
		}

		if (cmd->sidemove > 0)
		{
			cmd->buttons |= IN_MOVELEFT;
			cmd->buttons &= ~IN_MOVERIGHT;
		}
	}

	if (!interfaces::client_state->iChokedCommands)
		globals.anim_did_update = true;

	/* todo add support for disabling */
	g_math.normalize_angles(cmd->viewangles);
	g_math.clamp_angles(cmd->viewangles);

	//g_features.anti_aim.fake_animations ( cmd, send_packet );
	//g_features.anti_aim.real_animations ( cmd, send_packet );
	g_features.animations.on_create_move(send_packet);

	/* finish move */
	verified->m_cmd = *cmd;
	verified->m_crc = cmd->GetChecksum();
}

__declspec (naked) void __stdcall hooks::hk_create_move_proxy(int sequence_number, float input_sample_frametime, bool active)
{
	__asm
	{
		push ebp
		mov ebp, esp
		push ebx
		lea ecx, [esp]
		push ecx
		push dword ptr[active]
		push dword ptr[input_sample_frametime]
		push dword ptr[sequence_number]
		call hooks::hk_create_move
		pop ebx
		pop ebp
		retn 0Ch
	}
}
