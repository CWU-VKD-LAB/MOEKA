#define GLEW_STATIC

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include <imgui_impl_opengl3.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
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