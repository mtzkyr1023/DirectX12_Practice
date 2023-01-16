#include "descriptor_heap.h"

bool DescriptorHeap::create(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, D3D12_DESCRIPTOR_HEAP_FLAGS heapFlags, UINT descNum) {
	HRESULT res;

	m_device = device;
	m_heapType = heapType;

	D3D12_DESCRIPTOR_HEAP_DESC dhDesc{};
	dhDesc.NumDescriptors = descNum;
	dhDesc.Type = heapType;
	dhDesc.Flags = heapFlags;

	res = device->CreateDescriptorHeap(&dhDesc, IID_PPV_ARGS(m_descHeap.ReleaseAndGetAddressOf()));
	if (FAILED(res)) {
		MessageBox(NULL, "failed creating rtv heap.", "Error", MB_OK);
		return false;
	}

	m_descriptorSize = device->GetDescriptorHandleIncrementSize(heapType);

	return true;
}

void DescriptorHeap::destroy() {
	m_descHeap.Reset();
}

void DescriptorHeap::copyDescriptors(int destId, D3D12_CPU_DESCRIPTOR_HANDLE srcHandle) {

	auto dest = m_descHeap->GetCPUDescriptorHandleForHeapStart();
	dest.ptr += m_descriptorSize * destId;

	UINT count = 1;

	m_device->CopyDescriptorsSimple(1, dest, srcHandle, m_heapType);
}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::getCpuHandle(UINT num) {
	auto handle = m_descHeap->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += m_descriptorSize * num;
	return handle;
}

D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::getGpuHandle(UINT num) {
	auto handle = m_descHeap->GetGPUDescriptorHandleForHeapStart();
	handle.ptr += m_descriptorSize * num;
	return handle;
}