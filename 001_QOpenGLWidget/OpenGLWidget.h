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
protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
private:
    QOpenGLFunctions_4_1_Core *m_glFuncs = nullptr;
};


#endif //LEARNQTOPENGL_OPENGLWIDGET_H