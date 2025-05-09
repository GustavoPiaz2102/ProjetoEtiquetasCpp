#include "interface.h"


int Interface::menu() {
    using namespace nana;
    int escolha = 0;

    form fm;
    fm.caption("Menu Principal");

    button btn_iniciar(fm, "Iniciar Detecção");
    button btn_mudar_data(fm, "Mudar Data");
    button btn_sair(fm, "Sair");

    btn_iniciar.events().click([&] {
        escolha = 1;
        fm.close();
    });

    btn_mudar_data.events().click([&] {
        escolha = 2;
        fm.close();
    });

    btn_sair.events().click([&] {
        escolha = 3;
        fm.close();
    });

    place plc(fm);
    plc.div("<vertical gap=10 margin=20 arrange=[30,30,30] [btn_iniciar][btn_mudar_data][btn_sair]>");
    plc["btn_iniciar"] << btn_iniciar;
    plc["btn_mudar_data"] << btn_mudar_data;
    plc["btn_sair"] << btn_sair;

    fm.show();
    exec();

    return escolha;
}

std::map<std::string, std::string> Interface::mudar_data(std::map<std::string, std::string> data) {
    using namespace nana;
    std::map<std::string, std::string> resultado;

    form fm;
    fm.caption("Mudar Data");

    label lbl_fab(fm, "Data de Fabricação:");
    date_chooser dc_fab(fm);

    label lbl_val(fm, "Data de Validade:");
    date_chooser dc_val(fm);

    label lbl_lote(fm, "Lote:");
    combobox cb_lote(fm);
    for (int i = 1; i <= 100; ++i) {
        std::ostringstream oss;
        oss << std::setfill('0') << std::setw(3) << i << "/24"; // Ex: 006/24
        cb_lote.push_back(oss.str());
    }
    cb_lote.option(0); // seleciona o primeiro por padrão

    button btn_ok(fm, "OK");
    btn_ok.events().click([&] {
        auto fab = dc_fab.value();
        auto val = dc_val.value();

        auto format_date = [](nana::date d) {
            const char* meses[] = {"JAN", "FEV", "MAR", "ABR", "MAI", "JUN",
                                   "JUL", "AGO", "SET", "OUT", "NOV", "DEZ"};
            std::ostringstream oss;
            oss << std::setw(2) << std::setfill('0') << d.day() << "/"
                << meses[d.month() - 1] << "/"
                << d.year();
            return oss.str();
        };

        resultado["fab"] = format_date(fab);
        resultado["val"] = format_date(val);
        resultado["lt"] = cb_lote.text();

        fm.close();
    });

    place plc(fm);
    plc.div(R"(
        <vertical margin=10 gap=5
            <weight=25 <lbl_fab> <dc_fab>>
            <weight=25 <lbl_val> <dc_val>>
            <weight=25 <lbl_lote> <cb_lote>>
            <weight=30 <btn_ok>>
        >
    )");

    plc["lbl_fab"] << lbl_fab;
    plc["dc_fab"] << dc_fab;
    plc["lbl_val"] << lbl_val;
    plc["dc_val"] << dc_val;
    plc["lbl_lote"] << lbl_lote;
    plc["cb_lote"] << cb_lote;
    plc["btn_ok"] << btn_ok;

    fm.show();
    exec();

    return resultado;
}
