#include "MovableEnemy.h"

namespace BubbleShooter3D
{
    MovableEnemy::MovableEnemy(const char* filePath,
                               float collisionMassKg,
                               bool wantCollisionCallBack,
                               Beryll::CollisionFlags collFlag,
                               Beryll::CollisionGroups collGroup,
                               Beryll::CollisionGroups collMask,
                               Beryll::SceneObjectGroups sceneGroup,
                               float HP)
                                 : BaseEnemy(filePath,
                                             collisionMassKg,
                                             wantCollisionCallBack,
                                             collFlag,
                                             collGroup,
                                             collMask,
                                             sceneGroup,
                                             HP)
    {
        unitState = UnitState::MOVE;
    }

    MovableEnemy::~MovableEnemy()
    {

    }

    void MovableEnemy::update(const glm::vec3& playerOrigin)
    {
        if(unitState == UnitState::DYING)
        {
            if(m_obj->getIsOneTimeAnimationFinished())
                disableEnemy();

            return;
        }
        else if(m_currentHP <= 0.0f)
        {
            ++EnumsAndVars::enemiesKilledCount;
            m_obj->setCurrentAnimationByIndex(3 + Beryll::RandomGenerator::getInt(2), true, false);
            unitState = UnitState::DYING;
            m_obj->disableCollisionMesh();
            m_timeDie = Beryll::TimeStep::getSecFromStart();
            return;
        }

        if(getIsAttacking())
        {
            //BR_INFO("%s", "MovableEnemy is attacking");
            // Do nothing. Attack animation should be playing now.
        }
        else if(getIsDelayBeforeFirstAttack())
        {
            //BR_INFO("%s", "MovableEnemy DelayBeforeFirstAttack");
            unitState = UnitState::STAND_AIMING;
            m_obj->setCurrentAnimationByIndex(EnumsAndVars::AnimationIndexes::stand, false, false);
            m_obj->rotateToPoint(playerOrigin, true);
        }
        else if(glm::distance(m_obj->getOrigin(), playerOrigin) > attackDistance)
        {
            //BR_INFO("%s", "MovableEnemy move because distance");
            move();
        }
        else
        {
            //BR_INFO("%s", "MovableEnemy IN_ATTACK_RADIUS");
            unitState = UnitState::IN_ATTACK_RADIUS;

            if(getIsTimeToAttack())
            {
                //BR_INFO("%s", "MovableEnemy if(getIsTimeToAttack())");
                // Check if enemy see player.
                Beryll::RayClosestHit rayEnv = Beryll::Physics::castRayClosestHit(m_obj->getOrigin(),
                                                                                  playerOrigin,
                                                                                  Beryll::CollisionGroups::RAY_FOR_ENVIRONMENT,
                                                                                  Beryll::CollisionGroups::STATIC_ENVIRONMENT);

                if(rayEnv)
                {
                    //BR_INFO("%s", "MovableEnemy move because dont see");
                    move();
                }
                else
                {
                    if(m_prepareToFirstAttack)
                    {
                        //BR_INFO("%s", "MovableEnemy prepareToFirstAttack");
                        m_prepareToFirstAttack = false;
                        m_prepareToFirstAttackStartTime = EnumsAndVars::playTimeSec;
                    }
                    else
                    {
                        //BR_INFO("%s", "MovableEnemy CAN_ATTACK");
                        unitState = UnitState::CAN_ATTACK;
                    }
                }
            }
        }
    }

    void MovableEnemy::move()
    {
        unitState = UnitState::MOVE;

        m_prepareToFirstAttack = true;

        m_obj->setCurrentAnimationByIndex(EnumsAndVars::AnimationIndexes::run, false, false, true);

        m_obj->getController().moveToPosition(m_currentPointToMove3DFloats, true, true);

        if(!m_obj->getController().getIsMoving())
        {
            //BR_INFO("%s", "MovableEnemy NOT   moving");

            // Go to next point if exists.
            if(m_pathArrayIndexToMove + 1 < m_pathArray.size())
            {
                ++m_pathArrayIndexToMove;

                m_currentPointToMove2DIntegers = m_pathArray[m_pathArrayIndexToMove];
                float currentY = m_currentPointToMove3DFloats.y;
                m_currentPointToMove3DFloats = glm::vec3(m_currentPointToMove2DIntegers.x,
                                                         m_obj->getFromOriginToBottom(),
                                                         m_currentPointToMove2DIntegers.y);

                if(castRayToFindYPos)
                {
                    glm::vec3 rayFrom = m_currentPointToMove3DFloats;
                    rayFrom.y = currentY + 100.0f;
                    glm::vec3 rayTo = m_currentPointToMove3DFloats;
                    rayTo.y = currentY - 100.0f;
                    Beryll::RayClosestHit rayHit = Beryll::Physics::castRayClosestHit(rayFrom,
                                                                                      rayTo,
                                                                                      Beryll::CollisionGroups::RAY_FOR_ENVIRONMENT,
                                                                                      Beryll::CollisionGroups::STATIC_ENVIRONMENT);

                    if(rayHit)
                    {
                        m_currentPointToMove3DFloats.y = rayHit.hitPoint.y + m_obj->getFromOriginToBottom();
                    }
                }
            }
        }
    }

    void MovableEnemy::setPathArray(std::vector<glm::ivec2> pathArray, const int indexToMove)
    {
        if(pathArray.empty() || indexToMove < 0)
        {
            BR_ASSERT(false, "%s", "pathArray.empty() or pathArrayIndexToMove < 0");
        }

        m_pathArray = std::move(pathArray);

        if(indexToMove >= m_pathArray.size())
        {
            m_pathArrayIndexToMove = m_pathArray.size() - 1;
        }
        else
        {
            m_pathArrayIndexToMove = indexToMove;
        }

        m_currentPointToMove2DIntegers = m_pathArray[m_pathArrayIndexToMove];
        m_currentPointToMove3DFloats = glm::vec3(m_currentPointToMove2DIntegers.x,
                                                 m_obj->getFromOriginToBottom(),
                                                 m_currentPointToMove2DIntegers.y);
        m_startPointMoveFrom = glm::vec3(m_pathArray[0].x,
                                         m_obj->getFromOriginToBottom(),
                                         m_pathArray[0].y);

        if(castRayToFindYPos)
        {
            glm::vec3 rayFrom = m_currentPointToMove3DFloats;
            rayFrom.y = 200.0f;
            glm::vec3 rayTo = m_currentPointToMove3DFloats;
            rayTo.y = -20.0f;
            Beryll::RayClosestHit rayHit = Beryll::Physics::castRayClosestHit(rayFrom,
                                                                              rayTo,
                                                                              Beryll::CollisionGroups::RAY_FOR_ENVIRONMENT,
                                                                              Beryll::CollisionGroups::STATIC_ENVIRONMENT);

            if(rayHit)
            {
                m_currentPointToMove3DFloats.y = rayHit.hitPoint.y + m_obj->getFromOriginToBottom();
                m_startPointMoveFrom.y = rayHit.hitPoint.y + m_obj->getFromOriginToBottom();
            }

            if(m_pathArrayIndexToMove > 0)
            {
                // m_startPointMoveFrom != m_currentPointToMove3DFloats. Recalculate Y.
                glm::vec3 rayFrom2 = m_startPointMoveFrom;
                rayFrom2.y = 200.0f;
                glm::vec3 rayTo2 = m_startPointMoveFrom;
                rayTo2.y = -20.0f;
                Beryll::RayClosestHit rayHit2 = Beryll::Physics::castRayClosestHit(rayFrom2,
                                                                                   rayTo2,
                                                                                   Beryll::CollisionGroups::RAY_FOR_ENVIRONMENT,
                                                                                   Beryll::CollisionGroups::STATIC_ENVIRONMENT);

                if(rayHit2)
                {
                    m_startPointMoveFrom.y = rayHit2.hitPoint.y + m_obj->getFromOriginToBottom();
                }
            }
        }
    }
}
