#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "resource.h"
#include "swapchain.h"

class Texture : public Resource {
public:
	Texture() {
		m_resourceType = ResourceType::kTexture;
	}
	Texture(const char* name) : Resource(name) {
		m_resourceType = ResourceType::kTexture;
	}
	~Texture() = default;

	bool createBackBuffer(ID3D12Device* device, IDXGISwapChain3* swapchain, UINT textureCount);
	bool createRenderTarget2D(ID3D12Device* device, UINT textureCount, D3D12_RESOURCE_FLAGS flags, DXGI_FORMAT format,
		UINT width, UINT height);
	bool createRenderTarget2DArray(ID3D12Device* device, UINT textureCount, D3D12_RESOURCE_FLAGS flags, DXGI_FORMAT format,
		UINT width, UINT height, UINT depth);
	bool createRenderTarget3D(ID3D12Device* device, UINT textureCount, D3D12_RESOURCE_FLAGS flags, DXGI_FORMAT format,
		UINT width, UINT height, UINT depth);
	bool createDepthStencilBuffer(ID3D12Device* device, UINT textureCount, DXGI_FORMAT format,
		UINT width, UINT height);
	bool createResource(ID3D12Device* device, ID3D12CommandQueue* queue, UINT textureCount,
		UINT width, UINT height, UINT componentCount, void* data, bool isMipmap);
	bool createResource(ID3D12Device* device, ID3D12CommandQueue* queue, UINT textureCount, DXGI_FORMAT format,
		const char* filename, bool isMipmap);
	bool createCubeMap(ID3D12Device* device, ID3D12CommandQueue* queue, const std::vector<std::string>& filenames,
		DXGI_FORMAT format, bool isUnorderedAccess = false);

	void transitionResource(ID3D12GraphicsCommandList* command, UINT textureNum, D3D12_RESOURCE_BARRIER_FLAGS flag,
		D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after);

	uint32_t getWidth() { return m_width; }
	uint32_t getHeight() { return m_height; }
	uint32_t getDepth() { return m_depth; }

	uint32_t getMipCount() { return m_mipCount; }

	bool isShaderResource() { return m_isShaderResource; }
	bool isUnorderedAccess() { return m_isUnorderedAccess; }
	bool isRenderTarget() { return m_isRenderTarget; }
	bool isDepthStencil() { return m_isDepthStencil; }

	DXGI_FORMAT getFormat() { return m_format; }

private:
	uint32_t m_width;
	uint32_t m_height;
	uint32_t m_depth;

	uint32_t m_mipCount;

	bool m_isShaderResource;
	bool m_isUnorderedAccess;
	bool m_isRenderTarget;
	bool m_isDepthStencil;

	DXGI_FORMAT m_format;
};

#endif