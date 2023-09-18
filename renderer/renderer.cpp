#include "stdinc.hpp"

struct VERTEX { float x, y, z; };

#define HR_ASSERT(msg) if(FAILED(hr)) \
{ \
	log("renderer assert: %s, hr=%x.\nfile=%s func=%s line=%d\n", msg, hr, __FILE__, __FUNCTION__, __LINE__); \
	return; \
}

// #define HR_ASSERT(msg) 0

VERTEX vertices[] =
{
	{-1.0f, 1.0f, 0.0f},
	{3.0f, 1.0f, 0.0f},
	{-1.0f, -3.0f, 0.0f},
};

IDirect3DDevice9* renderer::device = nullptr;
HWND renderer::window_handle = nullptr;

renderer::renderer(float width, float height)
{
	screen_size[0] = width;
	screen_size[1] = height;
}

renderer::~renderer()
{
	self_swapchain->Release();

	CloseWindow(window_handle);
	CloseHandle(window_handle);
}

void renderer::init(IDirect3DDevice9* device)
{
	create_window();

	D3DPRESENT_PARAMETERS params;
	ZeroMemory(&params, sizeof(D3DPRESENT_PARAMETERS));

	params.BackBufferWidth = 0;
	params.BackBufferHeight = 0;
	params.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	params.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	params.Windowed = true;

	params.hDeviceWindow = window_handle;

	params.BackBufferCount = 1;
	params.BackBufferFormat = D3DFMT_X8R8G8B8;
	params.SwapEffect = D3DSWAPEFFECT_COPY;

	auto hr = device->CreateAdditionalSwapChain(&params, &self_swapchain);
	HR_ASSERT("failed to create swapchain");

	hr = device->CreateVertexBuffer(_countof(vertices) * sizeof(VERTEX), D3DUSAGE_DYNAMIC, D3DFVF_XYZ, D3DPOOL_DEFAULT, &vertex_buffer, nullptr);
	HR_ASSERT("failed to create vertex buffer");

	void* tmp_ptr;

	hr = vertex_buffer->Lock(0, 0, &tmp_ptr, 0);
	HR_ASSERT("failed to lock vertex buffer");

	memcpy(tmp_ptr, vertices, _countof(vertices) * sizeof(VERTEX));

	hr = vertex_buffer->Unlock();
	HR_ASSERT("failed to unlock vertex buffer");

	hr = device->GetSwapChain(0, &game_swapchain);
	HR_ASSERT("failed to get game swapchain");

	IDirect3DSurface9* backbuffer;
	hr = game_swapchain->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &backbuffer);
	HR_ASSERT("failed to get game backbuffer");

	D3DSURFACE_DESC desc;
	hr = backbuffer->GetDesc(&desc);
	HR_ASSERT("failed to get game backbuffer desc");

	for (size_t i = 0; i < frames.size(); i++)
	{
		hr = device->CreateTexture(desc.Width, desc.Height, 1, desc.Usage, desc.Format, desc.Pool, frames.data() + i, nullptr);
		HR_ASSERT("failed to create frame cache");
	}

	backbuffer->Release();

	ID3DBlob* blob, * err;
	hr = D3DCompileFromFile(L"./shaders/vs.hlsl", nullptr, nullptr, "main", "vs_3_0", 0, 0, &blob, &err);

	if (err)
	{
		auto error = err->GetBufferPointer();
		log("d3d compiler (vs): \n------------\n%s------------\n", error);
	}

	HR_ASSERT("failed to compile vertex shader");

	hr = device->CreateVertexShader(reinterpret_cast<const DWORD*>(blob->GetBufferPointer()), &vertex_shader);
	HR_ASSERT("failed to create vertex shader");

	if (blob) blob->Release();
	if (err) err->Release();

	hr = D3DCompileFromFile(L"./shaders/ps.hlsl", nullptr, nullptr, "main", "ps_3_0", 0, 0, &blob, &err);

	if (err)
	{
		auto error = err->GetBufferPointer();
		log("d3d compiler (ps): \n------------\n%s------------\n", error);
	}

	HR_ASSERT("failed to compile pixel shader");

	hr = device->CreatePixelShader(reinterpret_cast<const DWORD*>(blob->GetBufferPointer()), &pixel_shader);
	HR_ASSERT("failed to create pixel shader");

	if (blob) blob->Release();
	if (err) err->Release();

	ShowWindow(window_handle, SW_SHOW);

	this->device = device;
}

void renderer::create_window()
{
	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = wndproc;
	wc.hInstance = g_instance;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW);
	wc.lpszClassName = L"V120Win";

	RegisterClassExW(&wc);

	RECT wr = { 0, 0, static_cast<LONG>(screen_size[0]), static_cast<LONG>(screen_size[1]) };
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

	window_handle = CreateWindowEx(
		NULL,
		L"V120Win",
		L"Virtual Monitor View",
		WS_VISIBLE | WS_POPUP,
		0, 0,
		wr.right - wr.left, wr.bottom - wr.top,
		nullptr, nullptr, g_instance, nullptr
	);
}

auto renderer::backup_params()
{
	IDirect3DSurface9* rt;
	DWORD fvf;
	IDirect3DVertexBuffer9* vb;
	UINT vb_offset;
	UINT vb_stride;
	float shader_constant[] = { 0, 0, 0, 0 };

	IDirect3DBaseTexture9 *t1, *t2;

	IDirect3DVertexShader9* vs;
	IDirect3DPixelShader9* ps;

	device->GetRenderTarget(0, &rt);
	device->GetFVF(&fvf);
	device->GetStreamSource(0, &vb, &vb_offset, &vb_stride);
	device->GetPixelShaderConstantF(0, shader_constant, 1);
	device->GetTexture(0, &t1);
	device->GetTexture(1, &t2);
	device->GetVertexShader(&vs);
	device->GetPixelShader(&ps);

	return [=] {
		device->SetRenderTarget(0, rt);
		device->SetFVF(fvf);
		device->SetStreamSource(0, vb, vb_offset, vb_stride);
		device->SetPixelShaderConstantF(0, shader_constant, 1);
		device->SetTexture(0, t1);
		device->SetTexture(1, t2);
		device->SetVertexShader(vs);
		device->SetPixelShader(ps);

		if (rt) rt->Release();
		if (vb) vb->Release();
		if (t1) t1->Release();
		if (t2) t2->Release();
		if (vs) vs->Release();
		if (ps) ps->Release();
	};
}

void renderer::render()
{
	auto restore = backup_params();

	D3DCOLOR clear_color = D3DCOLOR_XRGB(45, 45, 45);

	IDirect3DSurface9* rt;
	auto hr = self_swapchain->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &rt);
	HR_ASSERT("failed to get backbuffer");

	hr = device->SetRenderTarget(0, rt);
	HR_ASSERT("failed to get backbuffer");

	hr = device->Clear(0, nullptr, D3DCLEAR_TARGET, clear_color, 1.0f, 0);
	HR_ASSERT("failed to clear");
	hr = device->BeginScene();
	HR_ASSERT("failed to begin scene");

	hr = device->SetFVF(D3DFVF_XYZ);
	HR_ASSERT("failed to set fvf");
	hr = device->SetStreamSource(0, vertex_buffer, 0, sizeof(VERTEX));
	HR_ASSERT("failed to set stream source");

	float screen_size_f[] = { screen_size[0], screen_size[1], 0, 0 };
	hr = device->SetPixelShaderConstantF(0, screen_size_f, 1);
	HR_ASSERT("failed to set pixel shader constant float");

	device->SetTexture(0, frames[0]);
	device->SetTexture(1, frames[1]);

	hr = device->SetVertexShader(vertex_shader);
	HR_ASSERT("failed to set vertex shader");

	hr = device->SetPixelShader(pixel_shader);
	HR_ASSERT("failed to set pixel shader");

	hr = device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);
	HR_ASSERT("failed to draw");

	hr = device->EndScene();
	HR_ASSERT("failed to end scene");
	hr = self_swapchain->Present(nullptr, nullptr, nullptr, nullptr, 0);
	HR_ASSERT("failed to present");

	rt->Release();

	// restore all params
	restore();

	frame_count = 0;
}

void renderer::queue_frame()
{
	auto target = frames[frame_count++];

	IDirect3DSurface9* buffer, * target_suf;
	auto hr = target->GetSurfaceLevel(0, &target_suf);
	HR_ASSERT("failed to get surface level");

	hr = game_swapchain->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &buffer);
	HR_ASSERT("failed to get backbuffer");

	hr = device->StretchRect(buffer, nullptr, target_suf, nullptr, D3DTEXF_NONE);
	HR_ASSERT("failed to copy backbuffer");

	if (frame_count >= 2)
	{
		loop();
		render();
	}

	buffer->Release();
}

void renderer::loop()
{
	MSG msg = {};

	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

LRESULT renderer::wndproc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}
	case WM_ACTIVATE:
	{
		if (!device) break;

		D3DDEVICE_CREATION_PARAMETERS params;
		device->GetCreationParameters(&params);

		SetForegroundWindow(params.hFocusWindow);
		SetWindowPos(window_handle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);
		return 0;
	}
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}
