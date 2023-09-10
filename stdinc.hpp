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

#include "d3d9/interface.hpp"
#include "d3d9ex/interface_ex.hpp"

void log(const char* format, ...);

const auto TARGET_FPS = 120;
constexpr auto FRAMETIME_TARGET = 1000. / (TARGET_FPS + 0.005);
void do_fps_limit(double* last);
