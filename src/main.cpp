/// \file main.cpp
/// \brief Точка входа ЛР2: поиск/бенчмарки 5 методов
///
/// Режимы:
///   ./hw2 search FILE.csv "ФИО"
///       загружает CSV, индексирует, ищет ФИО всеми 5 методами и
///       сверяет результаты
///   ./hw2 bench DATA_DIR RESULTS.csv COLLISIONS.csv
///       прогоняет вставку+поиск на наборах из DATA_DIR и пишет
///       таблицы времён и коллизий

#include "csv.hpp"
#include "teacher.hpp"
#include "timing.hpp"

#include "search/linear.hpp"
#include "search/bst.hpp"
#include "search/rbtree.hpp"
#include "search/hashtable.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <random>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace {

double median3(double a, double b, double c) {
    if (a > b) std::swap(a, b);
    if (b > c) std::swap(b, c);
    if (a > b) std::swap(a, b);
    return b;
}

int run_search(const std::string& path, const std::string& key) {
    auto v = pt::read_csv(path);
    std::cerr << "Загружено " << v.size() << " записей\n";

    auto lin = pt::search::linear_find(v, key);

    pt::search::BST bst;
    for (const auto& t : v) bst.insert(t.fio, &t);
    auto by_bst = bst.find_all(key);

    pt::search::RBTree rb;
    for (const auto& t : v) rb.insert(t.fio, &t);
    auto by_rb = rb.find_all(key);

    pt::search::HashTable ht;
    for (const auto& t : v) ht.insert(t.fio, &t);
    auto by_ht = ht.find_all(key);

    std::multimap<std::string, const pt::Teacher*> mm;
    for (const auto& t : v) mm.emplace(t.fio, &t);
    auto rng = mm.equal_range(key);
    std::vector<const pt::Teacher*> by_mm;
    for (auto it = rng.first; it != rng.second; ++it) by_mm.push_back(it->second);

    auto print = [&](const char* name, const std::vector<const pt::Teacher*>& r) {
        std::cout << name << ": " << r.size() << " совпадений\n";
        for (const auto* p : r) std::cout << "  " << *p << '\n';
    };
    print("linear",    lin);
    print("BST",       by_bst);
    print("RB-tree",   by_rb);
    print("HashTable", by_ht);
    print("multimap",  by_mm);

    bool ok = lin.size() == by_bst.size()
           && lin.size() == by_rb.size()
           && lin.size() == by_ht.size()
           && lin.size() == by_mm.size();
    std::cerr << (ok ? "OK: все методы согласованы\n"
                     : "ОШИБКА: расхождение в количестве совпадений\n");
    return ok ? 0 : 3;
}

int run_bench(const std::string& data_dir,
              const std::string& timings_path,
              const std::string& coll_path) {
    std::vector<fs::path> files;
    for (auto& e : fs::directory_iterator(data_dir))
        if (e.is_regular_file() && e.path().extension() == ".csv")
            files.push_back(e.path());
    std::sort(files.begin(), files.end(), [](const fs::path& a, const fs::path& b) {
        return fs::file_size(a) < fs::file_size(b);
    });

    std::ofstream tout(timings_path);
    std::ofstream cout_(coll_path);
    if (!tout || !cout_) {
        std::cerr << "Не удалось открыть выходные файлы\n";
        return 1;
    }
    tout << "n,linear_us,bst_us,rb_us,hash_us,multimap_us\n";
    cout_ << "n,bucket_count,collisions,load_factor\n";

    for (const auto& f : files) {
        auto v = pt::read_csv(f.string());
        const std::size_t n = v.size();

        // Готовим список ключей для поиска: 50 случайных ФИО из массива
        // плюс 50 заведомо отсутствующих (для смешанной нагрузки)
        std::mt19937 rng(123);
        std::uniform_int_distribution<std::size_t> idx(0, n - 1);
        std::vector<std::string> queries;
        queries.reserve(100);
        for (int i = 0; i < 50; ++i) queries.push_back(v[idx(rng)].fio);
        for (int i = 0; i < 50; ++i)
            queries.push_back("__NOPE__" + std::to_string(i));

        // Строим индексы один раз; замеряем суммарное время поиска
        pt::search::BST bst;
        pt::search::RBTree rb;
        pt::search::HashTable ht;
        std::multimap<std::string, const pt::Teacher*> mm;
        for (const auto& t : v) {
            bst.insert(t.fio, &t);
            rb.insert(t.fio, &t);
            ht.insert(t.fio, &t);
            mm.emplace(t.fio, &t);
        }

        auto bench = [&](auto&& fn) {
            double r[3];
            for (int k = 0; k < 3; ++k) {
                r[k] = pt::measure_ms([&] {
                    for (const auto& q : queries) {
                        auto x = fn(q);
                        (void)x;
                    }
                });
            }
            // переводим в микросекунды на запрос (на 100 запросов)
            return median3(r[0], r[1], r[2]) * 1000.0 / queries.size();
        };

        double t_lin = bench([&](const std::string& q) {
            return pt::search::linear_find(v, q);
        });
        double t_bst = bench([&](const std::string& q) {
            return bst.find_all(q);
        });
        double t_rb  = bench([&](const std::string& q) {
            return rb.find_all(q);
        });
        double t_ht  = bench([&](const std::string& q) {
            return ht.find_all(q);
        });
        double t_mm  = bench([&](const std::string& q) {
            std::vector<const pt::Teacher*> out;
            auto r = mm.equal_range(q);
            for (auto it = r.first; it != r.second; ++it) out.push_back(it->second);
            return out;
        });

        tout << n << ',' << t_lin << ',' << t_bst << ',' << t_rb << ','
             << t_ht << ',' << t_mm << '\n';
        cout_ << n << ',' << ht.bucket_count() << ',' << ht.collisions() << ','
              << ht.load_factor() << '\n';
        std::cerr << "n=" << n
                  << "  lin=" << t_lin << "us"
                  << "  bst=" << t_bst
                  << "  rb="  << t_rb
                  << "  ht="  << t_ht
                  << "  mm="  << t_mm
                  << "  | bkts=" << ht.bucket_count()
                  << " coll=" << ht.collisions()
                  << "\n";
    }
    return 0;
}

} // namespace

int main(int argc, char** argv) {
    try {
        if (argc >= 4 && std::string(argv[1]) == "search")
            return run_search(argv[2], argv[3]);
        if (argc >= 5 && std::string(argv[1]) == "bench")
            return run_bench(argv[2], argv[3], argv[4]);
        std::cerr << "Использование:\n"
                  << "  " << argv[0] << " search FILE.csv FIO\n"
                  << "  " << argv[0] << " bench DATA_DIR TIMINGS.csv COLLISIONS.csv\n";
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << "\n";
        return 2;
    }
}
