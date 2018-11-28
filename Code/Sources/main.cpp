// Local Headers
#include "globals.hpp"
#include "Utils/camera.hpp"
#include "Utils/textrenderer.hpp"
#include "Utils/scene.hpp"
#include "Utils/gameobject.hpp"
#include "Utils/model.hpp"
#include "Utils/shader.hpp"
#include "Objects/car.hpp"
#include "Objects/house.hpp"

// System Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Standard Headers
#include <cstdio>
#include <cstdlib>
#include <iomanip>
#include <memory>
#include <iostream>
#include <sstream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


const int FPS_ROLLING_FRAMES = 10;
const GLfloat EDGE_THRESHOLD = 75;
const double EDGE_ROTATE_SPEED = 10;

// Below used by other files
GLfloat width = 1280;
GLfloat height = 800;
std::shared_ptr<Utils::Shader> defaultShader;
std::unique_ptr<Utils::TextRenderer> textRenderer;

// Below local to translation unit
Utils::Scene scene;
bool firstMouse = true;
float lastX;
float lastY;


void errorCallback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
        return;
    }

    float xoffset = xpos - lastX;
    // Reversed since y-coordinates range from bottom to top
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

#ifdef CAMERA_MOUSE_MOVEMENT
    scene.getCamera()->processMouseMovement(xoffset, yoffset);
#endif
}

void framebufferSizeCallback(GLFWwindow* window, int newWidth, int newHeight) {
    width = newWidth;
    height = newHeight;
    glViewport(0, 0, width, height);
}

int main(int argc, char * argv[]) {
    // Load GLFW
    glfwInit();

    // Register error callback
    glfwSetErrorCallback(errorCallback);

    // Create a window
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    auto window = glfwCreateWindow(width, height, "Driving Scene - Jeffrey Everett", nullptr, nullptr);

    // Check for Valid Context
    if (window == nullptr) {
        fprintf(stderr, "Failed to Create OpenGL Context");
        return EXIT_FAILURE;
    }

    // Create Context and Load OpenGL Functions
    glfwMakeContextCurrent(window);
    gladLoadGL();
    fprintf(stderr, "OpenGL %s\n", glGetString(GL_VERSION));

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Enable/disable features
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);

    // OpenGL settings
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Initialize remaining globals
    textRenderer = std::make_unique<Utils::TextRenderer>(PROJECT_SOURCE_DIR "/Fonts/arial.ttf");
    defaultShader = std::make_shared<Utils::Shader>(
            PROJECT_SOURCE_DIR "/Shaders/VertexShaders/default.vert",
            PROJECT_SOURCE_DIR "/Shaders/FragmentShaders/default.frag"
    );

    // Create camera
    auto camera = std::make_shared<Utils::Camera>(glm::vec3(0,0,3));

    // Create cubemap
    std::vector<std::string> darkFaces {
            PROJECT_SOURCE_DIR "/Textures/CubeMaps/DarkStormy/DarkStormyLeft2048.png",
            PROJECT_SOURCE_DIR "/Textures/CubeMaps/DarkStormy/DarkStormyRight2048.png",
            PROJECT_SOURCE_DIR "/Textures/CubeMaps/DarkStormy/DarkStormyUp2048.png",
            PROJECT_SOURCE_DIR "/Textures/CubeMaps/DarkStormy/DarkStormyDown2048.png",
            PROJECT_SOURCE_DIR "/Textures/CubeMaps/DarkStormy/DarkStormyFront2048.png",
            PROJECT_SOURCE_DIR "/Textures/CubeMaps/DarkStormy/DarkStormyBack2048.png"
    };
    auto cubeMap = std::make_shared<Utils::CubeMap>(darkFaces);

    // Create scene
    scene.setCamera(camera);
    scene.setCubeMap(cubeMap);

    // Set up objects
    Objects::House::setup();
    Objects::Car::setup();

    // Add GameObjects to scene
    //scene.add(carObject);
    auto house = std::make_shared<Objects::House>();
    scene.add(house);
    auto car = std::make_shared<Objects::Car>();
    scene.getCamera()->setFollow(car);
    scene.add(car);

    // Register remaining callbacks
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    double deltaTime;
    double lastFrame = glfwGetTime();
    double rollingFPS;
    double latestFrameDeltas[FPS_ROLLING_FRAMES];
    int latestFrameIdx = 0;
    bool firstPassFrames = true;

    // Rendering Loop
    while (glfwWindowShouldClose(window) == 0) {
        // Background Fill Color
        glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Start text renderer at bottom
        textRenderer->resetVerticalOffset();

        // If at edge of screen, rotate in direction of edge
#ifdef CAMERA_MOUSE_MOVEMENT
        double xpos, ypos;
        double xoffset = 0, yoffset = 0;
        glfwGetCursorPos(window, &xpos, &ypos);
        if (xpos < EDGE_THRESHOLD) {
            xoffset = -EDGE_ROTATE_SPEED;
        }
        else if (xpos > width - EDGE_THRESHOLD) {
            xoffset = EDGE_ROTATE_SPEED;
        }

        if (ypos < EDGE_THRESHOLD) {
            yoffset = EDGE_ROTATE_SPEED;
        }
        else if (ypos > height - EDGE_THRESHOLD) {
            yoffset = -EDGE_ROTATE_SPEED;
        }

        scene.getCamera()->processMouseMovement(xoffset, yoffset);
#endif

        // FPS timing/display
        double currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        latestFrameDeltas[latestFrameIdx] = deltaTime;
        if (firstPassFrames) {
            float deltas = 0;
            for (int i = 0; i <= latestFrameIdx; i++) {
                deltas += latestFrameDeltas[i];
            }
            rollingFPS = (latestFrameIdx+1)/deltas;
        }
        else {
            float deltas = 0;
            for (int i = 0; i <= FPS_ROLLING_FRAMES; i++) {
                deltas += latestFrameDeltas[i];
            }
            rollingFPS = FPS_ROLLING_FRAMES/deltas;
        }
        latestFrameIdx = ++latestFrameIdx % FPS_ROLLING_FRAMES;
        if (latestFrameIdx == 0) {
            firstPassFrames = false;
        }
        scene.setDeltaTime(deltaTime);

#ifdef DEBUG
        std::cout << "Delta time: " << deltaTime;
#endif

        // Draw scene
        scene.draw();

        // Render FPS as string
        std::ostringstream fpsOSS;
        fpsOSS << std::fixed << std::setprecision(5) << "FPS: " << rollingFPS;
        textRenderer->renderText(fpsOSS.str(), 1, glm::vec3(0.5,0.5,0.5));

        // Process input
        scene.processInput(window);

        // Run object scripts
        scene.perFrame();

        // Flip Buffers and Draw
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return EXIT_SUCCESS;
}
