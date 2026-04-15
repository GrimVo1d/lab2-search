"""Графики для ЛР2: время поиска и число коллизий."""
import csv
import sys
from pathlib import Path

import matplotlib.pyplot as plt

T = Path("results/timings.csv")
C = Path("results/collisions.csv")
P_TIME = Path("results/timings.png")
P_COLL = Path("results/collisions.png")


def load(path):
    with open(path, newline="") as f:
        rows = [{k: float(v) for k, v in r.items()} for r in csv.DictReader(f)]
    rows.sort(key=lambda r: r["n"])
    return rows


def plot_timings():
    rows = load(T)
    n = [r["n"] for r in rows]
    series = [
        ("Линейный",       "linear_us"),
        ("BST",            "bst_us"),
        ("RB-tree",        "rb_us"),
        ("HashTable",      "hash_us"),
        ("std::multimap",  "multimap_us"),
    ]
    fig, ax = plt.subplots(figsize=(10, 6))
    for name, col in series:
        ax.plot(n, [r[col] for r in rows], marker="o", label=name)
    ax.set_xscale("log"); ax.set_yscale("log")
    ax.set_xlabel("Размер массива N")
    ax.set_ylabel("Среднее время одного поиска, мкс")
    ax.set_title("ЛР2: сравнение методов поиска")
    ax.grid(True, which="both", linestyle="--", alpha=0.4)
    ax.legend()
    fig.tight_layout(); fig.savefig(P_TIME, dpi=130)
    print("Сохранено:", P_TIME)


def plot_collisions():
    rows = load(C)
    n = [r["n"] for r in rows]
    fig, ax = plt.subplots(figsize=(10, 6))
    ax.plot(n, [r["collisions"] for r in rows], marker="o", label="коллизии")
    ax.plot(n, [r["bucket_count"] for r in rows], marker="s", label="бакетов в таблице")
    ax.set_xscale("log"); ax.set_yscale("log")
    ax.set_xlabel("Размер массива N"); ax.set_ylabel("Количество")
    ax.set_title("ЛР2: коллизии хэш-таблицы")
    ax.grid(True, which="both", linestyle="--", alpha=0.4)
    ax.legend()
    fig.tight_layout(); fig.savefig(P_COLL, dpi=130)
    print("Сохранено:", P_COLL)


def main():
    if not T.exists() or not C.exists():
        sys.exit("Сначала: make bench")
    plot_timings()
    plot_collisions()


if __name__ == "__main__":
    main()
