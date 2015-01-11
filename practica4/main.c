#include <GL/glut.h>    
#include <AR/gsub.h>    
#include <AR/video.h>   
#include <AR/param.h>   
#include <AR/ar.h>
#include <AR/arMulti.h>
//06_solucion_dibujo3d
// ==== Definicion de constantes y variables globales ===============
ARMultiMarkerInfoT  *mMarker;       // Estructura global Multimarca

int multipatt_id;

// ==== Definicion de estructuras ===================================
struct TObject{
  int id;                      // Identificador del patron
  int visible;                 // Es visible el objeto?
  double width;                // Ancho del patron
  double center[2];            // Centro del patron  
  double patt_trans[3][4];     // Matriz asociada al patron
  void (* drawme)(void);       // Puntero a funcion drawme
};

int nobjects = 0;
struct TObject *objects = NULL;

int dmode = 0;   // Modo dibujo (objeto centrado o cubos en marcas)

// ==== addObject (Anade objeto a la lista de objetos) ==============
void addObject(char *p, double w, double c[2], void (*drawme)(void)) 
{
  int pattid;

  if((pattid=arLoadPatt(p)) < 0) 
    print_error ("Error en carga de patron\n");

  nobjects++;
  objects = (struct TObject *) 
    realloc(objects, sizeof(struct TObject)*nobjects);

  objects[nobjects-1].id = pattid;
  objects[nobjects-1].width = w;
  objects[nobjects-1].center[0] = c[0];
  objects[nobjects-1].center[1] = c[1];
  objects[nobjects-1].drawme = drawme;
}

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
void drawPointer(void) {
  double m[3][4], m2[3][4];
  //arUtilMatInv(mMarker->trans, m);
  //arUtilMatMul(m, objects[0].patt_trans, m2);
  arUtilMatInv(objects[0].patt_trans, m);
  arUtilMatMul(m, mMarker->trans, m2);

  glPointSize(10.0);
  glBegin(GL_POINT_SMOOTH);
    glColor3f(0.5, 0.5, 0.5);
    glVertex3f(0, 0, 0);
    glColor3f(1, 0, 0);
    glVertex3f(m2[0][3], 0, 0);
    glColor3f(0, 0, 1);
    glVertex3f(0, 0, m2[2][3]);
    glColor3f(0, 1, 0);
    glVertex3f(0, m2[1][3], 0);
  glEnd();


  //dist01 = sqrt(pow(m2[0][3],2)+pow(m2[1][3],2)+pow(m2[2][3],2));
  //printf ("Distancia objects[0] y objects[1]= %G\n", dist01);

  drawline(5, 1, 0, 0, 0, 0, 0, m2[0][3], 0, 0);
  drawline(5, 0, 0, 1, 0, 0, 0, 0, 0, m2[2][3]);
  drawline(5, 0, 1, 0, 0, 0, 0, 0, m2[1][3], 0);
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

  int i;
  for (i=0; i<nobjects; i++) {
    if (objects[i].visible) {   // Si el objeto es visible
      argConvGlpara(objects[i].patt_trans, gl_para);   
      glMatrixMode(GL_MODELVIEW);           
      glLoadMatrixd(gl_para);   // Cargamos su matriz de transf.            
      objects[i].drawme();      // Llamamos a su función de dibujar
    }
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

  addObject("data/simple.patt", 120.0, c, drawPointer); 

  argInit(&cparam, 1.0, 0, 0, 0, 0);   // Abrimos la ventana 
}

// ======== mainLoop ================================================
static void mainLoop(void) {
  ARUint8 *dataPtr;
  ARMarkerInfo *marker_info;
  int marker_num, i, j, k;

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
  
  for (i=0; i<nobjects; i++) {
    for(j = 0, k = -1; j < marker_num; j++) {
      if(objects[i].id == marker_info[j].id) {
        if (k == -1) k = j;
        else if(marker_info[k].cf < marker_info[j].cf) k = j;
      }
    }
    
    if(k != -1) {   // Si ha detectado el patron en algun sitio...
      objects[i].visible = 1;
      arGetTransMat(&marker_info[k], objects[i].center, 
        objects[i].width, objects[i].patt_trans);
    } else { objects[i].visible = 0; }  // El objeto no es visible
  }

  if(arMultiGetTransMat(marker_info, marker_num, mMarker) > 0){
    draw();       // Dibujamos los objetos de la escena
  /*    int i,j;
  printf("multipatron\n");
  for(i=0;i<3;i++){
    for(j=0;j<4;j++)
      printf("%f\t", mMarker->trans[i][j]);
    printf("\n");
  }

  printf("patron\n");
  for(i=0;i<3;i++){
    for(j=0;j<4;j++)
      printf("%f\t", objects[0].patt_trans[i][j]);
    printf("\n");
  }
  sleep(0.5);*/
  }
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
