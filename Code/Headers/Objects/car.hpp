#pragma once

// Local Headers
#include "globals.hpp"
#include "Utils/gameobject.hpp"
#include "Utils/model.hpp"

// System Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

// Standard Headers
#include <vector>
#include <memory>
#include <iomanip>

const double SCALE_FACTOR = 1.0/400.0;
const double ACCELERATION = 15000;
const double WHEEL_TURN_RATE = 100;

// Define Namespace
namespace Objects
{
    class Car : public Utils::GameObject
    {
    public:

        // Implement Default Constructor and Destructor
        Car();
        ~Car();

        // Public Member Functions
        static void setup();
        void calculateRotation(double angle);
        void updateLighting();
        void processInput(GLFWwindow *window, double deltaTime) override;
        void perFrame(double deltaTime) override;

    private:

        // Disable Copying and Assignment
        Car(Car const &) = delete;
        Car & operator=(Car const &) = delete;

        // Private members
        float mWheelTurn;
        static glm::vec3 mSpotlightOffset1;
        static glm::vec3 mSpotlightOffset2;
        static std::shared_ptr<Utils::Drawable> mDrawable;
        static std::shared_ptr<Utils::Shader> mShader;
    };
}
