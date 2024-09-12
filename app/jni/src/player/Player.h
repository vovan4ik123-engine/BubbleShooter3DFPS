#pragma once

#include "EngineHeaders.h"

namespace BubbleShooter3D
{
    class Player : public Beryll::SimpleCollidingCharacter
    {
    public:
        Player(const char* filePath,  // Common params.
               float collisionMassKg,    // Physics params.
               bool wantCollisionCallBack,
               Beryll::CollisionFlags collFlag,
               Beryll::CollisionGroups collGroup,
               Beryll::CollisionGroups collMask,
               Beryll::SceneObjectGroups sceneGroup);
        ~Player() override;

        void update();

    private:

    };
}
