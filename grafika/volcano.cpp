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
    app.smokeY = 5.5f;
    app.smokeScale = 1.0f;
    app.smokeOffsetX = 0.0f;
    app.smokeOffsetZ = 0.0f;
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

void drawTerrain() {
    bindRockTexture();
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    int gridSize = 40;
    float size = 30.0f;
    float step = size / (float)gridSize;

    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) {
            float x1 = -size / 2.0f + (float)i * step;
            float x2 = -size / 2.0f + (float)(i + 1) * step;
            float z1 = -size / 2.0f + (float)j * step;
            float z2 = -size / 2.0f + (float)(j + 1) * step;

            float h1 = sin(x1 * 0.5f) * cos(z1 * 0.5f) * 0.3f;
            float h2 = sin(x2 * 0.5f) * cos(z1 * 0.5f) * 0.3f;
            float h3 = sin(x1 * 0.5f) * cos(z2 * 0.5f) * 0.3f;
            float h4 = sin(x2 * 0.5f) * cos(z2 * 0.5f) * 0.3f;

            float u1 = (float)i / (float)gridSize * 6.0f;
            float u2 = (float)(i + 1) / (float)gridSize * 6.0f;
            float v1 = (float)j / (float)gridSize * 6.0f;
            float v2 = (float)(j + 1) / (float)gridSize * 6.0f;

            float nx1 = sin(x1 * 0.3f) * 0.2f;
            float nz1 = cos(z1 * 0.3f) * 0.2f;
            float nx2 = sin(x2 * 0.3f) * 0.2f;
            float nz2 = cos(z2 * 0.3f) * 0.2f;

            glBegin(GL_QUADS);
            glNormal3f(nx1, 1.0f, nz1);
            glTexCoord2f(u1, v1);
            glVertex3f(x1, h1, z1);
            glNormal3f(nx2, 1.0f, nz2);
            glTexCoord2f(u2, v1);
            glVertex3f(x2, h2, z1);
            glNormal3f(nx2, 1.0f, nz2);
            glTexCoord2f(u2, v2);
            glVertex3f(x2, h4, z2);
            glNormal3f(nx1, 1.0f, nz1);
            glTexCoord2f(u1, v2);
            glVertex3f(x1, h3, z2);
            glEnd();
        }
    }
}

void drawSmokeCloud() {
    bindSmokeTexture();
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    float baseY = g_app.smokeY;
    float baseScale = g_app.smokeScale;
    float offsetX = g_app.smokeOffsetX;
    float offsetZ = g_app.smokeOffsetZ;

    if (g_app.state == VolcanoState::DORMANT) {
        baseY = 5.5f;
        baseScale = 0.6f;
        offsetX = 0.0f;
        offsetZ = 0.0f;
    } else if (g_app.state == VolcanoState::ACTIVE) {
        baseY = 5.5f + sin(g_app.stateTimer * 0.8f) * 0.3f;
        baseScale = 0.9f + sin(g_app.stateTimer * 1.2f) * 0.15f;
        offsetX = sin(g_app.stateTimer * 0.5f) * 0.2f;
        offsetZ = cos(g_app.stateTimer * 0.6f) * 0.2f;
    } else if (g_app.state == VolcanoState::ERUPTION) {
        baseY = 5.5f + sin(g_app.stateTimer * 1.5f) * 0.8f;
        baseScale = 1.2f + sin(g_app.stateTimer * 2.0f) * 0.4f;
        offsetX = sin(g_app.stateTimer * 1.0f) * 0.5f;
        offsetZ = cos(g_app.stateTimer * 1.2f) * 0.5f;
    }

    glPushMatrix();
    glTranslatef(offsetX, baseY, offsetZ);
    glScalef(baseScale, baseScale * 0.7f, baseScale);
    glutSolidSphere(0.8f, 16, 12);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(offsetX - 0.6f, baseY + 0.5f, offsetZ + 0.4f);
    glScalef(baseScale * 0.7f, baseScale * 0.5f, baseScale * 0.7f);
    glutSolidSphere(0.5f, 12, 10);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(offsetX + 0.5f, baseY + 0.3f, offsetZ - 0.4f);
    glScalef(baseScale * 0.8f, baseScale * 0.6f, baseScale * 0.8f);
    glutSolidSphere(0.6f, 12, 10);
    glPopMatrix();
}

void drawParticles(AppState& app) {
    for (int i = 0; i < 500; i++) {
        if (!app.particles[i].active) continue;

        float alpha = app.particles[i].life / app.particles[i].maxLife;
        float r, g, b;

        if (g_app.state == VolcanoState::ACTIVE) {
            r = 0.9f;
            g = 0.3f + 0.4f * alpha;
            b = 0.05f;
        } else if (g_app.state == VolcanoState::ERUPTION) {
            r = 1.0f;
            g = 0.15f + 0.6f * alpha;
            b = 0.02f;
        } else {
            r = 1.0f;
            g = 0.2f + 0.5f * alpha;
            b = 0.0f;
        }

        glColor4f(r, g, b, alpha);
        glPointSize(2.5f + alpha * 4.0f);

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

    drawSkybox();
    drawSun();

    updateLights(app);

    float volcanoAmbient[] = { 0.2f, 0.18f, 0.15f, 1.0f };
    float volcanoDiffuse[] = { 0.3f, 0.25f, 0.2f, 1.0f };
    float volcanoSpecular[] = { 0.1f, 0.1f, 0.1f, 1.0f };
    float volcanoEmissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    float volcanoShininess = 5.0f;

    if (app.state == VolcanoState::DORMANT) {
        volcanoAmbient[0] = 0.2f; volcanoAmbient[1] = 0.18f; volcanoAmbient[2] = 0.15f;
        volcanoDiffuse[0] = 0.3f; volcanoDiffuse[1] = 0.25f; volcanoDiffuse[2] = 0.2f;
        volcanoEmissive[0] = 0.0f; volcanoEmissive[1] = 0.0f; volcanoEmissive[2] = 0.0f;
        volcanoShininess = 5.0f;
    } else if (app.state == VolcanoState::ACTIVE) {
        volcanoAmbient[0] = 0.35f; volcanoAmbient[1] = 0.2f; volcanoAmbient[2] = 0.1f;
        volcanoDiffuse[0] = 0.45f; volcanoDiffuse[1] = 0.3f; volcanoDiffuse[2] = 0.15f;
        float pulse = 0.08f + 0.05f * sin(app.stateTimer * 3.0f);
        volcanoEmissive[0] = pulse;
        volcanoEmissive[1] = pulse * 0.4f;
        volcanoEmissive[2] = 0.0f;
        volcanoShininess = 10.0f;
    } else if (app.state == VolcanoState::ERUPTION) {
        volcanoAmbient[0] = 0.5f; volcanoAmbient[1] = 0.25f; volcanoAmbient[2] = 0.05f;
        volcanoDiffuse[0] = 0.55f; volcanoDiffuse[1] = 0.35f; volcanoDiffuse[2] = 0.1f;
        float pulse = 0.2f + 0.15f * sin(app.stateTimer * 6.0f);
        volcanoEmissive[0] = pulse;
        volcanoEmissive[1] = pulse * 0.5f;
        volcanoEmissive[2] = 0.0f;
        volcanoShininess = 15.0f;
    }

    glMaterialfv(GL_FRONT, GL_AMBIENT, volcanoAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, volcanoDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, volcanoSpecular);
    glMaterialfv(GL_FRONT, GL_EMISSION, volcanoEmissive);
    glMaterialf(GL_FRONT, GL_SHININESS, volcanoShininess);

    float terrainAmbient[] = { 0.25f, 0.22f, 0.18f, 1.0f };
    float terrainDiffuse[] = { 0.35f, 0.3f, 0.25f, 1.0f };
    float terrainSpecular[] = { 0.05f, 0.05f, 0.05f, 1.0f };
    float terrainEmissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };

    if (app.state == VolcanoState::ERUPTION) {
        float glow = 0.05f + 0.03f * sin(app.stateTimer * 4.0f);
        terrainEmissive[0] = glow;
        terrainEmissive[1] = glow * 0.3f;
        terrainAmbient[0] += 0.1f;
        terrainAmbient[1] += 0.03f;
    }

    glMaterialfv(GL_FRONT, GL_AMBIENT, terrainAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, terrainDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, terrainSpecular);
    glMaterialfv(GL_FRONT, GL_EMISSION, terrainEmissive);
    glMaterialf(GL_FRONT, GL_SHININESS, 8.0f);
    drawTerrain();

    drawVolcano();

    float lakeAmbient[] = { 0.4f, 0.15f, 0.02f, 1.0f };
    float lakeDiffuse[] = { 0.6f, 0.2f, 0.05f, 1.0f };
    float lakeSpecular[] = { 0.8f, 0.5f, 0.2f, 1.0f };
    float lakeEmissive[] = { 0.2f, 0.08f, 0.0f, 1.0f };
    float lakeShininess = 80.0f;

    if (app.state == VolcanoState::DORMANT) {
        lakeAmbient[0] = 0.15f; lakeAmbient[1] = 0.05f; lakeAmbient[2] = 0.01f;
        lakeDiffuse[0] = 0.2f; lakeDiffuse[1] = 0.08f; lakeDiffuse[2] = 0.02f;
        lakeEmissive[0] = 0.05f; lakeEmissive[1] = 0.02f; lakeEmissive[2] = 0.0f;
        lakeShininess = 40.0f;
    } else if (app.state == VolcanoState::ACTIVE) {
        float pulse = 0.15f + 0.1f * sin(app.stateTimer * 2.5f);
        lakeEmissive[0] = pulse;
        lakeEmissive[1] = pulse * 0.5f;
        lakeShininess = 70.0f;
    } else if (app.state == VolcanoState::ERUPTION) {
        float pulse = 0.35f + 0.2f * sin(app.stateTimer * 5.0f);
        lakeEmissive[0] = pulse;
        lakeEmissive[1] = pulse * 0.7f;
        lakeEmissive[2] = pulse * 0.1f;
        lakeShininess = 100.0f;
    }

    glMaterialfv(GL_FRONT, GL_AMBIENT, lakeAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, lakeDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, lakeSpecular);
    glMaterialfv(GL_FRONT, GL_EMISSION, lakeEmissive);
    glMaterialf(GL_FRONT, GL_SHININESS, lakeShininess);
    drawLavaLake();

    if (app.state != VolcanoState::DORMANT) {
        float smokeAlpha = 0.35f;
        float smokeR = 0.35f, smokeG = 0.3f, smokeB = 0.28f;
        float smokeEmissiveR = 0.0f, smokeEmissiveG = 0.0f;

        if (app.state == VolcanoState::ACTIVE) {
            smokeAlpha = 0.45f;
            smokeR = 0.4f; smokeG = 0.32f; smokeB = 0.25f;
            smokeEmissiveR = 0.05f + 0.03f * sin(app.stateTimer * 2.0f);
            smokeEmissiveG = smokeEmissiveR * 0.3f;
        } else if (app.state == VolcanoState::ERUPTION) {
            smokeAlpha = 0.65f;
            smokeR = 0.5f; smokeG = 0.3f; smokeB = 0.15f;
            smokeEmissiveR = 0.12f + 0.08f * sin(app.stateTimer * 4.0f);
            smokeEmissiveG = smokeEmissiveR * 0.4f;
        }

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        float smokeAmbient[] = { smokeR, smokeG, smokeB, smokeAlpha };
        float smokeEmissive[] = { smokeEmissiveR, smokeEmissiveG, 0.0f, 1.0f };
        glMaterialfv(GL_FRONT, GL_AMBIENT, smokeAmbient);
        glMaterialfv(GL_FRONT, GL_EMISSION, smokeEmissive);

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

    if (g_app.state != VolcanoState::DORMANT) {
        g_app.smokeOffsetX += sin(g_app.stateTimer * 0.7f) * 0.003f;
        g_app.smokeOffsetZ += cos(g_app.stateTimer * 0.9f) * 0.003f;
        if (g_app.state == VolcanoState::ERUPTION) {
            g_app.smokeScale = 1.0f + sin(g_app.stateTimer * 3.0f) * 0.15f;
            g_app.smokeY = 5.5f + sin(g_app.stateTimer * 2.0f) * 0.3f;
        } else {
            g_app.smokeScale = 1.0f + sin(g_app.stateTimer * 1.5f) * 0.08f;
            g_app.smokeY = 5.5f + sin(g_app.stateTimer * 1.0f) * 0.15f;
        }
    } else {
        g_app.smokeY = 5.5f;
        g_app.smokeScale = 0.6f;
        g_app.smokeOffsetX *= 0.95f;
        g_app.smokeOffsetZ *= 0.95f;
    }

    glutPostRedisplay();
}

void initSkybox(AppState& app) {
    const int texSize = 256;
    unsigned char* texData = new unsigned char[texSize * texSize * 4];

    for (int y = 0; y < texSize; y++) {
        for (int x = 0; x < texSize; x++) {
            int idx = (y * texSize + x) * 4;
            float ny = (float)y / (float)texSize;

            float r, g, b;
            if (ny < 0.5f) {
                float t = ny * 2.0f;
                r = 10.0f + t * 30.0f;
                g = 15.0f + t * 50.0f;
                b = 40.0f + t * 120.0f;
            } else {
                float t = (ny - 0.5f) * 2.0f;
                r = 40.0f + t * 80.0f;
                g = 65.0f + t * 100.0f;
                b = 160.0f + t * 80.0f;
            }

            texData[idx] = (unsigned char)fmin(255.0f, fmax(0.0f, r));
            texData[idx + 1] = (unsigned char)fmin(255.0f, fmax(0.0f, g));
            texData[idx + 2] = (unsigned char)fmin(255.0f, fmax(0.0f, b));
            texData[idx + 3] = 255;
        }
    }

    glGenTextures(1, &app.skyboxTextureID);
    glBindTexture(GL_TEXTURE_2D, app.skyboxTextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texSize, texSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    delete[] texData;
}

void initSun(AppState& app) {
    const int texSize = 64;
    unsigned char* texData = new unsigned char[texSize * texSize * 4];

    for (int y = 0; y < texSize; y++) {
        for (int x = 0; x < texSize; x++) {
            int idx = (y * texSize + x) * 4;
            float cx = (float)x / (float)texSize * 2.0f - 1.0f;
            float cy = (float)y / (float)texSize * 2.0f - 1.0f;
            float dist = sqrt(cx * cx + cy * cy);

            float alpha = 255.0f * (1.0f - fmin(1.0f, dist * 1.5f));
            float glow = exp(-dist * 3.0f) * 255.0f;

            texData[idx] = (unsigned char)fmin(255.0f, 255.0f);
            texData[idx + 1] = (unsigned char)fmin(255.0f, 200.0f + glow * 0.3f);
            texData[idx + 2] = (unsigned char)fmin(255.0f, 100.0f + glow * 0.5f);
            texData[idx + 3] = (unsigned char)fmax(0.0f, fmin(255.0f, alpha));
        }
    }

    glGenTextures(1, &app.sunTextureID);
    glBindTexture(GL_TEXTURE_2D, app.sunTextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texSize, texSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    delete[] texData;
}

void drawSkybox() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, g_app.skyboxTextureID);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    float size = 80.0f;
    float topY = size * 0.6f;
    float botY = -size * 0.3f;

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-size, botY, -size);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(size, botY, -size);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(size, topY, -size);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-size, topY, -size);

    glTexCoord2f(0.0f, 0.0f); glVertex3f(size, botY, size);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-size, botY, size);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-size, topY, size);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(size, topY, size);

    glTexCoord2f(0.0f, 0.0f); glVertex3f(-size, botY, size);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-size, botY, -size);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-size, topY, -size);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-size, topY, size);

    glTexCoord2f(0.0f, 0.0f); glVertex3f(size, botY, -size);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(size, botY, size);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(size, topY, size);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(size, topY, -size);
    glEnd();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}

void drawSun() {
    float sunX = 25.0f * cos(g_app.moonAngle * 3.14159265f / 180.0f);
    float sunY = 15.0f + 10.0f * sin(g_app.moonAngle * 3.14159265f / 180.0f);
    float sunZ = 25.0f * sin(g_app.moonAngle * 3.14159265f / 180.0f);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, g_app.sunTextureID);
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glPushMatrix();
    glTranslatef(sunX, sunY, sunZ);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.5f, -1.5f, 0.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(1.5f, -1.5f, 0.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(1.5f, 1.5f, 0.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.5f, 1.5f, 0.0f);
    glEnd();
    glPopMatrix();

    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
}
