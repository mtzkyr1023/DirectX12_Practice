#include "pipeline.h"


bool Pipeline::create(ID3D12Device* device, ID3D12RootSignature* rootSignature) {
	HRESULT res;

	D3D12_RENDER_TARGET_BLEND_DESC rtblendDesc{};
	if (m_blendState == BlendState::eNone) {
		rtblendDesc.BlendEnable = FALSE;
		rtblendDesc.LogicOpEnable = FALSE;
		rtblendDesc.SrcBlend = D3D12_BLEND_ONE;
		rtblendDesc.DestBlend = D3D12_BLEND_ZERO;
		rtblendDesc.BlendOp = D3D12_BLEND_OP_ADD;
		rtblendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
		rtblendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
		rtblendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		rtblendDesc.LogicOp = D3D12_LOGIC_OP_NOOP;
		rtblendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	}
	else if (m_blendState == BlendState::eAdd) {
		rtblendDesc.BlendEnable = TRUE;
		rtblendDesc.LogicOpEnable = FALSE;
		rtblendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		rtblendDesc.DestBlend = D3D12_BLEND_ONE;
		rtblendDesc.BlendOp = D3D12_BLEND_OP_ADD;
		rtblendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
		rtblendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
		rtblendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		rtblendDesc.LogicOp = D3D12_LOGIC_OP_NOOP;
		rtblendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	}
	else if (m_blendState == BlendState::eLinear) {
		rtblendDesc.BlendEnable = TRUE;
		rtblendDesc.LogicOpEnable = FALSE;
		rtblendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		rtblendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		rtblendDesc.BlendOp = D3D12_BLEND_OP_ADD;
		rtblendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
		rtblendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
		rtblendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		rtblendDesc.LogicOp = D3D12_LOGIC_OP_NOOP;
		rtblendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	}

	D3D12_BLEND_DESC blendDesc{};
	blendDesc.AlphaToCoverageEnable = m_blendState == BlendState::eNone ? false : false;
	blendDesc.IndependentBlendEnable = false;
	for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; i++) {
		blendDesc.RenderTarget[i] = rtblendDesc;
	}

	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpsDesc{};
	gpsDesc.InputLayout = { m_layout.data(), (UINT)m_layout.size() };
	gpsDesc.pRootSignature = rootSignature;
	gpsDesc.VS = { m_vertexShader->GetBufferPointer(), m_vertexShader->GetBufferSize() };
	if(m_geometoryShader) gpsDesc.GS = { m_geometoryShader->GetBufferPointer(), m_geometoryShader->GetBufferSize() };
	gpsDesc.PS = { m_pixelShader->GetBufferPointer(), m_pixelShader->GetBufferSize() };
	gpsDesc.RasterizerState = m_rasterDesc;
	gpsDesc.BlendState = blendDesc;
	gpsDesc.DepthStencilState.DepthEnable = m_depthEnable;
	gpsDesc.DepthStencilState.StencilEnable = m_stencilEnable;
	gpsDesc.DepthStencilState.DepthFunc = m_depthFunc;
	gpsDesc.DepthStencilState.FrontFace = m_frontFace;
	gpsDesc.DepthStencilState.BackFace = m_backFace;
	gpsDesc.DepthStencilState.StencilReadMask = m_readMask;
	gpsDesc.DepthStencilState.StencilWriteMask = m_writeMask;
	gpsDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	gpsDesc.SampleMask = UINT_MAX;
	gpsDesc.PrimitiveTopologyType = m_primitiveType;
	gpsDesc.NumRenderTargets = (UINT)m_rtvFormat.size();
	for (UINT i = 0; i < 8 && i < m_rtvFormat.size(); i++)
		gpsDesc.RTVFormats[i] = m_rtvFormat[i];
	gpsDesc.DSVFormat = m_depthStencilFormat;
	gpsDesc.SampleDesc.Count = 1;
	gpsDesc.SampleDesc.Quality = 0;

	res = device->CreateGraphicsPipelineState(&gpsDesc, IID_PPV_ARGS(m_pipelineState.ReleaseAndGetAddressOf()));
	if (FAILED(res)) {
		return false;
	}

	return true;
}

void Pipeline::addInputLayout(const char* name, DXGI_FORMAT format, UINT semanticIndex) {
	D3D12_INPUT_ELEMENT_DESC layout{};
	layout.SemanticName = name;
	layout.SemanticIndex = semanticIndex;
	layout.Format = format;
	layout.InputSlot = 0;
	layout.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	layout.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	layout.InstanceDataStepRate = 0;

	m_layout.push_back(layout);
}

void Pipeline::addRenderTargetFormat(DXGI_FORMAT format) {
	m_rtvFormat.push_back(format);
}

void Pipeline::setDepthStencilFormat(DXGI_FORMAT format) {
	m_depthStencilFormat = format;
}

void Pipeline::setDepthState(bool depthEnable, D3D12_COMPARISON_FUNC func) {
	m_depthFunc = func;
	m_depthEnable = depthEnable;
}

void Pipeline::setStencilEnable(bool stencilEnable) {
	m_stencilEnable = stencilEnable;
}

void Pipeline::setStencilStateFront(D3D12_STENCIL_OP stencilDepthFail, D3D12_STENCIL_OP stencilFail, D3D12_STENCIL_OP pass, D3D12_COMPARISON_FUNC func) {
	m_frontFace.StencilDepthFailOp = stencilDepthFail;
	m_frontFace.StencilFailOp = stencilFail;
	m_frontFace.StencilPassOp = pass;
	m_frontFace.StencilFunc = func;
}

void Pipeline::setStencilStateBack(D3D12_STENCIL_OP stencilDepthFail, D3D12_STENCIL_OP stencilFail, D3D12_STENCIL_OP pass, D3D12_COMPARISON_FUNC func) {
	m_backFace.StencilDepthFailOp = stencilDepthFail;
	m_backFace.StencilFailOp = stencilFail;
	m_backFace.StencilPassOp = pass;
	m_backFace.StencilFunc = func;
}

void Pipeline::setStencilMask(UINT8 readMask, UINT8 writeMask) {
	m_readMask = readMask;
	m_writeMask = writeMask;
}

void Pipeline::setRasterState(D3D12_FILL_MODE fillMode, D3D12_CULL_MODE cullMode) {
	m_rasterDesc.FillMode = fillMode;
	m_rasterDesc.CullMode = cullMode;
	m_rasterDesc.FrontCounterClockwise = false;
	m_rasterDesc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
	m_rasterDesc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
	m_rasterDesc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	m_rasterDesc.DepthClipEnable = true;
	m_rasterDesc.MultisampleEnable = false;
	m_rasterDesc.AntialiasedLineEnable = false;
	m_rasterDesc.ForcedSampleCount = 0;
	m_rasterDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
}

void Pipeline::setBlendState(BlendState blendState) {
	m_blendState = blendState;
}

void Pipeline::setVertexShader(IDxcBlob* shader) {
	m_vertexShader = shader;
}

void Pipeline::setGeometoryShader(IDxcBlob* shader) {
	m_geometoryShader = shader;
}

void Pipeline::setPixelShader(IDxcBlob* shader) {
	m_pixelShader = shader;
}

void Pipeline::setPrimitiveType(D3D12_PRIMITIVE_TOPOLOGY_TYPE type) {
	m_primitiveType = type;
}


bool ComputePipeline::create(ID3D12Device* device, ID3D12RootSignature* rootSignature) {
	HRESULT res;

	D3D12_COMPUTE_PIPELINE_STATE_DESC cpsDesc{};
	cpsDesc.CS = { m_computeShader->GetBufferPointer(), m_computeShader->GetBufferSize() };
	cpsDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	cpsDesc.pRootSignature = rootSignature;

	res = device->CreateComputePipelineState(&cpsDesc, IID_PPV_ARGS(m_pipelineState.ReleaseAndGetAddressOf()));
	if (FAILED(res))
		return false;

	return true;
}

void ComputePipeline::setComputeShader(IDxcBlob* blob) {
	m_computeShader = blob;
}