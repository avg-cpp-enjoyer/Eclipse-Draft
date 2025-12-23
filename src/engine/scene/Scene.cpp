#include "Scene.hpp"

#include <engine/mesh/MeshLoader.hpp>
#include <engine/mesh/IMesh.hpp>
#include <engine/mesh/StaticMesh.hpp>
#include <engine/core/GraphicsDevice.hpp>
#include <utility>
#include <algorithm>
#include <memory>
#include <string>
#include <functional>
#include <d3d11.h>

Scene::Scene() {
	StaticMesh grid = MeshLoader::LoadFromOBJ("assets\\grid.obj", GraphicsDevice::D3D11Device());
	m_grid.SetVertices(grid.Vertices());
	m_grid.SetIndices(grid.Indices());
	m_grid.CreateBuffers(GraphicsDevice::D3D11Device());
}

void Scene::AddMesh(const std::string& name, std::unique_ptr<IMesh> mesh) {
	IMesh* rawPtr = mesh.get(); 
	m_lookupTable[name] = rawPtr; 
	m_meshes.push_back(std::move(mesh));
}

IMesh* Scene::GetMeshByName(const std::string& name) {
	auto it = m_lookupTable.find(name);
	return it != m_lookupTable.end() ? it->second : nullptr;
}

void Scene::RemoveMesh(const std::string& name) {
	auto it = m_lookupTable.find(name); 
	if (it != m_lookupTable.end()) {
		IMesh* target = it->second;
		m_lookupTable.erase(it); 
		m_meshes.erase(
			std::remove_if(m_meshes.begin(), m_meshes.end(), 
			[target](const std::unique_ptr<IMesh>& mesh) { return mesh.get() == target; }), 
			m_meshes.end()
		);
	}
}

void Scene::ForEachMesh(const std::function<void(IMesh*)>& func) {
	for (auto& mesh : m_meshes) { 
		func(mesh.get()); 
	}
}

void Scene::DrawAll(ID3D11DeviceContext* context) {
	m_grid.Draw(context); 
	for (auto& mesh : m_meshes) { 
		mesh->Draw(context); 
	}
}
