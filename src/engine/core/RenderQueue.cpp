#include "RenderQueue.hpp"

#include <d3d11.h>
#include <mutex>
#include <unordered_map>
#include <Windows.h>
#include <utility>
#include <condition_variable>

RenderJob::RenderJob(RenderJob&& other) noexcept {
	commandList = other.commandList;
	swapChain = other.swapChain;
	other.commandList = nullptr;
	other.swapChain = nullptr;
}

RenderJob::~RenderJob() {
	if (commandList) {
		commandList->Release();
	}
}

RenderJob& RenderJob::operator=(RenderJob&& other) noexcept {
	if (this != &other) {
		commandList = other.commandList;
		swapChain = other.swapChain;
		other.commandList = nullptr;
		other.swapChain = nullptr;
	}
	return *this;
}

void RenderQueue::Execute(ID3D11DeviceContext* context) {
	std::unique_lock<std::mutex> lock(m_mutex);
	std::swap(m_activeJobs, m_pendingJobs);
	lock.unlock();

	for (auto& [hwnd, job] : m_activeJobs) {
		context->ExecuteCommandList(job.commandList, false);
	}

	for (auto& [hwnd, job] : m_activeJobs) {
		job.swapChain->Present(1, 0);
	}

	m_activeJobs.clear();
	m_cv.notify_all();
}

void RenderQueue::Push(HWND window, RenderJob renderJob) {
	std::lock_guard<std::mutex> lock(m_mutex);
	m_pendingJobs[window] = std::move(renderJob);
}

void RenderQueue::Remove(HWND window) {
	std::lock_guard<std::mutex> lock(m_mutex);
	m_pendingJobs.erase(window);
	m_activeJobs.erase(window);
}

std::condition_variable& RenderQueue::CV() {
	return m_cv;
}

std::mutex& RenderQueue::Mutex() {
	return m_mutex;
}

std::unordered_map<HWND, RenderJob>& RenderQueue::PendingJobs() {
	return m_pendingJobs;
}
