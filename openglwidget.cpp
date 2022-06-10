#include "openglwidget.h"

namespace Lightning {
    static GLfloat ambientLight[] = { 0.1, 0.1, 0.1, 1 };
    static GLfloat diffuseLight[] = { 1, 1, 1, 1 };
    static GLfloat specularLight[] = { 0, 0, 0, 1 };
};

struct Image
{
    unsigned long sizeX;
    unsigned long sizeY;
    GLubyte* data;
};

static unsigned int getint(FILE* fp)
{
    unsigned int c, c1, c2, c3;
    c = ((unsigned int)getc(fp));
    c1 = ((unsigned int)getc(fp)) << 8;
    c2 = ((unsigned int)getc(fp)) << 16;
    c3 = ((unsigned int)getc(fp)) << 24;
    return c | c1 | c2 | c3;
}

static unsigned short getshort(FILE* fp)
{
    unsigned short c, c1;
    c = ((unsigned short)getc(fp));
    c1 = ((unsigned short)getc(fp)) << 8;
    return c | c1;
}

/**
 * @brief ImageLoad
 * @param filename - path to file
 * @param image - image object
 * @return true if able to load image, false otherwise
 */
bool ImageLoad(char* filename, Image* image)
{
    FILE* file;
    unsigned long size;
    size_t i, j, k, linediff;
    unsigned short int planes;
    unsigned short int bpp;
    char temp;

    if ((file = fopen(filename, "rb")) == NULL) {
        qDebug() << "File not found:" << filename;
        return false;
    }

    fseek(file, 18, SEEK_CUR);

    image->sizeX = getint(file);
    qDebug() << filename << "width:" << image->sizeX;

    image->sizeY = getint(file);
    qDebug() << filename << "height:" << image->sizeY;

    size = 4.0 * ceil(image->sizeX * 24.0 / 32.0) * image->sizeY;

    planes = getshort(file);
    if (planes != 1) {
        qDebug() << "Planes from" << filename << "is not 1:" << planes;
        return false;
    }

    bpp = getshort(file);
    if (bpp != 24) {
        qDebug() << filename << "Bpp is not 24:" << bpp;
        return 0;
    }

    fseek(file, 24, SEEK_CUR);

    image->data = new GLubyte[size];
    if (image->data == NULL) {
        qDebug() << "Error allocating memory for color-corrected image data";
        return false;
    }

    i = fread(image->data, size, 1, file);
    if (i != 1) {
        qDebug() << "Error reading image data from" << filename;
        return false;
    }

    linediff = 4.0 * ceil(image->sizeX * 24.0 / 32.0) - image->sizeX * 3.0;
    k = 0;
    for (j = 0; j < image->sizeY; j++) {
        for (i = 0; i < image->sizeX; i++) {
            temp = image->data[k];
            image->data[k] = image->data[k + 2];
            image->data[k + 2] = temp;
            k += 3;
        }
        k += linediff;
    }
    return true;
}

/**
 * @brief drawHollowCircle
 * @param x
 * @param y
 * @param radius
 * @param lineAmount
 */
void drawHollowCircle(GLfloat x, GLfloat y, GLfloat radius, int lineAmount = 360){
    GLfloat color[] = {0.23, 0.64, 0.34};
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);
    glLineWidth(3.0);

    GLfloat twicePi = 2.0f * M_PI;

    glBegin(GL_LINE_LOOP);
        for (int i = 0; i <= lineAmount;i++) {
            glVertex2f(
                x + (radius * cos(i *  twicePi / lineAmount)),
                y + (radius* sin(i * twicePi / lineAmount))
            );
        }

    glMaterialfv(GL_FRONT, GL_AMBIENT, Lightning::ambientLight);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, Lightning::diffuseLight);
    glMaterialfv(GL_FRONT, GL_SPECULAR, Lightning::specularLight);

    glEnd();

}

/**
 * @brief OpenGLWidget::LoadTexture
 * @param filename
 */
void OpenGLWidget::LoadTexture(const char* filename) {
    Image* image;

    image = new Image();
    if (image == NULL) {
        qDebug() << "Error allocating space for image";
        exit(0);
    }

    if (!ImageLoad((char *)filename, image)) {
        exit(1);
    }

    glGenTextures(1, &texture[texture_amount]);
    glBindTexture(GL_TEXTURE_2D, texture[texture_amount]);
    texture_amount++;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    glTexImage2D(GL_TEXTURE_2D,     // 2D texture
        0,                  // level of detail 0 (normal)
        3,	                // 3 color components
        image->sizeX,      // x size from image
        image->sizeY,      // y size from image
        0,	                // border 0 (normal)
        GL_RGB,             // rgb color data order
        GL_UNSIGNED_BYTE,   // color component types
        image->data        // image data itself
    );
}

/**
 * @brief OpenGLWidget::LoadGLTextures
 */
void OpenGLWidget::LoadGLTextures() {
    LoadTexture("earthb.bmp");
    LoadTexture("moon.bmp");
};

OpenGLWidget::OpenGLWidget(QWidget *parent)
    : QOpenGLWidget {parent}
{
    setFocusPolicy(Qt::StrongFocus);

    earth = gluNewQuadric();
    moon = gluNewQuadric();
    xRotated = yRotated = zRotated = 0.0;

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(idleFunction()));
    timer->start(0);
}

/**
 * @brief OpenGLWidget::idleFunction
 */
void OpenGLWidget::idleFunction() {
    if (zRotated > 360.0) {
        zRotated -= 360.0;
    }
    if (yRotated > 360.0) {
        yRotated -= 360.0;
    }
    zRotated += 0.4;
    yRotated += 0.4;
    this->update();
}

void OpenGLWidget::keyPressEvent(QKeyEvent *event) {
    switch(event->key()) {
    case 'A':
        camera_angle -= 0.01f;
        dx = sin(camera_angle);
        dz = -cos(camera_angle);
        break;
    case 'D':
        camera_angle += 0.01f;
        dx = sin(camera_angle);
        dz = -cos(camera_angle);
        break;

    case 'W':
        cameraX += dx * camera_fraction;
        cameraZ += dz * camera_fraction;
        break;
    case 'S':
        cameraX -= dx * camera_fraction;
        cameraZ -= dz * camera_fraction;
        break;
    default:
        event->ignore();
        break;
    }
}

void OpenGLWidget::initializeGL() {
    // Some settings
    initializeOpenGLFunctions();

    LoadGLTextures();

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_AMBIENT, Lightning::ambientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, Lightning::diffuseLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, Lightning::specularLight);

    glEnable(GL_TEXTURE_2D);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_RGB);
    glEnable(GL_MULTISAMPLE_ARB);
    glDepthFunc(GL_LEQUAL);
}

/**
 * @brief OpenGLWidget::paintGL
 * @details Maing painting fucntion
 */
void OpenGLWidget::paintGL() {
    glMatrixMode(GL_MODELVIEW);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();


    gluLookAt(cameraX, 1.0f, cameraZ,
        cameraX + dx, 1.0f, cameraZ + dz,
        0.0f, 1.0f, 0.0f);

    glTranslatef(0.0, 0.0, -10.0); // moving a little bit
    glPushMatrix();						// Earth
        glRotatef(-90.0, 1.0, 0.0, 0.0);
        //glRotatef(90.0, 0.0, 1.0, 0.0);
        glTranslatef(0.0, 0.0, 0.0);
        glRotatef(zRotated, earthAngle, 0.0, 1.0); // Angle of the Earth relative to Greenwich.
        glBindTexture(GL_TEXTURE_2D, texture[0]); // Binding Earth's texture
        gluQuadricDrawStyle(earth, GLU_FILL);
        gluQuadricTexture(earth, GL_TRUE);
        gluQuadricNormals(earth, GLU_SMOOTH);
        gluSphere(earth, earthRadius, 25, 25);
    glPopMatrix();

    glPushMatrix(); // Moon Orbit
        glRotatef(-90.0, -1.0, 0.0, earthAngle);
        glRotatef(greenwichAngle, 0.0, 1.0, 0.0); // Angle of the Moon Orbit relative to Greenwich.
        glRotatef(equatorAngle, 1.0, 0.0, 0.0);
        drawHollowCircle(0.0, 0.0, moonOrbite * moonSliderCoefficient);
    glPopMatrix();

    glPushMatrix();						// Moon
        glRotatef(-90.0, -1.0, 0.0, earthAngle);

        glRotatef(greenwichAngle, 0.0, 1.0, 0.0); // Angle of the Moon Orbit relative to Greenwich.
        glRotatef(equatorAngle, 1.0, 0.0, 0.0);

        glRotatef(yRotated, 0.0, 0.0, -1.0); // Moon rotation

        glTranslatef(moonOrbite * moonSliderCoefficient, 0.0, 0.0);	// Moving to Moon Orbit
        glBindTexture(GL_TEXTURE_2D, texture[1]);
        gluQuadricDrawStyle(moon, GLU_FILL);
        gluQuadricTexture(moon, GL_TRUE);
        gluQuadricNormals(moon, GLU_SMOOTH);
        gluSphere(moon, moonRadius, 25, 25);
    glPopMatrix();
    glFlush();
}
void OpenGLWidget::resizeGL(int w, int h) {
    if (w == 0 || h == 0) return;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, GLdouble(w) / GLdouble(h), 0.6, 40.0); // fov
    glMatrixMode(GL_MODELVIEW);
    glViewport(0, 0, w, h);
}
