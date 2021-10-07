#pragma once
#include <array>

using namespace std;

template <class T, size_t N>
struct vecn : public array<T, N>
{

    vecn<T, N> operator+(const vecn<T, N> &other) const
    {
        vecn<T, N> result;
        for (size_t i = 0; i < this->size(); ++i)
        {
            result[i] = this->operator[](i) + other[i];
        }
        return result;
    }

    vecn<T, N> operator-(const vecn<T, N> &other) const
    {
        vecn<T, N> result;
        for (size_t i = 0; i < this->size(); ++i)
        {
            result[i] = this->operator[](i) - other[i];
        }
        return result;
    }

    vecn<T, N> operator*(const T &factor) const
    {
        vecn<T, N> result;
        for (size_t i = 0; i < this->size(); ++i)
        {
            result[i] = this->operator[](i) * factor;
        }
        return result;
    }

    vecn<T, N> operator/(const T &divisor) const
    {
        vecn<T, N> result;
        for (size_t i = 0; i < this->size(); ++i)
        {
            result[i] = this->operator[](i) / divisor;
        }
        return result;
    }
};

template <class T, size_t N>
T length(const vecn<T, N> &vec)
{
    T sum;
    for (const auto &e : vec)
        sum += e * e;
    return sqrt(sum);
}

template <class T, size_t N>
vecn<T, N> normalize(const vecn<T, N> &vec)
{
    return vec / length(vec);
}

template <class T, size_t N>
ostream &operator<<(ostream &stream, const vecn<T, N> &vec)
{
    stream << "[ ";
    for (const auto &e : vec)
        stream << e << " ";
    stream << "]";
    return stream;
}