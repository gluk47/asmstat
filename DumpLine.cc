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

#include "DumpLine.h"
#include "tools.h"

#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

struct line_data_t {
    uint64_t address = 0, offset = 0;
    Command cmd;
};

/// Parse the line from objdump
line_data_t parse_objdump (const vector <string>& data) {
    return {};
}

/// Parse the line from gdb
line_data_t parse_gdb(const vector <string>& data) {
//     cerr << "Parsing gdb line: [" << data[0] << "] " << data[1] << "\n";
    line_data_t ans;
    {
        stringstream ss (data[0]);
        ss >> hex >> ans.address;
        ss.ignore (10, '<');
        ss >> ans.offset;
    }
    {
        stringstream ss (data[1]);
        ss >> ans.cmd;
    }
//     cerr << "@" << hex << ans.address << " (+" << ans.offset << ") " << dec
//          << ans.cmd << "\n";
    return ans;
}

DumpLine::DumpLine(string src, int line_no)
: m_src(move(src)), m_line_number(line_no) {
    update_internal();
}

void DumpLine::parse(string src, int line_no) {
    m_src = move (src);
    m_line_number = line_no;
    update_internal();
}

void DumpLine::update_internal() {
    auto pos = m_src.find(';'); //< ignore comments
    vector <string> fields = split<vector>(m_src.substr(0, pos),
                                           '\t');
    struct ref_this {
        ref_this& operator= (const line_data_t& d) {
            address = d.address;
            cmd = d.cmd;
        }
        uint64_t& address;
        Command& cmd;
    } this_values { this->address, this->m_cmd };
    if (fields.size() == 2)
        this_values = parse_gdb(fields);
    else if (fields.size() == 3)
        this_values = parse_objdump(fields);
    else {
//         cerr << "\n";
    }
}

ostream& operator<<(ostream& str, const DumpLine& dl) {
    return str << "[" << hex << dl.address << dec << "] " << dl.command();
}
