#include <lib/adapters.hpp>
#include <iostream>
#include <vector>

int main() {
	std::vector<int> v = {1, 2, 3, 4, 5, 6};
    for (auto i: v | Filter([](int i){return i % 2;}) | Transform([](int i){return i * i;})){
        std::cout<<i<<'\n';
    }
	return 0;
}