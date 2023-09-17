class renderer
{
public:
	renderer(float width, float height);
	~renderer();

	void init();
	void start();
	void create_surface_queue(IDirect3DDevice9Ex* d3d9_device);
	void queue_frame();

private:
	void create_window();
	void render();
	static LRESULT CALLBACK wndproc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	HWND window_handle = nullptr;

	IDXGISwapChain* swapchain = nullptr;
	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* device_ctx = nullptr;

	ID3D11RenderTargetView* backbuffer = nullptr;

	ID3D11VertexShader* vs = nullptr;
	ID3D11PixelShader* ps = nullptr;

	ID3D11Buffer* vertex_buffer = nullptr;
	ID3D11Buffer* ps_constant_buffer = nullptr;
	ID3D11InputLayout* layout = nullptr;

	std::array<float, 2> screen_size;

	IDirect3DDevice9Ex* d3d9_device = nullptr;
	ISurfaceQueue* queue_9to11 = nullptr;
	ISurfaceProducer* surface_producer = nullptr;
	ISurfaceConsumer* surface_consumer = nullptr;
	size_t pending_surfaces = 0;

	ID3D11SamplerState* sampler_state = nullptr;

	CRITICAL_SECTION queue_lock;
	IDirect3DSurface9* copy_surface = nullptr;
};
