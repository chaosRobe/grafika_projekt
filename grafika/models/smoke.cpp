#include "volcano.h"
#include "textures.h"
#include <cmath>

extern AppState g_app;

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
