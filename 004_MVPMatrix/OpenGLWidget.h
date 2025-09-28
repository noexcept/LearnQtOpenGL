//
// Created by noexcept on 2025/9/23.
//

#ifndef LEARNQTOPENGL_OPENGLWIDGET_H
#define LEARNQTOPENGL_OPENGLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_1_Core>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

class OpenGLWidget : public QOpenGLWidget
{
    Q_OBJECT
public:
    OpenGLWidget(QWidget* parent = nullptr);
    ~OpenGLWidget();
    void initShaders();
    void initVBOAndVAO();

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void checkShaderCompileStatus(GLuint shader, const std::string &shaderType);
    void checkProgramLinkStatus();

    void mouseMoveEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void updateCameraVectors();
private:
    QOpenGLFunctions_4_1_Core *m_glFuncs = nullptr;
    // 顶点数组对象
    GLuint m_vao = 0;
    // 顶点缓冲对象
    GLuint m_vbo = 0;
    // 着色器程序
    GLuint m_program = 0;

    glm::mat4 m_projMatrix;
    glm::mat4 m_viewMatrix;
    glm::mat4 m_modelMatrix;

    // 相机的位置
    glm::vec3 m_cameraPos = glm::vec3(0.0f, 0.0f, 1.5f);
    // 相机正前方，即是指向拍摄的方向，初始化与z轴方向一致
    glm::vec3 m_cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    // 相机的向上方向，初始化与y轴正方向一致
    glm::vec3 m_cameraUp = glm::vec3(0.0, 1.0f, 0.0f);
    // 相机的向右方向，初始化与x轴正方向一致
    glm::vec3 m_cameraRight = glm::vec3(1.0f, 0.0f, 0.0f);
    // 相机旋转角度，这里是围绕y轴旋转，也叫偏航角，左右摇头，初始化和z轴正方向一致, x轴正方向到z轴正方向就是-90度
    float m_yaw = -90.0f;
    // 相机旋转角度，这里是围绕x轴旋转，也叫俯仰角，抬头低头
    float m_pitch = 0.0f;
    float m_movementSpeed = 0.05f;
    float m_mouseSensitivity = 0.1f;
    bool m_mousePressed = false;
    QPoint m_lastMousePos;
    // 相机的视野角度，初始化45度，想象一个相机的视野角度，
    // 越大，相机的视野范围就越大（能看见的物体范围越广），越小，相机的视野范围就越小（物体看起来就像被放大一样）
    float m_fov = 45.0f;
};


#endif //LEARNQTOPENGL_OPENGLWIDGET_H
