#include <GL/glut.h>    
#include <AR/gsub.h>    
#include <AR/video.h>   
#include <AR/param.h>   
#include <AR/ar.h>
#include <AR/arMulti.h>

void print_error (char *error) {  printf("%s\n",error); exit(0); }

// ==== Definicion de constantes y variables globales ===============
ARMultiMarkerInfoT *mMarker;    // Estructura global Multimarca
int simple_patt_id;             // Identificador unico de la marca simple
double simple_patt_trans[3][4]; // Matriz de transformacion de la marca simple


// ======== cleanup =================================================
static void cleanup(void) {   // Libera recursos al salir ...
  arVideoCapStop();   arVideoClose();   argCleanup();   exit(0);
}

// ======== keyboard ================================================
static void keyboard(unsigned char key, int x, int y) {
  switch (key) {
  case 0x1B: case 'Q': case 'q':  cleanup(); break;
  }
}


void drawline (float w, float r, float g, float b, float x1, float y1, float z1, float x2, float y2, float z2) {
  glColor3f(r, g, b);
  glLineWidth(w);
  glBegin(GL_LINES);
    glVertex3f(x1, y1, z1);
    glVertex3f(x2, y2, z2);
  glEnd();
}

void drawQuad () {
  glColor3f(1, 1, 1);
  glBegin(GL_QUADS);
    glVertex3f(0, 0, 0);
    glVertex3f(0, -210, 0);
    glVertex3f(297, -210, 0);
    glVertex3f(297, 0, 0);
  glEnd();

  drawline(5, 1, 0, 0, 0, 0, 0, 297, 0, 0);
  drawline(5, 0, 1, 0, 0, 0, 0, 0, -210, 0);
  drawline(5, 0, 0, 1, 0, 0, 0, 0, 0, 210);
}

// ==== draw****** (Dibujado especifico de cada objeto) =============
void drawPointer(double m2[3][4]) {
  // Se dibuja el puntero
  glPushMatrix();
  glTranslatef(m2[0][3], m2[1][3], m2[2][3]);
  glColor3f(0, 0, 0);
  glutSolidSphere(2, 20, 20);
  glPopMatrix();
  
  // Se dibujan las proyecciones. Dibujamos linea hasta el borde, trasladamos y dibujamos punto
  drawline(0.5, 1.0, 0.0, 0.0, m2[0][3], m2[1][3], m2[2][3], m2[0][3], 0.0, m2[2][3]);
  glPushMatrix();
  glTranslatef(m2[0][3], 0.0, m2[2][3]);
  glColor3f(1.0, 0.0, 0.0);
  glutSolidSphere(2.0, 20, 20);
  glPopMatrix();
    
  drawline(0.5, 0.0, 1.0, 0.0, m2[0][3], m2[1][3], m2[2][3], m2[0][3], m2[1][3], 0.0);
  glPushMatrix();
  glTranslatef(m2[0][3], m2[1][3], 0.0);
  glColor3f(0.0, 1.0, 0.0);
  glutSolidSphere(2.0, 20, 20);
  glPopMatrix();

  drawline(0.5, 0.0, 0.0, 1.0, m2[0][3], m2[1][3], m2[2][3], 0.0, m2[1][3], m2[2][3]);
  glPushMatrix();
  glTranslatef(0.0, m2[1][3], m2[2][3]);
  glColor3f(0.0, 0.0, 1.0);
  glutSolidSphere(2.0, 20, 20);
  glPopMatrix();
}

// ======== draw ====================================================
void drawAll(int pointer) {
  double gl_para[16];         // Esta matriz 4x4 es la usada por OpenGL
  double m[3][4], m2[3][4];   // Matrices para almacenar la inversa de la matriz del multipatron y para almacenar la posicion relativa entre patron y multipatron
  
  // Se calcula la posicion relativa entre patron y multipatron, si se ha detectado la marca que representa el puntero
  if(pointer != -1) {
    arUtilMatInv(mMarker->trans, m);
    arUtilMatMul(m, simple_patt_trans, m2);
  }
  
  argDrawMode3D();              // Se cambia el contexto a 3D
  argDraw3dCamera(0, 0);        // Y la vista de la camara a 3D
  glClear(GL_DEPTH_BUFFER_BIT); // Se limpia buffer de profundidad
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  
  // Se dibuja el lienzo
  argConvGlpara(mMarker->trans, gl_para);
  glMatrixMode(GL_MODELVIEW);
  glLoadMatrixd(gl_para);
  
  drawQuad();
  drawline(5.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 297.0, 0.0, 0.0);
  drawline(5.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, -210.0, 0.0);
  drawline(5.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 210.0);
  
  // Se dibuja el puntero si se ha detectado la marca
  if(pointer != -1) {
    drawPointer(m2);
  }
  
  glDisable(GL_DEPTH_TEST);
}

// ======== init ====================================================
static void init( void ) {
  ARParam  wparam, cparam;   // Parametros intrinsecos de la camara
  int xsize, ysize;          // Tamano del video de camara (pixels)
  double c[2] = {0.0, 0.0};  // Centro de patron (por defecto)

  // Abrimos dispositivo de video
  if(arVideoOpen("-dev=/dev/video0") < 0) exit(0);  
  if(arVideoInqSize(&xsize, &ysize) < 0) exit(0);

  // Cargamos los parametros intrinsecos de la camara
  if(arParamLoad("data/camera_para.dat", 1, &wparam) < 0)   
    print_error ("Error en carga de parametros de camara\n");
  
  arParamChangeSize(&wparam, xsize, ysize, &cparam);
  arInitCparam(&cparam);   // Inicializamos la camara con "cparam"

  // Cargamos el fichero de especificacion multimarca
  if( (mMarker = arMultiReadConfigFile("data/marker.dat")) == NULL )
    print_error("Error en fichero marker.dat\n");

  // Cargamos el fichero de la marca simple
  if((simple_patt_id = arLoadPatt("data/simple.patt")) < 0) 
    print_error ("Error en carga de patron\n");


  argInit(&cparam, 1.0, 0, 0, 0, 0);   // Abrimos la ventana 
}

// ======== mainLoop ================================================
static void mainLoop(void) {
  ARUint8 *dataPtr;
  ARMarkerInfo *marker_info;
  int marker_num, i, j, k;

  double p_width = 120.0;        // Ancho del patron (marca)
  double p_center[2] = {0.0, 0.0};   // Centro del patron (marca)

  // Capturamos un frame de la camara de video
  if((dataPtr = (ARUint8 *)arVideoGetImage()) == NULL) {
    // Si devuelve NULL es porque no hay un nuevo frame listo
    arUtilSleep(2);  return;  // Dormimos el hilo 2ms y salimos
  }

  argDrawMode2D();
  argDispImage(dataPtr, 0,0);    // Dibujamos lo que ve la camara 

  // Detectamos la marca en el frame capturado (return -1 si error)
  if(arDetectMarker(dataPtr, 100, &marker_info, &marker_num) < 0) {
    cleanup(); exit(0);   // Si devolvio -1, salimos del programa!
  }

  arVideoCapNext();      // Frame pintado y analizado... A por otro!
  
  // Si se detecta la multimarca (lienzo) se puede detectar el puntero
  if(mMarker != NULL) {
    // Se ve donde detecta el patron con mayor fiabilidad
    for(j = 0, k = -1; j < marker_num; j++) {
      if(simple_patt_id == marker_info[j].id) {
        if(k == -1) k = j;
        else if(marker_info[k].cf < marker_info[j].cf) k = j;
      }
    }
  }

  if(k != -1)   // Si ha detectado el patron en algun sitio...
    // Obtener transformacion relativa entre la marca y la camara real
    arGetTransMat(&marker_info[k], p_center, p_width, simple_patt_trans);

  //else if(canDraw) canDraw = !canDraw;  // Si no se detecta, no se puede pintar
  
  if(arMultiGetTransMat(marker_info, marker_num, mMarker) > 0)
    drawAll(k);       // Se dibujan los objetos (lienzo y puntero)
  
  argSwapBuffers(); // Se cambia el buffer con lo que tenga dibujado
}

// ======== Main ====================================================
int main(int argc, char **argv) {
  glutInit(&argc, argv);    // Creamos la ventana OpenGL con Glut
  init();                   // Llamada a nuestra funcion de inicio
  
  arVideoCapStart();        // Creamos un hilo para captura de video
  argMainLoop( NULL, keyboard, mainLoop );   // Asociamos callbacks...
  return (0);
}
