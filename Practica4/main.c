#include <GL/glut.h>    
#include <AR/gsub.h>    
#include <AR/video.h>   
#include <AR/param.h>   
#include <AR/ar.h>
#include <AR/arMulti.h>

// ==== Definicion de constantes y variables globales ===============
ARMultiMarkerInfoT  *mMarker;       // Estructura global Multimarca
int dmode = 0;   // Modo dibujo (objeto centrado o cubos en marcas)

void print_error (char *error) {  printf("%s\n",error); exit(0); }
// ======== cleanup =================================================
static void cleanup(void) {   // Libera recursos al salir ...
  arVideoCapStop();   arVideoClose();   argCleanup();   exit(0);
}

// ======== keyboard ================================================
static void keyboard(unsigned char key, int x, int y) {
  switch (key) {
  case 0x1B: case 'Q': case 'q':  cleanup(); break;
  case 'D': case'd': if (dmode == 0) dmode=1; else dmode=0; break;
  }
}


void drawline (float w, float r, float g, float b, 
                      float x1, float y1, float z1, 
                      float x2, float y2, float z2) {

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
// ======== draw ====================================================
static void draw( void ) {
  double  gl_para[16];   // Esta matriz 4x4 es la usada por OpenGL
  
  argDrawMode3D();              // Cambiamos el contexto a 3D
  argDraw3dCamera(0, 0);        // Y la vista de la camara a 3D
  glClear(GL_DEPTH_BUFFER_BIT); // Limpiamos buffer de profundidad
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);

  argConvGlpara(mMarker->trans, gl_para);   
  glMatrixMode(GL_MODELVIEW);           
  glLoadMatrixd(gl_para);               

  drawQuad();  
  glDisable(GL_DEPTH_TEST);
}

// ======== init ====================================================
static void init( void ) {
  ARParam  wparam, cparam;   // Parametros intrinsecos de la camara
  int xsize, ysize;          // Tamano del video de camara (pixels)
  
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

  argInit(&cparam, 1.0, 0, 0, 0, 0);   // Abrimos la ventana 
}

// ======== mainLoop ================================================
static void mainLoop(void) {
  ARUint8 *dataPtr;
  ARMarkerInfo *marker_info;
  int marker_num;

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
  
  if(arMultiGetTransMat(marker_info, marker_num, mMarker) > 0) 
    draw();       // Dibujamos los objetos de la escena
  
  argSwapBuffers(); // Cambiamos el buffer con lo que tenga dibujado
}

// ======== Main ====================================================
int main(int argc, char **argv) {
  glutInit(&argc, argv);    // Creamos la ventana OpenGL con Glut
  init();                   // Llamada a nuestra funcion de inicio
  
  arVideoCapStart();        // Creamos un hilo para captura de video
  argMainLoop( NULL, keyboard, mainLoop );   // Asociamos callbacks...
  return (0);
}
