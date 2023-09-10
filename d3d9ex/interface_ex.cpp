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

D3DDISPLAYMODE preset_display_modes[]
{
	{ 1920, 1080, 120, D3DFMT_X8R8G8B8 },
	{ 1280, 720, 60, D3DFMT_X8R8G8B8 },
};

UINT __stdcall d3d9ex_proxy::GetAdapterModeCount(UINT Adapter, D3DFORMAT Format)
{ 
	return 2;
	//return m_d3d->GetAdapterModeCount(Adapter, Format);
}

HRESULT __stdcall d3d9ex_proxy::EnumAdapterModes(UINT Adapter, D3DFORMAT Format, UINT Mode, D3DDISPLAYMODE* pMode)
{
	*pMode = preset_display_modes[Mode];

	return S_OK;
	//return m_d3d->EnumAdapterModes(Adapter, Format, Mode, pMode);
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

HRESULT __stdcall d3d9ex_proxy::CreateDevice(UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice9** ppReturnedDeviceInterface)
{
	IDirect3DDevice9* device = nullptr;

	auto hr = m_d3d->CreateDevice(Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, &device);
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

	if (pFullscreenDisplayMode)
	{
		log("fullscreen\n");

		const auto width = GetSystemMetrics(SM_CXSCREEN);
		const auto height = GetSystemMetrics(SM_CYSCREEN);

		const auto mode_count = m_d3d->GetAdapterModeCount(Adapter, D3DFMT_X8R8G8B8);
		D3DDISPLAYMODE preferred;
		preferred.RefreshRate = 0;

		for (int i = 0; i < mode_count; i++)
		{
			D3DDISPLAYMODE mode;

			m_d3d->EnumAdapterModes(Adapter, D3DFMT_X8R8G8B8, i, &mode);

			if ((mode.Width == width && mode.Height == height) && (mode.RefreshRate > preferred.RefreshRate))
			{
				memcpy(&preferred, &mode, sizeof(D3DDISPLAYMODE));
			}
		}
		
		if (!preferred.RefreshRate)
		{
			log("no matching refresh rate\n");
			return D3DERR_INVALIDCALL;
		}

		pPresentationParameters->BackBufferWidth = preferred.Width;
		pPresentationParameters->BackBufferHeight = preferred.Height;
		pPresentationParameters->FullScreen_RefreshRateInHz = preferred.RefreshRate;
		pFullscreenDisplayMode->Width = preferred.Width;
		pFullscreenDisplayMode->Height = preferred.Height;
		pFullscreenDisplayMode->RefreshRate = preferred.RefreshRate;
	}

	pPresentationParameters->PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	auto hr = m_d3d->CreateDeviceEx(Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, pFullscreenDisplayMode, &device);
	*ppReturnedDeviceInterface = new d3d9ex_device_proxy(device);

	log("interface ex: created device ex\n");

	return hr;
}

HRESULT __stdcall d3d9ex_proxy::GetAdapterLUID(UINT Adapter, LUID* pLUID)
{
	return m_d3d->GetAdapterLUID(Adapter, pLUID);
}
