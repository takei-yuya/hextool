#include <stdint.h>
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>

class Bin2Hex {
public:
	Bin2Hex(std::istream &in, std::ostream &out, std::ostream &err)
		: in_(in), out_(out), err_(err), width_(16)
	{
		buf_ = new char[width_];
		out_ << std::hex;
	}
	~Bin2Hex()
	{
		out_ << std::dec;
		delete [] buf_;
	}
	int run()
	{
		return PrintLine();
	}

private:
	int PrintLine()
	{
		for (size_t address = 0; !in_.eof(); address += width_) {
			in_.read((char*)buf_, sizeof(uint8_t) * width_);
			int doRead = in_.gcount();
			PrintAddress(address);
			for (int i = 0; i < width_; ++i) {
				if (i == width_ / 2) {
					PrintSpaces(1);
				}
				if (i < doRead) {
					PrintSpaces(1);
					PrintHex(buf_[i]);
				} else {
					PrintSpaces(1);
					PrintSpaces(2);
				}
			}
			PrintDelim();
			for (int i = 0; i < width_; ++i) {
				if (i < doRead) {
					PrintPrintable(buf_[i]);
				} else {
					PrintSpaces(1);
				}
			}
			out_ << std::endl;
		}
		return 0;
	}
	void PrintAddress(size_t address)
	{
		out_
			<< std::setw(0) << std::setfill(' ') << "/* "
			<< std::setw(sizeof(address)) << std::setfill('0') << address
			<< std::setw(0) << std::setfill(' ') << " */"
			;
	}
	void PrintDelim()
	{
		out_
			<< " // "
			;
	}
	void PrintSpaces(int num)
	{
		out_
			<< std::setw(num) << std::setfill(' ') << std::hex << ""
			;
	}
	void PrintHex(int byte)
	{
		out_
			<< std::setw(2) << std::setfill('0') << (byte & 0xff)
			;
	}
	void PrintPrintable(char ch)
	{
		if (isprint(ch)) {
			out_
				<< std::setw(0) << std::setfill(' ') << ch
				;
		} else if (isspace(ch)) {
			out_
				<< std::setw(0) << std::setfill(' ') << ' '
				;
		} else {
			out_
				<< std::setw(0) << std::setfill(' ') << '.'
				;
		}
	}

private:
	std::istream &in_;
	std::ostream &out_;
	std::ostream &err_;

	int width_;
	char *buf_;
};

int main( int argc, char** argv )
{
	int i;
	for( i = 1; i < argc; ++i ) {
		std::ifstream ifs( argv[i] );
		Bin2Hex b2h(ifs, std::cout, std::cerr);
		b2h.run();
		ifs.close();
	}
	if( i == 1 ) {
		Bin2Hex b2h(std::cin, std::cout, std::cerr);
		b2h.run();
	}
	return EXIT_SUCCESS;
}
