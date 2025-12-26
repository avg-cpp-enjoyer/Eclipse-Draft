#include "Renderer.hpp"
#include "GraphicsDevice.hpp"
#include "RenderTarget.hpp"

#include <engine/mesh/MeshLoader.hpp>
#include <engine/mesh/StaticMesh.hpp>
#include <engine/core/RenderQueue.hpp>
#include <engine/mesh/IMesh.hpp>

#include <cstdarg>
#include <thread>
#include <Windows.h>
#include <mutex>
#include <memory>
#include <utility>

Renderer::Renderer(HWND window) : m_window(window), m_renderTarget(window) {
	std::unique_ptr<IMesh> testMesh = std::make_unique<StaticMesh>(MeshLoader::LoadFromOBJ("assets\\M4A1.obj", GraphicsDevice::D3D11Device()));
	dynamic_cast<StaticMesh*>(testMesh.get())->SetScale({ 0.2f, 0.2f, 0.2f });
	m_scene.AddMesh("M4A1.obj", std::move(testMesh));
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
	RenderQueue::Remove(m_window);
}

RenderTarget& Renderer::GetRT() {
	return m_renderTarget;
}

void Renderer::RenderLoop() {
	while (m_running.load(std::memory_order::relaxed)) {
		std::unique_lock<std::mutex> lock(RenderQueue::Mutex());
		RenderQueue::CV().wait(lock, [&] { 
			return !m_running.load(std::memory_order::relaxed) ||
			RenderQueue::PendingJobs().count(m_window) == 0;
		});
		lock.unlock();
		RenderFrame();
	}
}

void Renderer::RenderFrame() {
	m_renderTarget.BeginRender();
	m_scene.DrawAll(m_renderTarget.Context(), m_renderTarget.GetCamera());
	m_renderTarget.EndRender();
}
