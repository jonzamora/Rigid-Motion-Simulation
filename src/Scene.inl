/**************************************************
Scene.inl contains the definition of the scene graph
*****************************************************/
#include "Scene.h"
#include "Obj.h"

using namespace glm;
void Scene::init(void){
    // Create a geometry palette
    geometry["planets"] = new Obj;
    geometry["planets"] -> init("models/planets.obj");
    
    // Create a material palette
    material["white"] = new Material;
    material["white"] -> ambient = vec4(0.05f, 0.05f, 0.05f, 1.0f);
    material["white"] -> diffuse = vec4(0.5f, 0.5f, 0.5f, 1.0f);
    material["white"] -> specular = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    material["white"] -> emision = vec4(0.7f,0.7f,0.7f,1.0f);
    material["white"] -> shininess = 78.125f;

    // Source: http://www.it.hiof.no/~borres/j3d/explain/light/p-materials.html
    material["red rubber"] = new Material;
    material["red rubber"] -> ambient = vec4(0.05f,0.0f,0.0f,1.0f);
    material["red rubber"] -> diffuse = vec4(0.5f,0.4f,0.4f,1.0f);
    material["red rubber"] -> specular = vec4(0.7f,0.04f,0.04f,1.0f);
    material["red rubber"] -> shininess = 10.0f ;
    
    // Create a model palette
    model["planets"] = new Model;
    model["planets"] -> geometry = geometry["planets"];
    model["planets"] -> material = material["red rubber"];
    
    // Create a light palette
    light["sun"] = new Light;
    light["sun"] -> position = vec4(3.0f,2.0f,1.0f,0.0f);
    light["sun"] -> color = 1.0f*vec4(1.0f,1.0f,1.0f,1.0f);
    
    // Build the scene graph
    node["planets"] = new Node;
    node["planets"] -> models.push_back( model["planets"] );
    node["planets"] -> modeltransforms.push_back( scale(vec3(1.0f)) ); // The horizontal position of planets is changed here
    
    node["world"] -> childnodes.push_back( node["planets"] );
    node["world"] -> childtransforms.push_back( translate(vec3(0.0f,0.0f,0.0f)) * rotate( 135.0f*float(M_PI)/180.0f, vec3(0.0f, 1.0f, 0.0f) ));
    
    // Put a camera
    camera = new Camera;
    camera -> target_default = vec3( 0.0f, 1.0f, 0.0f );
    camera -> eye_default = vec3( 0.0f, 1.0f, 5.0f );
    camera -> up_default = vec3( 0.0f, 1.0f, 0.0f );
    camera -> reset();
    
    // Initialize shader
    shader = new SurfaceShader;
    shader -> read_source( "shaders/projective.vert", "shaders/lighting.frag" );
    shader -> compile();
    glUseProgram(shader -> program);
    shader -> initUniforms();
}