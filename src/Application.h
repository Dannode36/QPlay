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
    std::shared_ptr<Cue> activeCue;

    OpenDMX dmx_interface;
    sf::RenderWindow window;

public:
    int Start();
    void Stop();

private:
    void setChannel();
    void storeLookAsCue(float pos);

private:
    void ImGuiRenderDMXDebug(bool* p_open = NULL);
};
