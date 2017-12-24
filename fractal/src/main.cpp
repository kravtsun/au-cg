#include <memory>

#include <AntTweakBar.h>

#include "glfw_window_manager.h"
#include "points.h"
#include "gl_holder.h"
#include "atb.h"

static void init_glew()
{
	glewExperimental = GL_TRUE; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		throw std::logic_error("Failed to initialize GLEW");
	}
}

int main()
{
	auto window_manager = std::make_shared<GLFWWindowManager>("Fractal", 1024, 768);
	init_glew();
	auto point_transformer = std::make_shared<PointTransformer>(window_manager.get());
	auto gl_holder = std::make_shared<GLHolder>(point_transformer.get());
	atb::init(window_manager.get(), point_transformer.get(), gl_holder.get());

	window_manager->main_loop([&gl_holder]()
	{
		gl_holder->paint();
		atb::draw();
	});
	TwTerminate();

	return 0;
}
