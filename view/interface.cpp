#include "interface.h"

void Interface::beginFullscreenWindow(const char* name) {
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::Begin(name, nullptr,
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus);
}

Interface::Interface(Validator& val) : validator(val) {
    if (!iniciar_janela()) {
        std::cerr << "Falha ao iniciar a janela!\n";
    }
    // Inicializa a textura com ID 0
    texture_id = 0;
    should_close = false;
}

Interface::~Interface() {
    if (texture_id != 0) {
        glDeleteTextures(1, &texture_id);
    }
    // Shutdown da ImGui só aqui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    finalizar_janela();
}


bool Interface::iniciar_janela() {
    if(janela_iniciada) {
        std::cerr << "Janela já foi iniciada!\n";
        return false;
    }
    else{
        if (!glfwInit()) {
            std::cerr << "Falha ao inicializar GLFW\n";
            return false;
        }

        window = glfwCreateWindow(JANELA_LARGURA, JANELA_ALTURA, JANELA_TITULO, NULL, NULL);
        if (!window) {
            glfwTerminate();
            std::cerr << "Falha ao criar janela GLFW\n";
            return false;
        }
        glfwMakeContextCurrent(window);
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 150");
        janela_iniciada = true;
        return true;
    }
}

bool Interface::finalizar_janela() {
    if (janela_iniciada) {
        if (window) {
            glfwDestroyWindow(window);
            glfwTerminate();
            window = NULL;

        }
        janela_iniciada = false;
        return true;
    } else {
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
    glClearColor(FUNDO_R, FUNDO_G, FUNDO_B, FUNDO_A);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
}

void Interface::menu(int& selected_option) {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));  // Remove padding interno
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, TAMANHO_BORDA_JANELA);  // tamanho das bordas
    beginFullscreenWindow("Main");
    ImGui::SetWindowFontScale(ESCALA_FONTE_MENU);  // Ajusta o tamanho da fonte
    ImGui::Text(" Escolha um botão:");
    if (ImGui::Button("Rodar Sistema", ImVec2(TAMANHO_BOTAO_LARG, TAMANHO_BOTAO_ALT))) {
        selected_option = 0;
    }
    ImGui::SameLine(0, ESPACO_ENTRE_BOTOES);
    std::string info = "L. "+validator.GetLT() + "\n" + "FAB.: "+validator.GetFAB() + "\n" + "VAL.: "+validator.GetVAL();
    ImGui::Text("%s", info.c_str());
    ImGui::Dummy(ImVec2(0, ESPACO_ENTRE_BOTOES)); // Espaço entre os botões e a data
    if (ImGui::Button("Atualizar Data", ImVec2(TAMANHO_BOTAO_LARG, TAMANHO_BOTAO_ALT))) {
        selected_option = 1;
    }

    ImGui::SameLine(0, ESPACO_ENTRE_BOTOES);
    if (ImGui::Button("Sair", ImVec2(TAMANHO_BOTAO_LARG, TAMANHO_BOTAO_ALT))) {
        selected_option = 2;
    }

    ImGui::End();
    ImGui::PopStyleVar(2); // Restaura os estilos
}


bool Interface::atualizar_frame(const cv::Mat& frame) {
    if (frame.empty()) {
        std::cerr << "Frame vazio recebido!" << std::endl;
        return false;
    }

    // Obtenha as dimensões do frame original
    int frame_width = frame.cols;
    int frame_height = frame.rows;

    // (Re)cria a textura APENAS se as dimensões do frame de entrada mudarem.
    // Isso acontece raramente, geralmente apenas no primeiro frame.
    if (texture_id == 0 || frame_width != last_width || frame_height != last_height) {
        if (texture_id != 0) {
            glDeleteTextures(1, &texture_id);
        }

        glGenTextures(1, &texture_id);
        glBindTexture(GL_TEXTURE_2D, texture_id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // Aloca o espaço na GPU para a textura com as dimensões do frame original.
        // Os dados ainda não são enviados aqui (o último parâmetro é nullptr).
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, frame_width, frame_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

        last_width = frame_width;
        last_height = frame_height;
    }

    // --- Preparação dos dados para upload ---
    cv::Mat image_rgba;
    // Converte o formato de cor do frame para RGBA, que é o que o OpenGL espera.
    // Esta operação é necessária.
    switch (frame.channels()) {
        case 1:
            cv::cvtColor(frame, image_rgba, cv::COLOR_GRAY2RGBA);
            break;
        case 3:
            cv::cvtColor(frame, image_rgba, cv::COLOR_BGR2RGBA);
            break;
        case 4:
            // Se já for RGBA, apenas use uma referência para evitar cópia.
            image_rgba = frame;
            break;
        default:
            std::cerr << "Formato de imagem não suportado!" << std::endl;
            return false;
    }

    // Atualiza a textura existente na GPU com os novos dados do frame.
    // Esta é a operação mais rápida e será usada na maioria das vezes.
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, frame_width, frame_height, GL_RGBA, GL_UNSIGNED_BYTE, image_rgba.data);

    // --- Desenho da Interface ---
    
    // Calcula a proporção da imagem para manter o aspecto na tela.
    // Usa as dimensões originais do frame para o cálculo.
    float aspect_ratio = static_cast<float>(frame_width) / static_cast<float>(frame_height);
    float display_width = ImGui::GetIO().DisplaySize.x * 0.8f; // 80% da largura da janela
    float display_height = display_width / aspect_ratio;

    beginFullscreenWindow("Camera View");
    
    // Centraliza a imagem na janela
    ImGui::SetCursorPosX((ImGui::GetWindowWidth() - display_width) * 0.5f);
    ImGui::SetCursorPosY((ImGui::GetWindowHeight() - display_height) * 0.5f);

    // Desenha a imagem. ImGui/GPU cuidará do escalonamento da textura
    // para o tamanho (display_width, display_height).
    ImGui::Image((ImTextureID)(intptr_t)texture_id, ImVec2(display_width, display_height));

    // Botão de retorno
    bool return_to_menu = false;
    ImGui::SetWindowFontScale(ESCALA_FONTE_MENU);
    if (ImGui::Button("Voltar ao Menu", ImVec2(TAMANHO_BOTAO_PEQUENO_LARG, TAMANHO_BOTAO_PEQUENO_ALT))) {
        return_to_menu = true;
    }

    ImGui::End();
    return return_to_menu;
}

// ... (o resto do seu código permanece igual)

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
bool Interface::requisitar_lt(std::string& selected_lt) {
    // Criar a janela principal onde os combos vão aparecer
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    beginFullscreenWindow("LotePage");
    static bool initialized = false;
    if (!initialized) {
        for (int i = 0; i <= 100; ++i) lotes[i] = i;
        for (int i = 0; i < 50; ++i) anos[i] = 23 + i;
        initialized = true;
    }

    // Aumenta o tamanho dos widgets e espaçamentos
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, PADDING_FRAME);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, SPACING_ITEM);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, SPACING_INTERNO);

    // Aumentar fonte
    ImGuiIO& io = ImGui::GetIO();
    float originalFontSize = io.FontDefault ? io.FontDefault->FontSize : 20.0f;
    ImGui::SetWindowFontScale(ESCALA_FONTE_DATA);

    // Lote
    char lote_label[4]; // 3 dígitos + \0
    snprintf(lote_label, sizeof(lote_label), "%03d", lotes[selected_lote]);
    ImGui::Text(" Insira o valor do lote: ");
    ImGui::Text(" L. ");
    ImGui::SameLine(0, ESPACO_ENTRE_BOTOES);
    ImGui::SetNextItemWidth(COMBO_LARGURA);
    if (ImGui::BeginCombo("/", lote_label)) {
        for (int i = 0; i <= 100; ++i) {
            char lote_item[4];
            snprintf(lote_item, sizeof(lote_item), "%03d", lotes[i]);
            bool is_selected = (selected_lote == i);
            if (ImGui::Selectable(lote_item, is_selected))
                selected_lote = i;
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
    ImGui::SameLine(0, ESPACO_ENTRE_BOTOES);

    // Ano
    ImGui::SetNextItemWidth(COMBO_LARGURA);
    char ano_label[3]; // 2 dígitos + \0
    snprintf(ano_label, sizeof(ano_label), "%02d", anos[selected_ano]);
    if (ImGui::BeginCombo(" ", ano_label)) {
        for (int i = 0; i < 50; ++i) {
            char ano_item[3];
            snprintf(ano_item, sizeof(ano_item), "%02d", anos[i]);
            bool is_selected = (selected_ano == i);
            if (ImGui::Selectable(ano_item, is_selected))
                selected_ano = i;
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    ImGui::Spacing();
    ImGui::Spacing();

    bool clicked = false;
    if (ImGui::Button("OK", ImVec2(TAMANHO_BOTAO_PEQUENO_LARG, TAMANHO_BOTAO_PEQUENO_ALT * 2))) {
        char result[7]; // 3 + 1 + 2 + \0 -> "006/25"
        snprintf(result, sizeof(result), "%03d/%02d", lotes[selected_lote], anos[selected_ano]);
        selected_lt = result;
        clicked = true;
    }

    // Volta ao normal
    ImGui::SetWindowFontScale(1.0f);
    ImGui::PopStyleVar(3);

    if (!selected_lt.empty()) {
        ImGui::Spacing();
        ImGui::Text("Lote selecionado: %s", selected_lt.c_str());
    }

    ImGui::End();

    return clicked;
}

bool Interface::requisitar_data(std::string& selected_date,int tipo) {
    // Criar a janela principal onde os combos vão aparecer
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    beginFullscreenWindow("MainPage");

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
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, PADDING_FRAME); // Mais preenchimento
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, SPACING_ITEM);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, SPACING_INTERNO);

    // Aumentar fonte
    ImGuiIO& io = ImGui::GetIO();
    float originalFontSize = io.FontDefault ? io.FontDefault->FontSize : 20.0f;
    ImGui::SetWindowFontScale(ESCALA_FONTE_DATA);  // Escala geral da fonte
    if (tipo == 0){
        ImGui::Text(" Escolha uma data de Fabricação:");
    }
    else if (tipo == 1){
        ImGui::Text(" Escolha uma data de Validade:");
    }

    // Dia
    ImGui::SetNextItemWidth(COMBO_LARGURA);
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
    ImGui::SameLine(0, ESPACO_ENTRE_BOTOES);

    // Mês
    ImGui::SetNextItemWidth(COMBO_LARGURA);
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
    ImGui::SameLine(0, ESPACO_ENTRE_BOTOES);

    // Ano
    ImGui::SetNextItemWidth(COMBO_LARGURA);
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
    if (ImGui::Button("OK", ImVec2(TAMANHO_BOTAO_PEQUENO_LARG, TAMANHO_BOTAO_PEQUENO_ALT * 2))) {
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
