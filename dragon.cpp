/********************************************************/
/*                     Dragon.cpp                                                 */
/********************************************************/
/*                                      */
/********************************************************/

/* inclusion des fichiers d'en-tete freeglut */

#ifdef __APPLE__
#include <GLUT/glut.h> /* Pour Mac OS X */
#else
#include <GL/glut.h>   /* Pour les autres systemes */


#include <GL/freeglut.h>
#include <stdlib.h>
#endif
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <jpeglib.h>
#include <jerror.h>

#include <string>
#define NM 30
#define NP 10



#define larging  800
#define hauting   531

#define large 804
#define haute 468


#include <cmath>
#include <math.h>

#define PI 3.14159265358979323846

using namespace std;

/*class Point*/

class Point{
    public :
	//coordonnées x, y et z du point
	double x;
	double y;
	double z;
	// couleur r, v et b du point
	float r;
	float g;
	float b;
};
//Tableau pour stocker les sommets du cube et leur couleur

Point pCube[8]={
    {-0.5,-0.5, 0.5,1.0,0.0,0.0},
    { 0.5, -0.5, 0.5,0.0,1.0,0.0},
    { 0.5, -0.5, -0.5,0.0,0.0,1.0},
    { -0.5, -0.5, -0.5,1.0,1.0,1.0},
    { -0.5,0.5, 0.5,1.0,0.0,0.0},
    { 0.5, 0.5, 0.5,0.0,1.0,0.0},
    { 0.5, 0.5, -0.5,0.0,0.0,1.0},
    { -0.5, 0.5, -0.5,1.0,1.0,1.0}};

//Tableau pour stocker les indices des sommets par face pour le cube
int fCube[6][4]={
  {0,1,2,3},//face envers endroit={0,3,2,1}
  {0,1,5,4},
  {1,2,6,5},
  {2,3,7,6},
  {0,4,7,3},
  {4,5,6,7}};

unsigned char image[larging*hauting*3];
unsigned char texture[hauting][larging][3];

unsigned char textureS[haute][large][3];
unsigned char imageS[large*haute*3];

char presse;
int anglex,angley,x,y,xold,yold;
double theta;
double fi;
double X[NP*NM];
double Y[NP*NM];
double Z[NP*NM];
int fSphere[(NP-1)*NM][4];
static double ycam=0;
static double xcam=0;
static double zoom=0;
static double Scalexc=1;
static double Scaleyc=1;
static double Scalezc=1;
GLfloat Scalex=0.05, Scaley=0.05, Scalez=0.05;//zoom
GLfloat Translatex, Translatey, Translatez;//deplacement
GLfloat angle=0;


/* Prototype des fonctions */
void affichage();
void clavier(unsigned char touche,int x,int y);
void reshape(int x,int y);
void idle();
void mouse(int bouton,int etat,int x,int y);
void mousemotion(int x,int y);
void animation();
void remplirFace();
void remplirPoint(double R);
void textureVentreDragon(char *fichier);
void textureCube(char *fichier);
void redim(int l,int h);
void constructionDragon();
void dessinCube();
void GestionSpecial(int key, int x, int y);//fonction qui permet la rotation de la caméra avec les flèches directionelle
void affichageScaled(GLfloat scalex,GLfloat scaley, GLfloat scalez);//fonction pour gerer le zoom et le dezoom
void affichageTranslated(GLfloat translatex,GLfloat translatey, GLfloat translatez); //fonction pour gere le deplacement du dragon
void textureSol(char *fichier);//texture pour le sol
void Sol();//fonction plaquage texture au sol

int main(int argc,char **argv)
{
  /* initialisation de glut et creation de la fenetre */
  glutInit(&argc,argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowPosition(400,400);
  glutInitWindowSize(1000,800);
  glutCreateWindow("DRAGON");


 textureVentreDragon("./peauRouge.jpg");//appel de l'image pour la texture
 textureSol("./sol.jpg");//texture placage au sol

  /* Initialisation de l'etat d'OpenGL */
  glClearColor(0.0,0.0,0.0,0.0);
  glShadeModel(GL_FLAT);
  glEnable(GL_DEPTH_TEST);

  /* Mise en place de la projection perspective */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
 // gluPerspective(45.0,1,1.0,5.0);
  glMatrixMode(GL_MODELVIEW);

  /* Parametrage du placage de textures */
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);



  /* enregistrement des fonctions de rappel */
  glutDisplayFunc(affichage);
  glutKeyboardFunc(clavier);
  glutReshapeFunc(reshape);
  glutMouseFunc(mouse);
  glutMotionFunc(mousemotion);
  glutSpecialFunc(GestionSpecial);//permet d'utiliser les touches directionelles du clavier
 glutIdleFunc(idle); //appel de la fonction pour animer càd activation du call back
  glutMainLoop();
    return 0;
}

void affichage()
{
/* effacement de l'image avec la couleur de fond */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glShadeModel(GL_SMOOTH);
  glLoadIdentity();
  glRotatef(angley,1.0,0.0,0.0);
  glRotatef(anglex,0.0,1.0,0.0);
  glScalef(Scalexc,Scaleyc,Scalezc);
  glTranslatef(0,0,0);
  glOrtho(-1.5,1.5,-1.5,1.5,-1.5,1.5);
//gluLookAt(sin(xcam),0,cos(xcam)+ zoom, 0, 0, 0., 0, 1., 0.);//placement de la caméra



/****appel de la fonction constructionDragon pour dessiner le dragon**********/
constructionDragon();

/****appel de la fonction poour plaquer une texture au sol **********/
Sol();

    //Repère
    //axe x en rouge
 glMatrixMode(GL_MODELVIEW);

   glBegin(GL_LINES);
        glColor3f(1,0,0);
    	glVertex3f(0, 0,0.0);
    	glVertex3f(1, 0,0.0);
    glEnd();
    //axe des y en vert
    glBegin(GL_LINES);
    	 glColor3f(0,1,0);
    	glVertex3f(0, 0,0.0);
    	glVertex3f(0, 1,0.0);
    glEnd();
    //axe des z en bleu
    glBegin(GL_LINES);
    	glColor3f(0.0,0.0,1.0);
    	glVertex3f(0, 0,0.0);
    	glVertex3f(0, 0,1.0);
    glEnd();
  glFlush();

  //On echange les buffers
  glutSwapBuffers();
}



/*********************methode de construction d'un dragon*****************************/

void constructionDragon(){

glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,larging,hauting,0,GL_RGB,GL_UNSIGNED_BYTE,texture);
glEnable(GL_TEXTURE_2D);

glTranslatef(Translatex,Translatey,Translatez);//fonction pour delacer haut bas gauche droite
 glScalef(Scalex,Scaley,Scalez);

 glMatrixMode(GL_MODELVIEW);

    glPushMatrix();
    glActiveTextureARB(GL_TEXTURE0_ARB);
      glDisable(GL_TEXTURE_2D);
      glRotatef(25,0,0,1);


/**************Tete du dragon******************************/
//cone pour le cou du dragon
     glPushMatrix();

       glColor3f(0.83,0.31,0.20);
        glTranslatef(-6.5,3.8,0.0);
        glRotatef(90,-1,-0.5,0);
        //glRotatef(10,1,0,0);
        glutSolidCone(1.8, 4.0, 20, 15);
     glPopMatrix();

//cone pour la tete du dragon
     glPushMatrix();
       glColor3f(0.83,0.31,0.20);
        glTranslatef(-8,5.2,0.0);
        glRotatef(90,0,-1,0);
        //glRotatef(10,1,0,0);
        glutSolidCone(1.8, 10.0, 20, 15);
     glPopMatrix();

//cone pour la corne gauche
      glPushMatrix();
        glColor3f(0.41,0.14,0.10);
        glTranslatef(-8.9,9,0.8);
        glRotatef(90,1,0,0);
        //glRotatef(10,1,0,0);
        glutSolidCone(0.2, 5.0, 20, 15);
        //glutSolidCylinder(0.2,2,8,8);

      glPopMatrix();

//cone pour la corne droit
      glPushMatrix();

        glColor3f(0.41,0.14,0.10);
        glTranslatef(-8.9,9,-0.8);
        glRotatef(90,1,0,0);
        //glRotatef(10,1,0,0);
        glutSolidCone(0.2, 5.0, 20, 15);
        //glutSolidCylinder(0.2,2,8,8);
      glPopMatrix();

//sphere oeil droit

        glPushMatrix();

        glColor3f(1.0,1.0,1.0);
        glTranslatef(-9.7,5.6,-1);
        glRotatef(90,1,0,0.0);
        //glRotatef(10,1,0,0);
        remplirPoint(0.5);
        glPopMatrix();

//sphere oeil gauche

        glPushMatrix();

        glColor3f(1.0,1.0,1.0);
        glTranslatef(-9.7,5.6,1);
        glRotatef(90,1,0,0.0);
        //glRotatef(10,1,0,0);
        remplirPoint(0.5);
        glPopMatrix();

//sphere nez

        glPushMatrix();

       glColor3f(0.41,0.14,0.10);
        glTranslatef(-13.5,5.7,0);
        glRotatef(90,1,0,0.0);
        //glRotatef(10,1,0,0);
        remplirPoint(0.5);
        glPopMatrix();


/*****************corps du dragon****************************/

       //premier sphere pour le ventre
      glPushMatrix();
      glActiveTextureARB(GL_TEXTURE0_ARB);
      glEnable(GL_TEXTURE_2D);
          glColor3f(1.0,1.0,1.0);
          glTranslatef(0,0,0);
            //glRotatef(0,0,1,0);
          //glRotatef(90,1,0,0);
          remplirPoint(3);
      glPopMatrix();

     //deuxieme sphere pour le ventre
      glPushMatrix();
         glColor3f(1.0,1.0,1.0);
        glTranslatef(-4,0.9,0);
        //glRotatef(0,0,1,0);
        //glRotatef(90,1,0,0);
        remplirPoint(2.5);
      glPopMatrix();

//cylindre pour coller les deux mains en dessous
      glPushMatrix();
      glActiveTextureARB(GL_TEXTURE0_ARB);
      glDisable(GL_TEXTURE_2D);
        glColor3f(0.41,0.14,0.10);
        glTranslatef(-5.4,-0.6,-2.0);
        glutSolidCylinder(0.3,4,8,8);

      glPopMatrix();

//creation du Cube

      glPushMatrix();
      glActiveTextureARB(GL_TEXTURE0_ARB);
      glEnable(GL_TEXTURE_2D);
      glColor3f(1.0,1.0,1.0);
        glTranslatef(-5.8,2.6,0.0);
        glRotatef(30,0,0,1);
        glScalef(3,3,3);
        dessinCube();
       glPopMatrix();

/**ecail dragon***/

     glPushMatrix();

        glColor3f(0.83,0.31,0.20);
        glTranslatef(0,3.6,0.0);
        glRotatef(90,1,0,0);
        //glRotatef(10,1,0,0);

       glutSolidCylinder(0.1,4,8,8);
     glPopMatrix();


    glPushMatrix();

        glColor3f(0.83,0.31,0.20);
        glTranslatef(2,2.7,0.0);
        glRotatef(90,1,0,0);
        //glRotatef(10,1,0,0);

       glutSolidCylinder(0.1,4,8,8);
     glPopMatrix();

     glPushMatrix();

        glColor3f(0.83,0.31,0.20);
        glTranslatef(4.3,1.4,0.0);
        glRotatef(90,1,0,0);
        //glRotatef(10,1,0,0);

       glutSolidCylinder(0.1,1.1,8,8);
     glPopMatrix();

     glPushMatrix();

       glColor3f(0.83,0.31,0.20);
        glTranslatef(6.6,-0.5,0.0);
        glRotatef(90,1,0,0);
        //glRotatef(10,1,0,0);

       glutSolidCylinder(0.1,1.1,8,8);
     glPopMatrix();



     glPushMatrix();
        glColor3f(0.83,0.31,0.20);
        glTranslatef(1.1,3.4,0.0);
        glRotatef(90,1,0,0);
        //glRotatef(10,1,0,0);

       glutSolidCylinder(0.1,4,8,8);

      glPopMatrix();


      glPushMatrix();

        glColor3f(0.83,0.31,0.20);
        glTranslatef(-0.9,3.6,0.0);
        glRotatef(90,1,0,0);
        //glRotatef(10,1,0,0);

       glutSolidCylinder(0.1,4,8,8);

      glPopMatrix();


      glPushMatrix();

        glColor3f(0.83,0.31,0.20);
        glTranslatef(-2,3.4,0.0);
        glRotatef(90,1,0,0);
        //glRotatef(10,1,0,0);

       glutSolidCylinder(0.1,4,8,8);

      glPopMatrix();


      glPushMatrix();

        glColor3f(0.83,0.31,0.20);
        glTranslatef(-3.3,3.9,0.0);
        glRotatef(90,1,0,0);
        //glRotatef(10,1,0,0);

       glutSolidCylinder(0.1,4,8,8);

      glPopMatrix();


      glPushMatrix();

        glColor3f(0.83,0.31,0.20);
        glTranslatef(-4.3,4.2,0.0);
        glRotatef(90,1,0,0);
        //glRotatef(10,1,0,0);

       glutSolidCylinder(0.1,4,8,8);

      glPopMatrix();


     glPushMatrix();

        glColor3f(0.83,0.31,0.20);
        glTranslatef(-6,6,0.0);
        glRotatef(90,1,0,0);
        //glRotatef(10,1,0,0);

       glutSolidCylinder(0.1,4,8,8);

      glPopMatrix();


     glPushMatrix();

        glColor3f(0.83,0.31,0.20);
        glTranslatef(-5.1,5.3,0.0);
        glRotatef(90,1,0,0);
        //glRotatef(10,1,0,0);

       glutSolidCylinder(0.1,4,8,8);

      glPopMatrix();

     glPushMatrix();

       glColor3f(0.83,0.31,0.20);
        glTranslatef(-7.1,6.8,0.0);
        glRotatef(90,1,0,0);
        //glRotatef(10,1,0,0);

       glutSolidCylinder(0.1,3,8,8);

      glPopMatrix();


/************************Conception de l'Aile*****************************/

   /***faire les ailes Gauche *****************/

   glPushMatrix();
   glRotatef(angle/7,1,0,0);
   glTranslatef(0,2.5,-2.2);
    glRotatef(60,10,0,0);

    //1er etage
    glPushMatrix();
      glActiveTextureARB(GL_TEXTURE0_ARB);
      glEnable(GL_TEXTURE_2D);
        glColor3f(1,1,1);
        glTranslatef(-2.0,3.3,1.1);
        //glRotatef(30,0,0,1);
        glScalef(4,3,0.1);
        dessinCube();

    glPopMatrix();
    //2eme etage du cube
     glPushMatrix();
      glActiveTextureARB(GL_TEXTURE0_ARB);
      glEnable(GL_TEXTURE_2D);
        glColor3f(1,1,1);
        glTranslatef(-1,6.3,1.1);
        //glRotatef(30,0,0,1);
        glScalef(6,3,0.1);
        dessinCube();

    glPopMatrix();
    //3eme etage du cube
     glPushMatrix();
     glActiveTextureARB(GL_TEXTURE0_ARB);
      glEnable(GL_TEXTURE_2D);
        glColor3f(1,1,1);
        glTranslatef(0,9.3,1.1);
        //glRotatef(30,0,0,1);
        glScalef(8,3,0.1);
        dessinCube();

    glPopMatrix();

    //les pointillee du cube grands
      glPushMatrix();
      glActiveTextureARB(GL_TEXTURE0_ARB);
      glDisable(GL_TEXTURE_2D);
        glColor3f(0.41,0.14,0.10);
        glTranslatef(5.7,9.3,1.1);
        //glRotatef(30,0,0,1);
       glScalef(2,1,0.07);
        //dessinCube();
        glutSolidCone(1.7,1.5,3,4);

    glPopMatrix();
    //les pointille du cube 2eme
     glPushMatrix();
         glColor3f(0.41,0.14,0.10);
        glTranslatef(3,6.6,1.1);
        //glRotatef(90,0,-1,0);
       glScalef(2,1,0.07);
        //dessinCube();
        glutSolidCone(2.4,1.5,3,4);

    glPopMatrix();
    //les pointille du cube 1er cube
     glPushMatrix();
        glColor3f(0.41,0.14,0.10);
        glTranslatef(0,4.3,1.1);
        //glRotatef(90,0,-1,0);
       glScalef(1.7,1,0.07);
        //dessinCube();
        glutSolidCone(1.7,1,3,4);

    glPopMatrix();
    //les pointille du cube pointe vers le haut du cube 3eme number 1
     glPushMatrix();
         glColor3f(0.41,0.14,0.10);
        glTranslatef(3,11,1.1);
        glRotatef(70,0,0,1);
       glScalef(1.7,1,0.07);
        //dessinCube();
        glutSolidCone(1.7,1,3,4);

    glPopMatrix();
     //les pointille du cube pointe vers le haut du cube 3eme number 2
     glPushMatrix();
         glColor3f(0.41,0.14,0.10);
        glTranslatef(0,11,1.1);
        glRotatef(90,0,0,1);
       glScalef(1.7,1,0.07);
        //dessinCube();
        glutSolidCone(1.7,1,3,4);

    glPopMatrix();

    //les pointille du cube pointe vers le haut du cube 3eme number 3
     glPushMatrix();
         glColor3f(0.41,0.14,0.10);
        glTranslatef(-3,11,1.1);
        glRotatef(120,0,0,1);
       glScalef(1.7,1,0.07);
        //dessinCube();
        glutSolidCone(1.7,1,3,4);

    glPopMatrix();
   //glPopMatrix();
glPopMatrix();
/***************ailes Droite*******************************/
  glPushMatrix();
     glRotatef(angle/7,-1,0,0);
     glTranslatef(0,2.5,2.2);
     glRotatef(60,-10,0,0);
      glActiveTextureARB(GL_TEXTURE0_ARB);
      glEnable(GL_TEXTURE_2D);
        //1er etage
       glPushMatrix();

        glColor3f(1,1,1);
        glTranslatef(-2.0,3.3,-1.1);
        //glRotatef(30,0,0,1);
        glScalef(4,3,0.1);
        dessinCube();

    glPopMatrix();
    //2eme etage du cube
     glPushMatrix();
      glActiveTextureARB(GL_TEXTURE0_ARB);
      glEnable(GL_TEXTURE_2D);
      glColor3f(1,1,1);
        glTranslatef(-1,6.3,-1.1);
        //glRotatef(30,0,0,1);
        glScalef(6,3,0.1);
        dessinCube();

    glPopMatrix();
    //3eme etage du cube
     glPushMatrix();
      glActiveTextureARB(GL_TEXTURE0_ARB);
      glEnable(GL_TEXTURE_2D);
        glColor3f(1,1,1);
        glTranslatef(0,9.3,-1.1);
        //glRotatef(30,0,0,1);
        glScalef(8,3,0.1);
        dessinCube();

    glPopMatrix();

    //les pointillee du cube grands
         glPushMatrix();
         glActiveTextureARB(GL_TEXTURE0_ARB);
         glDisable(GL_TEXTURE_2D);
         glColor3f(0.41,0.14,0.10);
        glTranslatef(5.7,9.3,-1.1);
        //glRotatef(30,0,0,1);
       glScalef(2,1,0.07);
        //dessinCube();
        glutSolidCone(1.7,1.5,3,4);

    glPopMatrix();
    //les pointille du cube 2eme
     glPushMatrix();
        glColor3f(0.41,0.14,0.10);
        glTranslatef(3,6.6,-1.1);
        //glRotatef(90,0,-1,0);
       glScalef(2,1,0.07);
        //dessinCube();
        glutSolidCone(2.4,1.5,3,4);

    glPopMatrix();
    //les pointille du cube 1er cube
     glPushMatrix();
         glColor3f(0.41,0.14,0.10);
        glTranslatef(0,4.3,-1.1);
        //glRotatef(90,0,-1,0);
       glScalef(1.7,1,0.07);
        //dessinCube();
        glutSolidCone(1.7,1,3,4);

    glPopMatrix();
    //les pointille du cube pointe vers le haut du cube 3eme number 1
     glPushMatrix();
        glColor3f(0.41,0.14,0.10);
        glTranslatef(3,11,-1.1);
        glRotatef(70,0,0,1);
       glScalef(1.7,1,0.07);
        //dessinCube();
        glutSolidCone(1.7,1,3,4);

    glPopMatrix();
     //les pointille du cube pointe vers le haut du cube 3eme number 2
     glPushMatrix();
         glColor3f(0.41,0.14,0.10);
        glTranslatef(0,11,-1.1);
        glRotatef(90,0,0,1);
       glScalef(1.7,1,0.07);
        //dessinCube();
        glutSolidCone(1.7,1,3,4);

    glPopMatrix();

    //les pointille du cube pointe vers le haut du cube 3eme number 3
     glPushMatrix();
        glColor3f(0.41,0.14,0.10);
        glTranslatef(-3,11,-1.1);
        glRotatef(120,0,0,1);
       glScalef(1.7,1,0.07);
        glutSolidCone(1.7,1,3,4);

    glPopMatrix();
glPopMatrix();






/*************************Main Gauche du dragon****************************/

        //cylindre haut de la main gauche
     glPushMatrix();

     glTranslatef(0,0,0);
         glPushMatrix();
         glActiveTextureARB(GL_TEXTURE0_ARB);
         glDisable(GL_TEXTURE_2D);
        glColor3f(0.41,0.14,0.10);
        glTranslatef(-5.5,-0.3,1.7);
        glRotatef(90,1,0,0.0);
        //glRotatef(10,1,0,0);

       glutSolidCylinder(0.5,2,8,8);

        glPopMatrix();

        //sphere articulation gauche haut

        glPushMatrix();
       glActiveTextureARB(GL_TEXTURE0_ARB);
       glEnable(GL_TEXTURE_2D);
       glColor3f(0.83,0.31,0.20);
        glTranslatef(-5.5,-2.6,1.7);
        glRotatef(90,1,0,0.0);
        //glRotatef(10,1,0,0);
        remplirPoint(0.5);


        glPopMatrix();



        //cylindre bas de la main gauche
        glPushMatrix();
        glActiveTextureARB(GL_TEXTURE0_ARB);
        glDisable(GL_TEXTURE_2D);
        glColor3f(0.83,0.31,0.20);
        glTranslatef(-5.5,-2.8,1.7);
        glRotatef(90,1,-1,0.0);
        //glRotatef(10,1,0,0);

        glutSolidCylinder(0.5,2,8,8);

        glPopMatrix();


        //doigt 1 de la main gauche

        glPushMatrix();
            glColor3f(0.83,0.31,0.20);
            glTranslatef(-6.9,-4.2,1.45);
            glRotatef(90,0,-1,0);
            glScalef(0.7,1,0.3);
            //glRotatef(10,1,0,0);
            glutSolidCone(0.3, 6.0, 20, 15);
        glPopMatrix();

        //doigt 2 de la main gauche
        glPushMatrix();
            glColor3f(0.83,0.31,0.20);
            glTranslatef(-6.9,-4.2,1.9);
            glRotatef(90,0,-1,0);
            glScalef(0.7,1,0.3);
            //glRotatef(10,1,0,0);
            glutSolidCone(0.3, 6.0, 20, 15);
        glPopMatrix();
    glPopMatrix();





/**********************Main Droite****************************/

        //cylindre haut de la main droite

        glPushMatrix();
        glActiveTextureARB(GL_TEXTURE0_ARB);
        glDisable(GL_TEXTURE_2D);
        glColor3f(0.41,0.14,0.10);
        glTranslatef(-5.5,-0.3,-1.7);
        glRotatef(90,1,0,0.0);
        //glRotatef(10,1,0,0);

        glutSolidCylinder(0.5,2,8,8);

        glPopMatrix();

        //sphere articulation droite haut

        glPushMatrix();
        glActiveTextureARB(GL_TEXTURE0_ARB);
        glEnable(GL_TEXTURE_2D);
        glColor3f(0.83,0.31,0.20);
        glTranslatef(-5.5,-2.6,-1.7);
        glRotatef(90,1,0,0.0);
        //glRotatef(10,1,0,0);
        remplirPoint(0.5);
        glPopMatrix();

        //cylindre bas de la main droite
        glPushMatrix();
         glActiveTextureARB(GL_TEXTURE0_ARB);
         glDisable(GL_TEXTURE_2D);
         glColor3f(0.83,0.31,0.20);
        glTranslatef(-5.5,-2.8,-1.7);
        glRotatef(90,1,-1,0.0);
        //glRotatef(10,1,0,0);

        glutSolidCylinder(0.5,2,8,8);

        glPopMatrix();

        //doigt 1 de la main droite

        glPushMatrix();
            glColor3f(0.83,0.31,0.20);
            glTranslatef(-6.9,-4.2,-1.45);
            glRotatef(90,0,-1,0);
            glScalef(0.7,1,0.3);
            //glRotatef(10,1,0,0);
            glutSolidCone(0.3, 6.0, 20, 15);
        glPopMatrix();

        //doigt 2 de la main droite
        glPushMatrix();
            glColor3f(0.83,0.31,0.20);
            glTranslatef(-6.9,-4.2,-1.9);
            glRotatef(90,0,-1,0);
            glScalef(0.7,1,0.3);
            //glRotatef(10,1,0,0);
            glutSolidCone(0.3, 6.0, 20, 15);
        glPopMatrix();

 /*****************dessin des Pieds****************************/
      //creation d'un cube pour fixer les pieds

      glPushMatrix();
      glActiveTextureARB(GL_TEXTURE0_ARB);
      glEnable(GL_TEXTURE_2D);

        glColor3f(1.0,1.0,1.0);
        glTranslatef(0,-2.3,0.0);
        //glRotatef(30,0,0,1);
        glScalef(3,2,3);
        dessinCube();
       glPopMatrix();

/**********************pieds Gauche****************************/

    //creation du bras du pieds gauche haut

      glPushMatrix();
      glActiveTextureARB(GL_TEXTURE0_ARB);
      glDisable(GL_TEXTURE_2D);

        glColor3f(0.41,0.14,0.10);
        glTranslatef(0,-2.6,1.1);
        glRotatef(90,1,0,0.0);
        //glRotatef(10,1,0,0);
        glutSolidCylinder(0.5,2,8,8);
      glPopMatrix();

    //sphere articulation pied gauche

      glPushMatrix();
     glActiveTextureARB(GL_TEXTURE0_ARB);
      glEnable(GL_TEXTURE_2D);
        glColor3f(1.0,1.0,1.0);
        glTranslatef(0,-4.7,1.1);
        glRotatef(90,1,0,0.0);
        //glRotatef(10,1,0,0);
        remplirPoint(0.5);
      glPopMatrix();

    //cylindre bas de pieds gauche
        glPushMatrix();
        glActiveTextureARB(GL_TEXTURE0_ARB);
        glDisable(GL_TEXTURE_2D);
        glColor3f(0.83,0.31,0.20);
        glTranslatef(0,-4.9,1.1);
        glRotatef(90,1,-0.5,0.0);
        //glScalef(0.5,4,0.5),
        //glRotatef(10,1,0,0);

        glutSolidCylinder(0.5,3,8,8);

        glPopMatrix();

     //doigt 1 du pieds gauche

        glPushMatrix();
            glColor3f(0.83,0.31,0.20);
            glTranslatef(-1.4,-7.3,0.9);
            glRotatef(90,0,-1,0);
            glScalef(0.7,1,0.3);
            //glRotatef(10,1,0,0);
            glutSolidCone(0.3, 6.0, 20, 15);
        glPopMatrix();

        //doigt 2 du pied gauche

        glPushMatrix();
            glColor3f(0.83,0.31,0.20);
            glTranslatef(-1.4,-7.3,1.4);
            glRotatef(90,0,-1,0);
            glScalef(0.7,1,0.3);
            //glRotatef(10,1,0,0);
            glutSolidCone(0.3, 6.0, 20, 15);
        glPopMatrix();

        //doigt 3 du pied gauche

        glPushMatrix();
             glColor3f(0.83,0.31,0.20);
            glTranslatef(-1.4,-7.3,1.4);
            glRotatef(90,0,-1,1.3);
            glScalef(0.7,1,0.3);
            //glRotatef(10,1,0,0);
            glutSolidCone(0.3, 6.0, 20, 15);
        glPopMatrix();

/********************Pied_droit*****************************/

    //creation du bras du pieds gauche droit

      glPushMatrix();
      glActiveTextureARB(GL_TEXTURE0_ARB);
      glDisable(GL_TEXTURE_2D);
       glColor3f(0.41,0.14,0.10);
        glTranslatef(0,-2.6,-1.1);
        glRotatef(90,1,0,0.0);
        //glRotatef(10,1,0,0);
        glutSolidCylinder(0.5,2,8,8);
      glPopMatrix();

    //sphere articulation pied droit

      glPushMatrix();
      glActiveTextureARB(GL_TEXTURE0_ARB);
      glEnable(GL_TEXTURE_2D);
        glColor3f(1.0,1.0,1.0);
        glTranslatef(0,-4.7,-1.1);
        glRotatef(90,1,0,0.0);
        //glRotatef(10,1,0,0);
        remplirPoint(0.5);
      glPopMatrix();

    //cylindre bas de pieds droit
        glPushMatrix();
        glActiveTextureARB(GL_TEXTURE0_ARB);
         glDisable(GL_TEXTURE_2D);

        glColor3f(0.83,0.31,0.20);
        glTranslatef(0,-4.9,-1.1);
        glRotatef(90,1,-0.5,0.0);
        //glScalef(0.5,4,0.5),
        //glRotatef(10,1,0,0);

        glutSolidCylinder(0.5,3,8,8);

        glPopMatrix();

     //doigt 1 du pieds droit

        glPushMatrix();
           glColor3f(0.83,0.31,0.20);
            glTranslatef(-1.4,-7.3,-0.9);
            glRotatef(90,0,-1,0);
            glScalef(0.7,1,0.3);
            //glRotatef(10,1,0,0);
            glutSolidCone(0.3, 6.0, 20, 15);
        glPopMatrix();

        //doigt 2 du pied droit

        glPushMatrix();
            glColor3f(0.83,0.31,0.20);
            glTranslatef(-1.4,-7.3,-1.4);
            glRotatef(90,0,-1,0);
            glScalef(0.7,1,0.3);
            //glRotatef(10,1,0,0);
            glutSolidCone(0.3, 6.0, 20, 15);
        glPopMatrix();

                //doigt 3 du pied gauche

        glPushMatrix();
            glColor3f(0.83,0.31,0.20);
            glTranslatef(-1.4,-7.3,-1.4);
            glRotatef(90,0,1,-1.3);
            glScalef(0.7,1,0.3);
            //glRotatef(10,1,0,0);
            glutSolidCone(0.3, 6.0, 20, 15);
        glPopMatrix();

/**********************QUEUE***************************/
 glPushMatrix();
   glRotatef(angle/50,0,0,1);
    //sphere 1 source sur la queue
      glPushMatrix();
        glActiveTextureARB(GL_TEXTURE0_ARB);
        glEnable(GL_TEXTURE_2D);
        glColor3f(1.0,1.0,1.0);
        glTranslatef(3,0.8,0);
        glRotatef(90,1,0,0.0);
        //glRotatef(10,1,0,0);
        remplirPoint(1);
      glPopMatrix();

    //Premier cylindre pour la queue
      glPushMatrix();
      glActiveTextureARB(GL_TEXTURE0_ARB);
      glDisable(GL_TEXTURE_2D);
        glColor3f(0.83,0.31,0.20);
        glTranslatef(3.1,0.5,0);
        glRotatef(90,1,1.3,0);
        //glScalef(0.5,4,0.5),
        //glRotatef(10,1,0,0);
        glutSolidCylinder(1,3,8,8);
      glPopMatrix();

   //sphere 2 articulation de la queue

     glPushMatrix();
     glActiveTextureARB(GL_TEXTURE0_ARB);
      glEnable(GL_TEXTURE_2D);
        glColor3f(1.0,1.0,1.0);
        glTranslatef(5.9,-1.6,0);
        glRotatef(90,1,0,0.0);
        //glRotatef(10,1,0,0);
        remplirPoint(1);
     glPopMatrix();

    //cylindre 2 pour la queue
     glPushMatrix();
      glActiveTextureARB(GL_TEXTURE0_ARB);
      glDisable(GL_TEXTURE_2D);
        glColor3f(0.83,0.31,0.20);
        glTranslatef(6.4,-2,0);
        glRotatef(90,1,1.3,0);
        //glScalef(0.5,4,0.5),
        //glRotatef(10,1,0,0);
        glutSolidCylinder(0.6,6,8,8);
      glPopMatrix();

    //sphere 3 articulation de la queue

     glPushMatrix();
     glActiveTextureARB(GL_TEXTURE0_ARB);
      glEnable(GL_TEXTURE_2D);
        glColor3f(1.0,1.0,1.0);
        glTranslatef(11.2,-5.59,0);
        glRotatef(90,1,1.3,0);
        //glRotatef(10,1,0,0);
        remplirPoint(0.6);
     glPopMatrix();


    //cylindre 3 pour la queue
     glPushMatrix();
      glActiveTextureARB(GL_TEXTURE0_ARB);
      glDisable(GL_TEXTURE_2D);
        glColor3f(0.83,0.31,0.20);
        glTranslatef(11.2,-5.7,0);
        glRotatef(90,1,10,0);
        //glScalef(0.5,4,0.5),
        //glRotatef(10,1,0,0);
        glutSolidCylinder(0.5,6,8,8);
      glPopMatrix();

    //sphere pour l'articulation de la fin de queue

     glPushMatrix();
     glActiveTextureARB(GL_TEXTURE0_ARB);
      glEnable(GL_TEXTURE_2D);
        glColor3f(1.0,1.0,1.0);
        glTranslatef(17,-6.3,0);
        glRotatef(90,1,0,0);
        //glRotatef(10,1,0,0);
        remplirPoint(0.6);
     glPopMatrix();
    //cylindre 4 pour la queue
     glPushMatrix();
      glActiveTextureARB(GL_TEXTURE0_ARB);
      glDisable(GL_TEXTURE_2D);
       glColor3f(0.83,0.31,0.20);
        glTranslatef(17,-6.3,0);
        glRotatef(100,1,15,0);
        //glScalef(0.5,4,0.5),
        //glRotatef(10,1,0,0);
        glutSolidCylinder(0.4,6,8,8);
      glPopMatrix();

    //sphere pour l'articulatin proche de la queue

     glPushMatrix();
        glActiveTextureARB(GL_TEXTURE0_ARB);
        glEnable(GL_TEXTURE_2D);
        glColor3f(1.0,1.0,1.0);
        glTranslatef(23,-6.7,-1);
        //glRotatef(90,1,0,0);
        //glRotatef(10,1,0,0);
        remplirPoint(0.4);
     glPopMatrix();

     //cone pour fin de queue

      glPushMatrix();
      glActiveTextureARB(GL_TEXTURE0_ARB);
      glDisable(GL_TEXTURE_2D);
       glColor3f(0.83,0.31,0.20);
        glTranslatef(28,-6.7,-1);
        glRotatef(90,0,-1,0);
        glScalef(1,1.8,1);
        //glRotatef(10,1,0,0);
        glutSolidCone(1.6, 6.0, 20, 15);
     glPopMatrix();

   glPopMatrix();
 glPopMatrix();


glEnd();



}


/*********************plaquer la texture de la foret au sol*************************/
void Sol()
{
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,large,haute,0,GL_RGB,GL_UNSIGNED_BYTE,textureS);
    glEnable(GL_TEXTURE_2D);
    glMatrixMode(GL_MODELVIEW);

 glPushMatrix();
    glColor3f(1,1,1);
    glTranslatef(4,-8,0);
    glScalef(60,1,60);
    dessinCube();

 glPopMatrix();


   glEnd();

}



/*******************methode pour remplir les faces**************************/
  void remplirFace()
{
    for(int j=0;j<NP-1;j++)
    {
      for(int i=0;i<NM;i++)
       {
        fSphere[j*NM+i][0]=j*NM+i;
        fSphere[j*NM+i][1]=j*NM+(i+1)%NM;
        fSphere[j*NM+i][2]=(j+1)*NM+(i+1)%NM;
        fSphere[j*NM+i][3]=j*NM+(i+NM);

       }

    }

}
/*******************methode pour remplir les point avec un rayon en parametre*****/
 void remplirPoint(double R )
 {
    remplirFace();

    for(int j=0;j<NP;j++)
    {
      for(int i=0;i<NM;i++)

        {
            fi=-(PI/2)+(j*PI)/(NP-1);
            theta=i*(2*PI/NM);

             X[i+j*NM]=R*cos(theta)*cos(fi);
             Y[i+j*NM]=R*sin(theta)*cos(fi);
             Z[i+j*NM]=R*sin(fi);

        }
    }


  for (int i=0;i<(NP-1)*NM;i++)
    {

      glBegin(GL_POLYGON);
       //coller la texture sur une face du sphere

            glTexCoord2f((i%NM)/(float)NM, ((NP-1)-(i/NM))/(float)(NP-1));
            glVertex3f(X[fSphere[i][0]],Y[fSphere[i][0]],Z[fSphere[i][0]]);


            glTexCoord2f(((i%NM)+1)/(float)NM, ((NP-1)-(i/NM))/(float)(NP-1));
            glVertex3f(X[fSphere[i][1]],Y[fSphere[i][1]],Z[fSphere[i][1]]);

            glTexCoord2f(((i%NM)+1)/(float)NM, ((NP-2)-(i/NM))/(float)(NP-1));
            glVertex3f(X[fSphere[i][2]],Y[fSphere[i][2]],Z[fSphere[i][2]]);


            glTexCoord2f((i%NM)/(float)NM, ((NP-2)-(i/NM))/(float)(NP-1));
            glVertex3f(X[fSphere[i][3]],Y[fSphere[i][3]],Z[fSphere[i][3]]);

        glEnd();
}
}


/********************Dessiner le cube pour le coup avec des formules parametriques******/
void dessinCube()
{
       // Dessin du cube
  for (int i=0;i<6;i++)
    {
      glBegin(GL_POLYGON);
      //for (int j=0;j<4;j++){
          glTexCoord2f(0,0);
          glVertex3f(pCube[fCube[i][0]].x,pCube[fCube[i][0]].y,pCube[fCube[i][0]].z);

          glTexCoord2f(0,1);
          glVertex3f(pCube[fCube[i][1]].x,pCube[fCube[i][1]].y,pCube[fCube[i][1]].z);

          glTexCoord2f(1,1);
          glVertex3f(pCube[fCube[i][2]].x,pCube[fCube[i][2]].y,pCube[fCube[i][2]].z);

          glTexCoord2f(1,0);
          glVertex3f(pCube[fCube[i][3]].x,pCube[fCube[i][3]].y,pCube[fCube[i][3]].z);


      glEnd();
    }
}






/***************Texture de peau du dragon********************************************/

void textureVentreDragon(char *fichier)
{
  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;
  FILE *file;
  unsigned char *ligne;
  //unsigned char image[256*256*3];
  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_decompress(&cinfo);


#ifdef __WIN32
  if (fopen_s(&file,fichier,"rb") != 0)
    {
      fprintf(stderr,"Erreur : impossible d'ouvrir le fichier texture.jpg\n");
      exit(1);
    }
#elif __GNUC__
  if ((file = fopen(fichier,"rb")) == 0)
    {
      fprintf(stderr,"Erreur : impossible d'ouvrir le fichier texture.jpg\n");
      exit(1);
    }
#endif
  jpeg_stdio_src(&cinfo, file);
  jpeg_read_header(&cinfo, TRUE);

  if ((cinfo.image_width!=larging)||(cinfo.image_height!=hauting)) {
    fprintf(stdout,"Erreur : l'image doit etre de taille 256x256\n");
    exit(1);
  }
  if (cinfo.jpeg_color_space==JCS_GRAYSCALE) {
    fprintf(stdout,"Erreur : l'image doit etre de type RGB\n");
    exit(1);
  }

  jpeg_start_decompress(&cinfo);
  ligne=image;
  while (cinfo.output_scanline<cinfo.output_height)
    {
      ligne=image+3*larging*cinfo.output_scanline;
      jpeg_read_scanlines(&cinfo,&ligne,1);
    }
  jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);

//stocke l'image dans la texture
  for(int i=0;i<hauting;i++)
    {
        for(int j=0;j<larging;j++)
        {
            texture[i][j][0]=image[i*larging*3+j*3];
            texture[i][j][1]=image[i*larging*3+j*3+1];
            texture[i][j][2]=image[i*larging*3+j*3+2];
        }
    }
}



/*************************Texture pour le sol ******************************/

void textureSol(char *fichier)
{
  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;
  FILE *file;
  unsigned char *ligne;
  //unsigned char image[256*256*3];
  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_decompress(&cinfo);


#ifdef __WIN32
  if (fopen_s(&file,fichier,"rb") != 0)
    {
      fprintf(stderr,"Erreur : impossible d'ouvrir le fichier texture.jpg\n");
      exit(1);
    }
#elif __GNUC__
  if ((file = fopen(fichier,"rb")) == 0)
    {
      fprintf(stderr,"Erreur : impossible d'ouvrir le fichier texture.jpg\n");
      exit(1);
    }
#endif
  jpeg_stdio_src(&cinfo, file);
  jpeg_read_header(&cinfo, TRUE);

  if ((cinfo.image_width!=large)||(cinfo.image_height!=haute)) {
    fprintf(stdout,"Erreur : l'image doit etre de taille 256x256\n");
    exit(1);
  }
  if (cinfo.jpeg_color_space==JCS_GRAYSCALE) {
    fprintf(stdout,"Erreur : l'image doit etre de type RGB\n");
    exit(1);
  }

  jpeg_start_decompress(&cinfo);
  ligne=imageS;
  while (cinfo.output_scanline<cinfo.output_height)
    {
      ligne=imageS+3*large*cinfo.output_scanline;
      jpeg_read_scanlines(&cinfo,&ligne,1);
    }
  jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);

//stocke l'image dans la texture
  for(int i=0;i<haute;i++)
    {
        for(int j=0;j<large;j++)
        {
            textureS[i][j][0]=imageS[i*large*3+j*3];
            textureS[i][j][1]=imageS[i*large*3+j*3+1];
            textureS[i][j][2]=imageS[i*large*3+j*3+2];
        }
    }
}

/*************activer les touches du clavier pour faire une tache**********/

void clavier(unsigned char touche,int x,int y)
{
  switch (touche)
    {
    case 'p': /* affichage du carre plein */
      glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
      glutPostRedisplay();
      break;
    case 'f': /* affichage en mode fil de fer */
      glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
      glutPostRedisplay();
      break;
    case 's' : /* Affichage en mode sommets seuls */
      glPolygonMode(GL_FRONT_AND_BACK,GL_POINT);
      glutPostRedisplay();
      break;
    case 'd':
      glEnable(GL_DEPTH_TEST);
      glutPostRedisplay();
      break;
    case 'D':
      glDisable(GL_DEPTH_TEST);
      glutPostRedisplay();
      break;
    case 'a':
      glPolygonMode(GL_FRONT,GL_FILL);
      glPolygonMode(GL_FRONT,GL_LINE);
      glutPostRedisplay();
    break;
    case 'z' : //zoomer
        affichageScaled(0.05, 0.05, 0.05);
        glutPostRedisplay();
    break;
    case 'Z' : //dezoomé
      affichageScaled(-0.05, -0.05, -0.05);
      glutPostRedisplay();
    break;

    case 'h' : // bouge en haut
        affichageTranslated(0.0,0.1,0.0);
        glutPostRedisplay();
    break;
    case 't' : // bouge a droite
        affichageTranslated(0.1,0.0,0.0);
        glutPostRedisplay();
    break;
    case 'g' : // bouge a gauche
        affichageTranslated(-0.1,0.0,0.0);
        glutPostRedisplay();
    break;
    case 'b' : // bouge en bas
        affichageTranslated(0.0,-0.1,0.0);
        glutPostRedisplay();
    break;
    case '9' : // bouge en avant
        affichageTranslated(0.0,0.0,0.1);
        glutPostRedisplay();
    break;
    case '7' : // bouge en arrière
        affichageTranslated(0.0,0.0,-0.1);
        glutPostRedisplay();
    break;

    case 'q' : /*la touche 'q' permet de quitter le programme */
      exit(0);
    }
}

/********************remodeler la vue********************/

void reshape(int x,int y)
{
  if (x<y)
    glViewport(0,(y-x)/2,x,x);
  else
    glViewport((x-y)/2,0,y,y);
}

/*********************souris*****************************/

void mouse(int button, int state,int x,int y)
{
  /* si on appuie sur le bouton gauche */
  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
  {
    presse = 1; /* le booleen presse passe a 1 (vrai) */
    xold = x; /* on sauvegarde la position de la souris */
    yold=y;
  }
  /* si on relache le bouton gauche */
  if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
    presse=0; /* le booleen presse passe a 0 (faux) */
}

/********************mouvement de souris*****************/

void mousemotion(int x,int y)
  {
   //glutPostRedisplay();
    if (presse) /* si le bouton gauche est presse */
    {
      /* on modifie les angles de rotation de l'objet
	 en fonction de la position actuelle de la souris et de la derniere
	 position sauvegardee */
      anglex=anglex+(x-xold);
      angley=angley+(y-yold);
      glutPostRedisplay(); /* on demande un rafraichissement de l'affichage */
    }

    xold=x; /* sauvegarde des valeurs courante de le position de la souris */
    yold=y;

  }

/************************Redimensionner******************/
 void redim(int l,int h)
{
  if (l<h)
    glViewport(0,(h-l)/2,l,l);
  else
    glViewport((l-h)/2,0,h,h);
}



/**********************Methode pour faire l'animation *********/

void idle()
 {
    angle+=2.0;
    if(angle>360)
        angle-=360;


glutPostRedisplay();

  }


/****fonction qui permet la rotation de la caméra avec les flèches directionelle**/

void GestionSpecial(int key, int x, int y)
  {
    switch (key)
    {
     case GLUT_KEY_LEFT ://rotation camera vers la droite
        anglex -= 2;
        glutPostRedisplay();
     break;
     case GLUT_KEY_RIGHT : //rotation de la camera vers la gauche
          anglex += 2;
          glutPostRedisplay();
     break;
     case GLUT_KEY_DOWN : //rotation de la caméra en bas vers le haut
          angley += 2;
          glutPostRedisplay();
     break;
     case GLUT_KEY_UP : //rotation de la camera en haut bas
          angley -= 2;
          glutPostRedisplay();
     break;
    }
}
/********************fonction pour gerer le zoom et le dezoom**************/

void affichageScaled(GLfloat scalex,GLfloat scaley, GLfloat scalez)//zoom dezoom
{
      Scalexc +=scalex;
      Scaleyc +=scaley;
      Scalezc +=scalez;
}

/*****************************fonction pour gere le deplacement du dragon***/


void affichageTranslated(GLfloat translatex,GLfloat translatey, GLfloat translatez)//bouger
{
    Translatex +=translatex;
    Translatey +=translatey;
    Translatez +=translatez;
}


