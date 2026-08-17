#include "volcano.h"
#include <cmath>

extern AppState g_app;

extern AppState g_app;

void reshape(int w, int h) {
    if (h <= 0) h = 1;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(50.0f, (float)w / (float)h, 0.1f, 100.0f);
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

    if (g_app.state != VolcanoState::DORMANT) {
        g_app.smokeOffsetX += sinf(g_app.stateTimer * 0.7f) * 0.003f;
        g_app.smokeOffsetZ += cosf(g_app.stateTimer * 0.9f) * 0.003f;
        if (g_app.state == VolcanoState::ERUPTION) {
            g_app.smokeScale = 1.0f + sinf(g_app.stateTimer * 3.0f) * 0.15f;
            g_app.smokeY = 5.5f + sinf(g_app.stateTimer * 2.0f) * 0.3f;
        } else {
            g_app.smokeScale = 1.0f + sinf(g_app.stateTimer * 1.5f) * 0.08f;
            g_app.smokeY = 5.5f + sinf(g_app.stateTimer * 1.0f) * 0.15f;
        }
    } else {
        g_app.smokeY = 5.5f;
        g_app.smokeScale = 0.6f;
        g_app.smokeOffsetX *= 0.95f;
        g_app.smokeOffsetZ *= 0.95f;
    }

    glutPostRedisplay();
}
