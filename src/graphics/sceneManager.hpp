#pragma once

#include <threepp/threepp.hpp>
#include <memory>

class SceneManager {
  public:
    SceneManager();

    // Scene access
    threepp::Scene& getScene();
    threepp::Camera& getCamera();
    threepp::GLRenderer& getRenderer();

    // Setup methods
    void setupLighting();
    void setupGround();
    void setupCamera(float aspectRatio);

    // Rendering
    void render();
    void resize(const threepp::WindowSize& size);

  private:
    std::unique_ptr<threepp::GLRenderer> renderer_;
    std::shared_ptr<threepp::Scene> scene_;
    std::shared_ptr<threepp::PerspectiveCamera> camera_;
    std::shared_ptr<threepp::Mesh> groundMesh_;
};
