#include "GraphicsDevice.hpp"
#include "BufferManager.hpp"

#include <engine/graphics/IShader.hpp>
#include <engine/graphics/ShaderProgram.hpp>
#include <engine/graphics/VertexShader.hpp>
#include <engine/graphics/PixelShader.hpp>
#include <engine/graphics/PipelineStateManager.hpp>

#include <memory>
#include <format>
#include <d3d11sdklayers.h>
#include <dxgi1_6.h>
#include <d3d11.h>
#include <cstdint>
#include <wrl/client.h>
#include <dxgidebug.h>
#include <Windows.h>
#include <utils/Log.hpp>
#include <dxgi1_3.h>
#include <cstdlib>
#include <utility>
#include <d3dcommon.h>

void GraphicsDevice::Initialize() {
	InitDXGIFactory();
	InitD3D11Device();
	InitShaders();
	BufferManager::Initialize(m_device.Get());
	PipelineStateManager::Initialize(m_device.Get());
#ifdef _DEBUG
	EnableD3D11DebugLayer();
	EnableDXGIDebugLayer();
#endif
}

IDXGIFactory6* GraphicsDevice::DXGIFactory() {
	return m_dxgiFactory.Get();
}

ID3D11DeviceContext* GraphicsDevice::ImmediateContext() {
	return m_immediateContext.Get();
}

ID3D11Device* GraphicsDevice::D3D11Device() {
	return m_device.Get();
}

void GraphicsDevice::EnableDXGIDebugLayer() {
	HMODULE dxgiDebugDll = LoadLibraryW(L"dxgidebug.dll");
	if (!dxgiDebugDll) {
		return;
	}
	auto DXGIGetDebugInterface1Ptr = 
		reinterpret_cast<long(_stdcall*)(uint32_t, const IID&, void**)>(
			GetProcAddress(dxgiDebugDll, "DXGIGetDebugInterface1"));

	if (!DXGIGetDebugInterface1Ptr) {
		FreeLibrary(dxgiDebugDll);
		return;
	}
	Microsoft::WRL::ComPtr<IDXGIDebug1> dxgiDebug;
	HR_LOG(DXGIGetDebugInterface1Ptr(0, IID_PPV_ARGS(&dxgiDebug)));
	dxgiDebug->ReportLiveObjects(__uuidof(IDXGIDebug), DXGI_DEBUG_RLO_ALL);
	FreeLibrary(dxgiDebugDll);
}

void GraphicsDevice::EnableD3D11DebugLayer() {
	m_device.As(&m_d3d11Debug);
}

void GraphicsDevice::DumpD3D11LiveObjects() {
	if (m_d3d11Debug) {
		m_d3d11Debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
	}
}

Microsoft::WRL::ComPtr<IDXGIAdapter4> GraphicsDevice::PickBestAdapter() {
	Microsoft::WRL::ComPtr<IDXGIAdapter4> adapter;
	if (SUCCEEDED(m_dxgiFactory->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
		IID_PPV_ARGS(&adapter)))) {
		DXGI_ADAPTER_DESC3 desc{};
		adapter->GetDesc3(&desc);

		OutputDebugString(std::format(L"Chosen adapter: {} ({} GB VRAM)\n",
			desc.Description, desc.DedicatedVideoMemory / (1024.0 * 1024 * 1024)).c_str());
		return adapter;
	}
	return nullptr;
}

void GraphicsDevice::InitDXGIFactory() {
	uint32_t factoryFlags = 0;
#ifdef _DEBUG
	factoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif
	HR_LOG(CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&m_dxgiFactory)));
}

void GraphicsDevice::InitD3D11Device() {
	uint32_t deviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
	deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	D3D_FEATURE_LEVEL obtained{};
	static const D3D_FEATURE_LEVEL levels[] = {
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};

	Microsoft::WRL::ComPtr<IDXGIAdapter4> adapter = PickBestAdapter();
	HR_LOG(D3D11CreateDevice(adapter.Get(), D3D_DRIVER_TYPE_UNKNOWN, nullptr, deviceFlags,
		levels, _countof(levels), D3D11_SDK_VERSION, &m_device, &obtained, &m_immediateContext));
#ifdef _DEBUG
	Microsoft::WRL::ComPtr<ID3D11InfoQueue> infoQueue;
	HR_LOG(m_device.As(&infoQueue));
	infoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, TRUE);
	infoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, TRUE);
#endif
}

void GraphicsDevice::InitShaders() {
	std::unique_ptr<IShader> vertexShader = std::make_unique<VertexShader>();
	vertexShader->CompileFromFile(m_device.Get(), L"src\\shaders\\VertexShader.hlsl", "VSMain");
	ShaderProgram::AddShader(std::move(vertexShader));

	std::unique_ptr<IShader> pixelShader = std::make_unique<PixelShader>();
	pixelShader->CompileFromFile(m_device.Get(), L"src\\shaders\\PixelShader.hlsl", "PSMain");
	ShaderProgram::AddShader(std::move(pixelShader));

	std::unique_ptr<IShader> gridShader = std::make_unique<PixelShader>();
	gridShader->CompileFromFile(m_device.Get(), L"src\\shaders\\Grid.hlsl", "PSMain");
	ShaderProgram::AddShader(std::move(gridShader));
}