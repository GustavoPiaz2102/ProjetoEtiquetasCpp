#ifndef ARQUIVER_H
#define ARQUIVER_H

#include <map>
#include <string>

class Arquiver {
	private:
		std::string path;

	public:
		std::map<std::string, std::string> dict;
		Arquiver(const std::string &caminho);
		bool load();
		bool save() const;
		void show() const;
		void fallback();
};

#endif // ARQUIVER_H
