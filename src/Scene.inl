/**************************************************
Scene.inl contains the definition of the scene graph
*****************************************************/
#include "Scene.h"
#include "Obj.h"

using namespace glm;
void Scene::init(std::string modelName){
    // Create a geometry palette
    geometry["model"] = new Obj;
    geometry["E ellipsoid"] = new Obj;
    geometry["F ellipsoid"] = new Obj;
    geometry["velocity"] = new Obj;

    std::string modelPath = "models/" + modelName;
    const char * fileName = + modelPath.c_str();
    geometry["model"] -> init(fileName); // baymax.obj, drone.obj, planets.obj, racket.obj
    geometry["E ellipsoid"] -> init("models/sphere.obj");
    geometry["F ellipsoid"] -> init("models/sphere.obj");
    geometry["velocity"] -> init("models/velocity_sphere.obj");
    
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

    material["black rubber"] = new Material;
    material["black rubber"] -> ambient = vec4(0.02f, 0.02f, 0.02f, 1.0f);
    material["black rubber"] -> diffuse = vec4(0.01f, 0.01f, 0.01f, 1.0f);
    material["black rubber"] -> specular = vec4(0.4f, 0.4f, 0.4f, 1.0f);
    material["black rubber"] -> shininess = 10.0f;

    material["white rubber"] = new Material;
    material["white rubber"] -> ambient = vec4(0.05f,0.05f,0.05f,1.0f);
    material["white rubber"] -> diffuse = vec4(0.5f,0.5f,0.5f,1.0f);
    material["white rubber"] -> specular = vec4(0.7f,0.7f,0.7f,1.0f);
    material["white rubber"] -> shininess = 10.0f;
    
    // Create a model palette
    model["model"] = new Model;
    model["model"] -> geometry = geometry["model"];

    if (modelName == "baymax.obj")
    {
        model["model"] -> material = material["white rubber"];
    }
    else if (modelName == "drone.obj")
    {
        model["model"] -> material = material["black rubber"];
    }
    else if (modelName == "planets.obj")
    {
        model["model"] -> material = material["red rubber"];
    }
    else if (modelName == "racket.obj")
    {
        model["model"] -> material = material["red rubber"];
    }
    else
    {
        model["model"] -> material = material["black rubber"];
    }

    model["E ellipsoid"] = new Model;
    model["E ellipsoid"] -> geometry = geometry["E ellipsoid"];
    model["E ellipsoid"] -> material = material["green rubber"];

    model["F ellipsoid"] = new Model;
    model["F ellipsoid"] -> geometry = geometry["F ellipsoid"];
    model["F ellipsoid"] -> material = material["cyan rubber"];

    model["velocity"] = new Model;
    model["velocity"] -> geometry = geometry["velocity"];
    model["velocity"] -> material = material["black rubber"];
    
    // Create a light palette
    light["sun"] = new Light;
    light["sun"] -> position = vec4(3.0f,2.0f,1.0f,0.0f);
    light["sun"] -> color = 1.0f*vec4(1.0f,1.0f,1.0f,1.0f);

    // Create a light palette
    light["sun2"] = new Light;
    light["sun2"] -> position = vec4(-3.0f,-2.0f,-1.0f,0.0f);
    light["sun2"] -> color = 1.0f*vec4(1.0f,1.0f,1.0f,1.0f);
    
    // Build the scene graph
    node["model"] = new Node;
    node["model"] -> models.push_back( model["model"] );
    node["model"] -> modeltransforms.push_back( scale(vec3(1.0f)) );

    node["E ellipsoid"] = new Node;
    node["E ellipsoid"] -> models.push_back( model["E ellipsoid"] );
    node["E ellipsoid"] -> modeltransforms.push_back( scale(vec3(1.0f)) );

    node["F ellipsoid"] = new Node;
    node["F ellipsoid"] -> models.push_back( model["F ellipsoid"] );
    node["F ellipsoid"] -> modeltransforms.push_back( scale(vec3(1.0f)) );

    node["velocity"] = new Node;
    node["velocity"] -> models.push_back( model["velocity"] );
    node["velocity"] -> modeltransforms.push_back( scale(vec3(1.0f)) );
    
    node["world"] -> childnodes.push_back( node["model"] );
    node["world"] -> childtransforms.push_back( translate(vec3(0.0f,0.5f,0.0f)));
    node["world"] -> childnodes.push_back( node["E ellipsoid"] );
    node["world"] -> childtransforms.push_back( translate(vec3(0.0f,0.5f,0.0f)));
    node["world"] -> childnodes.push_back( node["F ellipsoid"] );
    node["world"] -> childtransforms.push_back( translate(vec3(0.0f,0.5f,0.0f)));
    node["world"] -> childnodes.push_back( node["velocity"] );
    node["world"] -> childtransforms.push_back( translate(vec3(0.0f,0.5f,0.0f)));
    
    // Put a camera
    camera = new Camera;
    camera -> target_default = vec3( 0.0f, 1.0f, 0.0f );
    camera -> eye_default = vec3( 0.0f, 3.0f, 35.0f );
    camera -> up_default = vec3( 0.0f, 1.0f, 0.0f );
    camera -> reset();
    
    // Initialize shader
    shader = new SurfaceShader;
    shader -> read_source( "shaders/projective.vert", "shaders/lighting.frag" );
    shader -> compile();
    glUseProgram(shader -> program);
    shader -> initUniforms();
}