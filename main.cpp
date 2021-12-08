#include <stdlib.h>
#include <iostream>
// OSX systems need their own headers
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/glext.h>
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif
// Use of degrees is deprecated. Use radians for GLM functions
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include "Screenshot.h"
#include "Scene.h"
#include <ctime>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <math.h>

static const int width = 800;
static const int height = 600;
static const char* title = "3D Rigid Motion Viewer";
static const glm::vec4 background(0.055f, 0.09f, 0.098f, 1.0f);
static Scene scene;

#include "hw3AutoScreenshots.h"

void printHelp(){
    std::cout << R"(
    Available commands:
      press 'H' to print this message again.
      press Esc to quit.
      press 'O' to save a screenshot.
      press the arrow keys to rotate camera.
      press 'A'/'Z' to zoom.
      press 'R' to reset camera.
      press 'L' to turn on/off the lighting.
      press 'E' to visualize Poinsot's ellipsoids
      press 'T' to translate the rigid object
    
      press Spacebar to generate images for hw3 submission.
    
)";
}

static glm::mat3 R;
static glm::vec3 w;
static glm::mat3 M_model;
static glm::mat3 M_world;
static glm::vec3 L;
static float t1;
static glm::vec3 omega;
static float E;
static float F;
static float mu1, mu2, mu3;
static glm::vec3 SA1, SA2;
static bool ellipsoids = true;
static bool translation = true;
static bool resetT1 = false;
static glm::vec3 b0;
static glm::vec3 v0;
static glm::vec3 a;
static glm::vec3 b;

void initialize( void ) {
    
    printHelp();
    glClearColor(background[0], background[1], background[2], background[3]); // background color
    glViewport(0, 0, width, height);

    //start initializing
    mu1 = 2.0f; // width
    mu2 = 4.0f; // depth
    mu3 = 8.0f; // height

    t1 = glutGet(GLUT_ELAPSED_TIME); // get initial time
    R = glm::mat3(1.0f); // R is the Identity Matrix
    w = glm::vec3(0.1f, 1.0f, 0.3f); // angular velocity w \in R^3 world

    // Poinsot's ellipsoids initialization
    omega = glm::inverse(R) * w;
    E = mu1 * pow(omega.x, 2.0f) + mu2 * pow(omega.y, 2.0f) + mu3 * pow(omega.z, 2.0f); // equation (13)
    F = pow(mu1, 2.0f) * pow(omega.x, 2.0f) + pow(mu2, 2.0f) * pow(omega.y, 2.0f) + pow(mu3, 2.0f) * pow(omega.z, 2.0f); // equation (15)
    SA1 = glm::vec3(glm::sqrt(E/mu1), glm::sqrt(E/mu2), glm::sqrt(E/mu3));
    SA2 = glm::vec3(glm::sqrt(F)/mu1, glm::sqrt(F)/mu2, glm::sqrt(F)/mu3);

    M_model = glm::mat3(glm::vec3(mu1, 0.0f, 0.0f), glm::vec3(0.0f, mu2, 0.0f), glm::vec3(0.0f, 0.0f, mu3));
    M_world = R * M_model * glm::transpose(R);
    L = M_world * w; // angular momentum

    //translation
    b0 = glm::vec3(0.0f,0.5f,0.0f);
    v0 = glm::vec3(0.1f,0.1f,0.1f);
    a = glm::vec3(-0.5f,-0.5f,-0.5f);
    
    b = b0;
    
    // Initialize scene
    scene.init();

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
}

void display(void){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    scene.draw();
    
    glutSwapBuffers();
    glFlush();
    
}

void saveScreenShot(const char* filename = "test.png"){
    int currentwidth = glutGet(GLUT_WINDOW_WIDTH);
    int currentheight = glutGet(GLUT_WINDOW_HEIGHT);
    Screenshot imag = Screenshot(currentwidth,currentheight);
    imag.save(filename);
}

void keyboard(unsigned char key, int x, int y){
    switch(key){
        case 27: // Escape to quit
            exit(0);
            break;
        case 'h': // print help
            printHelp();
            break;
        case 'o': // save screenshot
            saveScreenShot();
            break;
        case 'r':
            scene.camera -> aspect_default = float(glutGet(GLUT_WINDOW_WIDTH))/float(glutGet(GLUT_WINDOW_HEIGHT));
            scene.camera -> reset();
            glutPostRedisplay();
            break;
        case 'a':
            scene.camera -> zoom(0.9f);
            glutPostRedisplay();
            break;
        case 'z':
            scene.camera -> zoom(1.1f);
            glutPostRedisplay();
            break;
        case 'l':
            scene.shader -> enablelighting = !(scene.shader -> enablelighting);
            glutPostRedisplay();
            break;
        case ' ':
            hw3AutoScreenshots();
            glutPostRedisplay();
            break;
        case 'e':
            ellipsoids = !ellipsoids;
            if (ellipsoids == 1)
            {
                std::cout << "ENABLE ELLIPSOIDS: YES" << std::endl;
            }
            else
            {
                std::cout << "ENABLE ELLIPSOIDS: NO" << std::endl;
            }
            break;
        case 't':
            translation = !translation;
            if (translation == 1)
            {
                std::cout << "ENABLE TRANSLATION: YES" << std::endl;
            }
            else
            {
                std::cout << "ENABLE TRANSLATION: NO" << std::endl;
            }
            break;
        default:
            glutPostRedisplay();
            break;
    }
}
void specialKey(int key, int x, int y){
    switch (key) {
        case GLUT_KEY_UP: // up
            scene.camera -> rotateUp(-10.0f);
            glutPostRedisplay();
            break;
        case GLUT_KEY_DOWN: // down
            scene.camera -> rotateUp(10.0f);
            glutPostRedisplay();
            break;
        case GLUT_KEY_RIGHT: // right
            scene.camera -> rotateRight(-10.0f);
            glutPostRedisplay();
            break;
        case GLUT_KEY_LEFT: // left
            scene.camera -> rotateRight(10.0f);
            glutPostRedisplay();
            break;
    }
}

void animation( void )
{
    float t2 = glutGet(GLUT_ELAPSED_TIME);
    float dt = (t2 - t1) / 1000;
    t1 = t2;

    // Algorithm 2: Buss' Augmented Second-Order Method
    w = glm::inverse(M_world) * L;
    glm::vec3 alpha = -glm::inverse(M_world) * glm::cross(w, L);
    glm::vec3 wNew = w + (float(dt / 2.0f) * alpha) + (float(pow(dt, 2.0f)/12.0f) * glm::cross(alpha, w));
    R = glm::mat3(glm::rotate(glm::mat4(1.0f), dt * glm::length(wNew), glm::normalize(wNew))) * R;
    M_world = R * M_model * glm::transpose(R);

    // Poinsot's Ellipsoids
    omega = glm::inverse(R) * w;
    
    // translation
    b = b0 + v0 * t1/1000.0f + 0.5f * a * (float(pow(t1/1000.0f, 2.0f)));

    scene.update(R, SA1, SA2, omega, b, ellipsoids, translation);
    scene.draw();

    glutPostRedisplay();
}

int main(int argc, char** argv)
{
    // BEGIN CREATE WINDOW
    glutInit(&argc, argv);
    
#ifdef __APPLE__
    glutInitDisplayMode( GLUT_3_2_CORE_PROFILE | GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
#else
    glutInitContextVersion(3,1);
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );
#endif
    glutInitWindowSize(width, height);
    glutCreateWindow(title);
#ifndef __APPLE__
    glewExperimental = GL_TRUE;
    GLenum err = glewInit() ;
    if (GLEW_OK != err) {
        std::cerr << "Error: " << glewGetErrorString(err) << std::endl;
    }
#endif
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    // END CREATE WINDOW

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
    
    initialize();
    glutDisplayFunc(display);
    glutIdleFunc(animation);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKey);
    
    glutMainLoop();
	return 0;   /* ANSI C requires main to return int. */
}
