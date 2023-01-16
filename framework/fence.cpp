#include "fence.h"


Fence::~Fence() {
	CloseHandle(m_fenceEvent);
}

bool Fence::create(ID3D12Device* device) {
	HRESULT res;

	res = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_fence.ReleaseAndGetAddressOf()));
	if (FAILED(res)) {
		return false;
	}

	m_fenceEvent = CreateEvent(0, FALSE, FALSE, 0);

	return true;
}