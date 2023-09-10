#include "stdinc.hpp"

d3d9_proxy::d3d9_proxy(IDirect3D9Ex* orig)
{
	this->m_d3d = orig;
}

HRESULT __stdcall d3d9_proxy::QueryInterface(REFIID riid, void** ppvObj)
{
	return m_d3d->QueryInterface(riid, ppvObj);
}

ULONG __stdcall d3d9_proxy::AddRef(void)
{
	return m_d3d->AddRef();
}

ULONG __stdcall d3d9_proxy::Release(void)
{
	return m_d3d->Release();
}

HRESULT __stdcall d3d9_proxy::RegisterSoftwareDevice(void* pInitializeFunction)
{
	return m_d3d->RegisterSoftwareDevice(pInitializeFunction);
}

UINT __stdcall d3d9_proxy::GetAdapterCount(void)
{
	return m_d3d->GetAdapterCount();
}

HRESULT __stdcall d3d9_proxy::GetAdapterIdentifier(UINT Adapter, DWORD Flags, D3DADAPTER_IDENTIFIER9* pIdentifier)
{
	return m_d3d->GetAdapterIdentifier(Adapter, Flags, pIdentifier);
}

UINT __stdcall d3d9_proxy::GetAdapterModeCount(UINT Adapter, D3DFORMAT Format)
{
	return m_d3d->GetAdapterModeCount(Adapter, Format);
}

HRESULT __stdcall d3d9_proxy::EnumAdapterModes(UINT Adapter, D3DFORMAT Format, UINT Mode, D3DDISPLAYMODE* pMode)
{
	return m_d3d->EnumAdapterModes(Adapter, Format, Mode, pMode);
}

HRESULT __stdcall d3d9_proxy::GetAdapterDisplayMode(UINT Adapter, D3DDISPLAYMODE* pMode)
{
	return m_d3d->GetAdapterDisplayMode(Adapter, pMode);
}

HRESULT __stdcall d3d9_proxy::CheckDeviceType(UINT Adapter, D3DDEVTYPE DevType, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, BOOL bWindowed)
{
	return m_d3d->CheckDeviceType(Adapter, DevType, AdapterFormat, BackBufferFormat, bWindowed);
}

HRESULT __stdcall d3d9_proxy::CheckDeviceFormat(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, DWORD Usage, D3DRESOURCETYPE RType, D3DFORMAT CheckFormat)
{
	return m_d3d->CheckDeviceFormat(Adapter, DeviceType, AdapterFormat, Usage, RType, CheckFormat);
}

HRESULT __stdcall d3d9_proxy::CheckDeviceMultiSampleType(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SurfaceFormat, BOOL Windowed, D3DMULTISAMPLE_TYPE MultiSampleType, DWORD* pQualityLevels)
{
	return m_d3d->CheckDeviceMultiSampleType(Adapter, DeviceType, SurfaceFormat, Windowed, MultiSampleType, pQualityLevels);
}

HRESULT __stdcall d3d9_proxy::CheckDepthStencilMatch(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, D3DFORMAT RenderTargetFormat, D3DFORMAT DepthStencilFormat)
{
	return m_d3d->CheckDepthStencilMatch(Adapter, DeviceType, AdapterFormat, RenderTargetFormat, DepthStencilFormat);
}

HRESULT __stdcall d3d9_proxy::CheckDeviceFormatConversion(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SourceFormat, D3DFORMAT TargetFormat)
{
	return m_d3d->CheckDeviceFormatConversion(Adapter, DeviceType, SourceFormat, TargetFormat);
}

HRESULT __stdcall d3d9_proxy::GetDeviceCaps(UINT Adapter, D3DDEVTYPE DeviceType, D3DCAPS9* pCaps)
{
	return m_d3d->GetDeviceCaps(Adapter, DeviceType, pCaps);
}

HMONITOR __stdcall d3d9_proxy::GetAdapterMonitor(UINT Adapter)
{
	return m_d3d->GetAdapterMonitor(Adapter);
}

HRESULT __stdcall d3d9_proxy::CreateDevice(UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice9** ppReturnedDeviceInterface)
{
	pPresentationParameters->FullScreen_RefreshRateInHz = 0;
	pPresentationParameters->PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	return m_d3d->CreateDevice(Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface);
}
