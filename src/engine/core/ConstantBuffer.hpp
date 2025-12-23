#pragma once

#include "ConstantBuffers.hpp"

#include <type_traits>
#include <d3d11.h>
#include <utils/Log.hpp>
#include <wrl/client.h>

template <typename T> 
concept IsConstantBufferType = 
	std::is_same_v<T, TransformBuffer> || 
	std::is_same_v<T, LightBuffer> || 
	std::is_same_v<T, GridParams> || 
	std::is_same_v<T, CameraParams>;

template <IsConstantBufferType T>
class ConstantBuffer {
public:
	void Initialize(ID3D11Device* device);
	void Update(ID3D11DeviceContext* context, const T& data);
	ID3D11Buffer* Get() const;
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
};

template<IsConstantBufferType T>
inline void ConstantBuffer<T>::Initialize(ID3D11Device* device) {
	D3D11_BUFFER_DESC desc{}; 
	desc.Usage = D3D11_USAGE_DEFAULT; 
	desc.ByteWidth = (sizeof(T) + 15u) & ~15u; 
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER; 
	
	HR_LOG(device->CreateBuffer(&desc, nullptr, &m_buffer));
}

template<IsConstantBufferType T>
inline void ConstantBuffer<T>::Update(ID3D11DeviceContext* context, const T& data) {
	context->UpdateSubresource(m_buffer.Get(), 0, nullptr, &data, 0, 0);
}

template<IsConstantBufferType T>
inline ID3D11Buffer* ConstantBuffer<T>::Get() const {
	return m_buffer.Get();
}
