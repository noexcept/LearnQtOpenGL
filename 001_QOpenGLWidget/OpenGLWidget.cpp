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
    if (m_glFuncs) {
        delete m_glFuncs;
        m_glFuncs = nullptr;
    }
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
}

void OpenGLWidget::resizeGL(int w, int h)
{
    QOpenGLWidget::resizeGL(w, h);
}

void OpenGLWidget::paintGL() {
    if (!m_glFuncs) {
        qFatal("OpenGL widget paintGL() failed, m_gl is null");
    }
    // 清除颜色缓冲区
    m_glFuncs->glClear(GL_COLOR_BUFFER_BIT);
    QOpenGLWidget::paintGL();
}
