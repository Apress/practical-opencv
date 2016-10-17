#include <boost/filesystem.hpp>

using namespace std;
using namespace boost::filesystem3;

int main() {
    string PATH("/home/samarthb/test");
    for(recursive_directory_iterator i = recursive_directory_iterator(PATH), end_iter; i != end_iter; i++) {
        cout << (i -> path()).filename().string() << " " << i.level() << endl;
    }

    return 0;
}
