#include <iostream>
#include <string>
#include <ctime>
#include <fstream>

int main() {
    std::ofstream fout;
    fout.open("test.txt");
    srand(time(NULL));
    unsigned long long value;
    for (int j = 0; j < 10000; ++j) {
	std::string input;
        for (int i = 0; i < 256; ++i) {
            char a = 'a' + rand()%('z'-'a');
            input += a;
        }
        value = rand();
        fout << input << ' ' << value << '\n';
    }
    return 0;
}

