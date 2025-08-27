/**
 * @file UFModel.cpp
 * @author Florian Corzilius <corzilius@cs.rwth-aachen.de>
 * @since 2014-10-24
 * @version 2014-10-24
 */

#include "UFModel.h"

#include "SortValueManager.h"

namespace carl {
bool UFModel::extend(const std::vector<SortValue>& _args, const SortValue& _value) {
    auto ret = emplace(_args, _value);
    assert(ret.second || ret.first->second == _value);  // Checks if the same arguments are not tried to map to different values.
    return ret.second;                                  // Mainly because of not getting a warning, but maybe some needs this return value.
}
SortValue UFModel::get(const std::vector<SortValue>& _args) const {
    auto iter = find(_args);
    if (iter != end()) {
        return iter->second;
    }
    return defaultSortValue(UFManager::getInstance().getCodomain(uf));
}

size_t UFModel::getHash() const {
    std::hash<SortValue> h;
    std::size_t result = 0;
    for (const auto& instance : *this) {
        carl::hash_add(result, h(instance.second));
        for (const auto& arg : instance.first) {
            carl::hash_add(result, h(arg));
        }
    }
    return result;
}

bool UFModel::operator==(const UFModel& _ufm) const {
    auto iterA = begin();
    auto iterB = _ufm.begin();
    while (iterA != end() && iterB != _ufm.end()) {
        if (!(iterA->second == iterB->second))
            return false;
        if (iterA->first != iterB->first)
            return false;
        ++iterA;
        ++iterB;
    }
    return iterA == end() && iterB == _ufm.end();
}

bool UFModel::operator<(const UFModel& _ufm) const {
    auto iterA = begin();
    auto iterB = _ufm.begin();
    while (iterA != end() && iterB != _ufm.end()) {
        if (iterA->second < iterB->second)
            return true;
        if (iterB->second < iterA->second)
            return false;
        if (iterA->first < iterB->first)
            return true;
        if (iterB->first < iterA->first)
            return false;
        ++iterA;
        ++iterB;
    }
    return iterA == end() && iterB != _ufm.end();
}

std::ostream& operator<<(std::ostream& _os, const UFModel& _ufm) {
    assert(!_ufm.empty());
    _os << "(define-fun " << _ufm.uf.name() << " (";
    // Print function signature
    std::size_t id = 1;
    for (const auto& arg : _ufm.uf.domain()) {
        if (id > 1)
            _os << " ";
        _os << "(x!" << id << " " << arg << ")";
        id++;
    }
    _os << ") " << _ufm.uf.codomain() << " ";
    // Print implementation
    for (const auto& instance : _ufm) {
        _os << "(ite (and ";
        std::size_t id = 1;
        for (const auto& param : instance.first) {
            if (id > 0)
                _os << " ";
            _os << "(= x!" << id << " " << param << ")";
            id++;
        }
        _os << ") " << instance.second << " ";
    }
    _os << _ufm.begin()->second;
    for (std::size_t i = 0; i < _ufm.size(); i++) _os << ")";
    _os << ")";
    return _os;
}
}  // namespace carl
