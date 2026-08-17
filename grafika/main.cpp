#include "volcano.h"

extern AppState g_app;

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(900, 700);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Wulkan - OpenGL");

    initAppState(g_app);
    initOpenGL(g_app);

    glutDisplayFunc([]() { displayScene(g_app); });
    glutReshapeFunc(reshape);
    glutKeyboardFunc([](unsigned char k, int x, int y) { keyboard(g_app, k, x, y); });
    glutSpecialFunc([](int k, int x, int y) { specialKeys(g_app, k, x, y); });
    glutMouseFunc([](int b, int s, int x, int y) { mouse(g_app, b, s, x, y); });
    glutTimerFunc(16, timerFunc, 0);

    createMenu(g_app);

    glutMainLoop();
    return 0;
}
