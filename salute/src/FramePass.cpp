#include <glm/vec3.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "FramePass.h"
#include "shader.h"

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

static const GLfloat g_vertex_buffer_data[] = {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
        1.0f,  1.0f, 0.0f,
};

FramePass::FramePass(const int width, const int height)
    : AbstractPass(width, height)
{
    program_id = load_shaders("particle.vsh", "particle.fsh");
    camera_up_id = glGetUniformLocation(program_id, "CameraUp_worldspace");
    camera_right_id = glGetUniformLocation(program_id, "CameraRight_worldspace");
    vp_id = glGetUniformLocation(program_id, "VP");
    particle_start_id = glGetUniformLocation(program_id, "ParticleStart_worldspace");
    particle_color_id = glGetUniformLocation(program_id, "ParticleColor");
    particle_size_id = glGetUniformLocation(program_id, "ParticleSize");
    time_after_explosion_id = glGetUniformLocation(program_id, "TimeAfterExplosion");
    
    // create test emission.
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    for (int i = 0; i < NPARTICLES; ++i) {
        for (int j = 0; j < 3; ++j) {
            speed[i][j] = rand_range<float>(-1.0, 1.0);
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

void FramePass::pass() {
// Projection matrix : 45 Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
//    auto projection = glm::perspective(glm::radians(FoV), 4.0f / 3.0f, 0.1f, 100.0f);
    const float limit = 100.f;
    const float zNear = 0.1f, zFar = 100.f;
    const float xLeft = -limit, xRight = limit;
    const float yBottom = -limit, yTop = limit;
    
    auto projection = glm::ortho(xLeft, xRight, yBottom, yTop, zNear, zFar);
    auto view = glm::lookAt(camera_position, camera_position + direction, up);
    auto pv = projection * view;
    
    glUseProgram(program_id);
    PASS_UNIFORM_3F(camera_up_id, up);
    PASS_UNIFORM_3F(camera_right_id, right);
    PASS_UNIFORM_MAT4(vp_id, pv);
    
    PASS_UNIFORM_3F(particle_start_id, vec3(0, 0, 0));
    PASS_UNIFORM_3F(particle_color_id, vec3(1, 1, 1));
    glUniform1f(particle_size_id, 0.6);
    glUniform1f(time_after_explosion_id, time_after_explosion);
    time_after_explosion += time_delta;

//    computeMatricesFromInputs(window_manager->window());
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
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
    glDeleteFramebuffers(1, &fbo);
    glDeleteTextures(1, &color_texture);
    glDeleteBuffers(1, &speed_buffer);
    glDeleteBuffers(1, &square_buffer);
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(program_id);
}

void FramePass::reset() {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, get_width(), get_height());
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    time_after_explosion = 0;
}
