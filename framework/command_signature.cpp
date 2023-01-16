#include "command_signature.h"


void CommandSignature::addConstantBuffer(UINT rootParameterIndex) {
	D3D12_INDIRECT_ARGUMENT_DESC desc{};
	desc.Type = D3D12_INDIRECT_ARGUMENT_TYPE_CONSTANT_BUFFER_VIEW;
	desc.ConstantBufferView.RootParameterIndex = rootParameterIndex;

	m_argDescArray.push_back(desc);
}

void CommandSignature::addShaderResource(UINT rootParameterIndex) {
	D3D12_INDIRECT_ARGUMENT_DESC desc{};
	desc.Type = D3D12_INDIRECT_ARGUMENT_TYPE_SHADER_RESOURCE_VIEW;
	desc.ShaderResourceView.RootParameterIndex = rootParameterIndex;

	m_argDescArray.push_back(desc);
}

void CommandSignature::addUnorderedAccess(UINT rootParameterIndex) {
	D3D12_INDIRECT_ARGUMENT_DESC desc{};
	desc.Type = D3D12_INDIRECT_ARGUMENT_TYPE_UNORDERED_ACCESS_VIEW;
	desc.UnorderedAccessView.RootParameterIndex = rootParameterIndex;

	m_argDescArray.push_back(desc);
}

void CommandSignature::addVertexBuffer(UINT slotIndex) {
	D3D12_INDIRECT_ARGUMENT_DESC desc{};
	desc.Type = D3D12_INDIRECT_ARGUMENT_TYPE_VERTEX_BUFFER_VIEW;
	desc.VertexBuffer.Slot = slotIndex;

	m_argDescArray.push_back(desc);
}

void CommandSignature::addDrawCommand() {
	D3D12_INDIRECT_ARGUMENT_DESC desc{};
	desc.Type = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW;

	m_argDescArray.push_back(desc);
}

void CommandSignature::addDrawIndexedCommand() {
	D3D12_INDIRECT_ARGUMENT_DESC desc{};
	desc.Type = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW_INDEXED;

	m_argDescArray.push_back(desc);
}

void CommandSignature::addDispatchCommand() {
	D3D12_INDIRECT_ARGUMENT_DESC desc{};
	desc.Type = D3D12_INDIRECT_ARGUMENT_TYPE_DISPATCH;

	m_argDescArray.push_back(desc);
}

bool CommandSignature::createDrawIndirect(ID3D12Device* device, ID3D12RootSignature* rootSignature) {
	HRESULT res;

	D3D12_COMMAND_SIGNATURE_DESC csDesc{};

	csDesc.ByteStride = sizeof(D3D12_DRAW_INDEXED_ARGUMENTS) + sizeof(uint64_t);
	csDesc.NumArgumentDescs = m_argDescArray.size();
	csDesc.pArgumentDescs = m_argDescArray.data();
	csDesc.NodeMask = 0;

	res = device->CreateCommandSignature(&csDesc, rootSignature, IID_PPV_ARGS(m_commandSignature.ReleaseAndGetAddressOf()));
	if (FAILED(res)) {
		return false;
	}

	return true;
}

bool CommandSignature::createDispatchIndirect(ID3D12Device* device, ID3D12RootSignature* rootSignature) {
	HRESULT res;

	D3D12_COMMAND_SIGNATURE_DESC csDesc{};

	D3D12_INDIRECT_ARGUMENT_DESC args[1];
	args[0].Type = D3D12_INDIRECT_ARGUMENT_TYPE_DISPATCH;

	csDesc.ByteStride = sizeof(D3D12_DISPATCH_ARGUMENTS);
	csDesc.NumArgumentDescs = ARRAYSIZE(args);
	csDesc.pArgumentDescs = args;
	csDesc.NodeMask = 1;

	res = device->CreateCommandSignature(&csDesc, rootSignature, IID_PPV_ARGS(m_commandSignature.ReleaseAndGetAddressOf()));
	if (FAILED(res)) {
		return false;
	}

	return true;
}