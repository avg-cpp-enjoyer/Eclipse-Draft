#include "Camera.hpp"

#include <engine/mesh/StaticMesh.hpp>
#include <d3d11.h>

class Grid : public StaticMesh {
public:
	void Draw(ID3D11DeviceContext* context, const Camera& camera) const override;
};