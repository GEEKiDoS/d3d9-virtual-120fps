#pragma once

class d3d9ex_swapchain_proxy : IDirect3DSwapChain9Ex
{
public:
	d3d9ex_swapchain_proxy(IDirect3DSwapChain9* orig, renderer* renderer);

	virtual HRESULT __stdcall QueryInterface(REFIID riid, void** ppvObj) override;
	virtual ULONG __stdcall AddRef(void) override;
	virtual ULONG __stdcall Release(void) override;
	virtual HRESULT __stdcall Present(const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion, DWORD dwFlags) override;
	virtual HRESULT __stdcall GetFrontBufferData(IDirect3DSurface9* pDestSurface) override;
	virtual HRESULT __stdcall GetBackBuffer(UINT iBackBuffer, D3DBACKBUFFER_TYPE Type, IDirect3DSurface9** ppBackBuffer) override;
	virtual HRESULT __stdcall GetRasterStatus(D3DRASTER_STATUS* pRasterStatus) override;
	virtual HRESULT __stdcall GetDisplayMode(D3DDISPLAYMODE* pMode) override;
	virtual HRESULT __stdcall GetDevice(IDirect3DDevice9** ppDevice) override;
	virtual HRESULT __stdcall GetPresentParameters(D3DPRESENT_PARAMETERS* pPresentationParameters) override;
	virtual HRESULT __stdcall GetLastPresentCount(UINT* pLastPresentCount) override;
	virtual HRESULT __stdcall GetPresentStats(D3DPRESENTSTATS* pPresentationStatistics) override;
	virtual HRESULT __stdcall GetDisplayModeEx(D3DDISPLAYMODEEX* pMode, D3DDISPLAYROTATION* pRotation) override;
	
private:
	IDirect3DSwapChain9Ex* m_swapchain;
	double m_lastTime;
	renderer *d3d11_renderer;
};
