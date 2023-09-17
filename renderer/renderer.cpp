#include "stdinc.hpp"

struct VERTEX { float X, Y, Z; float Color[4]; };

VERTEX vertices[] =
{
	{-1.0f, 1.0f, 0.0f, { 1.0f, 0.0f, 0.0f, 1.0f }},
	{3.0f, 1.0f, 0.0f, { 0.0f, 1.0f, 0.0f, 1.0f }},
	{-1.0f, -3.0f, 0.0f, { 0.0f, 0.0f, 1.0f, 1.0f }},
};

renderer::renderer(float width, float height)
{
	screen_size[0] = width;
	screen_size[1] = height;
}

renderer::~renderer()
{
	swapchain->SetFullscreenState(false, nullptr);

	SendMessage(window_handle, WM_CLOSE, 0, 0);
	CloseHandle(window_handle);

	vs->Release();
	ps->Release();
	swapchain->Release();
	queue_9to11->Release();
	surface_consumer->Release();
	surface_producer->Release();
	backbuffer->Release();
	device->Release();
	device_ctx->Release();
}

void renderer::init()
{
	create_window();

	UINT createDeviceFlags = D3D11_CREATE_DEVICE_DEBUG;
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1,
	};

	D3D_FEATURE_LEVEL featureLevel;

	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	scd.BufferCount = 1;
	scd.BufferDesc.Width = static_cast<UINT>(screen_size[0]);
	scd.BufferDesc.Height = static_cast<UINT>(screen_size[1]);
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.RefreshRate.Numerator = 60;
	scd.BufferDesc.RefreshRate.Denominator = 0;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow = window_handle;
	scd.SampleDesc.Count = 1;
	scd.SampleDesc.Quality = 0;
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	scd.Windowed = TRUE;
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	auto hr = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr, createDeviceFlags, featureLevels, _countof(featureLevels), D3D11_SDK_VERSION,
		&scd, &swapchain, &device, &featureLevel, &device_ctx
	);

	if (FAILED(hr))
	{
		log("renderer: failed to initialize d3d11, hr = %x\n", hr);
		return;
	}

	ID3D11Texture2D* backbuffer_tex = nullptr;
	hr = swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backbuffer_tex));

	if (FAILED(hr) || !backbuffer_tex)
	{
		log("renderer: failed to get backbuffer texture\n");
		return;
	}

	hr = device->CreateRenderTargetView(backbuffer_tex, nullptr, &backbuffer);

	if (FAILED(hr))
	{
		log("renderer: failed to create rt\n");
		return;
	}

	backbuffer_tex->Release();

	device_ctx->OMSetRenderTargets(1, &backbuffer, nullptr);

	D3D11_VIEWPORT vp = {};
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width = 1920;
	vp.Height = 1080;

	device_ctx->RSSetViewports(1, &vp);

	log("renderer: inited with feature level %x\n", featureLevel);

	ID3D10Blob* VS, * PS, * err;

	hr = D3DCompileFromFile(L"./shaders/vs.hlsl", nullptr, nullptr, "vs_main", "vs_4_0", 0, 0, &VS, &err);

	if (err)
	{
		auto error = err->GetBufferPointer();
		log("d3d compiler (vs): \n------------\n%s------------\n", error);
	}

	if (FAILED(hr))
	{
		log("renderer: failed to compile vertex shader.\n");

		return;
	}

	hr = D3DCompileFromFile(L"./shaders/ps.hlsl", nullptr, nullptr, "ps_main", "ps_4_0", 0, 0, &PS, &err);

	if (err)
	{
		auto error = err->GetBufferPointer();
		log("d3d compiler (ps): \n------------\n%s------------\n", error);
	}

	if (FAILED(hr))
	{
		log("renderer: failed to compile pixel shader.\n");

		return;
	}

	hr = device->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), nullptr, &vs);

	if (FAILED(hr))
	{
		log("renderer: failed to create vertex shader.\n");

		return;
	}

	hr = device->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), nullptr, &ps);

	if (FAILED(hr))
	{
		log("renderer: failed to create pixel shader.\n");

		return;
	}

	device_ctx->VSSetShader(vs, nullptr, 0);
	device_ctx->PSSetShader(ps, nullptr, 0);

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX) * _countof(vertices);
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	hr = device->CreateBuffer(&bd, NULL, &vertex_buffer);

	if (FAILED(hr) || !vertex_buffer)
	{
		log("renderer: failed to create vertex buffer.\n");

		return;
	}

	// copy the vertices into the buffer
	D3D11_MAPPED_SUBRESOURCE ms;
	hr = device_ctx->Map(vertex_buffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);

	if (FAILED(hr))
	{
		log("renderer: failed to map vertex buffer.\n");

		return;
	}

	memcpy(ms.pData, vertices, sizeof(vertices));
	device_ctx->Unmap(vertex_buffer, NULL);

	D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	hr = device->CreateInputLayout(ied, 2, VS->GetBufferPointer(), VS->GetBufferSize(), &layout);

	if (FAILED(hr))
	{
		log("renderer: failed to create input layout.\n");

		return;
	}

	device_ctx->IASetInputLayout(layout);

	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = 16;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	hr = device->CreateBuffer(&bd, NULL, &ps_constant_buffer);

	if (FAILED(hr) || !ps_constant_buffer)
	{
		log("renderer: failed to create vs constant buffer.\n");

		return;
	}

	hr = device_ctx->Map(ps_constant_buffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);

	if (FAILED(hr))
	{
		log("renderer: failed to map vs constant buffer.\n");

		return;
	}

	memcpy(ms.pData, screen_size.data(), screen_size.size() * sizeof(uint32_t));
	device_ctx->Unmap(ps_constant_buffer, NULL);

	device_ctx->PSSetConstantBuffers(0, 1, &ps_constant_buffer);

	D3D11_SAMPLER_DESC sd = {};
	sd.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	sd.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	sd.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	sd.BorderColor[0] = 1.0f;
	sd.BorderColor[1] = 1.0f;
	sd.BorderColor[2] = 1.0f;
	sd.BorderColor[3] = 1.0f;
	sd.ComparisonFunc = D3D11_COMPARISON_NEVER;

	device->CreateSamplerState(&sd, &sampler_state);
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
		WS_OVERLAPPEDWINDOW,
		300, 300,
		wr.right - wr.left, wr.bottom - wr.top,
		nullptr, nullptr, g_instance, nullptr
	);
}

void renderer::render()
{
	float clear_color[] = { 0.2f, 0.2f, 0.2f, 1.0f };

	device_ctx->ClearRenderTargetView(backbuffer, clear_color);

	ID3D11Texture2D* texture = NULL;
	
	while (SUCCEEDED(surface_consumer->Dequeue(_uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&texture), nullptr, 0, 0)))
	{
		ID3D11ShaderResourceView* sv = nullptr;

		device->CreateShaderResourceView(texture, nullptr, &sv);
		device_ctx->PSSetShaderResources(0, 1, &sv);
		device_ctx->PSSetSamplers(0, 1, &sampler_state);

		UINT stride = sizeof(VERTEX);
		UINT offset = 0;

		device_ctx->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);
		device_ctx->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		device_ctx->Draw(_countof(vertices), 0);

		EnterCriticalSection(&queue_lock);
		pending_surfaces--;
		LeaveCriticalSection(&queue_lock);
	}

	swapchain->Present(1, 0);
}

void renderer::create_surface_queue(IDirect3DDevice9Ex* d3d9_device)
{
	SURFACE_QUEUE_DESC Desc;
	Desc.Width = static_cast<UINT>(screen_size[0]);
	Desc.Height = static_cast<UINT>(screen_size[1]);
	Desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	Desc.NumSurfaces = 3;
	Desc.MetaDataSize = 0;
	Desc.Flags = 0;

	auto hr = CreateSurfaceQueue(&Desc, device, &queue_9to11);

	if (FAILED(hr))
	{
		log("renderer: failed to create surface queue. hr=%x\n", hr);
		return;
	}

	hr = queue_9to11->OpenProducer(d3d9_device, &surface_producer);

	if (FAILED(hr))
	{
		log("renderer: failed to open producer. hr=%x\n", hr);
		return;
	}

	hr = queue_9to11->OpenConsumer(device, &surface_consumer);

	if (FAILED(hr))
	{
		log("renderer: failed to open consumer. hr=%x\n", hr);
		return;
	}

	this->d3d9_device = d3d9_device;
	InitializeCriticalSection(&queue_lock);
}

void renderer::queue_frame()
{
	IDirect3DSurface9* buffer = nullptr;
	auto hr = d3d9_device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &buffer);

	if (FAILED(hr))
	{
		log("renderer: failed to get dx9 backbuffer. hr=%x\n", hr);
		return;
	}

	D3DSURFACE_DESC desc{};
	hr = buffer->GetDesc(&desc);

	if (FAILED(hr))
	{
		log("renderer: failed to get backbuffer surface desc. hr=%x\n", hr);
		buffer->Release();
		return;
	}

	if (!copy_surface)
	{
		hr = d3d9_device->CreateRenderTarget(
			desc.Width, desc.Height, desc.Format, desc.MultiSampleType,
			desc.MultiSampleQuality, TRUE, &copy_surface, nullptr);

		if (FAILED(hr) || copy_surface == nullptr) {
			log("renderer: failed to create render target. hr=%x\n", hr);
			buffer->Release();
			return;
		}
	}

	hr = d3d9_device->StretchRect(buffer, nullptr, copy_surface, nullptr, D3DTEXF_NONE);
	if (FAILED(hr)) {
		log("renderer: failed to copy backbuffer. hr=%x\n", hr);
		buffer->Release();
		return;
	}

	EnterCriticalSection(&queue_lock);
	hr = surface_producer->Enqueue(copy_surface, nullptr, 0, 0);
	if (FAILED(hr)) {
		log("renderer: failed to enqueue buffer. hr=%x\n", hr);
		buffer->Release();
		LeaveCriticalSection(&queue_lock);
		return;
	}

	pending_surfaces++;
	LeaveCriticalSection(&queue_lock);
}

void renderer::start()
{
	std::thread([=] {
		ShowWindow(window_handle, SW_SHOW);

		MSG msg = {};

		while (true)
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);

				if (msg.message == WM_QUIT)
					break;

				continue;
			}

			render();
		}
		}).detach();
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
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}
