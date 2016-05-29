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

#ifndef SWITCH_H
#define SWITCH_H

/**
 * Switch-case for generic types (surely, not compile-time).
 * The only caveat is: you have to place 'default' as the last item if you use it,
 * just like with catch (...).
 *
 * Example:
 *
 * Switch (command)
 *  .Case ("Hello", []{print_hello();}),
 *  .Case ("Bye", []{print_bye();})
 *  .Default ([]{print_unknown_cmd();});
 */
template <typename T>
class Switch_t {
    Switch_t (const T& value, bool already_fired = false)
    : value(value), already_fired(already_fired) {}
    Switch_t (const Switch_t&) = default;
    template <typename F>
    Switch_t& Case (const T& sample, F&& func) {
        if (not already_fired and value == sample) {
            func ();
            already_fired = true;
        }
        return *this;
    }
    template <typename F>
    void Default (F&& func) {
        if (not already_fired)
            func();
    }
private:
    const T& value;
    bool already_fired = false;
};

// Employ template type deduction, that does not work on classes (yes, in C++14 too).
template <typename T>
Switch_t<T> Switch (const T& value) {
    return Switch_t<T>(value);
}

#endif // SWITCH_H
