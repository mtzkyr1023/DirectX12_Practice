#ifndef _RENDER_PASS_H_
#define _RENDER_PASS_H_

#include "framework/device.h"
#include "framework/commandbuffer.h"
#include "framework/descriptor_heap.h"
#include "framework/pipeline.h"
#include "framework/queue.h"
#include "framework/root_signature.h"
#include "framework/shader.h"
#include "framework/buffer.h"
#include "framework/texture.h"
#include "framework/fence.h"


#include "glm-master/glm/glm.hpp"
#include "glm-master/glm/gtc/matrix_transform.hpp"
#include "glm-master/glm/gtc/quaternion.hpp"

#include <unordered_map>
#include <memory>
#include <algorithm>

static const char* BACK_BUFFER = "back_buffer";
static const char* ALBEDO_BUFFER = "albedo_bufer";
static const char* NORMAL_BUFFER = "normal_buffer";
static const char* PBR_BUFFER = "pbr_buffer";
static const char* DEPTH_BUFFER = "depth_buffer";
static const char* SAMPLE_TEXTURE = "sample_texture";
static const char* VIEW_PROJ_BUFFER = "view_proj_buffer";

static const char* TEST_VS = "test_vs";
static const char* TEST_PS = "test_ps";

class RenderPass {
public:
	virtual ~RenderPass() = default;

	virtual void render(ID3D12GraphicsCommandList* command, UINT curImageCount) = 0;

	virtual void run(UINT curImageCount) = 0;

public:
	struct ViewProjBuffer {
		glm::mat4 view;
		glm::mat4 proj;
		glm::mat4 padding1;
		glm::mat4 padding2;
	};

protected:
	RootSignature m_rootSignature;
	Pipeline m_pipeline;
	UINT m_width;
	UINT m_height;
};


class ResourceManager {
private:
	ResourceManager() :
		m_uniqueId(0)
	{}
	~ResourceManager() = default;

public:
	static ResourceManager& Instance() {
		static ResourceManager instance;
		return instance;
	}

	struct DescTableInfo {
		int start;
		int offset;
	};

	int createConstantBuffer(ID3D12Device* device, UINT size, UINT backBufferCount);
	int createStructuredBuffer(ID3D12Device* device, ID3D12CommandQueue* queue, UINT bufferCount, UINT stride, UINT elemCount, void* data);
	int createStructuredBuffer(ID3D12Device* device, UINT bufferCount, UINT stride, UINT elementCount, bool isCpuAccess, bool isUnorderedAccess);
	int createAppendStructuredBuffer(ID3D12Device* device, UINT bufferCount, UINT stride, UINT elementCount, bool isCpuAccess, bool isUnorderedAccess);
	int createByteAddressBuffer(ID3D12Device* device, DXGI_FORMAT format, UINT bufferCount, UINT size, bool isUnorderedAccess);
	int createVertexBuffer(ID3D12Device* device, ID3D12CommandQueue* queue, UINT bufferCount, UINT stride, UINT size, void* data);
	int createIndexBuffer(ID3D12Device* device, ID3D12CommandQueue* queue, UINT bufferCount, UINT stride, UINT size, void* data);
	int createBackBuffer(ID3D12Device* device, IDXGISwapChain3* swapchain, UINT backBufferCount);
	int createDepthStencilBuffer(ID3D12Device* device, UINT textureCount, UINT width, UINT height, bool isStencil);
	int createRenderTarget2D(ID3D12Device* device, UINT resourceCount, D3D12_RESOURCE_FLAGS flags, DXGI_FORMAT format, UINT width, UINT height);
	int createRenderTarget2DArray(ID3D12Device* device, UINT resourceCount, D3D12_RESOURCE_FLAGS flags, DXGI_FORMAT format, UINT width, UINT height, UINT depth);
	int createTexture(ID3D12Device* device, ID3D12CommandQueue* queue, UINT resourceCount, DXGI_FORMAT format,
		const char* filename, bool isMipmap);
	int createTexture(ID3D12Device* device, ID3D12CommandQueue* queue, UINT resourceCount, UINT width, UINT height, UINT componentCount, void* data, bool isMipmap);
	int createCubeMap(ID3D12Device* device, ID3D12CommandQueue* queue, std::vector<std::string>& filenames,
		DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM, bool isUnorderedAcces = false);

	int addSamplerState(D3D12_SAMPLER_DESC samplerState);

	int addVertexShader(const wchar_t* filename);
	int addPixelShader(const wchar_t* filename);
	int addComputeShader(const wchar_t* filename);

	Resource* getResource(int id) { return m_resourceArray[id].get(); }
	Texture* getResourceAsTexture(int id) { return static_cast<Texture*>(m_resourceArray[id].get()); }
	ConstantBuffer* getResourceAsCB(int id) { return static_cast<ConstantBuffer*>(m_resourceArray[id].get()); }
	StructuredBuffer* getResourceAsStuructured(int id) { return static_cast<StructuredBuffer*>(m_resourceArray[id].get()); }
	ByteAddressBuffer* getResourceAsByteAddressBuffer(int id) { return static_cast<ByteAddressBuffer*>(m_resourceArray[id].get()); }

	ShaderSp GetShader(int id) { return m_shaderTable[id]; }

	void updateDescriptorHeap(Device* device, int globalHeapCount);
	DescriptorHeap* getRtvHeap() { return &m_rtvHeap; }
	DescriptorHeap* getShaderResourceHeap() { return &m_shaderResourceHeap; }
	DescriptorHeap* getSamplerHeap() { return &m_samplerHeap; }
	DescriptorHeap* getGlobalHeap() { return &m_globalHeap; }

	const D3D12_CPU_DESCRIPTOR_HANDLE getShaderResourceCpuHandle(int id, int index) {
		auto& offsetTable = m_shaderResourceDescriptorTable[id];
		return m_shaderResourceHeap.getCpuHandle(offsetTable.start + std::min(offsetTable.offset, index));
	}

	const D3D12_GPU_DESCRIPTOR_HANDLE getShaderResourceGpuHandle(int id, int index) {
		auto& offsetTable = m_shaderResourceDescriptorTable[id];
		return m_shaderResourceHeap.getGpuHandle(offsetTable.start + std::min(offsetTable.offset, index));
	}

	const D3D12_CPU_DESCRIPTOR_HANDLE getRenderTargetCpuHandle(int id, int index) {
		auto& offsetTable = m_renderTargetDescriptorTable[id];
		return m_rtvHeap.getCpuHandle(offsetTable.start + std::min(offsetTable.offset, index));
	}

	const D3D12_GPU_DESCRIPTOR_HANDLE getRenderTargetGpuHandle(int id, int index) {
		auto& offsetTable = m_renderTargetDescriptorTable[id];
		return m_rtvHeap.getGpuHandle(offsetTable.start + std::min(offsetTable.offset, index));
	}

	const D3D12_CPU_DESCRIPTOR_HANDLE getDepthStencilCpuHandle(int id, int index) {
		auto& offsetTable = m_depthStencilDescriptorTable[id];
		return m_dsvHeap.getCpuHandle(offsetTable.start + std::min(offsetTable.offset, index));
	}

	const D3D12_GPU_DESCRIPTOR_HANDLE getDepthStencilGpuHandle(int id, int index) {
		auto& offsetTable = m_depthStencilDescriptorTable[id];
		return m_dsvHeap.getGpuHandle(offsetTable.start + std::min(offsetTable.offset, index));
	}

	const D3D12_CPU_DESCRIPTOR_HANDLE getSamplerStateCpuHandle(int id, int index) {
		auto& offsetTable = m_samplerDescriptorTable[id];
		return m_samplerHeap.getCpuHandle(offsetTable.start + std::min(offsetTable.offset, index));
	}

	const D3D12_GPU_DESCRIPTOR_HANDLE getSamplerStateGpuHandle(int id, int index) {
		auto& offsetTable = m_samplerDescriptorTable[id];
		return m_samplerHeap.getGpuHandle(offsetTable.start + std::min(offsetTable.offset, index));
	}

	const D3D12_GPU_DESCRIPTOR_HANDLE getGlobalHeap(int dest, int id, int index) {
		auto& offsetTable = m_shaderResourceDescriptorTable[id];
		m_globalHeap.copyDescriptors(dest, m_shaderResourceHeap.getCpuHandle(offsetTable.start + std::min(offsetTable.offset, index)));
		return m_globalHeap.getGpuHandle(dest);
	}

	const DescTableInfo& getShaderResourceTableInfo(int id) { return m_shaderResourceDescriptorTable[id]; }
	const DescTableInfo& getRenderTargetTableInfo(int id) { return m_renderTargetDescriptorTable[id]; }
	const DescTableInfo& getDepthStencilTableInfo(int id) { return m_depthStencilDescriptorTable[id]; }

private:

	std::unordered_map<int, std::unique_ptr<Resource>> m_resourceArray;
	std::unordered_map<int, D3D12_SAMPLER_DESC> m_samplerStateTable;
	std::unordered_map<int, std::shared_ptr<Shader>> m_shaderTable;

	std::unordered_map<int, DescTableInfo> m_shaderResourceDescriptorTable;
	std::unordered_map<int, DescTableInfo> m_unorderedAccessDescriptorTable;
	std::unordered_map<int, DescTableInfo> m_renderTargetDescriptorTable;
	std::unordered_map<int, DescTableInfo> m_depthStencilDescriptorTable;
	std::unordered_map<int, DescTableInfo> m_samplerDescriptorTable;

	DescriptorHeap m_shaderResourceHeap;
	DescriptorHeap m_dsvHeap;
	DescriptorHeap m_rtvHeap;
	DescriptorHeap m_samplerHeap;

	DescriptorHeap m_globalHeap;

	int m_uniqueId;
};

#endif