#include "utility.h"

namespace test {
    template <>
    bool VectorsEqual<std::u8string>(const std::vector<std::u8string>& vec1, const std::vector<std::u8string>& vec2) {
        bool result = VectorsEqual_(vec1, vec2);
        if (!result) {
            std::cout << "  Vector 1: {" << std::endl;
            for (const std::u8string& item: vec1) {
                std::cout << "    [";
                std::copy(item.begin(), item.end(), std::ostream_iterator<char>(std::cout, ""));
                std::cout << "]" << std::endl;
            }
            std::cout << std::endl << "  }" << std::endl;
            std::cout << "  Vector 2: {" << std::endl;
            for (const std::u8string& item: vec2) {
                std::cout << "    [";
                std::copy(item.begin(), item.end(), std::ostream_iterator<char>(std::cout, ""));
                std::cout << "]" << std::endl;
            }
            std::cout << std::endl << "  }" << std::endl;
        }
        return result;
    }
}
