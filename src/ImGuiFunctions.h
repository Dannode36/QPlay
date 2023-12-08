#pragma once
#include "imgui.h" // necessary for ImGui::*, imgui-SFML.h doesn't include imgui.h
#include "imgui-SFML.h" // for ImGui::SFML::* functions and SFML-specific overloads
#include "Application.h"
#include <WTypesbase.h>

inline void ImGuiRenderUCharArray(UCHAR* arr, int length, int wrap) {
    static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_SizingFixedSame | ImGuiTableFlags_NoHostExtendX;
    ImGui::Text("DMX Output");
    if (ImGui::BeginTable("DMX Output", wrap, flags)) {
        ImGui::TableSetupColumn("col1##", ImGuiTableColumnFlags_WidthFixed, 22);
        for (int i = 0; i < length; i++)
        {
            if (ImGui::TableNextColumn()) {
                ImGui::Text("%d", arr[i]);
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
    static int selectedChannel = 1;
    static UCHAR selectedChannelValue;

    if (ImGui::Begin("QPlay Debug", p_open)) {
        ImGui::Text(dmx_interface.connected ? "DMX USB Connection: Connected" : "DMX USB Connection: Disconnected");
        ImGui::Text(dmx_interface.streaming ? "DMX USB Streaming: Yes" : "DMX USB Streaming: No");
        ImGui::Text("DMX Interface Status: %d\n", dmx_interface.status);

        if (ImGui::Button("Connect")) {
            dmx_interface.start();
        }
        ImGui::SameLine();
        if (ImGui::Button("Disconnect")) {
            dmx_interface.stop();
        }

        ImGui::Spacing();
        
        selectedChannelValue = dmx_interface.getChannel(selectedChannel);
        if (ImGui::DragInt("Channel", &selectedChannel, 0.5F, faderMin + 1, DMX_CHANNELS)) {
        }
        if (ImGui::DragScalar("Value", ImGuiDataType_U8, &selectedChannelValue)) {
            dmx_interface.setChannel(selectedChannel, selectedChannelValue);
        }

        ImGui::Spacing();

        ImGui::SliderScalar("Master Fader", ImGuiDataType_U8, &dmx_interface.masterFaderPercent, &faderMin, &faderMax);
        ImGuiRenderUCharArray(dmx_interface.write_buffer, DMX_CHANNELS, 22);
    }
    ImGui::End();
}
