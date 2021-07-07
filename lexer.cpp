#include "lexer.h"
#include <fstream>
#include <iostream>

std::istream &Lexer::ReadInput() {
	std::ostringstream os;
	std::string line;
	std::getline(ifs, line);
	os << line + "\n";	
	// std::cout << os.str();
	input = os.str();
	idx = 0;
	ReadCh();
	return ifs;
}

Token Lexer::Scan() {
	for ( ; ; ReadCh()) {
		if (peek == ' ' || peek == '\t' || peek == '\n' || peek == '\r') continue;
		else break;
	}
	if (isdigit(peek)) {
		int val = 0;
		do {
			val = val * 10 + peek - '0';
			ReadCh();
		} while (isdigit(peek));
		if (peek != '.') {
			return Token(std::to_string(val), TokenType::INTEGER);
		}
	}
	if (peek == '+' || peek == '-' || peek == '*' || peek == '/') {
		std::string s;
		s.push_back(peek);
		ReadCh();
		return Token(s, TokenType::OPERRATOR);
	}
	if (peek == '(' || peek == ')') {
		std::string s;
		s.push_back(peek);
		ReadCh();
		return Token(s, TokenType::PAREN);
	}
	Token tok;
	peek = ' ';
	return tok;
}