#ifndef _BUFFER_H_
#define _BUFFER_H_


#include "resource.h"
#include "fence.h"


class VertexBuffer : public Resource {
public:
	VertexBuffer(const char* name) : Resource(name) {}
	VertexBuffer() : Resource("vertexBuffer") {}
	~VertexBuffer() = default;

	bool create(ID3D12Device* device, ID3D12CommandQueue* queue, UINT bufferCount, UINT stride, UINT size, void* data);

	D3D12_VERTEX_BUFFER_VIEW* getVertexBuferView(UINT num) { return &m_vertexBufferView[num]; }

	bool getIsUnorderedAccess() { return m_isUnorderedAccess; }
	bool getIsShaderResource() { return m_isShaderResource; }

private:
	std::vector<D3D12_VERTEX_BUFFER_VIEW> m_vertexBufferView;

	bool m_isShaderResource;
	bool m_isUnorderedAccess;
};


class IndexBuffer : public Resource {
public:
	IndexBuffer(const char* name) :Resource(name) {}
	IndexBuffer() : Resource("indexBuffer") {}
	~IndexBuffer() = default;

	bool create(ID3D12Device* device, ID3D12CommandQueue* queue, UINT bufferCount, UINT size, void* data);

	D3D12_INDEX_BUFFER_VIEW* getIndexBufferView(UINT num) { return &m_indexBufferView[num]; }

	bool getIsUnorderedAccess() { return m_isUnorderedAccess; }
	bool getIsShaderResource() { return m_isShaderResource; }

private:
	std::vector<D3D12_INDEX_BUFFER_VIEW> m_indexBufferView;

	bool m_isShaderResource;
	bool m_isUnorderedAccess;
};


class ConstantBuffer : public Resource {
public:
	ConstantBuffer() {}
	ConstantBuffer(const char* name) : Resource(name) {}
	~ConstantBuffer() {
		for (auto& ite : m_resource)
			ite->Unmap(0, nullptr);
	}

	bool create(ID3D12Device* device, UINT size, UINT bufferCount) {
		HRESULT res;

		m_resourceType = ResourceType::kConstanceBuffer;

		m_resource.resize(bufferCount);
		m_constantBufferPtr.resize(bufferCount);

		m_size = size;

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
				nullptr, IID_PPV_ARGS(m_resource[i].ReleaseAndGetAddressOf()));
			res = device->GetDeviceRemovedReason();
			if (FAILED(res))
				return false;

			m_resource[i]->Map(0, nullptr, reinterpret_cast<void**>(&m_constantBufferPtr[i]));
		}

		return true;
	}

	void updateBuffer(UINT bufferNum, UINT size, void* data) {
		memcpy_s(m_constantBufferPtr[bufferNum], size, data, size);
	}

	BYTE* getBuffer(UINT num) { return m_constantBufferPtr[num]; }

	size_t getSize() { return m_size; }

private:
	std::vector<BYTE*> m_constantBufferPtr;
	size_t m_size;
};


class StructuredBuffer : public Resource {
public:
	StructuredBuffer() : Resource("StructuredBuffer") {
		m_resourceType = ResourceType::kStrucuredBuffer;
	}
	StructuredBuffer(const char* name) : Resource(name) {
		m_resourceType = ResourceType::kStrucuredBuffer;
	}
	~StructuredBuffer() {
		if (m_isCpuAccess) {
			for (auto& ite : m_resource)
				ite->Unmap(0, nullptr);
		}
	}

	bool create(ID3D12Device* device, UINT stride, UINT bufferCount, UINT elementCount, bool isCpuAccess, bool isUnorderedAccess, bool isAppend = false, bool isConsume = false);

	bool create(ID3D12Device* dev, ID3D12CommandQueue* queue, UINT stride, UINT bufferCount, UINT elementCount, void* data);

	void updateBuffer(UINT bufferNum, UINT size, void* data) {
		memcpy_s(m_bufferPtr[bufferNum], size, data, size);
	}


	void transitionResource(ID3D12GraphicsCommandList* command, UINT textureNum, D3D12_RESOURCE_BARRIER_FLAGS flag,
		D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after);

	BYTE* getBuffer(UINT num) { return m_bufferPtr[num]; }

	bool getIsUnorderedAccess() { return m_isUnorderedAccess; }

	int getElementCount() { return m_elementCount; }
	int getStride() { return m_stride; }

	bool IsAppendBuffer() { return m_isAppendBuffer; }
	bool IsConsumeBuffer() { return m_isConsumeBuffer; }

	UINT getCounterBufferOffset() { return ((m_elementCount * m_stride) + D3D12_UAV_COUNTER_PLACEMENT_ALIGNMENT - (m_elementCount * m_stride) % D3D12_UAV_COUNTER_PLACEMENT_ALIGNMENT); }

private:
	std::vector<BYTE*> m_bufferPtr;
	bool m_isCpuAccess = false;
	bool m_isUnorderedAccess = false;

	bool m_isAppendBuffer = false;
	bool m_isConsumeBuffer = false;

	int m_elementCount;
	int m_stride;
};


class ByteAddressBuffer : public Resource {
public:
	ByteAddressBuffer() : Resource("ByteAddressBuffer") {
		m_resourceType = ResourceType::kByteAddressBuffer;
	}
	ByteAddressBuffer(const char* name) : Resource(name) {
		m_resourceType = ResourceType::kByteAddressBuffer;
	}
	~ByteAddressBuffer() {

	}

	bool create(ID3D12Device* device, DXGI_FORMAT format, UINT resourceCount, UINT size, bool isUnorderedAccess);

	void transitionResource(ID3D12GraphicsCommandList* command, UINT resourceCount, D3D12_RESOURCE_BARRIER_FLAGS flags,
		D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after);

	bool getIsUnorderedAccess() { return m_isUnorderedAccess; }

	UINT getSize() { return m_size; }

	DXGI_FORMAT getFormat() { return m_format; }

private:
	bool m_isUnorderedAccess = false;

	DXGI_FORMAT m_format;

	size_t m_size;
};

#endif