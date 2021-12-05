/**************************************************
Scene.inl contains the definition of the scene graph
*****************************************************/
#include "Scene.h"
#include "Obj.h"

using namespace glm;
void Scene::init(void){
    // Create a geometry palette
    geometry["planets"] = new Obj;
    geometry["E ellipsoid"] = new Obj;
    geometry["F ellipsoid"] = new Obj;

    geometry["planets"] -> init("models/planets.obj");
    geometry["E ellipsoid"] -> init("models/sphere.obj");
    geometry["F ellipsoid"] -> init("models/sphere.obj");
    
    // Create a material palette
    // Source: http://www.it.hiof.no/~borres/j3d/explain/light/p-materials.html
    material["red rubber"] = new Material;
    material["red rubber"] -> ambient = vec4(0.05f,0.0f,0.0f,1.0f);
    material["red rubber"] -> diffuse = vec4(0.5f,0.4f,0.4f,1.0f);
    material["red rubber"] -> specular = vec4(0.7f,0.04f,0.04f,1.0f);
    material["red rubber"] -> shininess = 10.0f;

    material["green rubber"] = new Material;
    material["green rubber"] -> ambient = vec4(0.0f,0.05f,0.0f,1.0f);
    material["green rubber"] -> diffuse = vec4(0.4f,0.5f,0.4f,1.0f);
    material["green rubber"] -> specular = vec4(0.04f,0.7f,0.04f,1.0f);
    material["green rubber"] -> shininess = 10.0f;

    material["cyan rubber"] = new Material;
    material["cyan rubber"] -> ambient = vec4(0.0f,0.05f,0.05f,1.0f);
    material["cyan rubber"] -> diffuse = vec4(0.4f,0.5f,0.5f,1.0f);
    material["cyan rubber"] -> specular = vec4(0.04f,0.7f,0.7f,1.0f);
    material["cyan rubber"] -> shininess = 10.0f;

    
    // Create a model palette
    model["planets"] = new Model;
    model["planets"] -> geometry = geometry["planets"];
    model["planets"] -> material = material["red rubber"];

    model["E ellipsoid"] = new Model;
    model["E ellipsoid"] -> geometry = geometry["E ellipsoid"];
    model["E ellipsoid"] -> material = material["green rubber"];

    model["F ellipsoid"] = new Model;
    model["F ellipsoid"] -> geometry = geometry["F ellipsoid"];
    model["F ellipsoid"] -> material = material["cyan rubber"];
    
    // Create a light palette
    light["sun"] = new Light;
    light["sun"] -> position = vec4(3.0f,2.0f,1.0f,0.0f);
    light["sun"] -> color = 1.0f*vec4(1.0f,1.0f,1.0f,1.0f);
    
    // Build the scene graph
    node["planets"] = new Node;
    node["planets"] -> models.push_back( model["planets"] );
    node["planets"] -> modeltransforms.push_back( scale(vec3(0.5f)) ); // we scale the planets to half the original size

    node["E ellipsoid"] = new Node;
    node["E ellipsoid"] -> models.push_back( model["E ellipsoid"] );
    node["E ellipsoid"] -> modeltransforms.push_back( scale(vec3(0.5f)) ); // we scale the E ellipsoid by half and translate up on y-axis

    node["F ellipsoid"] = new Node;
    node["F ellipsoid"] -> models.push_back( model["F ellipsoid"] );
    node["F ellipsoid"] -> modeltransforms.push_back( scale(vec3(0.5f)) ); // we scale the F ellipsoid by half and translate down on y-axis
    
    node["world"] -> childnodes.push_back( node["planets"] );
    node["world"] -> childtransforms.push_back( translate(vec3(0.0f,0.5f,0.0f)));
    node["world"] -> childnodes.push_back( node["E ellipsoid"] );
    node["world"] -> childtransforms.push_back( translate(vec3(-1.0f,0.25f,0.0f)));
    node["world"] -> childnodes.push_back( node["F ellipsoid"] );
    node["world"] -> childtransforms.push_back( translate(vec3(1.0f,1.75f,0.0f)));
    
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