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

    private:
        std::vector<std::shared_ptr<Beryll::GUIObject>> m_guiObjects;

        bool m_statisticsShow = true;
        std::shared_ptr<Beryll::Text> m_statistics1;
        std::shared_ptr<Beryll::Text> m_statistics2;
        uint64_t m_statisticsUpdateTime = 0;
    };
}
