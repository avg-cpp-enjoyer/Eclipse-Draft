#pragma once

#include "MainWindow.hpp"

class Application {
public:
	Application() = default;
	~Application() = default;

	int Initialize(int width, int height);
	int Exec(int cmdShow);
private:
	MainWindow m_mainWindow;
};