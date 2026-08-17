#include "volcano.h"
#include "textures.h"
#include <cmath>
#include <cstring>

AppState g_app;

void initAppState(AppState& app) {
    app.state = VolcanoState::DORMANT;
    app.stateTimer = 0.0f;
    app.numParticles = 0;
    app.cameraDist = 25.0f;
    app.cameraAngle = 45.0f;
    app.cameraHeight = 12.0f;
    app.isAnimating = true;
    app.mouseDragging = false;
    app.lastMouseX = 0;
    app.lastMouseY = 0;
    app.moonAngle = 0.0f;
    app.skyboxTextureID = 0;
    app.sunTextureID = 0;

    app.lights.lavaIntensity = 0.3f;
    app.lights.lavaR = 1.0f;
    app.lights.lavaG = 0.3f;
    app.lights.lavaB = 0.0f;
    app.lights.lavaPos[0] = 0.0f;
    app.lights.lavaPos[1] = 3.0f;
    app.lights.lavaPos[2] = 0.0f;
    app.lights.lavaPos[3] = 1.0f;

    app.lights.moonAngle = 0.0f;
    app.lights.moonR = 0.3f;
    app.lights.moonG = 0.4f;
    app.lights.moonB = 0.8f;
    app.lights.moonPos[0] = 5.0f;
    app.lights.moonPos[1] = 8.0f;
    app.lights.moonPos[2] = 5.0f;
    app.lights.moonPos[3] = 0.0f;

    memset(app.particles, 0, sizeof(app.particles));
}

void resetParticles(AppState& app) {
    app.numParticles = 0;
    for (int i = 0; i < 500; i++) {
        app.particles[i].active = false;
        app.particles[i].life = 0.0f;
    }
}

void initOpenGL(AppState& app) {
    glClearColor(0.45f, 0.6f, 0.85f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glShadeModel(GL_SMOOTH);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);

    initLights(app);
}

void initLights(AppState& app) {
    float ambient[] = { 0.5f, 0.45f, 0.4f, 1.0f };
    float diffuse[] = { app.lights.lavaR * app.lights.lavaIntensity,
                        app.lights.lavaG * app.lights.lavaIntensity,
                        app.lights.lavaB * app.lights.lavaIntensity, 1.0f };
    float specular[] = { 1.0f, 0.8f, 0.3f, 1.0f };

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    glLightfv(GL_LIGHT0, GL_POSITION, app.lights.lavaPos);
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.8f);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.3f);

    float sunAmbient[] = { 0.55f, 0.52f, 0.48f, 1.0f };
    float sunDiffuse[] = { 1.0f, 0.95f, 0.88f, 1.0f };
    float sunSpecular[] = { 0.9f, 0.85f, 0.8f, 1.0f };

    glLightfv(GL_LIGHT1, GL_AMBIENT, sunAmbient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, sunDiffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, sunSpecular);
    glLightfv(GL_LIGHT1, GL_POSITION, app.lights.moonPos);
    glLighti(GL_LIGHT1, GL_SPOT_EXPONENT, 0);
    glLighti(GL_LIGHT1, GL_SPOT_CUTOFF, 180);

    float matAmbient[] = { 0.6f, 0.55f, 0.5f, 1.0f };
    float matDiffuse[] = { 0.7f, 0.65f, 0.6f, 1.0f };
    float matSpecular[] = { 0.3f, 0.3f, 0.3f, 1.0f };

    glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
    glMaterialf(GL_FRONT, GL_SHININESS, 16.0f);
}

void updateLights(AppState& app) {
    float angleRad = app.lights.moonAngle * 3.14159265f / 180.0f;
    app.lights.moonPos[0] = 50.0f * cosf(angleRad);
    app.lights.moonPos[1] = 35.0f + 10.0f * sinf(angleRad);
    app.lights.moonPos[2] = 50.0f * sinf(angleRad);

    float intensity = app.lights.lavaIntensity;
    if (app.state == VolcanoState::ERUPTION) intensity *= 2.0f;
    float diffuse[] = { 1.0f * intensity, 0.4f * intensity, 0.1f * intensity, 1.0f };
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);

    float lavaPos[] = { 0.0f, 3.0f + sinf(app.stateTimer * 2.0f) * 0.5f, 0.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lavaPos);
}
