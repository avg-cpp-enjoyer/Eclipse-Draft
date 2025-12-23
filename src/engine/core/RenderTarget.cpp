#include "RenderTarget.hpp"
#include "ConstantBuffers.hpp"
#include "RenderQueue.hpp"
#include "BufferManager.hpp"

#include <utils/Log.hpp>

RenderTarget::RenderTarget(HWND window) : m_window(window) {
	RECT windowRect{};
	GetClientRect(m_window, &windowRect);
	m_width = windowRect.right - windowRect.left;
	m_height = windowRect.bottom - windowRect.top;
	Initialize();
}

void RenderTarget::ToggleFullscreen() {
	BOOL isFullscreen = false;
	DEVMODE devMode = GetCurrentMonitorMode();

	Microsoft::WRL::ComPtr<IDXGIOutput> output;
	m_swapChain->GetFullscreenState(&isFullscreen, &output);

	if (!isFullscreen) {
		DXGI_MODE_DESC modeDesc{};
		modeDesc.Width                    = devMode.dmPelsWidth;
		modeDesc.Height                   = devMode.dmPelsHeight;
		modeDesc.RefreshRate.Numerator    = devMode.dmDisplayFrequency;
		modeDesc.RefreshRate.Denominator  = 1;
		modeDesc.Format                   = DXGI_FORMAT_B8G8R8A8_UNORM;
		modeDesc.Scaling                  = DXGI_MODE_SCALING_UNSPECIFIED;
		modeDesc.ScanlineOrdering         = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

		m_swapChain->ResizeTarget(&modeDesc);
		m_swapChain->SetFullscreenState(true, nullptr);
	} else {
		m_swapChain->SetFullscreenState(false, nullptr);
		HandleResize();
	}
}

void RenderTarget::HandleResize() {
	RECT rc{};
	GetClientRect(m_window, &rc);
	uint32_t newWidth = rc.right - rc.left;
	uint32_t newHeight = rc.bottom - rc.top;
	
	m_rtv.Reset();
	m_deferredContext->Flush();

	RenderQueue::Remove(m_window);
	GraphicsDevice::ImmediateContext()->OMSetRenderTargets(0, nullptr, nullptr);
	GraphicsDevice::ImmediateContext()->ClearState();
	GraphicsDevice::ImmediateContext()->Flush();

	m_swapChain->ResizeBuffers(0, newWidth, newHeight, DXGI_FORMAT_UNKNOWN, 0);
	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
	m_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
	HR_LOG(GraphicsDevice::D3D11Device()->CreateRenderTargetView(backBuffer.Get(), nullptr, &m_rtv));
	m_depthStencilView.Reset();
	m_depthStencilBuffer.Reset();
	InitDepthStencilView();
	GraphicsDevice::SetProjectionMatrix(90.0f, float(newWidth) / float(newHeight), 0.1f, 1000.0f);
	GraphicsDevice::SetViewMatrix(m_camera.ViewMatrix());

	m_width = newWidth;
	m_height = newHeight;
}

void RenderTarget::BeginRender() {
	D3D11_VIEWPORT viewport{};
	viewport.TopLeftX  = 0.0f;
	viewport.TopLeftY  = 0.0f;
	viewport.Width     = static_cast<float>(m_width);
	viewport.Height    = static_cast<float>(m_height);
	viewport.MinDepth  = 0.0f;
	viewport.MaxDepth  = 1.0f;

	CameraParams cam{};
	cam.cameraPos = m_camera.Position();
	float clearColor[] = { 0.1f, 0.1f, 0.15f, 1.0f };
	m_deferredContext->ClearRenderTargetView(m_rtv.Get(), clearColor);
	m_deferredContext->RSSetViewports(1, &viewport);
	m_deferredContext->OMSetRenderTargets(1, m_rtv.GetAddressOf(), m_depthStencilView.Get());
	m_deferredContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	ID3D11Buffer* lightBuffer = BufferManager::GetLightBuffer();
	m_deferredContext->PSSetConstantBuffers(1, 1, &lightBuffer);

	ID3D11Buffer* transformBuffer = BufferManager::GetTransformBuffer();
	m_deferredContext->VSSetConstantBuffers(0, 1, &transformBuffer);

	BufferManager::UpdateCameraBuffer(m_deferredContext.Get(), cam);
	BufferManager::UpdateLightBuffer(m_deferredContext.Get());
	GraphicsDevice::SetProjectionMatrix(90.0f, float(m_width) / float(m_height), 0.1f, 1000.0f);
	GraphicsDevice::SetViewMatrix(m_camera.ViewMatrix());
}

void RenderTarget::EndRender() {
	m_deferredContext->FinishCommandList(FALSE, &m_cmdList);

	RenderJob job;
	job.commandList = m_cmdList.Get();
	job.commandList->AddRef();
	job.swapChain = m_swapChain.Get();

	RenderQueue::Push(m_window, std::move(job));
	m_cmdList.Reset();
}

ID3D11CommandList* RenderTarget::CommandList() {
	return m_cmdList.Get();
}

IDXGISwapChain4* RenderTarget::SwapChain() {
	return m_swapChain.Get();
}

ID3D11DeviceContext* RenderTarget::Context() {
	return m_deferredContext.Get();
}

Camera& RenderTarget::GetCamera() {
	return m_camera;
}

void RenderTarget::Initialize() {
	InitDXGISwapChain();
	InitDeferredContext();
	InitDepthStencilView();
}

DEVMODEW RenderTarget::GetCurrentMonitorMode() const {
	HMONITOR monitor = MonitorFromWindow(m_window, MONITOR_DEFAULTTONEAREST);
	MONITORINFOEXW monitorInfo{};
	monitorInfo.cbSize = sizeof(monitorInfo);
	GetMonitorInfoW(monitor, &monitorInfo);

	DEVMODEW devMode{};
	devMode.dmSize = sizeof(devMode);
	EnumDisplaySettingsW(monitorInfo.szDevice, ENUM_CURRENT_SETTINGS, &devMode);

	return devMode;
}

void RenderTarget::InitDXGISwapChain() {
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc           = {};
	swapChainDesc.Width                           = m_width;
	swapChainDesc.Height                          = m_height;
	swapChainDesc.Format                          = DXGI_FORMAT_B8G8R8A8_UNORM;
	swapChainDesc.SampleDesc.Count                = 1;
	swapChainDesc.SampleDesc.Quality              = 0;
	swapChainDesc.BufferUsage                     = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount                     = 2;
	swapChainDesc.SwapEffect                      = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.Scaling                         = DXGI_SCALING_NONE;
	swapChainDesc.Flags                           = {};

	DEVMODEW devMode = GetCurrentMonitorMode();
	DXGI_SWAP_CHAIN_FULLSCREEN_DESC swapChainFullscrDesc = {};
	swapChainFullscrDesc.RefreshRate.Numerator    = devMode.dmDisplayFrequency;
	swapChainFullscrDesc.RefreshRate.Denominator  = 1;
	swapChainFullscrDesc.Scaling                  = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainFullscrDesc.ScanlineOrdering         = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainFullscrDesc.Windowed                 = true;

	Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain1;
	HR_LOG(GraphicsDevice::DXGIFactory()->CreateSwapChainForHwnd(GraphicsDevice::D3D11Device(),
		m_window, &swapChainDesc, &swapChainFullscrDesc, nullptr, &swapChain1));
	HR_LOG(swapChain1.As(&m_swapChain));

	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
	HR_LOG(m_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer)));
	HR_LOG(GraphicsDevice::D3D11Device()->CreateRenderTargetView(backBuffer.Get(), nullptr, &m_rtv));
}

void RenderTarget::InitDeferredContext() {
	GraphicsDevice::D3D11Device()->CreateDeferredContext(0, &m_deferredContext);
}

void RenderTarget::InitDepthStencilView() {
	D3D11_TEXTURE2D_DESC depthDesc{};
	depthDesc.Width               = m_width;  
	depthDesc.Height              = m_height; 
	depthDesc.MipLevels           = 1;
	depthDesc.ArraySize           = 1;
	depthDesc.Format              = DXGI_FORMAT_D24_UNORM_S8_UINT; 
	depthDesc.SampleDesc.Count    = 1; 
	depthDesc.SampleDesc.Quality  = 0;
	depthDesc.Usage               = D3D11_USAGE_DEFAULT;
	depthDesc.BindFlags           = D3D11_BIND_DEPTH_STENCIL;
	depthDesc.CPUAccessFlags      = 0;
	depthDesc.MiscFlags           = 0;

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format                = depthDesc.Format;
	dsvDesc.ViewDimension         = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice    = 0;

	GraphicsDevice::D3D11Device()->CreateTexture2D(&depthDesc, nullptr, &m_depthStencilBuffer);
	GraphicsDevice::D3D11Device()->CreateDepthStencilView(m_depthStencilBuffer.Get(), &dsvDesc, &m_depthStencilView);
}
