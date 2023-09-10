#include "device.hpp"

// actually proxy to d3d9ex
class d3d9_proxy : public IDirect3D9
{
public:
	d3d9_proxy(IDirect3D9Ex* orig);

private:
	IDirect3D9Ex* m_d3d;

public:
	virtual HRESULT __stdcall QueryInterface(REFIID riid, void** ppvObj) override;
	virtual ULONG __stdcall AddRef(void) override;
	virtual ULONG __stdcall Release(void) override;
	virtual HRESULT __stdcall RegisterSoftwareDevice(void* pInitializeFunction) override;
	virtual UINT __stdcall GetAdapterCount(void) override;
	virtual HRESULT __stdcall GetAdapterIdentifier(UINT Adapter, DWORD Flags, D3DADAPTER_IDENTIFIER9* pIdentifier) override;
	virtual UINT __stdcall GetAdapterModeCount(UINT Adapter, D3DFORMAT Format) override;
	virtual HRESULT __stdcall EnumAdapterModes(UINT Adapter, D3DFORMAT Format, UINT Mode, D3DDISPLAYMODE* pMode) override;
	virtual HRESULT __stdcall GetAdapterDisplayMode(UINT Adapter, D3DDISPLAYMODE* pMode) override;
	virtual HRESULT __stdcall CheckDeviceType(UINT Adapter, D3DDEVTYPE DevType, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, BOOL bWindowed) override;
	virtual HRESULT __stdcall CheckDeviceFormat(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, DWORD Usage, D3DRESOURCETYPE RType, D3DFORMAT CheckFormat) override;
	virtual HRESULT __stdcall CheckDeviceMultiSampleType(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SurfaceFormat, BOOL Windowed, D3DMULTISAMPLE_TYPE MultiSampleType, DWORD* pQualityLevels) override;
	virtual HRESULT __stdcall CheckDepthStencilMatch(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, D3DFORMAT RenderTargetFormat, D3DFORMAT DepthStencilFormat) override;
	virtual HRESULT __stdcall CheckDeviceFormatConversion(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SourceFormat, D3DFORMAT TargetFormat) override;
	virtual HRESULT __stdcall GetDeviceCaps(UINT Adapter, D3DDEVTYPE DeviceType, D3DCAPS9* pCaps) override;
	virtual HMONITOR __stdcall GetAdapterMonitor(UINT Adapter) override;
	virtual HRESULT __stdcall CreateDevice(UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice9** ppReturnedDeviceInterface) override;
};
