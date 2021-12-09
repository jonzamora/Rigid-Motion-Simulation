
// The scene init definition 
#include "Scene.inl"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

using namespace glm;

static glm::mat4 originalM = scale(vec3(1.0f));

void Scene::draw(void){

    // Pre-draw sequence: assign uniforms that are the same for all Geometry::draw call.  These uniforms include the camera view, proj, and the lights.  These uniform do not include modelview and material parameters.
    camera -> computeMatrices();
    shader -> view = camera -> view;
    shader -> projection = camera -> proj;
    shader -> nlights = light.size();
    shader -> lightpositions.resize( shader -> nlights );
    shader -> lightcolors.resize( shader -> nlights );
    int count = 0;
    for ( std::pair<std::string,Light*> entry : light ){
        shader -> lightpositions[ count ] = (entry.second) -> position;
        shader -> lightcolors[ count ] = (entry.second) -> color;
        count++;
    }

    // Define stacks for depth-first search (DFS)
    std::stack < Node* > dfs_stack;
    std::stack < mat4 >  matrix_stack; // HW3: You will update this matrix_stack during the depth-first search while loop.

    // Initialize the current state variable for DFS
    Node* cur = node["world"]; // root of the tree
    mat4 cur_VM = camera -> view; // HW3: You will update this current modelview during the depth first search.  Initially, we are at the "world" node, whose modelview matrix is just camera's view matrix.
    
    // HW3: The following is the beginning of the depth-first search algorithm.
    // HW3: The depth-first search for the node traversal has already been implemented (cur, dfs_stack).
    // HW3: All you have to do is to also update the states of (cur_VM, matrix_stack) alongside the traversal.  You will only need to modify starting from this line.
    dfs_stack.push( cur );
    matrix_stack.push( cur_VM );
    while( ! dfs_stack.empty() ){
        // Detect whether the search runs into infinite loop by checking whether the stack is longer than the size of the graph.
        // Note that, at any time, the stack does not contain repeated element.
        if (dfs_stack.size() > node.size() ){
            std::cerr << "Error: The scene graph has a closed loop." << std::endl;
            exit(-1);
        }

        // top-pop the stacks
        cur = dfs_stack.top();
        dfs_stack.pop();
        // (HW3 hint: you should do something here)
        cur_VM = matrix_stack.top();
        matrix_stack.pop();

        // draw all the models at the current node
        for ( unsigned int i = 0; i < cur -> models.size(); i++ ){
            // Prepare to draw the geometry. Assign the modelview and the material.

            // (HW3 hint: you should do something here)
            shader -> modelview = (cur_VM * (cur->modeltransforms[i])); // HW3: Without updating cur_VM, modelview would just be camera's view matrix.
            shader -> material  = ( cur -> models[i] ) -> material;

            // The draw command
            shader -> setUniforms();
            ( cur -> models[i] ) -> geometry -> draw();
        }

        // Continue the DFS: put all the child nodes of the current node in the stack
        for ( unsigned int i = 0; i < cur -> childnodes.size(); i++ ){
            dfs_stack.push( cur -> childnodes[i] );
            // (HW3 hint: you should do something here)
            matrix_stack.push( cur_VM * (cur->childtransforms[i]) );
        }

    } // End of DFS while loop.
    // HW3: Your code will only be above this line.
}

void Scene::update( glm::mat3 R, glm::vec3 SA1, glm::vec3 SA2, glm::vec3 omega, glm::vec3 b, bool ellipsoids, bool translation)
{

    Node* modelR = node["model"];
    modelR->modeltransforms.pop_back();
    glm::mat4 Rt = glm::mat4(R);
    glm::mat4 B;

    if (translation)
    {
        Rt [3] = glm::vec4(0.0f,0.0f,0.0f,1.0f);
        B = glm::mat4(1.0f);
        B [3] = glm::vec4(b, 1.0f);   
    }

    if (translation)
    {
        modelR->modeltransforms.push_back(B * Rt * originalM);
    }
    else{
        modelR->modeltransforms.push_back(Rt * originalM);
    }

    if (ellipsoids)
    {
        Node* modelE = node["E ellipsoid"];
        modelE->modeltransforms.pop_back();
        modelE->modeltransforms.push_back(scale(SA1) * originalM);

        Node* modelF = node["F ellipsoid"];
        modelF->modeltransforms.pop_back();
        modelF->modeltransforms.push_back(scale(SA2) * originalM);

        Node* modelV = node["velocity"];
        modelV->modeltransforms.pop_back();
        modelV->modeltransforms.push_back(translate(omega) * originalM);
    }

    else
    {
        Node* modelE = node["E ellipsoid"];
        modelE->modeltransforms.pop_back();
        modelE->modeltransforms.push_back(scale(SA1) * originalM * scale(glm::vec3(0.0001f)));

        Node* modelF = node["F ellipsoid"];
        modelF->modeltransforms.pop_back();
        modelF->modeltransforms.push_back(scale(SA2) * originalM * scale(glm::vec3(0.0001f)));

        Node* modelV = node["velocity"];
        modelV->modeltransforms.pop_back();
        modelV->modeltransforms.push_back(translate(omega) * originalM * scale(glm::vec3(0.0001f)));
    }
    
}