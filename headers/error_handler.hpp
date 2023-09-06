#pragma once
#include <exception>
#include <string>

#define J_CTOKENSR const std::string&
#define J_CFILENR  const std::string&
#define J_TOKENS   std::string
#define J_FILE     std::string
#define J_MESSAGE  std::string

class JDMTokenizingHandler : public std::exception {
public:
	JDMTokenizingHandler(J_CFILENR filename, J_MESSAGE message, J_CTOKENSR lastToken, J_CTOKENSR currToken, size_t row, size_t col)
		: __file(filename), __message(message), __lastTok(lastToken), __currTok(currToken), __row(row), __col(col) {}

	const char* what() const noexcept override {
		std::string fullMessage = (
			"\n\nCaught Error on Tokenizing: " + this->__message + "\n > "
			+ this->__file + ":" + std::to_string(this->__row) + ":"
			+ std::to_string(this->__col) + "\n\t -> " + this->__lastTok + " " + this->__currTok
			+ "\n\n"
		);
        return fullMessage.c_str();
	}

private:
	std::string __file;
	std::string __message;
	size_t __row, __col;
	J_TOKENS __lastTok, __currTok;
};

class UnterminatedString : public JDMTokenizingHandler {
public:
	UnterminatedString(J_CFILENR filename, J_CTOKENSR lastToken, J_CTOKENSR currToken, size_t row, size_t col)
		: JDMTokenizingHandler(filename, "Unterminated String", lastToken, currToken, row, col) {}
};

class UnexpectedCharacter : public JDMTokenizingHandler {
public:
	UnexpectedCharacter(J_CFILENR filename, J_CTOKENSR lastToken, J_CTOKENSR currToken, size_t row, size_t col)
		: JDMTokenizingHandler(filename, "Unexpected Character", lastToken, currToken, row, col) {}
};

class UnrecognizeToken : public JDMTokenizingHandler {
public:
	UnrecognizeToken(J_CFILENR filename, J_CTOKENSR lastToken, J_CTOKENSR currToken, size_t row, size_t col)
		: JDMTokenizingHandler(filename, "Unrecognize Token", lastToken, currToken, row, col) {}
};

class IllegalEscape : public JDMTokenizingHandler {
public:
	IllegalEscape(J_CFILENR filename, J_CTOKENSR lastToken, J_CTOKENSR currToken, size_t row, size_t col)
		: JDMTokenizingHandler(filename, "Illegal Escape Sequence", lastToken, currToken, row, col) {}
};