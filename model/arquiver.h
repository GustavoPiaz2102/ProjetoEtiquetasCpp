#ifndef ARQUIVER_H
#define ARQUIVER_H
#include <string>
#include <iostream>
#include <fstream>
#define FILE_PATH "data/arquiver.txt"
void load_file(std::string & lt, std::string& fab, std::string& val);
void save_file(const std::string & lt, const std::string& fab, const std::string& val);
#endif