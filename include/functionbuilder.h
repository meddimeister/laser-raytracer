#pragma once
#include <cstddef>
#include <vector>
#include <tuple>
#include <functional>
#include <algorithm>
#include "vecn.h"

function<float(float)> getFunction(const vector<tuple<float, float>> &points) {
    return [&](float x){
        auto it = find_if(points.begin(), points.end(), [&](const tuple<float, float> &point){
            return x < get<0>(point);
        });
        if(it != points.end() && it != points.begin()){
            auto [x_0, f_0] = *(it-1);
            auto [x_1, f_1] = *(it);
            float alpha = (x - x_1)/(x_0 - x_1);
            return (1.0f - alpha) * f_0 + alpha * f_1;
        };
        return 0.0f;
    };
}