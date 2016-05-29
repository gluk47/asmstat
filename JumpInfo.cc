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

#include "JumpInfo.h"
#include "DumpLine.h"

void JumpInfo::addFrom(DumpLine* f) {
    m_from.push_back(f);
    update_cache(f);
}

void JumpInfo::setTarget(DumpLine* t) {
    m_to = t;
    update_cache(t);
}

uint64_t JumpInfo::length() const {
    if (m_first_line == nullptr or m_last_line == nullptr)
        return 0;
    return m_last_line->line_number() - m_first_line->line_number() + 1;
}

uint64_t JumpInfo::pc_begin() const {
    if (m_first_line == nullptr)
        return 0;
    return m_first_line->address;
}

uint64_t JumpInfo::pc_end() const {
    if (m_last_line == nullptr)
        return 0;
    return m_last_line->address;
}

void JumpInfo::update_cache(DumpLine* new_line) const {
    if (not m_first_line or m_first_line->address > new_line->address)
        m_first_line = new_line;
    if (not m_last_line or m_last_line->address < new_line->address)
        m_last_line = new_line;
}
