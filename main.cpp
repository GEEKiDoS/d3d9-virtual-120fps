#include <stdinc.hpp>

namespace
{
	IDirect3D9* (WINAPI* create_d3d9_orig)(UINT);
	HRESULT(WINAPI* create_d3d9ex_orig)(UINT, IDirect3D9Ex**);
	

	FILE* logfile = nullptr;
}

extern "C" 
{
	HRESULT WINAPI create_d3d9ex(UINT SDKVersion, IDirect3D9Ex** ppD3D9Ex)
	{
		IDirect3D9Ex* d3d9ex = nullptr;

		auto hr = create_d3d9ex_orig(SDKVersion, &d3d9ex);

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

double msec()
{
	LARGE_INTEGER now, freq;
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&now);

	return now.QuadPart / static_cast<double>(freq.QuadPart) * 1000.f;
}

void do_fps_limit(double *last)
{
	const auto frametime_max = *last + FRAMETIME_TARGET;

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

	logfile = fopen("d3d9_proxy.log", "w");

	CHAR mPath[MAX_PATH];

	GetSystemDirectoryA(mPath, MAX_PATH);
	strcat(mPath, "\\");
	strcat(mPath, "d3d9.dll");

	HMODULE mod = LoadLibraryA(mPath);

	if (mod == nullptr)
	{
		log("d3d9 proxy: failed to find original d3d9.\n");
		return FALSE;
	}

	create_d3d9_orig = reinterpret_cast<decltype(Direct3DCreate9)*>(GetProcAddress(mod, "Direct3DCreate9"));
	create_d3d9ex_orig = reinterpret_cast<decltype(Direct3DCreate9Ex)*>(GetProcAddress(mod, "Direct3DCreate9Ex"));

	HMODULE nt_dll = LoadLibraryA("ntdll.dll");

	if (nt_dll == nullptr)
	{
		log("d3d9 proxy: failed to load ntdll.\n");
		return FALSE;
	}

	return TRUE;
}