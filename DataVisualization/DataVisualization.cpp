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
#include "Model.h"
#include "Window.h"
#include "Render/Texture.h"


ImGuiWindowFlags flags = 0;
GLFWwindow* window;
Disk* disk;
Window w{};
Texture texture{ "../resources/icons.png" };

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
    disk->setScale(.5);
    disk->setTranslation(500, 500);
    disk->setScale(.1);
    
    Window::addToRender(disk);

    std::vector<int> test{1, 2, 3, 4, 5};
    Model m{ window, 300, 300 };
    m.createDisk(test);
    m.createDisk(test);

    texture.bind();

    // update loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT);


        m.draw();
        //disk->draw();
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

    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
}

void drawImGuiWindow (ImGuiWindowFlags* flags) {
    // create a new frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Options", nullptr, *flags);
    // create options table
    texture.bind();
    int numOfElements = std::min((int)round(config::windowX / 32.0f), (int)config::options.size());
    if (ImGui::BeginTable("table3", numOfElements)) {
        int x, y;
        for (int item = 0; item < config::options.size(); item++) {
            x = item%5;
            y = item/5;
            ImGui::TableNextColumn();

            ImVec2 size = ImVec2(config::buttonSize, config::buttonSize);                                                     // Size of the image we want to make visible
            
            // UV coordinates for lower-left
            ImVec2 uv0 = ImVec2(
                (config::buttonSize * x / texture.getWidth()),
                (config::buttonSize * y / texture.getHeight())
            );                    

            // UV coordinates for (32,32) in our texture
            ImVec2 uv1 = ImVec2(
                (config::buttonSize * (x + 1) / texture.getWidth()),
                (config::buttonSize * (y + 1) / texture.getHeight())
            );

            //config::options.at(item).c_str()
            if (ImGui::ImageButton("", (void*)texture.id, size, uv0, uv1)) {
                
            }
            if (ImGui::IsItemHovered()) {
                ImGui::BeginTooltip();
                ImGui::Text(config::options.at(item).c_str());
                ImGui::EndTooltip();
            }
            
        }
        ImGui::EndTable();
    }
    ImGui::End();

    if (Window::drawColorPicker && Window::focus != nullptr) {
        // create color picker
        ImGui::Begin("Color Picker", &Window::drawColorPicker);
        //if (ImGui::MenuItem("Close")) {
        //    Window::drawColorPicker = false;
        //    Window::focus = nullptr;
        //}
        if (Window::focus != nullptr) {
            ImGui::ColorPicker4("Shape Color", (float*)&Window::focus->color, NULL);
        }
        ImGui::End();
    }

    if (Window::s != nullptr) {
        ImGui::BeginTooltip();
        ImGui::Text(std::string("K Value: ").append(std::to_string(Window::s->getKValue())).c_str());
        ImGui::EndTooltip();
    }
    
    // render and update
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
}


