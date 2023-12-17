#include "Timeline.h"
#include <algorithm>
#include <imgui_neo_sequencer.h>

void Automation::AddNode(const int32_t& startTime, const UCHAR& value)
{
	nodes.emplace_back(startTime, value);
	std::sort(
		nodes.begin(),
		nodes.end(),
		[](const AutomationNode& lhs, const AutomationNode& rhs)
		{
			return lhs.startTime < rhs.startTime;
		});
}

void AutomationGroup::AddAutomation(const std::string& name, const int32_t& channel) {
	automations.emplace_back(name, channel);
}

void Timeline::Update(int32_t frame)
{
	for (auto& group : automationGroups) {
		for (auto& automation : group.automations) {

			for (int i = 0; i < automation.nodes.size(); i++) {
				if (automation.nodes[i].startTime > frame) {
					if (i != 0) {
						dmx_interface.setChannel(automation.channel, automation.nodes[i - 1].value);
					}
					else {
						dmx_interface.setChannel(automation.channel, 0);
					}
					break;
				}
				else if (i == automation.nodes.size() - 1) {
					dmx_interface.setChannel(automation.channel, automation.nodes[i].value);
				}
			}
		}
	}
}

void Timeline::Render()
{
	bool doDelete = false;

	if (ImGui::BeginNeoSequencer("Sequencer", currentFrame, &startFrame, &endFrame, { 0, 0 },
		ImGuiNeoSequencerFlags_EnableSelection |
		ImGuiNeoSequencerFlags_Selection_EnableDragging |
		ImGuiNeoSequencerFlags_Selection_EnableDeletion))
	{
		for (auto& group : automationGroups)
		{
			if (ImGui::BeginNeoGroup(group.name.c_str(), &group.open))
			{
				for (auto& automation : group.automations)
				{
					if (ImGui::BeginNeoTimelineEx(automation.name.c_str(), &automation.active))
					{
						for (auto&& node : automation.nodes)
						{
							ImGui::NeoKeyframe(&node.startTime);
							// Per keyframe code here
						}

						if (doDelete)
						{
							uint32_t count = ImGui::GetNeoKeyframeSelectionSize();

							ImGui::FrameIndexType* toRemove = new ImGui::FrameIndexType[count];

							ImGui::GetNeoKeyframeSelection(toRemove);

							//Delete keyframes from your structure
						}
						ImGui::EndNeoTimeLine();
					}
				}
				ImGui::EndNeoGroup();
			}
		}
		ImGui::EndNeoSequencer();
	}
}

int Timeline::AddGroup(std::string name){
	automationGroups.emplace_back(name);
	return automationGroups.size() - 1;
}

int Timeline::AddAutomation(int groupIndex, std::string name, int32_t channel) {
	automationGroups[groupIndex].AddAutomation(name, channel);
	return automationGroups[groupIndex].automations.size() - 1;
}

void Timeline::AddNode(int groupIndex, int automationIndex, int32_t frame, UCHAR value) {
	automationGroups[groupIndex].automations[automationIndex].AddNode(frame, value);
}
