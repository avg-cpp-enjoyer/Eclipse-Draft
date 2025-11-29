#include "Renderer.hpp"
#include "GraphicsDevice.hpp"
#include "RenderTarget.hpp"

#include <cstdarg>
#include <thread>
#include <Windows.h>
#include <mutex>
#include <engine/mesh/MeshLoader.hpp>
#include <engine/mesh/Mesh.hpp>

Renderer::Renderer(HWND window) : m_window(window), m_renderTarget(window) {
	m_testMesh = MeshLoader::LoadFromOBJ("assets\\M4A1.obj", GraphicsDevice::D3D11Device());
	m_testMesh.SetScale({ 0.2f, 0.2f, 0.2f });
	Mesh grid = MeshLoader::LoadFromOBJ("assets\\grid.obj", GraphicsDevice::D3D11Device());
	m_grid.SetVertices(grid.Vertices());
	m_grid.SetIndices(grid.Indices());
	m_grid.CreateBuffers(GraphicsDevice::D3D11Device());
}

void Renderer::Start(int threadPriority, uintptr_t affinityMask) {
	if (!m_running.exchange(true)) {
		m_renderThread = std::thread(&Renderer::RenderLoop, this);
		SetThreadPriority(m_renderThread.native_handle(), threadPriority);
		SetThreadAffinityMask(m_renderThread.native_handle(), affinityMask);
	}
}

void Renderer::Shutdown() {
	m_running.store(false);
	if (m_renderThread.joinable()) {
		m_renderThread.join();
	}
	GraphicsDevice::RemoveRenderJob(m_window);
}

RenderTarget& Renderer::GetRT() {
	return m_renderTarget;
}

void Renderer::RenderLoop() {
	while (m_running.load(std::memory_order_relaxed)) {
		std::unique_lock<std::mutex> lock(GraphicsDevice::Mutex());
		GraphicsDevice::CV().wait(lock, [&] { 
			return !m_running.load(std::memory_order_relaxed) ||
			GraphicsDevice::PendingJobs().count(m_window) == 0;
		});
		lock.unlock();
		RenderFrame();
	}
}

void Renderer::RenderFrame() {
	m_renderTarget.BeginRender();
	m_grid.Draw(m_renderTarget.Context());
	m_testMesh.Draw(m_renderTarget.Context());
	m_renderTarget.EndRender();
}
