#include "EngineHeaders.h"

int main(int argc, char* argv[])
{
    BR_INFO("%s", "main() started.");

    Beryll::GameLoop::create(Beryll::ScreenOrientation::MOBILE_LANDSCAPE_AND_FLIPPED);

    //Beryll::GameLoop::setFPSLimit(15.0f);

    Beryll::Camera::setProjectionNearClipPlane(5.0f);
    Beryll::Camera::setProjectionFarClipPlane(1000.0f);
    Beryll::Camera::setObjectsViewDistance(1500.0f);

    //Beryll::Physics::setResolution(1);
    //Beryll::Physics::setMinAcceptableFPS(20.0f);

    Beryll::Renderer::enableFaceCulling();

    //GameStateHelper::pushPlayState();

    Beryll::GameLoop::run();

    BR_INFO("%s", "main() finished.");
    return 0;
}
