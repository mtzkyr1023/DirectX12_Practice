#ifndef _DESCRIPTOR_HEAP_H_
#define _DESCRIPTOR_HEAP_H_

#include <d3d12.h>

#include <wrl/client.h>

class DescriptorHeap {
public:
	DescriptorHeap() = default;
	~DescriptorHeap() = default;

	bool create(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, D3D12_DESCRIPTOR_HEAP_FLAGS heapFlags, UINT descNum);
	void destroy();

	ID3D12DescriptorHeap* getDescriptorHeap() { return m_descHeap.Get(); }
	D3D12_CPU_DESCRIPTOR_HANDLE getCpuHandle(UINT num);
	D3D12_GPU_DESCRIPTOR_HANDLE getGpuHandle(UINT num);

	void copyDescriptors(int destId, D3D12_CPU_DESCRIPTOR_HANDLE srcHandle);

	UINT getDescriptorSize() { return m_descriptorSize; }

private:
	ID3D12Device* m_device;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_descHeap;
	D3D12_DESCRIPTOR_HEAP_TYPE m_heapType;
	UINT m_descriptorSize;
};

#endif