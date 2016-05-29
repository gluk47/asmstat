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

#ifndef DUMPLINE_H
#define DUMPLINE_H

#include <string>
#include "Command.h"

class DumpLine {
public:
    DumpLine () = default;
    DumpLine (std::string src, int line_no);
    /// Get information from the dump line src
    void parse (std::string src, int line_no);

    const std::string get_src () const& { return m_src; }
    std::string get_src () && { return std::move(m_src); }

    unsigned line_number () const { return m_line_number; }

    /// If it's a jump
    bool is_jump () const noexcept { return m_cmd.jump_address != 0; }
    DumpLine* jump_target () const& { return m_jumpsTo; }
    DumpLine*& jump_target () & { return m_jumpsTo; }
    uint64_t jump_target_address () const { return m_cmd.jump_address; }
    bool& is_jump_target () & { return m_is_jump_target; }
    bool is_jump_target () const& { return m_is_jump_target; }

    const Command& command () const { return m_cmd; }
    // self address
    uint64_t address;
private:
    /// Line number in the original dump
    unsigned m_line_number;
    /// If this is a jump, then m_jumpsTo is the target
    DumpLine* m_jumpsTo = nullptr;
    /// original source (objdump) line
    std::string m_src;
    /// Whether there exists a jump to this line
    bool m_is_jump_target = false;
    /// Update all the internal data base on m_src (really parse it)
    void update_internal ();
    Command m_cmd;
};

std::ostream& operator<< (std::ostream& str, const DumpLine& dl);

#endif // DUMPLINE_H
