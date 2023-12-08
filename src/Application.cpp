#include "Application.h"

int Application::Start()
{
    window.create(sf::VideoMode(1280, 720), "ImGui + SFML = <3");
    window.setFramerateLimit(60);
    ImGui::SFML::Init(window);

    //DMX
    std::vector<UCHAR[512]> cue_list;
    OpenDMX dmx_interface;
    dmx_interface.setChannel(0, 255);

    FT_STATUS ftStatus;
    DWORD numDevs;
    // create the device information list
    ftStatus = FT_CreateDeviceInfoList(&numDevs);
    if (ftStatus == FT_OK) {
        printf("Number of devices is %d\n", numDevs);
    }
    else {
        // FT_CreateDeviceInfoList failed
    }

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

        ImGui::SFML::SetCurrentWindow(window);

        ImGuiRenderDMXDebug();
        ImGui::ShowDemoWindow();

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
    cueSequence.emplace_back(pos, dmx_interface.buffer, ARRAY_SIZE(dmx_interface.buffer));
    std::sort(cueSequence.begin(),
        cueSequence.end(),
        [](const Cue& lhs, const Cue& rhs)
        {
            return lhs.number < rhs.number;
        });
}
