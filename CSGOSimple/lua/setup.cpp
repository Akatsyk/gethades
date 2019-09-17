#include "lua_support.h"
#include "../helpers/console.h"
#include "../helpers/input.hpp"
#include "../helpers/math.hpp"
#include "../renderer/vgui_render.h"
#include "../features/features.h"
#include "../options.h"
#include <filesystem>
#include "../helpers/debug_console.h"

#include "types.h"

lua_support g_lua;

void lua_support::setup ( )
{
    lua.open_libraries ( sol::lib::math, sol::lib::base );
    //lua.
    //sol::argument_handler
    //lua.set

#ifdef _DEBUG
    lua.set_function ( "print", [] ( const std::string msg )
    {
        console::write_line ( msg );
    } );
#else
    lua.set_function ( xor_str ( "print" ), [] ( std::string msg ) {} );
#endif

    // setting types
    lua.new_usertype< lua_color_t > (
        xor_str ( "color" ),
        sol::constructors< lua_color_t  ( ), lua_color_t  ( int, int, int, int ), lua_color_t  ( int, int, int ), lua_color_t  ( float ) >( )
    );
    lua.new_usertype< lua_vector_t > (
        xor_str ( "vector" ), sol::constructors< lua_vector_t  ( ), lua_vector_t  ( float, float, float ) >( ),
        xor_str ( "dot" ), &lua_vector_t::dot,
        xor_str ( "x" ), &lua_vector_t::x,
        xor_str ( "y" ), &lua_vector_t::y,
        xor_str ( "z" ), &lua_vector_t::z
    );
    lua.new_usertype< lua_vector2d_t > (
        xor_str ( "vector2d" ), sol::constructors< lua_vector2d_t  ( ), lua_vector2d_t  ( float, float ) >( ),
        xor_str ( "x" ), &lua_vector2d_t::x,
        xor_str ( "y" ), &lua_vector2d_t::y
    );
    lua.new_usertype< lua_angle_t > (
        xor_str ( "angle" ), sol::constructors< lua_angle_t  ( ), lua_angle_t  ( float, float, float ) >( ),
        xor_str ( "pitch" ), &lua_angle_t::pitch,
        xor_str ( "pitch" ), &lua_angle_t::yaw,
        xor_str ( "pitch" ), &lua_angle_t::roll
    );
    lua.new_usertype< lua_netvar_t > (
        xor_str ( "netvar" ), sol::constructors< lua_netvar_t  ( ) >( ),
        xor_str ( "get_bool" ), &lua_netvar_t::get_bool,
        xor_str ( "get_int" ), &lua_netvar_t::get_int,
        xor_str ( "get_float" ), &lua_netvar_t::get_float,
        xor_str ( "set_bool" ), &lua_netvar_t::set_bool,
        xor_str ( "set_int" ), &lua_netvar_t::set_int,
        xor_str ( "set_float" ), &lua_netvar_t::set_float,
        xor_str ( "get_angle" ), &lua_netvar_t::get_angle,
        xor_str ( "get_vector" ), &lua_netvar_t::get_vector,
        xor_str ( "set_angle" ), &lua_netvar_t::set_angle,
        xor_str ( "set_vector" ), &lua_netvar_t::set_vector
    );
    lua.new_usertype< lua_entity_t > (
        xor_str ( "entity" ), sol::constructors< lua_entity_t  ( ), lua_entity_t  ( int ) >( ),
        xor_str ( "get_abs_origin" ), &lua_entity_t::get_abs_origin,
        xor_str ( "get_bone_position" ), &lua_entity_t::get_bone_position,
        xor_str ( "get_eye_angles" ), &lua_entity_t::get_eye_angles,
        xor_str ( "get_health" ), &lua_entity_t::get_health,
        xor_str ( "get_hitbox_position" ), &lua_entity_t::get_hitbox_position,
        xor_str ( "get_index" ), &lua_entity_t::get_index,
        xor_str ( "get_maxs" ), &lua_entity_t::get_maxs,
        xor_str ( "get_mins" ), &lua_entity_t::get_mins,
        xor_str ( "get_team_number" ), &lua_entity_t::get_team_number,
        xor_str ( "get_weapon_inaccuracy" ), &lua_entity_t::get_weapon_inaccuracy,
        xor_str ( "get_weapon_spread" ), &lua_entity_t::get_weapon_spread,
        xor_str ( "is_alive" ), &lua_entity_t::is_alive,
        xor_str ( "is_player" ), &lua_entity_t::is_player,
        xor_str ( "is_valid" ), &lua_entity_t::is_valid,
        xor_str ( "set_eye_angles" ), &lua_entity_t::set_eye_angles,
        xor_str ( "get_netvar" ), &lua_entity_t::get_netvar
    );
    lua.new_usertype< lua_setting_var_t > (
        xor_str ( "hvar" ), sol::constructors< lua_setting_var_t  ( ), lua_setting_var_t  ( std::string ) >( ),
        xor_str ( "get_bool" ), &lua_setting_var_t::get_bool,
        xor_str ( "get_int" ), &lua_setting_var_t::get_int,
        xor_str ( "get_float" ), &lua_setting_var_t::get_float,
        xor_str ( "get_color" ), &lua_setting_var_t::get_color,
        xor_str ( "set_bool" ), &lua_setting_var_t::set_bool,
        xor_str ( "set_int" ), &lua_setting_var_t::set_int,
        xor_str ( "set_float" ), &lua_setting_var_t::set_float,
        xor_str ( "set_color" ), &lua_setting_var_t::set_color
    );
    lua.new_usertype< lua_player_info_t > (
        xor_str ( "player_data" ), sol::constructors< lua_player_info_t  ( ) >( ),
        xor_str ( "is_gotv" ), &lua_player_info_t::is_gotv,
        xor_str ( "name" ), &lua_player_info_t::name,
        xor_str ( "steamid" ), &lua_player_info_t::steamid,
        xor_str ( "userid" ), &lua_player_info_t::userid
    );
    lua.new_usertype< lua_convar_t > (
        xor_str ( "convar" ), sol::constructors< lua_convar_t  ( ), lua_convar_t  ( std::string ) >( ),
        xor_str ( "get_bool" ), &lua_convar_t::get_bool,
        xor_str ( "get_int" ), &lua_convar_t::get_int,
        xor_str ( "get_float" ), &lua_convar_t::get_float,
        xor_str ( "set_bool" ), &lua_convar_t::set_bool,
        xor_str ( "set_int" ), &lua_convar_t::set_int,
        xor_str ( "set_float" ), &lua_convar_t::set_float
    );
    //lua.set ( "color", lua_color_t() );

    // setting functions

    // global vars
    //auto globals = lua.create_named_table ( "globals" );
    auto globals = lua.create_named_table ( xor_str ( "globals" ) );
    globals.set_function ( xor_str ( "tick_interval" ), []
    {
        return interfaces::global_vars->interval_per_tick;
    } );
    globals.set_function ( xor_str ( "tick_count" ), []
    {
        return interfaces::global_vars->tickcount;
    } );
    globals.set_function ( xor_str ( "real_time" ), []
    {
        return interfaces::global_vars->realtime;
    } );
    globals.set_function ( xor_str ( "cur_time" ), []
    {
        return interfaces::global_vars->curtime;
    } );
    globals.set_function ( xor_str ( "frame_count" ), []
    {
        return interfaces::global_vars->framecount;
    } );
    globals.set_function ( xor_str ( "frame_time" ), []
    {
        return interfaces::global_vars->frametime;
    } );
    globals.set_function ( xor_str ( "absolute_frame_time" ), []
    {
        return interfaces::global_vars->absoluteframetime;
    } );
    globals.set_function ( xor_str ( "max_clients" ), []
    {
        return interfaces::global_vars->max_clients;
    } );

    // client
    auto client = lua.create_named_table ( xor_str ( "client" ) );
    client.set_function ( xor_str ( "is_in_game" ), []
    {
        return interfaces::engine_client->is_in_game( );
    } );
    client.set_function ( xor_str ( "chat_say" ), [] ( std::string msg )
    {
        interfaces::engine_client->ExecuteClientCmd ( std::string ( xor_str ( "say \"" ) + msg + "\"" ).data( ) );
    } );
    client.set_function ( xor_str ( "chat_team_say" ), [] ( std::string msg )
    {
        interfaces::engine_client->ExecuteClientCmd ( std::string ( xor_str ( "say_team \"" ) + msg + "\"" ).data( ) );
    } );
    client.set_function ( xor_str ( "get_player_name_by_index" ), [] ( int idx )
    {
        player_info_t pi;
        interfaces::engine_client->get_player_info ( idx, &pi );

        return std::string ( pi.sz_name );
    } );
    client.set_function ( xor_str ( "world_to_screen" ), [] ( lua_vector_t vec )
    {
        Vector tmp_out;
        g_math.world_to_screen ( Vector ( vec.x, vec.y, vec.z ), tmp_out );
        return lua_vector2d_t ( tmp_out.x, tmp_out.y );
    } );
    client.set_function ( xor_str ( "get_player_info" ), [] ( int id )
    {
        player_info_t pl;
        interfaces::engine_client->get_player_info ( id, &pl );
        return lua_player_info_t{ pl.sz_name, pl.user_id, std::to_string ( pl.steam_id ), pl.is_hltv };
    } );
    client.set_function ( xor_str ( "get_localplayer_index" ), [] ( )
    {
        return interfaces::engine_client->get_localplayer( );
    } );

    // input
    auto input = lua.create_named_table ( xor_str ( "input" ) );
    input.set_function ( xor_str ( "is_key_down" ), [] ( int btn )
    {
        return g_inputsys.is_key_down ( btn );
    } );

    // engine
    auto engine = lua.create_named_table ( xor_str ( "engine" ) );
    engine.set_function ( xor_str ( "trace_line" ), [] ( lua_vector_t pos1, lua_vector_t pos2, int mask ) -> lua_trace_t
    {
        if ( !g_local )
            return lua_trace_t( );

        CGameTrace* trace;

        CTraceFilter filter;
        filter.pSkip = g_local;
        Ray_t ray;
        ray.Init ( vector_t ( pos1.x, pos1.y, pos1.z ), vector_t ( pos2.x, pos2.y, pos2.z ) );
        interfaces::engine_trace->trace_ray ( ray, mask, &filter, trace );
        return lua_trace_t ( trace );
    } );

    // entities
    auto entities = lua.create_named_table ( xor_str ( "entities" ) );
    entities.set_function ( xor_str ( "get_local_player" ), [] ( ) -> lua_entity_t*
    {
        C_BaseEntity* e = g_local;

        if ( !e )
            return nullptr;

        return &lua_entity_t ( e );
    } );
    entities.set_function ( xor_str ( "get_by_index" ), [] ( int idx ) -> lua_entity_t*
    {
        const auto tmp = interfaces::entity_list->get_client_entity ( idx );

        if ( !tmp )
            return nullptr;

        return &lua_entity_t ( reinterpret_cast< C_BaseEntity* > ( tmp ) );
    } );

    // callbacks
    auto callbacks = lua.create_named_table ( xor_str ( "callbacks" ) );
    callbacks.set_function ( xor_str ( "register" ), [&] ( std::string id, sol::function cb )
    {
        if ( this->callback_names.empty( ) || std::find ( this->callback_names.begin( ), this->callback_names.end( ), id ) == this->callback_names.end( ) )
        {
            this->last_error = xor_str ( "invalid callback id" );
#ifdef _DEBUG
            console::write_line ( "invalid callback id: " + id );
#endif
            return false;
        }

        g_lua.callbacks[ id ].push_back ( cb );
        return true;
    } );
    callbacks.set_function ( xor_str ( "get_last_error" ), [&]
    {
        return this->last_error;
    } );

    // draw
    auto draw = lua.create_named_table ( xor_str ( "draw" ) );
    draw.set_function ( xor_str ( "draw_line" ), [] ( int x, int y, int x2, int y2, lua_color_t clr )
    {
        g_render.line ( x, y, x2, y2, Color ( clr.r, clr.g, clr.b, clr.a ), 1.f );
    } );
    draw.set_function ( xor_str ( "draw_box" ), [] ( int x, int y, int x2, int y2, lua_color_t clr )
    {
        g_render.box ( x, y, x2, y2, Color ( clr.r, clr.g, clr.b, clr.a ), 1.f );
    } );
    draw.set_function ( xor_str ( "draw_filled_box" ), [] ( int x, int y, int x2, int y2, lua_color_t clr )
    {
        g_render.filled_box ( x, y, x2, y2, Color ( clr.r, clr.g, clr.b, clr.a ) );
    } );
    draw.set_function ( xor_str ( "draw_circle" ), [] ( int x, int y, int rad, lua_color_t clr )
    {
        g_render.circle ( x, y, rad, 24, Color ( clr.r, clr.g, clr.b, clr.a ) );
    } );
    draw.set_function ( xor_str ( "draw_circle_3d" ), [] ( int x, int y, int z, int rad, lua_color_t clr )
    {
        if ( !interfaces::engine_client->is_in_game( ) )
            return;

        g_render.circle_3d ( Vector ( x, y, z ), 24, rad, Color ( clr.r, clr.g, clr.b, clr.a ) );
    } );
    draw.set_function ( xor_str ( "get_screen_size" ), []
    {
        return lua_vector2d_t ( g_features.visuals.get_screensize( ).x, g_features.visuals.get_screensize( ).y );
    } );
    draw.set_function ( xor_str ( "get_screen_center" ), []
    {
        const auto tmp = g_render.get_screen_center( );
        return lua_vector2d_t ( tmp.x, tmp.y );
    } );

    // antiaim
    auto antiaim = lua.create_named_table ( xor_str ( "antiaim" ) );
    antiaim.set_function ( xor_str ( "register_real_yaw" ), [] ( std::string name, sol::function cb )
    {
        if ( g_features.anti_aim.real_base_antiaims.find ( name ) == g_features.anti_aim.real_base_antiaims.end( ) )
        {
            //auto index = g_lua.yaw_antiaims.empty(  ) ? 0 : g_lua.yaw_antiaims.size(  );
            g_lua.yaw_antiaims[ name ] = cb;
            //g_lua.yaw_antiaims[ name ] = cb;

            //std::string name,

            g_features.anti_aim.register_real_base_antiaim ( name, [&] ( std::string n, usercmd_t* cmd, bool send_packet )
            {
                try
                {
                    if ( g_lua.yaw_antiaims.at ( n ).valid( ) )
                        g_lua.yaw_antiaims.at ( n ).call ( lua_usercmd_t ( cmd ), send_packet );
                }
                catch ( std::exception& e )
                {
#ifdef _DEBUG
                    console::write_line ( e.what( ) );
#else
                    debug_console::print ( e.what() );
#endif // _DEBUG
                }
            } );
        }
    } );
    antiaim.set_function ( xor_str ( "register_real_add_yaw" ), [] ( std::string name, sol::function cb )
    {
        //if ( g_features.anti_aim.real_add_antiaims.find ( name ) == g_features.anti_aim.real_add_antiaims.end() )
        //{
        //    g_lua.yaw_antiaims[ name ] = cb;

        //    g_features.anti_aim.register_real_add_antiaim ( name, [&] ( usercmd_t* cmd, bool & send_packet )
        //    {
        //        try
        //        {
        //            if ( g_lua.yaw_antiaims[ name ].valid(  ) )
        //                g_lua.yaw_antiaims[ name ].call ( lua_usercmd_t ( cmd ), send_packet );
        //        }
        //        catch ( std::exception& e )
        //        {
        //            #ifdef _DEBUG
        //            console::write_line ( e.what(  ) );
        //            #else
        //            debug_console::print ( e.what() );
        //            #endif // _DEBUG
        //        }
        //    } );
        //}
    } );
    antiaim.set_function ( xor_str ( "register_desync_yaw" ), [] ( std::string name, sol::function cb )
    {
        //
    } );

    this->init = true;

    load_lua_files( );

    //lua.script ( R"(
    //	register('draw', function()
    //            draw_filled_box(50, 50, 125, 100, color.new(255, 255, 255, 255))
    //            draw_line(100, 100, 200, 200, color.new(255, 255, 255, 255))
    //       end)
    //)" );
}

void lua_support::load_lua_files ( )
{
    //if ( !std::filesystem::exists ( g_options.settings_folder_ ) )
    const auto folder = g_options.settings_folder_ + xor_str ( "\\lua" );

    if ( !std::filesystem::exists ( folder ) )
    {
        std::filesystem::create_directory ( folder );
        return;
    }

    for ( auto& dir_entry : std::filesystem::recursive_directory_iterator ( folder ) )
    {
        if ( std::filesystem::is_directory ( dir_entry ) || std::filesystem::path ( dir_entry ).extension( ).u8string( ) != xor_str ( ".lua" ) )
            continue;

        try
        {
            this->lua.load_file ( dir_entry.path( ).u8string( ) )( );

            debug_console::print ( xor_str ( "loaded lua file: " ) + std::filesystem::path ( dir_entry ).filename( ).u8string( ) );
        }
        catch ( const std::exception& e )
        {
#ifdef _DEBUG
            console::write_line ( e.what( ) );
            console::write_line ( xor_str ( "lua load error file: " ) + dir_entry.path( ).u8string( ) );
#else
            debug_console::print ( e.what() );
#endif // _DEBUG
        }
        catch ( ... )
        {
#ifdef _DEBUG
            console::write_line ( xor_str ( "lua load error file: " ) + dir_entry.path( ).u8string( ) );
#else
            debug_console::print ( xor_str ( "error load lua file" ) + dir_entry.path( ).u8string( ) );
#endif // _DEBUG
        }
    }
}

// callback caller functions
void lua_support::on_create_move ( usercmd_t* cmd )
{
    if ( !this->init || this->callbacks.empty( ) || this->callbacks[ xor_str ( "create_move" ) ].empty( ) )
        return;

    for ( const auto& func : this->callbacks[ xor_str ( "draw" ) ] )
    {
        try
        {
            if ( func.valid( ) )
                func.call ( lua_usercmd_t ( cmd ) );
        }
        catch ( const std::exception& e )
        {
#ifdef _DEBUG
            console::write_line ( e.what( ) );
#else
            debug_console::print ( e.what() );
#endif
        }
    }
}

void lua_support::on_paint_traverse ( )
{
    if ( !this->init || this->callbacks.empty( ) || this->callbacks[ xor_str ( "draw" ) ].empty( ) )
        return;

    for ( const auto& func : this->callbacks[ xor_str ( "draw" ) ] )
    {
        try
        {
            if ( func.valid( ) )
                func.call( );
        }
        catch ( const std::exception& e )
        {
#ifdef _DEBUG
            console::write_line ( e.what( ) );
#else
            debug_console::print ( e.what() );
#endif
        }
    }
}

void lua_support::on_resolve ( )
{
}

void lua_support::on_antiaim ( )
{
}
