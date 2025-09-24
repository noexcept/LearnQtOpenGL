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

            // 基于PrimitiveID的颜色分配
            if (gl_PrimitiveID == 0) {
                FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
            } else if (gl_PrimitiveID == 1) {
                FragColor = vec4(0.0f, 0.5f, 0.2f, 1.0f);
            } else if (gl_PrimitiveID == 2) {
                FragColor = vec4(0.0f, 0.0f, 0.5f, 1.0f);
            } else {
                FragColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
            }
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

void OpenGLWidget::initVBOAndEBOAndVAO()
{
    GLfloat vertices[] = {
        -0.5f, -0.5f, 0.0f, // 左下角
        0.5f, -0.5f, 0.0f, // 右下角
        0.0f,  0.5f, 0.0f, // 顶部
        1.0f, -1.0f,  0.0f, // 最右下角 第二个三角形顶点 复用 顶点2和1
        -1.0f, 1.0f,  0.0f, // 最左上角 第三个三角形顶点 复用 顶点2和0
    };

    // 定义索引数据
    GLuint indices[] = {
        0, 1, 2, // 第一个三角形
        2, 1, 3, // 第二个三角形 记得顶点的顺序是逆时针为正面哦
        2, 4, 0, // 第三个三角形 记得顶点的顺序是逆时针为正面哦
    };

    // 建议最好一开始就创建VAO并绑定，当VBO创建后，才能设置顶点属性指针
    // VAO一定要先于EBO绑定，否则EBO绑定的数据不会关联到VAO
    // 结论，最佳实践，先绑定 VAO 再绑定 VBO，然后再设置顶点属性指针，最后如有EBO再绑定
    // 创建顶点数组对象
    m_glFuncs->glGenVertexArrays(1, &m_vao);
    // 绑定顶点数组对象
    m_glFuncs->glBindVertexArray(m_vao);

    // 创建顶点缓冲对象
    m_glFuncs->glGenBuffers(1, &m_vbo);
    // 绑定顶点缓冲对象
    m_glFuncs->glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    // 复制顶点数据到缓冲中
    m_glFuncs->glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 创建元素缓冲对象
    m_glFuncs->glGenBuffers(1, &m_ebo);
    // 绑定元素缓冲对象
    m_glFuncs->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    // 复制索引数据到缓冲中
    m_glFuncs->glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // 需要在绑定VBO后进行
    // 启用顶点属性数组，启用这个属性才能glVertexAttribPointer设置顶点属性
    m_glFuncs->glEnableVertexAttribArray(0);
    // 顶点属性指针
    m_glFuncs->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

    // 解除VAO的绑定
    m_glFuncs->glBindVertexArray(0);
    // 解除VBO的绑定
    m_glFuncs->glBindBuffer(GL_ARRAY_BUFFER, 0);
    // 解除EBO的绑定
    m_glFuncs->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
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

    // 初始化着色器
    initShaders();
    // 初始化VBO和EBO以及VAO
    initVBOAndEBOAndVAO();
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
    // m_glFuncs->glDrawArrays(GL_TRIANGLE_STRIP, 0, 5);
    m_glFuncs->glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, (GLvoid*)0);
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
