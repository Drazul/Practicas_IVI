#include <GL/glut.h>

static GLint angX = 0, angY = 0, angZ = 0;
static int sentido = 1;


void render () { 
  /* Limpieza de buffers */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  /* Carga de la matriz identidad */
  glLoadIdentity();
  /* Posición de la cámara virtual (position, look, up) */
  gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
  
  glRotatef(angX, 1.0, 0.0, 0.0);
  glRotatef(angY, 0.0, 1.0, 0.0);
  glRotatef(angZ, 0.0, 0.0, 1.0);

  /* En color blanco */
  glColor3f( 1.0, 1.0, 1.0 );  
  /* Renderiza la tetera */
  glutWireTeapot(1.5);
  /* Intercambio de buffers... Representation ---> Window */
  glutSwapBuffers();      
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

void rotatex(){
  if (sentido == 1) angX++;
  else angX--;
  glutPostRedisplay();  
}

void rotatey(){
  if (sentido == 1) angY++;
  else angY--;
  glutPostRedisplay();  
}

void rotatez(){
  if (sentido == 1) angZ++;
  else angZ--;
  glutPostRedisplay();  
}
void keyHandle(unsigned char key, int x, int y){
  switch(key){
    case 'x': glutIdleFunc(rotatex); break;
    case 'y': glutIdleFunc(rotatey); break;
    case 'z': glutIdleFunc(rotatez); break;
    case 's': glutIdleFunc(NULL); break;
  }
}

void KeySpecialHandle(int key, int x, int y) {
  switch(key){
    case GLUT_KEY_LEFT: sentido = 1; break;
    case GLUT_KEY_RIGHT: sentido = -1; break;
  }
}


int main (int argc, char* argv[]) { 
  glutInit( &argc, argv ); 
  glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE ); 
  glutInitWindowSize(640, 480); 
  glutCreateWindow( "IVI - Sesion 2" ); 
  glEnable (GL_DEPTH_TEST);
  
  /* Registro de funciones de retrollamada */
  glutDisplayFunc(render); 
  glutReshapeFunc(resize); 
  glutKeyboardFunc(keyHandle);
  glutSpecialFunc(KeySpecialHandle);
  
  /* Bucle de renderizado */
  glutMainLoop();  
  
  return 0; 
} 
