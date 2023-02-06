#include "SampleWidget.h"

using namespace KGCA41B;

void SampleWidget::Update()
{
    ImGui::SetCurrentContext(GUI->GetContext()); // essential : Keeping Context instance
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode); // selective : Make window dockable
}

void SampleWidget::Render()
{
    /*** IMPORTANT
    
    Begin() and End() must be paired!

    ***/

    // Main Menu bar
    ImGui::BeginMainMenuBar();
    {
        if (ImGui::BeginMenu("Tools"))
        {
            if (ImGui::MenuItem("CharacterTool")) // returns true when menu ckicked
            {
                // to do ...
            }

            if (ImGui::MenuItem("MapTool"))
            {
                // to do ...
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("ImportAsset"))
        {
            // to do ...

            ImGui::EndMenu();
        }        
    }
    ImGui::EndMainMenuBar();

    // Basic Window
    ImGui::Begin("sample widget");
    {

        // window settings                   keep this size always   set this size only at begin. changeable.
        ImGui::SetWindowSize(ImVec2(500, 500), ImGuiCond_Always      /*ImGuiCond_Once*/);

        // menu bar in windows
        if (ImGui::BeginMenuBar())
        {
            // to do ...
            ImGui::EndMenuBar();
        }

        // button
        if (ImGui::Button("this is button", ImVec2(120, 60)))
        {
            int_data++;
        }

        // text
        ImGui::Text((to_string(int_data).c_str()));

        // dummy(just empty space)
        ImGui::Dummy(ImVec2(200, 30));

        // button
        if (ImGui::Button("open new window", ImVec2(120, 60)))
        {
            open_window = true;
        }
    }
    ImGui::End();


    if (open_window)    // This Window Can open & close
    {
        // Push : styles, fonts, or others.
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0.7f, 0, 1));

        ImGui::Begin("open/close window", &open_window);

        {
            ImGui::SetWindowSize(ImVec2(360, 720), ImGuiCond_Once);

            ImGui::Text("This Window Can open & close.");

            ImGui::Text("use boolean value");
        }

        ImGui::End();

        // Pop
        ImGui::PopStyleColor(1);
    }

}
