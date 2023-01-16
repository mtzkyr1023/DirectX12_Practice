#ifndef _QUEUE_H_
#define _QUEUE_H_


#include <d3d12.h>

#include <wrl/client.h>

class Queue {
public:
	Queue() = default;
	~Queue() = default;

	bool createGraphicsQueue(ID3D12Device* device);

	void waitForFence(ID3D12Fence* fence, HANDLE fenceEvent, UINT64 fenceValue);

	ID3D12CommandQueue* getQueue() { return m_queue.Get(); }

private:
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_queue;
};

#endif