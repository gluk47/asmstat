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

#include "Command.h"
#include "Switch.h"
#include "tools.h"

using namespace std;

namespace cmd_basenames {
    set <string> jumps = { "j", "jmp" };
}

Command::Command(string fullname)
: m_fullname(move (fullname)) {
    parse_fullname();
}

void Command::parse_fullname() {
    if (m_fullname.empty())
        return;
    if (m_fullname[0] == 'j') {
        if (m_fullname.substr(0, 3) == "jmp")
            m_basename = "jmp";
        else
            m_basename = "j";
    }
}

void Command::parse_args() {
    if (cmd_basenames::jumps.count (m_basename) > 0) {
        if (m_arguments.size() < 1) {
            cerr << "Invalid jump command: not found a required argument to " << m_fullname
                 << "\n";
            return;
        }
        stringstream ss (m_arguments[0]);
        ss >> hex >> jump_address;
    }
}

istream& operator>>(istream& str, Command& cmd) {
    string opname;
    str >> opname;
    cmd.set_fullname(opname);
    str >> ws;
    getline(str, opname, '\n');
    auto args = split<vector>(opname, ',');
    cmd.set_args(args);
    return str;
}

ostream& operator<<(ostream& str, const Command& cmd) {
    str << cmd.fullname();
    if (cmd.jump_address == 0)
        for (const auto& arg : cmd.args())
            str << " " << arg;
    else
        str << " to 0x" << hex << cmd.jump_address;
    return str;
}
