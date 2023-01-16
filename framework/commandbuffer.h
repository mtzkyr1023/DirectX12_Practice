#ifndef _COMMANDBUFFER_H_
#define _COMMANDBUFFER_H_

#include <d3d12.h>

#include <wrl/client.h>


class CommandAllocator {
public:
	CommandAllocator() = default;
	~CommandAllocator() = default;

	bool createGraphicsCommandAllocator(ID3D12Device* device);
	bool createComputeCommandAllocator(ID3D12Device* device);
	bool createBundleCommandAllocator(ID3D12Device* device);

	ID3D12CommandAllocator* getCommandAllocator() { return m_commandAllocator.Get(); }

private:
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator;
};

class CommandList {
public:
	CommandList() = default;
	~CommandList() = default;

	bool createGraphicsCommandList(ID3D12Device* device, ID3D12CommandAllocator* commandAllocator);
	bool createComputeCommandList(ID3D12Device* device, ID3D12CommandAllocator* commandAllocator);

	bool createBundleCommandList(ID3D12Device* device, ID3D12CommandAllocator* commandAllocator);

	ID3D12GraphicsCommandList* getCommandList() { return m_commandList.Get(); }

private:
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList;
};





#endif