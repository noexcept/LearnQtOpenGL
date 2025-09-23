//
// Created by noexcept on 2025/9/23.
//

#include "OpenGLWidget.h"

OpenGLWidget::OpenGLWidget(QWidget* parent)
    : QOpenGLWidget(parent)
{
}

OpenGLWidget::~OpenGLWidget()
{
    if (m_program && m_glFuncs)
    {
        m_glFuncs->glDeleteProgram(m_program);
        m_program = 0;
    }
    if (m_glFuncs) {
        delete m_glFuncs;
        m_glFuncs = nullptr;
    }
}

void OpenGLWidget::initShaders()
{
    const char *vertexShaderSource = R"(
        #version 410 core
        layout (location = 0) in vec3 aPos;
        void main()
        {
            gl_Position = vec4(aPos, 1.0);
        }
    )";
    GLuint vertexShader = m_glFuncs->glCreateShader(GL_VERTEX_SHADER);
    m_glFuncs->glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    m_glFuncs->glCompileShader(vertexShader);
    checkShaderCompileStatus(vertexShader, "vertex");

    const char *fragmentShaderSource = R"(
        #version 410 core
        out vec4 FragColor;
        void main()
        {
            FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
        }
    )";
    GLuint fragmentShader = m_glFuncs->glCreateShader(GL_FRAGMENT_SHADER);
    m_glFuncs->glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    m_glFuncs->glCompileShader(fragmentShader);
    checkShaderCompileStatus(fragmentShader, "fragment");

    m_program = m_glFuncs->glCreateProgram();
    m_glFuncs->glAttachShader(m_program, vertexShader);
    m_glFuncs->glAttachShader(m_program, fragmentShader);
    m_glFuncs->glLinkProgram(m_program);
    checkProgramLinkStatus();

    m_glFuncs->glDeleteShader(vertexShader);
    m_glFuncs->glDeleteShader(fragmentShader);
}

void OpenGLWidget::initVBOAndVAO()
{
    GLfloat vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f,  0.5f, 0.0f
    };

    // 创建顶点缓冲对象
    m_glFuncs->glGenBuffers(1, &m_vbo);
    // 绑定顶点缓冲对象
    m_glFuncs->glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    // 复制顶点数据到缓冲中
    m_glFuncs->glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 创建顶点数组对象
    m_glFuncs->glGenVertexArrays(1, &m_vao);
    // 绑定顶点数组对象
    m_glFuncs->glBindVertexArray(m_vao);
    // 启用顶点属性数组，启用这个属性才能glVertexAttribPointer设置顶点属性
    m_glFuncs->glEnableVertexAttribArray(0);
    // 顶点属性指针
    m_glFuncs->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

    // 解除VAO的绑定
    m_glFuncs->glBindVertexArray(0);
    // 解除VBO的绑定
    m_glFuncs->glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void OpenGLWidget::initializeGL()
{
    QOpenGLWidget::initializeGL();

    // 初始化OpenGL函数指针
    m_glFuncs = new QOpenGLFunctions_4_1_Core();
    if (!m_glFuncs->initializeOpenGLFunctions())
    {
        qFatal("OpenGL widget initializeOpenGLFunctions() failed");
    }
    // 设置清除颜色为灰色
    m_glFuncs->glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

    initVBOAndVAO();

    initShaders();
}


void OpenGLWidget::resizeGL(int w, int h)
{
    QOpenGLWidget::resizeGL(w, h);
}

void OpenGLWidget::paintGL() {
    if (!m_glFuncs) {
        qFatal("OpenGL widget paintGL() failed, m_glFuncs is null");
    }
    // 清除颜色缓冲区
    m_glFuncs->glClear(GL_COLOR_BUFFER_BIT);
    // 使用着色器程序
    m_glFuncs->glUseProgram(m_program);
    // 绑定VAO
    m_glFuncs->glBindVertexArray(m_vao);
    // 绘制三角形
    m_glFuncs->glDrawArrays(GL_TRIANGLES, 0, 3);
    // 解除VAO的绑定
    m_glFuncs->glBindVertexArray(0);
}

void OpenGLWidget::checkProgramLinkStatus()
{
    GLint linkStatus;
    m_glFuncs->glGetProgramiv(m_program, GL_LINK_STATUS, &linkStatus);
    if (linkStatus != GL_TRUE) {
        GLchar infoLog[512];
        m_glFuncs->glGetProgramInfoLog(m_program, sizeof(infoLog), nullptr, infoLog);
        qFatal("Program link failed: %s", infoLog);
    }
}

void OpenGLWidget::checkShaderCompileStatus(GLuint shader, const std::string &shaderType)
{
    GLint compileStatus;
    m_glFuncs->glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
    if (compileStatus != GL_TRUE) {
        GLchar infoLog[512];
        m_glFuncs->glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
        qFatal("Shader compile failed: %s, type: %s", infoLog, shaderType.c_str());
    }
}
