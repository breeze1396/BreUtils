#include "flag.hpp"


int main() {
	const char* argv[] = { "testFlag","-str","brez","-intVar","1936", "--bool","T","-bool2", "-int", "-10", "-uint", "20", "-int64", "30", "-uint64", "40"};
	int argc = sizeof(argv) / sizeof(argv[0]);


	Flag flag("testFlag", "test flag");

	bool* b = flag.Bool("bool", true, "bool flag");
	bool* b2 = flag.Bool("bool2", false, "bool flag2");
	int* i = flag.Int("int", 0, "int flag");
	unsigned int* ui = flag.Uint("uint", 0, "uint flag");
	int64_t* i64 = flag.Int64("int64", 0, "int64 flag");
	uint64_t* ui64 = flag.Uint64("uint64", 0, "uint64 flag");
	int num1 = 0;
	flag.Int(&num1, "intVar", 0, "intVar flag");
	std::string str = "";
	flag.String(&str, "str", "breeze", "string flag");
	flag.Parse(argc, (char**)argv);


	auto t = flag.VisiedAll();
	for (auto& it : t) {
		it->PrintInfo();
		std::cout << "\n";
	}

	std::cout << "bool: " << *b << "\n";
	std::cout << "bool2: " << *b2 << "\n";
	std::cout << "int: " << *i << "\n";
	std::cout << "uint: " << *ui << "\n";
	std::cout << "int64: " << *i64 << "\n";
	std::cout << "uint64: " << *ui64 << "\n";
	std::cout << "intVar: " << num1 << "\n";
	std::cout << "str: " << str << "\n";



	int nargc = flag.Narg();
	int nflag = flag.Nflag();
	for (int i = 0;i < nargc; ++i) {
		std::cout << "²ÎÊý" << i << ": " << flag.Arg(i) << "\n";
	}

	return 0;
}