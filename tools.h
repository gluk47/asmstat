#pragma once

#include <string>
#include <sstream>
#include <vector>

// credits: http://stackoverflow.com/a/236803/1266605

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


template <template <class, class> class container_t = std::vector,
          class string_t = std::string,
          class allocator_t = std::allocator<string_t>>
container_t<string_t, allocator_t> split(const string_t& s, char delim) {
    container_t <string_t, allocator_t> elems;
    return split(s, delim, elems);
}
