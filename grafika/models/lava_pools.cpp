#include "volcano.h"
#include <cmath>

extern AppState g_app;

void drawLavaPools() {
    float lavaAmbient[] = { 0.5f, 0.1f, 0.02f, 1.0f };
    float lavaDiffuse[] = { 0.7f, 0.15f, 0.02f, 1.0f };
    float lavaSpecular[] = { 0.8f, 0.3f, 0.05f, 1.0f };
    float lavaEmissive[] = { 0.9f, 0.15f, 0.02f, 1.0f };

    glMaterialfv(GL_FRONT, GL_AMBIENT, lavaAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, lavaDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, lavaSpecular);
    glMaterialfv(GL_FRONT, GL_EMISSION, lavaEmissive);
    glMaterialf(GL_FRONT, GL_SHININESS, 80.0f);

    float poolPositions[][2] = {
        { 35.0f, 25.0f },
        { -30.0f, 35.0f },
        { 40.0f, -15.0f },
        { -35.0f, -25.0f },
        { 25.0f, -35.0f },
        { -40.0f, 10.0f },
        { 45.0f, 30.0f },
        { -20.0f, -40.0f },
    };

    float poolSizes[] = { 5.0f, 4.5f, 6.0f, 4.0f, 5.5f, 4.2f, 5.8f, 4.8f };

    for (int i = 0; i < 8; i++) {
        float px = poolPositions[i][0];
        float pz = poolPositions[i][1];

        float terrainH = sinf(px * 0.1f) * cosf(pz * 0.1f) * 0.5f;

        float radius = poolSizes[i];
        int segments = 16;

        glBegin(GL_TRIANGLE_FAN);
        glVertex3f(px, terrainH + 0.1f, pz);
        for (int j = 0; j <= segments; j++) {
            float angle = (float)j / segments * 6.28318f;
            float x = px + cosf(angle) * radius;
            float z = pz + sinf(angle) * radius;
            float h = sinf(x * 0.1f) * cosf(z * 0.1f) * 0.5f;
            glVertex3f(x, h + 0.15f, z);
        }
        glEnd();
    }
}
