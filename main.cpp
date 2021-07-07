#include "parser.h"
#include "lexer.h"
#include <string>
#include <assert.h>

int main() {
	Parser p("input");
	std::cout << "Lexer scan begin..." << std::endl;
	p.ReadTokens();
	std::string filename = "grammar.txt";
	std::cout << "\nReading grammar" << std::endl;
	p.ReadGrammar(filename);
	std::cout << p.ToString(p.GetGrammar()) << std::endl;
	std::cout << "Parser begin..." << std::endl;
	p.Output();
	std::cout << "Parser done..." << std::endl;
	return 0;
}