#include "GraphicsDevice.hpp"

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
	Instance().EnableGridAlphaBlending();
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
	Microsoft::WRL::ComPtr<ID3DBlob> vsBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> psBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> gridPsBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;

	HR_LOG(D3DCompileFromFile(L"src\\shaders\\VertexShader.hlsl", nullptr, nullptr, "VSMain", "vs_5_0",
		D3DCOMPILE_ENABLE_STRICTNESS, 0, &vsBlob, &errorBlob));
	HR_LOG(D3DCompileFromFile(L"src\\shaders\\PixelShader.hlsl", nullptr, nullptr, "PSMain", "ps_5_0",
		D3DCOMPILE_ENABLE_STRICTNESS, 0, &psBlob, &errorBlob));
	HR_LOG(D3DCompileFromFile(L"src\\shaders\\Grid.hlsl", nullptr, nullptr, "PSMain", "ps_5_0",
		D3DCOMPILE_ENABLE_STRICTNESS, 0, &gridPsBlob, &errorBlob));

	HR_LOG(m_device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &m_vertexShader));
	HR_LOG(m_device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &m_pixelShader));
	HR_LOG(m_device->CreatePixelShader(gridPsBlob->GetBufferPointer(), gridPsBlob->GetBufferSize(), nullptr, &m_gridShader));

	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	HR_LOG(m_device->CreateInputLayout(layout, _countof(layout), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &m_inputLayout));
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

void GraphicsDevice::EnableGridAlphaBlending() {
	D3D11_DEPTH_STENCIL_DESC depthStencil{};
	depthStencil.DepthEnable = TRUE;
	depthStencil.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthStencil.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	D3D11_BLEND_DESC blend{};
	blend.RenderTarget[0].BlendEnable            = TRUE;
	blend.RenderTarget[0].SrcBlend               = D3D11_BLEND_SRC_ALPHA;
	blend.RenderTarget[0].DestBlend              = D3D11_BLEND_INV_SRC_ALPHA;
	blend.RenderTarget[0].BlendOp                = D3D11_BLEND_OP_ADD;
	blend.RenderTarget[0].SrcBlendAlpha          = D3D11_BLEND_ONE;
	blend.RenderTarget[0].DestBlendAlpha         = D3D11_BLEND_ZERO;
	blend.RenderTarget[0].BlendOpAlpha           = D3D11_BLEND_OP_ADD;
	blend.RenderTarget[0].RenderTargetWriteMask  = D3D11_COLOR_WRITE_ENABLE_ALL;

	D3D11_RASTERIZER_DESC rasterizer{};
	rasterizer.FillMode                          = D3D11_FILL_SOLID;
	rasterizer.CullMode                          = D3D11_CULL_BACK;
	rasterizer.DepthClipEnable                   = TRUE;
	rasterizer.DepthBias                         = 0;
	rasterizer.SlopeScaledDepthBias              = 0.0f;

	HR_LOG(m_device->CreateDepthStencilState(&depthStencil, &m_dsGrid));
	HR_LOG(m_device->CreateBlendState(&blend, &m_bsGrid));
	HR_LOG(m_device->CreateRasterizerState(&rasterizer, &m_rsGrid));
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

ID3D11VertexShader* GraphicsDevice::VertexShader() {
	return Instance().m_vertexShader.Get();
}

ID3D11PixelShader* GraphicsDevice::PixelShader() {
	return Instance().m_pixelShader.Get();
}

ID3D11InputLayout* GraphicsDevice::InputLayout() {
	return Instance().m_inputLayout.Get();
}

ID3D11BlendState* GraphicsDevice::GridBlendState() {
	return Instance().m_bsGrid.Get();
}

ID3D11RasterizerState* GraphicsDevice::GridRasterizerState() {
	return Instance().m_rsGrid.Get();
}

ID3D11DepthStencilState* GraphicsDevice::GridDepthStencilState() {
	return Instance().m_dsGrid.Get();
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

ID3D11PixelShader* GraphicsDevice::GridShader() {
	return Instance().m_gridShader.Get();
}

ID3D11Buffer* GraphicsDevice::CameraParamsBuffer() {
	return Instance().m_cameraParamsBuffer.Get();
}
