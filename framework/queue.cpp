#include "queue.h"


bool Queue::createGraphicsQueue(ID3D12Device* device) {
	HRESULT res;

	D3D12_COMMAND_QUEUE_DESC cqDesc{};
	cqDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	cqDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	cqDesc.NodeMask = 0;
	cqDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;

	res = device->CreateCommandQueue(&cqDesc, IID_PPV_ARGS(m_queue.ReleaseAndGetAddressOf()));
	if (FAILED(res)) {
		MessageBox(NULL, "failed creating command queue.", "Error", MB_OK);
		return false;
	}

	return true;
}

void Queue::waitForFence(ID3D12Fence* fence, HANDLE fenceEvent, UINT64 fenceValue) {	
	m_queue->Signal(fence, fenceValue - 1);

	if (fence->GetCompletedValue() < fenceValue - 1) {
		fence->SetEventOnCompletion(fenceValue - 1, fenceEvent);
		WaitForSingleObject(fenceEvent, INFINITE);
	}
}