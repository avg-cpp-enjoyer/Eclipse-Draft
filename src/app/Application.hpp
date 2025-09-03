#pragma once

#include "engine/GraphicsDevice.hpp"
#include "MainWindow.hpp"

#include <chrono>

class Application {
public:
	Application() = default;
	~Application() = default;

	int Initialize(int width, int height);
	int Exec(int cmdShow);
private:
	MainWindow m_mainWindow;
};