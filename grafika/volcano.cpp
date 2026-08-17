#include "volcano.h"
#include "textures.h"
#include <cmath>
#include <cstring>

AppState g_app;

void initAppState(AppState& app) {
    app.state = VolcanoState::DORMANT;
    app.stateTimer = 0.0f;
    app.numParticles = 0;
    app.cameraDist = 12.0f;
    app.cameraAngle = 45.0f;
    app.cameraHeight = 8.0f;
    app.isAnimating = true;

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
    glClearColor(0.02f, 0.02f, 0.06f, 1.0f);
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
    // Light 0 - lava glow (point light)
    float ambient[] = { 0.02f, 0.01f, 0.0f, 1.0f };
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

    // Light 1 - moon light (directional)
    float moonAmbient[] = { 0.05f, 0.06f, 0.12f, 1.0f };
    float moonDiffuse[] = { app.lights.moonR, app.lights.moonG, app.lights.moonB, 1.0f };
    float moonSpecular[] = { 0.4f, 0.5f, 0.9f, 1.0f };

    glLightfv(GL_LIGHT1, GL_AMBIENT, moonAmbient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, moonDiffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, moonSpecular);
    glLightfv(GL_LIGHT1, GL_POSITION, app.lights.moonPos);

    // Default material
    float matAmbient[] = { 0.4f, 0.35f, 0.3f, 1.0f };
    float matDiffuse[] = { 0.5f, 0.45f, 0.35f, 1.0f };
    float matSpecular[] = { 0.2f, 0.2f, 0.2f, 1.0f };

    glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
    glMaterialf(GL_FRONT, GL_SHININESS, 16.0f);
}

void updateLights(AppState& app) {
    float angleRad = app.lights.moonAngle * 3.14159265f / 180.0f;
    app.lights.moonPos[0] = 8.0f * cos(angleRad);
    app.lights.moonPos[1] = 6.0f + 2.0f * sin(angleRad);
    app.lights.moonPos[2] = 8.0f * sin(angleRad);

    float intensity = app.lights.lavaIntensity;
    float diffuse[] = { 1.0f * intensity, 0.3f * intensity, 0.0f * intensity, 1.0f };
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);

    float lavaPos[] = { 0.0f, 2.5f + sin(app.stateTimer * 2.0f) * 0.3f, 0.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lavaPos);
}

void drawVolcano() {
    bindRockTexture();
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    float segments = 32;
    float volcanoHeight = 4.0f;
    float baseRadius = 3.5f;
    float craterRadius = 0.6f;
    float craterDepth = 0.4f;

    for (int i = 0; i < (int)segments; i++) {
        float a1 = i * 2.0f * 3.14159265f / segments;
        float a2 = (i + 1) * 2.0f * 3.14159265f / segments;

        float c1 = cos(a1), s1 = sin(a1);
        float c2 = cos(a2), s2 = sin(a2);

        float y1 = 0.0f;
        float y2 = volcanoHeight * 0.7f;
        float y3 = volcanoHeight;
        float y4 = volcanoHeight - craterDepth;

        float r1 = baseRadius;
        float r2 = baseRadius * 0.75f;
        float r3 = craterRadius * 1.3f;
        float r4 = craterRadius;

        float u1 = (float)i / (float)segments;
        float u2 = (float)(i + 1) / (float)segments;
        float v1 = 0.0f;
        float v2 = 0.5f;
        float v3 = 0.85f;
        float v4 = 1.0f;

        glBegin(GL_TRIANGLE_STRIP);

        glNormal3f(c1, 0.3f, s1);
        glTexCoord2f(u1, v1);
        glVertex3f(r1 * c1, y1, r1 * s1);
        glNormal3f(c1, 0.3f, s1);
        glTexCoord2f(u1, v2);
        glVertex3f(r2 * c1, y2, r2 * s1);

        glNormal3f(c2, 0.3f, s2);
        glTexCoord2f(u2, v1);
        glVertex3f(r1 * c2, y1, r1 * s2);
        glNormal3f(c2, 0.3f, s2);
        glTexCoord2f(u2, v2);
        glVertex3f(r2 * c2, y2, r2 * s2);

        glNormal3f(c1, 0.8f, s1);
        glTexCoord2f(u1, v2);
        glVertex3f(r2 * c1, y2, r2 * s1);
        glNormal3f(c1, 0.8f, s1);
        glTexCoord2f(u1, v3);
        glVertex3f(r3 * c1, y3, r3 * s1);

        glNormal3f(c2, 0.8f, s2);
        glTexCoord2f(u2, v2);
        glVertex3f(r2 * c2, y2, r2 * s2);
        glNormal3f(c2, 0.8f, s2);
        glTexCoord2f(u2, v3);
        glVertex3f(r3 * c2, y3, r3 * s2);

        glNormal3f(c1, -0.3f, s1);
        glTexCoord2f(u1, v3);
        glVertex3f(r3 * c1, y3, r3 * s1);
        glNormal3f(c1, -0.3f, s1);
        glTexCoord2f(u1, v4);
        glVertex3f(r4 * c1, y4, r4 * s1);

        glNormal3f(c2, -0.3f, s2);
        glTexCoord2f(u2, v3);
        glVertex3f(r3 * c2, y3, r3 * s2);
        glNormal3f(c2, -0.3f, s2);
        glTexCoord2f(u2, v4);
        glVertex3f(r4 * c2, y4, r4 * s2);

        glEnd();
    }
}

void drawLavaLake() {
    bindLavaTexture();
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    float segments = 24;
    float innerRadius = 1.5f;
    float outerRadius = 3.0f;

    glBegin(GL_TRIANGLE_STRIP);
    for (int i = 0; i <= segments; i++) {
        float a = i * 2.0f * 3.14159265f / segments;
        float c = cos(a), s = sin(a);

        float u = (float)i / (float)segments;

        glNormal3f(0.0f, 1.0f, 0.0f);
        glTexCoord2f(u, 0.0f);
        glVertex3f(innerRadius * c, -0.05f, innerRadius * s);
        glTexCoord2f(u, 1.0f);
        glVertex3f(outerRadius * c, -0.05f, outerRadius * s);
    }
    glEnd();
}

void drawSmokeCloud() {
    bindSmokeTexture();
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glPushMatrix();
    glTranslatef(0.0f, 5.5f, 0.0f);
    glutSolidSphere(0.8f, 16, 12);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.5f, 6.2f, 0.3f);
    glutSolidSphere(0.5f, 12, 10);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.4f, 6.0f, -0.3f);
    glutSolidSphere(0.6f, 12, 10);
    glPopMatrix();
}

void drawParticles(AppState& app) {
    for (int i = 0; i < 500; i++) {
        if (!app.particles[i].active) continue;

        float alpha = app.particles[i].life / app.particles[i].maxLife;
        float r = 1.0f;
        float g = 0.2f + 0.5f * alpha;
        float b = 0.0f;

        glColor4f(r, g, b, alpha);
        glPointSize(3.0f + alpha * 3.0f);

        glBegin(GL_POINTS);
        glVertex3f(app.particles[i].x, app.particles[i].y, app.particles[i].z);
        glEnd();
    }
}

void updateParticles(AppState& app, float dt) {
    for (int i = 0; i < 500; i++) {
        if (!app.particles[i].active) continue;

        app.particles[i].x += app.particles[i].vx * dt;
        app.particles[i].y += app.particles[i].vy * dt;
        app.particles[i].z += app.particles[i].vz * dt;

        app.particles[i].vy -= 3.0f * dt;
        app.particles[i].life -= dt;

        if (app.particles[i].life <= 0.0f) {
            app.particles[i].active = false;
        }
    }
}

void emitParticles(AppState& app) {
    int count = 0;
    for (int i = 0; i < 500; i++) {
        if (app.particles[i].active) count++;
    }

    int maxParticles = 0;
    int emitRate = 0;

    switch (app.state) {
        case VolcanoState::DORMANT:
            maxParticles = 0;
            emitRate = 0;
            break;
        case VolcanoState::ACTIVE:
            maxParticles = 30;
            emitRate = 2;
            break;
        case VolcanoState::ERUPTION:
            maxParticles = 300;
            emitRate = 8;
            break;
    }

    for (int i = 0; i < emitRate && count < maxParticles; i++) {
        for (int j = 0; j < 500; j++) {
            if (!app.particles[j].active) {
                app.particles[j].active = true;
                app.particles[j].x = 0.0f;
                app.particles[j].y = 3.5f;
                app.particles[j].z = 0.0f;

                float spread = 0.5f;
                if (app.state == VolcanoState::ERUPTION) spread = 1.5f;

                app.particles[j].vx = ((float)rand() / RAND_MAX - 0.5f) * spread;
                app.particles[j].vy = 3.0f + (float)rand() / RAND_MAX * 4.0f;
                if (app.state == VolcanoState::ERUPTION) app.particles[j].vy += 3.0f;
                app.particles[j].vz = ((float)rand() / RAND_MAX - 0.5f) * spread;

                app.particles[j].maxLife = 1.5f + (float)rand() / RAND_MAX * 2.0f;
                app.particles[j].life = app.particles[j].maxLife;

                count++;
                break;
            }
        }
    }
}

void displayScene(AppState& app) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    float angleRad = app.cameraAngle * 3.14159265f / 180.0f;
    gluLookAt(
        app.cameraDist * sin(angleRad),
        app.cameraHeight,
        app.cameraDist * cos(angleRad),
        0.0f, 1.5f, 0.0f,
        0.0f, 1.0f, 0.0f
    );

    updateLights(app);

    float lavaColor[] = { 0.3f, 0.25f, 0.2f, 1.0f };
    float lavaEmissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };

    if (app.state == VolcanoState::ACTIVE) {
        lavaColor[0] = 0.45f; lavaColor[1] = 0.3f; lavaColor[2] = 0.2f;
        lavaEmissive[0] = 0.1f; lavaEmissive[1] = 0.05f;
    } else if (app.state == VolcanoState::ERUPTION) {
        lavaColor[0] = 0.55f; lavaColor[1] = 0.3f; lavaColor[2] = 0.15f;
        float pulse = 0.15f + 0.1f * sin(app.stateTimer * 5.0f);
        lavaEmissive[0] = pulse; lavaEmissive[1] = pulse * 0.5f;
    }

    glMaterialfv(GL_FRONT, GL_AMBIENT, lavaColor);
    glMaterialfv(GL_FRONT, GL_EMISSION, lavaEmissive);

    drawVolcano();

    float lakeColor[] = { 0.6f, 0.2f, 0.05f, 1.0f };
    float lakeEmissive[] = { 0.3f, 0.1f, 0.0f, 1.0f };
    glMaterialfv(GL_FRONT, GL_AMBIENT, lakeColor);
    glMaterialfv(GL_FRONT, GL_EMISSION, lakeEmissive);
    glMaterialf(GL_FRONT, GL_SHININESS, 80.0f);
    drawLavaLake();

    if (app.state != VolcanoState::DORMANT) {
        float smokeAlpha = 0.4f;
        if (app.state == VolcanoState::ERUPTION) smokeAlpha = 0.7f;

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        float smokeColor[] = { 0.4f, 0.3f, 0.25f, smokeAlpha };
        glMaterialfv(GL_FRONT, GL_AMBIENT, smokeColor);
        glMaterialfv(GL_FRONT, GL_EMISSION, smokeColor);

        drawSmokeCloud();
        glDisable(GL_BLEND);
    }

    drawParticles(app);

    glutSwapBuffers();
}

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
            app.cameraDist = fmax(5.0f, app.cameraDist - 0.5f);
            break;
        case 's':
            app.cameraDist = fmin(25.0f, app.cameraDist + 0.5f);
            break;
        case 'q':
            app.cameraHeight = fmax(1.0f, app.cameraHeight - 0.5f);
            break;
        case 'e':
            app.cameraHeight = fmin(20.0f, app.cameraHeight + 0.5f);
            break;
        case 'r':
            app.cameraDist = 12.0f;
            app.cameraAngle = 45.0f;
            app.cameraHeight = 8.0f;
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
            app.cameraDist = fmax(5.0f, app.cameraDist - 0.5f);
            break;
        case GLUT_KEY_DOWN:
            app.cameraDist = fmin(25.0f, app.cameraDist + 0.5f);
            break;
    }
    glutPostRedisplay();
}

void mouse(AppState& app, int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        app.isAnimating = !app.isAnimating;
    }
}

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
            app.lights.lavaIntensity = 0.2f;
            break;
        case 2:
            app.state = VolcanoState::ACTIVE;
            resetParticles(app);
            app.lights.lavaIntensity = 0.5f;
            break;
        case 3:
            app.state = VolcanoState::ERUPTION;
            resetParticles(app);
            app.lights.lavaIntensity = 1.0f;
            break;
        case 99:
            app.cameraDist = 12.0f;
            app.cameraAngle = 45.0f;
            app.cameraHeight = 8.0f;
            app.state = VolcanoState::DORMANT;
            resetParticles(app);
            app.lights.lavaIntensity = 0.3f;
            break;
    }
    glutPostRedisplay();
}

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

    glutPostRedisplay();
}
