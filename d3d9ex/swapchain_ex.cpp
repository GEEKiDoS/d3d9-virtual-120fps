#include "stdinc.hpp"

d3d9ex_swapchain_proxy::d3d9ex_swapchain_proxy(IDirect3DSwapChain9* orig, renderer *renderer, UINT index)
{
	this->m_swapchain = reinterpret_cast<IDirect3DSwapChain9Ex*>(orig);
	this->monitor_renderer = renderer;
	this->swapchain_index = index;
}

HRESULT __stdcall d3d9ex_swapchain_proxy::QueryInterface(REFIID riid, void** ppvObj)
{
	*ppvObj = nullptr;

	auto hr = m_swapchain->QueryInterface(riid, ppvObj);
	if (hr == NOERROR) *ppvObj = this;

	return hr;
}

ULONG __stdcall d3d9ex_swapchain_proxy::AddRef(void)
{
	return m_swapchain->AddRef();
}

ULONG __stdcall d3d9ex_swapchain_proxy::Release(void)
{
	auto count = m_swapchain->Release();

	if (!count) delete this;

	return count;
}

HRESULT __stdcall d3d9ex_swapchain_proxy::Present(const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion, DWORD dwFlags)
{
	auto hr = m_swapchain->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion, dwFlags);

	if (GetIndex() == 0)
	{
		monitor_renderer->queue_frame();
	}
	
	// do_fps_limit(&m_lastTime);

	return hr;
}

HRESULT __stdcall d3d9ex_swapchain_proxy::GetFrontBufferData(IDirect3DSurface9* pDestSurface)
{
	return m_swapchain->GetFrontBufferData(pDestSurface);
}

HRESULT __stdcall d3d9ex_swapchain_proxy::GetBackBuffer(UINT iBackBuffer, D3DBACKBUFFER_TYPE Type, IDirect3DSurface9** ppBackBuffer)
{
	return m_swapchain->GetBackBuffer(iBackBuffer, Type, ppBackBuffer);
}

HRESULT __stdcall d3d9ex_swapchain_proxy::GetRasterStatus(D3DRASTER_STATUS* pRasterStatus)
{
	return m_swapchain->GetRasterStatus(pRasterStatus);
}

HRESULT __stdcall d3d9ex_swapchain_proxy::GetDisplayMode(D3DDISPLAYMODE* pMode)
{
	return m_swapchain->GetDisplayMode(pMode);
}

HRESULT __stdcall d3d9ex_swapchain_proxy::GetDevice(IDirect3DDevice9** ppDevice)
{
	return m_swapchain->GetDevice(ppDevice);
}

HRESULT __stdcall d3d9ex_swapchain_proxy::GetPresentParameters(D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	return m_swapchain->GetPresentParameters(pPresentationParameters);
}

HRESULT __stdcall d3d9ex_swapchain_proxy::GetLastPresentCount(UINT* pLastPresentCount)
{
	return m_swapchain->GetLastPresentCount(pLastPresentCount);
}

HRESULT __stdcall d3d9ex_swapchain_proxy::GetPresentStats(D3DPRESENTSTATS* pPresentationStatistics)
{
	return m_swapchain->GetPresentStats(pPresentationStatistics);
}

HRESULT __stdcall d3d9ex_swapchain_proxy::GetDisplayModeEx(D3DDISPLAYMODEEX* pMode, D3DDISPLAYROTATION* pRotation)
{
	return m_swapchain->GetDisplayModeEx(pMode, pRotation);
}
