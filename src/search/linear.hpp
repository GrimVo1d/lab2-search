/// \file linear.hpp
/// \brief Линейный поиск всех вхождений по ключу-ФИО
#pragma once

#include "../teacher.hpp"
#include <string>
#include <vector>

namespace pt::search {

/// \brief Линейный проход по массиву; возвращает указатели на все
/// объекты с заданным ФИО; сложность O(n)
inline std::vector<const Teacher*> linear_find(
        const std::vector<Teacher>& v, const std::string& key) {
    std::vector<const Teacher*> out;
    for (const auto& t : v) if (t.fio == key) out.push_back(&t);
    return out;
}

} // namespace pt::search
