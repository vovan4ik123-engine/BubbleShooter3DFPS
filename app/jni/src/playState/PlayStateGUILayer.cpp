#include "PlayStateGUILayer.h"
#include "EnumsAndVariables.h"

namespace BubbleShooter3D
{
    PlayStateGUILayer::PlayStateGUILayer()
    {
        m_ID = Beryll::LayerID::PLAY_GUI;

        const float screenAR = Beryll::MainImGUI::getInstance()->getGUIScreenAspectRation();

        if(m_statisticsShow)
        {
            m_statistics1 = std::make_shared<Beryll::Text>("Frame: 00000  FPS: 00000", EnumsAndVars::FontsPath::roboto, 0.05f, 0.005f, 0.0f, 0.5f, 0.06f);
            m_guiObjects.push_back(m_statistics1);
            m_statistics2 = std::make_shared<Beryll::Text>("Phys: 00000  Logic: 00000  GPU: 00000", EnumsAndVars::FontsPath::roboto, 0.05f, 0.005f, 0.05f, 0.5f, 0.06f);
            m_guiObjects.push_back(m_statistics2);
        }

//        slider1 = std::make_shared<Beryll::SliderHorizontal>("1", EnumsAndVars::FontsPath::roboto, 0.04f, 0.005f, 0.11f, 0.2f, 0.04f, 10.0f, 50.0f);
//        m_guiObjects.push_back(slider1);
//        slider1->setValue(10.0f);
//
//        slider2 = std::make_shared<Beryll::SliderHorizontal>("1", EnumsAndVars::FontsPath::roboto, 0.04f, 0.005f, 0.16f, 0.2f, 0.04f, 0.1f, 0.5f);
//        m_guiObjects.push_back(slider2);
//        slider2->setValue(0.2f);
//
//        slider3 = std::make_shared<Beryll::SliderHorizontal>("bullet power", EnumsAndVars::FontsPath::roboto, 0.04f, 0.005f, 0.21f, 0.2f, 0.04f, 50.0f, 1500.0f);
//        m_guiObjects.push_back(slider3);
//        slider3->setValue(600.0f);

        playerJoystick = std::make_shared<Beryll::Joystick>("GUI/JoystickDefault.png","",
                                                            0.0f, 0.25f, 0.40f, 0.40f * screenAR);
        m_guiObjects.push_back(playerJoystick);
        playerJoystick->disable();
    }

    PlayStateGUILayer::~PlayStateGUILayer()
    {

    }

    void PlayStateGUILayer::updateBeforePhysics()
    {
        for(const std::shared_ptr<Beryll::GUIObject>& go : m_guiObjects)
        {
            if(go->getIsEnabled())
            {
                go->updateBeforePhysics();
            }
        }

        if(m_statisticsShow && Beryll::TimeStep::getMilliSecFromStart() > m_statisticsUpdateTime + 200) // Update every 200 ms.
        {
            std::stringstream stream;
            stream << std::fixed << std::setprecision(1);
            stream << "Frame: " << Beryll::GameLoop::getFrameTime() << "  FPS: " << Beryll::GameLoop::getFPS();
            m_statistics1->text = stream.str();

            stream.str(""); // Way to clear std::stringstream.
            stream << std::fixed << std::setprecision(1);
            stream << "Phys: " << Beryll::Physics::getSimulationTime();
            stream << "  Logic: " << (Beryll::GameLoop::getCPUTime() - Beryll::Physics::getSimulationTime());
            stream << "  GPU: " << Beryll::GameLoop::getGPUTime();
            m_statistics2->text = stream.str();

            m_statisticsUpdateTime = Beryll::TimeStep::getMilliSecFromStart();
        }
    }

    void PlayStateGUILayer::updateAfterPhysics()
    {

    }

    void PlayStateGUILayer::draw()
    {
        for(const std::shared_ptr<Beryll::GUIObject>& go : m_guiObjects)
        {
            if(go->getIsEnabled())
            {
                go->draw();
            }
        }

        const float GUIWidth = Beryll::MainImGUI::getInstance()->getGUIWidth();
        const float GUIHeight = Beryll::MainImGUI::getInstance()->getGUIHeight();

    }
}
