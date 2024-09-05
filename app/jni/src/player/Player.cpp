#include "Player.h"

namespace BubbleShooter3D
{
    Player::Player(const char* filePath,  // Common params.
                   float collisionMassKg,    // Physics params.
                   bool wantCollisionCallBack,
                   Beryll::CollisionFlags collFlag,
                   Beryll::CollisionGroups collGroup,
                   Beryll::CollisionGroups collMask,
                   Beryll::SceneObjectGroups sceneGroup)
                   : Beryll::SimpleCollidingCharacter(filePath,
                                                      collisionMassKg,
                                                      wantCollisionCallBack,
                                                      collFlag,
                                                      collGroup,
                                                      collMask,
                                                      sceneGroup)
    {

    }

    Player::~Player()
    {

    }
}
