#include "volcano.h"
#include "textures.h"
#include <cmath>

void drawTerrain() {
    bindRockTexture();
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    int gridSize = 100;
    float size = 120.0f;
    float step = size / (float)gridSize;

    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) {
            float x1 = -size / 2.0f + (float)i * step;
            float x2 = -size / 2.0f + (float)(i + 1) * step;
            float z1 = -size / 2.0f + (float)j * step;
            float z2 = -size / 2.0f + (float)(j + 1) * step;

            float h1 = sin(x1 * 0.1f) * cos(z1 * 0.1f) * 0.5f;
            float h2 = sin(x2 * 0.1f) * cos(z1 * 0.1f) * 0.5f;
            float h3 = sin(x1 * 0.1f) * cos(z2 * 0.1f) * 0.5f;
            float h4 = sin(x2 * 0.1f) * cos(z2 * 0.1f) * 0.5f;

            float u1 = (float)i / (float)gridSize * 20.0f;
            float u2 = (float)(i + 1) / (float)gridSize * 20.0f;
            float v1 = (float)j / (float)gridSize * 20.0f;
            float v2 = (float)(j + 1) / (float)gridSize * 20.0f;

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
