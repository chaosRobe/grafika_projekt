#include "volcano.h"
#include "textures.h"
#include <cmath>

void drawVolcano() {
    bindRockTexture();
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    float segments = 32.0f;
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
