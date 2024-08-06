#define GLEW_STATIC

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include <imgui_impl_opengl3.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include "Shape.h"
#include "Section.h"
#include "Model.h"
#include "Window.h"
#include "Render/Texture.h"


GLFWwindow* window;

void init ();

int main() {
    init();
}

void init () {
    Window w{};
    Texture texture{ "../resources/icons.png" };
    window = w.window;

    std::vector<int> test {
        1, 2, 3, 4, 5, 6
    };
    std::vector<int> test2 {
        1, 2, 3, 4, 5
    };
    std::vector<int> test3 {
        1, 2, 3, 4
    };
   

    Model m{};
    m.addColumn(&test);
    m.addColumn(&test2);
    m.addColumn(&test2);
    m.addColumn(&test3);
    m.addColumn(&test3);
    m.fitToScreen();
    

    w.addToRender(&m);

    // update loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT);

        // draw models/any additional things
        m.draw();


        w.drawImGuiWindow(texture);
        glfwSwapBuffers(window);
    }
    w.endImGui();
}


