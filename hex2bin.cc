#include <stdint.h>
#include <iostream>
#include <fstream>
#include <sstream>

class Hex2Bin {
public:
	Hex2Bin(std::istream &in, std::ostream &out, std::ostream &err)
		: in_(in), out_(out), err_(err), buf_(0)
	{
	}
	int Run()
	{
		buf_ = 0;
		return ParseNormal();
	}

private:
	int ParseError()
	{
		return ParseError("Unknown error.");
	}
	int ParseError(const std::string& msg)
	{
		err_ << msg << std::endl;
		return -1;
	}
	int ParseErrorUnexpectedChar(char ch)
	{
		std::ostringstream ss;
		ss << "Unexpected char '" << (char)ch << "'(" << (int)ch << ") in normal mode.";
		return ParseError(ss.str());
	}
	int ParseErrorUnexpectedEOF(int line)
	{
		std::ostringstream ss;
		ss << "Unexpected EOF at " << line << ".";
		return ParseError(ss.str());
	}
	int ParseSuccess()
	{
		return 0;
	}

	int ParseNormal()
	{
		int16_t ch = Input();
		if (ch < 0) {
			return ParseSuccess();
		} else if (isspace(ch)) {
			return ParseNormal();
		} else if (isxdigit(ch)) {
			SetBuf(Char2Int(ch));
			return ParseHex();
		} else if (ch == '\\') {
			return ParseLiterally();
		} else if (ch == '/') {
			return ParseStartComment();
		} else {
			return ParseErrorUnexpectedChar(ch);
		}
	}
	int ParseHex()
	{
		int16_t ch = Input();
		if (ch < 0) {
			return ParseErrorUnexpectedEOF(__LINE__);
		} else if (isspace(ch)) {
			// MEMO: hexは連続しなくてもよいか？
			// return ParseHex();
			return ParseErrorUnexpectedChar(ch);
		} else if (isxdigit(ch)) {
			SetBuf(GetBuf() * 0x10 + Char2Int(ch));
			Output();
			return ParseNormal();
		} else {
			return ParseErrorUnexpectedChar(ch);
		}
	}
	int ParseStartComment()
	{
		int16_t ch = Input();
		if (ch < 0) {
			return ParseErrorUnexpectedEOF(__LINE__);
		} else if (ch == '/') {
			return ParseOnelineComment();
		} else if (ch == '*') {
			return ParseMultilineComment();
		} else {
			return ParseErrorUnexpectedChar(ch);
		}
	}
	int ParseOnelineComment()
	{
		int16_t ch = Input();
		if (ch < 0) {
			return ParseSuccess();
		} else if (ch == '\n') {
			return ParseNormal();
		} else if (ch == '\r') {
			return ParseNormal();
		} else {
			return ParseOnelineComment();
		}
	}
	int ParseMultilineComment()
	{
		int16_t ch = Input();
		if (ch < 0 ) {
			// MEMO: マルチラインコメント中のEOFはエラーにすべき？
			// return ParseErrorUnexpectedEOF(__LINE__);
			return ParseSuccess();
		} else if (ch == '*') {
			return ParseEndComment();
		} else {
			return ParseMultilineComment();
		}
	}
	int ParseEndComment()
	{
		int16_t ch = Input();
		if (ch < 0) {
			return ParseErrorUnexpectedEOF(__LINE__);
		} else if (ch == '/') {
			return ParseNormal();
		} else {
			return ParseMultilineComment();
		}
	}
	int ParseLiterally()
	{
		int16_t ch = Input();
		if (ch < 0) {
			return ParseErrorUnexpectedEOF(__LINE__);
		} else {
			SetBuf(ch);
			Output();
			return ParseNormal();
		}
	}

private:
	inline int Char2Int(char ch)
	{
		if ('0' <= ch && ch <= '9') {
			return ch - '0' + 0x0;
		} else if ('a' <= ch && ch <= 'f') {
			return ch - 'a' + 0xa;
		} else if ('A' <= ch && ch <= 'F') {
			return ch - 'A' + 0xA;
		}
		return -1;
	}

	int16_t Input()
	{
		char ch;
		if (!in_.get(ch)) {
			return -1;
		}
		return ch;
	}
	uint8_t GetBuf()
	{
		return buf_;
	}
	void SetBuf(uint8_t i)
	{
		buf_ = i;
	}
	void Output()
	{
		out_.put(buf_);
		buf_ = 0;
	}

private:
	std::istream &in_;
	std::ostream &out_;
	std::ostream &err_;

	uint8_t buf_;
};

int main(int argc, char **argv)
{
	int i;
	for (i = 1; i < argc; ++i) {
		const char *filename = argv[i];
		std::ifstream ifs(filename);
		if (!ifs.good()) {
			return EXIT_FAILURE;
		}
		Hex2Bin h2b(ifs, std::cout, std::cerr);
		h2b.Run();
		ifs.close();
	}
	if (i == 1) {
		Hex2Bin h2b(std::cin, std::cout, std::cerr);
		h2b.Run();
	}
	return EXIT_SUCCESS;
}
