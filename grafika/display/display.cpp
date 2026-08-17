#include "volcano.h"
#include "textures.h"
#include <cmath>

void displayScene(AppState& app) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    float angleRad = app.cameraAngle * 3.14159265f / 180.0f;
    gluLookAt(
        app.cameraDist * sin(angleRad),
        app.cameraHeight,
        app.cameraDist * cos(angleRad),
        0.0f, 1.5f, 0.0f,
        0.0f, 1.0f, 0.0f
    );

    drawSkybox();
    drawSun();

    updateLights(app);

    float volcanoAmbient[] = { 0.2f, 0.18f, 0.15f, 1.0f };
    float volcanoDiffuse[] = { 0.3f, 0.25f, 0.2f, 1.0f };
    float volcanoSpecular[] = { 0.1f, 0.1f, 0.1f, 1.0f };
    float volcanoEmissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    float volcanoShininess = 5.0f;

    if (app.state == VolcanoState::DORMANT) {
        volcanoAmbient[0] = 0.2f; volcanoAmbient[1] = 0.18f; volcanoAmbient[2] = 0.15f;
        volcanoDiffuse[0] = 0.3f; volcanoDiffuse[1] = 0.25f; volcanoDiffuse[2] = 0.2f;
        volcanoEmissive[0] = 0.0f; volcanoEmissive[1] = 0.0f; volcanoEmissive[2] = 0.0f;
        volcanoShininess = 5.0f;
    } else if (app.state == VolcanoState::ACTIVE) {
        volcanoAmbient[0] = 0.35f; volcanoAmbient[1] = 0.2f; volcanoAmbient[2] = 0.1f;
        volcanoDiffuse[0] = 0.45f; volcanoDiffuse[1] = 0.3f; volcanoDiffuse[2] = 0.15f;
        float pulse = 0.08f + 0.05f * sin(app.stateTimer * 3.0f);
        volcanoEmissive[0] = pulse;
        volcanoEmissive[1] = pulse * 0.4f;
        volcanoEmissive[2] = 0.0f;
        volcanoShininess = 10.0f;
    } else if (app.state == VolcanoState::ERUPTION) {
        volcanoAmbient[0] = 0.5f; volcanoAmbient[1] = 0.25f; volcanoAmbient[2] = 0.05f;
        volcanoDiffuse[0] = 0.55f; volcanoDiffuse[1] = 0.35f; volcanoDiffuse[2] = 0.1f;
        float pulse = 0.2f + 0.15f * sin(app.stateTimer * 6.0f);
        volcanoEmissive[0] = pulse;
        volcanoEmissive[1] = pulse * 0.5f;
        volcanoEmissive[2] = 0.0f;
        volcanoShininess = 15.0f;
    }

    glMaterialfv(GL_FRONT, GL_AMBIENT, volcanoAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, volcanoDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, volcanoSpecular);
    glMaterialfv(GL_FRONT, GL_EMISSION, volcanoEmissive);
    glMaterialf(GL_FRONT, GL_SHININESS, volcanoShininess);

    float terrainAmbient[] = { 0.25f, 0.22f, 0.18f, 1.0f };
    float terrainDiffuse[] = { 0.35f, 0.3f, 0.25f, 1.0f };
    float terrainSpecular[] = { 0.05f, 0.05f, 0.05f, 1.0f };
    float terrainEmissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };

    if (app.state == VolcanoState::ERUPTION) {
        float glow = 0.05f + 0.03f * sin(app.stateTimer * 4.0f);
        terrainEmissive[0] = glow;
        terrainEmissive[1] = glow * 0.3f;
        terrainAmbient[0] += 0.1f;
        terrainAmbient[1] += 0.03f;
    }

    glMaterialfv(GL_FRONT, GL_AMBIENT, terrainAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, terrainDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, terrainSpecular);
    glMaterialfv(GL_FRONT, GL_EMISSION, terrainEmissive);
    glMaterialf(GL_FRONT, GL_SHININESS, 8.0f);
    drawTerrain();

    drawVolcano();

    float lakeAmbient[] = { 0.4f, 0.15f, 0.02f, 1.0f };
    float lakeDiffuse[] = { 0.6f, 0.2f, 0.05f, 1.0f };
    float lakeSpecular[] = { 0.8f, 0.5f, 0.2f, 1.0f };
    float lakeEmissive[] = { 0.2f, 0.08f, 0.0f, 1.0f };
    float lakeShininess = 80.0f;

    if (app.state == VolcanoState::DORMANT) {
        lakeAmbient[0] = 0.15f; lakeAmbient[1] = 0.05f; lakeAmbient[2] = 0.01f;
        lakeDiffuse[0] = 0.2f; lakeDiffuse[1] = 0.08f; lakeDiffuse[2] = 0.02f;
        lakeEmissive[0] = 0.05f; lakeEmissive[1] = 0.02f; lakeEmissive[2] = 0.0f;
        lakeShininess = 40.0f;
    } else if (app.state == VolcanoState::ACTIVE) {
        float pulse = 0.15f + 0.1f * sin(app.stateTimer * 2.5f);
        lakeEmissive[0] = pulse;
        lakeEmissive[1] = pulse * 0.5f;
        lakeShininess = 70.0f;
    } else if (app.state == VolcanoState::ERUPTION) {
        float pulse = 0.35f + 0.2f * sin(app.stateTimer * 5.0f);
        lakeEmissive[0] = pulse;
        lakeEmissive[1] = pulse * 0.7f;
        lakeEmissive[2] = pulse * 0.1f;
        lakeShininess = 100.0f;
    }

    glMaterialfv(GL_FRONT, GL_AMBIENT, lakeAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, lakeDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, lakeSpecular);
    glMaterialfv(GL_FRONT, GL_EMISSION, lakeEmissive);
    glMaterialf(GL_FRONT, GL_SHININESS, lakeShininess);
    drawLavaLake();

    if (app.state != VolcanoState::DORMANT) {
        float smokeAlpha = 0.35f;
        float smokeR = 0.35f, smokeG = 0.3f, smokeB = 0.28f;
        float smokeEmissiveR = 0.0f, smokeEmissiveG = 0.0f;

        if (app.state == VolcanoState::ACTIVE) {
            smokeAlpha = 0.45f;
            smokeR = 0.4f; smokeG = 0.32f; smokeB = 0.25f;
            smokeEmissiveR = 0.05f + 0.03f * sin(app.stateTimer * 2.0f);
            smokeEmissiveG = smokeEmissiveR * 0.3f;
        } else if (app.state == VolcanoState::ERUPTION) {
            smokeAlpha = 0.65f;
            smokeR = 0.5f; smokeG = 0.3f; smokeB = 0.15f;
            smokeEmissiveR = 0.12f + 0.08f * sin(app.stateTimer * 4.0f);
            smokeEmissiveG = smokeEmissiveR * 0.4f;
        }

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        float smokeAmbient[] = { smokeR, smokeG, smokeB, smokeAlpha };
        float smokeEmissive[] = { smokeEmissiveR, smokeEmissiveG, 0.0f, 1.0f };
        glMaterialfv(GL_FRONT, GL_AMBIENT, smokeAmbient);
        glMaterialfv(GL_FRONT, GL_EMISSION, smokeEmissive);

        drawSmokeCloud();
        glDisable(GL_BLEND);
    }

    drawParticles(app);

    glutSwapBuffers();
}
