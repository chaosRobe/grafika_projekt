#include "volcano.h"
#include "textures.h"
#include <cmath>

void drawVolcano() {
    bindRockTexture();
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glShadeModel(GL_FLAT);

    float baseRadius = 8.0f;
    float midRadius = 5.5f;
    float topRadius = 2.0f;
    float baseHeight = 6.0f;
    float midHeight = 3.0f;
    int segs = 32;

    float avgNx = 0.7f;
    float avgNy = 0.7f;
    float avgLen = sqrtf(avgNx * avgNx + avgNy * avgNy);
    avgNx /= avgLen;
    avgNy /= avgLen;

    for (int i = 0; i < segs; i++) {
        float a1 = i * 2.0f * 3.14159265f / segs;
        float a2 = (i + 1) * 2.0f * 3.14159265f / segs;
        float c1 = cos(a1), s1 = sin(a1);
        float c2 = cos(a2), s2 = sin(a2);

        float r1 = baseRadius, r2 = midRadius;
        float y1 = 0.0f, y2 = baseHeight;
        float u1 = (float)i / segs, u2 = (float)(i + 1) / segs;
        float v1 = 0.0f, v2 = 0.7f;

        glBegin(GL_TRIANGLE_STRIP);
        glNormal3f(avgNx * c1, avgNy, avgNx * s1);
        glTexCoord2f(u1, v1);
        glVertex3f(r1 * c1, y1, r1 * s1);
        glNormal3f(avgNx * c1, avgNy, avgNx * s1);
        glTexCoord2f(u1, v2);
        glVertex3f(r2 * c1, y2, r2 * s1);
        glNormal3f(avgNx * c2, avgNy, avgNx * s2);
        glTexCoord2f(u2, v1);
        glVertex3f(r1 * c2, y1, r1 * s2);
        glNormal3f(avgNx * c2, avgNy, avgNx * s2);
        glTexCoord2f(u2, v2);
        glVertex3f(r2 * c2, y2, r2 * s2);
        glEnd();
    }

    for (int i = 0; i < segs; i++) {
        float a1 = i * 2.0f * 3.14159265f / segs;
        float a2 = (i + 1) * 2.0f * 3.14159265f / segs;
        float c1 = cos(a1), s1 = sin(a1);
        float c2 = cos(a2), s2 = sin(a2);

        float r1 = midRadius, r2 = topRadius;
        float y1 = baseHeight, y2 = baseHeight + midHeight;
        float u1 = (float)i / segs, u2 = (float)(i + 1) / segs;
        float v1 = 0.7f, v2 = 1.0f;

        glBegin(GL_TRIANGLE_STRIP);
        glNormal3f(avgNx * c1, avgNy, avgNx * s1);
        glTexCoord2f(u1, v1);
        glVertex3f(r1 * c1, y1, r1 * s1);
        glNormal3f(avgNx * c1, avgNy, avgNx * s1);
        glTexCoord2f(u1, v2);
        glVertex3f(r2 * c1, y2, r2 * s1);
        glNormal3f(avgNx * c2, avgNy, avgNx * s2);
        glTexCoord2f(u2, v1);
        glVertex3f(r1 * c2, y1, r1 * s2);
        glNormal3f(avgNx * c2, avgNy, avgNx * s2);
        glTexCoord2f(u2, v2);
        glVertex3f(r2 * c2, y2, r2 * s2);
        glEnd();
    }

    for (int i = 0; i < segs; i++) {
        float a1 = i * 2.0f * 3.14159265f / segs;
        float a2 = (i + 1) * 2.0f * 3.14159265f / segs;
        float c1 = cos(a1), s1 = sin(a1);
        float c2 = cos(a2), s2 = sin(a2);

        float innerR = topRadius * 0.35f;
        float outerR = topRadius;
        float y = baseHeight + midHeight;

        float u1i = 0.5f + 0.5f * (innerR / outerR) * cos(a1);
        float v1i = 0.5f + 0.5f * (innerR / outerR) * sin(a1);
        float u2i = 0.5f + 0.5f * (innerR / outerR) * cos(a2);
        float v2i = 0.5f + 0.5f * (innerR / outerR) * sin(a2);
        float u1o = 0.5f + 0.5f * cos(a1);
        float v1o = 0.5f + 0.5f * sin(a1);
        float u2o = 0.5f + 0.5f * cos(a2);
        float v2o = 0.5f + 0.5f * sin(a2);

        glBegin(GL_TRIANGLE_STRIP);
        glNormal3f(0.0f, 1.0f, 0.0f);
        glTexCoord2f(u1o, v1o);
        glVertex3f(outerR * c1, y, outerR * s1);
        glNormal3f(0.0f, 1.0f, 0.0f);
        glTexCoord2f(u1i, v1i);
        glVertex3f(innerR * c1, y, innerR * s1);
        glNormal3f(0.0f, 1.0f, 0.0f);
        glTexCoord2f(u2o, v2o);
        glVertex3f(outerR * c2, y, outerR * s2);
        glNormal3f(0.0f, 1.0f, 0.0f);
        glTexCoord2f(u2i, v2i);
        glVertex3f(innerR * c2, y, innerR * s2);
        glEnd();
    }

    glShadeModel(GL_SMOOTH);
}
