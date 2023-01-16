#ifndef _FENCE_H_
#define _FENCE_H_


#include <d3d12.h>

#include <wrl/client.h>

class Fence {
public:
	Fence() = default;
	~Fence();

	bool create(ID3D12Device* device);

	UINT64 getFenceValue() { return ++m_fenceValue; }
	ID3D12Fence* getFence() { return m_fence.Get(); }
	HANDLE getFenceEvent() { return m_fenceEvent; }

private:
	Microsoft::WRL::ComPtr<ID3D12Fence> m_fence;
	HANDLE m_fenceEvent;
	UINT64 m_fenceValue = 0;
};

#endif