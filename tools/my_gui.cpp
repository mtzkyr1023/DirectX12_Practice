#include "my_gui.h"

#include "../imgui_dx12/imgui.h"
#include "../imgui_dx12/imgui_impl_win32.h"
#include "../imgui_dx12/imgui_impl_dx12.h"

bool MyGui::create(HWND hwnd, ID3D12Device* device, DXGI_FORMAT format, UINT backBufferCount, ID3D12DescriptorHeap* descHeap) {
	HRESULT res;

	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	desc.NumDescriptors = 1024;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	res = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(m_srvDescHeap.ReleaseAndGetAddressOf()));
	if (FAILED(res))
		return false;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX12_Init(device, backBufferCount,
		format,
		m_srvDescHeap->GetCPUDescriptorHandleForHeapStart(),
		m_srvDescHeap->GetGPUDescriptorHandleForHeapStart());

	ImGui::StyleColorsDark();
	
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("");

	ImGui::End();

	ImGui::Render();

	return true;
}

void MyGui::destroy() {
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void MyGui::renderFrame(ID3D12GraphicsCommandList* command) {
	command->SetDescriptorHeaps(1, m_srvDescHeap.GetAddressOf());
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), command);
}