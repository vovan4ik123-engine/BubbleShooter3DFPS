#pragma once

namespace EnumsAndVars
{
    struct FontsPath
    {
        static const inline std::string roboto = "fonts/roboto.ttf";
        static const inline std::string cousine = "fonts/cousine.ttf";
    };

    struct AnimationIndexes
    {
        static constexpr inline int run = 0;    // 1_Run    - names in blender.
        static constexpr inline int stand = 1;  // 2_Stand
        static constexpr inline int attack = 2; // 3_Attack
        static constexpr inline int die1 = 3; // 4_Die1
        static constexpr inline int die2 = 4; // 5_Die2
    };

    struct Settings
    {
        static inline float cameraHorizontalSpeed = 480.0f * 0.9f;
        static inline float cameraVerticalSpeed = 173.0f * 0.9f;
        static inline float cameraSpeedThresholdToAccelerate = 50.0f;
    };

    // Player.
    constexpr inline float playerMass = 1.0f;
    constexpr inline glm::vec3 playerGravity{0.0f, -70.0f, 0.0f};
    inline int playerTotalDamage = 0;

    // Player bullet.
    constexpr inline float bulletMass = 0.001f;
    constexpr inline glm::vec3 bulletGravity{0.0f, -10.0f, 0.0f};
    inline float shotTimeSec = -9999.0f;
    constexpr inline float shotDelaySec = 0.04f;

    // Enemies.
    constexpr inline int enemiesMaxPathfindingInOneFrame = 10;
    constexpr inline float enemiesMinDistanceToSpawn = 200.0f;
    constexpr inline float enemiesMaxDistanceToSpawn = 500.0f;
    constexpr inline float enemiesDisableDistance = 501.0f;
    inline int enemiesCurrentPathfindingIndex = 0;
    inline int enemiesKilledCount = 0;

    // Play time.
    inline float playTimeSec = 0.0f;

    inline void reset()
    {
        // Player.
        playerTotalDamage = 0;

        // Player bullet.
        shotTimeSec = -9999.0;

        // Enemies.
        enemiesCurrentPathfindingIndex = 0;
        enemiesKilledCount = 0;

        // Play time.
        playTimeSec = 0.0f;
    }
}
