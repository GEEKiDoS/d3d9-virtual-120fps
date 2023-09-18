#include "stdinc.hpp"

d3d9ex_proxy::d3d9ex_proxy(IDirect3D9Ex* orig)
{
	this->m_d3d = orig;
}

HRESULT __stdcall d3d9ex_proxy::QueryInterface(REFIID riid, void** ppvObj)
{
	*ppvObj = nullptr;

	auto hr = m_d3d->QueryInterface(riid, ppvObj);
	if (hr == NOERROR) *ppvObj = this;

	return hr;
}

ULONG __stdcall d3d9ex_proxy::AddRef(void)
{
	return m_d3d->AddRef();
}

ULONG __stdcall d3d9ex_proxy::Release(void)
{
	auto count = m_d3d->Release();

	if (!count) delete this;

	return count;
}

HRESULT __stdcall d3d9ex_proxy::RegisterSoftwareDevice(void* pInitializeFunction)
{
	return m_d3d->RegisterSoftwareDevice(pInitializeFunction);
}

UINT __stdcall d3d9ex_proxy::GetAdapterCount(void)
{
	return m_d3d->GetAdapterCount();
}

HRESULT __stdcall d3d9ex_proxy::GetAdapterIdentifier(UINT Adapter, DWORD Flags, D3DADAPTER_IDENTIFIER9* pIdentifier)
{
	return m_d3d->GetAdapterIdentifier(Adapter, Flags, pIdentifier);
}

UINT __stdcall d3d9ex_proxy::GetAdapterModeCount(UINT Adapter, D3DFORMAT Format)
{ 
	if (config.mode_count) 
		return config.mode_count;

	return m_d3d->GetAdapterModeCount(Adapter, Format);
}

HRESULT __stdcall d3d9ex_proxy::EnumAdapterModes(UINT Adapter, D3DFORMAT Format, UINT Mode, D3DDISPLAYMODE* pMode)
{
	if (config.mode_count)
	{
		if (Mode >= config.mode_count) 
			return D3DERR_INVALIDCALL;

		*pMode = config.modes[Mode];

		return D3D_OK;
	}

	return m_d3d->EnumAdapterModes(Adapter, Format, Mode, pMode);
}

HRESULT __stdcall d3d9ex_proxy::GetAdapterDisplayMode(UINT Adapter, D3DDISPLAYMODE* pMode)
{
	return m_d3d->GetAdapterDisplayMode(Adapter, pMode);
}

HRESULT __stdcall d3d9ex_proxy::CheckDeviceType(UINT Adapter, D3DDEVTYPE DevType, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, BOOL bWindowed)
{
	return m_d3d->CheckDeviceType(Adapter, DevType, AdapterFormat, BackBufferFormat, bWindowed);
}

HRESULT __stdcall d3d9ex_proxy::CheckDeviceFormat(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, DWORD Usage, D3DRESOURCETYPE RType, D3DFORMAT CheckFormat)
{
	return m_d3d->CheckDeviceFormat(Adapter, DeviceType, AdapterFormat, Usage, RType, CheckFormat);
}

HRESULT __stdcall d3d9ex_proxy::CheckDeviceMultiSampleType(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SurfaceFormat, BOOL Windowed, D3DMULTISAMPLE_TYPE MultiSampleType, DWORD* pQualityLevels)
{
	return m_d3d->CheckDeviceMultiSampleType(Adapter, DeviceType, SurfaceFormat, Windowed, MultiSampleType, pQualityLevels);
}

HRESULT __stdcall d3d9ex_proxy::CheckDepthStencilMatch(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, D3DFORMAT RenderTargetFormat, D3DFORMAT DepthStencilFormat)
{
	return m_d3d->CheckDepthStencilMatch(Adapter, DeviceType, AdapterFormat, RenderTargetFormat, DepthStencilFormat);
}

HRESULT __stdcall d3d9ex_proxy::CheckDeviceFormatConversion(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SourceFormat, D3DFORMAT TargetFormat)
{
	return m_d3d->CheckDeviceFormatConversion(Adapter, DeviceType, SourceFormat, TargetFormat);
}

HRESULT __stdcall d3d9ex_proxy::GetDeviceCaps(UINT Adapter, D3DDEVTYPE DeviceType, D3DCAPS9* pCaps)
{
	return m_d3d->GetDeviceCaps(Adapter, DeviceType, pCaps);
}

HMONITOR __stdcall d3d9ex_proxy::GetAdapterMonitor(UINT Adapter)
{
	return m_d3d->GetAdapterMonitor(Adapter);
}

HRESULT d3d9ex_proxy::modify_present_params(UINT Adapter, D3DPRESENT_PARAMETERS* pPresentationParameters, D3DDISPLAYMODEEX* pFullscreenDisplayMode)
{
	if (!pPresentationParameters->Windowed)
	{
		log("running in fullscreen\n");

		const auto width = GetSystemMetrics(SM_CXSCREEN);
		const auto height = GetSystemMetrics(SM_CYSCREEN);

		DEVMODEA devmode;
		uint32_t refresh_rate = 0;

		for (auto i = 0; i < 1000; ++i)
		{
			if (!EnumDisplaySettingsA(0, i, &devmode))
				break;

			if ((devmode.dmPelsWidth == width && devmode.dmPelsHeight == height) && (devmode.dmDisplayFrequency > refresh_rate))
			{
				refresh_rate = devmode.dmDisplayFrequency;
			}
		}

		if (!refresh_rate)
		{
			log("no matching refresh rate\n");
			return D3DERR_INVALIDCALL;
		}

		pPresentationParameters->BackBufferWidth = width;
		pPresentationParameters->BackBufferHeight = height;
		pPresentationParameters->FullScreen_RefreshRateInHz = refresh_rate;

		if (pFullscreenDisplayMode)
		{
			pFullscreenDisplayMode->Width = width;
			pFullscreenDisplayMode->Height = height;
			pFullscreenDisplayMode->RefreshRate = refresh_rate;
		}

		log("auto fullscreen mode: width=%d, height=%d, refresh_rate=%d\n", width, height, refresh_rate);
	}

	pPresentationParameters->PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	return D3D_OK;
}

HRESULT __stdcall d3d9ex_proxy::CreateDevice(UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice9** ppReturnedDeviceInterface)
{
	IDirect3DDevice9* device = nullptr;

	auto hr = modify_present_params(Adapter, pPresentationParameters, nullptr);
	if (!SUCCEEDED(hr)) return hr;

	hr = m_d3d->CreateDevice(Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, &device);
	if (!SUCCEEDED(hr)) return hr;

	*ppReturnedDeviceInterface = new d3d9_device_proxy(device);

	log("interface ex: created device\n");

	return hr;
}

UINT __stdcall d3d9ex_proxy::GetAdapterModeCountEx(UINT Adapter, const D3DDISPLAYMODEFILTER* pFilter)
{
	return m_d3d->GetAdapterModeCountEx(Adapter, pFilter);
}

HRESULT __stdcall d3d9ex_proxy::EnumAdapterModesEx(UINT Adapter, const D3DDISPLAYMODEFILTER* pFilter, UINT Mode, D3DDISPLAYMODEEX* pMode)
{
	return m_d3d->EnumAdapterModesEx(Adapter, pFilter, Mode, pMode);
}

HRESULT __stdcall d3d9ex_proxy::GetAdapterDisplayModeEx(UINT Adapter, D3DDISPLAYMODEEX* pMode, D3DDISPLAYROTATION* pRotation)
{
	return m_d3d->GetAdapterDisplayModeEx(Adapter, pMode, pRotation);
}

HRESULT __stdcall d3d9ex_proxy::CreateDeviceEx(UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, D3DDISPLAYMODEEX* pFullscreenDisplayMode, IDirect3DDevice9Ex** ppReturnedDeviceInterface)
{
	IDirect3DDevice9Ex* device = nullptr;

	// auto hr = modify_present_params(Adapter, pPresentationParameters, pFullscreenDisplayMode);
	// if (!SUCCEEDED(hr)) return hr;

	pPresentationParameters->Windowed = true;
	pPresentationParameters->FullScreen_RefreshRateInHz = 0;
	pPresentationParameters->PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	auto hr = m_d3d->CreateDeviceEx(Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, nullptr, &device);
	if (!SUCCEEDED(hr)) return hr;

	*ppReturnedDeviceInterface = new d3d9ex_device_proxy(device);

	log("interface ex: created device ex\n");

	return hr;
}

HRESULT __stdcall d3d9ex_proxy::GetAdapterLUID(UINT Adapter, LUID* pLUID)
{
	return m_d3d->GetAdapterLUID(Adapter, pLUID);
}
