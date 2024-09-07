#include "Enemy.h"

namespace BubbleShooter3D
{
    Enemy::Enemy(std::shared_ptr<Beryll::SimpleCollidingObject> obj)
    {
        m_obj = std::move(obj);
        m_ID = m_obj->getID();
    }

    Enemy::~Enemy()
    {

    }

    void Enemy::enable()
    {
        if(!m_isEnabled)
        {
            m_isEnabled = true;

            m_obj->enableUpdate();
            m_obj->enableCollisionMesh();
            m_obj->enableDraw();
        }
    }

    void Enemy::disable()
    {
        if(m_isEnabled)
        {
            m_isEnabled = false;

            m_obj->disableUpdate();
            m_obj->disableCollisionMesh();
            m_obj->disableDraw();
        }
    }
}
