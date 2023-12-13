#include "Application.h"
#include <fmt/format.h>

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
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::D && event.key.control && event.key.alt) {
                    showImGuiDemoWindow = !showImGuiDemoWindow;
                }
            }
        }

        // Update
        const sf::Time dt = deltaClock.restart();
        ImGui::SFML::Update(window, dt);

        ImGui::SFML::SetCurrentWindow(window);

        ImGuiRenderDMXDebug();
        ImGuiRenderCueControls();

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

bool Application::storeLookAsCue(float number, bool overwrite) {
    auto it = std::find_if(cueSequence.begin(), cueSequence.end(), [&number](const Cue& cue) { return cue.number == number; });
    if (it != cueSequence.end()) {
        if (!overwrite) {
            fmt::print("WARNING: Cue with same ID already exists in the sequence\n");
            ImGui::OpenPopup("cueNumExistsInSeq");
            return false;
        }
    }

    if (overwrite) {
        cueSequence.erase(it);
    }

    cueSequence.emplace_back(number, dmx_interface.buffer, BUFFER_SIZE);
    std::sort(cueSequence.begin(),
        cueSequence.end(),
        [](const Cue& lhs, const Cue& rhs)
        {
            return lhs.number < rhs.number;
        });
    fmt::print("INFO: Added Cue {} to the sequence\n", number);
   
    return true;
}

void Application::recallCue(Cue* cue)
{
    activeCue = cue;
    memcpy(dmx_interface.buffer, cue->buffer, BUFFER_SIZE);
}
