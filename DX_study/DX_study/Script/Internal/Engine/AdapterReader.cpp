#include "AdapterReader.h"

std::vector<AdapterData> AdapterReader::s_Adapters; //static º¯¼ö

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

AdapterData::AdapterData(IDXGIAdapter *pAdapter) {
	this->m_Adapter = pAdapter;
	HRESULT hr = pAdapter->GetDesc(&this->m_Description);
	if (FAILED(hr)) {
		StringHelper::ErrorLog(hr, "Failed to Get Description for IDXGIAdapter.");
	}
}