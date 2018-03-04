#include <glm/vec3.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "FramePass.h"
#include "wrappers/AllWrappers.h"

using glm::vec3;

static const vec3 camera_position{0, 0, 1};
static const vec3 direction{0, 0, -1};
static const vec3 up{0, 1, 0};
static const vec3 right{1, 0, 0};

static const float SPEED_MAGNITUDE = 30.f;

static const int NPARTICLES = 1000;

static vec3 speed[NPARTICLES];

template<typename T=double>
static inline T rand_range(T a, T b) {
    assert(b > a);
    return a + static_cast<T>(rand()) / RAND_MAX * (b - a);
}

template<typename T>
T sqr(T x) {
    return x * x;
}

static const GLfloat g_vertex_buffer_data[] = {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
        1.0f,  1.0f, 0.0f,
};

FramePass::FramePass(int width, int height,
                     const glm::vec3 &particle_start,
                     const glm::vec3 &particle_color)
    : AbstractPass(width, height)
    , particle_start(particle_start)
    , particle_color(particle_color)
{
    program.reset(new Program("particle.vsh", "particle.fsh"));
    camera_up_id = glGetUniformLocation(*program, "CameraUp_worldspace");
    camera_right_id = glGetUniformLocation(*program, "CameraRight_worldspace");
    vp_id = glGetUniformLocation(*program, "VP");
    particle_start_id = glGetUniformLocation(*program, "ParticleStart_worldspace");
    particle_color_id = glGetUniformLocation(*program, "ParticleColor");
    particle_size_id = glGetUniformLocation(*program, "ParticleSize");
    time_after_explosion_id = glGetUniformLocation(*program, "TimeAfterExplosion");
    fade_multiplier_id = glGetUniformLocation(*program, "fade_multiplier");
    seconds_to_decelerate_id = glGetUniformLocation(*program, "seconds_to_decelerate");
    
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    for (int i = 0; i < NPARTICLES; ++i) {
        for (int j = 0; j < 3; ++j) {
            speed[i][j] = rand_range<float>(-1.f, 1.f);
        }
        speed[i] = glm::normalize(speed[i]) * SPEED_MAGNITUDE;
    }
    
    glGenBuffers(1, &square_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, square_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
    
    glGenBuffers(1, &speed_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, speed_buffer);
    static_assert(sizeof(vec3) == 3 * 4, "");
    glBufferData(GL_ARRAY_BUFFER, sizeof(speed), speed, GL_STATIC_DRAW);
    
    init_framebuffer_with_output_texture(fbo, color_texture);
}

float FramePass::get_fade_multiplier() const {
    auto time_normalized = time_after_explosion / seconds_to_decelerate;
    auto fade_multiplier = 1.f;
    if (time_normalized > fade_start_time_normalized) {
        fade_multiplier = expf(-fade_speed * sqr(time_normalized - fade_start_time_normalized));
    }
    return fade_multiplier;
}

void FramePass::pass() {
// Projection matrix : 45 Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
//    auto projection = glm::perspective(glm::radians(FoV), 4.0f / 3.0f, 0.1f, 100.0f);
    const float zNear = 0.1f, zFar = 100.f;
    const float xLeft = -limit, xRight = limit;
    const float yBottom = -limit, yTop = limit;
    
    auto projection = glm::ortho(xLeft, xRight, yBottom, yTop, zNear, zFar);
    auto view = glm::lookAt(camera_position, camera_position + direction, up);
    auto pv = projection * view;
    
    program->use();
    PASS_UNIFORM_3F(camera_up_id, up);
    PASS_UNIFORM_3F(camera_right_id, right);
    PASS_UNIFORM_MAT4(vp_id, pv);
    
    PASS_UNIFORM_3F(particle_start_id, particle_start);
    PASS_UNIFORM_3F(particle_color_id, particle_color);
    glUniform1f(particle_size_id, 0.6);
    glUniform1f(time_after_explosion_id, time_after_explosion);
    glUniform1f(seconds_to_decelerate_id, seconds_to_decelerate);
    glUniform1f(fade_multiplier_id, get_fade_multiplier());
    time_after_explosion += time_delta;

//    computeMatricesFromInputs(window_manager->window());
    fbo->bind();
    glViewport(0, 0, get_width(), get_height());
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ZERO);
//    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, square_buffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, speed_buffer);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    
    glVertexAttribDivisor(0, 0); // particles vertices : always reuse the same 4 vertices -> 0
    glVertexAttribDivisor(1, 1); // one per quad advance.
    
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, NPARTICLES);
//    glDrawArrays(GL_TRIANGLES, 0, 3);
    
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

FramePass::~FramePass() {
    glDeleteBuffers(1, &speed_buffer);
    glDeleteBuffers(1, &square_buffer);
    glDeleteVertexArrays(1, &vao);
}

void FramePass::reset() {
    fbo->bind();
    glViewport(0, 0, get_width(), get_height());
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    time_after_explosion = 0;
}
