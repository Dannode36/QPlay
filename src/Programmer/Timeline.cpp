#include "Timeline.h"
#include <algorithm>
#include <imgui_neo_sequencer.h>
#include "imgui_stdlib.h"
#include "qmath.h"
#include <helpers.h>

void Automation::AddNode(const int32_t& frame, const UCHAR& value, AutomationNodeFlags flags)
{
	nodes.emplace_back(frame, value, flags);
	SortNodes();
}

void Automation::SortNodes() {
	std::sort(
		nodes.begin(),
		nodes.end(),
		[](const AutomationNode& lhs, const AutomationNode& rhs)
		{
			return lhs.frame < rhs.frame;
		});
}

void AutomationGroup::AddAutomation(const std::string& name, const int32_t& channel) {
	automations.emplace_back(name, channel);
}

void Sequencer::Update(int32_t frame)
{
	for (auto& group : automationGroups) {
		for (auto& automation : group.automations) {
			for (size_t i = 0; i < automation.nodes.size(); i++) {
				if (automation.nodes[i].frame > frame) {
					if (i != 0) {
						if (automation.nodes[i - 1].flags & AutomationNodeFlags_Step) {
							dmx_interface.setChannel(automation.channel, automation.nodes[i - 1].value);
						}
						else if (automation.nodes[i - 1].flags & AutomationNodeFlags_Lerp) {
							dmx_interface.setChannel(automation.channel, lerp(automation.nodes[i - 1], automation.nodes[i], *currentFrame));
						}
						else {
							m_assert(false, "No interpolation mode was set for node");
						}
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

void Sequencer::Render()
{
	bool doDelete = false;
	bool wasNodeSelected = false;
	bool wasAutomationSelected = false;
	int timelineID = 1;

	if (ImGui::BeginNeoSequencer("Sequencer", currentFrame, &frame, &endFrame, { 0, 0 },
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
					if (ImGui::BeginNeoTimelineEx((automation.name + "###" + std::to_string(timelineID)).c_str(), &automation.active))
					{
						for (auto&& node : automation.nodes)
						{
							ImGui::NeoKeyframe(&node.frame);
							if (ImGui::IsNeoKeyframeSelected()) {
								selectedNode = &node;
								wasNodeSelected = true;
							}
							// Per keyframe code here
						}

						if (ImGui::IsNeoTimelineSelected()) {
							selectedAutomation = &automation;
							wasAutomationSelected = true;
						}

						if (doDelete)
						{
							uint32_t count = ImGui::GetNeoKeyframeSelectionSize();

							ImGui::FrameIndexType* toRemove = new ImGui::FrameIndexType[count];

							ImGui::GetNeoKeyframeSelection(toRemove);

							//Delete keyframes from your structure
						}
						timelineID++;
						ImGui::EndNeoTimeLine();
					}
				}
				ImGui::EndNeoGroup();
			}
		}
		ImGui::EndNeoSequencer();
	}
	if (!wasNodeSelected) {
		selectedNode = NULL;
	}
	if (!wasAutomationSelected) {
		selectedAutomation = NULL;
	}
}

void Sequencer::RenderEditor() {
	if (ImGui::Begin("Sequencer Editor")) {
		ImGui::Text("Sequencer:");
		ImGui::DragScalar("Start Frame", ImGuiDataType_S32, &frame, 0.25F, 0, &endFrame);
		ImGui::DragScalar("End Frame", ImGuiDataType_S32, &endFrame, 0.25F, &frame, 0);

		ImGui::NewLine();

		ImGui::Text("Automation:");
		if (selectedAutomation != NULL) {
			int previousChannel = selectedAutomation->channel;
			ImGui::InputText("Name", &selectedAutomation->name);
			if (ImGui::DragInt("Channel", &selectedAutomation->channel, 0.25F, 1, 512)) {
				dmx_interface.setChannel(previousChannel, 0);
			}
			//ImGui::Checkbox("Active", &selectedAutomation->active); //TODO: Setting inactive does not do as expected (incomplete)
		}

		ImGui::NewLine();

		ImGui::Text("Node:");
		if (selectedNode != NULL) {
			ImGui::DragScalar("Frame", ImGuiDataType_S32, &selectedNode->frame, 0.25F, &frame);
			ImGui::DragScalar("Value", ImGuiDataType_U8, &selectedNode->value, 0.25F);
		}
		ImGui::End();
	}
}

int Sequencer::AddGroup(std::string name){
	automationGroups.emplace_back(name);
	return automationGroups.size() - 1;
}

int Sequencer::AddAutomation(int groupIndex, std::string name, int32_t channel) {
	automationGroups[groupIndex].AddAutomation(name, channel);
	return automationGroups[groupIndex].automations.size() - 1;
}

void Sequencer::AddNode(int groupIndex, int automationIndex, int32_t frame, UCHAR value, AutomationNodeFlags flags) {
	automationGroups[groupIndex].automations[automationIndex].AddNode(frame, value, flags);
}
