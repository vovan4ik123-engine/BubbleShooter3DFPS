#pragma once

namespace EnumsAndVars
{
    struct FontsPath
    {
        static const inline std::string roboto = "fonts/roboto.ttf";
        static const inline std::string cousine = "fonts/cousine.ttf";
    };

    struct Settings
    {
        static inline float cameraHorizontalSpeed = 0.25f;
        static inline float cameraVerticalSpeed = 0.2f;
    };

    // Player
    constexpr inline float playerMass = 0.0f;

    // Player bullet
    constexpr inline float bulletMass = 0.001f;
    constexpr inline glm::vec3 bulletGravity{0.0f, -10.0f, 0.0f};
    inline float shotTimeSec = -9999.0;
    inline float shotDelaySec = 0.01;

    // Enemy
    constexpr inline float enemyMass = 10.0f;
    constexpr inline glm::vec3 enemyGravity{0.0f, -40.0f, 0.0f};

    // Play time.
    inline float playTimeSec = 0.0f;

    inline void reset()
    {
        shotTimeSec = -9999.0;
        shotDelaySec = 0.2;

        playTimeSec = 0.0f;
    }
}
