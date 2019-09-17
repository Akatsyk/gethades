#pragma once
#include "../valve_sdk/csgostructs.hpp"

#define SOL_CHECK_ARGUMENTS 1
#include "../include/sol.h"

class lua_support
{
public:
    // lua callbacks
    void on_create_move ( usercmd_t* cmd );
    void on_paint_traverse ( );
    void on_resolve ( );
    void on_antiaim ( );

    // setup lua state
    void setup ( );

    sol::state lua;
    std::unordered_map< std::string, std::vector< sol::function > > callbacks;
    std::unordered_map< std::string, sol::function > yaw_antiaims;
    //std::vector<sol::function> yaw_antiaims;
private:
    void load_lua_files ( );
    bool init = false;
    std::vector< std::string > callback_names = { xor_str ( "draw" ), xor_str ( "create_move" ) };
    std::string last_error = "";
};

extern lua_support g_lua;
