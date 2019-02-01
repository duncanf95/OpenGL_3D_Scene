// Framebuffer Multipass


#pragma comment(linker, "/NODEFAULTLIB:MSVCRT")

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
using namespace std;


#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GLM/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <GLM/gtx/transform.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

void errorCallbackGLFW(int error, const char* description);
void hintsGLFW();
void endProgram();
void render(GLfloat currentTime);
void update(GLfloat currentTime);
void setupRender();
void startup();
void onResizeCallback(GLFWwindow* window, int w, int h);
void onKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void onMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void onMouseMoveCallback(GLFWwindow* window, double x, double y);
void onMouseWheelCallback(GLFWwindow* window, double xoffset, double yoffset);
void debugGL();
void attachShaderFiles(string vsFile, string fsFile, struct modelData *obj);
static void APIENTRY openGLDebugCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const GLvoid* userParam);
string readShader(string name);
void checkErrorShader(GLuint shader);
void readObj(string name, struct modelData *obj);
void readTexture(string name, GLuint texture);
void bindVertexs(modelData *obj);
void attachLights(modelData *obj);
void attachTextures(modelData *obj);
void drawModels(modelData *obj, glm::mat4 viewMatrix);
void tardisFade(modelData *obj, glm::mat4 viewMatrix, GLfloat currentTime);
void tardisReMat(modelData *obj);
void tardisDeMat(modelData *obj, GLfloat currentTime);
void tardisCrashAnim();
float *calc_ark(modelData *obj, float retVal[]);
void dalekArk1(GLfloat currentTime);
void dalekArk2(GLfloat currentTime);
void placeDoors();
void SetLights();
void UpdateLights();
void SpawnWithDoors(modelData *obj, glm::mat4 viewMatrix);
void attachSpots(modelData *obj);
void dalekPatrol1(GLfloat currentTime);
void dalekPatrol2(GLfloat currentTime);
void openDoors(GLfloat currentTime);
void closeDoors(GLfloat currentTime);
void lockCamera();
GLfloat prevTime = 0.0f;
GLfloat updateTime = 0.0f;



// VARIABLES
GLFWwindow*		window;
int				windowWidth = 1520;
int				windowHeight = 960;
bool			running = true;
glm::mat4		proj_matrix;
glm::vec3		modelAngle = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3		cameraPosition = glm::vec3(3.7, 0.6f, -2.9f);
glm::vec3		cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3		cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float           aspect = (float)windowWidth / (float)windowHeight;
float			fovy = 45.0f;
bool			keyStatus[1024];
GLfloat			deltaTime = 0.0f;
GLfloat			lastTime = 0.0f;
bool			tardisDisp = false;
float			tardisDec;
const int		numDaleks = 12;
glm::vec3		arkStartPos[numDaleks];
bool			doorsMoved = true;
bool			open = false;
bool			close = false;
bool			lockCameraBool = false;
bool			cameraMode = false;
// FPS camera variables
GLfloat			yaw = -90.0f;	// init pointing to inside
GLfloat			pitch = 0.0f;	// start centered
GLfloat			lastX = (GLfloat)windowWidth / 2.0f;	// start middle screen
GLfloat			lastY = (GLfloat)windowHeight / 2.0f;	// start middle screen



bool			firstMouse = true;
struct modelLightData {
	
	
};
// OBJ Variables

struct AnimaStages {
	bool stage1 = false;
	bool stage2 = false;
	bool stage3 = false;
	bool stage4 = false;
	bool stage5 = false;
	bool stage6 = false;
	bool stage7 = false;
	bool stage8 = false;

	bool deMat = true;
	bool reMat = false;
}tStages, aStages, dalekArkStages, dalekArkStages2, patrolStages1, patrolStages2;

struct modelData {
	std::vector < glm::vec3 > out_vertices;
	std::vector < glm::vec2 > out_uvs;
	std::vector < glm::vec3 > out_normals;

	GLuint*		texture;
	GLuint      program;
	GLuint      vao;
	GLuint      buffer[2];
	GLint       mv_location;
	GLint       proj_location;
	GLint		tex_location;
	int			numModels = 1;

	glm::vec3	*positions;
	glm::vec3	*rotations;
	glm::vec3	disp = glm::vec3(0.0f, 0.0f, 0.0f);

	// extra variables for this example
	GLuint		matColor_location;
	GLuint		lightColor_location;

	modelLightData lightData;
	
	float			transparency = 1.0;
	bool			transparencyReduce = false;
	bool			transparencyIncrease = false;

	GLuint			vs;
	GLuint			fs;
	string			vs_text;
	string			fs_text;

	glm::vec3		scale;

	string vsFile = "vs_model.glsl";
	string fsFile = "fs_model.glsl";

	const char* vs_source;
	const char* fs_source;

	float arkAngle = 6.28319f;
	float arkRad;

	string modelFile;
} tardisModel, 
groundModel, 
buildingModel, 
dalekModel, 
tardisDoorLeft, 
tardisDoorRight, 
lampModels,
tardisNoDoors;

bool movingDalek = false;
bool tardisLanded = false;

modelData* models[] = { &tardisModel,  &groundModel, &buildingModel, &dalekModel, &lampModels};
modelData* noDoors[] = { &tardisDoorLeft, &tardisDoorRight, &tardisNoDoors };
int objNum = sizeof(models) / sizeof(*models);

const int LightConstant = 13;
const int SpotConstant = 12;
struct lightData {
	glm::vec3		lightDisp[LightConstant];	
	float			shininess[LightConstant];
	glm::vec3		id[LightConstant];
	glm::vec3		ia[LightConstant];
	glm::vec3		is[LightConstant];
	GLfloat			ka[LightConstant];
	GLfloat			kd[LightConstant];
	GLfloat			ks[LightConstant];
	int				index = 0;
	glm::vec3		lightSpotDirection = glm::vec3(0.1,0.0,0.0);
	float			lightSpotCutOff = 1.0f;
	bool			movingLight = true;


}lights, spots;


struct display {
	GLuint			vao;
	GLuint			buffer[2];
	std::vector < glm::vec2 > vertices;
	std::vector < glm::vec2 > uvs;
	GLuint			program;
}display;

// Light



GLuint			framebuffer;
GLuint			framebufferTexture;
GLuint			depthbuffer;
bool movingLight = false;
bool crashAnimation = false;

void errorCallbackGLFW(int error, const char* description) {
	std::cout << "Error GLFW: " << description << "\n";
}

void hintsGLFW() {
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);			// Create context in debug mode - for debug message callback
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
}

void endProgram() {
	glfwMakeContextCurrent(window);		// destroys window handler
	glfwTerminate();	// destroys all windows and releases resources.

						// tidy heap memory

	for (int i = 0; i < objNum; i++) 
	{
		delete[] models[i]->texture;
		std::cout<< i<< endl;
		if (i != 4) {
			delete[] models[i]->positions;
		}
		delete[] models[i]->rotations;
	}
}

void setupRender() {
	glfwSwapInterval(1);	// Ony render when synced (V SYNC)

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 0);
	glfwWindowHint(GLFW_STEREO, GL_FALSE);
}

void onResizeCallback(GLFWwindow* window, int w, int h) {
	windowWidth = w;
	windowHeight = h;

	aspect = (float)w / (float)h;
	proj_matrix = glm::perspective(glm::radians(fovy), aspect, 0.1f, 1000.0f);
}

void onKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) keyStatus[key] = true;
	else if (action == GLFW_RELEASE) keyStatus[key] = false;

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void onMouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {

}

void onMouseMoveCallback(GLFWwindow* window, double x, double y) {
	int mouseX = static_cast<int>(x);
	int mouseY = static_cast<int>(y);

	if (firstMouse) {
		lastX = (GLfloat)mouseX; lastY = (GLfloat)mouseY; firstMouse = false;
	}
	if (!lockCameraBool) {

		GLfloat xoffset = mouseX - lastX;
		GLfloat yoffset = lastY - mouseY; // Reversed
		lastX = (GLfloat)mouseX; lastY = (GLfloat)mouseY;

		GLfloat sensitivity = 0.05f;
		xoffset *= sensitivity; yoffset *= sensitivity;

		yaw += xoffset; pitch += yoffset;

		// check for pitch out of bounds otherwise screen gets flipped
		if (pitch > 89.0f) pitch = 89.0f;
		if (pitch < -89.0f) pitch = -89.0f;

		glm::vec3 front;
		front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front.y = sin(glm::radians(pitch));
		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

		cameraFront = glm::normalize(front);
	}

}

static void onMouseWheelCallback(GLFWwindow* window, double xoffset, double yoffset) {
	int yoffsetInt = static_cast<int>(yoffset);

	fovy += yoffsetInt;
	if (fovy >= 1.0f && fovy <= 45.0f) fovy -= (GLfloat)yoffset;
	if (fovy <= 1.0f) fovy = 1.0f;
	if (fovy >= 45.0f) fovy = 45.0f;
	proj_matrix = glm::perspective(glm::radians(fovy), aspect, 0.1f, 1000.0f);
}

void debugGL() {
	//Output some debugging information
	std::cout << "VENDOR: " << (char *)glGetString(GL_VENDOR) << endl;
	std::cout << "VERSION: " << (char *)glGetString(GL_VERSION) << endl;
	std::cout << "RENDERER: " << (char *)glGetString(GL_RENDERER) << endl;

	// Enable Opengl Debug
	//glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback((GLDEBUGPROC)openGLDebugCallback, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, true);
}

static void APIENTRY openGLDebugCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const GLvoid* userParam) {

	std::cout << "---------------------opengl-callback------------" << endl;
	std::cout << "Message: " << message << endl;
	std::cout << "type: ";
	switch (type) {
	case GL_DEBUG_TYPE_ERROR:
		std::cout << "ERROR";
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		std::cout << "DEPRECATED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		std::cout << "UNDEFINED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		std::cout << "PORTABILITY";
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		std::cout << "PERFORMANCE";
		break;
	case GL_DEBUG_TYPE_OTHER:
		std::cout << "OTHER";
		break;
	}
	std::cout << " --- ";

	std::cout << "id: " << id << " --- ";
	std::cout << "severity: ";
	switch (severity) {
	case GL_DEBUG_SEVERITY_LOW:
		std::cout << "LOW";
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		std::cout << "MEDIUM";
		break;
	case GL_DEBUG_SEVERITY_HIGH:
		std::cout << "HIGH";
		break;
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		std::cout << "NOTIFICATION";
	}
	std::cout << endl;
	std::cout << "-----------------------------------------" << endl;
}

string readShader(string name) {
	string vs_text;
	std::ifstream vs_file(name);

	string vs_line;
	if (vs_file.is_open()) {

		while (getline(vs_file, vs_line)) {
			vs_text += vs_line;
			vs_text += '\n';
		}
		vs_file.close();
	}
	return vs_text;
}

void  checkErrorShader(GLuint shader) {
	// Get log lenght
	GLint maxLength;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

	// Init a string for it
	std::vector<GLchar> errorLog(maxLength);

	if (maxLength > 1) {
		// Get the log file
		glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);

		std::cout << "--------------Shader compilation error-------------\n";
		std::cout << errorLog.data();
	}

}

void readObj(string name, struct modelData *obj) {
	std::cout << "Loading model " << name << "\n";

	std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
	std::vector< string > materials, textures;
	std::vector< glm::vec3 > obj_vertices;
	std::vector< glm::vec2 > obj_uvs;
	std::vector< glm::vec3 > obj_normals;

	std::ifstream dataFile(name);

	string rawData;		
	int countLines = 0;
	if (dataFile.is_open()) {
		string dataLineRaw;
		while (getline(dataFile, dataLineRaw)) {
			rawData += dataLineRaw;
			rawData += "\n";
			countLines++;
		}
		dataFile.close();
	}

	std::cout << "Finished reading model file " << name << "\n";

	istringstream rawDataStream(rawData);
	string dataLine;
	int linesDone = 0;
	while (std::getline(rawDataStream, dataLine)) {
		if (dataLine.find("v ") != string::npos) {	
			glm::vec3 vertex;

			int foundStart = dataLine.find(" ");  int foundEnd = dataLine.find(" ", foundStart + 1);
			vertex.x = stof(dataLine.substr(foundStart, foundEnd - foundStart));

			foundStart = foundEnd; foundEnd = dataLine.find(" ", foundStart + 1);
			vertex.y = stof(dataLine.substr(foundStart, foundEnd - foundStart));

			foundStart = foundEnd; foundEnd = dataLine.find(" ", foundStart + 1);
			vertex.z = stof(dataLine.substr(foundStart, foundEnd - foundStart));

			obj_vertices.push_back(vertex);
		}
		else if (dataLine.find("vt ") != string::npos) {	
			glm::vec2 uv;

			int foundStart = dataLine.find(" ");  int foundEnd = dataLine.find(" ", foundStart + 1);
			uv.x = stof(dataLine.substr(foundStart, foundEnd - foundStart));

			foundStart = foundEnd; foundEnd = dataLine.find(" ", foundStart + 1);
			uv.y = stof(dataLine.substr(foundStart, foundEnd - foundStart));

			obj_uvs.push_back(uv);
		}
		else if (dataLine.find("vn ") != string::npos) { 
			glm::vec3 normal;

			int foundStart = dataLine.find(" ");  int foundEnd = dataLine.find(" ", foundStart + 1);
			normal.x = stof(dataLine.substr(foundStart, foundEnd - foundStart));

			foundStart = foundEnd; foundEnd = dataLine.find(" ", foundStart + 1);
			normal.y = stof(dataLine.substr(foundStart, foundEnd - foundStart));

			foundStart = foundEnd; foundEnd = dataLine.find(" ", foundStart + 1);
			normal.z = stof(dataLine.substr(foundStart, foundEnd - foundStart));

			obj_normals.push_back(normal);
		}
		else if (dataLine.find("f ") != string::npos) { 
			string parts[3];

			int foundStart = dataLine.find(" ");  int foundEnd = dataLine.find(" ", foundStart + 1);
			parts[0] = dataLine.substr(foundStart + 1, foundEnd - foundStart - 1);

			foundStart = foundEnd; foundEnd = dataLine.find(" ", foundStart + 1);
			parts[1] = dataLine.substr(foundStart + 1, foundEnd - foundStart - 1);

			foundStart = foundEnd; foundEnd = dataLine.find(" ", foundStart + 1);
			parts[2] = dataLine.substr(foundStart + 1, foundEnd - foundStart - 1);

			for (int i = 0; i < 3; i++) {		

				vertexIndices.push_back(stoul(parts[i].substr(0, parts[i].find("/"))));

				int firstSlash = parts[i].find("/"); int secondSlash = parts[i].find("/", firstSlash + 1);

				if ((firstSlash + 1) != (secondSlash)) {	
					uvIndices.push_back(stoul(parts[i].substr(firstSlash + 1, secondSlash - firstSlash + 1)));
				}


				normalIndices.push_back(stoul(parts[i].substr(secondSlash + 1)));

			}
		}
		else if (dataLine.find("mtllib ") != string::npos) { 
			materials.push_back(dataLine.substr(dataLine.find(" ") + 1));
		}

		linesDone++;

		if (linesDone % 50000 == 0) {
			std::cout << "Parsed " << linesDone << " of " << countLines << " from model...\n";
		}

	}



	for (unsigned int i = 0; i < vertexIndices.size(); i++) {
		(*obj).out_vertices.push_back(obj_vertices[vertexIndices[i] - 1]);
		(*obj).out_normals.push_back(obj_normals[normalIndices[i] - 1]);
		(*obj).out_uvs.push_back(obj_uvs[uvIndices[i] - 1]);
	}



	for (unsigned int i = 0; i < materials.size(); i++) {

		std::ifstream dataFileMat(materials[i]);

		string rawDataMat;		
		int countLinesMat = 0;
		if (dataFileMat.is_open()) {
			string dataLineRawMat;
			while (getline(dataFileMat, dataLineRawMat)) {
				rawDataMat += dataLineRawMat;
				rawDataMat += "\n";
			}
			dataFileMat.close();
		}

		istringstream rawDataStreamMat(rawDataMat);
		string dataLineMat;
		while (std::getline(rawDataStreamMat, dataLineMat)) {
			if (dataLineMat.find("map_Kd ") != string::npos) {	
				textures.push_back(dataLineMat.substr(dataLineMat.find(" ") + 1));
			}
		}
	}

	(*obj).texture = new GLuint[textures.size()];		
	glGenTextures(textures.size(), (*obj).texture);

	for (unsigned int i = 0; i < textures.size(); i++) {
		readTexture(textures[i], (*obj).texture[i]);
	}

	std::cout << "done";
}

void readTexture(string name, GLuint textureName) {


	// Flip images as OpenGL expects 0.0 coordinates on the y-axis to be at the bottom of the image.
	stbi_set_flip_vertically_on_load(true);

	// Load image Information.
	int iWidth, iHeight, iChannels;
	unsigned char *iData = stbi_load(name.c_str(), &iWidth, &iHeight, &iChannels, 0);

	// Load and create a texture 
	glBindTexture(GL_TEXTURE_2D, textureName); // All upcoming operations now have effect on this texture object

	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, iWidth, iHeight);

	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, iWidth, iHeight, GL_RGBA, GL_UNSIGNED_BYTE, iData);


	// This only works for 2D Textures...
	// Set the texture wrapping parameters (next lecture)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Set texture filtering parameters (next lecture)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Generate mipmaps (next lecture)
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);// Unbind texture when done, so we won't accidentily mess up our texture.


}

void attachShaderFiles(string vsFile, string fsFile, struct modelData *obj) {
	(*obj).program = glCreateProgram();
	(*obj).vs_text = readShader(vsFile); (*obj).vs_source = (*obj).vs_text.c_str();
	(*obj).vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource((*obj).vs, 1, &(*obj).vs_source, NULL);
	glCompileShader((*obj).vs);
	checkErrorShader((*obj).vs);
	glAttachShader((*obj).program, (*obj).vs);

	(*obj).fs_text = readShader(fsFile); (*obj).fs_source = (*obj).fs_text.c_str();
	(*obj).fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource((*obj).fs, 1, &(*obj).fs_source, NULL);
	glCompileShader((*obj).fs);
	checkErrorShader((*obj).fs);
	glAttachShader((*obj).program, (*obj).fs);
}

void bindVertexs(modelData *obj) {
	
	glLinkProgram((*obj).program);
	glUseProgram((*obj).program);

	glGenVertexArrays(1, &((*obj).vao));			// Create Vertex Array Object
	glBindVertexArray((*obj).vao);				// Bind VertexArrayObject
	glGenBuffers(3, (*obj).buffer);			// Create new buffers (Vertices, Texture Coordinates, Normals)
	glBindBuffer(GL_ARRAY_BUFFER, (*obj).buffer[0]);	// Bind Buffer Vertex
	glBufferStorage(GL_ARRAY_BUFFER, (*obj).out_vertices.size() * sizeof(glm::vec3), &((*obj).out_vertices[0]), GL_DYNAMIC_STORAGE_BIT);
	glBindBuffer(GL_ARRAY_BUFFER, (*obj).buffer[1]);	// Bind Buffer UV
	glBufferStorage(GL_ARRAY_BUFFER, (*obj).out_uvs.size() * sizeof(glm::vec2), &((*obj).out_uvs[0]), GL_DYNAMIC_STORAGE_BIT);
	glBindBuffer(GL_ARRAY_BUFFER, (*obj).buffer[2]);	// Bind Buffer Normals
	glBufferStorage(GL_ARRAY_BUFFER, (*obj).out_normals.size() * sizeof(glm::vec3), &((*obj).out_normals[0]), GL_DYNAMIC_STORAGE_BIT);
	glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, 0);		// Vertices									
	glBindVertexBuffer(0, (*obj).buffer[0], 0, sizeof(GLfloat) * 3);
	glVertexAttribBinding(0, 0);
	glEnableVertexAttribArray(0);
	glVertexAttribFormat(1, 2, GL_FLOAT, GL_FALSE, 0);			// UV									
	glBindVertexBuffer(1, (*obj).buffer[1], 0, sizeof(GLfloat) * 2);
	glVertexAttribBinding(1, 1);
	glEnableVertexAttribArray(1);
	glVertexAttribFormat(2, 3, GL_FLOAT, GL_FALSE, 0);			// Normals									
	glBindVertexBuffer(2, (*obj).buffer[2], 0, sizeof(GLfloat) * 3);
	glVertexAttribBinding(2, 2);
	glEnableVertexAttribArray(2);

	(*obj).mv_location = glGetUniformLocation((*obj).program, "mv_matrix");
	(*obj).proj_location = glGetUniformLocation((*obj).program, "proj_matrix");
	(*obj).tex_location = glGetUniformLocation((*obj).program, "tex");
	(*obj).lightColor_location = glGetUniformLocation((*obj).program, "ia");
	(*obj).lightColor_location = glGetUniformLocation((*obj).program, "ka");

}

void createFramebuffer() {
	

	//-------------------------------------------------------------------------------------------------
	// NEW: FRAMEBUFFER
	// ----------Start Framebuffer Object---------------
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	// Create a texture for the framebuffer
	glGenTextures(1, &framebufferTexture);

	// Bind this texture so we can modify it.
	glBindTexture(GL_TEXTURE_2D, framebufferTexture);

	//  Start the texture and give it a size but no data- of course if you resize you need to change your texture.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, windowWidth, windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

	// filtering needed 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// Depth buffer texture	- Need to attach depth too otherwise depth testing will not be performed.
	glGenRenderbuffers(1, &depthbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, windowWidth, windowHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthbuffer);


	// Create a quad to display our framebuffer
	display.vertices.push_back(glm::vec2(-1.0f, 1.0f));
	display.vertices.push_back(glm::vec2(-1.0f, -1.0f));
	display.vertices.push_back(glm::vec2(1.0f, -1.0f));
	display.vertices.push_back(glm::vec2(-1.0f, 1.0f));
	display.vertices.push_back(glm::vec2(1.0f, -1.0f));
	display.vertices.push_back(glm::vec2(1.0f, 1.0f));

	display.uvs.push_back(glm::vec2(0.0f, 1.0f));
	display.uvs.push_back(glm::vec2(0.0f, 0.0f));
	display.uvs.push_back(glm::vec2(1.0f, 0.0f));
	display.uvs.push_back(glm::vec2(0.0f, 1.0f));
	display.uvs.push_back(glm::vec2(1.0f, 0.0f));
	display.uvs.push_back(glm::vec2(1.0f, 1.0f));

	glGenBuffers(2, display.buffer);		// Create a new buffer
	glBindBuffer(GL_ARRAY_BUFFER, display.buffer[0]);	// Bind Buffer
	glBufferStorage(GL_ARRAY_BUFFER, display.vertices.size() * sizeof(glm::vec2), &display.vertices[0], GL_DYNAMIC_STORAGE_BIT); // Store the vertices

	glBindBuffer(GL_ARRAY_BUFFER, display.buffer[1]);	// Bind Buffer												// Store the texture coordinates
	glBufferStorage(GL_ARRAY_BUFFER, display.uvs.size() * sizeof(glm::vec2), &display.uvs[0], GL_DYNAMIC_STORAGE_BIT);

	glGenVertexArrays(1, &display.vao);		// Create Vertex Array Object
	glBindVertexArray(display.vao);		// Bind VertexArrayObject

	glVertexAttribFormat(0, 2, GL_FLOAT, GL_FALSE, 0);		// Vertices									
	glBindVertexBuffer(0, display.buffer[0], 0, sizeof(GLfloat) * 2);
	glVertexAttribBinding(0, 0);
	glEnableVertexAttribArray(0);

	glVertexAttribFormat(1, 2, GL_FLOAT, GL_FALSE, 0);			// UV									
	glBindVertexBuffer(1, display.buffer[1], 0, sizeof(GLfloat) * 2);
	glVertexAttribBinding(1, 1);
	glEnableVertexAttribArray(1);

	//load shaders for framebuffer displays
	display.program = glCreateProgram();

	string dvs_text = readShader("vs_display.glsl"); const char* dvs_source = dvs_text.c_str();
	GLuint dvs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(dvs, 1, &dvs_source, NULL);
	glCompileShader(dvs);
	checkErrorShader(dvs);
	glAttachShader(display.program, dvs);

	string dfs_text = readShader("fs_display.glsl"); const char* dfs_source = dfs_text.c_str();
	GLuint dfs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(dfs, 1, &dfs_source, NULL);
	glCompileShader(dfs);
	checkErrorShader(dfs);
	glAttachShader(display.program, dfs);

	glLinkProgram(display.program);
	glUseProgram(display.program);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void attachLights(modelData *obj) {

	glUseProgram((*obj).program);
	glBindVertexArray((*obj).vao);
	glUniformMatrix4fv((*obj).proj_location, 1, GL_FALSE, &proj_matrix[0][0]);
	for (int i = 0; i < LightConstant; i++) {
		glUniform4f(glGetUniformLocation((*obj).program, ("lights[" + to_string(i) + "].lightPosition").c_str()), lights.lightDisp[i].x, lights.lightDisp[i].y, lights.lightDisp[i].z, 1.0);
		glUniform4f(glGetUniformLocation((*obj).program, ("lights[" + to_string(i) + "].id").c_str()), lights.id[i].r, lights.id[i].g, lights.id[i].b, 1.0);
		glUniform1f(glGetUniformLocation((*obj).program, ("lights[" + to_string(i) + "].shininess").c_str()), lights.shininess[i]);
		glUniform4f(glGetUniformLocation((*obj).program, ("lights[" + to_string(i) + "].is").c_str()), lights.is[i].r, lights.is[i].g, lights.is[i].b, 1.0);
		glUniform4f(glGetUniformLocation((*obj).program, ("lights[" + to_string(i) + "].ia").c_str()), lights.ia[i].r, lights.ia[i].g, lights.ia[i].b, 1.0);
		glUniform1f(glGetUniformLocation((*obj).program, ("lights[" + to_string(i) + "].ka").c_str()), lights.ka[i]);
		glUniform1f(glGetUniformLocation((*obj).program, ("lights[" + to_string(i) + "].kd").c_str()), lights.kd[i]);
		glUniform1f(glGetUniformLocation((*obj).program, ("lights[" + to_string(i) + "].ks").c_str()), lights.ks[i]);
		
	}
	//glUniform4f(glGetUniformLocation((*obj).program, "viewPosition"), cameraPosition.x, cameraPosition.y, cameraPosition.z, 1.0);
	
	
	glUniform1f(glGetUniformLocation((*obj).program, "transparency"), (*obj).transparency);

	// Bind textures and samplers - using 0 as I know there is only one texture - need to extend.
	
}

void attachSpots(modelData *obj) {

	glUseProgram((*obj).program);
	glBindVertexArray((*obj).vao);
	glUniformMatrix4fv((*obj).proj_location, 1, GL_FALSE, &proj_matrix[0][0]);
	for (int i = 0; i < SpotConstant; i++) {
		glUniform4f(glGetUniformLocation((*obj).program, ("lights[" + to_string(i+13) + "].lightPosition").c_str()), spots.lightDisp[i].x, spots.lightDisp[i].y, spots.lightDisp[i].z, 1.0);
		glUniform4f(glGetUniformLocation((*obj).program, ("lights[" + to_string(i+13) + "].id").c_str()), spots.id[i].r, spots.id[i].g, spots.id[i].b, 1.0);
		glUniform1f(glGetUniformLocation((*obj).program, ("lights[" + to_string(i+13) + "].shininess").c_str()), spots.shininess[i]);
		glUniform4f(glGetUniformLocation((*obj).program, ("lights[" + to_string(i+13) + "].is").c_str()), spots.is[i].r, spots.is[i].g, spots.is[i].b, 1.0);
		glUniform4f(glGetUniformLocation((*obj).program, ("lights[" + to_string(i+13) + "].ia").c_str()), spots.ia[i].r, spots.ia[i].g, spots.ia[i].b, 1.0);
		glUniform1f(glGetUniformLocation((*obj).program, ("lights[" + to_string(i+13) + "].ka").c_str()), spots.ka[i]);
		glUniform1f(glGetUniformLocation((*obj).program, ("lights[" + to_string(i+13) + "].kd").c_str()), spots.kd[i]);
		glUniform1f(glGetUniformLocation((*obj).program, ("lights[" + to_string(i+13) + "].ks").c_str()), spots.ks[i]);

	}
	glUniform4f(glGetUniformLocation((*obj).program, "viewPosition"), cameraPosition.x, cameraPosition.y, cameraPosition.z, 1.0);

	glUniform4f(glGetUniformLocation((*obj).program, "lightSpotDirection"), spots.lightSpotDirection.x, spots.lightSpotDirection.y , spots.lightSpotDirection.z, 0.0);
	glUniform1f(glGetUniformLocation((*obj).program, "lightSpotCutOff"), glm::cos(spots.lightSpotCutOff));

	glUniform1f(glGetUniformLocation((*obj).program, "transparency"), (*obj).transparency);

	// Bind textures and samplers - using 0 as I know there is only one texture - need to extend.

}

void attachTextures(modelData *obj) {
	int textureLen = sizeof((*obj).texture) / sizeof(*((*obj).texture));
	for (int i = 0; i < textureLen; i++) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, (*obj).texture[i]);
		glUniform1i((*obj).tex_location, 0);
	}
}
void drawModels(modelData *obj, glm::mat4 viewMatrix) {

	for (int i = 0; i < (*obj).numModels; i++) {
		glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3((*obj).positions[i].x, (*obj).positions[i].y, (*obj).positions[i].z));
		modelMatrix = glm::translate(modelMatrix, modelAngle);
		modelMatrix = glm::rotate(modelMatrix, modelAngle.x + (*obj).rotations[i].x, glm::vec3(1.0f, 0.0f, 0.0f));
		modelMatrix = glm::rotate(modelMatrix, modelAngle.y + (*obj).rotations[i].y, glm::vec3(0.0f, 1.0f, 0.0f));
		modelMatrix = glm::scale(modelMatrix, (*obj).scale);
		glm::mat4 mv_matrix = viewMatrix * modelMatrix;

		glUniformMatrix4fv(glGetUniformLocation((*obj).program, "model_matrix"), 1, GL_FALSE, &modelMatrix[0][0]);
		glUniformMatrix4fv(glGetUniformLocation((*obj).program, "view_matrix"), 1, GL_FALSE, &viewMatrix[0][0]);

		glDrawArrays(GL_TRIANGLES, 0, (*obj).out_vertices.size());
	}
}