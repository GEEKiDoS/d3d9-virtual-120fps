#include "stdinc.hpp"

d3d9_device_proxy::d3d9_device_proxy(IDirect3DDevice9* orig)
{
	this->m_device = orig;
}

HRESULT __stdcall d3d9_device_proxy::QueryInterface(REFIID riid, void** ppvObj)
{
	*ppvObj = nullptr;

	auto hr = m_device->QueryInterface(riid, ppvObj);
	if (hr == NOERROR) *ppvObj = this;

	return hr;
}

ULONG __stdcall d3d9_device_proxy::AddRef(void)
{
	return m_device->AddRef();
}

ULONG __stdcall d3d9_device_proxy::Release(void)
{
	auto count = m_device->Release();

	if (!count) delete this;

	return count;
}

HRESULT __stdcall d3d9_device_proxy::TestCooperativeLevel(void)
{
	return m_device->TestCooperativeLevel();
}

UINT __stdcall d3d9_device_proxy::GetAvailableTextureMem(void)
{
	return m_device->GetAvailableTextureMem();
}

HRESULT __stdcall d3d9_device_proxy::EvictManagedResources(void)
{
	return m_device->EvictManagedResources();
}

HRESULT __stdcall d3d9_device_proxy::GetDirect3D(IDirect3D9** ppD3D9)
{
	return m_device->GetDirect3D(ppD3D9);
}

HRESULT __stdcall d3d9_device_proxy::GetDeviceCaps(D3DCAPS9* pCaps)
{
	return m_device->GetDeviceCaps(pCaps);
}

HRESULT __stdcall d3d9_device_proxy::GetDisplayMode(UINT iSwapChain, D3DDISPLAYMODE* pMode)
{
	return m_device->GetDisplayMode(iSwapChain, pMode);
}

HRESULT __stdcall d3d9_device_proxy::GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS* pParameters)
{
	return m_device->GetCreationParameters(pParameters);
}

HRESULT __stdcall d3d9_device_proxy::SetCursorProperties(UINT XHotSpot, UINT YHotSpot, IDirect3DSurface9* pCursorBitmap)
{
	return m_device->SetCursorProperties(XHotSpot, YHotSpot, pCursorBitmap);
}

void __stdcall d3d9_device_proxy::SetCursorPosition(int X, int Y, DWORD Flags)
{
	return m_device->SetCursorPosition(X, Y, Flags);
}

BOOL __stdcall d3d9_device_proxy::ShowCursor(BOOL bShow)
{
	return m_device->ShowCursor(bShow);
}

HRESULT __stdcall d3d9_device_proxy::CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DSwapChain9** pSwapChain)
{
	log("device: created additional swapchain %i\n", m_swapchains.size());

	IDirect3DSwapChain9* swapchain;
	auto hr = m_device->CreateAdditionalSwapChain(pPresentationParameters, &swapchain);

	if (SUCCEEDED(hr))
	{
		// TODO
		auto proxy = new d3d9ex_swapchain_proxy(swapchain, nullptr);

		m_swapchains.emplace(swapchain, proxy);
		*pSwapChain = reinterpret_cast<IDirect3DSwapChain9*>(proxy);
	}

	return hr;
}

HRESULT __stdcall d3d9_device_proxy::GetSwapChain(UINT iSwapChain, IDirect3DSwapChain9** pSwapChain)
{
	log("device: get swapchain %i\n", iSwapChain);

	IDirect3DSwapChain9* swapchain;
	auto hr = m_device->GetSwapChain(iSwapChain, &swapchain);

	if (SUCCEEDED(hr))
	{
		auto pair = m_swapchains.find(swapchain);
		d3d9ex_swapchain_proxy* proxy = nullptr;

		if (pair != m_swapchains.end())
		{
			proxy = pair->second;
		}
		else
		{
			// TODO
			proxy = new d3d9ex_swapchain_proxy(swapchain, nullptr);
			m_swapchains.emplace(swapchain, proxy);
		}

		*pSwapChain = reinterpret_cast<IDirect3DSwapChain9*>(proxy);
	}

	return hr;
}

UINT __stdcall d3d9_device_proxy::GetNumberOfSwapChains(void)
{
	return m_device->GetNumberOfSwapChains();
}

HRESULT __stdcall d3d9_device_proxy::Reset(D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	return m_device->Reset(pPresentationParameters);
}

HRESULT __stdcall d3d9_device_proxy::Present(const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion)
{
	do_fps_limit(&m_lastTime);

	return m_device->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}

HRESULT __stdcall d3d9_device_proxy::GetBackBuffer(UINT iSwapChain, UINT iBackBuffer, D3DBACKBUFFER_TYPE Type, IDirect3DSurface9** ppBackBuffer)
{
	return m_device->GetBackBuffer(iSwapChain, iBackBuffer, Type, ppBackBuffer);
}

HRESULT __stdcall d3d9_device_proxy::GetRasterStatus(UINT iSwapChain, D3DRASTER_STATUS* pRasterStatus)
{
	return m_device->GetRasterStatus(iSwapChain, pRasterStatus);
}

HRESULT __stdcall d3d9_device_proxy::SetDialogBoxMode(BOOL bEnableDialogs)
{
	return m_device->SetDialogBoxMode(bEnableDialogs);
}

void __stdcall d3d9_device_proxy::SetGammaRamp(UINT iSwapChain, DWORD Flags, const D3DGAMMARAMP* pRamp)
{
	return m_device->SetGammaRamp(iSwapChain, Flags, pRamp);
}

void __stdcall d3d9_device_proxy::GetGammaRamp(UINT iSwapChain, D3DGAMMARAMP* pRamp)
{
	return m_device->GetGammaRamp(iSwapChain, pRamp);
}

HRESULT __stdcall d3d9_device_proxy::CreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture9** ppTexture, HANDLE* pSharedHandle)
{
	return m_device->CreateTexture(Width, Height, Levels, Usage, Format, Pool, ppTexture, pSharedHandle);
}

HRESULT __stdcall d3d9_device_proxy::CreateVolumeTexture(UINT Width, UINT Height, UINT Depth, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DVolumeTexture9** ppVolumeTexture, HANDLE* pSharedHandle)
{
	return m_device->CreateVolumeTexture(Width, Height, Depth, Levels, Usage, Format, Pool, ppVolumeTexture, pSharedHandle);
}

HRESULT __stdcall d3d9_device_proxy::CreateCubeTexture(UINT EdgeLength, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DCubeTexture9** ppCubeTexture, HANDLE* pSharedHandle)
{
	return m_device->CreateCubeTexture(EdgeLength, Levels, Usage, Format, Pool, ppCubeTexture, pSharedHandle);
}

HRESULT __stdcall d3d9_device_proxy::CreateVertexBuffer(UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, IDirect3DVertexBuffer9** ppVertexBuffer, HANDLE* pSharedHandle)
{
	return m_device->CreateVertexBuffer(Length, Usage, FVF, Pool, ppVertexBuffer, pSharedHandle);
}

HRESULT __stdcall d3d9_device_proxy::CreateIndexBuffer(UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DIndexBuffer9** ppIndexBuffer, HANDLE* pSharedHandle)
{
	return m_device->CreateIndexBuffer(Length, Usage, Format, Pool, ppIndexBuffer, pSharedHandle);
}

HRESULT __stdcall d3d9_device_proxy::CreateRenderTarget(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle)
{
	return m_device->CreateRenderTarget(Width, Height, Format, MultiSample, MultisampleQuality, Lockable, ppSurface, pSharedHandle);
}

HRESULT __stdcall d3d9_device_proxy::CreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle)
{
	return m_device->CreateDepthStencilSurface(Width, Height, Format, MultiSample, MultisampleQuality, Discard, ppSurface, pSharedHandle);
}

HRESULT __stdcall d3d9_device_proxy::UpdateSurface(IDirect3DSurface9* pSourceSurface, const RECT* pSourceRect, IDirect3DSurface9* pDestinationSurface, const POINT* pDestPoint)
{
	return m_device->UpdateSurface(pSourceSurface, pSourceRect, pDestinationSurface, pDestPoint);
}

HRESULT __stdcall d3d9_device_proxy::UpdateTexture(IDirect3DBaseTexture9* pSourceTexture, IDirect3DBaseTexture9* pDestinationTexture)
{
	return m_device->UpdateTexture(pSourceTexture, pDestinationTexture);
}

HRESULT __stdcall d3d9_device_proxy::GetRenderTargetData(IDirect3DSurface9* pRenderTarget, IDirect3DSurface9* pDestSurface)
{
	return m_device->GetRenderTargetData(pRenderTarget, pDestSurface);
}

HRESULT __stdcall d3d9_device_proxy::GetFrontBufferData(UINT iSwapChain, IDirect3DSurface9* pDestSurface)
{
	return m_device->GetFrontBufferData(iSwapChain, pDestSurface);
}

HRESULT __stdcall d3d9_device_proxy::StretchRect(IDirect3DSurface9* pSourceSurface, const RECT* pSourceRect, IDirect3DSurface9* pDestSurface, const RECT* pDestRect, D3DTEXTUREFILTERTYPE Filter)
{
	return m_device->StretchRect(pSourceSurface, pSourceRect, pDestSurface, pDestRect, Filter);
}

HRESULT __stdcall d3d9_device_proxy::ColorFill(IDirect3DSurface9* pSurface, const RECT* pRect, D3DCOLOR color)
{
	return m_device->ColorFill(pSurface, pRect, color);
}

HRESULT __stdcall d3d9_device_proxy::CreateOffscreenPlainSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DPOOL Pool, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle)
{
	return m_device->CreateOffscreenPlainSurface(Width, Height, Format, Pool, ppSurface, pSharedHandle);
}

HRESULT __stdcall d3d9_device_proxy::SetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9* pRenderTarget)
{
	return m_device->SetRenderTarget(RenderTargetIndex, pRenderTarget);
}

HRESULT __stdcall d3d9_device_proxy::GetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9** ppRenderTarget)
{
	return m_device->GetRenderTarget(RenderTargetIndex, ppRenderTarget);
}

HRESULT __stdcall d3d9_device_proxy::SetDepthStencilSurface(IDirect3DSurface9* pNewZStencil)
{
	return m_device->SetDepthStencilSurface(pNewZStencil);
}

HRESULT __stdcall d3d9_device_proxy::GetDepthStencilSurface(IDirect3DSurface9** ppZStencilSurface)
{
	return m_device->GetDepthStencilSurface(ppZStencilSurface);
}

HRESULT __stdcall d3d9_device_proxy::BeginScene(void)
{
	return m_device->BeginScene();
}

HRESULT __stdcall d3d9_device_proxy::EndScene(void)
{
	return m_device->EndScene();
}

HRESULT __stdcall d3d9_device_proxy::Clear(DWORD Count, const D3DRECT* pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil)
{
	return m_device->Clear(Count, pRects, Flags, Color, Z, Stencil);
}

HRESULT __stdcall d3d9_device_proxy::SetTransform(D3DTRANSFORMSTATETYPE State, const D3DMATRIX* pMatrix)
{
	return m_device->SetTransform(State, pMatrix);
}

HRESULT __stdcall d3d9_device_proxy::GetTransform(D3DTRANSFORMSTATETYPE State, D3DMATRIX* pMatrix)
{
	return m_device->GetTransform(State, pMatrix);
}

HRESULT __stdcall d3d9_device_proxy::MultiplyTransform(D3DTRANSFORMSTATETYPE Type, const D3DMATRIX* Matrix)
{
	return m_device->MultiplyTransform(Type, Matrix);
}

HRESULT __stdcall d3d9_device_proxy::SetViewport(const D3DVIEWPORT9* pViewport)
{
	return m_device->SetViewport(pViewport);
}

HRESULT __stdcall d3d9_device_proxy::GetViewport(D3DVIEWPORT9* pViewport)
{
	return m_device->GetViewport(pViewport);
}

HRESULT __stdcall d3d9_device_proxy::SetMaterial(const D3DMATERIAL9* pMaterial)
{
	return m_device->SetMaterial(pMaterial);
}

HRESULT __stdcall d3d9_device_proxy::GetMaterial(D3DMATERIAL9* pMaterial)
{
	return m_device->GetMaterial(pMaterial);
}

HRESULT __stdcall d3d9_device_proxy::SetLight(DWORD Index, const D3DLIGHT9* Light)
{
	return m_device->SetLight(Index, Light);
}

HRESULT __stdcall d3d9_device_proxy::GetLight(DWORD Index, D3DLIGHT9* Light)
{
	return m_device->GetLight(Index, Light);
}

HRESULT __stdcall d3d9_device_proxy::LightEnable(DWORD Index, BOOL Enable)
{
	return m_device->LightEnable(Index, Enable);
}

HRESULT __stdcall d3d9_device_proxy::GetLightEnable(DWORD Index, BOOL* pEnable)
{
	return m_device->GetLightEnable(Index, pEnable);
}

HRESULT __stdcall d3d9_device_proxy::SetClipPlane(DWORD Index, const float* pPlane)
{
	return m_device->SetClipPlane(Index, pPlane);
}

HRESULT __stdcall d3d9_device_proxy::GetClipPlane(DWORD Index, float* pPlane)
{
	return m_device->GetClipPlane(Index, pPlane);
}

HRESULT __stdcall d3d9_device_proxy::SetRenderState(D3DRENDERSTATETYPE State, DWORD Value)
{
	return m_device->SetRenderState(State, Value);
}

HRESULT __stdcall d3d9_device_proxy::GetRenderState(D3DRENDERSTATETYPE State, DWORD* pValue)
{
	return m_device->GetRenderState(State, pValue);
}

HRESULT __stdcall d3d9_device_proxy::CreateStateBlock(D3DSTATEBLOCKTYPE Type, IDirect3DStateBlock9** ppSB)
{
	return m_device->CreateStateBlock(Type, ppSB);
}

HRESULT __stdcall d3d9_device_proxy::BeginStateBlock(void)
{
	return m_device->BeginStateBlock();
}

HRESULT __stdcall d3d9_device_proxy::EndStateBlock(IDirect3DStateBlock9** ppSB)
{
	return m_device->EndStateBlock(ppSB);
}

HRESULT __stdcall d3d9_device_proxy::SetClipStatus(const D3DCLIPSTATUS9* pClipStatus)
{
	return m_device->SetClipStatus(pClipStatus);
}

HRESULT __stdcall d3d9_device_proxy::GetClipStatus(D3DCLIPSTATUS9* pClipStatus)
{
	return m_device->GetClipStatus(pClipStatus);
}

HRESULT __stdcall d3d9_device_proxy::GetTexture(DWORD Stage, IDirect3DBaseTexture9** ppTexture)
{
	return m_device->GetTexture(Stage, ppTexture);
}

HRESULT __stdcall d3d9_device_proxy::SetTexture(DWORD Stage, IDirect3DBaseTexture9* pTexture)
{
	return m_device->SetTexture(Stage, pTexture);
}

HRESULT __stdcall d3d9_device_proxy::GetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD* pValue)
{
	return m_device->GetTextureStageState(Stage, Type, pValue);
}

HRESULT __stdcall d3d9_device_proxy::SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value)
{
	return m_device->SetTextureStageState(Stage, Type, Value);
}

HRESULT __stdcall d3d9_device_proxy::GetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD* pValue)
{
	return m_device->GetSamplerState(Sampler, Type, pValue);
}

HRESULT __stdcall d3d9_device_proxy::SetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value)
{
	return m_device->SetSamplerState(Sampler, Type, Value);
}

HRESULT __stdcall d3d9_device_proxy::ValidateDevice(DWORD* pNumPasses)
{
	return m_device->ValidateDevice(pNumPasses);
}

HRESULT __stdcall d3d9_device_proxy::SetPaletteEntries(UINT PaletteNumber, const PALETTEENTRY* pEntries)
{
	return m_device->SetPaletteEntries(PaletteNumber, pEntries);
}

HRESULT __stdcall d3d9_device_proxy::GetPaletteEntries(UINT PaletteNumber, PALETTEENTRY* pEntries)
{
	return m_device->GetPaletteEntries(PaletteNumber, pEntries);
}

HRESULT __stdcall d3d9_device_proxy::SetCurrentTexturePalette(UINT PaletteNumber)
{
	return m_device->SetCurrentTexturePalette(PaletteNumber);
}

HRESULT __stdcall d3d9_device_proxy::GetCurrentTexturePalette(UINT* PaletteNumber)
{
	return m_device->GetCurrentTexturePalette(PaletteNumber);
}

HRESULT __stdcall d3d9_device_proxy::SetScissorRect(const RECT* pRect)
{
	return m_device->SetScissorRect(pRect);
}

HRESULT __stdcall d3d9_device_proxy::GetScissorRect(RECT* pRect)
{
	return m_device->GetScissorRect(pRect);
}

HRESULT __stdcall d3d9_device_proxy::SetSoftwareVertexProcessing(BOOL bSoftware)
{
	return m_device->SetSoftwareVertexProcessing(bSoftware);
}

BOOL __stdcall d3d9_device_proxy::GetSoftwareVertexProcessing(void)
{
	return m_device->GetSoftwareVertexProcessing();
}

HRESULT __stdcall d3d9_device_proxy::SetNPatchMode(float nSegments)
{
	return m_device->SetNPatchMode(nSegments);
}

float __stdcall d3d9_device_proxy::GetNPatchMode(void)
{
	return m_device->GetNPatchMode();
}

HRESULT __stdcall d3d9_device_proxy::DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount)
{
	return m_device->DrawPrimitive(PrimitiveType, StartVertex, PrimitiveCount);
}

HRESULT __stdcall d3d9_device_proxy::DrawIndexedPrimitive(D3DPRIMITIVETYPE PrimitiveType, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount)
{
	return m_device->DrawIndexedPrimitive(PrimitiveType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
}

HRESULT __stdcall d3d9_device_proxy::DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, const void* pVertexStreamZeroData, UINT VertexStreamZeroStride)
{
	return m_device->DrawPrimitiveUP(PrimitiveType, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride);
}

HRESULT __stdcall d3d9_device_proxy::DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertices, UINT PrimitiveCount, const void* pIndexData, D3DFORMAT IndexDataFormat, const void* pVertexStreamZeroData, UINT VertexStreamZeroStride)
{
	return m_device->DrawIndexedPrimitiveUP(PrimitiveType, MinVertexIndex, NumVertices, PrimitiveCount, pIndexData, IndexDataFormat, pVertexStreamZeroData, VertexStreamZeroStride);
}

HRESULT __stdcall d3d9_device_proxy::ProcessVertices(UINT SrcStartIndex, UINT DestIndex, UINT VertexCount, IDirect3DVertexBuffer9* pDestBuffer, IDirect3DVertexDeclaration9* pVertexDecl, DWORD Flags)
{
	return m_device->ProcessVertices(SrcStartIndex, DestIndex, VertexCount, pDestBuffer, pVertexDecl, Flags);
}

HRESULT __stdcall d3d9_device_proxy::CreateVertexDeclaration(const D3DVERTEXELEMENT9* pVertexElements, IDirect3DVertexDeclaration9** ppDecl)
{
	return m_device->CreateVertexDeclaration(pVertexElements, ppDecl);
}

HRESULT __stdcall d3d9_device_proxy::SetVertexDeclaration(IDirect3DVertexDeclaration9* pDecl)
{
	return m_device->SetVertexDeclaration(pDecl);
}

HRESULT __stdcall d3d9_device_proxy::GetVertexDeclaration(IDirect3DVertexDeclaration9** ppDecl)
{
	return m_device->GetVertexDeclaration(ppDecl);
}

HRESULT __stdcall d3d9_device_proxy::SetFVF(DWORD FVF)
{
	return m_device->SetFVF(FVF);
}

HRESULT __stdcall d3d9_device_proxy::GetFVF(DWORD* pFVF)
{
	return m_device->GetFVF(pFVF);
}

HRESULT __stdcall d3d9_device_proxy::CreateVertexShader(const DWORD* pFunction, IDirect3DVertexShader9** ppShader)
{
	return m_device->CreateVertexShader(pFunction, ppShader);
}

HRESULT __stdcall d3d9_device_proxy::SetVertexShader(IDirect3DVertexShader9* pShader)
{
	return m_device->SetVertexShader(pShader);
}

HRESULT __stdcall d3d9_device_proxy::GetVertexShader(IDirect3DVertexShader9** ppShader)
{
	return m_device->GetVertexShader(ppShader);
}

HRESULT __stdcall d3d9_device_proxy::SetVertexShaderConstantF(UINT StartRegister, const float* pConstantData, UINT Vector4fCount)
{
	return m_device->SetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount);
}

HRESULT __stdcall d3d9_device_proxy::GetVertexShaderConstantF(UINT StartRegister, float* pConstantData, UINT Vector4fCount)
{
	return m_device->SetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount);
}

HRESULT __stdcall d3d9_device_proxy::SetVertexShaderConstantI(UINT StartRegister, const int* pConstantData, UINT Vector4iCount)
{
	return m_device->SetVertexShaderConstantI(StartRegister, pConstantData, Vector4iCount);
}

HRESULT __stdcall d3d9_device_proxy::GetVertexShaderConstantI(UINT StartRegister, int* pConstantData, UINT Vector4iCount)
{
	return m_device->GetVertexShaderConstantI(StartRegister, pConstantData, Vector4iCount);
}

HRESULT __stdcall d3d9_device_proxy::SetVertexShaderConstantB(UINT StartRegister, const BOOL* pConstantData, UINT BoolCount)
{
	return m_device->SetVertexShaderConstantB(StartRegister, pConstantData, BoolCount);
}

HRESULT __stdcall d3d9_device_proxy::GetVertexShaderConstantB(UINT StartRegister, BOOL* pConstantData, UINT BoolCount)
{
	return m_device->GetVertexShaderConstantB(StartRegister, pConstantData, BoolCount);
}

HRESULT __stdcall d3d9_device_proxy::SetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9* pStreamData, UINT OffsetInBytes, UINT Stride)
{
	return m_device->SetStreamSource(StreamNumber, pStreamData, OffsetInBytes, Stride);
}

HRESULT __stdcall d3d9_device_proxy::GetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9** ppStreamData, UINT* pOffsetInBytes, UINT* pStride)
{
	return m_device->GetStreamSource(StreamNumber, ppStreamData, pOffsetInBytes, pStride);
}

HRESULT __stdcall d3d9_device_proxy::SetStreamSourceFreq(UINT StreamNumber, UINT Setting)
{
	return m_device->SetStreamSourceFreq(StreamNumber, Setting);
}

HRESULT __stdcall d3d9_device_proxy::GetStreamSourceFreq(UINT StreamNumber, UINT* pSetting)
{
	return m_device->GetStreamSourceFreq(StreamNumber, pSetting);
}

HRESULT __stdcall d3d9_device_proxy::SetIndices(IDirect3DIndexBuffer9* pIndexData)
{
	return m_device->SetIndices(pIndexData);
}

HRESULT __stdcall d3d9_device_proxy::GetIndices(IDirect3DIndexBuffer9** ppIndexData)
{
	return m_device->GetIndices(ppIndexData);
}

HRESULT __stdcall d3d9_device_proxy::CreatePixelShader(const DWORD* pFunction, IDirect3DPixelShader9** ppShader)
{
	return m_device->CreatePixelShader(pFunction, ppShader);
}

HRESULT __stdcall d3d9_device_proxy::SetPixelShader(IDirect3DPixelShader9* pShader)
{
	return m_device->SetPixelShader(pShader);
}

HRESULT __stdcall d3d9_device_proxy::GetPixelShader(IDirect3DPixelShader9** ppShader)
{
	return m_device->GetPixelShader(ppShader);
}

HRESULT __stdcall d3d9_device_proxy::SetPixelShaderConstantF(UINT StartRegister, const float* pConstantData, UINT Vector4fCount)
{
	return m_device->SetPixelShaderConstantF(StartRegister, pConstantData, Vector4fCount);
}

HRESULT __stdcall d3d9_device_proxy::GetPixelShaderConstantF(UINT StartRegister, float* pConstantData, UINT Vector4fCount)
{
	return m_device->GetPixelShaderConstantF(StartRegister, pConstantData, Vector4fCount);
}

HRESULT __stdcall d3d9_device_proxy::SetPixelShaderConstantI(UINT StartRegister, const int* pConstantData, UINT Vector4iCount)
{
	return m_device->SetPixelShaderConstantI(StartRegister, pConstantData, Vector4iCount);
}

HRESULT __stdcall d3d9_device_proxy::GetPixelShaderConstantI(UINT StartRegister, int* pConstantData, UINT Vector4iCount)
{
	return m_device->GetPixelShaderConstantI(StartRegister, pConstantData, Vector4iCount);
}

HRESULT __stdcall d3d9_device_proxy::SetPixelShaderConstantB(UINT StartRegister, const BOOL* pConstantData, UINT BoolCount)
{
	return m_device->SetPixelShaderConstantB(StartRegister, pConstantData, BoolCount);
}

HRESULT __stdcall d3d9_device_proxy::GetPixelShaderConstantB(UINT StartRegister, BOOL* pConstantData, UINT BoolCount)
{
	return m_device->GetPixelShaderConstantB(StartRegister, pConstantData, BoolCount);
}

HRESULT __stdcall d3d9_device_proxy::DrawRectPatch(UINT Handle, const float* pNumSegs, const D3DRECTPATCH_INFO* pRectPatchInfo)
{
	return m_device->DrawRectPatch(Handle, pNumSegs, pRectPatchInfo);
}

HRESULT __stdcall d3d9_device_proxy::DrawTriPatch(UINT Handle, const float* pNumSegs, const D3DTRIPATCH_INFO* pTriPatchInfo)
{
	return m_device->DrawTriPatch(Handle, pNumSegs, pTriPatchInfo);
}

HRESULT __stdcall d3d9_device_proxy::DeletePatch(UINT Handle)
{
	return m_device->DeletePatch(Handle);
}

HRESULT __stdcall d3d9_device_proxy::CreateQuery(D3DQUERYTYPE Type, IDirect3DQuery9** ppQuery)
{
	return m_device->CreateQuery(Type, ppQuery);
}
