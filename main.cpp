#include <stdinc.hpp>

config_t config;

namespace
{
	void* create_d3d9ex_orig;
	void* enum_display_settings_orig;

	#define AS(type, pointer) (reinterpret_cast<decltype(type)*>(pointer))

	FILE* logfile = nullptr;
	double frametime_target;

	double msec()
	{
		LARGE_INTEGER now, freq;
		QueryPerformanceFrequency(&freq);
		QueryPerformanceCounter(&now);

		return now.QuadPart / static_cast<double>(freq.QuadPart) * 1000.f;
	}

	BOOL WINAPI enum_display_settings_hook(LPCSTR lpszDeviceName, DWORD iModeNum, DEVMODEA* lpDevMode)
	{
		if (iModeNum >= config.mode_count)
			return false;

		auto mode = &config.modes[iModeNum];
		
		lpDevMode->dmPelsWidth = mode->Width;
		lpDevMode->dmPelsHeight = mode->Height;
		lpDevMode->dmDisplayFrequency = mode->RefreshRate;
		lpDevMode->dmBitsPerPel = 32;
		lpDevMode->dmDisplayFlags = 0;

		return true;
	}
}

extern "C"
{
	HRESULT WINAPI create_d3d9ex(UINT SDKVersion, IDirect3D9Ex** ppD3D9Ex)
	{
		IDirect3D9Ex* d3d9ex = nullptr;

		auto hr = AS(Direct3DCreate9Ex, create_d3d9ex_orig)(SDKVersion, &d3d9ex);

		if (SUCCEEDED(hr))
		{
			*ppD3D9Ex = new d3d9ex_proxy(d3d9ex);
		}
		else
		{
			*ppD3D9Ex = nullptr;
		}

		return hr;
	}

	IDirect3D9* WINAPI create_d3d9(UINT SDKVersion)
	{
		IDirect3D9Ex* d3d9ex = nullptr;

		auto hr = create_d3d9ex(SDKVersion, &d3d9ex);
		if (SUCCEEDED(hr)) return new d3d9_proxy(d3d9ex);

		return nullptr;
	}
}

void log(const char* format, ...)
{
	if (!logfile) return;

	va_list va;
	va_start(va, format);
	vfprintf(logfile, format, va);
	va_end(va);

	fflush(logfile);
}

void do_fps_limit(double* last)
{
	const auto frametime_max = *last + frametime_target;

	while (msec() < frametime_max)
	{
		Sleep(0);
	}

	*last = msec();
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	if (ul_reason_for_call != DLL_PROCESS_ATTACH)
		return TRUE;

	CHAR tmp[MAX_PATH];

	config.enable_logger = GetPrivateProfileIntA("config", "enable_log", 1, "./d3d9_proxy.ini");
	config.target_fps = GetPrivateProfileIntA("config", "target_fps", 120, "./d3d9_proxy.ini");

	auto int_bias = GetPrivateProfileIntA("config", "bias", 50, "./d3d9_proxy.ini");
	config.bias = int_bias / 10000.f;

	frametime_target = 1000. / (config.target_fps + config.bias);

	config.mode_count = GetPrivateProfileIntA("modes", "count", 0, "./d3d9_proxy.ini");
	config.modes = new D3DDISPLAYMODE[config.mode_count];

	if (config.enable_logger)
	{
		logfile = fopen("d3d9_proxy.log", "w");
	}

	for (auto i = 0u; i < config.mode_count; i++)
	{
		sprintf(tmp, "mode%d", i + 1);
		GetPrivateProfileStringA("modes", tmp, "1920x1080@120", tmp, MAX_PATH, "./d3d9_proxy.ini");

		int width, height, hz;
		if (sscanf(tmp, "%dx%d@%d", &width, &height, &hz) != 3)
		{
			log("d3d9 proxy: invalied display mode format for mode %d (%s).\n", i, tmp);

			return FALSE;
		}

		config.modes[i].Width = width;
		config.modes[i].Height = height;
		config.modes[i].RefreshRate = hz;
		config.modes[i].Format = D3DFMT_X8R8G8B8;

		log("mode %d: width=%d height=%d hz=%d\n", i, width, height, hz);
	}

	if (!config.mode_count)
	{
		log("d3d9 proxy: mode count is 0, monitor mode spoofing is disabled.\n");
	}

	GetSystemDirectoryA(tmp, MAX_PATH);
	strcat(tmp, "\\");
	strcat(tmp, "d3d9.dll");

	HMODULE mod = LoadLibraryA(tmp);

	if (mod == nullptr)
	{
		log("d3d9 proxy: failed to find original d3d9.\n");
		return FALSE;
	}

	create_d3d9ex_orig = reinterpret_cast<decltype(Direct3DCreate9Ex)*>(GetProcAddress(mod, "Direct3DCreate9Ex"));

	if (!create_d3d9ex_orig)
	{
		log("d3d9 proxy: failed to load original d3d9 functions.\n");
		return FALSE;
	}

	if (MH_Initialize())
	{
		log("d3d9 proxy: failed to initialize minhook.\n");
		return FALSE;
	}

	return TRUE;
}