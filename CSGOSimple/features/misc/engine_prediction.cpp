#include "engine_prediction.h"
#include "../../helpers/globals.h"

CMoveData cMoveData;

void c_engine_prediction::run_engine_pred(usercmd_t* cmd)
{
    if (!g_local)
        return;

    static usercmd_t* pLastCmd;
    pLastCmd = cmd;

    static auto nTickBase = 0;

    flOldCurtime = interfaces::global_vars->curtime;
    flOldFrametime = interfaces::global_vars->frametime;
    ctx::cheat.unpredicted_velocity = g_local->vec_velocity();

    globals.curtime = g_utils.get_estimate_server_time(cmd);

    if (!pLastCmd || pLastCmd->hasbeenpredicted) {
        nTickBase = g_local->tick_base();
    }
    else {
        nTickBase++;
    }

    interfaces::global_vars->curtime = globals.curtime;
    interfaces::global_vars->frametime = interfaces::global_vars->interval_per_tick;

    CMoveData data;
    memset(&data, 0, sizeof(CMoveData));

    if (!interfaces::engine_client->is_connected() && !interfaces::engine_client->is_in_game())
        return;

    if (!interfaces::move_helper || !cmd || !g_local || !g_local->is_alive())
        return;

    iFlagsBackup = g_local->flags();
    iButtonsBackup = cmd->buttons;

    g_local->current_command() = cmd;

    if (!m_pPredictionRandomSeed || !m_pSetPredictionPlayer)
    {
        m_pPredictionRandomSeed = *reinterpret_cast<int**> (g_utils.pattern_scan(xor_str("client.dll"),
            xor_str("A3 ? ? ? ? 66 0F 6E 86")) + 0x1);

        m_pSetPredictionPlayer = *reinterpret_cast<int**> (g_utils.pattern_scan(xor_str("client.dll"),
            xor_str("89 35 ? ? ? ? F3 0F 10 48")) + 0x2);
    }

    *m_pPredictionRandomSeed = MD5_PseudoRandom(cmd->command_number) & 0x7FFFFFFF;
    *m_pSetPredictionPlayer = uintptr_t(g_local);

    interfaces::game_movement->start_track_prediction_errors(g_local);

    interfaces::move_helper->set_host(g_local);
    interfaces::prediction->setup_move(g_local, cmd, interfaces::move_helper, &data);
    interfaces::game_movement->process_movement(g_local, &data);
    interfaces::prediction->finish_move(g_local, cmd, &data);
    interfaces::move_helper->set_host(nullptr);

    g_local->current_command() = nullptr;

    pLastCmd = cmd;
}

void c_engine_prediction::end_engine_pred() const
{
    interfaces::game_movement->finish_track_prediction_errors(g_local);
    interfaces::move_helper->set_host(nullptr);

    interfaces::global_vars->curtime = flOldCurtime;
    interfaces::global_vars->frametime = flOldFrametime;
}