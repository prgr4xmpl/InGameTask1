#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <cmath>
#include <windows.h>
#include <GL/glu.h>
#include <QTimer>
#include <QKeyEvent>
#include <QDebug>

class OpenGLWidget : public QOpenGLWidget, public QOpenGLFunctions
{
    Q_OBJECT
public:
    OpenGLWidget(QWidget *parent = nullptr);

    GLfloat moonSliderCoefficient = 1.0;
    GLfloat greenwichAngle = 23.0; // current "greenwichAngle"
    GLfloat equatorAngle = 8.0;

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;
    void keyPressEvent(QKeyEvent *event) override;

    void LoadTexture(const char* filename);
    void LoadGLTextures();

private:
    GLuint texture[2];
    int texture_amount = 0;

    GLUquadricObj* earth = NULL;
    GLUquadricObj* moon = NULL;

    GLdouble earthRadius = 2;
    GLdouble moonRadius = earthRadius / 4;
    GLfloat moonOrbite = 5.0;
    GLfloat camera_fraction = 0.3; // camera "speed"
    GLfloat baseGreenwichAngle = 23.0 / 100.0;

    GLfloat xRotated, yRotated, zRotated;

    GLfloat camera_angle = 0.0;
    GLfloat dx = 0.0, dz = -1.0;
    GLfloat cameraX = 0.0, cameraZ = 5.0;

protected slots:
    void idleFunction();
};

#endif // OPENGLWIDGET_H
