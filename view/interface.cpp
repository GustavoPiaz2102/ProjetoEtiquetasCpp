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
    ImGui::Begin("Main", nullptr,
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus |
        ImGuiWindowFlags_NoBackground);
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
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


bool Interface::atualizar_frame(const cv::Mat& frame) {

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

bool Interface::atualizar_texto(std::string text){ /*
    // Define tamanho da janela (por exemplo, 800x300)
    ImVec2 window_size(800, 300);
    ImVec2 screen_size = ImGui::GetIO().DisplaySize;

    // Centraliza a janela na tela
    ImVec2 window_pos((screen_size.x - window_size.x) * 0.5f,
                      (screen_size.y - window_size.y) * 0.5f);

    ImGui::SetNextWindowPos(window_pos);
    ImGui::SetNextWindowSize(window_size);

    ImGui::Begin("Texto", nullptr,
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus);

    ImGui::TextWrapped("%s", text.c_str());

    ImGui::End();
    return true;
    */
   return true;
}





// Função auxiliar para exibir a data formatada
std::string Interface::FormatDate(int day, int month, int year) {
    const char* month_names[] = {
        "JAN", "FEV", "MAR", "ABR", "MAI", "JUN",
        "JUL", "AGO", "SET", "OUT", "NOV", "DEZ"
    };

    std::ostringstream oss;
    oss.fill('0');
    oss.width(2);
    oss << day << '/';
    oss << month_names[month] << '/';
    oss << year;

    return oss.str();
}

bool Interface::requisitar_data(std::string& selected_date) {
    // Criar a janela principal onde os combos vão aparecer
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::Begin("MainPage", nullptr,
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus);

    static int selected_day = 0;
    static int selected_month = 0;
    static int selected_year = 0;

    static const char* month_names[] = {
        "JAN", "FEV", "MAR", "ABR", "MAI", "JUN",
        "JUL", "AGO", "SET", "OUT", "NOV", "DEZ"
    };

    static int days[31];
    static int years[100];

    static bool initialized = false;
    if (!initialized) {
        for (int i = 0; i < 31; ++i) days[i] = i + 1;
        for (int i = 0; i < 100; ++i) years[i] = 2025 + i;
        initialized = true;
    }

    // Aumenta o tamanho dos widgets e espaçamentos
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(15, 25)); // Mais preenchimento
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(15, 15));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(15, 15));

    // Aumentar fonte
    ImGuiIO& io = ImGui::GetIO();
    float originalFontSize = io.FontDefault ? io.FontDefault->FontSize : 20.0f;
    ImGui::SetWindowFontScale(1.5f);  // Escala geral da fonte

    // Dia
    ImGui::SetNextItemWidth(200.0f);
    if (ImGui::BeginCombo("Dia", std::to_string(days[selected_day]).c_str())) {
        for (int i = 0; i < 31; ++i) {
            bool is_selected = (selected_day == i);
            if (ImGui::Selectable(std::to_string(days[i]).c_str(), is_selected))
                selected_day = i;
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
    ImGui::SameLine(0, 20.0f);

    // Mês
    ImGui::SetNextItemWidth(200.0f);
    if (ImGui::BeginCombo("Mês", month_names[selected_month])) {
        for (int i = 0; i < 12; ++i) {
            bool is_selected = (selected_month == i);
            if (ImGui::Selectable(month_names[i], is_selected))
                selected_month = i;
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
    ImGui::SameLine(0, 20.0f);

    // Ano
    ImGui::SetNextItemWidth(200.0f);
    if (ImGui::BeginCombo("Ano", std::to_string(years[selected_year]).c_str())) {
        for (int i = 0; i < 100; ++i) {
            bool is_selected = (selected_year == i);
            if (ImGui::Selectable(std::to_string(years[i]).c_str(), is_selected))
                selected_year = i;
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    ImGui::Spacing();
    ImGui::Spacing();

    bool clicked = false;
    if (ImGui::Button("OK", ImVec2(200, 100))) {
        selected_date = FormatDate(days[selected_day], selected_month, years[selected_year]);
        clicked = true; 
    }

    // Volta ao normal
    ImGui::SetWindowFontScale(1.0f);
    ImGui::PopStyleVar(3);

    if (!selected_date.empty()) {
        ImGui::Spacing();
        ImGui::Text("Data selecionada: %s", selected_date.c_str());
    }

    ImGui::End();

    return clicked;
}
