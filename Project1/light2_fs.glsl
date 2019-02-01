#version 430 core
out vec4 color;
in VS_OUT
{
 vec2 tc;
 vec4 normals;
 vec4 fragPos;
} fs_in;
layout(binding=0) uniform sampler2D tex;
uniform mat4 model_matrix;
struct lightStruc {
 vec4 lightPosition;
 vec4 id;
};
#define LIGHTS 4
uniform lightStruc lights[LIGHTS];
uniform vec4 viewPosition;
uniform vec4 ia; // Ambient colour
uniform float ka; // Ambient constant
//uniform vec4 id; // diffuse colour
uniform float kd; // Diffuse constant
uniform vec4 is; // specular colour
uniform float ks; // specular constant
uniform float shininess;// shininess constant
uniform float lightConstant;
uniform float lightLinear;
uniform float lightQuadratic;
void main(void){
// Ambient
vec3 ambient = ka * ia.rgb;
vec3 totalAmbient;
vec3 totalDiffuse;
vec3 totalSpecular;
for(int i = 0; i < LIGHTS; i++){
// Diffuse
vec4 lightDir = normalize(lights[i].lightPosition - fs_in.fragPos);
float diff = max(dot(normalize(fs_in.normals), lightDir), 0.0);
vec3 diffuse = kd * lights[i].id.rgb * diff;
// Specular
vec4 viewDir = normalize(viewPosition - fs_in.fragPos);
vec4 reflectDir = reflect(-lightDir, normalize(fs_in.normals));
float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
vec3 specular = ks * is.rgb * spec;
// Attenuation
 float distance = length(lights[i].lightPosition - fs_in.fragPos);
 float attenuation = 1.0f / (lightConstant + lightLinear * distance +
lightQuadratic * (distance * distance));
ambient *= attenuation;
 diffuse *= attenuation;
specular *= attenuation;
totalAmbient += ambient;
totalDiffuse += diffuse;
totalSpecular += specular;
}
// Final Light Contribution
color = vec4(totalAmbient + totalDiffuse + totalSpecular, 1.0) * texture(tex,
fs_in.tc);
}
