#define GLEW_STATIC

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include <imgui_impl_opengl3.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Window.h"
#include "windows.h"
#include "Render/Texture.h"


bool console = false;

GLFWwindow* window;

void init ();

int main() 
{
    if (console)
    {
        // moeka from console
        moeka o('x');
        o.init();
        o.start();

        std::fstream results;
        results.open("housingResults.csv", std::ios::out | std::ios::app);
        o.printToFile(results); // start the parent
    }
    else
    {
        init();
    }
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
            w.addModelFromFunctionForm();
        }

        if (w.form.getCompare().comparisons)
        {
            w.addModelFromCompareForm();
        }

        w.draw();

        w.drawImGuiWindow(texture);
        glfwSwapBuffers(window);
    }
    w.endImGui();
}























