#ifndef FRACTAL_POINTS_H
#define FRACTAL_POINTS_H

#include <vector>
#include <GL/glew.h>
#include <glm/vec2.hpp>

template<typename T>
struct PointT {
	T x, y;
	explicit PointT(T x=0, T y=0)
			: x(x)
			, y(y)
	{}
};

using Point = PointT<GLfloat>;

class GLFWWindowManager;

class PointTransformer {
public:
	explicit PointTransformer(GLFWWindowManager *window_manager);
	
	void change_center_with_mouse_move(glm::dvec2 pos_delta);
	
	// TODO fix name clashing with center field.
	glm::dvec2 get_scale() const {
		return glm::dvec2(get_scale_x(), get_scale_y());
	}

    const std::vector<Point>& get_points() const;
	
	void multiply_scale(double scale_delta_multiplier, glm::dvec2 mouse_pos);

	double scale = 1.0;
	// center in shader coordinates.
	glm::dvec2 center;

private:
	GLFWWindowManager *window_manager_;
	
	double get_scale_x() const;
	
	double get_scale_y() const;
	
	double get_x_delta() const;

	double get_y_delta() const;
};

#endif //FRACTAL_POINTS_H
