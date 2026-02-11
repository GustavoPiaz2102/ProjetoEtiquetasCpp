#ifndef ARQUIVER_H
#define ARQUIVER_H

#include <string>
#include <map>

class Arquiver{
    private:
        std::string path;

    public:
        std::map<std::string, std::string> dict;
        Arquiver(const std::string& caminho);
        bool load();
        bool save() const;
        void show() const;
};

#endif // ARQUIVER_H
