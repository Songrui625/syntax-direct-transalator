#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <vector>

static std::string input;
static char peek;
static size_t idx;

enum class TokenType {
	INTEGER,
	REAL,
	PAREN,
	OPERRATOR
};

class Token {
public:
	Token() = default;

	Token(const std::string lexme, const TokenType type): lexme_(lexme), type_(type) {}

	Token(const Token &other): lexme_(other.lexme_), type_(other.type_) {}

	inline std::string GetLexme() { return lexme_; }

	inline TokenType GetType() { return type_; }

	Token &operator=(const Token &rhs) {
		lexme_ = rhs.lexme_;
		type_ = rhs.type_;
		return *this;
	}

	inline std::string ToString() {
		std::ostringstream os;
		os << "[lexme: " << lexme_ << ", "
		   << "type: ";
		if (type_ == TokenType::INTEGER) {
			os << "INTEGER";
		} else if (type_ == TokenType::REAL) {
			os << "REAL";
		} else if (type_ == TokenType::PAREN) {
			os << "PAREN";
		} else if (type_ == TokenType::OPERRATOR) {
			os << "OPERATOR";
		}
		os << "]";
		return os.str();
	}

private:
	std::string lexme_;
	TokenType type_;
};

class Lexer {
public:
	Lexer() = default;

	Lexer(std::string filename) {
		ifs.open(filename, std::ios::binary | std::ios::in);
		if (!ifs.is_open()) {
			fprintf(stderr, "can not open file: \"%s\" \n", filename.c_str());
		}
	}

	inline void ReadCh() { peek = input[idx++]; }

	inline bool ReadCh(char c) {
		ReadCh();
		if (peek != c) return false;
		peek = ' ';
		return true;
	}

	std::istream &ReadInput();

	Token Scan();

private:
	std::ifstream ifs;
	// std::vector<Token> tokens_;
};