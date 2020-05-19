#include "AdapterReader.h"

std::vector<AdapterData> AdapterReader::s_Adapters; //static º¯¼ö

AdapterData::AdapterData(IDXGIAdapter *pAdapter) {
	this->m_Adapter = pAdapter;
}

DXGI_ADAPTER_DESC AdapterData::GetDescription()
{
	DXGI_ADAPTER_DESC desc;
	HRESULT hr = m_Adapter->GetDesc(&desc);
	if (FAILED(hr)) {
		StringHelper::ErrorLog(hr, "Failed to Get Description for IDXGIAdapter.");
	}

	return desc;
}

std::vector<AdapterData> AdapterReader::GetAdapters() {
	if (s_Adapters.size() > 0) {
		return s_Adapters;
	}

	Microsoft::WRL::ComPtr<IDXGIFactory> pFactory;

	//Create a DXGIFactory object
	HRESULT hr = CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(pFactory.GetAddressOf()));
	if (FAILED(hr)) {
		StringHelper::ErrorLog(hr, "Failed to create DXGIFactory for enumerating adapters.");
		exit(-1);
	}

	IDXGIAdapter* pAdapter;
	UINT index = 0;
	while (SUCCEEDED(pFactory->EnumAdapters(index, &pAdapter))) {
		s_Adapters.push_back(AdapterData(pAdapter));
		index += 1;
	}
	return s_Adapters;
}


