#include "device.h"


bool Device::create() {
	HRESULT res;

	UUID experimentalFeatures[] = { D3D12ExperimentalShaderModels };
	res = D3D12EnableExperimentalFeatures(_countof(experimentalFeatures), experimentalFeatures, NULL, NULL);


	UINT flagsDXGI = 0;

#ifdef _DEBUG
	{
		Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
		res = D3D12GetDebugInterface(IID_PPV_ARGS(debugController.ReleaseAndGetAddressOf()));
		if (FAILED(res)) {
			MessageBox(NULL, "failed getting debug interface.", "Error.", MB_OK);
			return false;
		}

		debugController->EnableDebugLayer();
	}

	flagsDXGI |= DXGI_CREATE_FACTORY_DEBUG;
#endif

	res = CreateDXGIFactory2(flagsDXGI, IID_PPV_ARGS(m_factory.ReleaseAndGetAddressOf()));

	res = m_factory->EnumAdapters(0, (IDXGIAdapter**)m_adapter.ReleaseAndGetAddressOf());

	res = D3D12CreateDevice(m_adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(m_device.ReleaseAndGetAddressOf()));
	if (FAILED(res)) {
		MessageBox(NULL, "failed creating device.", "Error", MB_OK);
		return false;
	}

	return true;
}