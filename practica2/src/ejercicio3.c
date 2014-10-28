#include <GL/glut.h>

static int type = 1; /* Type indica el triangulo a dibujar. Valores 1 o -1 */

enum {
  MENU_BLANCO = 1,
  MENU_COLOR,
  MENU_EXIT
};

/* Función de renderizado */
void render () {
  /* Limpieza de buffers */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  /* Carga de la matriz identidad */
  glLoadIdentity();
  /* Traslación */
  glTranslatef(0.0, 0.0, -4.0);

  /* Renderiza un triángulo blanco */
  glBegin(GL_TRIANGLES);

  if (type == 1){
    glColor3f(1.0, 1.0, 1.0);
    glVertex3f(0.0, 1.0, 0.0);
    glVertex3f(-1.0, -1.0, 0.0);
    glVertex3f(1.0, -1.0, 0.0);
  }else{
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(0.0, 1.0, 0.0);

    glColor3f(0.0, 1.0, 0.0);
    glVertex3f(-1.0, -1.0, 0.0);

    glColor3f(0.0, 0.0, 1.0);
    glVertex3f(1.0, -1.0, 0.0);
  }
  glEnd();

  /* Intercambio de buffers */
  glutSwapBuffers();
}

void keyHandle(unsigned char key, int x, int y){
  if (key == 'c') type = -type;
  glutPostRedisplay();
}

void SelectFromMenu(int idCommand){
  switch (idCommand){
    case MENU_BLANCO: type = 1; break;
    case MENU_COLOR: type = -1; break;
    case MENU_EXIT: exit(0);
  }
  glutPostRedisplay();

}

int BuildPopupMenu (void)
{
  int menu;
  menu = glutCreateMenu (SelectFromMenu);
  glutAddMenuEntry ("Triangulo blanco\tl", MENU_BLANCO);
  glutAddMenuEntry ("Triangulo color\tp", MENU_COLOR);
  glutAddMenuEntry ("Salir\tEsc", MENU_EXIT);
  return menu;
}

void resize (int w, int h) {
  /* Definición del viewport */
  glViewport(0, 0, w, h);

  /* Cambio a transform. vista */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  /* Actualiza el ratio ancho/alto */
  gluPerspective(50., w/(double)h, 1., 10.);

  /* Vuelta a transform. modelo */
  glMatrixMode(GL_MODELVIEW);
}

void init (void) {
  glEnable(GL_DEPTH_TEST);
}

int main(int argc, char *argv[]) {
  glutInit(&argc, argv);

  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(400, 400);
  glutInitWindowPosition(200, 200);

  glutCreateWindow("Hola Mundo con OpenGL!");

  init();

  /* Create our popup menu */
  BuildPopupMenu ();
  glutAttachMenu (GLUT_RIGHT_BUTTON);

  /* Registro de funciones de retrollamada */
  glutDisplayFunc(render);
  glutReshapeFunc(resize);
  glutKeyboardFunc(keyHandle);

  /* Bucle de renderizado */
  glutMainLoop();

  return 0;
}
