#define GLEW_STATIC

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include <imgui_impl_opengl3.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "DecisionTable.h"
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

    DecisionTable<int> dt{};
    dt.readData("test.csv");
    std::vector<int> test {NULL, NULL, 3};
    std::vector<int> result = dt.getDecision(test);

    std::cout << "Checking" << std::endl;
    for (auto a : result) {
        std::cout << a << std::endl;
    }

    /*std::vector<int> test {
        1, 2, 3, 4, 5, 6
    };
    std::vector<int> test2 {
        1, 2, 3, 4, 5
    };
    std::vector<int> test3 {
        1, 2, 3, 4
    };*/
   

    /*Model m{};
    m.addColumn(&test);
    m.addColumn(&test2);
    m.addColumn(&test2);
    m.addColumn(&test3);
    m.addColumn(&test3);
    m.fitToScreen();
    w.addToRender(&m);
    */

    // update loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT);

        if (w.form.addModel)
        {
            w.addModelFromForm();
        }

        w.draw();

        w.drawImGuiWindow(texture);
        glfwSwapBuffers(window);
    }
    w.endImGui();
}


