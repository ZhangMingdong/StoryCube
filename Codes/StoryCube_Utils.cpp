#include <vector>
#include <string>

using namespace std;

/*
* get the index of str in list
*/
int getStrIndex(const vector<string>& list, const string& str) noexcept {
    const auto it = std::find(list.cbegin(), list.cend(), str);
    return it != list.cend() ? static_cast<int>(std::distance(list.cbegin(), it)) : -1;
}