#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <type_traits>

#include "DumpLine.h"

using namespace std;

int run (int i) {
    int iteration;
    while (i > 0) {
        if (i % 2)
            i = 3 * i + 1;
        else
            i /= 2;
        ++iteration;
    }
    return iteration;
}

struct jumpInfo {
    DumpLine* from,
            * to;
    uint64_t length () const {
        return to->line_number() - from->line_number();
    }
};

int main(int argc, char **argv) {
    if (argc < 2) {
        cout << "Usage: " << argv[0] << " filename\n"
                "The file to inspect needs to be a gdb- or objdump-produced\n"
                "disassembly. This tool will print to stdout that file with\n"
                "all jumps drawn in pseudographics.\n";
        return 1;
    }
    ifstream dump (argv[1]);
    if (not dump.is_open()) {
        cerr << "Failed to open the file '" << argv[1] << "'\n";
        return 2;
    }
    list <DumpLine> lines;
    map <uint64_t, jumpInfo> jump_targets;
    map <uint64_t, DumpLine*> lines_by_address;
    for (int i = 0; ; ++i) {
        char linebuf [2048];
        dump >> ws;
        dump.getline(linebuf, extent<decltype(linebuf)>());
        if (not dump)
            break;
//         cerr << "[" << i << "] ";
//         cerr << line << "...";
        lines.emplace_back(linebuf, i);
        if (lines.back().address > 0)
            lines_by_address [lines.back().address] = &lines.back();
    }
    for (auto& line : lines) {
        if (line.is_jump())
            jump_targets [line.jump_target_address()] =
            {&line, lines_by_address[line.jump_target_address()]};
    }
    for (auto& line: lines) {
        if (line.is_jump())
            cout << "—← ";
        else if (jump_targets.count(line.address) > 0)
            cout << "—→ ";
        else if (line.address > 0)
            // if there's no address, then it's smth like a comment
            cout << "   ";
        cout << line.get_src() << "\n";
    }
    return 0;
}
