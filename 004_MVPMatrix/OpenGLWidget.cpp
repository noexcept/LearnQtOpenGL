//
// Created by noexcept on 2025/9/23.
//

#include "OpenGLWidget.h"
#include <QMouseEvent>

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
        layout (location = 1) in vec3 aColor;
        out vec3 ourColor;
        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;
        void main()
        {
            gl_Position = projection * view * model * vec4(aPos, 1.0);
            ourColor = aColor;
        }
    )";
    GLuint vertexShader = m_glFuncs->glCreateShader(GL_VERTEX_SHADER);
    m_glFuncs->glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    m_glFuncs->glCompileShader(vertexShader);
    checkShaderCompileStatus(vertexShader, "vertex");

    const char *fragmentShaderSource = R"(
        #version 410 core
        out vec4 FragColor;
        in vec3 ourColor;
        void main()
        {
            FragColor = vec4(ourColor, 1.0f);
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
        -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
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
    // 顶点属性显存布局
    m_glFuncs->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    // 颜色属性
    m_glFuncs->glEnableVertexAttribArray(1);
    // 颜色属性显存布局
    m_glFuncs->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

    // 解除VAO的绑定
    m_glFuncs->glBindVertexArray(0);
    // 解除VBO的绑定
    m_glFuncs->glBindBuffer(GL_ARRAY_BUFFER, 0);

    m_projMatrix = glm::perspective(glm::radians(m_fov), (GLfloat)width() / (GLfloat)height(), 0.1f, 100.0f);
    m_modelMatrix = glm::mat4(1.0f);
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
    m_glFuncs->glEnable(GL_DEPTH_TEST);

    initShaders();

    initVBOAndVAO();

    m_lastMousePos = QPoint(width() / 2, height() / 2);
}


void OpenGLWidget::resizeGL(int w, int h)
{
    QOpenGLWidget::resizeGL(w, h);

    m_glFuncs->glViewport(0, 0, width(), height());

    m_projMatrix = glm::perspective(glm::radians(m_fov), (GLfloat)w / (GLfloat)h, 0.1f, 100.0f);
}

void OpenGLWidget::paintGL() {
    if (!m_glFuncs) {
        qFatal("OpenGL widget paintGL() failed, m_glFuncs is null");
    }
    // 清除颜色缓冲区
    m_glFuncs->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // 使用着色器程序
    m_glFuncs->glUseProgram(m_program);

    m_viewMatrix = glm::lookAt(m_cameraPos, m_cameraPos + m_cameraFront, m_cameraUp);

    GLint modelLoc = m_glFuncs->glGetUniformLocation(m_program, "model");
    m_glFuncs->glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(m_modelMatrix));

    GLint viewLoc = m_glFuncs->glGetUniformLocation(m_program, "view");
    m_glFuncs->glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(m_viewMatrix));

    GLint projLoc = m_glFuncs->glGetUniformLocation(m_program, "projection");
    m_glFuncs->glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(m_projMatrix));

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

void OpenGLWidget::updateCameraVectors()
{
    glm::vec3 front;
    front.x = glm::cos(glm::radians(m_yaw)) * glm::cos(glm::radians(m_pitch));
    front.y = glm::sin(glm::radians(m_pitch));
    front.z = glm::sin(glm::radians(m_yaw)) * glm::cos(glm::radians(m_pitch));
    m_cameraFront = glm::normalize(front);

    m_cameraRight = glm::normalize(glm::cross(m_cameraFront, glm::vec3(0.0f, 1.0f, 0.0f)));
    m_cameraUp = glm::normalize(glm::cross(m_cameraRight, m_cameraFront));

    update();
}

void OpenGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_mousePressed)
    {
        int xPos = event->x();
        int yPos = event->y();

        float xOffset = xPos - m_lastMousePos.x();
        float yOffset = m_lastMousePos.y() - yPos;

        m_lastMousePos = QPoint(xPos, yPos);

        xOffset *= m_mouseSensitivity;
        yOffset *= m_mouseSensitivity;

        m_yaw += xOffset; // 这里是围绕y轴旋转
        m_pitch += yOffset; // 这里是围绕x轴旋转

        if (m_pitch > 89.0f)
        {
            m_pitch = 89.0f;
        }
        if (m_pitch < -89.0f)
        {
            m_pitch = -89.0f;
        }
        updateCameraVectors();
    }
}

void OpenGLWidget::keyPressEvent(QKeyEvent *event)
{

    glm::vec3 changedPos = glm::vec3(0.0f);
    switch (event->key())
    {
        case Qt::Key_W:
            // 按相机当前的方向前进
            changedPos = m_movementSpeed * m_cameraFront;
            break;
        case Qt::Key_S:
            // 按相机当前的方向后退
            changedPos = -(m_movementSpeed * m_cameraFront);
            break;
        case Qt::Key_A:
            // 按相机当前的方向向左移动
            changedPos = -(m_movementSpeed * m_cameraRight);
            break;
        case Qt::Key_D:
            // 按相机当前的方向向右移动
            changedPos = -(m_movementSpeed * m_cameraRight);
            break;
        case Qt::Key_Q:
            // 按相机当前的方向向下移动
            changedPos = -(m_movementSpeed * m_cameraUp);
            break;
        case Qt::Key_E:
            // 按相机当前的方向向上移动
            changedPos = (m_movementSpeed * m_cameraUp);
            break;
    }
    if (changedPos != glm::vec3(0.0f))
    {
        m_cameraPos += changedPos;
        update();
    }
}

void OpenGLWidget::wheelEvent(QWheelEvent *event)
{
    if (event->angleDelta().y() > 0)
    {
        m_fov -= 2.0f;
    }
    else
    {
        m_fov += 2.0f;
    }

    if (m_fov < 1.0f)
    {
        m_fov = 1.0f;
    }
    if (m_fov > 45.0f)
    {
        m_fov = 45.0f;
    }

    glm::mat4 projMatrix = glm::perspective(glm::radians(m_fov), (GLfloat)width() / (GLfloat)height(), 0.1f, 100.0f);
    if (m_projMatrix != projMatrix)
    {
        m_projMatrix = projMatrix;
        update();
    }
}

void OpenGLWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_mousePressed = true;
        m_lastMousePos = event->pos();
        setCursor(Qt::BlankCursor);
    }
}

void OpenGLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_mousePressed = false;
        setCursor(Qt::ArrowCursor);
    }
}