#include "PipelineStateManager.hpp"

#include <d3d11.h>
#include <utils/Log.hpp>

void PipelineStateManager::Initialize(ID3D11Device* device) {
	D3D11_RASTERIZER_DESC rsSolid{};
	rsSolid.FillMode = D3D11_FILL_SOLID;
	rsSolid.CullMode = D3D11_CULL_BACK;
	rsSolid.DepthClipEnable = true;
	HR_LOG(device->CreateRasterizerState(&rsSolid, m_rsSolid.ReleaseAndGetAddressOf()));

	D3D11_RASTERIZER_DESC rsWire{};
	rsWire.FillMode = D3D11_FILL_WIREFRAME;
	rsWire.CullMode = D3D11_CULL_NONE;
	rsWire.DepthClipEnable = true;
	HR_LOG(device->CreateRasterizerState(&rsWire, m_rsWire.ReleaseAndGetAddressOf()));

	D3D11_BLEND_DESC blendOpaque{};
	blendOpaque.RenderTarget[0].BlendEnable = false;
	blendOpaque.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	HR_LOG(device->CreateBlendState(&blendOpaque, m_blendOpaque.ReleaseAndGetAddressOf()));

	D3D11_BLEND_DESC blendAlpha{};
	blendAlpha.RenderTarget[0].BlendEnable = true;
	blendAlpha.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendAlpha.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendAlpha.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendAlpha.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendAlpha.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendAlpha.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendAlpha.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	HR_LOG(device->CreateBlendState(&blendAlpha, m_blendAlpha.ReleaseAndGetAddressOf()));

	D3D11_BLEND_DESC blendAdd{};
	blendAdd.RenderTarget[0].BlendEnable = true;
	blendAdd.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendAdd.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendAdd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendAdd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendAdd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendAdd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendAdd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	HR_LOG(device->CreateBlendState(&blendAdd, m_blendAdd.ReleaseAndGetAddressOf()));

	D3D11_DEPTH_STENCIL_DESC dsDefault{};
	dsDefault.DepthEnable = true;
	dsDefault.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDefault.DepthFunc = D3D11_COMPARISON_LESS;
	HR_LOG(device->CreateDepthStencilState(&dsDefault, m_depthDefault.ReleaseAndGetAddressOf()));

	D3D11_DEPTH_STENCIL_DESC dsRead{};
	dsRead.DepthEnable = true;
	dsRead.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	dsRead.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	HR_LOG(device->CreateDepthStencilState(&dsRead, m_depthRead.ReleaseAndGetAddressOf()));

	D3D11_DEPTH_STENCIL_DESC dsNone{};
	dsNone.DepthEnable = false;
	HR_LOG(device->CreateDepthStencilState(&dsNone, m_depthNone.ReleaseAndGetAddressOf()));

	m_states[PipelineMode::OpaqueSolid]       = { m_rsSolid.Get(), m_blendOpaque.Get(), m_depthDefault.Get() };
	m_states[PipelineMode::TransparentAlpha]  = { m_rsSolid.Get(), m_blendAlpha.Get(),  m_depthRead.Get() };
	m_states[PipelineMode::AdditiveParticles] = { m_rsSolid.Get(), m_blendAdd.Get(),    m_depthNone.Get() };
	m_states[PipelineMode::Wireframe]         = { m_rsWire.Get(),  m_blendOpaque.Get(), m_depthDefault.Get() };
	m_states[PipelineMode::DepthOnlyPass]     = { m_rsSolid.Get(), m_blendOpaque.Get(), m_depthDefault.Get() };
	m_states[PipelineMode::ShadowPass]        = { m_rsSolid.Get(), m_blendOpaque.Get(), m_depthDefault.Get() };
}

const PipelineState& PipelineStateManager::Get(PipelineMode mode) {
	return m_states[mode];
}
