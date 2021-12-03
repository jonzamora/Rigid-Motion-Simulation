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
    
      press Spacebar to generate images for hw3 submission.
    
)";
}

static glm::mat3 R;
static glm::vec3 w;
static glm::mat3 modelM;
static glm::mat3 worldM;
static glm::vec3 L;
static double t1;

void initialize( void ) {
    
    printHelp();
    glClearColor(background[0], background[1], background[2], background[3]); // background color
    glViewport(0, 0, width, height);

    //start initializing
    float width = 3; // mu1
    float depth = 4; // mu2
    float height = 5; // mu3

    t1 = glutGet(GLUT_ELAPSED_TIME); // get initial time
    R = glm::mat3(1.0f); // R is the Identity Matrix
    w = glm::vec3(0.0f, 0.1f, 0.0f); // angular velocity w \in R^3 world
    modelM = glm::mat3(glm::vec3(width, 0.0f, 0.0f), glm::vec3(0.0f, depth, 0.0f), glm::vec3(0.0f, 0.0f, height));
    worldM = R * modelM * glm::transpose(R);
    L = worldM * w; // angular momentum
    
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

// Quaternion multiplication p * q
glm::vec4 quatmultiply(const glm::vec4 p, const glm::vec4 q){
    const float p_re = p.w;
    const float q_re = q.w;
    const glm::vec3 p_im(p.x,p.y,p.z);
    const glm::vec3 q_im(q.x,q.y,q.z);
    float r_re = p_re * q_re - glm::dot(p_im,q_im);
    glm::vec3 r_im = p_re * q_im + q_re * p_im + glm::cross(p_im,q_im);
    glm::vec4 r = glm::vec4(r_im,r_re);
    return r;
}
// Quaternion conjugation
glm::vec4 quatconj(const glm::vec4 q){return glm::vec4(-q.x,-q.y,-q.z,q.w);}

glm::mat3 rot(const float degrees, const glm::vec3 axis){
    const float angle = degrees * M_PI/180.0f; // convert to radians
    const glm::vec3 a = glm::normalize(axis);
    glm::mat3 R;
    glm::vec4 q = glm::vec4( glm::sin(0.5f*angle)*a, glm::cos(0.5f*angle) );
    glm::vec4 ii(1.0f, 0.0f, 0.0f, 0.0f);
    glm::vec4 jj(0.0f, 1.0f, 0.0f, 0.0f);
    glm::vec4 kk(0.0f, 0.0f, 1.0f, 0.0f);
    R[0] = glm::vec3(quatmultiply(q,quatmultiply(ii,quatconj(q))));
    R[1] = glm::vec3(quatmultiply(q,quatmultiply(jj,quatconj(q))));
    R[2] = glm::vec3(quatmultiply(q,quatmultiply(kk,quatconj(q))));
    return R;
}

void animation( void ){
        double t2 = glutGet(GLUT_ELAPSED_TIME);
        double dt = t2 - t1;
        t1 = t2;

        w = glm::inverse(worldM) * L;
        glm::vec3 alpha = -glm::inverse(worldM) * (w * L);
        glm::vec3 wNew = w + ((alpha * (dt/2.0f)) + ( (pow(dt, 2) / 12.0f) * (alpha * w)));
        R = rot(dt * glm::length(wNew), glm::normalize(wNew)) * R;
        worldM = R * modelM * glm::transpose(R);

        scene.update(R);
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
    
    initialize();
    glutDisplayFunc(display);
    glutIdleFunc(animation);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKey);
    
    glutMainLoop();
	return 0;   /* ANSI C requires main to return int. */
}
