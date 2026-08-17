#include "volcano.h"

void mouse(AppState& app, int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        app.mouseDragging = true;
        app.lastMouseX = x;
        app.lastMouseY = y;
    } else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        app.mouseDragging = false;
    } else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
        app.cameraDist -= 1.0f;
        if (app.cameraDist < 5.0f) app.cameraDist = 5.0f;
        glutPostRedisplay();
    } else if (button == 3 && state == GLUT_DOWN) {
        app.cameraDist -= 1.0f;
        if (app.cameraDist < 5.0f) app.cameraDist = 5.0f;
        glutPostRedisplay();
    } else if (button == 4 && state == GLUT_DOWN) {
        app.cameraDist += 1.0f;
        if (app.cameraDist > 25.0f) app.cameraDist = 25.0f;
        glutPostRedisplay();
    }
}

void mouseMotion(AppState& app, int x, int y) {
    if (app.mouseDragging) {
        int dx = x - app.lastMouseX;
        int dy = y - app.lastMouseY;
        app.cameraAngle += dx * 0.5f;
        app.cameraHeight -= dy * 0.1f;
        if (app.cameraHeight < 2.0f) app.cameraHeight = 2.0f;
        if (app.cameraHeight > 20.0f) app.cameraHeight = 20.0f;
        app.lastMouseX = x;
        app.lastMouseY = y;
    }
}
