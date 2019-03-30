#include <rttr/registration>
//using namespace rttr;

#include "../../core/matadata.h"

struct vector2d {
    vector2d() {
    }
    vector2d(double x, double y) : x_(x), y_(y) {
    }

    double x_ = 0;
    double y_ = 0;
};

struct Calculator {
    Calculator(){};

	template <typename T>
    T sum(T val1, T val2) {
        return val1 + val2;
    };

    vector2d multiply(double val1, vector2d val2) {
        return vector2d(val1 * val2.x_, val1 * val2.y_);
    };

    vector2d multiply(vector2d val1, double val2) {
        return vector2d(val2 * val1.x_, val2 * val1.y_);
    };

    vector2d sum(vector2d val1, vector2d val2) {
        return vector2d(val1.x_ + val2.x_, val1.y_ + val2.y_);
    };

    double dot(vector2d val1, vector2d val2) {
        return val1.x_ * val2.x_ + val1.y_ * val2.y_;
    };
};
