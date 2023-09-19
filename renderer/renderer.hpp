class renderer
{
public:
	renderer(float width, float height);
	~renderer();

	void init(IDirect3DDevice9* d3d9);
	void loop();
	void queue_frame();

private:
	void create_window();
	auto backup_params();
	void render();
	static LRESULT CALLBACK wndproc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	std::array<float, 4> screen_size;

	static HWND window_handle;
	static IDirect3DDevice9* device;

	IDirect3DSwapChain9* self_swapchain = nullptr;
	IDirect3DSwapChain9* game_swapchain = nullptr;

	IDirect3DVertexBuffer9* vertex_buffer;

	IDirect3DVertexShader9* vertex_shader;
	IDirect3DPixelShader9* pixel_shader;

	int frame_count = 0;
	std::array<IDirect3DTexture9*, 2> frames;
};
