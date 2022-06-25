//Standard for fragment and vertex shaders
#version 330 core
uniform sampler2D tex0;
uniform sampler2D tex1;
uniform sampler2D norm_tex;

//Variables for light position and color
uniform vec3 lightPos;
uniform vec3 lightColor;

//Variables for ambient lighting
uniform float ambientStr;
uniform vec3 ambientColor;

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
    
    vec3 normal = texture(norm_tex, texCoord).rgb;
    normal = normalize(normal * 2.0 - 1.0);
    normal = normalize(TBN * normal);

    vec3 lightDir = normalize(lightPos - fragPos);

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 ambientCol = ambientColor * ambientStr;

    vec3 viewDir = normalize(cameraPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);

    float spec = pow(max(dot(reflectDir, viewDir), 0.1), specPhong);
    vec3 specColor = spec * specStr * lightColor;

    FragColor =  vec4(specColor + ambientCol + diffuse, 1.0) * pixelColor + pixelColor2;
}
