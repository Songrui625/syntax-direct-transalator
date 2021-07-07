#include "parser.h"
#include <utility>
#include <iostream>
#include <regex>
#include <stack>
#include <memory>

static std::stack<int> args;

bool operator==(const Triple &lhs, const Triple &rhs) {
	return lhs.op_ == rhs.op_ && lhs.arg1_ == rhs.arg1_ && lhs.arg2_ == rhs.arg2_;
}

void Parser::ReadGrammar(std::string filename) {
	std::ifstream io;
	io.open(filename, std::ios::binary | std::ios::in);
	if (!io.is_open()) {
		fprintf(stderr, "open file failed");
	}
	std::string line;
	std::regex ws_re("\\s+");  // whitespace
	while (std::getline(io, line)) {
		std::vector<std::string> v(std::sregex_token_iterator(line.begin(), line.end(), ws_re, -1), std::sregex_token_iterator());
		// get the producer head
		Producer p;
		p.head = v[0];
		// get the producer body
		int n = v.size();
		for (int i = 2; i < n; ++i) {
			p.body.push_back(v[i]);
		}
		grammar_.push_back(p);
	}
	io.close();
}

void Parser::ReadTokens() {
	Token tok;
	while(lexer_.ReadInput()) {
		tokens_.clear();
		index = 0;
		while (!(tok = lexer_.Scan()).GetLexme().empty()) {
			std::cout << tok.ToString() << std::endl;
			tokens_.push_back(tok);
		}
		Parse();
	}
}

Token Parser::NextWord() {
	if (index < tokens_.size())  {
		return tokens_[index++];
	}
	return Token();
}

bool Parser::Parse() {
	index = 0;
	word = NextWord();
	if (ParseExpr() && NextWord().GetLexme().empty()) {
		return true;
	} else {
		return false;
	}
}

bool Parser::ParseExpr() {
	if (!ParseTerm()) {
		return false;
	} else {
		return ParseExprTail();
	}
}

int calc(const int64_t &num1, const int64_t &num2, const char op) {
	switch (op) {
		case '+': return num1 + num2;
		case '-': return num1 - num2;
		case '*': return num1 * num2;
		case '/': return num1 / num2;
	}
	return 0;
}

bool Parser::ParseExprTail() {
	if (word.GetLexme() == "+" || word.GetLexme() == "-") {
		char op = word.GetLexme()[0];
		word = NextWord();
		if (!ParseTerm()) {
			return false;
		} else {
			// std::cout << op;
			// construct triple
			int64_t arg2 = args.top();
			args.pop();
			int64_t arg1 = args.top();
			args.pop();
			args.push(calc(arg1, arg2, op));
			Triple triple(op, arg1, arg2);
			// look up the triple int the intermediate table firstly
			auto it =  intermediate_.begin();
			for (; it != intermediate_.end(); ++it) {
				if (*it == triple) {
					break;
				}
			}
			// triple was not found in the intermediate table.
			if (it == intermediate_.end()) {
				triple.SetId(next_triple_id++);
				intermediate_.push_back(triple);
				triples_.push_back(triple);
			}
			bool ret = ParseExprTail();
			return ret;
		}
	}
	return true;
}

bool Parser::ParseTerm() {
	if (!ParseFactor()) {
		return false;
	} else {
		return ParseTermTail();
	}
}

bool Parser::ParseTermTail() {
	if (word.GetLexme() == "*" || word.GetLexme() == "/") {
		char op = word.GetLexme()[0];
		word = NextWord();
		if (!ParseFactor()) {
			return false;
		} else {
			// std::cout << op;
			// construct triple
			int64_t arg2 = args.top();
			args.pop();
			int64_t arg1 = args.top();
			args.pop();
			args.push(calc(arg1, arg2, op));
			Triple triple(op, arg1, arg2);
			// look up the triple int the intermediate table firstly
			auto it =  intermediate_.begin();
			for (; it != intermediate_.end(); ++it) {
				if (*it == triple) {
					break;
				}
			}
			// triple was not found in the intermediate table.
			if (it == intermediate_.end()) {
				triple.SetId(next_triple_id++);
				intermediate_.push_back(triple);
				triples_.push_back(triple);
			}
			bool ret = ParseTermTail();
			return ret;
		}
	}
	return true;
}

bool Parser::ParseFactor() {
	if (word.GetLexme() == "(") {
		word = NextWord();
		if (!ParseExpr()) {
			return false;
		} else if (word.GetLexme() != ")") {
			return false;
		}
		word = NextWord();
		return true;
	} else if (isdigit(word.GetLexme()[0])) {
		args.push(word.GetLexme()[0] - '0');
		// std::cout << word;
		word = NextWord();
		return true;
	}
	return false;
}

void Parser::Output() {
	int n = triples_.size();
	int arg1_found, arg2_found;
	for (int i = 0; i < n; ++i) {
		arg1_found = arg2_found = -1;
		std::ostringstream os;
		os.clear();
		os << "(" << i << ")    " << triples_[i].op_;
		for (int j = 0; j < i; ++j) {
			if (triples_[i].arg1_ == calc(intermediate_[j].arg1_, intermediate_[j].arg2_, intermediate_[j].op_)) {
				arg1_found = j;
			}
			if (triples_[i].arg2_ == calc(intermediate_[j].arg1_, intermediate_[j].arg2_, intermediate_[j].op_)) {
				arg2_found = j;
			}
		}
		os << "     ";
		if (arg1_found != -1) {
			os << "(" << intermediate_[arg1_found].id_ << ")";
		} else {
			os << triples_[i].arg1_;
		}
		os << "       ";
		if (arg2_found != -1) {
			os << "(" << intermediate_[arg2_found].id_ << ")";
		} else {
			os << triples_[i].arg2_;
		}
		os << "\n";
		fs.write(os.str().c_str(), os.str().size());
		fs.flush();
		fs.clear();
	}
}

std::string Parser::ToString(const std::list<Producer> &grammar) const {
	std::ostringstream os;
	for (auto &producer : grammar) {
		os << producer.ToString();
	}
	return os.str();
}