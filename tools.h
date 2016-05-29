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
 * The code in this file has various origins, see the comments below.
 */

#pragma once

#include <string>
#include <sstream>
#include <vector>

/**
 * @brief Split string by a delimiter into elements of a container
 * @credits http://stackoverflow.com/a/236803/1266605
 */
template <template <class, class> class container_t = std::vector,
          class string_t = std::string,
          class allocator_t = std::allocator<string_t>>
container_t<string_t, allocator_t> &split(const string_t& s,
                             char delim,
                             container_t <string_t, allocator_t>& elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

/**
 * @brief Split string by a delimiter into elements of a container
 * @credits http://stackoverflow.com/a/236803/1266605
 */
template <template <class, class> class container_t = std::vector,
          class string_t = std::string,
          class allocator_t = std::allocator<string_t>>
container_t<string_t, allocator_t> split(const string_t& s, char delim) {
    container_t <string_t, allocator_t> elems;
    return split(s, delim, elems);
}
