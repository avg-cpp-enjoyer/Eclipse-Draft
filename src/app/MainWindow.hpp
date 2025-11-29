#pragma once

#include "Win32Base.hpp"

#include <engine/core/Renderer.hpp>
#include <optional>

class MainWindow : public Win32Base<MainWindow> {
public:
	MainWindow() = default;
	~MainWindow() = default;

	const wchar_t* ClassName() const override;
	intptr_t HandleMessage(uint32_t msg, uintptr_t wParam, intptr_t lParam) override;
private:
	void OnCreate();
	void OnDestroy();
	void OnSizeChanged();
private:
	std::optional<Renderer> m_renderer;
	bool m_pendingResize = false;
};