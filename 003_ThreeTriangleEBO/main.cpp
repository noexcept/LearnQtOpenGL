//
// Created by noexcept on 2025/9/23.
//

#include <QApplication>
#include "OpenGLWidget.h"

int main(int argc, char *argv[])
{
#if defined(Q_OS_APPLE)
    // MacOS最高支持4.1 需要设置一下QSurfaceFormat，不设置初始化4.1版本的函数会失败
    QSurfaceFormat surfaceFormat = QSurfaceFormat::defaultFormat();
    surfaceFormat.setVersion(4, 1);
    surfaceFormat.setRenderableType(QSurfaceFormat::OpenGL);
    surfaceFormat.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(surfaceFormat);
#endif
    QApplication app(argc, argv);
    OpenGLWidget widget;
    widget.show();
    return QApplication::exec();
}
