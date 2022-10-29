#include "Headers.h"
#include "ImWindowConsole.h"
#include "Console.h"
#include "cimport.h"

using CSL = Console;

static struct aiLogStream stream;

void AssimpCallback(const char* message, char* user)
{
	std::string strM = message;
	CSL::S_Log("Assimp: " + strM);
}

ImWindowConsole::ImWindowConsole()
{
	windowName = "Console";

	isEnabled = true;

	CSL::S_Init();

	stream.callback = AssimpCallback;
	aiAttachLogStream(&stream);

	for (size_t i = 0; i < 10; i++)
	{
		CSL::S_Log("hello");
	}
}

ImWindowConsole::~ImWindowConsole()
{
	CSL::S_Close();
}

void ImWindowConsole::Update()
{
	if (focus)
	{
		focus = false;
		ImGui::SetWindowFocus(windowName.c_str());
	}

	if (ImGui::Begin(windowName.c_str(), &isEnabled, ImGuiWindowFlags_NoCollapse))
	{
		if (ImGui::SmallButton("Save Log"))
		{
			CSL::S_SaveLog();
		}ImGui::SameLine();

		if (ImGui::SmallButton("Collapse"))
		{
			_isCollapse = !_isCollapse;
		}ImGui::SameLine();

		if (ImGui::SmallButton("Clear"))
		{
			CSL::S_ClearLog();
		}ImGui::SameLine();

		ImGui::Text("Total Logs: %s", CSL::S_GetLogCounts());

		ImGui::Separator();

		// Reserve enough left-over height for 1 separator + 1 input text
		const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
		ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), true, ImGuiWindowFlags_HorizontalScrollbar);

		if(!_isCollapse) // if isn't collapsing
		{
			std::string* buffer = nullptr;

			uint cslSize = CSL::S_GetLog(&buffer);

			for (size_t i = 0; i < cslSize; buffer++, i++)
			{
				ImGui::Text((*buffer).c_str());
			}
		}
		else // if is collapsing
		{
			auto logs = CSL::S_GetCollapseLog();

			for (auto log: logs)
			{
				ImGui::Text((log.first + "\tcount: " + std::to_string(log.second)).c_str());
			}
		}

		// Automatically set scroll to bottom
		if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())	ImGui::SetScrollHereY(1.0f);

		ImGui::EndChild();	

		ImGui::Text(" Hello Engine :) ");
	}
	ImGui::End();
}

void ImWindowConsole::FocusConsole()
{
	focus = true;
}
