#ifndef _MY_GUI_H_
#define _MY_GUI_H_

#include <d3d12.h>
#include <wrl/client.h>

#include <Windows.h>

class MyGui {
public:
	MyGui() = default;
	~MyGui() = default;

	bool create(HWND hwnd, ID3D12Device* device, DXGI_FORMAT format, UINT backBufferCount, ID3D12DescriptorHeap* descHeap);
	void destroy();

	void renderFrame(ID3D12GraphicsCommandList* command);

private:
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_srvDescHeap;
};

#endif