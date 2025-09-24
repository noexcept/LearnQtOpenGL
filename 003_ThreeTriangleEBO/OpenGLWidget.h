//
// Created by noexcept on 2025/9/23.
//

#ifndef LEARNQTOPENGL_OPENGLWIDGET_H
#define LEARNQTOPENGL_OPENGLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_1_Core>

class OpenGLWidget : public QOpenGLWidget
{
    Q_OBJECT
public:
    OpenGLWidget(QWidget* parent = nullptr);
    ~OpenGLWidget();
    void initShaders();
    void initVBOAndEBOAndVAO();

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void checkShaderCompileStatus(GLuint shader, const std::string &shaderType);
    void checkProgramLinkStatus();
private:
    QOpenGLFunctions_4_1_Core *m_glFuncs = nullptr;
    // 顶点数组对象
    GLuint m_vao = 0;
    // 元素缓冲对象
    GLuint m_ebo = 0;
    // 顶点缓冲对象
    GLuint m_vbo = 0;
    // 着色器程序
    GLuint m_program = 0;
};


#endif //LEARNQTOPENGL_OPENGLWIDGET_H
