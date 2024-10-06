#pragma once

#include "EngineHeaders.h"

namespace BubbleShooter3D
{
    class PlayStateGUILayer : public Beryll::Layer
    {
    public:
        PlayStateGUILayer();
        ~PlayStateGUILayer() override;

        void updateBeforePhysics() override;
        void updateAfterPhysics() override;
        void draw() override;

        std::shared_ptr<Beryll::SliderHorizontal> slider1;
        std::shared_ptr<Beryll::SliderHorizontal> slider2;
        std::shared_ptr<Beryll::SliderHorizontal> slider3;

        std::shared_ptr<Beryll::Joystick> playerJoystick;
        float playerHPFraction = 0.0f; // 0.0f...1.0f range.

        bool dieMenuShow = false;

    private:
        std::vector<std::shared_ptr<Beryll::GUIObject>> m_guiObjects;

        bool m_statisticsShow = true;
        std::shared_ptr<Beryll::Text> m_statistics1;
        std::shared_ptr<Beryll::Text> m_statistics2;
        uint64_t m_statisticsUpdateTime = 0;

        // GUI based on raw ImGUI.
        // ImGUI flags.
        int m_noBackgroundNoFrame = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                                    ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground |
                                    ImGuiWindowFlags_NoScrollbar;

        int m_noFrame = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar;

        // Counters.
        ImFont* m_countersFont;
        std::string m_counterStr;

        std::unique_ptr<Beryll::Texture> m_restartButtonTexture;
        bool m_restartButtonClicked = false;
    };
}
