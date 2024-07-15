#include "Window.h"

std::vector<Drawable*> Window::managers{};
Shape* Window::s = nullptr;
Shape* Window::focus = nullptr;

bool Window::drawColorPicker = false;
void cursorCallback(GLFWwindow* window, int button, int action, int mods);
void keyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods);
void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);

Window::Window () {
    // init glfw
    initGLFW();

    // create window
    window = glfwCreateWindow(config::windowX, config::windowY, "Data Visualization", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        exit(1);
    }
    glfwMakeContextCurrent(window);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //

    // init glew
    initGLEW();

    glfwSetMouseButtonCallback(window, cursorCallback);
    glfwSetKeyCallback(window, keyCallBack);
    glfwSetCursorPosCallback(window, cursorPositionCallback);

}
// ensure that GLEW was successful
void Window::initGLEW () {
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        std::cout << glewGetErrorString(err) << std::endl;
    }
    else {
        std::cout << "GLEW OK!" << std::endl;
    }
}

// ensure that GLFW was successful
void Window::initGLFW () {
    if (!glfwInit()) {
        exit(1);
    }
    else {
        std::cout << "GLFW OK!" << std::endl;
    }
}

void cursorCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_2 && action == GLFW_PRESS) {
        Window::focus = Window::s;
        Window::drawColorPicker = true;
    }
}

void keyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // cleanup later but fine in short term.
    if (key == GLFW_KEY_UP && (action == GLFW_PRESS || glfwGetKey(window, key))) {
        for (auto a : Window::managers) {
            a->setY(a->getY()-5);
            a->setTranslation(a->getX(), a->getY());
        }
    }
    if (key == GLFW_KEY_DOWN && (action == GLFW_PRESS || glfwGetKey(window, key))) {
        for (auto a : Window::managers) {
            a->setY(a->getY() + 5);
            a->setTranslation(a->getX(), a->getY());
        }
    }
    if (key == GLFW_KEY_LEFT && (action == GLFW_PRESS || glfwGetKey(window, key))) {
        for (auto a : Window::managers) {
            a->setX(a->getX() - 5);
            a->setTranslation(a->getX(), a->getY());
        }
    }
    if (key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || glfwGetKey(window, key))) {
        for (auto a : Window::managers) {
            a->setX(a->getX() + 5);
            a->setTranslation(a->getX(), a->getY());
        }
    }
    if (key == GLFW_KEY_KP_ADD && (action == GLFW_PRESS || glfwGetKey(window, key))) {
        for (auto a : Window::managers) {
            a->setScale(a->totalScale + .01);
        }
    }
    if (key == GLFW_KEY_KP_SUBTRACT && (action == GLFW_PRESS || glfwGetKey(window, key))) {
        for (auto a : Window::managers) {
            if ((a->totalScale - .01) > 0.00001) {
                a->setScale(a->totalScale - .01);
            }
            
        }
    }
}

void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos) {
    Window::s = nullptr;
    for (auto a : Window::managers) {
        Window::s = a->selectedShape();
        if (Window::s != nullptr) {
            break;
        }
    }
}
