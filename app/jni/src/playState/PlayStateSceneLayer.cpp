#include "PlayStateSceneLayer.h"
#include "EnumsAndVariables.h"
#include "Sounds.h"

namespace BubbleShooter3D
{
    PlayStateSceneLayer::PlayStateSceneLayer(std::shared_ptr<PlayStateGUILayer> gui) : m_gui(std::move(gui))
    {
        m_ID = Beryll::LayerID::PLAY_SCENE;

        m_playerBullets.reserve(200);
        m_enemies.reserve(500);
        m_allDynamicObjects.reserve(700);
        m_staticEnv.reserve(10);
        m_simpleObjForShadowMap.reserve(10);

        loadPlayer();
        loadEnv();
        loadEnemies();
        loadShadersAndLight();

        Sounds::loadSounds();
        Sounds::reset();
        EnumsAndVars::reset();

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

        handleControls();
        //handlePlayer();
    }

    void PlayStateSceneLayer::updateAfterPhysics()
    {
        for(const std::shared_ptr<Beryll::SceneObject>& so : m_allDynamicObjects)
        {
            if(so->getIsEnabledUpdate())
                so->updateAfterPhysics();

            if(so->getOrigin().y < -5.0f)
            {
                so->disableUpdate();
                so->disableCollisionMesh();
                so->disableDraw();
            }
        }

        for(const std::shared_ptr<Beryll::SceneObject>& bullet : m_playerBullets)
        {
            if(bullet->getIsEnabledUpdate())
            {
                int collisionWithID = Beryll::Physics::getAnyCollisionForID(bullet->getID());
                if(collisionWithID > 0)
                {
                    for(Enemy& enemy : m_enemies)
                    {
                        if(collisionWithID == enemy.getID())
                        {
                            //enemy.disable();
                            Sounds::playSoundEffect(SoundType::BULLET_HIT);
                            break;
                        }
                    }
                }
            }
        }

        handleEnemies();
        handleCamera();
    }

    void PlayStateSceneLayer::draw()
    {
        //BR_INFO("%s", "scene draw call");

        // 1. Draw into shadow map.
        glm::vec3 sunPos = m_player->getOrigin() + m_dirToSun * 100.0f;
        glm::mat4 lightProjection = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, 5.0f, 150.0f);
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
        m_simpleObjSunLight->set1Float("ambientLight", 0.7f);
        m_simpleObjSunLight->set1Float("sunLightStrength", 0.25f);
        m_simpleObjSunLight->set1Float("specularLightStrength", 0.75f);

        modelMatrix = m_player->getModelMatrix();
        m_simpleObjSunLight->setMatrix4x4Float("modelMatrix", modelMatrix);
        m_simpleObjSunLight->setMatrix3x3Float("normalMatrix", glm::mat3(modelMatrix));
        Beryll::Renderer::drawObject(m_player, modelMatrix, m_simpleObjSunLight);

        for(const auto& bullet : m_playerBullets)
        {
            if(bullet->getIsEnabledDraw())
            {
                modelMatrix = bullet->getModelMatrix();
                m_simpleObjSunLight->setMatrix4x4Float("modelMatrix", modelMatrix);
                m_simpleObjSunLight->setMatrix3x3Float("normalMatrix", glm::mat3(modelMatrix));
                Beryll::Renderer::drawObject(bullet, modelMatrix, m_simpleObjSunLight);
            }
        }

        for(auto& enemy : m_enemies)
        {
            if(enemy.getObj()->getIsEnabledDraw())
            {
                modelMatrix = enemy.getObj()->getModelMatrix();
                m_simpleObjSunLight->setMatrix4x4Float("modelMatrix", modelMatrix);
                m_simpleObjSunLight->setMatrix3x3Float("normalMatrix", glm::mat3(modelMatrix));
                Beryll::Renderer::drawObject(enemy.getObj(), modelMatrix, m_simpleObjSunLight);
            }
        }

        m_simpleObjSunLightShadows->bind();
        m_simpleObjSunLightShadows->set3Float("sunLightDir", m_sunLightDir);
        m_simpleObjSunLightShadows->set3Float("cameraPos", Beryll::Camera::getCameraPos());
        m_simpleObjSunLightShadows->set1Float("ambientLight", 0.2f);
        m_simpleObjSunLightShadows->set1Float("sunLightStrength", 1.0f);
        m_simpleObjSunLightShadows->set1Float("specularLightStrength", 0.3f);

        for(const auto& staticObj : m_staticEnv)
        {
            modelMatrix = staticObj->getModelMatrix();
            m_simpleObjSunLightShadows->setMatrix4x4Float("MVPLightMatrix", m_sunLightVPMatrix * modelMatrix);
            m_simpleObjSunLightShadows->setMatrix4x4Float("modelMatrix", modelMatrix);
            m_simpleObjSunLightShadows->setMatrix3x3Float("normalMatrix", glm::mat3(modelMatrix));
            Beryll::Renderer::drawObject(staticObj, modelMatrix, m_simpleObjSunLightShadows);
        }

        m_bulletTrajectory.calculateAndDraw(EnumsAndVars::bulletMass,
                                            EnumsAndVars::bulletGravity,
                                            m_bulletStartPosition,
                                            m_bulletAngleRadians,
                                            m_bulletImpulseVector,
                                            glm::vec3(1.0f),
                                            m_sunLightDir);
    }

    void PlayStateSceneLayer::loadPlayer()
    {
        m_player = std::make_shared<Player>("models3D/player/Player.fbx",
                                            EnumsAndVars::playerMass,
                                            true,
                                            Beryll::CollisionFlags::KINEMATIC,
                                            Beryll::CollisionGroups::PLAYER,
                                            Beryll::CollisionGroups::MOVABLE_ENEMY,
                                            Beryll::SceneObjectGroups::PLAYER);

        m_player->getController().moveSpeed = 25.0f;

        m_simpleObjForShadowMap.push_back(m_player);

        for(int i = 0; i < 10; ++i)
        {
            const auto bullet = std::make_shared<Beryll::SimpleCollidingObject>("models3D/player/PlayerBullet.fbx",
                                                                                EnumsAndVars::bulletMass,
                                                                                true,
                                                                                Beryll::CollisionFlags::DYNAMIC,
                                                                                Beryll::CollisionGroups::PLAYER_BULLET,
                                                                                Beryll::CollisionGroups::GROUND | Beryll::CollisionGroups::MOVABLE_ENEMY,
                                                                                Beryll::SceneObjectGroups::GROUND);

            bullet->disableUpdate();
            bullet->disableCollisionMesh();
            bullet->disableDraw();

            m_playerBullets.push_back(bullet);
            m_allDynamicObjects.push_back(bullet);
        }
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
        for(int i = 0; i < 10; ++i) // 10 * 10 = 100
        {
            const auto enemies = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/Enemies.fbx",
                                                                                          EnumsAndVars::enemyMass,
                                                                                          false,
                                                                                          Beryll::CollisionFlags::DYNAMIC,
                                                                                          Beryll::CollisionGroups::MOVABLE_ENEMY,
                                                                                          Beryll::CollisionGroups::PLAYER | Beryll::CollisionGroups::PLAYER_BULLET,
                                                                                          Beryll::SceneObjectGroups::ENEMY);

            for(auto& obj : enemies)
            {
                obj->setGravity(EnumsAndVars::enemyGravity);

                m_enemies.emplace_back(obj);
                m_allDynamicObjects.push_back(obj);
                m_simpleObjForShadowMap.push_back(obj);
                m_enemies.back().disable();
            }
        }

        spawnEnemy();
        spawnEnemy();
        spawnEnemy();
        spawnEnemy();
        spawnEnemy();
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

        m_shadowMap = Beryll::Renderer::createShadowMap(2000, 2000);

        m_dirToSun = glm::normalize(glm::vec3(0.05f, 2.0f, 0.0f));
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

                m_player->getController().moveToDirection(moveDir, false, true, true);
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

                m_angleXZ += deltaX * EnumsAndVars::Settings::cameraHorizontalSpeed;
                m_angleYZ -= deltaY * EnumsAndVars::Settings::cameraVerticalSpeed;
                if(m_angleYZ < -40.0f) m_angleYZ = -40.0f;
                if(m_angleYZ > 70.0f) m_angleYZ = 70.0f;
                //BR_INFO("m_angleYZ %f", m_angleYZ);

                // Euler angles.
                float m_angleXZRadians = glm::radians(m_angleXZ);
                float m_angleYZRadians = glm::radians(m_angleYZ);
                m_cameraOffset.x = glm::cos(m_angleXZRadians) * glm::cos(m_angleYZRadians);
                m_cameraOffset.y = glm::sin(m_angleYZRadians);
                m_cameraOffset.z = glm::sin(m_angleXZRadians) * glm::cos(m_angleYZRadians);
                break;
            }
        }

        m_cameraFront = m_player->getOrigin();
        m_cameraFront.y += 8.0f;
        Beryll::Camera::setCameraPos(m_cameraFront - m_cameraOffset * m_cameraDistance);
        Beryll::Camera::setCameraFrontPos(m_cameraFront);
        Beryll::Camera::updateCameraVectors();

        //if(m_3DSceneTouched && m_gui->playerJoystick->getIsTouched())
        m_player->rotateToDirection(Beryll::Camera::getCameraFrontDirectionXZ(), true);

        // Move camera a bit to left side(player's character will not directly on the middle of the screen).
        // That allow player see all trajectory including part going down.
        Beryll::Camera::setCameraPos(Beryll::Camera::getCameraPos() + Beryll::Camera::getCameraLeftXYZ() * 8.0f);
        Beryll::Camera::setCameraFrontPos(Beryll::Camera::getCameraFrontPos() + Beryll::Camera::getCameraLeftXYZ() * 7.0f);

        // Update shoot dir after camera.
        float angleBetweenWorldUpAndCameraBack = BeryllUtils::Common::getAngleInRadians(BeryllConstants::worldUp, Beryll::Camera::getCameraBackDirectionXYZ());
        m_bulletAngleRadians = angleBetweenWorldUpAndCameraBack - glm::half_pi<float>() + 0.1745f; // + 10 degrees.

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

    void PlayStateSceneLayer::handleEnemies()
    {
        for(Enemy& enemy : m_enemies)
        {
            if(!enemy.getIsEnabled())
                continue;

            glm::vec3 linearVeloc = enemy.getObj()->getLinearVelocity();
            glm::vec3 moveDir = glm::normalize(linearVeloc);
            glm::vec3 newMoveDir{0.0f};
            const float actualSpeed = glm::length(linearVeloc);
            glm::vec3 origin = enemy.getObj()->getOrigin();
            bool changeMoveDir = false;

            if(origin.x < m_enemyMinX)
            {
                changeMoveDir = true;
                origin.x = m_enemyMinX;
                newMoveDir = glm::reflect(moveDir, glm::vec3{1.0f, 0.0f, 0.0f});
            }
            else if(origin.x > m_enemyMaxX)
            {
                changeMoveDir = true;
                origin.x = m_enemyMaxX;
                newMoveDir = glm::reflect(moveDir, glm::vec3{-1.0f, 0.0f, 0.0f});
            }
            else if(origin.z < m_enemyMinZ)
            {
                changeMoveDir = true;
                origin.z = m_enemyMinZ;
                newMoveDir = glm::reflect(moveDir, glm::vec3{0.0f, 0.0f, 1.0f});
            }
            else if(origin.z > m_enemyMaxZ)
            {
                changeMoveDir = true;
                origin.z = m_enemyMaxZ;
                newMoveDir = glm::reflect(moveDir, glm::vec3{0.0f, 0.0f, -1.0f});
            }
            else if(origin.y < enemy.getObj()->getFromOriginToBottom())
            {
                origin.y = enemy.getObj()->getFromOriginToBottom();
                enemy.getObj()->setOrigin(origin, false);
                linearVeloc.y = 0;
                enemy.getObj()->setLinearVelocity(linearVeloc);
                glm::vec3 impulse = BeryllConstants::worldUp * (200.0f + 200.0f * Beryll::RandomGenerator::getFloat());
                enemy.getObj()->applyCentralImpulse(impulse);
            }

            if(changeMoveDir)
            {
                enemy.getObj()->setOrigin(origin, false);
                moveDir = glm::normalize(newMoveDir);

                if(glm::any(glm::isnan(moveDir)))
                {
                    BR_ASSERT(false, "%s", "if(glm::any(glm::isnan(moveDir)))");
                }

                enemy.getObj()->setLinearVelocity(moveDir * actualSpeed);
            }
        }
    }

    void PlayStateSceneLayer::handlePlayer()
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

    void PlayStateSceneLayer::spawnEnemy()
    {
        if(m_currentEnemyIndex >= m_enemies.size())
            m_currentEnemyIndex = 0;

        glm::vec3 newOrigin = m_player->getOrigin();
        newOrigin.y + 30.0f + Beryll::RandomGenerator::getFloat() * 30.0f;

        float xDir = Beryll::RandomGenerator::getFloat() * 2.0f - 1.0f;
        float zDir = Beryll::RandomGenerator::getFloat() * 2.0f - 1.0f;
        m_enemies[m_currentEnemyIndex].enable();
        m_enemies[m_currentEnemyIndex].getObj()->setOrigin(newOrigin);
        m_enemies[m_currentEnemyIndex].getObj()->applyCentralImpulse(glm::normalize(glm::vec3(xDir, 0.0f, zDir)) * 200.0f);

        ++m_currentEnemyIndex;
    }
}
