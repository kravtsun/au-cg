#ifndef DEFERRED_LIGHT_H
#define DEFERRED_LIGHT_H

#include<cmath>
#include <glm/glm.hpp>

struct Light {
    static constexpr double y = 3;
    
    virtual void step() = 0;
    
    glm::vec3 getPosition() const {
        return position;
    }
    
    glm::vec3 getColor() const {
        return color;
    }
    
    void setColor(const glm::vec3 &new_color) {
        color = new_color;
    }
    
    float getAngle() const {
        return angle;
    }
    
    void setAngle(float new_angle) {
        angle = new_angle;
    }

protected:
    void setPosition(const glm::vec3 &new_position) {
        position = new_position;
    }
    
    virtual ~Light() = default;

private:
    glm::vec3 position{0, Light::y, 0};
    glm::vec3 color{1.0, 1.0, 1.0};
//    float angle = static_cast<float>(M_PI);
    float angle = static_cast<float>(M_PI_2 / 2);
};

struct LissajousLight : public Light {
    static constexpr double delta = M_PI_2;
    static constexpr double A = 5;
    static constexpr double B = 5;
    
    LissajousLight(int a, int b, double t = 0.0, double tstep = 0.01)
            : a(a), b(b), t(t), tstep(tstep) {
        assert(std::abs(a - b) == 1);
    }
    
    void step() override {
        double X, Y;
        t += tstep;
        if (t > 2 * M_PI) {
            t = 0.0;
        }
        calculate(a, b, t, X, Y);
        setPosition(glm::vec3(X, LissajousLight::y, Y));
    }

private:
    int a, b;
    double t, tstep;
    
    static void calculate(const double a, const double b, const double t, double &x, double &y) {
        x = A * sin(a * t + delta);
        y = B * sin(b * t);
    }
};


struct StationaryLight : public Light {
    StationaryLight() = default;
    
    explicit StationaryLight(const glm::vec3 &position) {
        setPosition(position);
    }
    
    void step() override {}
};

#endif //DEFERRED_LIGHT_H
