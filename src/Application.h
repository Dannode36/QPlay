#pragma once
#include "imgui.h" // necessary for ImGui::*, imgui-SFML.h doesn't include imgui.h
#include "imgui-SFML.h" // for ImGui::SFML::* functions and SFML-specific overloads

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include "OpenDMX.h"
#include <vector>

class Application
{
private:
	std::vector<UCHAR[512]> cue_list;
	OpenDMX dmx_interface;
	sf::RenderWindow window;

public:
	int Start();
	void Stop();
private:
	void SetChannel();
};
