#ifndef _SWAPCHAIN_H_
#define _SWAPCHAIN_H_

#include <d3d12.h>
#include <dxgi1_4.h>

#include <wrl/client.h>

class Swapchain {
public:
	Swapchain() = default;
	~Swapchain() = default;

	bool create(ID3D12CommandQueue* queue, HWND hwnd, UINT backBufferCount, UINT width, UINT height, bool vsync);

	IDXGISwapChain3* getSwapchain() { return m_swapchain.Get(); }

private:
	Microsoft::WRL::ComPtr<IDXGISwapChain3> m_swapchain;
};


#endif