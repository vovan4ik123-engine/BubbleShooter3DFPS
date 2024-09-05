#pragma once

#include "EngineHeaders.h"
#include "PlayStateGUILayer.h"
#include "player/Player.h"

namespace BubbleShooter3D
{
    class PlayStateSceneLayer : public Beryll::Layer
    {
    public:
        PlayStateSceneLayer(std::shared_ptr<PlayStateGUILayer> gui);
        ~PlayStateSceneLayer() override;

        void updateBeforePhysics() override;
        void updateAfterPhysics() override;
        void draw() override;

    private:
        void loadPlayer();
        void loadEnv();
        void loadEnemies();
        void loadShadersAndLight();
        void handleControls();
        void handleCamera();

        std::shared_ptr<PlayStateGUILayer> m_gui;

        std::shared_ptr<Player> m_player;
        std::vector<std::shared_ptr<Beryll::BaseSimpleObject>> m_playerBullets;
        std::vector<std::shared_ptr<Beryll::BaseSimpleObject>> m_enemies;
        std::vector<std::shared_ptr<Beryll::BaseSimpleObject>> m_staticEnv;
        std::vector<std::shared_ptr<Beryll::BaseSimpleObject>> m_simpleObjForShadowMap;

        // Shaders and light.
        std::shared_ptr<Beryll::Shader> m_simpleObjSunLight;
        std::shared_ptr<Beryll::Shader> m_simpleObjSunLightShadows;
        std::unique_ptr<Beryll::ShadowMap> m_shadowMap;
        glm::mat4 m_sunLightVPMatrix{1.0f};
        // Assign in subclass constructor.
        glm::vec3 m_dirToSun{0.0f};
        glm::vec3 m_sunLightDir{0.0f};

        std::unique_ptr<Beryll::SkyBox> m_skyBox;
    };
}
