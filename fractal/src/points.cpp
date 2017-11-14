#include "points.h"
#include <GL/glew.h>
#include "glfw_window_manager.h"

PointTransformer::PointTransformer(GLFWWindowManager *window_manager)
		: window_manager_(window_manager)
{}

void PointTransformer::change_center_with_mouse_move(glm::dvec2 pos_delta) {
	pos_delta.x *= -get_scale_x() * get_x_delta();
	pos_delta.y *= get_scale_y() * get_y_delta();
	center += pos_delta;
}

std::vector<Point> PointTransformer::get_points() const {
	const int win_height = window_manager_->win_height();
	const int win_width = window_manager_->win_width();
	
	const GLsizei npts = win_width * win_height;
	std::vector<Point> points;
	points.reserve((unsigned long) npts);
	
	for (int y = 0; y < win_height; ++y)
	{
		for (int x = 0; x < win_width; ++x)
		{
			points.emplace_back(static_cast<GLfloat>(-1.0f + x * get_x_delta()),
								static_cast<GLfloat>(-1.0f + y * get_y_delta()));
		}
	}
	assert(points.size() == npts);
	return points;
}

void PointTransformer::multiply_scale(double scale_delta_multiplier, glm::dvec2 mouse_pos) {
	const glm::dvec2 win_size{window_manager_->win_width(), window_manager_->win_height()};
	
	// x,y \in [-1.0; 1.0]
	auto normalized_mouse_pos = (2.0 * mouse_pos - win_size) / win_size;
	auto old_mouse_pos_on_shader = center + get_scale() * normalized_mouse_pos;
	
	scale *= scale_delta_multiplier;
	
	// Centering mouse_position for scaling.
	auto new_mouse_pos_on_shader = center + get_scale() * normalized_mouse_pos;
	
	auto center_bias = old_mouse_pos_on_shader - new_mouse_pos_on_shader;
	center_bias.y *= -1; // compensating different y direction on shader and in our window.
	center += center_bias;
}

double PointTransformer::get_scale_x() const {
	const int win_height = window_manager_->win_height();
	const int win_width = window_manager_->win_width();
	if (win_height > win_width)
	{
		return scale;
	}
	return scale * static_cast<double>(win_width) / win_height;
}

double PointTransformer::get_scale_y() const {
	const int win_height = window_manager_->win_height();
	const int win_width = window_manager_->win_width();
	if (win_height > win_width)
	{
		return scale * static_cast<double>(win_height) / win_width;
	}
	return scale;
}

double PointTransformer::get_x_delta() const {
	return 2.0 / window_manager_->win_width();
}

double PointTransformer::get_y_delta() const {
	return 2.0 / window_manager_->win_height();
}
