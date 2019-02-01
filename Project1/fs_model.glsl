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

uniform vec4 lightPosition;
uniform vec4 viewPosition;

struct lightStruc {
 vec4 lightPosition;
 vec4 id;
 float shininess;
 vec4 ia;
 vec4 is;
 float ka;
 float kd;
 float ks;
};
#define LIGHTS 25
uniform lightStruc lights[LIGHTS];
uniform vec4 lightSpotDirection;
uniform float lightSpotCutOff;

		// Ambient colour
		// Ambient constant
    // diffuse colour
		// Diffuse constant
    // specular colour
	   // specular constant
uniform float transparency;


void main(void){
  vec3 ambient;
  vec3 totalAmbient;
  vec3 totalDiffuse;
  vec3 totalSpecular;
  vec4 lightDir = normalize(lightPosition - fs_in.fragPos);
  float theta = dot(lightDir, normalize(-lightSpotDirection));




for(int i = 0; i < LIGHTS; i++){


	// Diffuse
    ambient = lights[i].ka * lights[i].ia.rgb;
	  vec4 lightDir = normalize(lights[i].lightPosition - fs_in.fragPos);
	  float diff = max(dot(normalize(fs_in.normals), lightDir), 0.0);
	  vec3 diffuse =  lights[i].kd * lights[i].id.rgb * diff;

	// Specular
	  vec4 viewDir = normalize(viewPosition - fs_in.fragPos);
	  vec4 reflectDir = reflect(-lightDir, normalize(fs_in.normals));
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), lights[i].shininess);
    vec3 specular = lights[i].ks * lights[i].is.rgb * spec;


    totalAmbient += ambient;
    totalDiffuse += diffuse;
    totalSpecular += specular;

  color = vec4(totalAmbient + totalDiffuse + totalSpecular, transparency) * texture(tex, fs_in.tc);


}
}
