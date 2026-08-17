#include "volcano.h"
#include <cmath>

extern AppState g_app;

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
