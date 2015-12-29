#include <iostream>
#include <cmath>

int main(int argc, char const *argv[]) {
	unsigned int value = 65333;
	std::cout << sizeof(value) << "\n";
	unsigned char *array = (unsigned char *)calloc(8*sizeof(value), sizeof(char));
	for(int i = 31; i >=0; i--) {
		*array = (unsigned char)((value >> (i)) & 1) + '0';
		std::cout << *array++;
	}
	std::cout << "\n";
}