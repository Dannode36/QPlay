#pragma once
#include "imgui.h" // necessary for ImGui::*, imgui-SFML.h doesn't include imgui.h
#include "imgui-SFML.h" // for ImGui::SFML::* functions and SFML-specific overloads

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include "OpenDMX.h"
#include <vector>
#include "Programmer/Cue.h"

class Application
{
private:
    std::vector<Cue> cueSequence;
    Cue* activeCue;

    OpenDMX dmx_interface;
    sf::RenderWindow window;
    bool showImGuiDemoWindow{ false };

    bool timelinePlaying{ false };
    int32_t currentFrame = 0;
    sf::Time frameDelta = sf::milliseconds(25); //40Hz
public:
    int Start();
    void Stop();

private:
    //void setChannel();
    bool storeLookAsCue(float pos, bool overwrite);
    void recallCue(Cue* cue);

private:
    void RenderToolbar();
    void ImGuiRenderDMXDebug(bool* p_open = NULL);
    void ImGuiRenderCueControls(bool* p_open = NULL);
    void ImGuiRenderCueList(std::vector<Cue>& cueSequence);
};
