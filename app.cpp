

static const int kBackBufferCount = 3;
static const int kScreenWidth = 1920;
static const int kScreenHeight = 1080;

#include <random>
#include <utility>
#include <algorithm>
#include <thread>

#include "tools/input.h"

#include "imgui_dx12/imgui.h"
#include "imgui_dx12/imgui_impl_win32.h"
#include "imgui_dx12/imgui_impl_dx12.h"


#include "resource_manager.h"

#include "App.hpp"

App::App() {

}

App::~App() {

}

bool App::initialize(HWND hwnd) {

	m_device.create();

	m_queue.createGraphicsQueue(m_device.getDevice());

	m_swapchain.create(m_queue.getQueue(), hwnd, kBackBufferCount, kScreenWidth, kScreenHeight, false);

	auto& resMgr = ResourceManager::Instance();

	m_commandAllocator.resize(kBackBufferCount);
	m_commandList.resize(kBackBufferCount);
	for (int i = 0; i < kBackBufferCount; i++) {
		m_commandAllocator[i].createGraphicsCommandAllocator(m_device.getDevice());
		m_commandList[i].createGraphicsCommandList(m_device.getDevice(), m_commandAllocator[i].getCommandAllocator());
	}

	m_presentFence.create(m_device.getDevice());


	m_backBuffer = resMgr.createBackBuffer(m_device.getDevice(), m_swapchain.getSwapchain(), kBackBufferCount);

	m_depthBuffer = resMgr.createDepthStencilBuffer(m_device.getDevice(), kBackBufferCount, kScreenWidth, kScreenHeight, false);

	m_cb0 = resMgr.createConstantBuffer(m_device.getDevice(), sizeof(glm::mat4) * 4, kBackBufferCount);

	m_visibilityBuffer = resMgr.createRenderTarget2D(m_device.getDevice(), kBackBufferCount, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		DXGI_FORMAT_R32G32_UINT, kScreenWidth, kScreenHeight);

	m_model.create(m_device.getDevice(), m_queue.getQueue(), "models/sponza/gltf/", "models/sponza/gltf/sponza.gltf");

	{

		D3D12_SAMPLER_DESC samplerDesc{};
		samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		samplerDesc.BorderColor[0] = 0.0f;
		samplerDesc.BorderColor[1] = 0.0f;
		samplerDesc.BorderColor[2] = 0.0f;
		samplerDesc.BorderColor[3] = 0.0f;
		samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		samplerDesc.Filter = D3D12_FILTER_ANISOTROPIC;
		samplerDesc.MaxAnisotropy = 16.0f;
		samplerDesc.MaxLOD = FLT_MAX;
		samplerDesc.MinLOD = 0.0f;
		samplerDesc.MipLODBias = 0.0f;

		m_wrapSampler = resMgr.addSamplerState(samplerDesc);
	}

	m_vs = resMgr.addVertexShader(L"shaders/vs.fx");
	m_ps = resMgr.addPixelShader(L"shaders/ps.fx");
	m_materialCountCS = resMgr.addComputeShader(L"shaders/material_count_cs.fx");
	m_materialSortCS = resMgr.addComputeShader(L"shaders/material_sort_cs.fx");
	m_renderingCS = resMgr.addComputeShader(L"shaders/rendering_cs.fx");

	m_rootSignature.addDescriptorCount(D3D12_SHADER_VISIBILITY_ALL, D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 0, 1);
	m_rootSignature.addDescriptorCount(D3D12_SHADER_VISIBILITY_ALL, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1);
	m_rootSignature.addDescriptorCount(D3D12_SHADER_VISIBILITY_ALL, D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 0, 1);
	m_rootSignature.create(m_device.getDevice(),
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS);

	ShaderSp vs = resMgr.GetShader(m_vs);
	ShaderSp ps = resMgr.GetShader(m_ps);

	m_pipeline.addInputLayout("POSITION", DXGI_FORMAT_R32G32B32_FLOAT, 0);
	m_pipeline.addInputLayout("NORMAL", DXGI_FORMAT_R32G32B32_FLOAT, 0);
	m_pipeline.addInputLayout("TANGENT", DXGI_FORMAT_R32G32B32_FLOAT, 0);
	m_pipeline.addInputLayout("TEXCOORD", DXGI_FORMAT_R32G32_FLOAT, 0);
	m_pipeline.addRenderTargetFormat(DXGI_FORMAT_R32G32_UINT);
	m_pipeline.setBlendState(BlendState::eNone);
	m_pipeline.setDepthState(true, D3D12_COMPARISON_FUNC_LESS_EQUAL);
	m_pipeline.setDepthStencilFormat(DXGI_FORMAT_D32_FLOAT);
	m_pipeline.setStencilEnable(false);
	m_pipeline.setRasterState(D3D12_FILL_MODE_SOLID, D3D12_CULL_MODE_FRONT);
	m_pipeline.setVertexShader(vs->getByteCode());
	m_pipeline.setPixelShader(ps->getByteCode());
	m_pipeline.setPrimitiveType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

	m_pipeline.create(m_device.getDevice(), m_rootSignature.getRootSignature());

	m_materialCountRS.addDescriptorCount(D3D12_SHADER_VISIBILITY_ALL, D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 0, 1);
	m_materialCountRS.addDescriptorCount(D3D12_SHADER_VISIBILITY_ALL, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1);
	m_materialCountRS.addDescriptorCount(D3D12_SHADER_VISIBILITY_ALL, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);
	m_materialCountRS.addDescriptorCount(D3D12_SHADER_VISIBILITY_ALL, D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 0, 1);

	m_materialCountRS.create(m_device.getDevice(),
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS);

	ShaderSp materialCountCS = resMgr.GetShader(m_materialCountCS);
	ShaderSp materialSortCS = resMgr.GetShader(m_materialSortCS);
	ShaderSp renderingCS = resMgr.GetShader(m_renderingCS);

	m_materialSortRS.addDescriptorCount(D3D12_SHADER_VISIBILITY_ALL, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1);
	m_materialSortRS.addDescriptorCount(D3D12_SHADER_VISIBILITY_ALL, D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 0, 1);
	m_materialSortRS.addDescriptorCount(D3D12_SHADER_VISIBILITY_ALL, D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 1);
	m_materialSortRS.addDescriptorCount(D3D12_SHADER_VISIBILITY_ALL, D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 2, 1);

	m_materialSortRS.create(m_device.getDevice(),
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS);

	m_renderingRS.addDescriptorCount(D3D12_SHADER_VISIBILITY_ALL, D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 0, 1);
	m_renderingRS.addDescriptorCount(D3D12_SHADER_VISIBILITY_ALL, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);
	m_renderingRS.addDescriptorCount(D3D12_SHADER_VISIBILITY_ALL, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 2, 1);
	m_renderingRS.addDescriptorCount(D3D12_SHADER_VISIBILITY_ALL, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 3, 1);
	m_renderingRS.addDescriptorCount(D3D12_SHADER_VISIBILITY_ALL, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 4, 1);
	m_renderingRS.addDescriptorCount(D3D12_SHADER_VISIBILITY_ALL, D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 0, 1);
	m_renderingRS.addDescriptorCount(D3D12_SHADER_VISIBILITY_ALL, D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 0, 1);
	m_renderingRS.create(m_device.getDevice(),
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS);


	m_materialCountPipeline.setComputeShader(materialCountCS->getByteCode());
	m_materialCountPipeline.create(m_device.getDevice(), m_materialCountRS.getRootSignature());

	m_materialSortPipeline.setComputeShader(materialSortCS->getByteCode());
	m_materialSortPipeline.create(m_device.getDevice(), m_materialSortRS.getRootSignature());

	m_renderingPipeline.setComputeShader(renderingCS->getByteCode());
	m_renderingPipeline.create(m_device.getDevice(), m_renderingRS.getRootSignature());

	resMgr.updateDescriptorHeap(&m_device, 1024 * kBackBufferCount);


	m_gui.create(hwnd, m_device.getDevice(), DXGI_FORMAT_R8G8B8A8_UNORM, kBackBufferCount, resMgr.getGlobalHeap()->getDescriptorHeap());

	return true;
}

void App::shutdown() {
	m_queue.waitForFence(m_presentFence.getFence(), m_presentFence.getFenceEvent(), m_presentFence.getFenceValue());
	m_gui.destroy();
}

void App::render() {
	UINT curImageCount = m_swapchain.getSwapchain()->GetCurrentBackBufferIndex();

	this->run((curImageCount + 1) % kBackBufferCount);

	static int heapIndex;
	if (curImageCount == 0) {
		heapIndex = 0;
	}

	std::thread th1([this]() {
		auto& resMgr = ResourceManager::Instance();
	UINT curImageCount = (m_swapchain.getSwapchain()->GetCurrentBackBufferIndex() + 1) % kBackBufferCount;
	m_commandAllocator[curImageCount].getCommandAllocator()->Reset();
	ID3D12GraphicsCommandList* command = m_commandList[curImageCount].getCommandList();
	command->Reset(m_commandAllocator[curImageCount].getCommandAllocator(), nullptr);

	{
		D3D12_VIEWPORT viewport{};
		viewport.Width = (float)kScreenWidth;
		viewport.Height = (float)kScreenHeight;
		viewport.MaxDepth = 1.0f;
		viewport.MinDepth = 0.0f;
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;

		D3D12_RECT scissor = { 0, 0, kScreenWidth, kScreenHeight };

		command->RSSetViewports(1, &viewport);
		command->RSSetScissorRects(1, &scissor);

		ID3D12DescriptorHeap* descHeaps[] = {
			resMgr.getGlobalHeap()->getDescriptorHeap(),
			resMgr.getSamplerHeap()->getDescriptorHeap(),
		};

		size_t heapCount = sizeof(descHeaps) / sizeof(descHeaps[0]);
		command->SetDescriptorHeaps(heapCount, descHeaps);


		Texture* backBuffer = static_cast<Texture*>(resMgr.getResource(m_backBuffer));
		Texture* depthBuffer = static_cast<Texture*>(resMgr.getResource(m_depthBuffer));
		backBuffer->transitionResource(command, curImageCount, D3D12_RESOURCE_BARRIER_FLAG_NONE, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		depthBuffer->transitionResource(command, curImageCount, D3D12_RESOURCE_BARRIER_FLAG_NONE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_DEPTH_WRITE);

		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle[] = {
	resMgr.getRenderTargetCpuHandle(m_backBuffer, curImageCount)
		};
		D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = resMgr.getDepthStencilCpuHandle(m_depthBuffer, curImageCount);
		command->OMSetRenderTargets(1, rtvHandle, false, &dsvHandle);

		float clearcolor[] = { 0.5f, 0.5f, 0.0f, 0.0f };
		command->ClearRenderTargetView(rtvHandle[0], clearcolor, 0, nullptr);
		command->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

		auto vertexBuffer = static_cast<VertexBuffer*>(resMgr.getResource(m_model.vertexBuffer()));
		auto indexBuffer = static_cast<IndexBuffer*>(resMgr.getResource(m_model.indexBuffer()));

		command->SetGraphicsRootSignature(m_rootSignature.getRootSignature());

		command->SetPipelineState(m_pipeline.getPipelineState());

		command->SetGraphicsRootDescriptorTable(0, resMgr.getGlobalHeap((heapIndex++) % 1024, m_cb0, curImageCount));
		command->SetGraphicsRootDescriptorTable(2, resMgr.getSamplerStateGpuHandle(m_wrapSampler, 0));


		command->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		command->IASetVertexBuffers(0, 1, vertexBuffer->getVertexBuferView(0));
		command->IASetIndexBuffer(indexBuffer->getIndexBufferView(0));

		int vertexOffset = 0;
		int indexOffset = 0;
		for (int i = 0; i < m_model.meshCount(); i++) {
			command->SetGraphicsRootDescriptorTable(1, resMgr.getGlobalHeap((heapIndex++) % 1024, m_model.normalIndex(m_model.materialIndex(i)), 0));

			command->DrawIndexedInstanced(m_model.indexCount(i), 1, indexOffset, vertexOffset, 0);

			vertexOffset += m_model.vertexCount(i);
			indexOffset += m_model.indexCount(i);
		}

		depthBuffer->transitionResource(command, curImageCount, D3D12_RESOURCE_BARRIER_FLAG_NONE, D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

		m_gui.renderFrame(command);

		backBuffer->transitionResource(command, curImageCount, D3D12_RESOURCE_BARRIER_FLAG_NONE, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	}


	command->Close();
		});

	ID3D12GraphicsCommandList* command = m_commandList[curImageCount].getCommandList();

	ID3D12CommandList* cmdList[] = { command };
	m_queue.getQueue()->ExecuteCommandLists(_countof(cmdList), cmdList);


	m_queue.waitForFence(m_presentFence.getFence(), m_presentFence.getFenceEvent(), m_presentFence.getFenceValue());

	m_swapchain.getSwapchain()->Present(0, 0);

	th1.join();
}


void App::run(UINT curImageCount) {
	auto& resMgr = ResourceManager::Instance();
	{
		static float scl = 1.0f;
		static float pitch, yaw;

		pitch -= Input::Instance().GetMoveYLeftPushed() * ImGui::GetIO().DeltaTime;
		yaw += Input::Instance().GetMoveXLeftPushed() * ImGui::GetIO().DeltaTime;

		scl += Input::Instance().GetMoveXRightPushed() * ImGui::GetIO().DeltaTime;

		glm::quat rotation;
		rotation = glm::angleAxis(pitch, glm::vec3(1.0f, 0.0f, 0.0f)) * glm::angleAxis(yaw, glm::vec3(0.0f, 1.0f, 0.0f));

		auto cb0 = resMgr.getResourceAsCB(m_cb0);
		struct cb_t {
			glm::mat4 view;
			glm::mat4 proj;
			glm::mat4 world;
		};

		cb_t cb{};
		cb.view = glm::lookAt(glm::vec3(0.0f, 0.0f, -4.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		cb.proj = glm::perspective(glm::half_pi<float>(), 16.0f / 9.0f, 0.1f, 100.0f);
		cb.world = glm::scale(glm::identity<glm::mat4>(), glm::vec3(scl, scl, scl)) * glm::mat4(rotation);

		cb0->updateBuffer(curImageCount, sizeof(cb_t), &cb);
	}

	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();


	ImGui::Text("deltaTime: %.4f", ImGui::GetIO().DeltaTime);
	ImGui::Text("framerate: %.2f", ImGui::GetIO().Framerate);

	ImGui::Render();
}
