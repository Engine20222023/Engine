#pragma once

#include <queue>

#include "ImWindow.h"

class Application;

namespace Htool
{
	template<class T> class CycleArray;
}

using CArrayF = Htool::CycleArray<float>;

class ImWindowConfiguration :public ImWindow
{
public:

	ImWindowConfiguration();

	~ImWindowConfiguration();

	void Update() override;

public:
	std::map<std::string, std::pair<bool, int>> renderConfigs;


private:
	Application* app = nullptr;

	// Need more testing, but i think is good
	Htool::CycleArray<float>* frames = nullptr;

	bool fullscreen = false;

	int countCPU = 0;

	float systemRAM = 0;

	int frameLimit = 60;

	const int* windowWidth = nullptr;
	const int* windowHeight = nullptr;

	float* windowBrightness = nullptr;

	bool* isVSyncOn = nullptr;

	bool automaticCompilation = false;


	ModuleRenderer3D* moduleRenderer = nullptr;
};