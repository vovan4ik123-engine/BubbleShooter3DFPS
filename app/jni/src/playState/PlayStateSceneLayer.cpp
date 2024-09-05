#include "PlayStateSceneLayer.h"
#include "EnumsAndVariables.h"

namespace BubbleShooter3D
{
    PlayStateSceneLayer::PlayStateSceneLayer(std::shared_ptr<PlayStateGUILayer> gui) : m_gui(std::move(gui))
    {
        m_ID = Beryll::LayerID::PLAY_SCENE;

        loadPlayer();
        loadEnv();
        loadEnemies();
        loadShadersAndLight();

        //BR_INFO(" X:%f Y:%f Z:%f", .x, .y, .z);
        //BR_INFO("%s", "");
        //BR_INFO(" %f", );
        //BR_INFO(" %d", );
    }

    PlayStateSceneLayer::~PlayStateSceneLayer()
    {

    }

    void PlayStateSceneLayer::updateBeforePhysics()
    {
        handleControls();
    }

    void PlayStateSceneLayer::updateAfterPhysics()
    {



        handleCamera();
    }

    void PlayStateSceneLayer::draw()
    {
        //BR_INFO("%s", "scene draw call");

        // 1. Draw into shadow map.
        glm::vec3 sunPos = m_player->getOrigin() + m_dirToSun * 20.0f;
        glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 5.0f, 40.0f);
        glm::mat4 lightView = glm::lookAt(sunPos, sunPos + m_sunLightDir, BeryllConstants::worldUp);

        m_sunLightVPMatrix = lightProjection * lightView;

        Beryll::Renderer::disableFaceCulling();
        m_shadowMap->drawIntoShadowMap(m_simpleObjForShadowMap, {}, m_sunLightVPMatrix);
        Beryll::Renderer::enableFaceCulling();

        // 2. Draw scene.
        glm::mat4 modelMatrix{1.0f};

        m_simpleObjSunLight->bind();
        m_simpleObjSunLight->set3Float("sunLightDir", m_sunLightDir);
        m_simpleObjSunLight->set3Float("cameraPos", Beryll::Camera::getCameraPos());
        m_simpleObjSunLight->set1Float("ambientLight", 0.5f);
        m_simpleObjSunLight->set1Float("sunLightStrength", 1.0f);
        m_simpleObjSunLight->set1Float("specularLightStrength", 1.1f);

        modelMatrix = m_player->getModelMatrix();
        m_simpleObjSunLight->setMatrix4x4Float("modelMatrix", modelMatrix);
        m_simpleObjSunLight->setMatrix3x3Float("normalMatrix", glm::mat3(modelMatrix));
        Beryll::Renderer::drawObject(m_player, modelMatrix, m_simpleObjSunLight);

        m_simpleObjSunLightShadows->bind();
        m_simpleObjSunLightShadows->set3Float("sunLightDir", m_sunLightDir);
        m_simpleObjSunLightShadows->set3Float("cameraPos", Beryll::Camera::getCameraPos());
        m_simpleObjSunLightShadows->set1Float("ambientLight", 0.2f);
        m_simpleObjSunLightShadows->set1Float("sunLightStrength", 1.0f);
        m_simpleObjSunLightShadows->set1Float("specularLightStrength", 0.5f);

        for(const auto& staticObj : m_staticEnv)
        {
            modelMatrix = staticObj->getModelMatrix();
            m_simpleObjSunLightShadows->setMatrix4x4Float("MVPLightMatrix", m_sunLightVPMatrix * modelMatrix);
            m_simpleObjSunLightShadows->setMatrix4x4Float("modelMatrix", modelMatrix);
            m_simpleObjSunLightShadows->setMatrix3x3Float("normalMatrix", glm::mat3(modelMatrix));
            Beryll::Renderer::drawObject(staticObj, modelMatrix, m_simpleObjSunLightShadows);
        }
    }

    void PlayStateSceneLayer::loadPlayer()
    {
        m_player = std::make_shared<Player>("models3D/Player.fbx",
                                            EnumsAndVars::playerMass,
                                            true,
                                            Beryll::CollisionFlags::STATIC,
                                            Beryll::CollisionGroups::PLAYER,
                                            Beryll::CollisionGroups::MOVABLE_ENEMY,
                                            Beryll::SceneObjectGroups::PLAYER);

        m_simpleObjForShadowMap.push_back(m_player);
    }

    void PlayStateSceneLayer::loadEnv()
    {
        const auto ground = std::make_shared<Beryll::SimpleCollidingObject>("models3D/Ground.fbx",
                                                                            0.0f,
                                                                            false,
                                                                            Beryll::CollisionFlags::STATIC,
                                                                            Beryll::CollisionGroups::GROUND,
                                                                            Beryll::CollisionGroups::PLAYER_BULLET,
                                                                            Beryll::SceneObjectGroups::GROUND);

        m_staticEnv.push_back(ground);
    }

    void PlayStateSceneLayer::loadEnemies()
    {

    }

    void PlayStateSceneLayer::loadShadersAndLight()
    {
        m_simpleObjSunLight = Beryll::Renderer::createShader("shaders/GLES/SimpleObjectSunLight.vert",
                                                             "shaders/GLES/SimpleObjectSunLight.frag");
        m_simpleObjSunLight->bind();
        m_simpleObjSunLight->activateDiffuseTextureMat1();

        m_simpleObjSunLightShadows = Beryll::Renderer::createShader("shaders/GLES/SimpleObjectSunLightShadows.vert",
                                                                    "shaders/GLES/SimpleObjectSunLightShadows.frag");
        m_simpleObjSunLightShadows->bind();
        m_simpleObjSunLightShadows->activateDiffuseTextureMat1();
        m_simpleObjSunLightShadows->activateShadowMapTexture();

        m_shadowMap = Beryll::Renderer::createShadowMap(1024, 1024);

        m_dirToSun = glm::normalize(glm::vec3(-0.5f, 2.0f, -1.0f));
        m_sunLightDir = -m_dirToSun;
    }

    void PlayStateSceneLayer::handleControls()
    {

    }

    void PlayStateSceneLayer::handleCamera()
    {
        Beryll::Camera::setCameraPos(m_player->getOrigin() + glm::vec3(-1.0f, 0.6f, 0.0f) * 30.0f);
        Beryll::Camera::setCameraFrontPos(m_player->getOrigin());
    }
}
