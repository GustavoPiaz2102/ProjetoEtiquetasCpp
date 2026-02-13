#ifndef VALIDATOR_H
#define VALIDATOR_H
#include <string>
#include <iostream>
class Validator {
	private:
		std::string LT;
		std::string FAB;
		std::string VAL;
	public:
		Validator(std::string lt, std::string fab, std::string val);
		bool Validate(std::string str);
		bool ValidateLT(std::string str);
		bool ValidateFAB(std::string str);
		bool ValidateVAL(std::string str);
		// Getters and Setters
		std::string GetLT();
		std::string GetFAB();
		std::string GetVAL();
		void SetLT(std::string& lt);
		void SetFAB(const std::string& fab);
		void SetVAL(const std::string& val);

		// Método para imprimir todos os dados
		void printall();


};
#endif