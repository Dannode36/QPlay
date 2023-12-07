#include "Application.h"

int Application::Start()
{
    int faderMin = 0;
    int faderMax = 100;

    window.create(sf::VideoMode(1280, 720), "ImGui + SFML = <3");
    window.setFramerateLimit(60);
    ImGui::SFML::Init(window);

    //DMX
    std::vector<UCHAR[512]> cue_list;
    OpenDMX dmx_interface;
    dmx_interface.start(); //Clears up own resources and threads in deconstructor. Calling stop() is not necessary
    dmx_interface.setChannel(0, 255);
    printf("Status of: DMX Interface: %d\n", dmx_interface.status);

    sf::Clock deltaClock;
    while (window.isOpen()) {
        // Main window event processing
        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(window, event);
            if (event.type == sf::Event::Closed) {
                window.close();
                ImGui::SFML::Shutdown(); // will shutdown all windows
                return 0; // return here so that we don't call Update/Render
            }
        }

        // Update
        const sf::Time dt = deltaClock.restart();
        ImGui::SFML::Update(window, dt);

        // Add ImGui widgets in the first window
        ImGui::SFML::SetCurrentWindow(window);
        ImGui::Begin("Hello, world!");
        ImGui::Button("Look at this pretty button");
        ImGui::DragScalar("Master Fader", ImGuiDataType_U8, &dmx_interface.masterFaderPercent, 1.0F, &faderMin, &faderMax);
        ImGui::End();
        ImGui::ShowDemoWindow();
        // Add ImGui widgets in the child window

        // Main window drawing
        /*sf::CircleShape shape(100.f);
        shape.setFillColor(sf::Color::Green);*/

        window.clear();
        //window.draw(shape);
        ImGui::SFML::Render(window);
        window.display();
    }

    return 0;
}

void Application::Stop()
{
}

void Application::storeLookAsCue(float pos)
{
    cueSequence.emplace_back(pos, dmx_interface.buffer);
    std::sort(cueSequence.begin(),
        cueSequence.end(),
        [](const Cue& lhs, const Cue& rhs)
        {
            return lhs.number < rhs.number;
        });
}
