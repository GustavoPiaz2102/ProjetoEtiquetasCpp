#include "../heaters/validator.h"
Validator::Validator(std::string lt, std::string fab, std::string val)
    : LT(lt), FAB(fab), VAL(val) {}
bool Validator::ValidateLT(std::string str) {
    if (str.find(LT) != std::string::npos) {
        return true;
    }
    else {
        return false;
    }
}
bool Validator::ValidateFAB(std::string str) {
    if (str.find(FAB) != std::string::npos) {
        return true;
    }
    else {
        return false;
    }
}
bool Validator::ValidateVAL(std::string str) {
    if (str.find(VAL) != std::string::npos) {
        return true;
    }
    else {
        return false;
    }
}
bool Validator::Validate(std::string str) {
    if (ValidateLT(str) && ValidateFAB(str) && ValidateVAL(str)) {
        return true;
    }
    else {
        return false;
    }
}
std::string Validator::GetLT(){ return LT; }
std::string Validator::GetFAB(){ return FAB; }
std::string Validator::GetVAL(){ return VAL; }
void Validator::SetLT(std::string& lt) { LT = lt; }
void Validator::SetFAB(const std::string& fab) {
    FAB = fab;
}
void Validator::SetVAL(const std::string& val) {
    VAL = val;
}
void Validator::printall(){
    std::cout<< "\n" << "L: " << LT << "\n"
              << "F: " << FAB << "\n"
              << "V: " << VAL << "\n";
}
