/************************************************************
This code was written for the completion of the requirement:
Finals - Machine Project for GDGRAP1 - S11

NAME: GALURA, MARC LAWRENCE C.
SECTION: S11
DATE: JUNE 2, 2022
*************************************************************/

//Headers used for the program
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//Defines the type for the window output screen
GLFWwindow* window;

//Variables used for the window output screen size
float width = 800.0f;
float height = 800.0f;

//Variables for time and frame logic and camera movement
float timeframe = 0.0f;
float lastFrame = 0.0f;
float currentFrame = 0.0f;
float cameraSpeed = 0.0f;

//Defines the identity matrix for matrix transformations
glm::mat4 identity = glm::mat4(1.0f);

//Variables that control which state is active
int camstate = 1;
int lightstate = 1;

//Array that defines the vertices for the cube in which the skybox will be placed
float skyboxVertices[]
{
	-200.f, -200.f, 200.f,
	200.f, -200.f, 200.f,
	200.f, -200.f, -200.f,
	-200.f, -200.f, -200.f,
	-200.f, 200.f, 200.f,
	200.f, 200.f, 200.f,
	200.f, 200.f, -200.f,
	-200.f, 200.f, -200.f
};

//Array that defines the indices for the skybox that will be used in the scene
unsigned int skyboxIndices[]
{
	1,2,6,6,5,1,
	0,4,7,7,3,0,
	4,5,6,6,7,4,
	0,3,2,2,1,0,
	0,1,5,5,4,0,
	3,7,6,6,2,3
};

//Array that lists the images that will be used for each face of the skybox
std::string skyboxFaces[]
{
	"Skybox/bkg1_right.png",
	"Skybox/bkg1_left.png",
	"Skybox/bkg1_top.png",
	"Skybox/bkg1_bot.png",
	"Skybox/bkg1_front.png",
	"Skybox/bkg1_back.png"
};

//Class that holds the variables for the objects inside in the scene
class Object
{
public:
	//Variables that control the transformation matrices of the object
	glm::mat4 transform;
	glm::vec3 position;
	glm::vec3 scaling;
	glm::vec3 rotation;

	//Variables that hold the relative path to the mesh and attributes to the object
	std::string path;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> material;
	std::string warning, error;
	tinyobj::attrib_t attributes;

	std::vector<GLuint> mesh_indices;
	std::vector<GLfloat> fullVertexData;

	//Variables used for initializing shaders and buffers
	std::stringstream vertStrStream;
	std::stringstream fragStrStream;

	//Variables for the textures and normal maps
	GLuint texture;
	GLuint texture2;
	GLuint normal;
	GLuint shaderProgram;
	GLuint VAO, VBO;

	//Variables for the tangents and bitangents for the object normals
	std::vector<glm::vec3> tangents;
	std::vector<glm::vec3> bitangents;

	//Variables for the details of the image
	int img_width, img_width2, img_width3;
	int img_height, img_height2, img_height3;
	int colorChannel, colorChannel2, colorChannel3;

	//Variables to retrieve the relative path to the textures in the main function
	unsigned char* picturepath;
	unsigned char* picturepath2;
	unsigned char* normalpath;

	//Function that loads the mesh of the object and initializes the array of vertex data
	void objectLoad()
	{
		//Iterate through all of the vertices and push the indices to the vector
		bool success = tinyobj::LoadObj(&attributes, &shapes, &material, &warning, &error, path.c_str());

		for (int i = 0; i < shapes[0].mesh.indices.size(); i++)
		{
			mesh_indices.push_back(shapes[0].mesh.indices[i].vertex_index);
		}

		//Iterates through the tangents and bitangents of the object normals
		for (int i = 0; i < shapes[0].mesh.indices.size(); i += 3)
		{
			tinyobj::index_t vData1 = shapes[0].mesh.indices[i];
			tinyobj::index_t vData2 = shapes[0].mesh.indices[i + 1];
			tinyobj::index_t vData3 = shapes[0].mesh.indices[i + 2];

			glm::vec3 v1 = glm::vec3(
				attributes.vertices[(vData1.vertex_index * 3)],
				attributes.vertices[(vData1.vertex_index * 3) + 1],
				attributes.vertices[(vData1.vertex_index * 3) + 2]);

			glm::vec3 v2 = glm::vec3(
				attributes.vertices[(vData2.vertex_index * 3)],
				attributes.vertices[(vData2.vertex_index * 3) + 1],
				attributes.vertices[(vData2.vertex_index * 3) + 2]);

			glm::vec3 v3 = glm::vec3(
				attributes.vertices[(vData3.vertex_index * 3)],
				attributes.vertices[(vData3.vertex_index * 3) + 1],
				attributes.vertices[(vData3.vertex_index * 3) + 2]);

			glm::vec2 uv1 = glm::vec2(
				attributes.texcoords[vData1.texcoord_index * 2],
				attributes.texcoords[(vData1.texcoord_index * 2) + 1]);

			glm::vec2 uv2 = glm::vec2(
				attributes.texcoords[vData2.texcoord_index * 2],
				attributes.texcoords[(vData2.texcoord_index * 2) + 1]);

			glm::vec2 uv3 = glm::vec2(
				attributes.texcoords[vData3.texcoord_index * 2],
				attributes.texcoords[(vData3.texcoord_index * 2) + 1]);

			glm::vec3 deltaPos1 = v2 - v1;
			glm::vec3 deltaPos2 = v3 - v1;

			glm::vec2 deltaUV1 = uv2 - uv1;
			glm::vec2 deltaUV2 = uv3 - uv1;

			float r = 1.0f / ((deltaUV1.x * deltaUV2.y) - (deltaUV1.y * deltaUV2.x));

			glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
			glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

			//Pushes back 3 times because it is incremented by 3
			tangents.push_back(tangent);
			tangents.push_back(tangent);
			tangents.push_back(tangent);

			bitangents.push_back(bitangent);
			bitangents.push_back(bitangent);
			bitangents.push_back(bitangent);
		}

		for (int i = 0; i < shapes[0].mesh.indices.size(); i++)
		{
			//Assignment of index data for easier access
			tinyobj::index_t vData = shapes[0].mesh.indices[i];

			//Pushes the XYZ positions, normals, and UT values of the Vertex respectively
			fullVertexData.push_back(attributes.vertices[(vData.vertex_index * 3)]);
			fullVertexData.push_back(attributes.vertices[(vData.vertex_index * 3) + 1]);
			fullVertexData.push_back(attributes.vertices[(vData.vertex_index * 3) + 2]);

			fullVertexData.push_back(attributes.normals[(vData.normal_index * 3)]);
			fullVertexData.push_back(attributes.normals[(vData.normal_index * 3) + 1]);
			fullVertexData.push_back(attributes.normals[(vData.normal_index * 3) + 2]);

			fullVertexData.push_back(attributes.texcoords[(vData.texcoord_index * 2)]);
			fullVertexData.push_back(attributes.texcoords[(vData.texcoord_index * 2) + 1]);

			fullVertexData.push_back(tangents[i].x);
			fullVertexData.push_back(tangents[i].y);
			fullVertexData.push_back(tangents[i].z);

			fullVertexData.push_back(bitangents[i].x);
			fullVertexData.push_back(bitangents[i].y);
			fullVertexData.push_back(bitangents[i].z);
		}
	}

	//Function to initialize the shader for the objects
	void initShader()
	{
		std::string vertStr = vertStrStream.str();
		std::string fragStr = fragStrStream.str();

		const char* vertSrc = vertStr.c_str();
		const char* fragSrc = fragStr.c_str();

		GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
		GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);

		glShaderSource(vertShader, 1, &vertSrc, NULL);
		glShaderSource(fragShader, 1, &fragSrc, NULL);
		glCompileShader(vertShader);
		glCompileShader(fragShader);

		shaderProgram = glCreateProgram();
		glAttachShader(shaderProgram, vertShader);
		glAttachShader(shaderProgram, fragShader);

		glLinkProgram(shaderProgram);
	}

	//Function that generates the textures without an alpha channel for the object
	void genTexNA()
	{
		//Generates 1 texture onto the selected 3D model
		glGenTextures(1, &texture);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);

		//Repeats and clamps the texture image on the object
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

		//Sets texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img_width, img_height, 0, GL_RGB, GL_UNSIGNED_BYTE, picturepath);

		//Mipmaps used for the object textures
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(picturepath);
	}

	//Function that generates the textures with an alpha channel for the object
	void genTexWA()
	{
		//Generates 1 texture onto the selected 3D model
		glGenTextures(1, &texture);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);

		//Repeats and clamps the texture image on the object
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

		//Sets texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_width, img_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, picturepath);

		//Mipmaps used for the object textures
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(picturepath);
	}

	//Function that generates the secondary textures without an alpha channel for the object
	void genTex2NA()
	{
		//Generates 1 texture onto the selected 3D model
		glGenTextures(1, &texture2);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);

		//Repeats and clamps the texture image on the object
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

		//Sets texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img_width2, img_height2, 0, GL_RGB, GL_UNSIGNED_BYTE, picturepath2);

		//Mipmaps used for the object textures
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(picturepath2);
	}

	//Function that generates the secondary textures with an alpha channel for the object
	void genTex2WA()
	{
		//Generates 1 texture onto the selected 3D model
		glGenTextures(1, &texture2);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);

		//Repeats and clamps the texture image on the object
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

		//Sets texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_width2, img_height2, 0, GL_RGBA, GL_UNSIGNED_BYTE, picturepath2);

		//Mipmaps used for the object textures
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(picturepath2);
	}

	//Function that generates the normal textures of the object	
	void genNorm()
	{
		//Generates 1 texture onto the selected 3D model
		glGenTextures(1, &normal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, normal);

		//Repeats and clamps the texture image on the object
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

		//Sets texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img_width3, img_height3, 0, GL_RGB, GL_UNSIGNED_BYTE, normalpath);

		//Mipmaps used for the object textures
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(normalpath);
	}

	//Function that initializes the buffers for objects in the scene
	void initBuff()
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData
		(
			GL_ARRAY_BUFFER,
			sizeof(GLfloat) * fullVertexData.size(),
			fullVertexData.data(),
			GL_DYNAMIC_DRAW
		);

		//Creates an offset for the vertex array
		GLintptr uvPtr = 6 * sizeof(GLfloat);
		GLintptr normPtr = 3 * sizeof(GLfloat);
		GLintptr tangentPtr = 8 * sizeof(GLfloat);
		GLintptr bitangentPtr = 11 * sizeof(GLfloat);

		//Obtains the position, normal, and UV data from the array (Index, Position, Data Type, Normalized State, Floats present)
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 14 * sizeof(float), (void*)normPtr);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)uvPtr);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)tangentPtr);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)bitangentPtr);

		//Enables the vertex attribute arrays and binds the buffers
		glEnableVertexAttribArray(4);
		glEnableVertexAttribArray(3);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	//Applies the respective transformations for the objects
	void objectTransform()
	{
		transform = glm::translate(identity, position);
		transform = glm::scale(transform, scaling);
		transform = glm::rotate(transform, glm::radians(rotation.x), glm::vec3(1, 0, 0));
		transform = glm::rotate(transform, glm::radians(rotation.y), glm::vec3(0, 1, 0));
		transform = glm::rotate(transform, glm::radians(rotation.z), glm::vec3(0, 0, 1));

		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);

		unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

		glActiveTexture(GL_TEXTURE0);
		GLuint tex0Loc = glGetUniformLocation(shaderProgram, "tex0");
		glBindTexture(GL_TEXTURE_2D, texture);
		glUniform1i(tex0Loc, 0);

		glActiveTexture(GL_TEXTURE1);
		GLuint tex1Loc = glGetUniformLocation(shaderProgram, "tex1");
		glBindTexture(GL_TEXTURE_2D, texture2);
		glUniform1i(tex1Loc, 1);

		glActiveTexture(GL_TEXTURE2);
		GLuint norm_texLoc = glGetUniformLocation(shaderProgram, "norm_tex");
		glBindTexture(GL_TEXTURE_2D, normal);
		glUniform1i(norm_texLoc, 2);
	}

	//Resets the buffers after use
	void resetBuff()
	{
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
	}
};

//Class that holds the variables for the lighting inside in the scene
class Light
{
public:
	//Handles the light position and color
	glm::vec3 lightPos = glm::vec3(0.0f, 0.0f, 100.0f);
	glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);

	//Handles the color and strength of ambient light
	glm::vec3 ambientColor = lightColor;
	float ambientStr = 0.3f;

	//Handles the strength and phong size of the specular light
	float specStr = 0.5f;
	float specPhong = 16.0f;

	//Applies the respective transformations for the light
	void lightTransform(GLuint program)
	{
		GLuint lightPosAddress = glGetUniformLocation(program, "lightPos");
		glUniform3fv(lightPosAddress, 1, glm::value_ptr(lightPos));

		GLuint lightColorAddress = glGetUniformLocation(program, "lightColor");
		glUniform3fv(lightColorAddress, 1, glm::value_ptr(lightColor));

		GLuint ambientColorAddress = glGetUniformLocation(program, "ambientColor");
		glUniform3fv(ambientColorAddress, 1, glm::value_ptr(ambientColor));

		GLuint ambientStrAddress = glGetUniformLocation(program, "ambientStr");
		glUniform1f(ambientStrAddress, ambientStr);

		GLuint specStrAddress = glGetUniformLocation(program, "specStr");
		glUniform1f(specStrAddress, ambientStr);

		GLuint specPhongAddress = glGetUniformLocation(program, "specPhong");
		glUniform1f(specPhongAddress, specPhong);
	}
};

//Class that holds the variables for controlling the camera in the scene
class Camera
{
public:
	//Variables for the camera's position and orientation
	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, -10.0f);
	glm::vec3 WorldUp = glm::vec3(0, 1, 0);
	glm::vec3 centerPos = glm::vec3(0.0f, 0.0f, 0.0f);

	glm::vec3 F = glm::normalize(centerPos - cameraPos);
	glm::vec3 R = glm::normalize(glm::cross(F, WorldUp));
	glm::vec3 U = glm::normalize(glm::cross(R, F));
	glm::vec3 front;

	glm::mat4 projection;
	glm::mat4 view = glm::lookAt(cameraPos, centerPos, WorldUp);

	//Variables used for the camera to mouse movement
	float yaw = 90.0f;
	float pitch = 0.0f;
	float radius = 10.0f;

	bool mouseState = true;
	float mousexPos = width / 2.0f;
	float mouseyPos = height / 2.0f;

	//Applies the respective transformations for the camera
	void cameraTransform(GLuint program)
	{
		unsigned int projectionLoc = glGetUniformLocation(program, "projection");
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

		unsigned int viewLoc = glGetUniformLocation(program, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	}
};

//Instantiation of the objects, cameras, and lighting present in the scene
Object Ship;
Object Planet;
Object AmongUs;
Object Asteroid;
Object Moon;
Object Moon2;
Object Station;
Object Background;
Light MainLight;
Camera PlayerPOV;

//Ensures that GLFW registers the space key as one input instead of querying every frame
void input_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	//Determines which projection matrix will be used
	if (key == GLFW_KEY_1 && action == 1)
	{
		if (camstate == 1)
			camstate = 2;

		else if (camstate == 2)
			camstate = 1;			
	}

	//Allows the light to move from planet to the ship
	if (key == GLFW_KEY_F && action == 1)
	{
		if (lightstate == 1)
			lightstate = 2;

		else if (lightstate == 2)
			lightstate = 1;
	}

	//Allows the user to exit the program with the escape key
	if (key == GLFW_KEY_ESCAPE && action == 1)
		glfwSetWindowShouldClose(window, true);
}

//Function to control fps camera movement via cursor
void cursorMove(GLFWwindow* window, double xpos, double ypos)
{
	//Sets the mouse's coordinates accordingly, and disables mouseState to prevent repetition in the next instance
	if (PlayerPOV.mouseState)
	{
		PlayerPOV.mousexPos = xpos;
		PlayerPOV.mouseyPos = ypos;
		PlayerPOV.mouseState = false;
	}

	//Records the changes in x and y values of the cursor, then sets the new changes as the latest x and y positions
	float xoffset = xpos - PlayerPOV.mousexPos;
	float yoffset = PlayerPOV.mouseyPos - ypos;
	PlayerPOV.mousexPos = xpos;
	PlayerPOV.mouseyPos = ypos;

	//Determines the sensitivity of the pitch and yaw axis movement accordingly to the given value
	float sensitivity = 0.05f;

	xoffset *= sensitivity;
	yoffset *= sensitivity;

	PlayerPOV.yaw += xoffset;
	PlayerPOV.pitch += yoffset;

	//Prevents screen flipping by setting bounds to the y/pitch axis
	if (PlayerPOV.pitch > 89.9f)
		PlayerPOV.pitch = 89.9f;

	if (PlayerPOV.pitch < -89.9f)
		PlayerPOV.pitch = -89.9f;

	//Computation of xyz values of the front of the camera when moving the cursor
	PlayerPOV.front.x = cos(glm::radians(PlayerPOV.yaw)) * cos(glm::radians(PlayerPOV.pitch));
	PlayerPOV.front.y = sin(glm::radians(PlayerPOV.pitch));
	PlayerPOV.front.z = sin(glm::radians(PlayerPOV.yaw)) * cos(glm::radians(PlayerPOV.pitch));

	PlayerPOV.F = glm::normalize(PlayerPOV.front);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

int main(void)
{
	//Determines if the glfw library has been initialized
	if (!glfwInit())
		return -1;

	//Creates a windowed mode window and its OpenGL context
	window = glfwCreateWindow(width, height, "MP-GALURA_MARC", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	//Sets the current window's context to current
	glfwMakeContextCurrent(window);
	gladLoadGL();

	//Handles change in mouse cursor position movement
	glfwSetCursorPosCallback(window, cursorMove);

	//Hides cursor when opening the window
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//Sets the path for the respective object files and calls the ObjectLoad() function
	Ship.path = "3D/UFO.obj";
	Ship.objectLoad();

	AmongUs.path = "3D/Among_Us.obj";
	AmongUs.objectLoad();

	Asteroid.path = "3D/Asteroid.obj";
	Asteroid.objectLoad();

	Moon.path = "3D/moon.obj";
	Moon.objectLoad();

	Moon2.path = "3D/moon.obj";
	Moon2.objectLoad();

	Station.path = "3D/Station.obj";
	Station.objectLoad();

	Planet.path = "3D/Earth.obj";
	Planet.objectLoad();

	//Vertically flips the image to prevent an upside-down image
	stbi_set_flip_vertically_on_load(true);

	//Initializes the texture bytes and normal bytes for the main ship; assigns the retrieved relative path to pass onto the texture functions
	unsigned char* ship_tex_bytes = stbi_load("3D/ufo_diffuse.png", &Ship.img_width, &Ship.img_height, &Ship.colorChannel, 0);
	unsigned char* ship_tex_bytes2 = stbi_load("3D/ufo_diffuse_glow.png", &Ship.img_width2, &Ship.img_height2, &Ship.colorChannel2, 0);
	unsigned char* ship_norm_bytes = stbi_load("3D/ufo_Normal.png", &Ship.img_width3, &Ship.img_height3, &Ship.colorChannel3, 0);

	Ship.picturepath = ship_tex_bytes;
	Ship.picturepath2 = ship_tex_bytes2;
	Ship.normalpath = ship_norm_bytes;

	Ship.genTexWA();
	Ship.genTex2WA();
	Ship.genNorm();

	//Initializes the texture bytes and normal bytes for the Among Us Debris; assigns the retrieved relative path to pass onto the texture functions
	unsigned char* among_tex_bytes = stbi_load("3D/Among_Diffuse.jpg", &AmongUs.img_width, &AmongUs.img_height, &AmongUs.colorChannel, 0);
	unsigned char* among_tex_bytes2 = stbi_load("3D/Among_Reflect.jpg", &AmongUs.img_width2, &AmongUs.img_height2, &AmongUs.colorChannel2, 0);
	unsigned char* among_norm_bytes = stbi_load("3D/Among_Normal.jpg", &AmongUs.img_width3, &AmongUs.img_height3, &AmongUs.colorChannel3, 0);

	AmongUs.picturepath = among_tex_bytes;
	AmongUs.picturepath2 = among_tex_bytes2;
	AmongUs.normalpath = among_norm_bytes;

	AmongUs.genTexNA();
	AmongUs.genTex2NA();
	AmongUs.genNorm();

	//Initializes the texture bytes and normal bytes for the Asteroid Debris; assigns the retrieved relative path to pass onto the texture functions
	unsigned char* asteroid_tex_bytes = stbi_load("3D/Asteroid_Diffuse.jpg", &Asteroid.img_width, &Asteroid.img_height, &Asteroid.colorChannel, 0);
	unsigned char* asteroid_norm_bytes = stbi_load("3D/Asteroid_Normal.jpg", &Asteroid.img_width2, &Asteroid.img_height2, &Asteroid.colorChannel2, 0);

	Asteroid.picturepath = asteroid_tex_bytes;
	Asteroid.normalpath = asteroid_norm_bytes;

	Asteroid.genTexNA();
	Asteroid.genNorm();

	//Initializes the texture bytes and normal bytes for the First Moon Debris; assigns the retrieved relative path to pass onto the texture functions
	unsigned char* moon_tex_bytes = stbi_load("3D/Moon_Diffuse.jpg", &Moon.img_width, &Moon.img_height, &Moon.colorChannel, 0);
	unsigned char* moon_norm_bytes = stbi_load("3D/Moon_Normal.jpg", &Moon.img_width2, &Moon.img_height2, &Moon.colorChannel2, 0);

	Moon.picturepath = moon_tex_bytes;
	Moon.normalpath = moon_norm_bytes;

	Moon.genTexNA();
	Moon.genNorm();

	//Initializes the texture bytes and normal bytes for the Second Moon Debris; assigns the retrieved relative path to pass onto the texture functions
	unsigned char* moon2_tex_bytes = stbi_load("3D/Moon2_Diffuse.png", &Moon2.img_width, &Moon2.img_height, &Moon2.colorChannel, 0);
	Moon2.picturepath = moon2_tex_bytes;
	Moon2.genTexWA();

	//Initializes the texture bytes and normal bytes for the Space Station Debris; assigns the retrieved relative path to pass onto the texture functions
	unsigned char* station_tex_bytes = stbi_load("3D/Station_Base.png", &Station.img_width, &Station.img_height, &Station.colorChannel, 0);
	unsigned char* station_tex_bytes2 = stbi_load("3D/Station_Foil.png", &Station.img_width2, &Station.img_height2, &Station.colorChannel2, 0);
	unsigned char* station_norm_bytes = stbi_load("3D/Station_Normal.png", &Station.img_width3, &Station.img_height3, &Station.colorChannel3, 0);

	Station.picturepath = station_tex_bytes;
	Station.picturepath2 = station_tex_bytes2;
	Station.normalpath = station_norm_bytes;

	Station.genTexNA();
	Station.genTex2NA();
	Station.genNorm();

	//Initializes the texture bytes and normal bytes for the Main Planet; assigns the retrieved relative path to pass onto the texture functions
	unsigned char* planet_tex_bytes = stbi_load("3D/Earth_Diffuse.jpg", &Planet.img_width, &Planet.img_height, &Planet.colorChannel, 0);
	unsigned char* planet_tex_bytes2 = stbi_load("3D/Earth_Clouds.jpg", &Planet.img_width2, &Planet.img_height2, &Planet.colorChannel2, 0);
	unsigned char* planet_norm_bytes = stbi_load("3D/Earth_Normal.jpg", &Planet.img_width3, &Planet.img_height3, &Planet.colorChannel3, 0);

	Planet.picturepath = planet_tex_bytes;
	Planet.picturepath2 = planet_tex_bytes2;
	Planet.normalpath = planet_norm_bytes;

	Planet.genTexNA();
	Planet.genTex2NA();
	Planet.genNorm();

	//Allows the program to determine which side to render
	glEnable(GL_DEPTH_TEST);

	//Initializes the vertex and fragment shaders that will be used for the 3D object selected
	std::fstream shipvertstr("Shaders/ship.vert");
	std::fstream shipfragstr("Shaders/ship.frag");
	Ship.vertStrStream << shipvertstr.rdbuf();
	Ship.fragStrStream << shipfragstr.rdbuf();
	Ship.initShader();
	Ship.initBuff();

	std::fstream amongvertstr("Shaders/debris.vert");
	std::fstream amongfragstr("Shaders/debris.frag");
	AmongUs.vertStrStream << amongvertstr.rdbuf();
	AmongUs.fragStrStream << amongfragstr.rdbuf();
	AmongUs.initShader();
	AmongUs.initBuff();

	std::fstream asteroidvertstr("Shaders/debris.vert");
	std::fstream asteroidfragstr("Shaders/debris.frag");
	Asteroid.vertStrStream << asteroidvertstr.rdbuf();
	Asteroid.fragStrStream << asteroidfragstr.rdbuf();
	Asteroid.initShader();
	Asteroid.initBuff();

	std::fstream moonvertstr("Shaders/debris.vert");
	std::fstream moonfragstr("Shaders/debris.frag");
	Moon.vertStrStream << moonvertstr.rdbuf();
	Moon.fragStrStream << moonfragstr.rdbuf();
	Moon.initShader();
	Moon.initBuff();

	std::fstream moon2vertstr("Shaders/debris.vert");
	std::fstream moon2fragstr("Shaders/debris.frag");
	Moon2.vertStrStream << moon2vertstr.rdbuf();
	Moon2.fragStrStream << moon2fragstr.rdbuf();
	Moon2.initShader();
	Moon2.initBuff();

	std::fstream statvertstr("Shaders/debris.vert");
	std::fstream statfragstr("Shaders/debris.frag");
	Station.vertStrStream << statvertstr.rdbuf();
	Station.fragStrStream << statfragstr.rdbuf();
	Station.initShader();
	Station.initBuff();

	std::fstream planetvertstr("Shaders/planet.vert");
	std::fstream planetfragstr("Shaders/planet.frag");
	Planet.vertStrStream << planetvertstr.rdbuf();
	Planet.fragStrStream << planetfragstr.rdbuf();
	Planet.initShader();
	Planet.initBuff();

	std::fstream skyboxvertstr("Shaders/skybox.vert");
	std::fstream skyboxfragstr("Shaders/skybox.frag");
	Background.vertStrStream << skyboxvertstr.rdbuf();
	Background.fragStrStream << skyboxfragstr.rdbuf();
	Background.initShader();		

	//Setup for skybox VAO, VBO, and EBO buffers
	unsigned int sVAO, sVBO, sEBO;
	glGenVertexArrays(1, &sVAO);
	glGenBuffers(1, &sVBO);
	glGenBuffers(1, &sEBO);

	glBindVertexArray(sVAO);
	glBindBuffer(GL_ARRAY_BUFFER, sVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GL_INT) * 36, &skyboxIndices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	unsigned int skyboxTex;
	glGenTextures(1, &skyboxTex);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);

	//Prevents the skybox images from pixelating by blurring it
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	//Clamps the images to the edges of the cube
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//Iterates through the six faces of the cube
	for (unsigned int i = 0; i < 6; i++)
	{
		int w, h, skyCChanel;
		stbi_set_flip_vertically_on_load(false);
		unsigned char* data = stbi_load(skyboxFaces[i].c_str(), &w, &h, &skyCChanel, 0);

		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
	}

	//Manipulates the position, size, and rotation of the object
	Ship.position = glm::vec3(0.0f, -3.0f, 5.0f);
	Ship.scaling = glm::vec3(0.1f, 0.1f, 0.1f);
	Ship.rotation = glm::vec3(0.0f, 0.0f, 0.0f);

	AmongUs.position = glm::vec3(20.0f, -15.0f, 40.0f);
	AmongUs.scaling = glm::vec3(0.003f, 0.003f, 0.003f);
	AmongUs.rotation = glm::vec3(0.0f, 90.0f, 0.0f);

	Asteroid.position = glm::vec3(60.0f, 40.0f, 120.0f);
	Asteroid.scaling = glm::vec3(0.2f, 0.2f, 0.2f);
	Asteroid.rotation = glm::vec3(0.0f, 0.0f, 0.0f);

	Moon.position = glm::vec3(-40.0f, 20.0f, 40.0f);
	Moon.scaling = glm::vec3(3.0f, 3.0f, 3.0f);
	Moon.rotation = glm::vec3(0.0f, 90.0f, 0.0f);

	Moon2.position = glm::vec3(-35.0f, 15.0f, 40.0f);
	Moon2.scaling = glm::vec3(1.0f, 1.0f, 1.0f);
	Moon2.rotation = glm::vec3(0.0f, 90.0f, 0.0f);

	Station.position = glm::vec3(20.0f, -10.0f, 45.0f);
	Station.scaling = glm::vec3(0.05f, 0.05f, 0.05f);
	Station.rotation = glm::vec3(0.0f, 50.0f, 0.0f);

	Planet.position = glm::vec3(0.0f, 0.0f, 100.0f);
	Planet.scaling = glm::vec3(0.2f, 0.2f, 0.2f);
	Planet.rotation = glm::vec3(0.0f, 0.0f, 0.0f);

	//Initialization of the default projection matrix for the camera
	PlayerPOV.projection = glm::perspective(glm::radians(60.0f), height / width, 0.1f, 500.0f);

	//Variables used for the rotation of the object (Time & Rotation value)
	float lastTime = glfwGetTime();
	float rotation = 0.0f;

	//Loops until the user closes the window
	while (!glfwWindowShouldClose(window))
	{
		//Rendering of the object
		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);

		glDepthMask(GL_FALSE);
		glDepthFunc(GL_LEQUAL);

		//Calls the function that allows for a singular input of keyboard inputs
		glfwSetKeyCallback(window, input_callback);

		//Dictates what the camera's position and where it is loooking
		PlayerPOV.view = glm::lookAt(PlayerPOV.cameraPos, PlayerPOV.cameraPos + PlayerPOV.F, PlayerPOV.WorldUp);

		//Frame time for camera movement adjustment
		currentFrame = glfwGetTime();
		lastFrame = 0.0f;
		timeframe = currentFrame - lastFrame;

		lastFrame = currentFrame;
		cameraSpeed = 0.002f * timeframe;

		//Rendering of the Skybox
		glUseProgram(Background.shaderProgram);
		glm::mat4 sky_view = glm::mat4(1.0f);
		sky_view = glm::mat4(glm::mat3(PlayerPOV.view));

		unsigned int skyboxViewLoc = glGetUniformLocation(Background.shaderProgram, "view");
		glUniformMatrix4fv(skyboxViewLoc, 1, GL_FALSE, glm::value_ptr(sky_view));

		unsigned int skyboxProjLoc = glGetUniformLocation(Background.shaderProgram, "projection");
		glUniformMatrix4fv(skyboxProjLoc, 1, GL_FALSE, glm::value_ptr(PlayerPOV.projection));

		//Uses perspective projection for the camera once the camera state is set to 1
		if (camstate == 1)
		{
			if (PlayerPOV.cameraPos == glm::vec3(0.0f, 65.0f, 100.0f))
				PlayerPOV.cameraPos = glm::vec3(0.0f, 3.0f, -15.0f) + Ship.position;

			//Changes the view matrix to perspective view
			PlayerPOV.view = glm::lookAt(PlayerPOV.cameraPos, PlayerPOV.cameraPos + PlayerPOV.F, PlayerPOV.WorldUp);
			PlayerPOV.projection = glm::perspective(glm::radians(60.0f), height / width, 0.1f, 500.0f);

			//Enables WASDQE for forward/backward movement, strafing, and ascending/descending movement
			if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			{
				PlayerPOV.cameraPos.z = PlayerPOV.cameraPos.z + cameraSpeed;
				Ship.position.z += cameraSpeed;
			}

			if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			{
				PlayerPOV.cameraPos.z = PlayerPOV.cameraPos.z - cameraSpeed;
				Ship.position.z -= cameraSpeed;
			}

			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			{
				PlayerPOV.cameraPos.x = PlayerPOV.cameraPos.x - cameraSpeed;
				Ship.position.x -= cameraSpeed;
			}

			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			{
				PlayerPOV.cameraPos.x = PlayerPOV.cameraPos.x + cameraSpeed;
				Ship.position.x += cameraSpeed;
			}

			if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
			{
				PlayerPOV.cameraPos.y = PlayerPOV.cameraPos.y + cameraSpeed;
				Ship.position.y += cameraSpeed;
			}

			if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
			{
				PlayerPOV.cameraPos.y = PlayerPOV.cameraPos.y - cameraSpeed;
				Ship.position.y -= cameraSpeed;
			}

		}

		//Uses orthographic projection for the camera once the camera state is set to 2
		if (camstate == 2)
		{
			//Sets the camera position accordingly
			PlayerPOV.cameraPos = glm::vec3(0.0f, 65.0f, 100.0f);
			PlayerPOV.F = glm::normalize(PlayerPOV.centerPos - PlayerPOV.cameraPos);

			//Changes the view matrix and uses orthographic view
			PlayerPOV.view = glm::lookAt(PlayerPOV.cameraPos, PlayerPOV.centerPos, PlayerPOV.WorldUp);
			PlayerPOV.projection = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, -1.0f, 200.0f);

			//Enables WASD to pan in orthographic view
			if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
				PlayerPOV.centerPos.y += 0.1f;

			if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
				PlayerPOV.centerPos.y -= 0.1f;

			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
				PlayerPOV.centerPos.x += 0.1f;

			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
				PlayerPOV.centerPos.x -= 0.1f;
		}

		//Sets the light position to the planet by default
		if (lightstate == 1)
			MainLight.lightPos = Planet.position;

		//Sets the light position to the ship
		if (lightstate == 2)
			MainLight.lightPos = Ship.position;

		glBindVertexArray(sVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glDepthMask(GL_TRUE);
		glDepthFunc(GL_LESS);

		//Handles the rotation for the main planet
		float currTime = glfwGetTime();
		float deltTime = currTime - lastTime;
		rotation = rotation + (deltTime * 5.0f);

		Ship.rotation.y = rotation * 4.0f;

		Planet.rotation.y = rotation * 0.1f;
		Planet.rotation.z = rotation * 0.1f;

		AmongUs.rotation.y = rotation * 2.0f;
		AmongUs.rotation.z = rotation;

		Asteroid.rotation.x = rotation * 2.0f;
		Asteroid.rotation.y = rotation * 2.0f;

		//Rendering of the Ship
		Ship.objectTransform();
		MainLight.lightTransform(Ship.shaderProgram);
		PlayerPOV.cameraTransform(Ship.shaderProgram);
		glDrawArrays(GL_TRIANGLES, 0, Ship.fullVertexData.size() / 8);

		//Rendering of the AmongUs
		AmongUs.objectTransform();
		MainLight.lightTransform(AmongUs.shaderProgram);
		PlayerPOV.cameraTransform(AmongUs.shaderProgram);
		glDrawArrays(GL_TRIANGLES, 0, AmongUs.fullVertexData.size() / 8);

		//Rendering of the Asteroid
		Asteroid.objectTransform();
		MainLight.lightTransform(Asteroid.shaderProgram);
		PlayerPOV.cameraTransform(Asteroid.shaderProgram);
		glDrawArrays(GL_TRIANGLES, 0, Asteroid.fullVertexData.size() / 8);

		//Rendering of the Moon
		Moon.objectTransform();
		MainLight.lightTransform(Moon.shaderProgram);
		PlayerPOV.cameraTransform(Moon.shaderProgram);
		glDrawArrays(GL_TRIANGLES, 0, Moon.fullVertexData.size() / 8);

		//Rendering of the Secondary Moon
		Moon2.objectTransform();
		MainLight.lightTransform(Moon2.shaderProgram);
		PlayerPOV.cameraTransform(Moon2.shaderProgram);
		glDrawArrays(GL_TRIANGLES, 0, Moon2.fullVertexData.size() / 8);

		//Rendering of the Station
		Station.objectTransform();
		MainLight.lightTransform(Station.shaderProgram);
		PlayerPOV.cameraTransform(Station.shaderProgram);
		glDrawArrays(GL_TRIANGLES, 0, Station.fullVertexData.size() / 8);

		//Rendering of the Ship
		Planet.objectTransform();
		PlayerPOV.cameraTransform(Planet.shaderProgram);
		glDrawArrays(GL_TRIANGLES, 0, Planet.fullVertexData.size() / 8);

		//Swaps the front and back buffers
		glfwSwapBuffers(window);

		//Poll for and process events
		glfwPollEvents();

		//Resets the time
		lastTime = currTime;
	}
	//Resets values after use
	Ship.resetBuff();
	AmongUs.resetBuff();
	Asteroid.resetBuff();
	Moon.resetBuff();
	Moon2.resetBuff();
	Station.resetBuff();
	Planet.resetBuff();

	glDeleteVertexArrays(1, &sVAO);
	glDeleteBuffers(1, &sVBO);
	glDeleteBuffers(1, &sEBO);

	//Ends the program
	glfwTerminate();
	return 0;
}
/***********************************************************************************************************************

This is to certify that this project is my own work, based on my personal efforts in studying and applying the concepts
learned. I have constructed the functions and their respective algorithms and corresponding code by myself. The program
was run, tested, and debugged by my own efforts.  I further certify that I have not copied in part or whole or otherwise
plagiarized the work of other students and/or persons for the coding process. 3D Objects used fall under free use, and
is credited. Please refer to the document

										  Marc Lawrence C. Galura, DLSU ID# 12023817

***********************************************************************************************************************/
