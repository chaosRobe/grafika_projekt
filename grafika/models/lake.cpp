#include "volcano.h"
#include "textures.h"
#include <cmath>

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
