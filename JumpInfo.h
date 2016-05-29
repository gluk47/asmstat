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

#include <cstdint>
#include <vector>
class DumpLine;

struct JumpInfo {
    /// where to draw the arrow vertically,
    ///  0 means leftmost, the more — the righter
    short level = -1;

    void addFrom (DumpLine* f);
    void setTarget (DumpLine* t);

    const std::vector <DumpLine*>& from () const& { return m_from; }
    /// The jump target
    DumpLine* to () const { return m_to; }
    /// Length in source code lines, for proper ordering.
    inline uint64_t length () const;
    /// The first address where to draw the jump line
    uint64_t pc_begin () const;
    /// The last address where to draw the jump line
    uint64_t pc_end () const;

    /// The first dump line within the jump range
    const DumpLine* first_line () const { return m_first_line; }
    /// The last dump line within the jump range
    const DumpLine* last_line () const { return m_last_line; }
private:
    /// Which jumps point to this target
    std::vector <DumpLine*> m_from;
    DumpLine* m_to = nullptr;
    mutable DumpLine* m_first_line = nullptr,
                    * m_last_line = nullptr;
    void update_cache(DumpLine* new_line) const;
};
