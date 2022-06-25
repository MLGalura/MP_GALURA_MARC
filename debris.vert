//Standard for fragment and vertex shaders
#version 330 core

//Calls OpenGL to assign the vector 3 to a position "aPos"
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 aTex;
layout(location = 3) in vec3 m_tangent;
layout(location = 4) in vec3 m_bitangent;

out vec2 texCoord;
out vec3 normCoord;
out vec3 fragPos;

out mat3 TBN;

//Assignment of Transformation Matrix
uniform mat4 transform;

uniform mat4 projection;

uniform mat4 view; 

//Creates a single column matrix in the current position
void main()
{
	gl_Position = projection * view * transform * vec4(aPos, 1);

	texCoord = aTex;

	mat3 modelMatrix =  mat3(transpose(inverse(transform)));
	normCoord = modelMatrix * vertexNormal;

	vec3 T = normalize(modelMatrix * m_tangent);
	vec3 N = normCoord;

	T = normalize(T-dot(T,N) * N);
	vec3 B = cross(N, T);

	TBN = mat3(T, B, N);
	fragPos = vec3(transform * vec4(aPos, 1));
}
