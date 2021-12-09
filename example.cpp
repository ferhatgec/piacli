#include "include/piacli.hpp"

using pc = piacli;
using ch = pc::chords;

int main(int argc, char const* const* argv) {
    if(argc < 2) {
        std::cout << "piacli - simple piano chord engine (example usage of library, C2 -> C7)\n"
                     "-----\n" <<
                     argv[0] << " [wave or mp3 file]\n";
        return 1;
    }
    piacli init((std::string(argv[1])));
    init.sequence.sequence = std::vector<ch> {
        ch::C7,
        ch::B6, ch::A6, ch::G6, ch::F6, ch::E6, ch::D6, ch::C6,
        ch::B5, ch::A5, ch::G5, ch::F5, ch::E5, ch::D5, ch::C5,
        ch::B4, ch::A4, ch::G4, ch::F4, ch::E4, ch::D4, ch::C4,
        ch::B3, ch::A3, ch::G3, ch::F3, ch::E3, ch::D3, ch::C3,
        ch::B2, ch::A2, ch::G2, ch::F2, ch::E2, ch::D2, ch::C2,
    };
    init.compile_sequence();
}