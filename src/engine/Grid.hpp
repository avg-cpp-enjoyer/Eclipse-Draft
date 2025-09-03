#include "Mesh.hpp"

class Grid : public Mesh {
public:
	Grid() = default;
	~Grid() = default;
	void Draw(ID3D11DeviceContext* context) const override;
};