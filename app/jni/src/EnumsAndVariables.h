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
    };

    struct Settings
    {
        static inline float cameraHorizontalSpeed = 0.25f;
        static inline float cameraVerticalSpeed = 0.2f;
    };

    // Player.
    constexpr inline float playerMass = 1.0f;
    constexpr inline glm::vec3 playerGravity{0.0f, -70.0f, 0.0f};

    // Player bullet.
    constexpr inline float bulletMass = 0.001f;
    constexpr inline glm::vec3 bulletGravity{0.0f, -10.0f, 0.0f};
    inline float shotTimeSec = -9999.0f;
    constexpr inline float shotDelaySec = 0.04f;

    // Enemies.
    constexpr inline int enemiesCurrentPathfindingIndexDefault = 0;
    inline int enemiesCurrentPathfindingIndex = enemiesCurrentPathfindingIndexDefault;
    constexpr inline int enemiesMaxPathfindingInOneFrame = 10;
    constexpr inline float enemiesMinDistanceToSpawn = 200.0f;
    constexpr inline float enemiesMaxDistanceToSpawn = 500.0f;
    constexpr inline float enemiesDisableDistance = 501.0f;

    // Play time.
    inline float playTimeSec = 0.0f;

    inline void reset()
    {
        // Player bullet.
        shotTimeSec = -9999.0;

        // Play time.
        playTimeSec = 0.0f;

        // Enemies.
        enemiesCurrentPathfindingIndex = enemiesCurrentPathfindingIndexDefault;
    }
}
