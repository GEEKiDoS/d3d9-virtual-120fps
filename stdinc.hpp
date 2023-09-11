#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d9.h>
#include <winternl.h>

#include <cstdio>
#include <cassert>
#include <ctime>
#include <cstdlib>

#include <unordered_map>

#include "MinHook.h"

#include "d3d9ex/interface_ex.hpp"
#include "d3d9/interface.hpp"

void log(const char* format, ...);
void do_fps_limit(double* last);

struct config_t {
	bool enable_logger;
	int target_fps;
	float bias;

	UINT mode_count;
	D3DDISPLAYMODE* modes;
};

extern config_t config;
