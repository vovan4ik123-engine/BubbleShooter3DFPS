#include "PlayStateSceneLayer.h"
#include "EnumsAndVariables.h"
#include "Sounds.h"

namespace BubbleShooter3D
{
    PlayStateSceneLayer::PlayStateSceneLayer(std::shared_ptr<PlayStateGUILayer> gui) : m_gui(std::move(gui))
    {
        m_ID = Beryll::LayerID::PLAY_SCENE;

        m_playerBullets.reserve(200);
        m_enemies.reserve(700);
        m_allDynamicObjects.reserve(200);
        m_staticEnv.reserve(10);
        m_simpleObjForShadowMap.reserve(10);

        loadPlayer();
        loadEnv();
        loadEnemies();
        loadShadersAndLight();

        m_skyBox = Beryll::Renderer::createSkyBox("skyboxes/nightClouds");

        Sounds::loadSounds();
        Sounds::reset();
        EnumsAndVars::reset();
        Beryll::TimeStep::fixateTime();

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
        EnumsAndVars::playTimeSec += Beryll::TimeStep::getTimeStepSec();
        Sounds::update();

        m_player->update();
        handleControls();
        checkMapBorders();
    }

    void PlayStateSceneLayer::updateAfterPhysics()
    {
        for(const std::shared_ptr<Beryll::SceneObject>& so : m_allDynamicObjects)
        {
            if(so->getIsEnabledUpdate())
                so->updateAfterPhysics();


            if(so->getOrigin().y < -50.0f)
            {
                so->disableUpdate();
                so->disableCollisionMesh();
                so->disableDraw();
            }
        }

        handleCamera();
    }

    void PlayStateSceneLayer::draw()
    {
        //BR_INFO("%s", "scene draw call");

        // 1. Draw into shadow map.
        glm::vec3 sunPos = m_player->getOrigin() +
                           (Beryll::Camera::getCameraFrontDirectionXZ() * 200.0f) +
                           (m_dirToSun * 200.0f);
        glm::mat4 lightProjection = glm::ortho(-300.0f, 300.0f, -300.0f, 300.0f, 5.0f, 400.0f);
        glm::mat4 lightView = glm::lookAt(sunPos, sunPos + m_sunLightDir, BeryllConstants::worldUp);

        m_sunLightVPMatrix = lightProjection * lightView;

        //Beryll::Renderer::disableFaceCulling();
        m_shadowMap->drawIntoShadowMap(m_simpleObjForShadowMap, {}, m_sunLightVPMatrix);
        //Beryll::Renderer::enableFaceCulling();

        // 2. Draw scene.
        glm::mat4 modelMatrix{1.0f};

        m_simpleObjSunLightShadows->bind();
        m_simpleObjSunLightShadows->set3Float("sunLightDir", m_sunLightDir);
        m_simpleObjSunLightShadows->set3Float("cameraPos", Beryll::Camera::getCameraPos());
        m_simpleObjSunLightShadows->set1Float("ambientLight", 0.7f);
        m_simpleObjSunLightShadows->set1Float("specularLightStrength", 0.5f);

        modelMatrix = m_player->getModelMatrix();
        m_simpleObjSunLightShadows->setMatrix4x4Float("MVPLightMatrix", m_sunLightVPMatrix * modelMatrix);
        m_simpleObjSunLightShadows->setMatrix4x4Float("modelMatrix", modelMatrix);
        m_simpleObjSunLightShadows->setMatrix3x3Float("normalMatrix", glm::mat3(modelMatrix));
        Beryll::Renderer::drawObject(m_player, modelMatrix, m_simpleObjSunLightShadows);

        for(const auto& bullet : m_playerBullets)
        {
            if(bullet->getIsEnabledDraw())
            {
                modelMatrix = bullet->getModelMatrix();
                m_simpleObjSunLightShadows->setMatrix4x4Float("MVPLightMatrix", m_sunLightVPMatrix * modelMatrix);
                m_simpleObjSunLightShadows->setMatrix4x4Float("modelMatrix", modelMatrix);
                m_simpleObjSunLightShadows->setMatrix3x3Float("normalMatrix", glm::mat3(modelMatrix));
                Beryll::Renderer::drawObject(bullet, modelMatrix, m_simpleObjSunLightShadows);
            }
        }

        for(const auto& staticObj : m_staticEnv)
        {
            modelMatrix = staticObj->getModelMatrix();
            m_simpleObjSunLightShadows->setMatrix4x4Float("MVPLightMatrix", m_sunLightVPMatrix * modelMatrix);
            m_simpleObjSunLightShadows->setMatrix4x4Float("modelMatrix", modelMatrix);
            m_simpleObjSunLightShadows->setMatrix3x3Float("normalMatrix", glm::mat3(modelMatrix));
            Beryll::Renderer::drawObject(staticObj, modelMatrix, m_simpleObjSunLightShadows);
        }

        m_simpleObjSunLightShadowsNormals->bind();
        m_simpleObjSunLightShadowsNormals->set3Float("sunLightDir", m_sunLightDir);
        m_simpleObjSunLightShadowsNormals->set3Float("cameraPos", Beryll::Camera::getCameraPos());
        m_simpleObjSunLightShadowsNormals->set1Float("ambientLight", 0.5f);
        m_simpleObjSunLightShadowsNormals->set1Float("specularLightStrength", 0.3f);

        for(const auto& normalMapObj : m_objWithNormalMap)
        {
            modelMatrix = normalMapObj->getModelMatrix();
            m_simpleObjSunLightShadowsNormals->setMatrix4x4Float("MVPLightMatrix", m_sunLightVPMatrix * modelMatrix);
            m_simpleObjSunLightShadowsNormals->setMatrix4x4Float("modelMatrix", modelMatrix);
            m_simpleObjSunLightShadowsNormals->setMatrix3x3Float("normalMatrix", glm::mat3(modelMatrix));
            Beryll::Renderer::drawObject(normalMapObj, modelMatrix, m_simpleObjSunLightShadowsNormals);
        }

        m_bulletTrajectory.calculateAndDraw(EnumsAndVars::bulletMass,
                                            EnumsAndVars::bulletGravity,
                                            m_bulletStartPosition,
                                            m_bulletAngleRadians,
                                            m_bulletImpulseVector,
                                            glm::vec3(1.0f),
                                            m_sunLightDir);

        m_skyBox->draw();
        //Beryll::ParticleSystem::draw();
    }

    void PlayStateSceneLayer::loadPlayer()
    {
        m_player = std::make_shared<Player>("models3D/player/Player.fbx",
                                            EnumsAndVars::playerMass,
                                            true,
                                            Beryll::CollisionFlags::DYNAMIC,
                                            Beryll::CollisionGroups::PLAYER,
                                            Beryll::CollisionGroups::STATIC_ENVIRONMENT | Beryll::CollisionGroups::JUMPPAD,
                                            Beryll::SceneObjectGroups::PLAYER);

        m_player->setOrigin(glm::vec3(350.0f, 5.0f, -150.0f));
        m_player->getController().moveSpeed = 50.0f;
        m_player->setGravity(glm::vec3(0.0f, -70.0f, 0.0f));
        m_player->setAngularFactor(glm::vec3(0.0f));
        m_player->setLinearFactor(glm::vec3(1.0f, 1.0f, 1.0f));

        m_allDynamicObjects.push_back(m_player);
        m_simpleObjForShadowMap.push_back(m_player);

        for(int i = 0; i < 40; ++i)
        {
            const auto bullet = std::make_shared<Beryll::SimpleCollidingObject>("models3D/player/PlayerBullet.fbx",
                                                                                EnumsAndVars::bulletMass,
                                                                                true,
                                                                                Beryll::CollisionFlags::DYNAMIC,
                                                                                Beryll::CollisionGroups::PLAYER_BULLET,
                                                                                Beryll::CollisionGroups::STATIC_ENVIRONMENT,
                                                                                Beryll::SceneObjectGroups::BULLET);

            bullet->disableUpdate();
            bullet->disableCollisionMesh();
            bullet->disableDraw();

            m_playerBullets.push_back(bullet);
            m_allDynamicObjects.push_back(bullet);
        }
    }

    void PlayStateSceneLayer::loadEnv()
    {
        const auto groundsNormalMap = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/map1/GroundNormalMap.fbx",
                                                                            0.0f,
                                                                            false,
                                                                            Beryll::CollisionFlags::STATIC,
                                                                            Beryll::CollisionGroups::STATIC_ENVIRONMENT,
                                                                            Beryll::CollisionGroups::PLAYER | Beryll::CollisionGroups::PLAYER_BULLET,
                                                                            Beryll::SceneObjectGroups::STATIC_ENVIRONMENT);

        for(const auto& obj : groundsNormalMap)
        {
            m_objWithNormalMap.push_back(obj);
        }

        const auto staticEnv = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/map1/StaticEnv.fbx",
                                                                                        0.0f,
                                                                                        false,
                                                                                        Beryll::CollisionFlags::STATIC,
                                                                                        Beryll::CollisionGroups::STATIC_ENVIRONMENT,
                                                                                        Beryll::CollisionGroups::PLAYER | Beryll::CollisionGroups::PLAYER_BULLET,
                                                                                        Beryll::SceneObjectGroups::STATIC_ENVIRONMENT);

        for(const auto& obj : staticEnv)
        {
            m_staticEnv.push_back(obj);
            m_simpleObjForShadowMap.push_back(obj);
        }

        const auto jumpPads = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/map1/JumpPads.fbx",
                                                                                       0.0f,
                                                                                       false,
                                                                                       Beryll::CollisionFlags::STATIC,
                                                                                       Beryll::CollisionGroups::JUMPPAD,
                                                                                       Beryll::CollisionGroups::PLAYER,
                                                                                       Beryll::SceneObjectGroups::JUMPPAD);

        for(const auto& obj : jumpPads)
        {
            m_staticEnv.push_back(obj);
        }

        const auto envNoColliders1 = Beryll::SimpleObject::loadManyModelsFromOneFile("models3D/map1/EnvNoColliders.fbx", Beryll::SceneObjectGroups::STATIC_ENVIRONMENT);

        for(const auto& obj : envNoColliders1)
        {
            m_staticEnv.push_back(obj);
            m_simpleObjForShadowMap.push_back(obj);
        }
    }

    void PlayStateSceneLayer::loadEnemies()
    {

    }

    void PlayStateSceneLayer::loadShadersAndLight()
    {
        m_simpleObjSunLightShadows = Beryll::Renderer::createShader("shaders/GLES/SimpleObjectSunLightShadows.vert",
                                                                    "shaders/GLES/SimpleObjectSunLightShadows.frag");
        m_simpleObjSunLightShadows->bind();
        m_simpleObjSunLightShadows->activateDiffuseTextureMat1();
        m_simpleObjSunLightShadows->activateShadowMapTexture();

        m_simpleObjSunLightShadowsNormals = Beryll::Renderer::createShader("shaders/GLES/SimpleObjectSunLightShadowsNormals.vert",
                                                                           "shaders/GLES/SimpleObjectSunLightShadowsNormals.frag");
        m_simpleObjSunLightShadowsNormals->bind();
        m_simpleObjSunLightShadowsNormals->activateDiffuseTextureMat1();
        m_simpleObjSunLightShadowsNormals->activateNormalMapTextureMat1();
        m_simpleObjSunLightShadowsNormals->activateShadowMapTexture();

        m_animatedObjSunLight = Beryll::Renderer::createShader("shaders/GLES/AnimatedObjectSunLight.vert",
                                                               "shaders/GLES/AnimatedObjectSunLight.frag");
        m_animatedObjSunLight->bind();
        m_animatedObjSunLight->activateDiffuseTextureMat1();

        m_shadowMap = Beryll::Renderer::createShadowMap(3000, 3000);

        m_dirToSun = glm::normalize(glm::vec3(1.0f, 2.0f, -0.5f));
        m_sunLightDir = -m_dirToSun;
    }

    void PlayStateSceneLayer::handleControls()
    {
        if(m_gui->playerJoystick->getIsTouched())
        {
            glm::vec2 joyDIr = m_gui->playerJoystick->getDirection();

            if(glm::length(joyDIr) > 0.0f)
            {
                // Default face direction is {1.0f, 0.0f, 0.0f}.
                // needRotationToCamera = rotation between camera and default face dir.
                glm::quat needRotationToCamera = glm::rotation(glm::vec3{1.0f, 0.0f, 0.0f}, Beryll::Camera::getCameraFrontDirectionXZ());
                // Move on XZ plane.
                glm::vec3 moveDir = needRotationToCamera * glm::vec4(joyDIr.y, 0.0f, joyDIr.x, 1.0f);

                m_player->getController().moveToDirection(moveDir, false, false);
            }
        }
        else
        {
            m_gui->playerJoystick->disable();
        }

        std::vector<Beryll::Finger>& fingers = Beryll::EventHandler::getFingers();
        for(Beryll::Finger& f : fingers)
        {
            if(f.handled)
                continue;

            if(f.normalizedPos.x < 0.4f && f.normalizedPos.y > 0.4f)
            {
                if(f.downEvent)
                {
                    m_gui->playerJoystick->enable();
                    m_gui->playerJoystick->setOrigin(f.normalizedPos);
                    f.handled = true;
                }
            }
        }
    }

    void PlayStateSceneLayer::handleCamera()
    {
        const std::vector<Beryll::Finger>& fingers = Beryll::EventHandler::getFingers();
        for(const Beryll::Finger& f : fingers)
        {
            if(f.handled || f.normalizedPos.x < 0.5f)
                continue;

            shootBullet();

            if(f.downEvent)
            {
                m_lastFingerMovePosX = f.SDL2ScreenPos.x;
                m_lastFingerMovePosY = f.SDL2ScreenPos.y;
                break;
            }
            else
            {
                float deltaX = f.SDL2ScreenPos.x - m_lastFingerMovePosX;
                float deltaY = f.SDL2ScreenPos.y - m_lastFingerMovePosY;
                m_lastFingerMovePosX = f.SDL2ScreenPos.x;
                m_lastFingerMovePosY = f.SDL2ScreenPos.y;

                m_eyesLookAngleXZ += deltaX * EnumsAndVars::Settings::cameraHorizontalSpeed;
                m_eyesLookAngleY -= deltaY * EnumsAndVars::Settings::cameraVerticalSpeed;
                if(m_eyesLookAngleY > 35.0f) m_eyesLookAngleY = 35.0f; // Eye up.
                if(m_eyesLookAngleY < -75.0f) m_eyesLookAngleY = -75.0f; // Eye down.
                //BR_INFO("m_eyesLookAngleY %f", m_eyesLookAngleY);
                break;
            }
        }

        // Euler angles.
        float m_eyesLookAngleXZRadians = glm::radians(m_eyesLookAngleXZ);
        float m_eyesLookAngleYRadians = glm::radians(m_eyesLookAngleY);
        m_cameraOffset.x = glm::cos(m_eyesLookAngleXZRadians) * glm::cos(m_eyesLookAngleYRadians);
        m_cameraOffset.y = glm::sin(m_eyesLookAngleYRadians);
        m_cameraOffset.z = glm::sin(m_eyesLookAngleXZRadians) * glm::cos(m_eyesLookAngleYRadians);
        m_cameraOffset = glm::normalize(m_cameraOffset);

        m_cameraFront = m_player->getOrigin();
        m_cameraFront.y += 15.0f;
        Beryll::Camera::setCameraPos(m_cameraFront - m_cameraOffset * m_cameraDistance);
        Beryll::Camera::setCameraFrontPos(m_cameraFront);
        Beryll::Camera::updateCameraVectors();

        m_player->rotateToDirection(Beryll::Camera::getCameraFrontDirectionXZ(), true);

        // Move camera a bit to left side(player's character will not directly on the middle of the screen).
        // That allow player see all trajectory including part going down.
        Beryll::Camera::setCameraPos(Beryll::Camera::getCameraPos() + Beryll::Camera::getCameraLeftXYZ() * 8.0f);
        Beryll::Camera::setCameraFrontPos(Beryll::Camera::getCameraFrontPos() + Beryll::Camera::getCameraLeftXYZ() * 7.0f);

        // Update shoot dir after camera.
        float angleBetweenWorldUpAndCameraBack = BeryllUtils::Common::getAngleInRadians(BeryllConstants::worldUp, Beryll::Camera::getCameraBackDirectionXYZ());
        m_bulletAngleRadians = angleBetweenWorldUpAndCameraBack - glm::half_pi<float>() + 0.2618f; // + 15 degrees.

        m_bulletImpulseVector = m_player->getFaceDirXZ();
        m_bulletImpulseVector.y = glm::tan(m_bulletAngleRadians);
        m_bulletImpulseVector = glm::normalize(m_bulletImpulseVector);
        m_bulletImpulseVector *= EnumsAndVars::bulletMass;
        m_bulletImpulseVector *= m_gui->slider3->getValue();

        m_bulletStartPosition = m_player->getOrigin() + m_player->getFaceDirXZ() * 4.0f;
        m_bulletStartPosition.y += 4.0f;
    }

    void PlayStateSceneLayer::shootBullet()
    {
        if(EnumsAndVars::shotTimeSec + EnumsAndVars::shotDelaySec < EnumsAndVars::playTimeSec)
        {
            if(m_currentBulletIndex >= m_playerBullets.size())
                m_currentBulletIndex = 0;

            m_playerBullets[m_currentBulletIndex]->enableCollisionMesh();
            m_playerBullets[m_currentBulletIndex]->enableUpdate();
            m_playerBullets[m_currentBulletIndex]->enableDraw();

            m_playerBullets[m_currentBulletIndex]->setOrigin(m_bulletStartPosition, true);
            m_playerBullets[m_currentBulletIndex]->applyCentralImpulse(m_bulletImpulseVector);

            ++m_currentBulletIndex;
            EnumsAndVars::shotTimeSec = EnumsAndVars::playTimeSec;
        }
    }

    void PlayStateSceneLayer::checkMapBorders()
    {
        glm::vec3 origin = m_player->getOrigin();
        bool resetOrigin = false;

        if(origin.x < m_playerMinX)
        {
            resetOrigin = true;
            origin.x = m_playerMinX;
        }
        else if(origin.x > m_playerMaxX)
        {
            resetOrigin = true;
            origin.x = m_playerMaxX;
        }

        if(origin.z < m_playerMinZ)
        {
            resetOrigin = true;
            origin.z = m_playerMinZ;
        }
        else if(origin.z > m_playerMaxZ)
        {
            resetOrigin = true;
            origin.z = m_playerMaxZ;
        }

        if(resetOrigin)
        {
            m_player->setOrigin(origin, false);
        }
    }
}
