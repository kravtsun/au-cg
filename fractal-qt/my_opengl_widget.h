#ifndef MYOPENGLWIDGET_H
#define MYOPENGLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLContext>

class MyOpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
public:
    MyOpenGLWidget();

    virtual ~MyOpenGLWidget();

protected:
    void paintGL() override;

    void resizeGL(int w, int h) override;

private:
//    QOpenGLFunctions_3_3_Core *glfun;
    QOpenGLContext* ctx;
    QSurfaceFormat* fmt;

    GLuint VertexArrayID;
    GLuint vertexbuffer;

    GLuint programID;
    GLuint TextureID;

    GLuint MatrixID;
    GLuint IterationsID;
    GLuint AbsLimID;
};

#endif // MYOPENGLWIDGET_H
