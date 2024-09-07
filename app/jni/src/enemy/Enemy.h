#pragma once

#include "EngineHeaders.h"

namespace BubbleShooter3D
{
    class Enemy
    {
    public:
        Enemy(std::shared_ptr<Beryll::SimpleCollidingObject> obj);
        ~Enemy();

        bool getIsEnabled() { return m_isEnabled; }
        const std::shared_ptr<Beryll::SimpleCollidingObject> getObj() { return m_obj; }
        int getID() { return m_ID; }

        void enable();
        void disable();

    private:
        bool m_isEnabled = true;
        std::shared_ptr<Beryll::SimpleCollidingObject> m_obj;
        int m_ID = 0;
    };
}
