#include "BaseEnemy.h"

namespace BubbleShooter3D
{
    int BaseEnemy::m_activeEnemiesCount = 0;

    BaseEnemy::BaseEnemy(const char* filePath,
                         float collisionMassKg,
                         bool wantCollisionCallBack,
                         Beryll::CollisionFlags collFlag,
                         Beryll::CollisionGroups collGroup,
                         Beryll::CollisionGroups collMask,
                         Beryll::SceneObjectGroups sceneGroup)
                         : AnimatedCollidingCharacter(filePath,
                                                      collisionMassKg,
                                                      wantCollisionCallBack,
                                                      collFlag,
                                                      collGroup,
                                                      collMask,
                                                      sceneGroup)
    {
        disableEnemy();
    }

    BaseEnemy::~BaseEnemy()
    {
        //BR_INFO("%s", "BaseEnemy::~BaseEnemy()");
        disableEnemy();
    }

    void BaseEnemy::enableEnemy()
    {
        enableDraw();
        enableUpdate();
        enableCollisionMesh();

        ++BaseEnemy::m_activeEnemiesCount;
        m_isEnabled = true;
    }

    void BaseEnemy::disableEnemy()
    {
        disableDraw();
        disableUpdate();
        disableCollisionMesh();

        if(BaseEnemy::m_activeEnemiesCount > 0)
            --BaseEnemy::m_activeEnemiesCount;

        m_isEnabled = false;
        m_lastAttackTime = -9999.0f;
        m_prepareToFirstAttackStartTime = -9999.0f;
        m_prepareToFirstAttack = true;
    }

    void BaseEnemy::attack(const glm::vec3& playerOrigin)
    {
        //BR_INFO("%s", "BaseEnemy::attack()");
        rotateToPoint(playerOrigin, true);
        setCurrentAnimationByIndex(EnumsAndVars::AnimationIndexes::attack, true, true);
        m_lastAttackTime = EnumsAndVars::playTimeSec;
        unitState = UnitState::ATTACKING;

        Sounds::playSoundEffect(attackSound);
        Sounds::playSoundEffect(attackHitSound);
    }
}
