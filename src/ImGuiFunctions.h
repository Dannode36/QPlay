#pragma once
#include "imgui.h" // necessary for ImGui::*, imgui-SFML.h doesn't include imgui.h
#include "imgui-SFML.h" // for ImGui::SFML::* functions and SFML-specific overloads
#include "Application.h"
#include <WTypesbase.h>
#include "fmt/format.h"

void ImGuiRenderUCharArray(UCHAR* arr, int length, int wrap, int offset = 0) {
    static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_SizingFixedSame | ImGuiTableFlags_NoHostExtendX;
    ImGui::Text("DMX Output");
    if (ImGui::BeginTable("DMX Output", wrap, flags)) {
        ImGui::TableSetupColumn("col1##", ImGuiTableColumnFlags_WidthFixed, 22);
        for (int i = offset; i < length; i++)
        {
            if (ImGui::TableNextColumn()) {
                ImGui::Text("%d", arr[i]);
            }
        }
        ImGui::EndTable();
    }
}

void ImGuiRenderSelectableUCharArray(UCHAR* arr, int& selection, int length, int wrap, int offset = 0) {
    static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_SizingFixedSame | ImGuiTableFlags_NoHostExtendX;
    ImGui::Text("DMX Output");
    if (ImGui::BeginTable("DMX Output", wrap, flags)) {
        ImGui::TableSetupColumn("col1##", ImGuiTableColumnFlags_WidthFixed, 22);
        for (int i = offset; i < length; i++)
        {
            if (ImGui::TableNextColumn()) {
                std::string name = "Dude";

                if (ImGui::Selectable(fmt::format("{}##{}", arr[i], i).c_str())) {
                    ImGui::SetNextItemAllowOverlap();
                    ImGui::Text("%d", arr[i]); //array value
                    selection = i;
                    fmt::print("Selection: DMX channel {}\n", i);
                }
            }
        }
        ImGui::EndTable();
    }
}

void Application::ImGuiRenderDMXDebug(bool* p_open) {
    const int faderMin = 0;
    const int faderMax = 100;
    const int valueMin = 0;
    const int valueMax = 255;
    static int prevDmxChannel = 1;
    static int selDmxChannel = 1;
    static UCHAR prevDmxValue = 0;
    static UCHAR selDmxValue = 0;

    if (ImGui::Begin("DMX Debugger", p_open)) {
        ImGui::Text(dmx_interface.connected ? "DMX USB Connection: Connected" : "DMX USB Connection: Disconnected");
        ImGui::Text(dmx_interface.streaming ? "DMX USB Streaming: Yes" : "DMX USB Streaming: No");
        ImGui::Text("DMX Interface Status: %d\n", dmx_interface.status);

        if (ImGui::Button("Connect")) {
            printf("Starting DMX connection\n");
            dmx_interface.start();
        }
        ImGui::SameLine();
        if (ImGui::Button("Disconnect")) {
            dmx_interface.stop();
        }

        ImGui::Spacing();

        //Channel Testing
        if (ImGui::CollapsingHeader("Channel Tester")) {
            if (ImGui::DragInt("Channel", &selDmxChannel, 0.25F, 1, BUFFER_SIZE - 1)) {
                dmx_interface.setChannel(prevDmxChannel, prevDmxValue); //Reset channel value if not stored
                prevDmxValue = dmx_interface.getChannel(selDmxChannel); //Assign selected value to previous
                prevDmxChannel = selDmxChannel; //Assign selected channel to previous

                dmx_interface.setChannel(selDmxChannel, selDmxValue); //Update current channel
            }
            if (ImGui::DragScalar("Value", ImGuiDataType_U8, &selDmxValue, 0.25F)) {
                dmx_interface.setChannel(selDmxChannel, selDmxValue);
            }

            ImGui::Spacing();

            if (ImGui::Button("Store")) {
                prevDmxValue = selDmxValue; //Channel is not overriden when the sel channel is changed
            }
            ImGui::SameLine();
            if (ImGui::Button("Release")) {
                dmx_interface.bufferResetWithValue(0);
            }
        }

        ImGui::Spacing();

        ImGui::SliderScalar("Master Fader", ImGuiDataType_U8, &dmx_interface.masterFaderPercent, &faderMin, &faderMax);

        //Display DMX output
        const char* items[] = { "Programmer Buffer", "Output Buffer" };
        static const char* current_item = "Programmer Buffer";
        if (ImGui::BeginCombo("DMX Buffers", current_item)) // The second parameter is the label previewed before opening the combo.
        {
            for (int n = 0; n < IM_ARRAYSIZE(items); n++)
            {
                bool is_selected = (current_item == items[n]); // You can store your selection however you want, outside or inside your objects
                if (ImGui::Selectable(items[n], is_selected)) {
                    current_item = items[n];
                    if (is_selected) {
                        ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
                    }
                }
            }
            ImGui::EndCombo();
        }
        if (current_item == "Programmer Buffer") {
            ImGuiRenderUCharArray(dmx_interface.buffer, BUFFER_SIZE, 21, 1);
        }
        else if (current_item == "Output Buffer") {
            ImGuiRenderUCharArray(dmx_interface.write_buffer, BUFFER_SIZE, 21, 1);
        }
    }
    ImGui::End();
}

//-----------------------------------------------------------------------------
//Cues
//-----------------------------------------------------------------------------

void Application::ImGuiRenderCueList(std::vector<Cue>& cueSequence) {
    if (ImGui::BeginListBox("Cue Sequence 1")) {
        int i = 0;
        for (auto& cue : cueSequence) {
            if (ImGui::Selectable(fmt::format("#{}: Cue {}", i, cue.number).c_str()))
            {
                recallCue(&cue);
            }
            i++;
        }
        ImGui::EndListBox();
    }
}

void Application::ImGuiRenderCueControls(bool* p_open) {
    if (ImGui::Begin("Cues")) {
        static float cueNumber = 0;
        ImGui::InputFloat("Cue Number", &cueNumber);
        if (ImGui::Button("Store Look As Cue")) {
            if (storeLookAsCue(cueNumber, false)) {
                cueNumber = floor(cueNumber + 1);
            }
        }
        if (ImGui::BeginPopupModal("cueNumExistsInSeq"))
        {
            ImGui::TextColored(ImColor(200, 10, 10), fmt::format("Cue {} already exists", cueNumber).c_str());
            if (ImGui::Button("Cancel")) { ImGui::CloseCurrentPopup(); }

            ImGui::SameLine();

            if (ImGui::Button("Overwrite")) {
                if (storeLookAsCue(cueNumber, true)) {
                    cueNumber = floor(cueNumber + 1);
                }
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
        ImGuiRenderCueList(cueSequence);
    }
    ImGui::End();
}
