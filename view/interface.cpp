#include "interface.h"

Interface::Interface() {
    // Construtor da classe. Nenhuma inicialização extra é necessária.
}

Interface::~Interface() {
    if (texture_id != 0) {
        glDeleteTextures(1, &texture_id);  // Libera a textura se existir
    }
    finalizar_janela();  // Finaliza a janela GLFW
}

bool Interface::iniciar_janela() {
    if(janela_iniciada == true) {
        std::cerr << "Janela já foi iniciada!\n";
        return false;
    }
    else{
    if (!glfwInit()) {
        std::cerr << "Falha ao inicializar GLFW\n";
        return false;
    }

    window = glfwCreateWindow(1920, 1080, "Menu", NULL, NULL);
    if (!window) {
        glfwTerminate();
        std::cerr << "Falha ao criar janela GLFW\n";
        return false;
    }
    glfwMakeContextCurrent(window);
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");
    janela_iniciada = true;
    return true;
}
}

bool Interface::finalizar_janela() {
    if(janela_iniciada == true) {

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    if (window) {
        glfwDestroyWindow(window);
        glfwTerminate();
        window = NULL;
    }
    janela_iniciada = false;
    return true;
}
    else {
        std::cerr << "Janela não foi iniciada!\n";
        return false;
    }
}

void Interface::process_events() {

    
    glfwPollEvents();
    should_close = glfwWindowShouldClose(window);
}

bool Interface::shouldClose() const {

    return should_close;
}

void Interface::begin_frame() {

    
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void Interface::end_frame() {

    
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
}

void Interface::menu(int& selected_option) {

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::Begin("Main", nullptr,
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus |
        ImGuiWindowFlags_NoBackground);

    ImGui::Text("Escolha um botão:");

    if (ImGui::Button("Rodar Sistema", ImVec2(800, 400))) {
        selected_option = 0;
    }
    
    ImGui::SameLine(0, 10.0f);
    if (ImGui::Button("Atualizar Data", ImVec2(800, 400))) {
        selected_option = 1;
    }

    ImGui::Dummy(ImVec2(0, 10.0f));
    if (ImGui::Button("Sair", ImVec2(800, 400))) {
        selected_option = 2;
    }

    ImGui::End();
}

bool Interface::atulizar_frame(const cv::Mat& frame) {

    if (frame.empty()) {
        std::cerr << "Frame vazio recebido!" << std::endl;
        return false;
    }

    cv::Mat image_rgba;
    if (frame.channels() == 1) {
        cv::cvtColor(frame, image_rgba, cv::COLOR_GRAY2RGBA);
    } else if (frame.channels() == 3) {
        cv::cvtColor(frame, image_rgba, cv::COLOR_BGR2RGBA);
    } else if (frame.channels() == 4) {
        image_rgba = frame.clone();
    } else {
        ImGui::Text("Formato de imagem não suportado!");
        return false;
    }

    image_width = image_rgba.cols;
    image_height = image_rgba.rows;

    if (texture_id == 0) {
        glGenTextures(1, &texture_id);
    }

    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 
                0, GL_RGBA, GL_UNSIGNED_BYTE, image_rgba.data);

    float aspect_ratio = (float)image_width / (float)image_height;
    float display_width = ImGui::GetIO().DisplaySize.x * 0.8f;
    float display_height = display_width / aspect_ratio;

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::Begin("Camera View", nullptr,
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus);

    // Posição da imagem
    ImGui::SetCursorPosX((ImGui::GetWindowWidth() - 1.1 * display_width) * 0.5f);
    ImGui::SetCursorPosY((ImGui::GetWindowHeight() - 1.1 * display_height) * 0.5f);
    ImGui::Image((ImTextureID)(intptr_t)texture_id, ImVec2(display_width, display_height));

    bool return_to_menu = false;
    if (ImGui::Button("Voltar ao Menu", ImVec2(200, 50))) {
        return_to_menu = true;
    }

    ImGui::End();

    return return_to_menu;
}

bool Interface::atualizar_texto(std::string text) {
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    
    return true;
}