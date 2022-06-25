//Standard for fragment and vertex shaders
#version 330 core
uniform sampler2D tex0;
uniform sampler2D tex1;
uniform sampler2D norm_tex;

//Variables for specular lighting
uniform vec3 cameraPos;
uniform float specStr;
uniform float specPhong;

//Variables for texture coordinates, normals coordinates, and vertex position
in vec2 texCoord;
in vec3 normCoord;
in vec3 fragPos;

in mat3 TBN;

//The color of the vertex
out vec4 FragColor;

void main()
{
    //Initialization of the two textures
    vec4 pixelColor = texture(tex0, texCoord);
    vec4 pixelColor2 = texture(tex1, texCoord);

	vec3 normal = normalize(normCoord);

    FragColor =  vec4(1,1,1,1) * pixelColor + pixelColor2;
}
