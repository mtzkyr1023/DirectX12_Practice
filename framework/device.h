#ifndef _DEVICE_H_
#define _DEVICE_H_

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#include <d3d12.h>
#include <dxgi1_4.h>

#include <wrl/client.h>

class Device {
public:
	Device() = default;
	~Device() = default;

	bool create();

	ID3D12Device* getDevice() { return m_device.Get(); }

private:
	Microsoft::WRL::ComPtr<ID3D12Device> m_device;
	Microsoft::WRL::ComPtr<IDXGIFactory4> m_factory;
	Microsoft::WRL::ComPtr<IDXGIAdapter3> m_adapter;
};

#endif