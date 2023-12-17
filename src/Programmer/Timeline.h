#pragma once
#include "imgui.h" // necessary for ImGui::*, imgui-SFML.h doesn't include imgui.h
#include "imgui-SFML.h" // for ImGui::SFML::* functions and SFML-specific overloads
#include <vector>
#include <string>
#include <wtypes.h>
#include "Fixture.h"
#include "../OpenDMX.h"

struct AutomationNode {
	int32_t startTime;
	UCHAR value;

	AutomationNode() = default;
	AutomationNode(const int32_t& frame, const UCHAR& value)
		: startTime(frame), value(value)
	{
	}
};

struct Automation {
	std::string name;
	int32_t channel;
	std::vector<AutomationNode> nodes;
	bool active;

	void AddNode(const int32_t& frame, const UCHAR& value);
	//AutomationNode& GetNode(float startTime);
	//void SetNode(float startTime, UCHAR value);
	Automation(const std::string& name, const int32_t& channel)
		: name(name), channel(channel), nodes(), active(true)
	{
	}
};

struct AutomationGroup {
	std::string name;
	std::vector<Automation> automations;
	bool open;

	void AddAutomation(const std::string& name, const int32_t& channel);

	AutomationGroup(const std::string& name)
		: name(name), automations(), open(true)
	{
	}
};

class Timeline
{
	OpenDMX& dmx_interface;
	std::vector<AutomationGroup> automationGroups;

	int32_t* currentFrame;
	int32_t startFrame = 0;
	int32_t endFrame = 180 * 40; //Seconds * refresh rate of output

public:
	Timeline(OpenDMX& dmx_interface, int32_t* currentFrame, const int32_t& startFrame, const int32_t& endFrame)
		: dmx_interface(dmx_interface), currentFrame(currentFrame), startFrame(startFrame), endFrame(endFrame)
	{
	}
	void Render();
	void Update(int32_t frame);

	int AddGroup(std::string name); //Returns index of new element
	int AddAutomation(int groupIndex, std::string name, int32_t channe); //Returns index of new element
	void AddNode(int groupIndex, int automationIndex, int32_t frame, UCHAR value); //Nodes are sorted therefore cannot return valid index
};
