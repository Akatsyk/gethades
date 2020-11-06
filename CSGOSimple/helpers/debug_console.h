#pragma once

#include "../valve_sdk/csgostructs.hpp"

namespace debug_console
{
	void print(const std::string& msg);
	void debug(const std::string& msg);

	void clear();
};
