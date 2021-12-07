#version 330 core

in vec4 position; // raw position in the model coord
in vec3 normal;   // raw normal in the model coord

uniform mat4 modelview; // from model coord to eye coord
uniform mat4 view;      // from world coord to eye coord

// Material parameters
uniform vec4 ambient;
uniform vec4 diffuse;
uniform vec4 specular;
uniform vec4 emision;
uniform float shininess;
uniform float alpha;

// Light source parameters
const int maximal_allowed_lights = 10;
uniform bool enablelighting;
uniform int nlights;
uniform vec4 lightpositions[ maximal_allowed_lights ];
uniform vec4 lightcolors[ maximal_allowed_lights ];

// Output the frag color
out vec4 fragColor;

vec3 direction_vec(vec4 p,vec4 q){
    vec3 v = normalize(p.w*q.xyz-q.w*p.xyz);
    return v;
}

void main (void){
    vec4 origin = vec4(0.0f,0.0f,0.0f,1.0f);
    if (!enablelighting){
        // Default normal coloring (you don't need to modify anything here)
        vec3 N = normalize(normal);
        fragColor = vec4(0.5f*N + 0.5f , 1.0f);
    } else {
        // mat4 eye2w = inverse(view);
        mat3 A = mat3(modelview);
        vec4 eye = origin;
        vec4 pos =  modelview * position;
        vec3 n = normalize(transpose(inverse(A)) * normal);
        fragColor = emision;
        for (int i = 0; i < nlights; i++){
            vec3 l = normalize(direction_vec(pos,view * lightpositions[i]));
            vec3 v = normalize(direction_vec(pos,eye));
            vec3 h = normalize(v + l);
            fragColor += ( ambient + diffuse*max(dot(n,l),0) + specular*pow(max(dot(n,h),0),shininess) ) * lightcolors[i];
            fragColor.a = 0.5f;
        }
        // HW3: You will compute the lighting here.
        
    }
}
