#include "buffer.h"
#include "commandbuffer.h"


bool VertexBuffer::create(ID3D12Device* device, ID3D12CommandQueue* queue, UINT bufferCount, UINT stride, UINT size, void* data) {
	HRESULT res;

	m_resourceType = ResourceType::kVertexBuffer;

	m_resource.resize(bufferCount);
	m_vertexBufferView.resize(bufferCount);

	Microsoft::WRL::ComPtr<ID3D12Resource> stagingBuffer;

	CommandAllocator commandAllocator;
	CommandList commandList;
	if (!commandAllocator.createGraphicsCommandAllocator(device))
		return false;
	if (!commandList.createGraphicsCommandList(device, commandAllocator.getCommandAllocator()))
		return false;

	D3D12_HEAP_PROPERTIES heapProp{};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProp.CreationNodeMask = 1;
	heapProp.VisibleNodeMask = 1;

	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Alignment = 0;
	resDesc.Width = size;
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.Format = DXGI_FORMAT_UNKNOWN;
	resDesc.SampleDesc.Count = 1;
	resDesc.SampleDesc.Quality = 0;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	res = device->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &resDesc, D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr, IID_PPV_ARGS(stagingBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(res)) {
		return false;
	}

	BYTE* mappedData;
	res = stagingBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mappedData));
	if (FAILED(res)) {
		return false;
	}

	memcpy_s(mappedData, size, data, size);

	stagingBuffer->Unmap(0, nullptr);
	
	heapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProp.CreationNodeMask = 1;
	heapProp.VisibleNodeMask = 1;

	resDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

	for (UINT i = 0; i < bufferCount; i++) {
		res = device->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &resDesc, D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr, IID_PPV_ARGS(m_resource[i].GetAddressOf()));
		if (FAILED(res)) {
			return false;
		}

		ID3D12CommandAllocator* commandAlloc = commandAllocator.getCommandAllocator();
		ID3D12GraphicsCommandList* command = commandList.getCommandList();

		commandAlloc->Reset();
		command->Reset(commandAlloc, nullptr);


		command->CopyBufferRegion(m_resource[i].Get(), 0, stagingBuffer.Get(), 0, size);


		D3D12_RESOURCE_BARRIER barrier{};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = m_resource[i].Get();
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_FLAG_NONE;

		command->ResourceBarrier(1, &barrier);

		command->Close();

		ID3D12CommandList* cmdList[] = { command };
		queue->ExecuteCommandLists(_countof(cmdList), cmdList);

		Microsoft::WRL::ComPtr<ID3D12Fence> fence;
		res = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence.ReleaseAndGetAddressOf()));
		
		HANDLE fenceEvent = CreateEvent(0, FALSE, FALSE, 0);
		UINT fenceValue = 1;

		queue->Signal(fence.Get(), fenceValue);

		fence->SetEventOnCompletion(fenceValue, fenceEvent);
		WaitForSingleObject(fenceEvent, INFINITE);

		CloseHandle(fenceEvent);

		m_vertexBufferView[i].BufferLocation = m_resource[i]->GetGPUVirtualAddress();
		m_vertexBufferView[i].SizeInBytes = size;
		m_vertexBufferView[i].StrideInBytes = stride;
	}

	m_isUnorderedAccess = true;
	m_isShaderResource = true;

	return true;
}


bool IndexBuffer::create(ID3D12Device* device, ID3D12CommandQueue* queue, UINT bufferCount, UINT size, void* data) {
	HRESULT res;

	m_resourceType = ResourceType::kIndexBuffer;

	m_resource.resize(bufferCount);
	m_indexBufferView.resize(bufferCount);

	Microsoft::WRL::ComPtr<ID3D12Resource> stagingBuffer;

	CommandAllocator commandAllocator;
	CommandList commandList;
	if (!commandAllocator.createGraphicsCommandAllocator(device))
		return false;
	if (!commandList.createGraphicsCommandList(device, commandAllocator.getCommandAllocator()))
		return false;

	D3D12_HEAP_PROPERTIES heapProp{};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProp.CreationNodeMask = 1;
	heapProp.VisibleNodeMask = 1;

	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Alignment = 0;
	resDesc.Width = size;
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.Format = DXGI_FORMAT_UNKNOWN;
	resDesc.SampleDesc.Count = 1;
	resDesc.SampleDesc.Quality = 0;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	res = device->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &resDesc, D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr, IID_PPV_ARGS(stagingBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(res)) {
		return false;
	}

	BYTE* mappedData;
	res = stagingBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mappedData));
	if (FAILED(res)) {
		return false;
	}

	memcpy_s(mappedData, size, data, size);

	stagingBuffer->Unmap(0, nullptr);
	
	heapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProp.CreationNodeMask = 1;
	heapProp.VisibleNodeMask = 1;

	resDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

	for (UINT i = 0; i < bufferCount; i++) {
		heapProp.Type = D3D12_HEAP_TYPE_DEFAULT;

		res = device->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &resDesc, D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr, IID_PPV_ARGS(m_resource[i].GetAddressOf()));
		if (FAILED(res)) {
			return false;
		}

		ID3D12CommandAllocator* commandAlloc = commandAllocator.getCommandAllocator();
		ID3D12GraphicsCommandList* command = commandList.getCommandList();

		commandAlloc->Reset();
		command->Reset(commandAlloc, nullptr);


		command->CopyBufferRegion(m_resource[i].Get(), 0, stagingBuffer.Get(), 0, size);


		D3D12_RESOURCE_BARRIER barrier{};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = m_resource[i].Get();
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_INDEX_BUFFER;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_FLAG_NONE;

		command->ResourceBarrier(1, &barrier);

		command->Close();

		ID3D12CommandList* cmdList[] = { command };
		queue->ExecuteCommandLists(_countof(cmdList), cmdList);

		Microsoft::WRL::ComPtr<ID3D12Fence> fence;
		res = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence.ReleaseAndGetAddressOf()));

		HANDLE fenceEvent = CreateEvent(0, FALSE, FALSE, 0);
		UINT fenceValue = 1;

		queue->Signal(fence.Get(), fenceValue);

		fence->SetEventOnCompletion(fenceValue, fenceEvent);
		WaitForSingleObject(fenceEvent, INFINITE);

		CloseHandle(fenceEvent);

		m_indexBufferView[i].BufferLocation = m_resource[i]->GetGPUVirtualAddress();
		m_indexBufferView[i].SizeInBytes = size;
		m_indexBufferView[i].Format = DXGI_FORMAT_R32_UINT;
	}

	m_isUnorderedAccess = true;
	m_isShaderResource = true;

	return true;
}


bool StructuredBuffer::create(ID3D12Device* device, UINT stride, UINT bufferCount, UINT elementCount, bool isCpuAccess, bool isUnorderedAccess, bool isAppend, bool isConsume) {
	HRESULT res;

	m_resourceType = ResourceType::kStrucuredBuffer;
	if (isCpuAccess) {
		m_isUnorderedAccess = false;
	}
	else {
		if (isUnorderedAccess) {
			m_isUnorderedAccess = true;
		}
	}
	m_elementCount = elementCount;
	m_resource.resize(bufferCount);
	m_bufferPtr.resize(bufferCount);

	m_stride = stride;
	m_isCpuAccess = isCpuAccess;

	if (isCpuAccess) {
		for (UINT i = 0; i < bufferCount; i++) {
			D3D12_HEAP_PROPERTIES heapProp{};
			heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
			heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
			heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
			heapProp.CreationNodeMask = 1;
			heapProp.VisibleNodeMask = 1;

			D3D12_RESOURCE_DESC resDesc{};
			resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
			resDesc.Alignment = 0;
			resDesc.Width = (UINT64)(stride * elementCount);
			resDesc.Height = 1;
			resDesc.DepthOrArraySize = 1;
			resDesc.MipLevels = 1;
			resDesc.Format = DXGI_FORMAT_UNKNOWN;
			resDesc.SampleDesc.Count = 1;
			resDesc.SampleDesc.Quality = 0;
			resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
			resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

			res = device->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &resDesc, D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr, IID_PPV_ARGS(m_resource[i].ReleaseAndGetAddressOf()));
			res = device->GetDeviceRemovedReason();
			if (FAILED(res))
				return false;

			m_resource[i]->Map(0, nullptr, reinterpret_cast<void**>(&m_bufferPtr[i]));
		}
		m_isUnorderedAccess = false;
	}
	else {
		for (UINT i = 0; i < bufferCount; i++) {
			D3D12_HEAP_PROPERTIES heapProp{};
			heapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
			heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
			heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
			heapProp.CreationNodeMask = 1;
			heapProp.VisibleNodeMask = 1;

			D3D12_RESOURCE_DESC resDesc{};
			resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
			resDesc.Alignment = 0;
			resDesc.Width = (UINT64)(stride * elementCount) + (isAppend ? (D3D12_UAV_COUNTER_PLACEMENT_ALIGNMENT - (stride * elementCount) % D3D12_UAV_COUNTER_PLACEMENT_ALIGNMENT + sizeof(UINT)) : 0);
			resDesc.Height = 1;
			resDesc.DepthOrArraySize = 1;
			resDesc.MipLevels = 1;
			resDesc.Format = DXGI_FORMAT_UNKNOWN;
			resDesc.SampleDesc.Count = 1;
			resDesc.SampleDesc.Quality = 0;
			resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
			resDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

			res = device->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &resDesc, D3D12_RESOURCE_STATE_COMMON,
				nullptr, IID_PPV_ARGS(m_resource[i].ReleaseAndGetAddressOf()));
			res = device->GetDeviceRemovedReason();
			if (FAILED(res))
				return false;
		}

		m_isUnorderedAccess = true;
	}

	m_isAppendBuffer = isAppend;

	return true;
}

bool StructuredBuffer::create(ID3D12Device * dev, ID3D12CommandQueue * queue, UINT stride, UINT bufferCount, UINT elementCount, void * data)
{
	{
		HRESULT res;

		m_resource.resize(bufferCount);

		Microsoft::WRL::ComPtr<ID3D12Resource> stagingBuffer;

		m_elementCount = elementCount;
		m_stride = stride;
		CommandAllocator commandAllocator;
		CommandList commandList;
		if (!commandAllocator.createGraphicsCommandAllocator(dev))
			return false;
		if (!commandList.createGraphicsCommandList(dev, commandAllocator.getCommandAllocator()))
			return false;

		D3D12_HEAP_PROPERTIES heapProp{};
		heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
		heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProp.CreationNodeMask = 1;
		heapProp.VisibleNodeMask = 1;

		D3D12_RESOURCE_DESC resDesc{};
		resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resDesc.Alignment = 0;
		resDesc.Width = (UINT64)(stride * elementCount);
		resDesc.Height = 1;
		resDesc.DepthOrArraySize = 1;
		resDesc.MipLevels = 1;
		resDesc.Format = DXGI_FORMAT_UNKNOWN;
		resDesc.SampleDesc.Count = 1;
		resDesc.SampleDesc.Quality = 0;
		resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;


		D3D12_RESOURCE_DESC resDesc2{};
		resDesc2.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resDesc2.Alignment = 0;
		resDesc2.Width = (UINT64)(stride * elementCount);
		resDesc2.Height = 1;
		resDesc2.DepthOrArraySize = 1;
		resDesc2.MipLevels = 1;
		resDesc2.Format = DXGI_FORMAT_UNKNOWN;
		resDesc2.SampleDesc.Count = 1;
		resDesc2.SampleDesc.Quality = 0;
		resDesc2.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		resDesc2.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

		res = dev->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &resDesc, D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr, IID_PPV_ARGS(stagingBuffer.ReleaseAndGetAddressOf()));
		if (FAILED(res)) {
			return false;
		}

		BYTE* mappedData;
		res = stagingBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mappedData));
		if (FAILED(res)) {
			return false;
		}

		memcpy_s(mappedData, rsize_t(stride * elementCount), data, rsize_t(stride * elementCount));

		stagingBuffer->Unmap(0, nullptr);

		heapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
		heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProp.CreationNodeMask = 1;
		heapProp.VisibleNodeMask = 1;

		for (UINT i = 0; i < bufferCount; i++) {
			res = dev->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &resDesc2, D3D12_RESOURCE_STATE_COPY_DEST,
				nullptr, IID_PPV_ARGS(m_resource[i].GetAddressOf()));
			if (FAILED(res)) {
				return false;
			}

			ID3D12CommandAllocator* commandAlloc = commandAllocator.getCommandAllocator();
			ID3D12GraphicsCommandList* command = commandList.getCommandList();

			commandAlloc->Reset();
			command->Reset(commandAlloc, nullptr);


			command->CopyBufferRegion(m_resource[i].Get(), 0, stagingBuffer.Get(), 0, (UINT64)(stride * elementCount));


			D3D12_RESOURCE_BARRIER barrier{};
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barrier.Transition.pResource = m_resource[i].Get();
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
			barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_FLAG_NONE;

			command->ResourceBarrier(1, &barrier);

			command->Close();

			ID3D12CommandList* cmdList[] = { command };
			queue->ExecuteCommandLists(_countof(cmdList), cmdList);

			Microsoft::WRL::ComPtr<ID3D12Fence> fence;
			res = dev->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence.ReleaseAndGetAddressOf()));

			HANDLE fenceEvent = CreateEvent(0, FALSE, FALSE, 0);
			UINT fenceValue = 1;

			queue->Signal(fence.Get(), fenceValue);

			fence->SetEventOnCompletion(fenceValue, fenceEvent);
			WaitForSingleObject(fenceEvent, INFINITE);

			CloseHandle(fenceEvent);
		}

		m_isCpuAccess = false;
		m_isUnorderedAccess = true;

		return true;
	}
}

void StructuredBuffer::transitionResource(ID3D12GraphicsCommandList* command, UINT textureNum, D3D12_RESOURCE_BARRIER_FLAGS flag,
	D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after) {
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = flag;
	barrier.Transition.pResource = m_resource[textureNum].Get();
	barrier.Transition.StateBefore = before;
	barrier.Transition.StateAfter = after;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	command->ResourceBarrier(1, &barrier);
}


bool ByteAddressBuffer::create(ID3D12Device* device, DXGI_FORMAT format, UINT resourceCount, UINT size, bool isUnorderedAccess) {
	m_resource.resize(resourceCount);

	HRESULT res;

	for (UINT i = 0; i < m_resource.size(); i++) {
		D3D12_HEAP_PROPERTIES heapProp{};
		heapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
		heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProp.CreationNodeMask = 1;
		heapProp.VisibleNodeMask = 1;

		D3D12_RESOURCE_DESC resDesc{};
		resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resDesc.Alignment = 0;
		resDesc.Width = size;
		resDesc.Height = 1;
		resDesc.DepthOrArraySize = 1;
		resDesc.MipLevels = 1;
		resDesc.Format = DXGI_FORMAT_UNKNOWN;
		resDesc.SampleDesc.Count = 1;
		resDesc.SampleDesc.Quality = 0;
		resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		resDesc.Flags = isUnorderedAccess ? D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS : D3D12_RESOURCE_FLAG_NONE;

		res = device->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_ALLOW_SHADER_ATOMICS, &resDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(m_resource[i].ReleaseAndGetAddressOf()));
		if (FAILED(res))
			return false;
	}


	m_isUnorderedAccess = isUnorderedAccess;

	m_format = format;

	return true;
}


void ByteAddressBuffer::transitionResource(ID3D12GraphicsCommandList* command, UINT resourceCount, D3D12_RESOURCE_BARRIER_FLAGS flags,
	D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after) {
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = flags;
	barrier.Transition.pResource = m_resource[resourceCount].Get();
	barrier.Transition.StateBefore = before;
	barrier.Transition.StateAfter = after;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	command->ResourceBarrier(1, &barrier);
}