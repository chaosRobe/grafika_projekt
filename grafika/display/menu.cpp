#include "volcano.h"

extern AppState g_app;

void createMenu(AppState& app) {
    int menu = glutCreateMenu([](int v) { menuCallback(g_app, v); });
    glutAddMenuEntry("Stan: Uspiony", 1);
    glutAddMenuEntry("Stan: Aktywny", 2);
    glutAddMenuEntry("Stan: Erupcja", 3);
    glutAddMenuEntry("---", 0);
    glutAddMenuEntry("Reset", 99);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void menuCallback(AppState& app, int value) {
    switch (value) {
        case 1:
            app.state = VolcanoState::DORMANT;
            resetParticles(app);
            initSmokeParticles();
            app.lights.lavaIntensity = 0.2f;
            break;
        case 2:
            app.state = VolcanoState::ACTIVE;
            resetParticles(app);
            initSmokeParticles();
            app.lights.lavaIntensity = 0.5f;
            break;
        case 3:
            app.state = VolcanoState::ERUPTION;
            resetParticles(app);
            initSmokeParticles();
            app.lights.lavaIntensity = 1.0f;
            break;
        case 99:
            app.cameraDist = 50.0f;
            app.cameraAngle = 45.0f;
            app.cameraHeight = 25.0f;
            app.state = VolcanoState::DORMANT;
            resetParticles(app);
            initSmokeParticles();
            app.lights.lavaIntensity = 0.3f;
            break;
    }
    glutPostRedisplay();
}
