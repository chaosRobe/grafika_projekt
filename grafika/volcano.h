#pragma once
#include <GL/glut.h>

enum class VolcanoState {
    DORMANT,
    ACTIVE,
    ERUPTION
};

struct LightParams {
    float lavaIntensity;
    float lavaR, lavaG, lavaB;
    float lavaPos[4];
    float moonAngle;
    float moonR, moonG, moonB;
    float moonPos[4];
};

struct Particle {
    float x, y, z;
    float vx, vy, vz;
    float life;
    float maxLife;
    bool active;
};

struct AppState {
    VolcanoState state;
    float stateTimer;
    LightParams lights;
    Particle particles[500];
    int numParticles;
    float cameraDist;
    float cameraAngle;
    float cameraHeight;
    bool isAnimating;
    float smokeY;
    float smokeScale;
    float smokeOffsetX;
    float smokeOffsetZ;
};

void initAppState(AppState& app);
void resetParticles(AppState& app);
void initOpenGL(AppState& app);
void initLights(AppState& app);
void updateLights(AppState& app);
void drawVolcano();
void drawLavaLake();
void drawTerrain();
void drawSmokeCloud();
void drawParticles(AppState& app);
void updateParticles(AppState& app, float deltaTime);
void emitParticles(AppState& app);
void displayScene(AppState& app);
void keyboard(AppState& app, unsigned char key, int x, int y);
void specialKeys(AppState& app, int key, int x, int y);
void mouse(AppState& app, int button, int state, int x, int y);
void createMenu(AppState& app);
void menuCallback(AppState& app, int value);
void reshape(int w, int h);
void timerFunc(int value);
