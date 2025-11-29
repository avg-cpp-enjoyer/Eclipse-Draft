#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <cstdint>

namespace Direct3D11Utils {
	inline DirectX::XMMATRIX SetWorldMatrix(const DirectX::XMFLOAT3& pos,
		const DirectX::XMFLOAT3& rotation, const DirectX::XMFLOAT3& scale) {

		DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
		DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
		DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(
			DirectX::XMConvertToRadians(rotation.x),
			DirectX::XMConvertToRadians(rotation.y),
			DirectX::XMConvertToRadians(rotation.z)
		);
		return scaleMatrix * rotationMatrix * translationMatrix;
	}
}
