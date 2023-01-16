#ifndef _PIPELINE_H_
#define _PIPELINE_H_

#include <d3d12.h>
#include <dxcapi.h>

#include <wrl/client.h>
#include <vector>


enum class BlendState {
	eNone,
	eAdd,
	eLinear,
};

class Pipeline {
public:
	Pipeline() = default;
	~Pipeline() = default;

	bool create(ID3D12Device* device, ID3D12RootSignature* rootSignature);

	void addInputLayout(const char* name, DXGI_FORMAT format, UINT semanticIndex);
	void addRenderTargetFormat(DXGI_FORMAT format);
	void setDepthStencilFormat(DXGI_FORMAT format);
	void setDepthState(bool depthEnable, D3D12_COMPARISON_FUNC func);
	void setRasterState(D3D12_FILL_MODE fillMode, D3D12_CULL_MODE cullMode);
	void setStencilEnable(bool stencilEnable);
	void setStencilStateFront(D3D12_STENCIL_OP stencilDepthFail, D3D12_STENCIL_OP stencilFail, D3D12_STENCIL_OP pass, D3D12_COMPARISON_FUNC func);
	void setStencilStateBack(D3D12_STENCIL_OP stencilDepthFail, D3D12_STENCIL_OP stencilFail, D3D12_STENCIL_OP pass, D3D12_COMPARISON_FUNC func);
	void setStencilMask(UINT8 readMask, UINT8 writeMask);
	void setBlendState(BlendState blendState);
	void setVertexShader(IDxcBlob* blob);
	void setGeometoryShader(IDxcBlob* blob);
	void setPixelShader(IDxcBlob* blob);
	void setPrimitiveType(D3D12_PRIMITIVE_TOPOLOGY_TYPE type);

	ID3D12PipelineState* getPipelineState() { return m_pipelineState.Get(); }

private:
	Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipelineState;
	std::vector<D3D12_INPUT_ELEMENT_DESC> m_layout;
	D3D12_RASTERIZER_DESC m_rasterDesc;
	D3D12_COMPARISON_FUNC m_depthFunc;
	bool m_depthEnable;
	bool m_stencilEnable;
	D3D12_DEPTH_STENCILOP_DESC m_frontFace;
	D3D12_DEPTH_STENCILOP_DESC m_backFace;
	std::vector<DXGI_FORMAT> m_rtvFormat;
	DXGI_FORMAT m_depthStencilFormat;
	BlendState m_blendState = BlendState::eNone;
	IDxcBlob* m_vertexShader = nullptr;
	IDxcBlob* m_geometoryShader = nullptr;
	IDxcBlob* m_pixelShader = nullptr;
	D3D12_PRIMITIVE_TOPOLOGY_TYPE m_primitiveType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	UINT8 m_readMask;
	UINT8 m_writeMask;
};


class ComputePipeline {
public:
	ComputePipeline() = default;
	~ComputePipeline() = default;

	bool create(ID3D12Device* device, ID3D12RootSignature* rootSignature);

	void setComputeShader(IDxcBlob* blob);

	ID3D12PipelineState* getPipelineState() { return m_pipelineState.Get(); }

private:
	Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipelineState;
	IDxcBlob* m_computeShader;
};


#endif