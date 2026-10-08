<p align="center"> Министерство образования Республики Беларусь</p>
<p align="center">Учреждение образования</p>
<p align="center">“Брестский Государственный технический университет”</p>
<p align="center">Кафедра ИИТ</p>
<br><br><br><br><br><br><br>
<p align="center">Лабораторная работа №1</p>
<p align="center">По дисциплине “Общая теория интеллектуальных систем”</p>
<p align="center">Тема: “Моделирование температуры объекта”</p>
<br><br><br><br><br>
<p align="right">Выполнил:</p>
<p align="right">Студент 2 курса</p>
<p align="right">Группы ИИ-30</p>
<p align="right">Низамутдинов М. А.</p>
<p align="right">Проверил:</p>
<p align="right">Дворанинович Д. А.</p>
<br><br><br><br><br>
<p align="center">Брест 2026</p>

## Вариант

- Линейная модель: Model 1.6
- Нелинейная модель: Model 2.8
- Дифференциальное уравнение: Model 3.10

## Реализованные модели

### Model 1.6 — Third-Order Dynamic System

y(τ+1) = a1*y(τ) + a2*y(τ-1) + a3*y(τ-2) + b*u(τ)

### Model 2.8 — Chaotic Logistic Map Disturbance

y(τ+1) = a*y(τ)*(1 - y(τ)) + b*u(τ) + c*sin(y(τ-1)*u(τ))

### Model 3.10 — Basic External Constant Offset

dy/dt = -a*y + b + u

Для Model 3.10 используется метод Эйлера:

y(τ+1) = y(τ) + dt*(-a*y(τ) + b + u(τ))

## Входные воздействия

1. Ступенчатое: u(τ) = A
2. Импульсное: u(0) = A, u(τ > 0) = 0
3. Гармоническое: u(τ) = A*sin(τ)

## ООП

Создан абстрактный базовый класс Model с виртуальным методом calculateNext().

От него наследуются:

- Model1_6
- Model2_8
- Model3_10

## Результат

Программа:

- позволяет выбрать модель;
- принимает коэффициенты;
- позволяет выбрать входное воздействие;
- позволяет задать количество шагов n;
- выводит результаты в табличном виде;
- сохраняет результаты в results.csv;
- позволяет построить график через Python/Matplotlib.

## Сборка

Сборка через build.bat, который использует CMake.

## UML

```mermaid
classDiagram
    class Model {
        <<abstract>>
        +calculateNext(y, yPrev, yPrev2, u, dt) double
        +getName() const char*
    }

    class Model1_6 {
        -a1 double
        -a2 double
        -a3 double
        -b double
        +calculateNext(...) double
    }

    class Model2_8 {
        -a double
        -b double
        -c double
        +calculateNext(...) double
    }

    class Model3_10 {
        -a double
        -b double
        +calculateNext(...) double
    }

    Model <|-- Model1_6
    Model <|-- Model2_8
    Model <|-- Model3_10
