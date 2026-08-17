#include "volcano.h"
#include <cmath>

extern AppState g_app;

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
            float dist = sqrtf(cx * cx + cy * cy);

            float alpha = 255.0f * (1.0f - fmin(1.0f, dist * 1.5f));
            float glow = expf(-dist * 3.0f) * 255.0f;

            texData[idx] = (unsigned char)fmin(255.0f, 255.0f);
            texData[idx + 1] = (unsigned char)fmin(255.0f, 230.0f + glow * 0.4f);
            texData[idx + 2] = (unsigned char)fmin(255.0f, 50.0f + glow * 0.2f);
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
    glPushMatrix();

    float angleRad = g_app.cameraAngle * 3.14159265f / 180.0f;
    float camX = g_app.cameraDist * sin(angleRad);
    float camY = g_app.cameraHeight;
    float camZ = g_app.cameraDist * cos(angleRad);
    glTranslatef(camX, camY, camZ);

    glDisable(GL_LIGHTING);
    glDepthMask(GL_FALSE);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_COLOR_MATERIAL);

    float size = 200.0f;
    float topY = size * 0.6f;
    float botY = -size * 0.3f;

    glBegin(GL_QUADS);
    glColor3f(0.04f, 0.06f, 0.15f);
    glVertex3f(-size, botY, -size);
    glColor3f(0.04f, 0.06f, 0.15f);
    glVertex3f(size, botY, -size);
    glColor3f(0.16f, 0.26f, 0.56f);
    glVertex3f(size, topY, -size);
    glColor3f(0.16f, 0.26f, 0.56f);
    glVertex3f(-size, topY, -size);

    glColor3f(0.04f, 0.06f, 0.15f);
    glVertex3f(size, botY, size);
    glColor3f(0.04f, 0.06f, 0.15f);
    glVertex3f(-size, botY, size);
    glColor3f(0.16f, 0.26f, 0.56f);
    glVertex3f(-size, topY, size);
    glColor3f(0.16f, 0.26f, 0.56f);
    glVertex3f(size, topY, size);

    glColor3f(0.04f, 0.06f, 0.15f);
    glVertex3f(-size, botY, size);
    glColor3f(0.04f, 0.06f, 0.15f);
    glVertex3f(-size, botY, -size);
    glColor3f(0.16f, 0.26f, 0.56f);
    glVertex3f(-size, topY, -size);
    glColor3f(0.16f, 0.26f, 0.56f);
    glVertex3f(-size, topY, size);

    glColor3f(0.04f, 0.06f, 0.15f);
    glVertex3f(size, botY, -size);
    glColor3f(0.04f, 0.06f, 0.15f);
    glVertex3f(size, botY, size);
    glColor3f(0.16f, 0.26f, 0.56f);
    glVertex3f(size, topY, size);
    glColor3f(0.16f, 0.26f, 0.56f);
    glVertex3f(size, topY, -size);

    glColor3f(0.20f, 0.30f, 0.65f);
    glVertex3f(-size, topY, -size);
    glColor3f(0.20f, 0.30f, 0.65f);
    glVertex3f(size, topY, -size);
    glColor3f(0.20f, 0.30f, 0.65f);
    glVertex3f(size, topY, size);
    glColor3f(0.20f, 0.30f, 0.65f);
    glVertex3f(-size, topY, size);
    glEnd();

    glDisable(GL_COLOR_MATERIAL);
    glDepthMask(GL_TRUE);
    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glPopMatrix();
}

void drawSun() {
    float sunX = 50.0f * cosf(g_app.moonAngle * 3.14159265f / 180.0f);
    float sunY = 35.0f + 10.0f * sinf(g_app.moonAngle * 3.14159265f / 180.0f);
    float sunZ = 50.0f * sinf(g_app.moonAngle * 3.14159265f / 180.0f);

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glDisable(GL_DEPTH_TEST);

    glPushMatrix();
    glTranslatef(sunX, sunY, sunZ);

    glColor4f(1.0f, 0.95f, 0.6f, 0.9f);
    glutSolidSphere(5.0f, 16, 12);

    glColor4f(1.0f, 0.8f, 0.3f, 0.3f);
    glutSolidSphere(7.0f, 12, 8);

    glColor4f(1.0f, 0.7f, 0.2f, 0.1f);
    glutSolidSphere(9.0f, 8, 6);

    glPopMatrix();

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
}
