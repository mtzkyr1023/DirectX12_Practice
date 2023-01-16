#include "swapchain.h"

bool Swapchain::create(ID3D12CommandQueue* queue, HWND hwnd, UINT backBufferCount, UINT width, UINT height, bool vsync) {
	HRESULT res;

	Microsoft::WRL::ComPtr<IDXGIFactory4> factory;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
	res = CreateDXGIFactory1(IID_PPV_ARGS(factory.ReleaseAndGetAddressOf()));
	if (FAILED(res)) {
		MessageBox(NULL, "failed creating dxgi factory 4.", "Error", MB_OK);
		return false;
	}

	DXGI_SWAP_CHAIN_DESC scDesc{};
	scDesc.BufferCount = backBufferCount;
	scDesc.BufferDesc.Width = width;
	scDesc.BufferDesc.Height = height;
	scDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	scDesc.OutputWindow = hwnd;
	scDesc.SampleDesc.Count = 1;
	if (vsync) {
		scDesc.BufferDesc.RefreshRate.Numerator = 60;
		scDesc.BufferDesc.RefreshRate.Denominator = 1;
	}
	else {
		scDesc.BufferDesc.RefreshRate.Numerator = 0;
		scDesc.BufferDesc.RefreshRate.Denominator = 1;
	}
	scDesc.Windowed = true;

	res = factory->CreateSwapChain(queue, &scDesc, swapChain.ReleaseAndGetAddressOf());
	if (FAILED(res)) {
		MessageBox(NULL, "failed creating swapchain.", "Error", MB_OK);
		return false;
	}

	res = swapChain->QueryInterface(IID_PPV_ARGS(m_swapchain.ReleaseAndGetAddressOf()));
	if (FAILED(res)) {
		MessageBox(NULL, "failed creating swapchain3.", "Error", MB_OK);
		return false;
	}

	return true;
}