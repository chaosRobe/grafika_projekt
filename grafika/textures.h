#pragma once
#include <GL/glut.h>

struct AppState;

void initTextures(AppState& app);
void generateRockTexture(unsigned char* texData, int width, int height);
void generateLavaTexture(unsigned char* texData, int width, int height);
void generateSmokeTexture(unsigned char* texData, int width, int height);
void bindRockTexture();
void bindLavaTexture();
void bindSmokeTexture();
void deleteTextures();
