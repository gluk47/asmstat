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

#ifndef COMMAND_H
#define COMMAND_H

#include <iostream>
#include <string>
#include <set>
#include <vector>

class Command {
public:
    Command (std::string fullname = "");
    void set_fullname (std::string fullname) {
        m_fullname = std::move (fullname);
        parse_fullname();
    }
    void set_args (std::vector <std::string> new_args) {
        m_arguments.swap(new_args);
        parse_args();
    }
    const std::string& fullname () const& { return m_fullname; }
    const std::string& basename () const& { return m_basename; }
    // Return the suffixes substring (e.g. "nb" for "jnb" or "zbl" for "movzbl")
    std::string suffixes () const {
        return m_basename.size() < m_fullname.size()
             ? m_fullname.substr(m_basename.size() + 1)
             : "";
    }
          std::vector <std::string>& args ()      & { return m_arguments; }
    const std::vector <std::string>& args () const& { return m_arguments; }

    // If the command is a jump, then it's the target
    uint64_t jump_address = 0;
private:
    std::string m_fullname; //< The name with all suffixes
    std::string m_basename; //< The name without suffixes. E.g. "j" for all jumps.
    std::vector <std::string> m_arguments;

    void parse_fullname ();
    void parse_args ();
};

namespace cmd_basenames {
    extern std::set <std::string> jumps;
}

std::istream& operator>> (std::istream&, Command&);
std::ostream& operator<< (std::ostream&, const Command&);

#endif // COMMAND_H
