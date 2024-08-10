#include "Window.h"


std::vector<Drawable*> Window::managedList{};
Drawable* Window::s = nullptr;
Drawable* Window::focus = nullptr;
Form Window::form{};

bool Window::drawColorPicker = false;
void cursorCallback(GLFWwindow* window, int button, int action, int mods);
void keyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods);
void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
void windowResizeCallback (GLFWwindow* window, int width, int height);

Window::Window () {
    std::cout << "Creating window..." << std::endl;
    // init glfw
    initGLFW();

    // create window
    window = glfwCreateWindow((int)config::windowX, (int)config::windowY, "Data Visualization", nullptr, nullptr);
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
    glfwSetWindowSizeCallback(window, windowResizeCallback);
    
    initImGui();
    //form.readCSV("output.csv");
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

void Window::initImGui () {
    // create the ImGui context and set some settings
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    flags |= ImGuiWindowFlags_NoMove;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    font = io.Fonts->AddFontFromFileTTF("resources/fonts/ProggyClean.ttf", 13.0f);
    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
}

void Window::endImGui () {
    // end the ImGui context
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
}

void Window::drawImGuiWindow(Texture& texture) {
    // create a new frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::PushFont(font);
    createTable(texture);
    if (!Window::form.open) {
        createColorPicker();
        createTooltip();
    }
    else {
        form.draw();
    }
    ImGui::PopFont();

    // render and update
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
}

// function that is called when the mouse is clicked
void cursorCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_2 && action == GLFW_PRESS) {
        Window::focus = Window::s;
        Window::drawColorPicker = true;
    }
}

// function that is called when a key is pressed
void keyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // cleanup later but fine in short term.
    if (key == GLFW_KEY_UP && (action == GLFW_PRESS || glfwGetKey(window, key))) {
        for (auto a : Window::managedList) {
            a->setTranslation(a->getX(), a->getY()-5);
        }
    }
    if (key == GLFW_KEY_DOWN && (action == GLFW_PRESS || glfwGetKey(window, key))) {
        for (auto a : Window::managedList) {
            a->setTranslation(a->getX(), a->getY()+5);
        }
    }
    if (key == GLFW_KEY_LEFT && (action == GLFW_PRESS || glfwGetKey(window, key))) {
        for (auto a : Window::managedList) {
            a->setTranslation(a->getX()-5, a->getY());
        }
    }
    if (key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || glfwGetKey(window, key))) {
        for (auto a : Window::managedList) {
            a->setTranslation(a->getX()+5, a->getY());
        }
    }
    if (key == GLFW_KEY_KP_ADD && (action == GLFW_PRESS || glfwGetKey(window, key))) {
        for (auto a : Window::managedList) {
            a->setScale((float)(a->getScale() + .01));
        }
    }
    if (key == GLFW_KEY_KP_SUBTRACT && (action == GLFW_PRESS || glfwGetKey(window, key))) {
        for (auto a : Window::managedList) {
            if ((a->getScale() - .01) > 0.00001) {
                a->setScale((float)(a->getScale() - .01));
            }
        }
    }
    if (key == GLFW_KEY_ESCAPE && (action == GLFW_PRESS || glfwGetKey(window, key))) {
        Window::form.open = !Window::form.open;
    }
}

// function that is called when the cursor moves
void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos) {
    Window::s = nullptr;
    for (auto a : Window::managedList) {
        Window::s = a->selected(window);
        if (Window::s != nullptr) {
            break;
        }
    }
}

void windowResizeCallback (GLFWwindow* window, int width, int height) {
    float scale = std::min(config::windowX/width, config::windowY/height);
    config::windowX = width;
    config::windowY = height;
    //config::proj = glm::ortho(-config::windowX / 2, config::windowX / 2, config::windowY / 2, -config::windowY / 2, 1.0f, -1.0f);
}

// creates the table of buttons by sampling a texture
void Window::createTable (Texture& texture) {
    ImGui::Begin("Options", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);
    // create options table
    texture.bind();
    int numOfElements = std::min((int)round(config::windowX / 32.0f), (int)config::options.size());
    ImVec2 padd{ 0.0, 0.0 };
    if (ImGui::BeginTable("table3", numOfElements, ImGuiTableFlags_NoPadInnerX)) {
        int x, y;
        for (int item = 0; item < config::options.size(); item++) {
            x = item % 5;
            y = item / 5;
            ImGui::TableNextColumn();

            ImVec2 size = ImVec2(config::buttonSize, config::buttonSize);   // Size of the image we want to make visible

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

            ImGui::PushID(item);
            if (ImGui::ImageButton("", (void*)texture.id, size, uv0, uv1)) {
                Window::buttonActions(item);
            }
            if (ImGui::IsItemHovered()) {
                ImGui::BeginTooltip();
                ImGui::Text(config::options.at(item).c_str());
                ImGui::EndTooltip();
            }
            ImGui::PopID();

        }
        ImGui::EndTable();
    }
    ImGui::SetWindowSize(ImVec2(config::windowX, config::buttonSize*2));
    ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));
    ImGui::End();
}

// when you click, it checks if we are focusing on a shape, if we are, create the color picker window.
void Window::createColorPicker () {
    if (Window::drawColorPicker && Window::focus != nullptr) {
        // create color picker
        ImGui::Begin("Color Picker", &Window::drawColorPicker);

        if (Window::focus != nullptr) {
            ImGui::ColorPicker4("Shape Color", (float*)Window::focus->getColor(), NULL);
        }
        ImGui::End();
    }
}

// created when the mouse hovers a Shape
void Window::createTooltip () {
    if (Window::s != nullptr) {
        ImGui::BeginTooltip();
        // If we are hovering a Bar, we would want to know some specific information.
        if (dynamic_cast<Bar*>(Window::s)) {
            ImGui::Text(
                std::string("Chain Number: ")
                .append(std::to_string(static_cast<Bar*>(Window::s)->getChainNumber()))
                .c_str()
            );
        }
        ImGui::EndTooltip();
    }
}

// draws the members of the managedList
void Window::draw () {
    for (auto a : managedList) {
        a->draw();
    }
}

// when a button is pressed, it calls this function with a "val" equal to which button in the window is pressed.
void Window::buttonActions(int val) {
    switch (val) {
    case (0): {
        std::cout << "beep" << std::endl;
        break;
    }
    case (1): {
        std::cout << "boop" << std::endl;
        break;
    }
    case (2): {
        std::cout << "bop" << std::endl;
        break;
    }
    case (3): {
        break;
    }
    case (4): {
        break;
    }
    case (5): {
        break;
    }
    case (6): {
        break;
    }
    case (7): {
        break;
    }
    case (8): {
        break;
    }
    case (9): {
        break;
    }
    case (10): {
        break;
    }
    case (11): {
        break;
    }
    case (12): {
        break;
    }
    case (13): {
        break;
    }
    case (14): {
        break;
    }
    case (15): {
        break;
    }
    case (16): {
        break;
    }
    case (17): {
        break;
    }
    case (18): {
        break;
    }
    case (19): {
        break;
    }
    }
}