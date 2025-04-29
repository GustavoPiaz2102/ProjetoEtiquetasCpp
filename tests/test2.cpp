/*
Programa que faz uma interface gráfica com GTK e printa quando o botão é clicado
*/
#include <gtkmm.h>
#include <iostream>

class MyWindow : public Gtk::Window {
public:
    MyWindow() {
        set_title("Janela com Botões");
        set_default_size(600, 200);

        // Configura os botões
        button.set_label("Clique aqui");
        button2.set_label("Clique aqui 2");

        // Conecta os sinais
        button.signal_clicked().connect(sigc::mem_fun(*this, &MyWindow::on_button_clicked));
        button2.signal_clicked().connect(sigc::mem_fun(*this, &MyWindow::on_button2_clicked));

        // Configura a caixa vertical
        button.set_size_request(300,100);
        button2.set_size_request(300,100);

        box.set_orientation(Gtk::ORIENTATION_VERTICAL);
        box.set_spacing(30);
        box.set_border_width(30);  // substitui set_margin

        // Adiciona os botões à caixa
        box.pack_start(button, Gtk::PACK_SHRINK);
        box.pack_start(button2, Gtk::PACK_SHRINK);

        // Adiciona a caixa à janela
        add(box);
        show_all_children();
    }

protected:
    void on_button_clicked() {
        std::cout << "Botão 1 clicado!" << std::endl;
    }

    void on_button2_clicked() {
        std::cout << "Botão 2 clicado!" << std::endl;
    }

private:
    Gtk::Box box{Gtk::ORIENTATION_VERTICAL}; // container para empilhar os botões
    Gtk::Button button, button2;
};

int main(int argc, char *argv[]) {
    auto app = Gtk::Application::create(argc, argv, "org.exemplo.botao");
    MyWindow window;
    return app->run(window);
}
