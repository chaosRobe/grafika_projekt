#include "volcano.h"
#include "textures.h"
#include <cmath>

extern AppState g_app;

struct SmokeParticle {
    float x, y, z;
    float vx, vy, vz;
    float size;
    float life;
    float maxLife;
};

static SmokeParticle smokeParticles[100];
static int smokeParticleCount = 0;

void initSmokeParticles() {
    smokeParticleCount = 0;
}

void updateSmokeParticles(float dt) {
    for (int i = smokeParticleCount - 1; i >= 0; i--) {
        SmokeParticle& p = smokeParticles[i];
        p.life -= dt;
        if (p.life <= 0) {
            smokeParticles[i] = smokeParticles[smokeParticleCount - 1];
            smokeParticleCount--;
            continue;
        }
        p.x += p.vx * dt;
        p.y += p.vy * dt;
        p.z += p.vz * dt;
        p.vy *= 0.995f;
        p.vx *= 0.99f;
        p.vz *= 0.99f;
        p.size += dt * 0.8f;
    }

    if (g_app.state != VolcanoState::DORMANT && smokeParticleCount < 100) {
        float emissionRate = (g_app.state == VolcanoState::ERUPTION) ? 5.0f : 2.5f;
        for (int i = 0; i < (int)emissionRate && smokeParticleCount < 100; i++) {
            SmokeParticle& p = smokeParticles[smokeParticleCount++];
            p.x = (float)(rand() % 100) / 100.0f * 0.6f - 0.3f;
            p.y = 18.0f;
            p.z = (float)(rand() % 100) / 100.0f * 0.6f - 0.3f;
            p.vx = (float)(rand() % 1000) / 1000.0f * 0.2f - 0.1f;
            p.vy = 4.0f + (float)(rand() % 100) / 100.0f * 3.0f;
            p.vz = (float)(rand() % 1000) / 1000.0f * 0.2f - 0.1f;
            p.size = 0.25f + (float)(rand() % 50) / 100.0f * 0.25f;
            p.life = 6.0f + (float)(rand() % 100) / 100.0f * 3.0f;
            p.maxLife = p.life;
        }
    }
}

void drawSmokeCloud() {
    glPushMatrix();
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);

    updateSmokeParticles(0.016f);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (int i = 0; i < smokeParticleCount; i++) {
        SmokeParticle& p = smokeParticles[i];
        float alpha = p.life / p.maxLife;
        float heightRatio = fmin(1.0f, p.y / 25.0f);
        float expand = 0.35f + (1.0f - alpha) * 0.2f;

        float xzScale = expand * (1.0f + heightRatio * 10.0f);
        float yScale = expand * (1.0f + heightRatio * 0.15f);

        float gray = 0.25f + alpha * 0.2f;
        glColor4f(gray, gray, gray, 0.3f);
        glPushMatrix();
        glTranslatef(p.x, p.y, p.z);
        glScalef(xzScale * 3.5f, yScale * 3.5f, xzScale * 3.5f);
        glutSolidSphere(0.5f, 8, 6);
        glPopMatrix();
    }

    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glPopMatrix();
}
