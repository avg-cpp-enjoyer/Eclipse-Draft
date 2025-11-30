#include "GraphicsDevice.hpp"

#include <engine/graphics/IShader.hpp>
#include <engine/graphics/ShaderProgram.hpp>
#include <engine/graphics/VertexShader.hpp>
#include <engine/graphics/PixelShader.hpp>
#include <engine/graphics/PipelineStateManager.hpp>
#include <memory>
#include <format>

RenderJob::RenderJob(RenderJob&& other) noexcept {
	commandList        = other.commandList; 
	swapChain          = other.swapChain; 
	other.commandList  = nullptr;
	other.swapChain    = nullptr;
}

RenderJob::~RenderJob() {
	if (commandList) {
		commandList->Release();
	}
}

RenderJob& RenderJob::operator=(RenderJob&& other) noexcept {
	if (this != &other) {
		commandList        = other.commandList; 
		swapChain          = other.swapChain;
		other.commandList  = nullptr;
		other.swapChain    = nullptr;
	}
	return *this;
}

void GraphicsDevice::Initialize() {
	Instance().InitDXGIFactory();
	Instance().InitD3D11Device();
	Instance().InitShaders();
	Instance().SetLightBuffer({ {0.0f, 5.0f, -5.0f}, 0.0f, {1.0f, 1.0f, 1.0f}, 0.0f });
	Instance().SetGridParams({ 0.5f, 0.02f, 5.0f, 0.05f, 0.5f, {0.8f, 0.8f, 0.8f},  
		{0.6f, 0.6f, 0.6f}, {0.0f, 0.5f, 1.0f}, {1.0f, 0.0f, 0.0f}, 0.3f, 50.0f });
	Instance().InitBuffers();
	PipelineStateManager::Initialize(Instance().m_device.Get());
#ifdef _DEBUG
	Instance().EnableD3D11DebugLayer();
	Instance().EnableDXGIDebugLayer();
#endif
}

IDXGIFactory6* GraphicsDevice::DXGIFactory() {
	return Instance().m_dxgiFactory.Get();
}

ID3D11DeviceContext* GraphicsDevice::ImmediateContext() {
	return Instance().m_immediateContext.Get();
}

ID3D11Device* GraphicsDevice::D3D11Device() {
	return Instance().m_device.Get();
}

void GraphicsDevice::PushRenderJob(HWND window, RenderJob renderJob) {
	std::lock_guard<std::mutex> lock(Mutex());
	PendingJobs()[window] = std::move(renderJob);
}

void GraphicsDevice::ExecuteRenderJobs() {
	auto& pendingJobs = Instance().m_pendingJobs;
	auto& activeJobs = Instance().m_activeJobs;
	{
		std::lock_guard<std::mutex> lock(Mutex());
		std::swap(activeJobs, pendingJobs);
	}
	for (auto& [hwnd, job] : activeJobs) {
		Instance().m_immediateContext->ExecuteCommandList(job.commandList, FALSE);
	}

	for (auto& [hwnd, job] : activeJobs) {
		job.swapChain->Present(1, 0);
	}

	activeJobs.clear();
	CV().notify_all();
}

void GraphicsDevice::RemoveRenderJob(HWND window) {
	std::lock_guard<std::mutex> lock(Mutex());
	Instance().m_pendingJobs.erase(window);
	Instance().m_activeJobs.erase(window);
}

std::unordered_map<HWND, RenderJob>& GraphicsDevice::PendingJobs() {
	return Instance().m_pendingJobs;
}

std::mutex& GraphicsDevice::Mutex() {
	return Instance().m_renderMutex;
}

std::condition_variable& GraphicsDevice::CV() {
	return Instance().m_renderCV;
}

GraphicsDevice& GraphicsDevice::Instance() {
	static GraphicsDevice instance;
	return instance;
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
	if (Instance().m_d3d11Debug) {
		Instance().m_d3d11Debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
	}
}

void GraphicsDevice::SetViewMatrix(const DirectX::XMMATRIX& view) {
	Instance().m_viewMatrix = view;
}

void GraphicsDevice::SetProjectionMatrix(float fov, float aspectRatio, float zNear, float zFar) {
	Instance().m_projMatrix = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(fov), aspectRatio, zNear, zFar);
}

const DirectX::XMMATRIX& GraphicsDevice::ViewMatrix() {
	return Instance().m_viewMatrix;
}

const DirectX::XMMATRIX& GraphicsDevice::ProjectionMatrix() {
	return Instance().m_projMatrix;
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

void GraphicsDevice::InitBuffers() {
	D3D11_BUFFER_DESC transformBufferDesc{};
	transformBufferDesc.Usage      = D3D11_USAGE_DEFAULT;
	transformBufferDesc.ByteWidth  = sizeof(TransformBuffer);
	transformBufferDesc.BindFlags  = D3D11_BIND_CONSTANT_BUFFER;

	D3D11_BUFFER_DESC lightBufferDesc{};
	lightBufferDesc.Usage      = D3D11_USAGE_DEFAULT;
	lightBufferDesc.ByteWidth  = sizeof(LightBuffer);
	lightBufferDesc.BindFlags  = D3D11_BIND_CONSTANT_BUFFER;

	D3D11_BUFFER_DESC gridParamsDesc{};
	gridParamsDesc.Usage = D3D11_USAGE_DEFAULT;
	gridParamsDesc.ByteWidth = sizeof(GridParams);
	gridParamsDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	auto align16 = [](uint32_t x) -> uint32_t { return (x + 15u) & ~15u; };
	gridParamsDesc.ByteWidth = align16(sizeof(GridParams));

	D3D11_BUFFER_DESC camDesc{};
	camDesc.Usage = D3D11_USAGE_DEFAULT;
	camDesc.ByteWidth = sizeof(CameraParams);
	camDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	HR_LOG(m_device->CreateBuffer(&transformBufferDesc, nullptr, &m_transformBuffer));
	HR_LOG(m_device->CreateBuffer(&lightBufferDesc, nullptr, &m_lightBuffer));
	HR_LOG(m_device->CreateBuffer(&gridParamsDesc, nullptr, &m_gridParamsBuffer));
	HR_LOG(m_device->CreateBuffer(&camDesc, nullptr, &m_cameraParamsBuffer));
}

void GraphicsDevice::SetLightBuffer(const LightBuffer& buffer) {
	Instance().m_lightBufferData = buffer;
}

void GraphicsDevice::UpdateLightBuffer(ID3D11DeviceContext* context) {
	context->UpdateSubresource(Instance().m_lightBuffer.Get(), 0, nullptr, &Instance().m_lightBufferData, 0, 0);
}

ID3D11Buffer** GraphicsDevice::LightBufferAddr() {
	return Instance().m_lightBuffer.GetAddressOf();
}

ID3D11Buffer** GraphicsDevice::TransformBufferAddr() {
	return Instance().m_transformBuffer.GetAddressOf();
}

ID3D11Buffer* GraphicsDevice::TransformBufferPtr() {
	return Instance().m_transformBuffer.Get();
}
}

void GraphicsDevice::SetGridParams(const GridParams& params) {
	Instance().m_gridParams = params;
}

ID3D11Buffer* GraphicsDevice::GridParamsBuffer() {
	return Instance().m_gridParamsBuffer.Get();
}

GridParams& GraphicsDevice::GridParamsData() {
	return Instance().m_gridParams;
}

ID3D11Buffer* GraphicsDevice::CameraParamsBuffer() {
	return Instance().m_cameraParamsBuffer.Get();
}
