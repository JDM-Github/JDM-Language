#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>

 void findAndReturn(std::vector<char> &vec, const std::vector<char> &trav) {
    auto it = std::find(trav.begin(), trav.end(), ',');
    if (it != trav.end()) {
        // DO THE FUN STUFF HERE
        // std::vector<char> turnToExpression = { trav.begin(), it };
        vec.push_back('E');
        if (it+1 != trav.end())
            findAndReturn(vec, { it+1, trav.end() });
        return;
    }
    // std::vector<char> turnToExpression = trav;
    vec.push_back('E');
}

int main() {
    std::vector<char> vec;
    findAndReturn(vec, {'1', ',', '1', '+', '4', ',', '5'});
    for (const auto &v: vec)
        std::cout << v << std::endl;
    return 0;
}



