#define GLEW_STATIC

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include <imgui_impl_opengl3.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "tools/config.h"
#include "Square.h"
#include "ShapeManager.h"
#include "Disk.h"
#include <vector>
#include "Window.h"


ImGuiWindowFlags flags = 0;
GLFWwindow* window;
Disk* disk;
Window w{};

extern float color[3];
extern std::vector<std::string> options;

void initImGui (GLFWwindow* window);
void init ();
void drawImGuiWindow (ImGuiWindowFlags* flags);


int main() {
    init();
}

void init () {
    window = w.window;

    // init ImGui
    initImGui(window);

    Square s1{ 75, 75 };
    Square s2{ 75, 75 };
    Square s3{ 100, 100 };
    Square s4{ 75, 75 };

    disk = new Disk{ window, 400, 100, 200, 200 };
    disk->addShape(s1);
    disk->addShape(s2);
    disk->addShape(s3);
    disk->addShape(s4);
    disk->setTranslation(300, 300);

    w.managers.insert(w.managers.end(), disk);

    // update loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT);

        disk->draw();        
        drawImGuiWindow(&flags);


        glfwSwapBuffers(window);
    }

    // end the ImGui context
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
}

void initImGui (GLFWwindow* window) {
    // create the ImGui context and set some settings
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    flags |= ImGuiWindowFlags_NoMove;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void drawImGuiWindow (ImGuiWindowFlags* flags) {
    // create a new frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Options", nullptr, *flags);
    // create options table
    if (ImGui::BeginTable("table3", 5)) {
        for (int item = 0; item < config::options.size(); item++) {
            ImGui::TableNextColumn();
            ImGui::Button(config::options.at(item).c_str());
        }
        ImGui::EndTable();
    }
    ImGui::End();

    if (Window::drawColorPicker && Window::s != nullptr) {
        // create color picker
        ImGui::Begin("Color Picker");
        if (ImGui::MenuItem("Close")) {
            Window::drawColorPicker = false;
        }
        if (Window::s != nullptr) {
            ImGui::ColorPicker4("Shape Color", (float*)&Window::s->color, NULL);
        }
        ImGui::End();
    }
    

    // render and update
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
}


