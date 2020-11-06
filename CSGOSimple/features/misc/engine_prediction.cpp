#include "engine_prediction.h"
#include "../../helpers/globals.h"

CMoveData cMoveData;

void c_engine_prediction::run_engine_pred ( usercmd_t* cmd )
{
    if ( !g_local )
        return;

    static usercmd_t* pLastCmd;
    static auto nTickBase = 0;
    flOldCurtime = interfaces::global_vars->curtime;
    flOldFrametime = interfaces::global_vars->frametime;
    ctx::cheat.unpredicted_velocity = g_local->vec_velocity( );

    globals.curtime = g_utils.get_estimate_server_time ( cmd );

    if (!pLastCmd || pLastCmd->hasbeenpredicted) {
        nTickBase = g_local->tick_base();
    }
    else {
        nTickBase++;
    }

    interfaces::global_vars->curtime = nTickBase * interfaces::global_vars->interval_per_tick;
    interfaces::global_vars->frametime = interfaces::global_vars->interval_per_tick;


    //Let's back things up for later use
    iFlagsBackup = g_local->flags( );
    iButtonsBackup = cmd->buttons;

    //Set m_pCurrentCommand to cmd. We'll set this to nullptr later.
    g_local->current_command( ) = cmd;

    //Let's get some signatures now
    if ( !m_pPredictionRandomSeed || !m_pSetPredictionPlayer )
    {
        m_pPredictionRandomSeed = *reinterpret_cast< int** > ( g_utils.pattern_scan ( xor_str ( "client.dll" ),
                                                                                      xor_str ( "A3 ? ? ? ? 66 0F 6E 86" ) ) + 1 );
        // A3 ? ? ? ? 66 0F 6E 86  / +1
        m_pSetPredictionPlayer = *reinterpret_cast< int** > ( g_utils.pattern_scan ( xor_str ( "client.dll" ),
                                                                                     xor_str ( "89 35 ? ? ? ? F3 0F 10 48" ) ) + 2 );
    }
    *m_pPredictionRandomSeed = MD5_PseudoRandom(cmd->command_number) & 0x7FFFFFFF; //crashing here!!!!!!! some nullptr
    *m_pSetPredictionPlayer = uintptr_t(g_local);
    memset(&cMoveData, 0, sizeof(CMoveData));
    //Let's set it up so that it predicts ourselves
    interfaces::move_helper->set_host ( g_local );
  

    interfaces::prediction->setup_move ( g_local, cmd, interfaces::move_helper, &cMoveData);
    interfaces::game_movement->process_movement ( g_local, &cMoveData);
    interfaces::prediction->finish_move ( g_local, cmd, &cMoveData);
    interfaces::game_movement->finish_track_prediction_errors(g_local);
    interfaces::move_helper->set_host(nullptr);
    g_local->current_command() = nullptr;
    pLastCmd = cmd;
}

void c_engine_prediction::end_engine_pred ( ) const
{
   
   
    interfaces::global_vars->curtime = flOldCurtime;
    interfaces::global_vars->frametime = flOldFrametime;
}
