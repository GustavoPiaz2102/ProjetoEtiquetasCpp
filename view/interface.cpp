#include "interface.h"

Interface::Interface(Validator& val) : validator(val) {
    texture_id = 0;
    last_width = last_height = 0;
    should_close = false;
    resolution_scale = 1.0f;  // Controle de escala de resolução
    max_display_width = 1920; // Largura máxima padrão

    if (!iniciar_janela()) {
        std::cerr << "Falha ao iniciar a janela!\n";
    }
}

Interface::~Interface() {
    if (texture_id) {
        glDeleteTextures(1, &texture_id);
    }

    if (pboInitialized) {
        glDeleteBuffers(2, pboIds);
    }

    if (janela_iniciada) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        finalizar_janela();
    }
}

void Interface::beginFullscreenWindow(const char* name) {
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::Begin(name, nullptr,
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus);
}

    bool Interface::iniciar_janela() {
        if (janela_iniciada) return false;

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
        glfwSwapInterval(0);

        // Substitui a verificação do GLAD por GLEW
        glewExperimental = GL_TRUE;
        if (glewInit() != GLEW_OK) {
            std::cerr << "Falha ao inicializar GLEW" << std::endl;
            return false;
        }

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 130");

        janela_iniciada = true;
        return true;
    }

bool Interface::finalizar_janela() {
    if (!janela_iniciada) return false;

    if (window) {
        glfwDestroyWindow(window);
        window = NULL;
    }
    glfwTerminate();
    janela_iniciada = false;
    return true;
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
    // Define um pequeno padding para evitar que a borda sobreponha os widgets
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, TAMANHO_BORDA_JANELA);

    beginFullscreenWindow("Main");
    ImGui::SetWindowFontScale(ESCALA_FONTE_MENU);

    // Layout para o primeiro botão e informações do validador
    ImGui::Text(" Escolha um botão:");
    if (ImGui::Button("Rodar Sistema", ImVec2(TAMANHO_BOTAO_LARG, TAMANHO_BOTAO_ALT))) {
        selected_option = 0;
    }

    ImGui::SameLine(0, ESPACO_ENTRE_BOTOES);
    ImGui::Text("L. %s\nFAB.: %s\nVAL.: %s", 
                validator.GetLT().c_str(), 
                validator.GetFAB().c_str(), 
                validator.GetVAL().c_str());

    // Espaçamento entre a primeira linha de elementos e a segunda
    ImGui::Dummy(ImVec2(0, ESPACO_ENTRE_BOTOES));

    // Layout para o segundo botão
    if (ImGui::Button("Atualizar Data", ImVec2(TAMANHO_BOTAO_LARG, TAMANHO_BOTAO_ALT))) {
        selected_option = 1;
    }

    ImGui::SameLine(0, ESPACO_ENTRE_BOTOES);

    // Lógica do botão "Imprimir" com mudança de cor
    bool style_pushed_for_print_button = false;
    if (imprimindo) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 0.2f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.7f, 0.3f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.5f, 0.1f, 1.0f));
        style_pushed_for_print_button = true;
    }

    if (ImGui::Button("Imprimir", ImVec2(TAMANHO_BOTAO_LARG, TAMANHO_BOTAO_ALT))) {
        selected_option = 2;
        imprimindo = !imprimindo;
    }

    if (style_pushed_for_print_button) {
        ImGui::PopStyleColor(3);
    }

    ImGui::Dummy(ImVec2(0, ESPACO_ENTRE_BOTOES));

    if (ImGui::Button("Configurar", ImVec2(TAMANHO_BOTAO_LARG / 2, TAMANHO_BOTAO_ALT / 5))) {
        selected_option = 5;
    }

    ImGui::End();
    ImGui::PopStyleVar(2);
}


bool Interface::atualizar_frame(const cv::Mat& frame) {
    if (frame.empty()) return false;

    cv::Mat resized_frame;
    const int max_display_width = 1920; // Largura máxima para exibição
    
    // Redimensionar se necessário usando GPU
    if (frame.cols > max_display_width) {
        double scale = static_cast<double>(max_display_width) / frame.cols;
        cv::resize(frame, resized_frame, cv::Size(), scale, scale, cv::INTER_LINEAR);
    } else {
        resized_frame = frame;
    }

    const int frame_width = resized_frame.cols;
    const int frame_height = resized_frame.rows;
    const int data_size = frame_width * frame_height * resized_frame.elemSize();

    if (!texture_id || frame_width != last_width || frame_height != last_height) {
        if (texture_id) glDeleteTextures(1, &texture_id);
        if (pboInitialized) {
            glDeleteBuffers(2, pboIds);
            pboInitialized = false;
        }

        // Criar textura com formato compactado
        glGenTextures(1, &texture_id);
        glBindTexture(GL_TEXTURE_2D, texture_id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGB, frame_width, frame_height, 
                     0, GL_BGR, GL_UNSIGNED_BYTE, nullptr);

        // Criar os PBOs
        glGenBuffers(2, pboIds);
        for (int i = 0; i < 2; ++i) {
            glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboIds[i]);
            glBufferData(GL_PIXEL_UNPACK_BUFFER, data_size, nullptr, GL_STREAM_DRAW);
        }
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

        last_width = frame_width;
        last_height = frame_height;
        pboInitialized = true;
    }

    if (!pboInitialized) return false;

    pboIndex = (pboIndex + 1) % 2;
    int nextIndex = (pboIndex + 1) % 2;

    // Upload do frame anterior (do PBO "pronto")
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboIds[pboIndex]);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, frame_width, frame_height, GL_BGR, GL_UNSIGNED_BYTE, 0);

    // Mapear o próximo PBO e copiar os dados do frame atual
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboIds[nextIndex]);
    if (!pboInitialized || data_size != last_data_size) {
        glBufferData(GL_PIXEL_UNPACK_BUFFER, data_size, nullptr, GL_STREAM_DRAW);
        last_data_size = data_size;
    }
    void* ptr = glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
    if (ptr) {
        memcpy(ptr, resized_frame.data, data_size);
        glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
    }
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    glFlush(); // Sincronização assíncrona

    // ImGui - exibindo textura
    beginFullscreenWindow("Camera View");

    float aspect_ratio = static_cast<float>(frame_width) / frame_height;
    float display_width = ImGui::GetIO().DisplaySize.x * 0.75f * resolution_scale;
    float display_height = display_width / aspect_ratio;

    ImGui::SetCursorPosX((ImGui::GetWindowWidth() - display_width) * 0.5f);
    ImGui::SetCursorPosY((ImGui::GetWindowHeight() - display_height) * 0.5f);
    ImGui::Image((ImTextureID)(intptr_t)texture_id, ImVec2(display_width, display_height));

    // Controle de escala de resolução
    ImGui::SetNextItemWidth(200);
    //ImGui::SliderFloat("Escala", &resolution_scale, 0.5f, 2.0f);

    ImGui::SetWindowFontScale(ESCALA_FONTE_MENU);
    bool return_to_menu = ImGui::Button("Voltar ao Menu", ImVec2(TAMANHO_BOTAO_PEQUENO_LARG, TAMANHO_BOTAO_PEQUENO_ALT));

    ImGui::End();
    return return_to_menu;
}


std::string Interface::FormatDate(int day, int month, int year) {
    static const char* month_names[] = {
        "JAN", "FEV", "MAR", "ABR", "MAI", "JUN",
        "JUL", "AGO", "SET", "OUT", "NOV", "DEZ"
    };
    
    char buffer[20];
    snprintf(buffer, sizeof(buffer), "%02d/%s/%d", day, month_names[month], year);
    return buffer;
}

bool Interface::requisitar_lt(std::string& selected_lt) {
    static int lotes[101];
    static int anos[50];
    static bool initialized = false;
    
    if (!initialized) {
        for (int i = 0; i <= 100; ++i) lotes[i] = i;
        for (int i = 0; i < 50; ++i) anos[i] = 23 + i;
        initialized = true;
    }

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);   
    beginFullscreenWindow("LotePage");
    
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, PADDING_FRAME);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, SPACING_ITEM);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, SPACING_INTERNO);
    ImGui::SetWindowFontScale(ESCALA_FONTE_DATA);

    ImGui::Text(" Insira o valor do lote: ");
    ImGui::Text(" L. ");
    ImGui::SameLine(0, ESPACO_ENTRE_BOTOES);
    
    char lote_label[4];
    snprintf(lote_label, sizeof(lote_label), "%03d", lotes[selected_lote]);
    ImGui::SetNextItemWidth(COMBO_LARGURA);
    if (ImGui::BeginCombo("##lote", lote_label)) {
        for (int i = 0; i <= 100; ++i) {
            char lote_item[4];
            snprintf(lote_item, sizeof(lote_item), "%03d", i);
            if (ImGui::Selectable(lote_item, selected_lote == i)) {
                selected_lote = i;
            }
        }
        ImGui::EndCombo();
    }
    
    ImGui::SameLine(0, ESPACO_ENTRE_BOTOES);
    char ano_label[3];
    snprintf(ano_label, sizeof(ano_label), "%02d", anos[selected_ano]);
    ImGui::SetNextItemWidth(COMBO_LARGURA);
    if (ImGui::BeginCombo("##ano", ano_label)) {
        for (int i = 0; i < 50; ++i) {
            char ano_item[3];
            snprintf(ano_item, sizeof(ano_item), "%02d", anos[i]);
            if (ImGui::Selectable(ano_item, selected_ano == i)) {
                selected_ano = i;
            }
        }
        ImGui::EndCombo();
    }

    ImGui::Spacing();
    ImGui::Spacing();

    bool clicked = false;
    if (ImGui::Button("OK", ImVec2(TAMANHO_BOTAO_PEQUENO_LARG, TAMANHO_BOTAO_PEQUENO_ALT * 2))) {
        char result[7];
        snprintf(result, sizeof(result), "%03d/%02d", selected_lote, anos[selected_ano]);
        selected_lt = result;
        clicked = true;
    }

    if (!selected_lt.empty()) {
        ImGui::Spacing();
        ImGui::Text("Lote selecionado: %s", selected_lt.c_str());
    }

    ImGui::PopStyleVar(3);
    ImGui::End();
    return clicked;
}

bool Interface::config_menu(){
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    beginFullscreenWindow("MainPage");
    
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, PADDING_FRAME);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, SPACING_ITEM);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, SPACING_INTERNO);
    ImGui::SetWindowFontScale(ESCALA_FONTE_DATA);

    ImGui::Text(" Escolha uma data de Fabricação:");
    ImGui::PopStyleVar(3);
    ImGui::End();
    return true;
}

bool Interface::requisitar_data(std::string& selected_date, int tipo) {
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

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    beginFullscreenWindow("MainPage");
    
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, PADDING_FRAME);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, SPACING_ITEM);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, SPACING_INTERNO);
    ImGui::SetWindowFontScale(ESCALA_FONTE_DATA);

    if (tipo == 0) ImGui::Text(" Escolha uma data de Fabricação:");
    else ImGui::Text(" Escolha uma data de Validade:");

    ImGui::SetNextItemWidth(COMBO_LARGURA);
    if (ImGui::BeginCombo("##dia", std::to_string(days[selected_day]).c_str())) {
        for (int i = 0; i < 31; ++i) {
            if (ImGui::Selectable(std::to_string(days[i]).c_str(), selected_day == i)) {
                selected_day = i;
            }
        }
        ImGui::EndCombo();
    }
    
    ImGui::SameLine(0, ESPACO_ENTRE_BOTOES);
    ImGui::SetNextItemWidth(COMBO_LARGURA);
    if (ImGui::BeginCombo("##mes", month_names[selected_month])) {
        for (int i = 0; i < 12; ++i) {
            if (ImGui::Selectable(month_names[i], selected_month == i)) {
                selected_month = i;
            }
        }
        ImGui::EndCombo();
    }
    
    ImGui::SameLine(0, ESPACO_ENTRE_BOTOES);
    ImGui::SetNextItemWidth(COMBO_LARGURA);
    if (ImGui::BeginCombo("##ano", std::to_string(years[selected_year]).c_str())) {
        for (int i = 0; i < 100; ++i) {
            if (ImGui::Selectable(std::to_string(years[i]).c_str(), selected_year == i)) {
                selected_year = i;
            }
        }
        ImGui::EndCombo();
    }

    ImGui::Spacing();
    ImGui::Spacing();

    bool clicked = ImGui::Button("OK", ImVec2(TAMANHO_BOTAO_PEQUENO_LARG, TAMANHO_BOTAO_PEQUENO_ALT * 2));
    if (clicked) {
        selected_date = FormatDate(days[selected_day], selected_month, years[selected_year]);
    }

    if (!selected_date.empty()) {
        ImGui::Spacing();
        ImGui::Text("Data selecionada: %s", selected_date.c_str());
    }

    ImGui::PopStyleVar(3);
    ImGui::End();
    return clicked;
}