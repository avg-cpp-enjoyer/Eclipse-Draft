#pragma once

#include "Grid.hpp"

#include <engine/mesh/IMesh.hpp>
#include <memory>
#include <vector>
#include <unordered_map>
#include <string>
#include <functional>
#include <d3d11.h>

class Scene {
public:
	Scene();
	~Scene() = default;

	void AddMesh(const std::string& name, std::unique_ptr<IMesh> mesh);
	IMesh* GetMeshByName(const std::string& name);
	void RemoveMesh(const std::string& name);
	void ForEachMesh(const std::function<void(IMesh*)>& func);
	void DrawAll(ID3D11DeviceContext* context);
private:
	std::unordered_map<std::string, IMesh*> m_lookupTable;
	std::vector<std::unique_ptr<IMesh>> m_meshes;
	Grid m_grid;
};