/// \file hashtable.hpp
/// \brief Самописная хэш-таблица с цепочками + полиномиальный хэш строки
///
/// Хэш-функция — полиномиальная по основанию P=31 с автоматическим
/// усечением до 64 бит
/// Разрешение коллизий — метод цепочек (бакет = std::vector)
/// Рехэширование при load_factor > MAX_LOAD до ближайшего простого
#pragma once

#include "../teacher.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace pt::search {

class HashTable {
public:
    explicit HashTable(std::size_t initial = 16) {
        buckets_.resize(next_prime(initial));
    }

    /// \brief Полиномиальный хэш строки: h = sum(s[i] * P^i) mod 2^64
    static std::uint64_t hash_str(const std::string& s) noexcept {
        std::uint64_t h = 0;
        std::uint64_t p = 1;
        for (unsigned char c : s) {
            h += static_cast<std::uint64_t>(c) * p;
            p *= P;
        }
        return h;
    }

    /// \brief Вставка (ключ, объект); дубликаты допустимы
    /// \return true если эта вставка вызвала коллизию (бакет был непуст)
    bool insert(const std::string& key, const Teacher* obj) {
        if (load_factor() > MAX_LOAD) rehash(buckets_.size() * 2 + 1);
        std::size_t idx = hash_str(key) % buckets_.size();
        bool collision = !buckets_[idx].empty();
        buckets_[idx].push_back({key, obj});
        ++size_;
        if (collision) ++collisions_;
        return collision;
    }

    std::vector<const Teacher*> find_all(const std::string& key) const {
        std::vector<const Teacher*> out;
        if (buckets_.empty()) return out;
        const auto& bucket = buckets_[hash_str(key) % buckets_.size()];
        for (const auto& e : bucket) if (e.key == key) out.push_back(e.obj);
        return out;
    }

    std::size_t size() const noexcept { return size_; }
    std::size_t bucket_count() const noexcept { return buckets_.size(); }
    std::size_t collisions() const noexcept { return collisions_; }
    double load_factor() const noexcept {
        return buckets_.empty() ? 0.0
            : static_cast<double>(size_) / static_cast<double>(buckets_.size());
    }

private:
    static constexpr std::uint64_t P = 31;
    static constexpr double MAX_LOAD = 0.75;

    struct Entry {
        std::string key;
        const Teacher* obj;
    };

    std::vector<std::vector<Entry>> buckets_;
    std::size_t size_ = 0;
    std::size_t collisions_ = 0;

    void rehash(std::size_t new_size) {
        std::size_t prime = next_prime(new_size);
        std::vector<std::vector<Entry>> old;
        old.swap(buckets_);
        buckets_.assign(prime, {});
        size_ = 0;
        collisions_ = 0;
        for (auto& b : old)
            for (auto& e : b)
                insert(e.key, e.obj);
    }

    static bool is_prime(std::size_t n) {
        if (n < 4) return n >= 2;
        if (n % 2 == 0) return false;
        for (std::size_t i = 3; i * i <= n; i += 2)
            if (n % i == 0) return false;
        return true;
    }

    static std::size_t next_prime(std::size_t n) {
        if (n < 2) return 2;
        if (n % 2 == 0) ++n;
        while (!is_prime(n)) n += 2;
        return n;
    }
};

} // namespace pt::search
