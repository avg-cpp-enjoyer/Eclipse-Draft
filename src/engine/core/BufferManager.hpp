#pragma once

#include "ConstantBuffers.hpp"
#include "ConstantBuffer.hpp"

#include <d3d11.h>
#include <typeindex>
#include <memory>
#include <unordered_map>

class BufferManager { 
public: 
	static void Initialize(ID3D11Device* device);
	template <IsConstantBufferType T>
	static void UpdateBuffer(ID3D11DeviceContext* context, const T& data);
	static void UpdateGridBuffer(ID3D11DeviceContext* context);
	template <IsConstantBufferType T>
	static ID3D11Buffer* GetBuffer();
private: 
	static inline std::unordered_map<std::type_index, std::unique_ptr<IConstantBuffer>> m_buffers;
	inline static GridParams m_gridParamsData;
};

template<IsConstantBufferType T>
inline void BufferManager::UpdateBuffer(ID3D11DeviceContext* context, const T& data) {
	auto it = m_buffers.find(typeid(T)); 
	if (it != m_buffers.end()) { 
		static_cast<ConstantBuffer<T>*>(it->second.get())->Update(context, data); 
	}
}

template<IsConstantBufferType T>
inline ID3D11Buffer* BufferManager::GetBuffer() {
	auto it = m_buffers.find(typeid(T)); 
	return it != m_buffers.end() ? it->second->Get() : nullptr;
}
