#pragma once
// #ifndef _PARSER_H_
// #define _PARSER_H_
#include "lexer.h"
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include <vector>

static size_t index;
static Token word;
static int32_t next_triple_id = 0;

struct Producer {
	/** the head of the producer. */
	std::string head;
	/** the body of the producer */
	std::vector<std::string> body;

	inline std::string ToString() const {
		std::ostringstream os;
		os << head
		   << " -> ";
		for (const auto &s : body) {
			os << " "
			   << s
			   << " ";
		}
		os << std::endl;
		return os.str();
	}
};

class Triple {
public:
	Triple() = default;

	Triple(char op, int arg1, int arg2): op_(op), arg1_(arg1), arg2_(arg2) {}

	explicit Triple(const Triple &triple): id_(triple.id_), op_(triple.op_), arg1_(triple.arg1_), arg2_(triple.arg2_) {}

	inline int64_t GetArg1() { return this->arg1_; }

	inline int64_t GetArg2() { return this->arg2_; }

	inline char GetOp() { return this->op_; }

	inline void SetId(int32_t id) { id_ = id; }

	Triple &operator=(const Triple &rhs) {
		op_ = rhs.op_;
		arg1_ = rhs.arg1_;
		arg2_ = rhs.arg2_;
		return *this;
	} 

	inline std::string ToString() const {
		std::ostringstream os;
		os << "[op: "   << op_
		   << " arg1: " << arg1_
		   << " arg2: " << arg2_
		   << "]";
		return os.str();
	}

	int32_t id_;
	char op_;
	int64_t arg1_;
	int64_t arg2_;
};

bool operator==(const Triple &lhs, const Triple &rhs);

class Parser {
public:
	Parser() = default;
	Parser(std::string input):lexer_(input) {
		std::string filename = "output"; 
		fs.open(filename, std::ios::binary| std::ios::out);
		if (!fs.is_open()) {
			fprintf(stderr, "can not open output file: %s", filename);
		}
		std::string header = " #     op    arg1    arg2\n";
		fs.write(header.c_str(), header.size());
		fs.flush();
	}

	/** get context free grammar from given file */
	void ReadGrammar(std::string filename);
	/** read token set from lexer */
	void ReadTokens();
	/** look ahead get next word(char)*/
	Token NextWord();
	/** parse the expression from commandline forever and never return */
	bool Parse();
	/** Expr -> Term ExprTail */
	bool ParseExpr();
	/** ExprTail -> + Term ExprTail | - Term ExprTail | null */
	bool ParseExprTail();
	/** Term -> Factor TermTal */
	bool ParseTerm();
	/** TermTail -> * Factor TermTail */
	bool ParseTermTail();
	/** Factor -> (Expr) | num */
	bool ParseFactor();
	/** Output the intermediate and triples table to the file */
	void Output();
	/** output the grammar */
	std::string ToString(const std::list<Producer> &grammar) const;

	inline std::list<Producer> GetGrammar() { return grammar_; }
private:
	Lexer lexer_;
	std::list<Producer> grammar_;
	// the token set from lexer;
	std::vector<Token> tokens_;
	// the table of triples
	std::vector<Triple> triples_;
	// intermediate table
	std::vector<Triple> intermediate_;
	// output file stream
	std::fstream fs;
};


// #endif