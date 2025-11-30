#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <unordered_map>

enum class PipelineMode {
	OpaqueSolid,
	TransparentAlpha,
	AdditiveParticles,
	Wireframe,
	DepthOnlyPass,
	ShadowPass
};

struct PipelineState {
	ID3D11RasterizerState* rasterizer = nullptr;
	ID3D11BlendState* blend = nullptr;
	ID3D11DepthStencilState* depthStencil = nullptr;
};

class PipelineStateManager {
public:
	static void Initialize(ID3D11Device* device);
	static const PipelineState& Get(PipelineMode mode);
private:
	static inline Microsoft::WRL::ComPtr<ID3D11RasterizerState>   m_rsSolid;
	static inline Microsoft::WRL::ComPtr<ID3D11RasterizerState>   m_rsWire;
	static inline Microsoft::WRL::ComPtr<ID3D11BlendState>        m_blendOpaque;
	static inline Microsoft::WRL::ComPtr<ID3D11BlendState>        m_blendAlpha;
	static inline Microsoft::WRL::ComPtr<ID3D11BlendState>        m_blendAdd;
	static inline Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthDefault;
	static inline Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthRead;
	static inline Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthNone;
	static inline std::unordered_map<PipelineMode, PipelineState> m_states;
};