"""
plot.py — графики переходного процесса замкнутой системы «ПИД + объект».

Читает pid_result.csv (формат: t;w;e;u;y, разделитель ';') и строит два
сабплота: регулируемую величину y(t) с уставкой w и управляющее воздействие u(t).
"""
import csv

import matplotlib.pyplot as plt


def to_float(s):
    return float(s.replace(",", "."))


t, w, u, y = [], [], [], []

with open("pid_result.csv", encoding="utf-8") as f:
    reader = csv.reader(f, delimiter=";")
    next(reader)                              # заголовок t;w;e;u;y
    for row in reader:
        if not row:
            continue
        t.append(int(row[0]))
        w.append(to_float(row[1]))
        u.append(to_float(row[3]))
        y.append(to_float(row[4]))

fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(8, 7), sharex=True)

ax1.plot(t, y, "b-o", markersize=3, label="y(t) — регулируемая величина")
ax1.plot(t, w, "r--", label="w — задание (уставка)")
ax1.set_ylabel("y, w")
ax1.set_title("Переходный процесс (ПИД-регулятор)")
ax1.grid(True)
ax1.legend()

ax2.plot(t, u, "g-", label="u(t) — управляющее воздействие")
ax2.set_xlabel("t (номер такта)")
ax2.set_ylabel("u")
ax2.grid(True)
ax2.legend()

plt.tight_layout()
plt.savefig("graph.png", dpi=150)
plt.show()
