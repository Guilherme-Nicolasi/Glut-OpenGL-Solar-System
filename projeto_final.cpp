#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

float alfa = 0.0, beta = -5000.0;
float dx = 0.0f, dy = 0.0f, dz = 1.0f;
float x = 90.0, y = 0.0, z = 18.0;
float speed = 0.2f, rotSpeed = 0.005f;
static double rt_sun = 0.0, rt_mercury = 0.0, tr_mercury = 0.0, rt_venus = 0, tr_venus = -15.0, tr_moon = 80.0, tr_earth = -4.0, rt_earth = 0.0, rt_moon = 0.0, rt_mars = 0.0, tr_mars = -20.0, rt_deathstar = 0.0, tr_rocket = 0.0, s_rocket = 0.01, rt_rocket = 0.0;
int count = 0;
GLuint c_deathstar;
GLuint c_rocket;
GLdouble clips[][4] = {{ 0.0, 1.0, 0.0, 0.0 },
                       { 0.0, -1.0, 0.0, 0.0 },
                       { 0.0, 0.5, 0.80, 0.17 }};
GLfloat cta[][2] = {{ -1.0, -1.0 },
                    { 1.0, -1.0 },
                    { 1.0, 1.0 },
                    { -1.0, 1.0 }};

typedef struct BMPImagem
{
    int width;
    int height;
    char *data;
} BMPImage;

#define MAX_NO_TEXTURES 10

GLuint texture_id[MAX_NO_TEXTURES];

char *filenameArray[MAX_NO_TEXTURES] = {
    "sun.bmp",
    "mercury.bmp",
    "venus.bmp",
    "earth.bmp",
    "moon.bmp",
    "mars.bmp",
    "death_star.bmp",
    "nocturne.bmp",
    "metallic.bmp",
    "laser.bmp"
};

void getBitmapImageData(char *pFileName, BMPImage *pImage)
{
    int i;
    FILE *pFile = NULL;
    unsigned short nNumPlanes;
    unsigned short nNumBPP;

    if((pFile = fopen(pFileName, "rb") ) == NULL)
        printf("ERROR: getBitmapImageData - %s not found.\n", pFileName);

    fseek(pFile, 18, SEEK_CUR);

    if((i = fread(&pImage->width, 4, 1, pFile)) != 1)
        printf("ERROR: getBitmapImageData - Couldn't read width from %s.\n ", pFileName);

    if((i = fread(&pImage->height, 4, 1, pFile)) != 1)
        printf("ERROR: getBitmapImageData - Couldn't read height from %s.\n ", pFileName);

    if((fread(&nNumPlanes, 2, 1, pFile)) != 1)
        printf("ERROR: getBitmapImageData - Couldn't read plane count from %s.\n", pFileName);

    if(nNumPlanes != 1)
        printf("ERROR: getBitmapImageData - Plane count from %s.\n ", pFileName);

    if((i = fread(&nNumBPP, 2, 1, pFile)) != 1)
        printf( "ERROR: getBitmapImageData - Couldn't read BPP from %s.\n ", pFileName);

    if(nNumBPP != 24)
        printf("ERROR: getBitmapImageData - BPP from %s.\n ", pFileName);

    fseek(pFile, 24, SEEK_CUR);

    int nTotalImagesize = (pImage->width * pImage->height) * 3;

    pImage->data = (char *) malloc(nTotalImagesize);

    if((i = fread(pImage->data, nTotalImagesize, 1, pFile)) != 1)
        printf("ERROR: getBitmapImageData - Couldn't read image data from %s.\n ", pFileName);

    char charTemp;
    for(i = 0; i < nTotalImagesize; i += 3)
    {
        charTemp = pImage->data[i];
        pImage->data[i] = pImage->data[i + 2];
        pImage->data[i + 2] = charTemp;
    }
}

void CarregaTexturas(void)
{
    int i;
    BMPImage textura;

    glGenTextures(MAX_NO_TEXTURES, texture_id);

    for(i = 0; i < MAX_NO_TEXTURES; i++)
    {
        getBitmapImageData(filenameArray[i], &textura);
        glBindTexture(GL_TEXTURE_2D, texture_id[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, 3, textura.width, textura.height, 0, GL_RGB, GL_UNSIGNED_BYTE, textura.data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }
}

void InitTexture(void)
{
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    CarregaTexturas();
}

void Lightning(void)
{
    GLfloat ambientLight[] = {0.0f, 0.0f, 0.0f, 1.0f};
    GLfloat diffuseLight[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat specularLight[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);

    GLfloat global_ambient[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);

    GLfloat lightPosition[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    GLfloat shininess[] = {100.0f};
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambientLight);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseLight);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specularLight);
    glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
}

void Init(void)
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glShadeModel(GL_SMOOTH);
    glEnable(GL_NORMALIZE);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0, 0.0, z,
              0.0, 0.0, 0.0,
              0.0, 1.0, 0.0);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(50.0, 1.0, 1.0, 200.0);
    glViewport(0, 0, 500, 500);
    Lightning();
    glutSetCursor(GLUT_CURSOR_NONE);
    glutWarpPointer(250, 250);
    glPushMatrix();
}

void Sun(void)
{
    Lightning();
    GLUquadricObj *sphere = gluNewQuadric();
    gluQuadricDrawStyle(sphere, GLU_FILL);
    gluQuadricTexture(sphere, GL_TRUE);
    gluQuadricNormals(sphere, GLU_SMOOTH);
    gluSphere(sphere, 30, 50, 50);
    gluDeleteQuadric(sphere);
}

void Mercury(void)
{
    GLUquadricObj *sphere = gluNewQuadric();
    gluQuadricDrawStyle(sphere, GLU_FILL);
    gluQuadricTexture(sphere, GL_TRUE);
    gluQuadricNormals(sphere, GLU_SMOOTH);
    gluSphere(sphere, 0.4, 50, 50);
    gluDeleteQuadric(sphere);
}

void Venus(void)
{
    GLUquadricObj *sphere = gluNewQuadric();
    gluQuadricDrawStyle(sphere, GLU_FILL);
    gluQuadricTexture(sphere, GL_TRUE);
    gluQuadricNormals(sphere, GLU_SMOOTH);
    gluSphere(sphere, 0.98, 50, 50);
    gluDeleteQuadric(sphere);
}

void Earth(void)
{
    GLUquadricObj *sphere = gluNewQuadric();
    gluQuadricDrawStyle(sphere, GLU_FILL);
    gluQuadricTexture(sphere, GL_TRUE);
    gluQuadricNormals(sphere, GLU_SMOOTH);
    gluSphere(sphere, 1, 50, 50);
    gluDeleteQuadric(sphere);
}

void Moon(void)
{
    GLUquadricObj *sphere = gluNewQuadric();
    gluQuadricDrawStyle(sphere, GLU_FILL);
    gluQuadricNormals(sphere, GLU_SMOOTH);
    gluQuadricTexture(sphere, TRUE);
    gluSphere(sphere, 0.34, 50, 50);
    gluDeleteQuadric(sphere);
}

void Mars(void)
{
    GLUquadricObj *sphere = gluNewQuadric();
    gluQuadricDrawStyle(sphere, GLU_FILL);
    gluQuadricTexture(sphere, GL_TRUE);
    gluQuadricNormals(sphere, GLU_SMOOTH);
    gluSphere(sphere, 0.51, 50, 50);
    gluDeleteQuadric(sphere);
}

void DeathStar(void)
{
    GLUquadricObj *sphere = gluNewQuadric();
    gluQuadricDrawStyle(sphere, GLU_FILL);
    gluQuadricTexture(sphere, GL_TRUE);
    gluQuadricNormals(sphere, GLU_SMOOTH);

    //c_deathstar = glGenLists(4);
    //glNewList(c_deathstar, GL_COMPILE);
        sphere = gluNewQuadric();
        gluQuadricTexture(sphere, GL_TRUE);
        glPushMatrix();
            glRotatef((GLfloat) rt_deathstar, 0.0, 1.0, 0.0);
            glTranslatef(0.0f, 0.01f, 0.0f);
            glClipPlane(GL_CLIP_PLANE0, clips[0]);
            glEnable(GL_CLIP_PLANE0);
            glRotatef(90.0, 1.0, 0.0, 0.0);
            glBindTexture(GL_TEXTURE_2D, texture_id[6]);
            gluSphere(sphere, 0.2, 50, 50);
        glPopMatrix();

        sphere = gluNewQuadric();
        gluQuadricTexture(sphere, GL_TRUE);
        glPushMatrix();
            glTranslatef(0.0f, -0.01f, 0.0f);
            glClipPlane(GL_CLIP_PLANE0, clips[1]);
            glEnable(GL_CLIP_PLANE0);
            glClipPlane(GL_CLIP_PLANE0 + 1, clips[2]);
            glEnable(GL_CLIP_PLANE0 + 1);
            glRotatef(90.0, 1.0, 0.0, 0.0);
            glBindTexture(GL_TEXTURE_2D, texture_id[6]);
            gluSphere(sphere, 0.2, 50, 50);
        glPopMatrix();

        sphere = gluNewQuadric();
        gluQuadricTexture(sphere, GL_FALSE);
        glPushMatrix();
            glDisable(GL_CLIP_PLANE0);
            gluSphere(sphere, 0.14, 50, 50);
        glPopMatrix();

        glPushMatrix();
            sphere = gluNewQuadric();
            gluQuadricTexture(sphere, GL_FALSE);
            glPushMatrix();
                glTranslatef(0.0f, -0.125f, -0.18f);
                glRotatef(60, 1.0, 0.0, 0.0);
                glTranslatef(0.0f, 0.0f, 0.0f);
                glClipPlane(GL_CLIP_PLANE0, clips[0]);
                glEnable(GL_CLIP_PLANE0);
                gluSphere(sphere, 0.0852, 50, 50);
            glPopMatrix();
        glPopMatrix();
    //glEndList();

    glDisable(GL_CLIP_PLANE0);
    glDisable(GL_CLIP_PLANE0 + 1);
    gluDeleteQuadric(sphere);
}

void Rocket(void)
{
    GLUquadricObj *quadric = gluNewQuadric();
    gluQuadricDrawStyle(quadric, GLU_FILL);
    gluQuadricNormals(quadric, GLU_SMOOTH);
    gluQuadricTexture(quadric, TRUE);

    GLfloat wing[][3] = {{ -4.0, 0.0, 0.0 },
                        { 4.0, 0.0, 0.0 },
                        { 0.0, 0.0, 3.0 }};

    GLfloat tail[][3] = {{ 0.0, 0.0, 0.0 },
                          { 0.0, 2.0, -1.0 },
                          { 0.0, 2.0, 0.0 },
                          { 0.0, 0.0, 2.0 }};

    //c_rocket = glGenLists(1);
    //glNewList(c_rocket, GL_COMPILE);
        quadric = gluNewQuadric();
        gluQuadricTexture(quadric, GL_TRUE);
        glBegin(GL_TRIANGLES);
            glTexCoord2fv(cta[0]); glVertex3fv(wing[0]);
            glTexCoord2fv(cta[1]); glVertex3fv(wing[1]);
            glTexCoord2fv(cta[3]); glVertex3fv(wing[2]);
        glEnd();

        quadric = gluNewQuadric();
        gluQuadricTexture(quadric, GL_TRUE);
        gluCylinder(quadric, 0.5, 0.5, 4, 12, 3);

        quadric = gluNewQuadric();
        gluQuadricTexture(quadric, GL_TRUE);
        glPushMatrix();
            glTranslatef(0,0,4);
            gluCylinder(quadric, 0.5, 0.0, 1.5, 12, 3);
        glPopMatrix();

        quadric = gluNewQuadric();
        gluQuadricTexture(quadric, GL_TRUE);
        glBegin(GL_POLYGON);
            glTexCoord2fv(cta[0]); glVertex3fv(tail[0]);
            glTexCoord2fv(cta[1]); glVertex3fv(tail[1]);
            glTexCoord2fv(cta[2]); glVertex3fv(tail[2]);
            glTexCoord2fv(cta[3]); glVertex3fv(tail[3]);
        glEnd();

        glTranslatef(0, 0.3, 3.5);
        glPushMatrix();
            glScalef(0.7, 0.7, 2.0);
            quadric = gluNewQuadric();
            glBindTexture(GL_TEXTURE_2D, texture_id[1]);
            gluSphere(quadric, 0.5, 12, 12);
        glPopMatrix();
    //glEndList();

    gluDeleteQuadric(quadric);
}

void Laser(void)
{
    GLUquadricObj *cylinder = gluNewQuadric();
    gluQuadricDrawStyle(cylinder, GLU_FILL);
    gluQuadricTexture(cylinder, GL_TRUE);
    gluQuadricNormals(cylinder, GLU_SMOOTH);
    cylinder = gluNewQuadric();
    gluQuadricTexture(cylinder, GL_TRUE);
    gluCylinder(cylinder, 0.01, 0.05, 2.0, 50, 50);
    gluDeleteQuadric(cylinder);
}

void DisplayFunc(void)
{
    GLfloat objectEmissionLight[] = {0.0, 0.0, 0.0, 1.0};
    GLfloat sunEmissionLight[] = {1.0, 1.0, 1.0, 1.0};
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(x, y, z,
              x + dx, y + dy, z + dz,
              0.0, 1.0, 0.0);

    glPushMatrix();
        glPushMatrix();
            glMaterialfv(GL_FRONT, GL_EMISSION, sunEmissionLight);
            glPushMatrix();
                glRotatef((GLfloat) rt_sun, 0.0, 1.0, 0.0);
                glRotatef(-90.0f, 1.0, 0.0, 0.0);
                glBindTexture(GL_TEXTURE_2D, texture_id[0]);
                Sun();
            glPopMatrix();
        glPopMatrix();
        glPushMatrix();
            glMaterialfv(GL_FRONT, GL_EMISSION, objectEmissionLight);
            glPushMatrix();
                glRotatef((GLfloat) tr_mercury, 0.0, 1.0, 0.0);
                glPushMatrix();
                    glTranslatef(65, 0.0, 0.0);
                    glRotatef((GLfloat) rt_mercury, 0.0, 1.0, 0.0);
                    glRotatef(-90.0f, 1.0, 0.0, 0.0);
                    glBindTexture(GL_TEXTURE_2D, texture_id[1]);
                    Mercury();
                glPopMatrix();
            glPopMatrix();
            glPushMatrix();
                glRotatef((GLfloat) tr_venus, 0.0, 1.0, 0.0);
                glPushMatrix();
                    glTranslatef(80.0, 0.0, 0.0);
                    glRotatef((GLfloat) rt_venus, 0.0, 1.0, 0.0);
                    glRotatef(-90.0f, 1.0, 0.0, 0.0);
                    glBindTexture(GL_TEXTURE_2D, texture_id[2]);
                    Venus();
                glPopMatrix();
            glPopMatrix();
            glPushMatrix();
                glRotatef((GLfloat) tr_earth, 0.0, 1.0, 0.0);
                glPushMatrix();
                    glTranslatef(90.0, 0.0, 0.0);
                    glRotatef((GLfloat) rt_earth, 0.0, 1.0, 0.0);
                    glRotatef(-110.0f, 1.0, 0.0, 0.0);
                    glBindTexture(GL_TEXTURE_2D, texture_id[3]);
                    Earth();
                glPopMatrix();
                glPushMatrix();
                    glTranslatef(90.0, 0.0, 0.0);
                    glRotatef((GLfloat) tr_moon, 0.0, 1.0, 0.0);
                    glPushMatrix();
                        glTranslatef(2.0, 0.0, 0.0);
                        glRotatef((GLfloat) rt_moon, 0.0, 1.0, 0.0);
                        glRotatef(-90.0f, 1.0, 0.0, 0.0);
                        glBindTexture(GL_TEXTURE_2D, texture_id[4]);
                        Moon();
                    glPopMatrix();
                glPopMatrix();
                    glPushMatrix();
                    glTranslatef(90.0, 0.0, 0.0);
                    glPushMatrix();
                        glTranslatef(4.0, 0.0, 1.0);
                        glRotatef((GLfloat) rt_deathstar, 0.0, 1.0, 0.0);
                        glRotatef(180.0f, 1.0, 0.0, 0.0);
                        DeathStar();
                    glPopMatrix();
                glPopMatrix();
                /*glPushMatrix();
                    glTranslatef(90.0, 0.0, 0.0);
                    glPushMatrix();
                        glTranslatef(4.0, 0.0, 1.0);
                        glRotatef(-90.0f, 0.0, 0.0, 1.0);
                        glBindTexture(GL_TEXTURE_2D, texture_id[9]);
                        if(rt_deathstar <= -120) Laser();
                    glPopMatrix();
                glPopMatrix();*/
            glPopMatrix();
            glPushMatrix();
                glTranslatef(0.0, 0.0, tr_rocket);
                glPushMatrix();
                    glTranslatef(90.0, 0.0, 6.0);
                    glScalef(s_rocket, s_rocket, s_rocket);
                    glBindTexture(GL_TEXTURE_2D, texture_id[10]);
                    Rocket();
                    //glCallList(c_rocket);
                glPopMatrix();
            glPopMatrix();
            glPushMatrix();
                glRotatef((GLfloat) tr_mars, 0.0, 1.0, 0.0);
                glPushMatrix();
                    glTranslatef(115.0, 0.0, 0.0);
                    glRotatef((GLfloat) rt_mars, 1.0, 0.0, 0.0);
                    glRotatef(-90.0f, 1.0, 0.0, 0.0);
                    glBindTexture(GL_TEXTURE_2D, texture_id[5]);
                    Mars();
                glPopMatrix();
            glPopMatrix();
        glPopMatrix();
    glPopMatrix();

    glFlush();

    glutSwapBuffers();
}

void IdleFunc(void)
{
    rt_sun += 0.0009;
    rt_mercury += 0.0005;
    rt_venus += 0.00005;
    rt_earth += 0.005;
    rt_moon += 0.00085;
    rt_mars += 0.00498;
    tr_mercury += 0.00035;
    tr_venus += 0.00006;
    tr_earth += 0.00002;
    tr_moon += 0.00085;
    tr_mars += 0.000018;
    if(rt_deathstar > -120.0) rt_deathstar -= 0.0009;
    tr_rocket += 0.0005;
    if(s_rocket < 25.0) s_rocket += 0.000001;
    glutPostRedisplay();
}

void KeyBoard(unsigned char key, int kx, int ky)
{
	switch(key)
	{
		case 'w':
        case 'W':
			x += dx * speed;
			y += dy * speed;
			z += dz * speed;
        break;
		case 's':
        case 'S':
			x -= dx * speed;
			y -= dy * speed;
            z -= dz * speed;
        break;
        case 'a':
        case 'A':
			x += dz * speed;
            z -= dx * speed;
        break;
        case 'd':
        case 'D':
			x -= dz * speed;
            z += dx * speed;
        break;
        default:
        break;
    }
}

void Mouse(int mx, int my)
{
    int mdx = (mx - 250), mdy = (my - 250);
    alfa += mdx * rotSpeed;
    beta += mdy * rotSpeed;
    dx = sin(alfa);
    dy = cos(beta);
    dz = -sin(beta) * cos(alfa);
    glutWarpPointer(250, 250);
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(0, 0);
    glutCreateWindow(argv[0]);
    Init();
    InitTexture();
    glutDisplayFunc(DisplayFunc);
    glutIdleFunc(IdleFunc);
    glutKeyboardFunc(KeyBoard);
    glutPassiveMotionFunc(Mouse);
    glutMainLoop();
    return 0;
}
