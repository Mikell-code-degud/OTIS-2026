"""
plot.py — график переходного процесса замкнутой системы «ПИД + объект».

Формат графика повторяет рисунки из методички: один график, три кривые —
  зелёная — реакция объекта без регулятора (открытый контур, скачок u = 1);
  синяя   — регулируемая величина y(t) с ПИД-регулятором;
  красная — управляющее воздействие u(t);
пунктирная линия — задание (уставка) w.

Читает pid_result.csv (формат: t;w;e;u;y;y_open, разделитель ';').
"""
import csv

import matplotlib.pyplot as plt


def to_float(s):
    return float(s.replace(",", "."))


t, w, u, y, y_open = [], [], [], [], []

with open("pid_result.csv", encoding="utf-8") as f:
    reader = csv.reader(f, delimiter=";")
    next(reader)                              # заголовок t;w;e;u;y;y_open
    for row in reader:
        if not row:
            continue
        t.append(int(row[0]))
        w.append(to_float(row[1]))
        u.append(to_float(row[3]))
        y.append(to_float(row[4]))
        y_open.append(to_float(row[5]))

fig, ax = plt.subplots(figsize=(9, 6))

ax.plot(t, y_open, "g-", linewidth=1.6,
        label="объект без регулятора (открытый контур)")
ax.plot(t, y, "b-o", markersize=3, label="y(t) — регулируемая величина")
ax.plot(t, u, "r-", linewidth=1.6, label="u(t) — управляющее воздействие")
ax.plot(t, w, "k--", linewidth=1.0, label="w — задание (уставка)")

ax.set_xlabel("t (номер такта)")
ax.set_ylabel("y, u, w")
ax.set_title("Переходный процесс (ПИД-регулятор)")
ax.grid(True)
ax.legend()

plt.tight_layout()
plt.savefig("graph.png", dpi=150)
plt.show()
