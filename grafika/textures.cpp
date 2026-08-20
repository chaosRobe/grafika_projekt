#include "textures.h"
#include "volcano.h"
#include <cmath>
#include <cstdlib>

static GLuint rockTextureID = 0;
static GLuint lavaTextureID = 0;
static GLuint smokeTextureID = 0;

static float hash(float x, float y) {
    float n = sin(x * 127.1f + y * 311.7f) * 43758.5453f;
    return n - floor(n);
}

static float smoothNoise(float x, float y) {
    int ix = (int)floor(x);
    int iy = (int)floor(y);
    float fx = x - ix;
    float fy = y - iy;
    fx = fx * fx * (3.0f - 2.0f * fx);
    fy = fy * fy * (3.0f - 2.0f * fy);
    float a = hash((float)ix, (float)iy);
    float b = hash((float)ix + 1.0f, (float)iy);
    float c = hash((float)ix, (float)iy + 1.0f);
    float d = hash((float)ix + 1.0f, (float)iy + 1.0f);
    return a + (b - a) * fx + (c - a) * fy + (a - b - c + d) * fx * fy;
}

static float fbm(float x, float y, int octaves) {
    float value = 0.0f;
    float amplitude = 0.5f;
    float frequency = 1.0f;
    for (int i = 0; i < octaves; i++) {
        value += amplitude * smoothNoise(x * frequency, y * frequency);
        amplitude *= 0.5f;
        frequency *= 2.0f;
    }
    return value;
}

void generateRockTexture(unsigned char* texData, int width, int height) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int idx = (y * width + x) * 4;
            float nx = (float)x / (float)width * 20.0f;
            float ny = (float)y / (float)height * 20.0f;

            float n1 = fbm(nx, ny, 7);
            float n2 = fbm(nx * 5.0f + 15.0f, ny * 5.0f + 15.0f, 5);
            float n3 = fbm(nx * 12.0f + 40.0f, ny * 12.0f + 40.0f, 4);

            float detail = n1 * 0.4f + n2 * 0.35f + n3 * 0.25f;

            float threshold = 0.4f;
            float rock = (detail < threshold) ? (detail / threshold) * 0.25f : 0.25f + (detail - threshold) / (1.0f - threshold) * 0.75f;

            float baseGray = rock * 200.0f - 10.0f;

            float vesicle = fbm(nx * 2.5f + 60.0f, ny * 2.5f + 60.0f, 3);
            if (vesicle > 0.65f && detail < 0.45f) {
                baseGray -= 35.0f;
            }

            float crack = fbm(nx * 10.0f + 70.0f, ny * 10.0f + 70.0f, 2);
            if (crack > 0.75f) {
                baseGray -= 50.0f;
            }

            float flow = fbm(nx * 1.5f + 80.0f, ny * 3.0f + 80.0f, 2);
            if (flow > 0.6f) {
                baseGray += 15.0f * (flow - 0.6f) * 2.5f;
            }

            texData[idx] = (unsigned char)fmin(255.0f, fmax(0.0f, baseGray));
            texData[idx + 1] = (unsigned char)fmin(255.0f, fmax(0.0f, baseGray));
            texData[idx + 2] = (unsigned char)fmin(255.0f, fmax(0.0f, baseGray));
            texData[idx + 3] = 255;
        }
    }
}

void generateLavaTexture(unsigned char* texData, int width, int height) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int idx = (y * width + x) * 4;
            float nx = (float)x / (float)width * 6.0f;
            float ny = (float)y / (float)height * 6.0f;
            float n = fbm(nx, ny, 4);

            float lava = fbm(nx * 1.5f, ny * 1.5f, 3);
            float r = 180.0f + lava * 75.0f;
            float g = 40.0f + n * 80.0f + lava * 40.0f;
            float b = 5.0f + n * 15.0f;

            texData[idx] = (unsigned char)fmin(255.0f, fmax(0.0f, r));
            texData[idx + 1] = (unsigned char)fmin(255.0f, fmax(0.0f, g));
            texData[idx + 2] = (unsigned char)fmin(255.0f, fmax(0.0f, b));
            texData[idx + 3] = 255;
        }
    }
}

void generateSmokeTexture(unsigned char* texData, int width, int height) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int idx = (y * width + x) * 4;
            float cx = (float)x / (float)width * 2.0f - 1.0f;
            float cy = (float)y / (float)height * 2.0f - 1.0f;
            float dist = sqrt(cx * cx + cy * cy);

            float n = fbm((float)x / (float)width * 4.0f, (float)y / (float)height * 4.0f, 3);
            float alpha = 255.0f * (1.0f - fmin(1.0f, dist * 1.2f)) * (0.5f + 0.5f * n);
            alpha = fmax(0.0f, fmin(255.0f, alpha));

            float gray = 100.0f + n * 60.0f;
            texData[idx] = (unsigned char)gray;
            texData[idx + 1] = (unsigned char)gray;
            texData[idx + 2] = (unsigned char)(gray + 10.0f);
            texData[idx + 3] = (unsigned char)alpha;
        }
    }
}

void initTextures(AppState& app) {
    const int texSize = 256;
    unsigned char* texData = new unsigned char[texSize * texSize * 4];

    generateRockTexture(texData, texSize, texSize);
    glGenTextures(1, &rockTextureID);
    glBindTexture(GL_TEXTURE_2D, rockTextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texSize, texSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    generateLavaTexture(texData, texSize, texSize);
    glGenTextures(1, &lavaTextureID);
    glBindTexture(GL_TEXTURE_2D, lavaTextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texSize, texSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    generateSmokeTexture(texData, texSize, texSize);
    glGenTextures(1, &smokeTextureID);
    glBindTexture(GL_TEXTURE_2D, smokeTextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texSize, texSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    delete[] texData;
    glEnable(GL_TEXTURE_2D);
}

void bindRockTexture() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, rockTextureID);
}

void bindLavaTexture() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, lavaTextureID);
}

void bindSmokeTexture() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, smokeTextureID);
}

void deleteTextures() {
    if (rockTextureID) glDeleteTextures(1, &rockTextureID);
    if (lavaTextureID) glDeleteTextures(1, &lavaTextureID);
    if (smokeTextureID) glDeleteTextures(1, &smokeTextureID);
}
