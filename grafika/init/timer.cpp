#include "volcano.h"
#include <cmath>

extern AppState g_app;

void reshape(int w, int h) {
    if (h <= 0) h = 1;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(50.0f, (float)w / (float)h, 0.1f, 500.0f);
    glMatrixMode(GL_MODELVIEW);
}

void timerFunc(int value) {
    glutTimerFunc(16, timerFunc, 0);

    static float lastTime = 0.0f;
    float currentTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    float dt = currentTime - lastTime;
    if (dt > 0.1f) dt = 0.016f;
    lastTime = currentTime;

    g_app.stateTimer += dt;

    updateParticles(g_app, dt);
    emitParticles(g_app);

    glutPostRedisplay();
}
