#include <memory>
#include <cassert>
#include <glm/glm.hpp>
#include "gl_holder.h"
#include "shader.h"
#include "scene.h"
#include "controls.h"

#define PASS_UNIFORM_3F(id, value) \
    glUniform3f(id, (value).x, (value).y, (value).z);

#define PASS_UNIFORM_MAT4(id, value)                                \
    do {                                                            \
        const glm::mat4 stable_value = (value);                     \
        glUniformMatrix4fv(id, 1, GL_FALSE, &(stable_value)[0][0]); \
    } while (false);


struct Light {
    static constexpr double y = 3;
    
    virtual void step() = 0;
    
    glm::vec3 getPosition() const {
        return position;
    }
    
    glm::vec3 getColor() const {
        return color;
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
    
    void setColor(const glm::vec3 &new_color) {
        color = new_color;
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

enum GBUFFER_TEXTURE_TYPE {
    GBUFFER_TEXTURE_TYPE_POSITION = 0,
    GBUFFER_TEXTURE_TYPE_NORMAL,
    GBUFFER_TEXTURE_TYPE_DIFFUSE,
    GBUFFER_TEXTURE_TYPE_AMBIENT,
//    GBUFFER_TEXTURE_TYPE_TEXCOORD,
    GBUFFER_NUM_TEXTURES
};
GLuint textures[GBUFFER_NUM_TEXTURES];
GLuint depth_texture;

static GLuint fbo;

namespace geom {
static GLuint program_id;
static GLuint vertex_array_id, vertexbuffer, normalbuffer;
// geometry pass variables.
static GLint mvp_id, m_id, v_id;

static GLint diffuse_color_id, ambient_color_id;
//static GLint vertex_color_id;
}

namespace light {
static GLuint program_id;
static GLuint quad_vertexbuffer;
// light pass variables.
static GLint position_texture_id, normal_texture_id;
static GLint diffuse_texture_id, ambient_texture_id;

static GLint specular_color_id;
static GLint v_id;
static GLint light_position_id, light_color_id, light_angle_id;
}

namespace ambient_render {
static GLuint program_id;
static GLint texture_id;
}

GLuint quad_VertexArrayID;

static std::shared_ptr<Scene> scene;

static std::vector<std::shared_ptr<Light>> lights;

static void init_gbuffer(const int width, const int height) {
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    
    // Создаем текстуры gbuffer
    glGenTextures(GBUFFER_NUM_TEXTURES, textures);
    glGenTextures(1, &depth_texture);
    
    for (unsigned int i = 0; i < GBUFFER_NUM_TEXTURES; i++) {
        glBindTexture(GL_TEXTURE_2D, textures[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
        
        // Poor filtering
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, textures[i], 0);
    }
    
    glBindTexture(GL_TEXTURE_2D, depth_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_texture, 0);
    
    const GLenum DrawBuffers[] = {
            GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1
             , GL_COLOR_ATTACHMENT2
             , GL_COLOR_ATTACHMENT3
    };
    const GLsizei drawBuffersCount = sizeof(DrawBuffers) / sizeof(DrawBuffers[0]);
    static_assert(drawBuffersCount == GBUFFER_NUM_TEXTURES);
    glDrawBuffers(drawBuffersCount, DrawBuffers);
    
    const GLenum fbo_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    assert(fbo_status == GL_FRAMEBUFFER_COMPLETE);
}

GLHolder::GLHolder(std::shared_ptr<GLFWWindowManager> window_manager)
        : window_manager(window_manager) {
    glClearColor(0.1f, 0.0f, 0.0f, 0.0f);
    assert(glGetError() == GL_NO_ERROR);
    
    const auto width = window_manager->win_width();
    const auto height = window_manager->win_height();
    init_gbuffer(width, height);
    
    glGenVertexArrays(1, &geom::vertex_array_id);
    glBindVertexArray(geom::vertex_array_id);
    
    geom::program_id = load_shaders("geom.vsh", "geom.fsh");
    
    glUseProgram(geom::program_id);
    geom::mvp_id = glGetUniformLocation(geom::program_id, "MVP");
    geom::m_id = glGetUniformLocation(geom::program_id, "M");
    geom::v_id = glGetUniformLocation(geom::program_id, "V");
    geom::diffuse_color_id = glGetUniformLocation(geom::program_id, "diffuse_color");
    geom::ambient_color_id = glGetUniformLocation(geom::program_id, "ambient_color");
//    vertex_color_id = glGetUniformLocation(geom::program_id, "vertexColor");
    scene = std::make_shared<Scene>("sagar.obj");
    
    glGenBuffers(1, &geom::vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, geom::vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, Model::all_vertices.size() * sizeof(glm::vec3), &Model::all_vertices[0],
                 GL_STATIC_DRAW);
    
    glGenBuffers(1, &geom::normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, geom::normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, Model::all_normals.size() * sizeof(glm::vec3), &Model::all_normals[0],
                 GL_STATIC_DRAW);
    
    light::program_id = load_shaders("light.vsh", "light.fsh");
    light::position_texture_id = glGetUniformLocation(light::program_id, "positionTexture");
    light::normal_texture_id = glGetUniformLocation(light::program_id, "normalTexture");
    light::diffuse_texture_id = glGetUniformLocation(light::program_id, "diffuseTexture");
    light::ambient_texture_id = glGetUniformLocation(light::program_id, "ambientTexture");
    
    light::specular_color_id = glGetUniformLocation(light::program_id, "specular_color");
    light::v_id = glGetUniformLocation(light::program_id, "V");
    light::light_position_id = glGetUniformLocation(light::program_id, "lightPos");
    light::light_color_id = glGetUniformLocation(light::program_id, "lightColor");
    light::light_angle_id = glGetUniformLocation(light::program_id, "lightAngle");
    
    static const GLfloat quadz = 1.0f;
    static const GLfloat g_quad_vertex_buffer_data[] = {
            -1.0f, -1.0f, quadz,
            1.0f, -1.0f, quadz,
            -1.0f, 1.0f, quadz,
            -1.0f, 1.0f, quadz,
            1.0f, -1.0f, quadz,
            1.0f, 1.0f, quadz,
    };
    
    glGenBuffers(1, &light::quad_vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, light::quad_vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);
    
    glGenVertexArrays(1, &quad_VertexArrayID);
    glBindVertexArray(quad_VertexArrayID);
    
    ambient_render::program_id = load_shaders("light.vsh", "debug.fsh");
    ambient_render::texture_id = glGetUniformLocation(ambient_render::program_id, "renderedTexture");
    
    lights.push_back(std::make_shared<StationaryLight>());
    lights.push_back(std::make_shared<LissajousLight>(1, 2));
    lights.push_back(std::make_shared<LissajousLight>(2, 1));
    lights.push_back(std::make_shared<LissajousLight>(3, 2));
    lights.push_back(std::make_shared<LissajousLight>(2, 3));
    lights.push_back(std::make_shared<LissajousLight>(5, 4));
    lights.push_back(std::make_shared<LissajousLight>(4, 5));
    lights.push_back(std::make_shared<LissajousLight>(5, 6));
    lights.push_back(std::make_shared<LissajousLight>(6, 5));
    lights.push_back(std::make_shared<LissajousLight>(9, 8));
    lights.push_back(std::make_shared<LissajousLight>(8, 9));
}

void GLHolder::geometry_pass() {
    using namespace geom;
    const int width = window_manager->win_width();
    const int height = window_manager->win_height();
    
    // first pass.
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(program_id);
    
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(
            0, // attribute. No particular reason for 0, but must match the layout in the shader.
            3, // size
            GL_FLOAT, // type
            GL_FALSE, // normalized?
            0, // stride
            static_cast<void *>(nullptr) // array buffer offset
    );
    
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glVertexAttribPointer(
            1,
            3,
            GL_FLOAT,
            GL_FALSE,
            0,
            static_cast<void *>(nullptr) // array buffer offset
    );
    
    computeMatricesFromInputs(window_manager->window());
    const glm::mat4 projection_matrix = getProjectionMatrix();
    const glm::mat4 view_matrix = getViewMatrix();
//    PASS_UNIFORM_3F(camera_position_id, getCameraPosition());
    for (auto const &model : scene->models) {
        const auto &model_matrix = model->model_matrix;
        const glm::mat4 mvp = projection_matrix * view_matrix * model_matrix;
        PASS_UNIFORM_MAT4(mvp_id, mvp);
        PASS_UNIFORM_MAT4(m_id, model_matrix);
        PASS_UNIFORM_MAT4(v_id, view_matrix);
    
        glUniform3f(diffuse_color_id, 1.0, 0.0f, 0.0f);
        glUniform3f(ambient_color_id, 0.0, 0.0f, 0.0f);
        
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDrawArrays(GL_TRIANGLES, static_cast<GLint>(model->all_vertices_offset), static_cast<GLsizei>(model->size()));
        model->step();
    }
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

static void ambient_render_pass(int texId) {
    glUseProgram(ambient_render::program_id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[texId]);
    glUniform1i(ambient_render::texture_id, 0);
    
    {
        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, light::quad_vertexbuffer);
        glVertexAttribPointer(
                0,
                3,
                GL_FLOAT,
                GL_FALSE,
                0,
                nullptr
        );
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glDisableVertexAttribArray(0);
    }
}

void GLHolder::light_pass() {
    using namespace light;
    const int width = window_manager->win_width();
    const int height = window_manager->win_height();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    int texId = mode == DEFERRED ? GBUFFER_TEXTURE_TYPE_AMBIENT : static_cast<int>(mode);
    ambient_render_pass(texId);
    
    if (mode == DEFERRED) {
        glUseProgram(program_id);
        const GLint texture_ids[] = {
                position_texture_id, normal_texture_id, diffuse_texture_id, ambient_texture_id
        };
        const int texture_ids_count = sizeof(texture_ids) / sizeof(texture_ids[0]);
        static_assert(texture_ids_count == GBUFFER_NUM_TEXTURES);
        for (int itex = 0; itex < texture_ids_count; ++itex) {
            glActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + itex));
            glBindTexture(GL_TEXTURE_2D, textures[itex]);
            glUniform1i(texture_ids[itex], itex);
        }
    
        glUniform3f(specular_color_id, 1.0, 1.0f, 1.0f);
    
        auto v = getViewMatrix();
        PASS_UNIFORM_MAT4(v_id, v);
    
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
        glVertexAttribPointer(
                0,
                3,
                GL_FLOAT,
                GL_FALSE,
                0,
                nullptr
        );
    
        bool blendEnabledBefore = glIsEnabled(GL_BLEND);
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
    
        for (int i = 0; i < lightsCount && i < static_cast<int>(lights.size()); ++i) {
            auto const &light = lights[i];
            PASS_UNIFORM_3F(light_position_id, light->getPosition());
            PASS_UNIFORM_3F(light_color_id, light->getColor());
            glUniform1f(light_angle_id, light->getAngle());
            glDrawArrays(GL_TRIANGLES, 0, 6);
            light->step();
        }
    
        if (!blendEnabledBefore) {
            glBlendFunc(GL_ONE, GL_ZERO);
            glDisable(GL_BLEND);
        }
    
        glDisableVertexAttribArray(0);
    }
}

void GLHolder::paint() {
    geometry_pass();
    assert(glGetError() == GL_NO_ERROR);
    light_pass();
    assert(glGetError() == GL_NO_ERROR);
}

GLHolder::~GLHolder() {
    // TODO create singletons from geom and light namespaces and move this deletions to destructors
    // (GLHolder should own those singletons)
    glDeleteTextures(GBUFFER_NUM_TEXTURES, textures);
    
    glDeleteVertexArrays(1, &quad_VertexArrayID);
    
    glDeleteBuffers(1, &geom::vertexbuffer);
    glDeleteBuffers(1, &geom::normalbuffer);
    glDeleteVertexArrays(1, &geom::vertex_array_id);
    glDeleteBuffers(1, &geom::vertexbuffer);
    glDeleteBuffers(1, &geom::normalbuffer);
    glDeleteProgram(geom::program_id);
    
    glDeleteBuffers(1, &light::quad_vertexbuffer);
    glDeleteProgram(light::program_id);
}

