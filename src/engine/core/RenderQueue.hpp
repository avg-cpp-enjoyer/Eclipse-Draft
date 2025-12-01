#pragma once

#include <d3d11.h>
#include <dxgi.h>
#include <Windows.h>
#include <unordered_map>
#include <mutex>
#include <condition_variable>

struct RenderJob {
	RenderJob() = default;
	~RenderJob();

	RenderJob(const RenderJob&) = delete;
	RenderJob& operator=(const RenderJob&) = delete;
	RenderJob(RenderJob&& other) noexcept;
	RenderJob& operator=(RenderJob&& other) noexcept;

	ID3D11CommandList* commandList = nullptr;
	IDXGISwapChain* swapChain = nullptr;
};

class RenderQueue {
public:
	static void Execute(ID3D11DeviceContext* context);
	static void Push(HWND window, RenderJob renderJob);
	static void Remove(HWND window);
	static std::condition_variable& CV();
	static std::mutex& Mutex();
	static std::unordered_map<HWND, RenderJob>& PendingJobs();
private:
	static inline std::unordered_map<HWND, RenderJob> m_pendingJobs{};
	static inline std::unordered_map<HWND, RenderJob> m_activeJobs{};
	static inline std::mutex m_mutex{};
	static inline std::condition_variable m_cv{};
};