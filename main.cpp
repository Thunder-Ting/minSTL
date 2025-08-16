#include <iostream>
#include <vector.h>
#include <string>
#include <vector>
using std::cout;
using std::endl;
using std::string;

struct KV {
    string key;
    string value;
};

int main() {
    // leistd::vector<KV> v;
    std::vector<int> v = {1, 2, 3};
    v.insert(v.begin(), 4);
    v.insert(v.begin() + 1, {99, 99, 99});

    return 0;
}
