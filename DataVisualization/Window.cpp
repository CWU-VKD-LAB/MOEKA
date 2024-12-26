#include "Window.h"
#define NOMINMAX

std::vector<Drawable*> Window::managedList{};
Drawable* Window::s = nullptr;
Drawable* Window::focus = nullptr;
Form Window::form{};

bool Window::drawColorPicker = false;
void cursorCallback(GLFWwindow* window, int button, int action, int mods);
void keyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods);
void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
void windowResizeCallback (GLFWwindow* window, int width, int height);
void frameBufferCallback(GLFWwindow* window, int width, int height);

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
    glViewport(0, 0, (GLsizei)config::windowX, (GLsizei)config::windowY);
    //

    // init glew
    initGLEW();

    glfwSetMouseButtonCallback(window, cursorCallback);
    glfwSetKeyCallback(window, keyCallBack);
    glfwSetCursorPosCallback(window, cursorPositionCallback);
    glfwSetWindowSizeCallback(window, windowResizeCallback);
    glfwSetFramebufferSizeCallback(window, frameBufferCallback);
    
    initImGui();

    for (int a = 0; a < classCount; a++) {
        float val = 0.8f * (((float)a+1)/(config::maxClassValue+1));
        //config::classColors.insert(config::classColors.end(), ImVec4{val, val, val, 1.0f});
        config::classColors.push_back(ImVec4{ val,val,val,1.0f });
    }
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
    io = ImGui::GetIO();
    flags |= ImGuiWindowFlags_NoMove;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    font = io.Fonts->AddFontFromFileTTF("resources/fonts/ProggyClean.ttf", 20.0f);
    monoFont = io.Fonts->AddFontFromFileTTF("resources/fonts/JetBrainsMono-Medium.ttf", config::windowY / 42.0f);
    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
}

void Window::endImGui () {
    ImGui::ClearWindowSettings("FunctionView");
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
    createOptions(texture);
    if (!Window::form.open) {
        createColorPicker();
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

    if (config::drawIndex < 0 || config::drawIndex >= Window::managedList.size() || Window::managedList[config::drawIndex] == nullptr) {
        std::cerr << "Invalid drawIndex or null pointer in managedList. Window.cpp" << std::endl;
        return;
    }

    // cleanup later but fine in short term.
    if (key == GLFW_KEY_UP && (action == GLFW_REPEAT || action == GLFW_PRESS || glfwGetKey(window, key))) {
        Window::managedList[config::drawIndex]->setTranslation(Window::managedList[config::drawIndex]->getX(), Window::managedList[config::drawIndex]->getY() - 5);
    }
    if (key == GLFW_KEY_DOWN && (action == GLFW_REPEAT || action == GLFW_PRESS || glfwGetKey(window, key))) {
        Window::managedList[config::drawIndex]->setTranslation(Window::managedList[config::drawIndex]->getX(), Window::managedList[config::drawIndex]->getY() + 5);
    }
    if (key == GLFW_KEY_LEFT && (action == GLFW_REPEAT || action == GLFW_PRESS || glfwGetKey(window, key))) {
        Window::managedList[config::drawIndex]->setTranslation(Window::managedList[config::drawIndex]->getX() - 5, Window::managedList[config::drawIndex]->getY());
    }
    if (key == GLFW_KEY_RIGHT && (action == GLFW_REPEAT || action == GLFW_PRESS || glfwGetKey(window, key))) {
        Window::managedList[config::drawIndex]->setTranslation(Window::managedList[config::drawIndex]->getX() + 5, Window::managedList[config::drawIndex]->getY());
    }
    if (key == GLFW_KEY_KP_ADD && (action == GLFW_REPEAT || action == GLFW_PRESS || glfwGetKey(window, key))) {
        Window::managedList[config::drawIndex]->setScale(Window::managedList[config::drawIndex]->getScaleX() + .02f, Window::managedList[config::drawIndex]->getScaleY() + .02f);
    }
    if (key == GLFW_KEY_KP_SUBTRACT && (action == GLFW_REPEAT || action == GLFW_PRESS || glfwGetKey(window, key))) {
        if ((Window::managedList[config::drawIndex]->getScaleX() - .02) > 0.00001 && (Window::managedList[config::drawIndex]->getScaleY() - .02) > 0.00001) {
            Window::managedList[config::drawIndex]->setScale(Window::managedList[config::drawIndex]->getScaleX() - .02f, Window::managedList[config::drawIndex]->getScaleY() - .02f);
        }
    }
    if (key == GLFW_KEY_ESCAPE && (action == GLFW_PRESS || glfwGetKey(window, key))) {
        if (Window::form.current == state::FUNCTION && Window::form.open) {
            Window::form.functionList.pop_back();
        }
        
        Window::form.open = !Window::form.open;
        // prob better in the actual form class, but sets it so if the user presses escape while the form is open on the intro screen, it goes to the prep screen rather than
        // the intro screen again when the window is opened.
        if (Window::form.current == state::INTRODUCTION) {
            Window::form.current = state::FUNCTION;
        }  
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
    config::windowX = (float)width;
    config::windowY = (float)height;
}

void frameBufferCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void Window::createOptions(Texture& texture) {
    // Set up the window properties for the Options window
    ImGui::Begin("Options", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);

    // Calculate the width of the options window based on the number of buttons and their size
    float optionWidth = config::buttonSize * config::options.size() + (config::options.size() - 1) * ImGui::GetStyle().ItemSpacing.x + (2.0f * ImGui::GetStyle().WindowPadding.x);
    ImGui::SetWindowSize(ImVec2(optionWidth, config::buttonSize + (2.0f * ImGui::GetStyle().WindowPadding.y)));
    ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));

    // Start drawing the buttons
    ImGui::SetCursorPos(ImVec2(ImGui::GetStyle().WindowPadding.x, ImGui::GetStyle().WindowPadding.y));
    texture.bind();

    // Calculate the number of elements to fit within the window size
    int numOfElements = std::min((int)round(config::windowX / 32.0f), (int)config::options.size());
    ImVec2 windowSize = ImGui::GetWindowSize();

    // Loop through each option and create a button for each
    for (int item = 0; item < config::options.size(); ++item) {
        int x = item % 5;  // Determine the X position
        int y = item / 5;  // Determine the Y position

        // Ensure proper button spacing
        if (item != 0) {
            ImGui::SameLine();
        }

        ImVec2 size(config::buttonSize, config::buttonSize); // Size of the button

        // Calculate the UV coordinates for the texture slice
        ImVec2 uv0(
            (config::buttonSize * x / texture.getWidth()),
            (config::buttonSize * y / texture.getHeight())
        );

        ImVec2 uv1(
            (config::buttonSize * (x + 1) / texture.getWidth()),
            (config::buttonSize * (y + 1) / texture.getHeight())
        );

        ImGui::PushID(item);  // Push an ID for ImGui state management
        if (ImGui::ImageButton("", (void*)texture.id, size, uv0, uv1)) {
            Window::buttonActions(item);
        }

        // Display a tooltip when the button is hovered
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text("Currently non-functional");
            ImGui::Text(config::options.at(item).c_str());
            ImGui::EndTooltip();
        }
        ImGui::PopID();
    }

    ImGui::End();

    // Hovering functionality for class name display
    if (Window::s != nullptr && !drawColorPicker && !form.open) {
        ImGui::Begin("##", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoFocusOnAppearing);

        double x, y;
        glfwGetCursorPos(window, &x, &y);
        ImGui::SetWindowPos(ImVec2((float)x, (float)y));
        ImGui::SetWindowSize(ImVec2(200.0f, 150.0f));

        int c = reinterpret_cast<Bar*>(Window::s)->classVal;
        std::string text = "Classified As:\n ";
        text += (c == -1) ? "Unavailable" : classNames[c];

        ImGui::Text(text.c_str());
        ImGui::End();
    }

    // Scaling buttons window
    ImGui::Begin("ScaleWindow", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
    ImVec2 scalingWindowSize(config::windowX * 0.15f, config::windowY * BUTTON_HEIGHT);
    ImGui::SetWindowSize(scalingWindowSize);
    ImGui::SetWindowPos(ImVec2(config::windowX * 0.5f, config::windowY - (config::windowY * BUTTON_HEIGHT)));

    if (Window::managedList.empty() || config::drawIndex < 0 || config::drawIndex >= Window::managedList.size() || Window::managedList[config::drawIndex] == nullptr) {
        ImGui::Text("No Loaded Model.");
    }
    else {
        auto model = Window::managedList[config::drawIndex];

        // Set larger button sizes
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(20, 5));

        // Scale X buttons
        ImGui::PushItemFlag(ImGuiItemFlags_ButtonRepeat, true);
        ImGui::Text("Scale X");
        ImGui::SameLine();
        if (ImGui::ArrowButton("##downScaleX", ImGuiDir_Down)) {
            if (model->getScaleX() - Window::scaleBy.x > 0.2f) {
                model->setScale(model->getScaleX() - Window::scaleBy.x, model->getScaleY());
            }
        }

        ImGui::SameLine();
        if (ImGui::ArrowButton("##upScaleX", ImGuiDir_Up)) {
            model->setScale(model->getScaleX() + Window::scaleBy.x, model->getScaleY());
        }
        ImGui::PopItemFlag();

        // Scale Y buttons
        ImGui::PushItemFlag(ImGuiItemFlags_ButtonRepeat, true);
        ImGui::Text("Scale Y");
        ImGui::SameLine();
        if (ImGui::ArrowButton("##downScaleY", ImGuiDir_Down)) {
            if (model->getScaleY() - Window::scaleBy.y > 0.2f) {
                model->setScale(model->getScaleX(), model->getScaleY() - Window::scaleBy.y);
            }
        }
        ImGui::SameLine();
        if (ImGui::ArrowButton("##upScaleY", ImGuiDir_Up)) {
            model->setScale(model->getScaleX(), model->getScaleY() + Window::scaleBy.y);
        }

        ImGui::PopItemFlag();
        // Pop Style Variables
        ImGui::PopStyleVar(2);
    }
    ImGui::End();

    ImGui::Begin("FunctionView", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
    ImGui::SetWindowSize(ImVec2(config::windowX * .15f, config::windowY * BUTTON_HEIGHT));
    ImGui::SetWindowPos(ImVec2(config::windowX * .75f, config::windowY * (1.0f - BUTTON_HEIGHT)));

    if (!Form::functionList.empty()) {
        for (auto& func : Form::functionList) {
            tree(func);
        }
        if (Form::comparisonFunction != nullptr) {
            tree(Form::comparisonFunction);
        }
    }
    else {
        ImGui::SetNextItemWidth(config::windowX - optionWidth - ImGui::GetStyle().WindowPadding.x);
        ImGui::TextWrapped("There are no currently loaded functions. Please Open the prep window to create or load functions to view them in this window.");
    }

    ImGui::End();

    // Bottom row of buttons
    ImGui::Begin("##formState", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
    const int numButtons = 5;  // Number of buttons in the bottom row

    // using .5 so it takes up half the bottom
    ImVec2 buttonWindowSize(config::windowX * .5f, config::windowY * BUTTON_HEIGHT); // Calculate window size
    ImGui::SetWindowSize(buttonWindowSize);
    ImGui::SetWindowPos(ImVec2(0.0f, config::windowY * (1.0f - BUTTON_HEIGHT)));

    // Buttons for various functions
    ImVec2 buttonSize((0.95f) * buttonWindowSize.x / numButtons, (.9f) * buttonWindowSize.y);
    if (ImGui::Button("Open Help##", buttonSize)) {
        form.current = state::INTRODUCTION;
        form.openWindow();
    }
    ImGui::SameLine();
    if (ImGui::Button("Open Prep##", buttonSize)) {
        form.current = state::PREP;
        form.setNewFunc();
        form.openWindow();
    }
    ImGui::SameLine();
    if (ImGui::Button("Edit colors##", buttonSize)) {
        form.current = state::COLOR;
        form.openWindow();
    }
    ImGui::SameLine();
    if (ImGui::Button("Compare Funcs", buttonSize)) {
        form.current = state::COMPARE;
        form.openWindow();
    }
    ImGui::SameLine();
    if (ImGui::Button("ML", buttonSize)) {
        form.current = state::ML;
        form.openWindow();
        std::string path = "..\\DataVisualization\\mlModels";
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            std::string temp = entry.path().string();
            form.ml.mlFilePaths.push_back(temp.replace(temp.begin(), temp.begin() + 2, ""));
        }
    }
    ImGui::End();
}

void Window::treeDescription (Function* function) {
    //ImGui::SameLine();
    
    if (ImGui::Button("Math##", ImVec2(ImGui::CalcTextSize("Math").x * 1.0f, ImGui::GetFontSize() * 1.0f))){
        //std::cout << "asdf" << std::endl;
        ImGui::OpenPopup("Math Version");
    }
    ImGui::PushFont(monoFont);
    if (ImGui::BeginPopup("Math Version", ImGuiWindowFlags_AlwaysAutoResize)) {
        if (!function->siblingfunctionList.empty()) {
            std::vector<std::string> full{};

            // sibling func
            for (int a = 0; a < function->siblingfunctionList.size(); a++) {
                // clause
                for (int b = 0; b < function->siblingfunctionList[a].size(); b++) {
                    std::vector<std::string> functionDef{};
                    // individual values
                    std::vector<std::string> clause{};
                    for (int c = 0; c < function->siblingfunctionList[a][b]->size(); c++) {
                        if (function->siblingfunctionList[a][b]->at(c) != 0) {
                            clause.push_back( function->attributeNames[c] + std::string(" >= ") + std::to_string( function->siblingfunctionList[a][b]->at(c) ) );
                        }
                    }
                    for (int c = 0; c < clause.size(); c++) {
                        //ss << clause[c];
                        functionDef.push_back(clause[c]);
                        if (c + 1 < clause.size()) {
                            functionDef.push_back(" AND ");
                        }
                    }
                    if (!functionDef.empty()) {
                        std::ostringstream imploded;
                        std::copy(functionDef.begin(), functionDef.end(), std::ostream_iterator<std::string>(imploded));
                        full.push_back(imploded.str());
                    }
                }
                
            }
            
            std::string fullString = "";
            for (int a = 0; a < full.size(); a++) {
                fullString += full[a];
                if ((a+1) < full.size()) {
                    fullString+= "\nOR\n";
                }
            }
            if (fullString == "") {
                fullString = "There were no loaded Clauses.";
            }
            ImGui::Text(fullString.c_str());
        }
        else {
            ImGui::Text("Function does not have its own clauses.");
        }
        ImGui::EndPopup();
    }
    ImGui::PopFont();


    // English representation of the function.
    ImGui::SameLine();
    if (ImGui::Button( (std::string("English##") + std::string(function->functionName)).c_str(), ImVec2{ ImGui::CalcTextSize("English").x * 1.0f, ImGui::GetFontSize() * 1.0f })) {
        ImGui::OpenPopup("English Version");
    }
    ImGui::PushFont(monoFont);
    if (ImGui::BeginPopup("English Version")) {
        if (!function->siblingfunctionList.empty()) {
            std::vector<std::string> full{};

            // sibling func
            for (int a = 0; a < function->siblingfunctionList.size(); a++) {
                // clause
                for (int b = 0; b < function->siblingfunctionList[a].size(); b++) {
                    std::vector<std::string> functionDef{};
                    // individual values
                    std::vector<std::string> clause{};
                    for (int c = 0; c < function->siblingfunctionList[a][b]->size(); c++) {
                        if (function->siblingfunctionList[a][b]->at(c) != 0) {
                            clause.push_back("If " + std::string(function->attributeNames[c]) + " is " + std::to_string(function->siblingfunctionList[a][b]->at(c)));
                        }
                    }
                    for (int c = 0; c < clause.size(); c++) {
                        //ss << clause[c];
                        functionDef.push_back(clause[c]);
                        if ((long)c + 1 < clause.size()) {
                            functionDef.push_back(" AND ");
                        }
                    }
                    if (!functionDef.empty()) {
                        std::ostringstream imploded;
                        std::copy(functionDef.begin(), functionDef.end(), std::ostream_iterator<std::string>(imploded));
                        full.push_back(imploded.str());
                    }
                }

            }

            std::string fullString = "";
            for (int a = 0; a < full.size(); a++) {
                fullString += full[a];
                if ((a + 1) < full.size()) {
                    fullString += "\nOR\n";
                }
            }
            if (fullString == "") {
                fullString = "There were no loaded Clauses.";
            }
            ImGui::Text(fullString.c_str());
        }
        else {
            ImGui::Text("Function does not have its own clauses.");
        }
        ImGui::EndPopup();
    }
    ImGui::PopFont();
}

void Window::tree (Function* function) {
    if (function)
    {
        if (ImGui::TreeNode(("##" + std::string(function->functionName)).c_str(), function->functionName)) {
            ImGui::SetNextItemOpen(false, ImGuiCond_Once);
            // Math representation of the function.
            treeDescription(function);

            if (!function->siblingfunctionList.empty()) {
                int siblingIndex = 1;
                for (auto a : function->siblingfunctionList) {
                    if (ImGui::TreeNode((std::string("Sibling k = ") + std::to_string(siblingIndex) + "##").c_str())) {
                        for (auto b : a) {
                            std::stringstream ss;
                            for (int c = 0; c < b->size(); c++) {
                                ss << std::to_string(b->at(c));
                                if ((int)c + 1 != b->size()) {
                                    ss << ", ";
                                }
                            }
                            ImGui::Text(ss.str().c_str());
                        }
                        ImGui::TreePop();
                        siblingIndex++;
                    }
                }
            }


            for (auto a : function->subfunctionList) {
                tree(a);
            }
            ImGui::TreePop();
        }
    }
}

// when you click, it checks if we are focusing on a shape, if we are, create the color picker window.
void Window::createColorPicker () {
    if (Window::drawColorPicker && Window::focus != nullptr) {
        // create color picker
        ImGui::Begin("Color Picker", &Window::drawColorPicker);
        if (Window::focus->isBar) {
            ImGui::Checkbox("Use individual color?", &static_cast<Bar*>(static_cast<Shape*>(Window::focus))->useIndividualColor );
        }
        //ImGui::Checkbox("##", );
        if (Window::focus != nullptr) {
            ImGui::ColorPicker4("Shape Color", (float*)Window::focus->getColor(), NULL);
        }
        ImGui::End();
    }
}

// draws the members of the managedList
void Window::draw () {
    if (!managedList.empty()) {
        managedList[config::drawIndex]->draw();
    }
}

// TODO: reverse classes vector and add information to bar for the tooltip
void Window::addModelFromFunctionForm() {
    const auto classIndex = this->form.getFunc()->hanselChains->dimension - 1;
    Model* m = new Model();

    for (const auto& chain : this->form.getFunc()->hanselChains->hanselChainSet)
    {
        std::vector<int> classes;

        for (const auto& e : chain)
        {
            classes.push_back(e[classIndex]);
        }


        m->addColumn(&classes);
    }

    m->fitToScreen();
    this->addToRender(m);
    this->form.addModel = !this->form.addModel;
}

void Window::addModelFromCompareForm()
{
    Model* m = new Model();

    for (auto c : *form.getCompare().comparisons)
    {
        m->addColumn(&c);
    }

    m->fitToScreen();
    this->addToRender(m);
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
    }
}