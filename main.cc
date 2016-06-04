/*
 * Asm stat tool for reading objdumps
 * Copyright © 2016  gluk47@gmail.com
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * The code in this file is fully authored by Egor Kochetov <gluk47@gmail.com>
 * Feel free to contact him if you need this file with GPL restrictions waived.
 */

#include <algorithm>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <type_traits>

#include "DumpLine.h"
#include "JumpInfo.h"

using namespace std;

/** Extract data from the dump stream.
 * @param[in,out] dump the stream with data. It is exhausted when the function returns.
 * @param[out] lines the list of all the lines in order they are in @c dump.
 *              this list includes lines that are not correct assembly.
 *              This list actually owns all the lines, do not delete it until you want
 *              to free the lines' memory.
 * @param[out] jumps mapping: jump address -> jump info.
 *              an entry is created for both jump_from and jump_to addresses,
 *              hence shared_ptr.
 * @param[out] lines_by_address the list of all valid lines by their code addresses.
 */
void readDump (std::istream& dump,
               list <unique_ptr<DumpLine>>& lines,
               map <uint64_t, shared_ptr <JumpInfo>>& jumps,
               map <uint64_t, DumpLine*>& lines_by_address) {
    map <uint64_t, JumpInfo*> unresolved_jump_targets;
    for (int i = 0; ; ++i) {
        char linebuf [2048];
        dump >> ws;
        dump.getline(linebuf, extent<decltype(linebuf)>());
        if (not dump)
            break;
        lines.emplace_back(make_unique<DumpLine> (linebuf, i));
        auto& newLine = *lines.back();
        const auto& addr = newLine.address;
        if (addr > 0)
            lines_by_address [addr] = &newLine;
        else
            continue;
        if (newLine.is_jump()) {
            const auto& target_addr = newLine.jump_target_address();
            auto jump_info = jumps.find(target_addr);
            if (jump_info != jumps.end()) {
                //add another jump_from to this jump
                jumps[addr] = jump_info->second;
                jump_info->second->addFrom(&newLine);
            } else {
                auto new_jump_info = make_shared<JumpInfo>();
                new_jump_info->addFrom(&newLine);
                jumps [addr] = new_jump_info;
                jumps [target_addr] = new_jump_info;
                auto jump_to = lines_by_address.find(target_addr);
                if (jump_to == lines_by_address.end())
                    unresolved_jump_targets [target_addr] = new_jump_info.get();
                else
                    new_jump_info->setTarget(jump_to->second);
            }
        }
        auto resolve_jump = unresolved_jump_targets.find(addr);
        if (resolve_jump != unresolved_jump_targets.end()) {
            resolve_jump->second->setTarget (&newLine);
            unresolved_jump_targets.erase(resolve_jump);
        }
    }
#ifdef DEBUG
    if (not unresolved_jump_targets.empty()) {
        cerr << "Not all jump targets were resolved. The list of unresolved jumps:\n";
        for (auto j : unresolved_jump_targets)
            for (auto f : j.second->from())
                cerr << f << "\n";
        cerr << "\n";
    }
#endif
}

/// Set up the level value for each jump in jumps.
/// Return the maximal level value among all jumps
int sortJumps (const map <uint64_t, shared_ptr <JumpInfo>>& jumps) {
    vector <JumpInfo*> current_jumps;
    for (auto& j : jumps) {
        JumpInfo& ji = *j.second;
        if (ji.level > -1)
            continue;
        for (size_t l = 0; l < current_jumps.size(); ++l) {
            JumpInfo& cj = *current_jumps[l];
            if (cj.pc_end() < ji.pc_begin()) {
                ji.level = l;
                current_jumps[l] = j.second.get();
                break;
            }
        }
        if (ji.level > -1)
            continue;
        ji.level = current_jumps.size();
        current_jumps.push_back(j.second.get());
    }
    return current_jumps.size();
}

/**
 * @brief Print all the sources annotated with jumps
 *
 * @param lines dump lines, in order
 * @param jumps jumps, sorted by PC address and (thus) by dump line number
 * @param prefix_l how many columns to leave for jumps lines
 *              (how many jumps go through one src line max)
 * @return void
 */
void printLines (list <unique_ptr <DumpLine>>& lines,
                 map <uint64_t, shared_ptr <JumpInfo>>& jumps,
                 int prefix_l) {
    auto next_jump = jumps.begin();
    // C++ vs UTF-8. A terrible story.
    // Since std::string does not support UTF-8 (unlike e.g. QString),
    // I'll use here vector<string>.
    vector <string> prefix (prefix_l + 1, " ");
    auto print_prefix = [&prefix](ostream& str){
        for (const auto& p : prefix)
            str << p;
    };
    for (auto& pline : lines) {
        auto& nj_address = next_jump->first;
        JumpInfo& nj_info = *next_jump->second;
        DumpLine& line = *pline;
        if (line.address == 0
            and find_if (prefix.begin(), prefix.end(), [](string& s){ return (s != " "); }) == prefix.end()) {
            cout << line.get_src() << "\n";
            continue;
        }

        if (next_jump == jumps.end() or line.address < nj_address) {
            print_prefix(cout);
            cout << line.get_src() << "\n";
            continue;
        }
        // hit next jump
        string jmp_prefix;
        for (size_t i = 0; i < nj_info.level; ++i)
            jmp_prefix += prefix [i];
        if (nj_info.pc_begin() == line.address) {
            jmp_prefix += "┌";
            prefix [nj_info.level] = "│";
        } else if (nj_info.pc_end() == line.address) {
            jmp_prefix += "└";
            prefix [nj_info.level] = " ";
        } else {
            jmp_prefix += "├";
            prefix [nj_info.level] = "│";
        }
        size_t hbar = prefix_l - nj_info.level - 1;
        while (hbar --> 0)
            jmp_prefix += "─";
        jmp_prefix +=
            line.address == nj_info.to()->address? "→" : "←";
        cout << jmp_prefix << line.get_src() << "\n";
        ++next_jump;
    }
}

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
    list <unique_ptr <DumpLine>> lines;
    map <uint64_t, shared_ptr <JumpInfo>> jumps;
    map <uint64_t, DumpLine*> lines_by_address;
    readDump(dump, lines, jumps, lines_by_address);
    int max_open_jumps = sortJumps(jumps);
    cerr << "max_open_jumps: " << max_open_jumps << "\n";
    printLines(lines, jumps, max_open_jumps);
    return 0;
}
