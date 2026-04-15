# ЛР2 — Алгоритмы поиска данных

Лабораторная работа №2 по дисциплине «Программные технологии», вариант 5.

**Реализованы:** линейный поиск, бинарное дерево поиска (BST),
красно-чёрное дерево (RB-Tree), хэш-таблица (полиномиальный хэш +
метод цепочек). В качестве эталона использован `std::multimap`.

**Ключ поиска:** ФИО преподавателя (первое НЕ числовое поле объекта).
Ключи не уникальны — находим ВСЕ вхождения.


## Сборка и прогон

```bash
make plots docs
```

Цели Makefile:

- `make` — собрать `hw2` и `data_gen`;
- `make gen-data` — сгенерировать наборы от 100 до 1 000 000 элементов;
- `make bench` — прогон бенчмарка → `results/timings.csv`,
  `results/collisions.csv`;
- `make plots` — графики (`results/*.png`);
- `make docs` — Doxygen HTML.

## Зависимости

- C++17 (`clang++` или `g++`)
- GNU Make
- Python 3 + `matplotlib` + `pandas` (для `make plots`)
- Doxygen (для `make docs`)

## Структура

```
src/
  search/
    linear.hpp
    bst.hpp
    rbtree.hpp
    hashtable.hpp
  teacher.hpp, teacher.cpp
  csv.hpp, csv.cpp
  data_gen.cpp
  main.cpp
  timing.hpp
data/             сгенерированные CSV
results/          timings.csv, collisions.csv, *.png
docs/html/        Doxygen
Makefile
Doxyfile
plot.py
```
