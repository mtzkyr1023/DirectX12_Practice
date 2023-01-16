#include "resource_manager.h"


int ResourceManager::createBackBuffer(ID3D12Device* device, IDXGISwapChain3* swapchain, UINT backBufferCount) {
	int id = m_uniqueId;
	if (m_resourceArray.find(id) != m_resourceArray.end()) m_resourceArray.erase(id);
	m_resourceArray[id] = std::make_unique<Texture>();
	if (!static_cast<Texture*>(m_resourceArray[id].get())->createBackBuffer(device, swapchain, backBufferCount))
		return -1;

	m_resourceArray[id]->setId(id);

	m_uniqueId++;

	return id;
}


int ResourceManager::createDepthStencilBuffer(ID3D12Device* device, UINT textureCount, UINT width, UINT height, bool isStencil) {
	int id = m_uniqueId;
	if (m_resourceArray.find(id) != m_resourceArray.end()) m_resourceArray.erase(id);
	m_resourceArray[id] = std::make_unique<Texture>();
	if (isStencil) {
		if (!static_cast<Texture*>(m_resourceArray[id].get())->createDepthStencilBuffer(device, textureCount, DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS, width, height))
			return -1;
	}
	else {
		if (!static_cast<Texture*>(m_resourceArray[id].get())->createDepthStencilBuffer(device, textureCount, DXGI_FORMAT_R32_TYPELESS, width, height))
			return -1;
	}

	m_resourceArray[id]->setId(id);

	m_uniqueId++;

	return id;
}


int ResourceManager::createRenderTarget2D(ID3D12Device* device, UINT resourceCount, D3D12_RESOURCE_FLAGS flags,
	DXGI_FORMAT format, UINT width, UINT height) {
	int id = m_uniqueId;
	if (m_resourceArray.find(id) != m_resourceArray.end()) m_resourceArray.erase(id);
	m_resourceArray[id] = std::make_unique<Texture>();
	if (!static_cast<Texture*>(m_resourceArray[id].get())->createRenderTarget2D(device, resourceCount, flags, format, width, height))
		return -1;

	m_resourceArray[id]->setId(id);

	m_uniqueId++;

	return id;
}

int ResourceManager::createRenderTarget2DArray(ID3D12Device* device, UINT resourceCount, D3D12_RESOURCE_FLAGS flags,
	DXGI_FORMAT format, UINT width, UINT height, UINT depth) {
	int id = m_uniqueId;
	if (m_resourceArray.find(id) != m_resourceArray.end()) m_resourceArray.erase(id);
	m_resourceArray[id] = std::make_unique<Texture>();
	if (!static_cast<Texture*>(m_resourceArray[id].get())->createRenderTarget2DArray(device, resourceCount, flags, format, width, height, depth))
		return -1;

	m_resourceArray[id]->setId(id);

	m_uniqueId++;

	return id;
}

int ResourceManager::createTexture(ID3D12Device* device, ID3D12CommandQueue* queue, UINT resourceCount, DXGI_FORMAT format,
	const char* filename, bool isMipmap) {
	int id = m_uniqueId;
	if (m_resourceArray.find(id) != m_resourceArray.end()) m_resourceArray.erase(id);
	m_resourceArray[id] = std::make_unique<Texture>();
	if (!static_cast<Texture*>(m_resourceArray[id].get())->createResource(device, queue, resourceCount, format, filename, isMipmap))
		return -1;

	m_resourceArray[id]->setId(id);

	m_uniqueId++;

	return id;
}

int ResourceManager::createTexture(ID3D12Device* device, ID3D12CommandQueue* queue, UINT resourceCount, UINT width, UINT height, UINT componentCount, void* data, bool isMipmap)
{
	int id = m_uniqueId;
	if (m_resourceArray.find(id) != m_resourceArray.end()) m_resourceArray.erase(id);
	m_resourceArray[id] = std::make_unique<Texture>();
	if (!static_cast<Texture*>(m_resourceArray[id].get())->createResource(device, queue, resourceCount, width, height, componentCount, data, isMipmap))
		return -1;

	m_resourceArray[id]->setId(id);

	m_uniqueId++;

	return id;
}

int ResourceManager::createCubeMap(ID3D12Device* device, ID3D12CommandQueue* queue, std::vector<std::string>& filenames, DXGI_FORMAT format, bool isUnorderedAccess) {
	int id = m_uniqueId;
	if (m_resourceArray.find(id) != m_resourceArray.end()) m_resourceArray.erase(id);

	m_resourceArray[id] = std::make_unique<Texture>();

	if (!static_cast<Texture*>(m_resourceArray[id].get())->createCubeMap(device, queue, filenames, format, isUnorderedAccess))
		return -1;

	m_resourceArray[id]->setId(id);

	m_uniqueId++;

	return id;
}

int ResourceManager::createConstantBuffer(ID3D12Device* device, UINT size, UINT backBufferCount) {
	int id = m_uniqueId;
	if (m_resourceArray.find(id) != m_resourceArray.end()) m_resourceArray.erase(id);
	m_resourceArray[id] = std::make_unique<ConstantBuffer>();
	if (!static_cast<ConstantBuffer*>(m_resourceArray[id].get())->create(device, size, backBufferCount))
		return -1;

	m_resourceArray[id]->setId(id);

	m_uniqueId++;

	return id;
}

int ResourceManager::createStructuredBuffer(ID3D12Device* device, ID3D12CommandQueue* queue, UINT bufferCount, UINT stride, UINT elemCount, void* data)
{
	int id = m_uniqueId;
	if (m_resourceArray.find(id) != m_resourceArray.end()) m_resourceArray.erase(id);
	m_resourceArray[id] = std::make_unique<StructuredBuffer>();
	if (!static_cast<StructuredBuffer*>(m_resourceArray[id].get())->create(device, queue, stride, bufferCount, elemCount, data))
		return -1;

	m_resourceArray[id]->setId(id);

	m_uniqueId++;

	return id;
}

int ResourceManager::createStructuredBuffer(ID3D12Device* device, UINT bufferCount, UINT stride, UINT elementCount, bool isCpuAccess, bool isUnorderedAccess)
{

	int id = m_uniqueId;
	if (m_resourceArray.find(id) != m_resourceArray.end()) m_resourceArray.erase(id);
	m_resourceArray[id] = std::make_unique<StructuredBuffer>();
	if (!static_cast<StructuredBuffer*>(m_resourceArray[id].get())->create(device, stride, bufferCount, elementCount, isCpuAccess, isUnorderedAccess))
		return -1;

	m_resourceArray[id]->setId(id);

	m_uniqueId++;

	return id;
}

int ResourceManager::createAppendStructuredBuffer(ID3D12Device* device, UINT bufferCount, UINT stride, UINT elementCount, bool isCpuAccess, bool isUnorderedAccess) {
	int id = m_uniqueId;
	if (m_resourceArray.find(id) != m_resourceArray.end()) m_resourceArray.erase(id);
	m_resourceArray[id] = std::make_unique<StructuredBuffer>();
	if (!static_cast<StructuredBuffer*>(m_resourceArray[id].get())->create(device, stride, bufferCount, elementCount, isCpuAccess, isUnorderedAccess, true))
		return -1;

	m_resourceArray[id]->setId(id);

	m_uniqueId++;

	return id;
}

int ResourceManager::createByteAddressBuffer(ID3D12Device* device, DXGI_FORMAT format, UINT bufferCount, UINT size, bool isUnorderedAccess) {
	int id = m_uniqueId;
	if (m_resourceArray.find(id) != m_resourceArray.end()) m_resourceArray.erase(id);
	m_resourceArray[id] = std::make_unique<ByteAddressBuffer>();
	if (!static_cast<ByteAddressBuffer*>(m_resourceArray[id].get())->create(device, format, bufferCount, size, isUnorderedAccess))
		return -1;

	m_resourceArray[id]->setId(id);

	m_uniqueId++;

	return id;
}

int ResourceManager::createVertexBuffer(ID3D12Device* device, ID3D12CommandQueue* queue, UINT bufferCount, UINT stride, UINT size, void* data) {
	int id = m_uniqueId;
	if (m_resourceArray.find(id) != m_resourceArray.end()) m_resourceArray.erase(id);
	m_resourceArray[id] = std::make_unique<VertexBuffer>();
	if (!static_cast<VertexBuffer*>(m_resourceArray[id].get())->create(device, queue, bufferCount, stride, size, data))
		return -1;

	m_resourceArray[id]->setId(id);

	m_uniqueId++;

	return id;
}

int ResourceManager::createIndexBuffer(ID3D12Device* device, ID3D12CommandQueue* queue, UINT bufferCount, UINT stride, UINT size, void* data) {
	int id = m_uniqueId;
	if (m_resourceArray.find(id) != m_resourceArray.end()) m_resourceArray.erase(id);
	m_resourceArray[id] = std::make_unique<IndexBuffer>();
	if (!static_cast<IndexBuffer*>(m_resourceArray[id].get())->create(device, queue, bufferCount, size, data))
		return -1;

	m_resourceArray[id]->setId(id);

	m_uniqueId++;

	return id;
}

int ResourceManager::addSamplerState(D3D12_SAMPLER_DESC samplerState) {
	int id = m_uniqueId;
	m_samplerStateTable[id] = samplerState;

	m_uniqueId++;

	return id;
}

int ResourceManager::addVertexShader(const wchar_t* filename) {
	int id = m_uniqueId;
	m_shaderTable[id] = std::make_shared<Shader>();
	m_shaderTable[id]->createVertexShader(filename);

	m_uniqueId++;

	return id;
}

int ResourceManager::addPixelShader(const wchar_t* filename) {
	int id = m_uniqueId;
	m_shaderTable[id] = std::make_shared<Shader>();
	m_shaderTable[id]->createPixelShader(filename);

	m_uniqueId++;

	return id;
}

int ResourceManager::addComputeShader(const wchar_t* filename) {
	int id = m_uniqueId;
	m_shaderTable[id] = std::make_shared<Shader>();
	m_shaderTable[id]->createComputeShader(filename);

	m_uniqueId++;

	return id;
}


void ResourceManager::updateDescriptorHeap(Device* device, int globalHeapCount) {

	m_globalHeap.create(device->getDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, globalHeapCount);

	std::vector<Resource*> shaderResource;
	std::vector<Resource*> renderTarget;
	std::vector<Resource*> depthStencil;
	int shaderResourceCount = 4;
	int renderTargetCount = 0;
	int depthStencilCount = 0;

	for (auto& ite : m_resourceArray) {
		switch (ite.second->GetResourceType()) {
		case ResourceType::kConstanceBuffer:
			shaderResource.push_back(ite.second.get());
			shaderResourceCount += ite.second->getResourceCount();
			break;

		case ResourceType::kStrucuredBuffer:
		{
			StructuredBuffer* buf = static_cast<StructuredBuffer*>(ite.second.get());

			shaderResource.push_back(ite.second.get());
			shaderResourceCount += ite.second->getResourceCount() * (buf->getIsUnorderedAccess() ? 2 : 1);
			break;
		}

		case ResourceType::kByteAddressBuffer:
		{
			ByteAddressBuffer* buf = static_cast<ByteAddressBuffer*>(ite.second.get());
			shaderResource.push_back(ite.second.get());
			shaderResourceCount += ite.second->getResourceCount() * (buf->getIsUnorderedAccess() ? 2 : 1);
			break;
		}

		case ResourceType::kTexture:
		{
			Texture* tex = static_cast<Texture*>(ite.second.get());
			if (tex->isDepthStencil()) {
				depthStencil.push_back(ite.second.get());
				depthStencilCount += ite.second->getResourceCount();
			}
			if (tex->isShaderResource()) {
				shaderResource.push_back(ite.second.get());
				shaderResourceCount += ite.second->getResourceCount() * (tex->isUnorderedAccess() ? 2 : 1);
			}
			if (tex->isRenderTarget()) {
				renderTarget.push_back(ite.second.get());
				renderTargetCount += ite.second->getResourceCount();
				if (tex->getDepth() != 1) {
					renderTargetCount += tex->getDepth() - 1;
				}
			}
			break;
		}
		case ResourceType::kVertexBuffer:
		{
			VertexBuffer* vertexBuffer = static_cast<VertexBuffer*>(ite.second.get());
			if (vertexBuffer->getIsShaderResource()) {
				shaderResource.push_back(ite.second.get());
				shaderResourceCount += ite.second->getResourceCount() * (vertexBuffer->getIsUnorderedAccess() ? 2 : 1);
			}
			break;
		}

		case ResourceType::kIndexBuffer:
		{
			IndexBuffer* indexBuffer = static_cast<IndexBuffer*>(ite.second.get());
			if (indexBuffer->getIsShaderResource()) {
				shaderResource.push_back(ite.second.get());
				shaderResourceCount += ite.second->getResourceCount() * (indexBuffer->getIsUnorderedAccess() ? 2 : 1);
			}
			break;
		}
		}
	}

	m_shaderResourceHeap.destroy();
	m_rtvHeap.destroy();
	m_dsvHeap.destroy();
	m_samplerHeap.destroy();

	if (shaderResource.size() != 0)
		m_shaderResourceHeap.create(device->getDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, (UINT)shaderResourceCount);
	if (renderTarget.size() != 0)
		m_rtvHeap.create(device->getDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, (UINT)renderTargetCount);
	if (depthStencil.size() != 0)
		m_dsvHeap.create(device->getDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, (UINT)depthStencilCount);

	if (m_samplerStateTable.size() != 0)
		m_samplerHeap.create(device->getDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, (UINT)m_samplerStateTable.size());


	int offset = 0;
	for (int i = 0; i < (int)shaderResource.size(); i++) {
		if (shaderResource[i]->GetResourceType() == ResourceType::kTexture) {
			Texture* tex = static_cast<Texture*>(shaderResource[i]);
			int start = offset;

			if (tex->isShaderResource()) {
				for (int j = 0; j < tex->getResourceCount(); j++) {
					D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
					if (tex->isDepthStencil()) {
						if (tex->getFormat() == DXGI_FORMAT_R32_TYPELESS) {
							srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
							srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
							srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
							srvDesc.Texture2D.MipLevels = 1;
							srvDesc.Texture2D.MostDetailedMip = 0;
							srvDesc.Texture2D.PlaneSlice = 0;
							srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;


							auto shaderResourceHandle = m_shaderResourceHeap.getCpuHandle(offset);
							ID3D12Resource* res = tex->getResource(j);

							device->getDevice()->CreateShaderResourceView(res, &srvDesc, shaderResourceHandle);

							offset++;
						}
						else if (tex->getFormat() == DXGI_FORMAT_R24G8_TYPELESS) {
							{
								srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
								srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
								srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
								srvDesc.Texture2D.MipLevels = 1;
								srvDesc.Texture2D.MostDetailedMip = 0;
								srvDesc.Texture2D.PlaneSlice = 0;
								srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;


								auto shaderResourceHandle = m_shaderResourceHeap.getCpuHandle(offset);
								ID3D12Resource* res = tex->getResource(j);

								device->getDevice()->CreateShaderResourceView(res, &srvDesc, shaderResourceHandle);

								offset++;
							}
							{
								srvDesc.Format = DXGI_FORMAT_X24_TYPELESS_G8_UINT;
								srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
								srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
								srvDesc.Texture2D.MipLevels = 1;
								srvDesc.Texture2D.MostDetailedMip = 0;
								srvDesc.Texture2D.PlaneSlice = 0;
								srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;


								auto shaderResourceHandle = m_shaderResourceHeap.getCpuHandle(offset);
								ID3D12Resource* res = tex->getResource(j);

								device->getDevice()->CreateShaderResourceView(res, &srvDesc, shaderResourceHandle);

								offset++;
							}
						}
						else if (tex->getFormat() == DXGI_FORMAT_R32G32_TYPELESS) {
							{
								srvDesc.Format = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
								srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
								srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
								srvDesc.Texture2D.MipLevels = 1;
								srvDesc.Texture2D.MostDetailedMip = 0;
								srvDesc.Texture2D.PlaneSlice = 0;
								srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;


								auto shaderResourceHandle = m_shaderResourceHeap.getCpuHandle(offset);
								ID3D12Resource* res = tex->getResource(j);

								device->getDevice()->CreateShaderResourceView(res, &srvDesc, shaderResourceHandle);

								offset++;
							}
							{
								srvDesc.Format = DXGI_FORMAT_X32_TYPELESS_G8X24_UINT;
								srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
								srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
								srvDesc.Texture2D.MipLevels = 1;
								srvDesc.Texture2D.MostDetailedMip = 0;
								srvDesc.Texture2D.PlaneSlice = 0;
								srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;


								auto shaderResourceHandle = m_shaderResourceHeap.getCpuHandle(offset);
								ID3D12Resource* res = tex->getResource(j);

								device->getDevice()->CreateShaderResourceView(res, &srvDesc, shaderResourceHandle);

								offset++;
							}

						}
					}
					else {
						srvDesc.Format = tex->getFormat();
						srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
						if (tex->getDepth() == 1) {
							srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
							srvDesc.Texture2D.MipLevels = 1;
							srvDesc.Texture2D.MostDetailedMip = 0;
							srvDesc.Texture2D.PlaneSlice = 0;
							srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
						}
						else {
							srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
							srvDesc.Texture2DArray.MipLevels = 1;
							srvDesc.Texture2DArray.MostDetailedMip = 0;
							srvDesc.Texture2DArray.PlaneSlice = 0;
							srvDesc.Texture2DArray.ResourceMinLODClamp = 0.0f;
							srvDesc.Texture2DArray.ArraySize = (UINT)tex->getDepth();
							srvDesc.Texture2DArray.FirstArraySlice = 0;
						}


						auto shaderResourceHandle = m_shaderResourceHeap.getCpuHandle(offset);
						ID3D12Resource* res = tex->getResource(j);

						device->getDevice()->CreateShaderResourceView(res, &srvDesc, shaderResourceHandle);

						offset++;
					}
				}
			}

			if (tex->isUnorderedAccess()) {
				for (int j = 0; j < tex->getResourceCount(); j++) {
					D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
					if (tex->getFormat() == DXGI_FORMAT_R32_TYPELESS) {
						uavDesc.Format = DXGI_FORMAT_D32_FLOAT;
						uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
						uavDesc.Texture2D.MipSlice = 0;
						uavDesc.Texture2D.PlaneSlice = 0;
					}
					else if (tex->getFormat() == DXGI_FORMAT_R24G8_TYPELESS) {
						uavDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
						uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
						uavDesc.Texture2D.MipSlice = 0;
						uavDesc.Texture2D.PlaneSlice = 0;
					}
					else if (tex->getFormat() == DXGI_FORMAT_R32G32_TYPELESS) {
						uavDesc.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
						uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
						uavDesc.Texture2D.MipSlice = 0;
						uavDesc.Texture2D.PlaneSlice = 0;
					}
					else {
						if (tex->getDepth() == 1) {
							uavDesc.Format = tex->getFormat();
							uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
							uavDesc.Texture2D.MipSlice = 0;
							uavDesc.Texture2D.PlaneSlice = 0;
						}
						else {
							uavDesc.Format = tex->getFormat();
							uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
							uavDesc.Texture2DArray.ArraySize = (UINT)tex->getDepth();
							uavDesc.Texture2DArray.FirstArraySlice = 0;
							uavDesc.Texture2DArray.MipSlice = 0;
							uavDesc.Texture2DArray.PlaneSlice = 0;
						}
					}

					auto shaderResourceHandle = m_shaderResourceHeap.getCpuHandle(offset);
					ID3D12Resource* res = tex->getResource(j);

					device->getDevice()->CreateUnorderedAccessView(res, nullptr, &uavDesc, shaderResourceHandle);

					offset++;
				}
			}

			m_shaderResourceDescriptorTable[tex->getId()] = { start, tex->getResourceCount() + offset - start };
		}
		else if (shaderResource[i]->GetResourceType() == ResourceType::kConstanceBuffer) {
			ConstantBuffer* cb = static_cast<ConstantBuffer*>(shaderResource[i]);

			int start = offset;
			for (int j = 0; j < cb->getResourceCount(); j++) {
				D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};

				cbvDesc.BufferLocation = cb->getResource(j)->GetGPUVirtualAddress();
				cbvDesc.SizeInBytes = (UINT)cb->getSize();

				device->getDevice()->CreateConstantBufferView(&cbvDesc, m_shaderResourceHeap.getCpuHandle(offset));

				offset++;
			}

			m_shaderResourceDescriptorTable[cb->getId()] = { start, cb->getResourceCount() };
		}
		else if (shaderResource[i]->GetResourceType() == ResourceType::kStrucuredBuffer) {
			StructuredBuffer* sb = static_cast<StructuredBuffer*>(shaderResource[i]);

			int start = offset;
			for (int j = 0; j < sb->getResourceCount(); j++) {
				D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};

				srvDesc.Format = DXGI_FORMAT_UNKNOWN;
				srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
				srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
				srvDesc.Buffer.FirstElement = 0;
				srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
				srvDesc.Buffer.NumElements = sb->getElementCount();
				srvDesc.Buffer.StructureByteStride = sb->getStride();

				device->getDevice()->CreateShaderResourceView(sb->getResource(j), &srvDesc, m_shaderResourceHeap.getCpuHandle(offset));

				offset++;

				if (sb->getIsUnorderedAccess()) {
					D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
					uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
					uavDesc.Format = DXGI_FORMAT_UNKNOWN;
					uavDesc.Buffer.CounterOffsetInBytes = sb->IsAppendBuffer() ? sb->getCounterBufferOffset() : 0;
					uavDesc.Buffer.FirstElement = 0;
					uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
					uavDesc.Buffer.NumElements = sb->getElementCount();
					uavDesc.Buffer.StructureByteStride = sb->getStride();

					device->getDevice()->CreateUnorderedAccessView(sb->getResource(j), sb->IsAppendBuffer() ? sb->getResource(j) : nullptr, &uavDesc, m_shaderResourceHeap.getCpuHandle(offset));

					offset++;
				}
			}
			m_shaderResourceDescriptorTable[sb->getId()] = { start, offset };
		}
		else if (shaderResource[i]->GetResourceType() == ResourceType::kByteAddressBuffer) {
			ByteAddressBuffer* bb = static_cast<ByteAddressBuffer*>(shaderResource[i]);

			int start = offset;
			for (int j = 0; j < bb->getResourceCount(); j++) {
				D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};

				srvDesc.Format = bb->getFormat();
				srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
				srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
				srvDesc.Buffer.FirstElement = 0;
				srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
				srvDesc.Buffer.NumElements = 1;
				srvDesc.Buffer.StructureByteStride = bb->getSize();

				device->getDevice()->CreateShaderResourceView(bb->getResource(j), &srvDesc, m_shaderResourceHeap.getCpuHandle(offset));

				offset++;

				if (bb->getIsUnorderedAccess()) {
					D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
					uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
					uavDesc.Format = bb->getFormat();
					uavDesc.Buffer.CounterOffsetInBytes = 0;
					uavDesc.Buffer.FirstElement = 0;
					uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
					uavDesc.Buffer.NumElements = 1;
					uavDesc.Buffer.StructureByteStride = bb->getSize();

					device->getDevice()->CreateUnorderedAccessView(bb->getResource(j), nullptr, &uavDesc, m_shaderResourceHeap.getCpuHandle(offset));

					offset++;
				}
			}

			m_shaderResourceDescriptorTable[bb->getId()] = { start, offset };

		}
		else if (shaderResource[i]->GetResourceType() == ResourceType::kVertexBuffer) {
			VertexBuffer* vb = static_cast<VertexBuffer*>(shaderResource[i]);
			int start = offset;
			for (int j = 0; j < vb->getResourceCount(); j++) {
				D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};

				srvDesc.Format = DXGI_FORMAT_UNKNOWN;
				srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
				srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
				srvDesc.Buffer.FirstElement = 0;
				srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
				srvDesc.Buffer.NumElements = vb->getVertexBuferView(j)->SizeInBytes / vb->getVertexBuferView(j)->StrideInBytes;
				srvDesc.Buffer.StructureByteStride = vb->getVertexBuferView(j)->StrideInBytes;

				device->getDevice()->CreateShaderResourceView(vb->getResource(j), &srvDesc, m_shaderResourceHeap.getCpuHandle(offset));

				offset++;

				if (vb->getIsUnorderedAccess()) {
					D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
					uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
					uavDesc.Format = DXGI_FORMAT_UNKNOWN;
					uavDesc.Buffer.CounterOffsetInBytes = 0;
					uavDesc.Buffer.FirstElement = 0;
					uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
					uavDesc.Buffer.NumElements = vb->getVertexBuferView(j)->SizeInBytes / vb->getVertexBuferView(j)->StrideInBytes;
					uavDesc.Buffer.StructureByteStride = vb->getVertexBuferView(j)->StrideInBytes;

					device->getDevice()->CreateUnorderedAccessView(vb->getResource(j), nullptr, &uavDesc, m_shaderResourceHeap.getCpuHandle(offset));

					offset++;
				}
			}
			m_shaderResourceDescriptorTable[vb->getId()] = { start, offset };

		}
		else if (shaderResource[i]->GetResourceType() == ResourceType::kIndexBuffer) {
			IndexBuffer* ib = static_cast<IndexBuffer*>(shaderResource[i]);
			int start = offset;
			for (int j = 0; j < ib->getResourceCount(); j++) {
				D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};

				srvDesc.Format = DXGI_FORMAT_UNKNOWN;
				srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
				srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
				srvDesc.Buffer.FirstElement = 0;
				srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
				srvDesc.Buffer.NumElements = ib->getIndexBufferView(j)->SizeInBytes / sizeof(int);
				srvDesc.Buffer.StructureByteStride = sizeof(int);

				device->getDevice()->CreateShaderResourceView(ib->getResource(j), &srvDesc, m_shaderResourceHeap.getCpuHandle(offset));

				offset++;

				if (ib->getIsUnorderedAccess()) {
					D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
					uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
					uavDesc.Format = DXGI_FORMAT_UNKNOWN;
					uavDesc.Buffer.CounterOffsetInBytes = 0;
					uavDesc.Buffer.FirstElement = 0;
					uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
					uavDesc.Buffer.NumElements = ib->getIndexBufferView(j)->SizeInBytes / sizeof(int);
					uavDesc.Buffer.StructureByteStride = sizeof(int);

					device->getDevice()->CreateUnorderedAccessView(ib->getResource(j), nullptr, &uavDesc, m_shaderResourceHeap.getCpuHandle(offset));

					offset++;
				}
			}
			m_shaderResourceDescriptorTable[ib->getId()] = { start, offset };

		}
	}

	offset = 0;
	for (int i = 0; i < (int)renderTarget.size(); i++) {
		Texture* tex = static_cast<Texture*>(renderTarget[i]);

		int start = offset;
		for (int j = 0; j < tex->getResourceCount(); j++) {
			if (tex->getDepth() == 1) {
				D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
				rtvDesc.Format = tex->getFormat();
				rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
				rtvDesc.Texture2D.MipSlice = 0;
				rtvDesc.Texture2D.PlaneSlice = 0;

				auto rtvHandle = m_rtvHeap.getCpuHandle(offset);
				device->getDevice()->CreateRenderTargetView(tex->getResource(j), &rtvDesc, rtvHandle);

				offset++;
			}
			else {
				for (int k = 0; k < tex->getDepth(); k++) {
					D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
					rtvDesc.Format = tex->getFormat();
					rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
					rtvDesc.Texture2DArray.FirstArraySlice = k;
					rtvDesc.Texture2DArray.ArraySize = 1;

					auto rtvHandle = m_rtvHeap.getCpuHandle(offset);
					device->getDevice()->CreateRenderTargetView(tex->getResource(j), &rtvDesc, rtvHandle);

					offset++;
				}
			}
		}

		m_renderTargetDescriptorTable[tex->getId()] = { start, offset - start };
	}

	offset = 0;
	for (int i = 0; i < (int)depthStencil.size(); i++) {
		Texture* tex = static_cast<Texture*>(depthStencil[i]);

		int start = offset;
		for (int j = 0; j < tex->getResourceCount(); j++) {
			D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
			if (tex->getFormat() == DXGI_FORMAT_R32_TYPELESS) {
				dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
				dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
				dsvDesc.Texture2D.MipSlice = 0;
			}
			else if (tex->getFormat() == DXGI_FORMAT_R24G8_TYPELESS) {
				dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
				dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
				dsvDesc.Texture2D.MipSlice = 0;
			}
			else if (tex->getFormat() == DXGI_FORMAT_R32G32_TYPELESS) {
				dsvDesc.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
				dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
				dsvDesc.Texture2D.MipSlice = 0;
			}

			auto dsvHandle = m_dsvHeap.getCpuHandle(offset);
			device->getDevice()->CreateDepthStencilView(tex->getResource(j), &dsvDesc, dsvHandle);

			offset++;
		}

		m_depthStencilDescriptorTable[tex->getId()] = { start, tex->getResourceCount() };
	}

	offset = 0;
	m_samplerDescriptorTable.clear();
	for (auto& ite : m_samplerStateTable) {
		auto samplerHandle = m_samplerHeap.getCpuHandle(offset);

		device->getDevice()->CreateSampler(&ite.second, samplerHandle);

		m_samplerDescriptorTable[ite.first] = { offset, 1 };

		offset++;
	}
}