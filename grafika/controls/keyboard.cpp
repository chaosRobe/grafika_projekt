#include "volcano.h"
#include <cmath>

void keyboard(AppState& app, unsigned char key, int x, int y) {
    switch (key) {
        case 27:
            exit(0);
            break;
        case '1':
            app.state = VolcanoState::DORMANT;
            resetParticles(app);
            app.lights.lavaIntensity = 0.2f;
            break;
        case '2':
            app.state = VolcanoState::ACTIVE;
            resetParticles(app);
            app.lights.lavaIntensity = 0.5f;
            break;
        case '3':
            app.state = VolcanoState::ERUPTION;
            resetParticles(app);
            app.lights.lavaIntensity = 1.0f;
            break;
        case '+':
        case '=':
            app.lights.lavaIntensity = fmin(1.5f, app.lights.lavaIntensity + 0.1f);
            break;
        case '-':
            app.lights.lavaIntensity = fmax(0.1f, app.lights.lavaIntensity - 0.1f);
            break;
        case 'w':
            app.cameraDist = fmax(15.0f, app.cameraDist - 0.5f);
            break;
        case 's':
            app.cameraDist = fmin(120.0f, app.cameraDist + 0.5f);
            break;
        case 'q':
            app.cameraHeight = fmax(1.0f, app.cameraHeight - 0.5f);
            break;
        case 'e':
            app.cameraHeight = fmin(60.0f, app.cameraHeight + 0.5f);
            break;
        case 'r':
            app.cameraDist = 50.0f;
            app.cameraAngle = 45.0f;
            app.cameraHeight = 25.0f;
            app.state = VolcanoState::DORMANT;
            resetParticles(app);
            app.lights.lavaIntensity = 0.3f;
            break;
    }
    glutPostRedisplay();
}

void specialKeys(AppState& app, int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_LEFT:
            app.cameraAngle -= 5.0f;
            break;
        case GLUT_KEY_RIGHT:
            app.cameraAngle += 5.0f;
            break;
        case GLUT_KEY_UP:
            app.cameraDist = fmax(15.0f, app.cameraDist - 0.5f);
            break;
        case GLUT_KEY_DOWN:
            app.cameraDist = fmin(120.0f, app.cameraDist + 0.5f);
            break;
    }
    glutPostRedisplay();
}
