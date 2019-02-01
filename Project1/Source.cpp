#include "Header.h"

/*main function, contains main loop and activates functions required for operation*/
int main()
{
	if (!glfwInit()) {							// Checking for GLFW
		std::cout << "Could not initialise GLFW...";
		return 0;
	}

	glfwSetErrorCallback(errorCallbackGLFW);	// Setup a function to catch and display all GLFW errors.

	hintsGLFW();								// Setup glfw with various hints.		

												// Start a window using GLFW
	string title = "My OpenGL Application";

	// Fullscreen
	//const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	//windowWidth = mode->width; windowHeight = mode->height;
	//window = glfwCreateWindow(windowWidth, windowHeight, title.c_str(), glfwGetPrimaryMonitor(), NULL);

	// Window
	window = glfwCreateWindow(windowWidth, windowHeight, title.c_str(), NULL, NULL);
	if (!window) {								// Window or OpenGL context creation failed
		std::cout << "Could not initialise GLFW...";
		endProgram();
		return 0;
	}

	glfwMakeContextCurrent(window);				// making the OpenGL context current

												// Start GLEW (note: always initialise GLEW after creating your window context.)
	glewExperimental = GL_TRUE;					// hack: catching them all - forcing newest debug callback (glDebugMessageCallback)
	GLenum errGLEW = glewInit();
	if (GLEW_OK != errGLEW) {					// Problems starting GLEW?
		std::cout << "Could not initialise GLEW...";
		endProgram();
		return 0;
	}

	debugGL();									// Setup callback to catch openGL errors.	

												// Setup all the message loop callbacks.
	glfwSetWindowSizeCallback(window, onResizeCallback);		// Set callback for resize
	glfwSetKeyCallback(window, onKeyCallback);					// Set Callback for keys
	glfwSetMouseButtonCallback(window, onMouseButtonCallback);	// Set callback for mouse click
	glfwSetCursorPosCallback(window, onMouseMoveCallback);		// Set callback for mouse move
	glfwSetScrollCallback(window, onMouseWheelCallback);		// Set callback for mouse wheel.
																//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);	// Set mouse cursor. Fullscreen
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);	// Set mouse cursor FPS.

	setupRender();								// setup some render variables.
	startup();									// Setup all necessary information for startup (aka. load texture, shaders, models, etc).

	std::cout << "entering loop" << endl;
	do {										// run until the window is closed
		GLfloat currentTime = (GLfloat)glfwGetTime();		// retrieve timelapse
		deltaTime = currentTime - lastTime;		// Calculate delta time
		lastTime = currentTime;					// Save for next frame calculations.
		glfwPollEvents();						// poll callbacks
		update(currentTime);					// update (physics, animation, structures, etc)
		render(currentTime);					// call render function.

		glfwSwapBuffers(window);				// swap buffers (avoid flickering and tearing)

		running &= (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_RELEASE);	// exit if escape key pressed
		running &= (glfwWindowShouldClose(window) != GL_TRUE);
	} while (running);

	endProgram();			// Close and clean everything up...

	std::cout << "\nPress any key to continue...\n";
	cin.ignore(); cin.get(); // delay closing console to read debugging errors.

	return 0;
}

/*sets the initial positions and the rotations of all of the models*/
void startup() {
	std::cout << "startup" << endl;

	

	tardisModel.numModels = 1;
	tardisModel.modelFile = "tardis_doors_attached.obj";

	tardisModel.rotations = new glm::vec3[tardisModel.numModels];
	tardisModel.rotations[0] = glm::vec3(0.0f, 3.14159f, 0.0f);

	tardisModel.positions = new glm::vec3[tardisModel.numModels];
	tardisModel.positions[0] = glm::vec3(-0.125f, 0.19f, 6.5f);

	tardisModel.scale = glm::vec3(0.05, 0.05, 0.05);

	tardisNoDoors.numModels = 1;
	tardisNoDoors.modelFile = "tardis_no_doors.obj";

	tardisNoDoors.rotations = tardisModel.rotations;
	tardisNoDoors.positions = tardisModel.positions;

	tardisNoDoors.scale = glm::vec3(0.05, 0.05, 0.05);
	

	tardisDoorLeft.numModels = 1;
	tardisDoorLeft.modelFile = "tardis_left_door.obj";

	tardisDoorLeft.rotations = new glm::vec3[tardisDoorLeft.numModels];
	tardisDoorLeft.rotations[0] = glm::vec3(0.0f, 3.14159f, 0.0f);

	tardisDoorLeft.positions = new glm::vec3[tardisDoorLeft.numModels];
	tardisDoorLeft.positions[0] = glm::vec3(tardisNoDoors.positions[0].x + 0.12, tardisModel.positions[0].y + 0.029f, tardisModel.positions[0].z - 0.125);

	tardisDoorLeft.scale = glm::vec3(0.05, 0.05, 0.05);

	tardisDoorRight.numModels = 1;
	tardisDoorRight.modelFile = "tardis_right_door.obj";

	tardisDoorRight.rotations = new glm::vec3[tardisDoorLeft.numModels];
	tardisDoorRight.rotations[0] = glm::vec3(0.0f, -3.14159f, 0.0f);

	tardisDoorRight.positions = new glm::vec3[tardisDoorLeft.numModels];
	tardisDoorRight.positions[0] = glm::vec3(tardisNoDoors.positions[0].x - 0.12, tardisModel.positions[0].y + 0.032f, tardisModel.positions[0].z - 0.125);

	tardisDoorRight.scale = glm::vec3(0.05, 0.05, 0.05);

	dalekModel.numModels = numDaleks;
	dalekModel.modelFile = "dalek.obj";
	dalekArkStages.deMat = false;

	dalekModel.rotations = new glm::vec3[dalekModel.numModels];
	for (int i = 0; i < 6; i++) {
		dalekModel.rotations[i] = glm::vec3(0.0f, 3.14159f, 0.0f);
	}
	for (int i = 0; i < 6; i++) {
		dalekModel.rotations[i+6] = glm::vec3(0.0f, 0.0f, 0.0f);
	}
	
	dalekModel.positions = new glm::vec3[dalekModel.numModels];
	dalekModel.positions[0] = glm::vec3(4.15f, 0.1f, 4.35f);
	for (int i = 0; i < 5; i++) {
		if ((i + 1) < 3) {

			dalekModel.positions[i + 1] = glm::vec3((dalekModel.positions[0].x - 0.5) + (1 * i), dalekModel.positions[0].y, dalekModel.positions[0].z + 0.5);
		}
		if ((i + 1) >= 3) {

			dalekModel.positions[i + 1] = glm::vec3((dalekModel.positions[0].x - 1.0) + (1 * (i - 2)), dalekModel.positions[0].y, dalekModel.positions[0].z + 1.0);
		}
	}

	dalekModel.positions[6] = glm::vec3(-4.15f, 0.1f, -4.35f);
	for (int i = 0; i < 5; i++) {
		if ((i + 1) < 3) {

			dalekModel.positions[i + 7] = glm::vec3((dalekModel.positions[6].x - 0.5) + (1 * i), dalekModel.positions[6].y, dalekModel.positions[6].z - 0.5);
		}
		if ((i + 1) >= 3) {

			dalekModel.positions[i + 7] = glm::vec3((dalekModel.positions[6].x - 1.0) + (1 * (i - 2)), dalekModel.positions[6].y, dalekModel.positions[6].z - 1.0);
		}
	}

	dalekModel.scale = glm::vec3(0.05, 0.05, 0.05);


	buildingModel.numModels = 12;
	buildingModel.modelFile = "building_2.obj";

	buildingModel.rotations = new glm::vec3[buildingModel.numModels];
	buildingModel.rotations[0] = glm::vec3(0.0f, 0.0f, 0.0f);
	buildingModel.rotations[1] = glm::vec3(0.0f, 0.0f, 0.0f);
	buildingModel.rotations[2] = glm::vec3(0.0f, 0.0f, 0.0f);
	buildingModel.rotations[3] = glm::vec3(0.0f, 0.0f, 0.0f);
	buildingModel.rotations[4] = glm::vec3(0.0f, 0.0f, 0.0f);
	buildingModel.rotations[5] = glm::vec3(0.0f, 0.0f, 0.0f);
	buildingModel.rotations[6] = glm::vec3(0.0f, 0.0f, 0.0f);
	buildingModel.rotations[7] = glm::vec3(0.0f, 0.0f, 0.0f);
	buildingModel.rotations[8] = glm::vec3(0.0f, 0.0f, 0.0f);
	buildingModel.rotations[9] = glm::vec3(0.0f, 0.0f, 0.0f);
	buildingModel.rotations[10] = glm::vec3(0.0f, 0.0f, 0.0f);
	buildingModel.rotations[11] = glm::vec3(0.0f, 0.0f, 0.0f);

	buildingModel.positions = new glm::vec3[buildingModel.numModels];
	buildingModel.positions[0] = glm::vec3(7.75f, 0.0f, 7.5f);
	buildingModel.positions[1] = glm::vec3(1.25f, 0.0f, 7.5f);
	buildingModel.positions[2] = glm::vec3(-1.5f, 0.0f, 7.5f);
	buildingModel.positions[3] = glm::vec3(-7.75f, 0.0f, 7.5f);
	buildingModel.positions[4] = glm::vec3(-7.75f, 0.0f, 1.00f);
	buildingModel.positions[5] = glm::vec3(-7.75f, 0.0f, -1.75f);
	buildingModel.positions[6] = glm::vec3(-7.75f, 0.0f, -8.0f);
	buildingModel.positions[7] = glm::vec3(-1.5f, 0.0f, -8.0f);
	buildingModel.positions[8] = glm::vec3(1.25f, 0.0f, -8.0f);
	buildingModel.positions[9] = glm::vec3(7.75f, 0.0f, -8.0f);
	buildingModel.positions[10] = glm::vec3(7.75f, 0.0f, -1.75f);
	buildingModel.positions[11] = glm::vec3(7.75f, 0.0f, 1.00f);


	buildingModel.scale = glm::vec3(0.15, 0.15, 0.15);



	lampModels.numModels = 12;
	lampModels.modelFile = "lamppostblend.obj";

	lampModels.rotations = new glm::vec3[lampModels.numModels];
	lampModels.rotations[0] = glm::vec3(0.0f, 0.0f, 0.0f);
	lampModels.rotations[1] = glm::vec3(0.0f, 3.14159f, 0.0f);
	lampModels.rotations[2] = glm::vec3(0.0f, 3.14159f, 0.0f);
	lampModels.rotations[3] = glm::vec3(0.0f, 1.5708f, 0.0f);
	lampModels.rotations[4] = glm::vec3(0.0f, 1.5708f, 0.0f);
	lampModels.rotations[5] = glm::vec3(0.0f, 4.71239f, 0.0f);
	lampModels.rotations[6] = glm::vec3(0.0f, 0.0f, 0.0f);
	lampModels.rotations[7] = glm::vec3(0.0f, 0.0f, 0.0f);
	lampModels.rotations[8] = glm::vec3(0.0f, 3.14159f, 0.0f);
	lampModels.rotations[9] = glm::vec3(0.0f, 4.71239f, 0.0f);
	lampModels.rotations[10] = glm::vec3(0.0f, 4.71239f, 0.0f);
	lampModels.rotations[11] = glm::vec3(0.0f, 1.5708f, 0.0f);

	lampModels.positions = new glm::vec3[lampModels.numModels];
	lampModels.positions[0] = glm::vec3(0.0f, 0.2f, 2.5f);
	lampModels.positions[1] = glm::vec3(1.25f, 0.2f, 5.5f);
	lampModels.positions[2] = glm::vec3(-1.5f, 0.2f, 5.5f);
	lampModels.positions[3] = glm::vec3(-5.8f, 0.2f, 1.0f);
	lampModels.positions[4] = glm::vec3(-5.8f, 0.2f, -1.75f);
	lampModels.positions[5] = glm::vec3(-2.5f, 0.2f, 0.0f);
	lampModels.positions[6] = glm::vec3(-1.5f, 0.2f, -6.1f);
	lampModels.positions[7] = glm::vec3(1.25f, 0.2f, -6.1f);
	lampModels.positions[8] = glm::vec3(0.0f, 0.2f, -3.0f);
	lampModels.positions[9] = glm::vec3(5.8f, 0.2f, 1.0f);
	lampModels.positions[10] = glm::vec3(5.8f, 0.2f, -1.75f);
	lampModels.positions[11] = glm::vec3(2.5f, 0.2f, 0.0f);

	lampModels.scale = glm::vec3(0.3, 0.3, 0.3);
	std::cout << "model rotations and positions" << endl;



	SetLights();
	

	groundModel.numModels = 1;
	groundModel.modelFile = "ground_2.obj";

	groundModel.rotations = new glm::vec3[groundModel.numModels];
	groundModel.rotations[0] = glm::vec3(0.0f, 0.0f, 0.0f);

	groundModel.positions = new glm::vec3[groundModel.numModels];
	groundModel.positions[0] = glm::vec3(0.0f, 0.0f, 0.0f);


	groundModel.scale = glm::vec3(1.0, 1.0, 1.0);
	std::cout << "model rotations and positions" << endl;


	

	for (int i = 0; i < objNum; i++) {
		attachShaderFiles(models[i]->vsFile, models[i]->fsFile, models[i]);
		readObj(models[i]->modelFile, models[i]);
		bindVertexs(models[i]);
	}

	for (int i = 0; i < 3; i++) {
		attachShaderFiles(noDoors[i]->vsFile, noDoors[i]->fsFile, noDoors[i]);
		readObj(noDoors[i]->modelFile, noDoors[i]);
		bindVertexs(noDoors[i]);
	}

	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	aspect = (float)windowWidth / (float)windowHeight;
	proj_matrix = glm::perspective(glm::radians(fovy), aspect, 0.1f, 1000.0f);

	createFramebuffer();

	for (int i = 0; i < models[0]->out_vertices.size(); i++) {
		//cout << "x:"<< models[0].out_vertices[i].x <<endl;
		//cout << "y:" << models[0].out_vertices[i].y << endl;
		//cout << "z:" << models[0].out_vertices[i].z << endl;

	}
	if (tStages.deMat) {
		tardisDec = 0;
	}
	
}

/*sets the initial position and attributes of all the lights*/
void SetLights() {
	// lamp disps
	lights.lightDisp[0] = glm::vec3(lampModels.positions[0].x + 0.25, lampModels.positions[0].y + 1.85, lampModels.positions[0].z + 0.3);
	lights.lightDisp[1] = glm::vec3(lampModels.positions[1].x - 0.25, lampModels.positions[1].y + 1.85, lampModels.positions[1].z - 0.3);
	lights.lightDisp[2] = glm::vec3(lampModels.positions[2].x - 0.25, lampModels.positions[2].y + 1.85, lampModels.positions[2].z - 0.3);
	lights.lightDisp[3] = glm::vec3(lampModels.positions[3].x + 0.3, lampModels.positions[3].y + 1.85, lampModels.positions[3].z - 0.25);
	lights.lightDisp[4] = glm::vec3(lampModels.positions[4].x + 0.3, lampModels.positions[4].y + 1.85, lampModels.positions[4].z - 0.25);
	lights.lightDisp[5] = glm::vec3(lampModels.positions[5].x - 0.3, lampModels.positions[5].y + 1.85, lampModels.positions[5].z + 0.25);
	lights.lightDisp[6] = glm::vec3(lampModels.positions[6].x + 0.25, lampModels.positions[6].y + 1.85, lampModels.positions[6].z + 0.3);
	lights.lightDisp[7] = glm::vec3(lampModels.positions[7].x + 0.25, lampModels.positions[7].y + 1.85, lampModels.positions[7].z + 0.3);
	lights.lightDisp[8] = glm::vec3(lampModels.positions[8].x - 0.25, lampModels.positions[8].y + 1.85, lampModels.positions[8].z - 0.3);
	lights.lightDisp[9] = glm::vec3(lampModels.positions[9].x - 0.3, lampModels.positions[9].y + 1.85, lampModels.positions[9].z + 0.25);
	lights.lightDisp[10] = glm::vec3(lampModels.positions[10].x - 0.3, lampModels.positions[10].y + 1.85, lampModels.positions[10].z + 0.25);
	lights.lightDisp[11] = glm::vec3(lampModels.positions[11].x + 0.3, lampModels.positions[11].y + 1.85, lampModels.positions[11].z - 0.25);

	for (int i = 0; i < 12; i++) {
		lights.ka[i] = 0.07;
		lights.ks[i] = 0.2;
		lights.kd[i] = 0.05;
	}

	// lamp id
	lights.id[0] = glm::vec3(0.93f, 0.75f, 0.32f);
	lights.id[1] = glm::vec3(0.93f, 0.75f, 0.32f);
	lights.id[2] = glm::vec3(0.93f, 0.75f, 0.32f);
	lights.id[3] = glm::vec3(0.93f, 0.75f, 0.32f);
	lights.id[4] = glm::vec3(0.93f, 0.75f, 0.32f);
	lights.id[5] = glm::vec3(0.93f, 0.75f, 0.32f);
	lights.id[6] = glm::vec3(0.93f, 0.75f, 0.32f);
	lights.id[7] = glm::vec3(0.93f, 0.75f, 0.32f);
	lights.id[8] = glm::vec3(0.93f, 0.75f, 0.32f);
	lights.id[9] = glm::vec3(0.93f, 0.75f, 0.32f);
	lights.id[10] = glm::vec3(0.93f, 0.75f, 0.32f);
	lights.id[11] = glm::vec3(0.93f, 0.75f, 0.32f);

	float shininess = 10.0;
	// lamp shininess
	lights.shininess[0] = shininess;
	lights.shininess[1] = shininess;
	lights.shininess[2] = shininess;
	lights.shininess[3] = shininess;
	lights.shininess[4] = shininess;
	lights.shininess[5] = shininess;
	lights.shininess[6] = shininess;
	lights.shininess[7] = shininess;
	lights.shininess[8] = shininess;
	lights.shininess[9] = shininess;
	lights.shininess[10] = shininess;
	lights.shininess[11] = shininess;
	
	//lamp ia
	lights.ia[0] = glm::vec3(0.53f, 0.25f, 0.12f);
	lights.ia[1] = glm::vec3(0.53f, 0.25f, 0.12f);
	lights.ia[2] = glm::vec3(0.53f, 0.25f, 0.12f);
	lights.ia[3] = glm::vec3(0.53f, 0.25f, 0.12f);
	lights.ia[4] = glm::vec3(0.53f, 0.25f, 0.12f);
	lights.ia[5] = glm::vec3(0.53f, 0.25f, 0.12f);
	lights.ia[6] = glm::vec3(0.53f, 0.25f, 0.12f);
	lights.ia[7] = glm::vec3(0.53f, 0.25f, 0.12f);
	lights.ia[8] = glm::vec3(0.53f, 0.25f, 0.12f);
	lights.ia[9] = glm::vec3(0.53f, 0.25f, 0.12f);
	lights.ia[10] = glm::vec3(0.53f, 0.25f, 0.12f);
	lights.ia[11] = glm::vec3(0.53f, 0.25f, 0.12f);

	//lamp is
	lights.is[0] = glm::vec3(0.5f, 0.5f, 0.5f);
	lights.is[1] = glm::vec3(0.5f, 0.5f, 0.5f);
	lights.is[2] = glm::vec3(0.5f, 0.5f, 0.5f);
	lights.is[3] = glm::vec3(0.5f, 0.5f, 0.5f);
	lights.is[4] = glm::vec3(0.5f, 0.5f, 0.5f);
	lights.is[5] = glm::vec3(0.5f, 0.5f, 0.5f);
	lights.is[6] = glm::vec3(0.5f, 0.5f, 0.5f);
	lights.is[7] = glm::vec3(0.5f, 0.5f, 0.5f);
	lights.is[8] = glm::vec3(0.5f, 0.5f, 0.5f);
	lights.is[6] = glm::vec3(0.5f, 0.5f, 0.5f);
	lights.is[9] = glm::vec3(0.5f, 0.5f, 0.5f);
	lights.is[10] = glm::vec3(0.5f, 0.5f, 0.5f);
	lights.is[11] = glm::vec3(0.5f, 0.5f, 0.5f);


	// dalek ligh disp
	spots.lightDisp[0] = dalekModel.positions[0];
	spots.lightDisp[1] = dalekModel.positions[1];
	spots.lightDisp[2] = dalekModel.positions[2];
	spots.lightDisp[3] = dalekModel.positions[3];
	spots.lightDisp[4] = dalekModel.positions[4];
	spots.lightDisp[5] = dalekModel.positions[5];
	spots.lightDisp[6] = dalekModel.positions[6];
	spots.lightDisp[7] = dalekModel.positions[7];
	spots.lightDisp[8] = dalekModel.positions[8];
	spots.lightDisp[9] = dalekModel.positions[9];
	spots.lightDisp[10] = dalekModel.positions[10];
	spots.lightDisp[11] = dalekModel.positions[11];

glm::vec3 id = glm::vec3(0.0f, 0.7f, 0.00f);
// dalek light id
spots.id[0] = id;
spots.id[1] = id;
spots.id[2] = id;
spots.id[3] = id;
spots.id[4] = id;
spots.id[5] = id;
spots.id[6] = id;
spots.id[7] = id;
spots.id[8] = id;
spots.id[9] = id;
spots.id[10] = id;
spots.id[11] = id;

shininess = 0.0001;

spots.shininess[0] = shininess;
spots.shininess[1] = shininess;
spots.shininess[2] = shininess;
spots.shininess[3] = shininess;
spots.shininess[4] = shininess;
spots.shininess[5] = shininess;
spots.shininess[6] = shininess;
spots.shininess[7] = shininess;
spots.shininess[8] = shininess;
spots.shininess[9] = shininess;
spots.shininess[10] = shininess;
spots.shininess[11] = shininess;

glm::vec3 ia = glm::vec3(0.00f, 0.70f, 0.00f);
//lamp ia
spots.ia[0] = ia;
spots.ia[1] = ia;
spots.ia[2] = ia;
spots.ia[3] = ia;
spots.ia[4] = ia;
spots.ia[5] = ia;
spots.ia[6] = ia;
spots.ia[7] = ia;
spots.ia[8] = ia;
spots.ia[9] = ia;
spots.ia[10] = ia;
spots.ia[11] = ia;

glm::vec3 is = glm::vec3(0.00f, 0.7f, 0.00f);
//lamp is
spots.is[0] = is;
spots.is[1] = is;
spots.is[2] = is;
spots.is[3] = is;
spots.is[4] = is;
spots.is[5] = is;
spots.is[6] = is;
spots.is[7] = is;
spots.is[8] = is;
spots.is[9] = is;
spots.is[10] = is;
spots.is[11] = is;

for (int i = 0; i < 12; i++) {
	spots.ka[i] = 0.005;
	spots.ks[i] = 0.05;
	spots.kd[i] = 0.005;
}

lights.lightDisp[12] = tardisModel.positions[0];
lights.id[12] = glm::vec3(0.0f, 0.0f, 0.9f);
lights.shininess[12] = 0.0001;
lights.ia[12] = glm::vec3(0.0f, 0.0f, 0.9f);
lights.is[12] = glm::vec3(0.0f, 0.0f, 0.9f);
lights.ka[12] = 0.01;
lights.ks[12] = 0.01;
lights.kd[12] = 0.01;
}

/*updates the position of lights*/
void UpdateLights() {
	spots.lightDisp[0] = dalekModel.positions[0];
	spots.lightDisp[1] = dalekModel.positions[1];
	spots.lightDisp[2] = dalekModel.positions[2];
	spots.lightDisp[3] = dalekModel.positions[3];
	spots.lightDisp[4] = dalekModel.positions[4];
	spots.lightDisp[5] = dalekModel.positions[5];
	spots.lightDisp[6] = dalekModel.positions[6];
	spots.lightDisp[7] = dalekModel.positions[7];
	spots.lightDisp[8] = dalekModel.positions[8];
	spots.lightDisp[9] = dalekModel.positions[9];
	spots.lightDisp[10] = dalekModel.positions[10];
	spots.lightDisp[11] = dalekModel.positions[11];
}

/*updates variables based on the input key of the user*/
void update(GLfloat currentTime) {
	// calculate movement
	GLfloat cameraSpeed = 10.0f * deltaTime;
	if (keyStatus[GLFW_KEY_W]) cameraPosition += cameraSpeed * cameraFront;
	if (keyStatus[GLFW_KEY_S]) cameraPosition -= cameraSpeed * cameraFront;
	if (keyStatus[GLFW_KEY_A]) cameraPosition -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (keyStatus[GLFW_KEY_D]) {
		cameraPosition += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

	}


	if (keyStatus[GLFW_KEY_L] && (movingLight == false)) {
		std::cout << "Change mode to moving light...\n";
		movingLight = true;
		movingDalek = false;
		cameraMode = false;
	}
	if (keyStatus[GLFW_KEY_C] && (cameraMode == false)){
		std::cout << "Change mode to camera...\n";
		movingLight = false;
		movingDalek = false;
		cameraMode = true;
	}
	if (keyStatus[GLFW_KEY_M] && (movingLight == true || (movingDalek == true) || (cameraMode==true))) {
		std::cout << "Change mode to moving object...\n";
		movingLight = false;
		movingDalek = false;
		cameraMode = false;
	}
	if (keyStatus[GLFW_KEY_F4] && (movingDalek == false)) {
		std::cout << "Change mode to moving dalek...\n";
		movingDalek = true;
		movingLight = false;
		cameraMode = false;
	}

	if (movingLight == false && movingDalek == false && cameraMode == false) {		// moving object rotation and z displacement
		if (keyStatus[GLFW_KEY_LEFT])			models[0]->positions[0].x += 0.05f;
		if (keyStatus[GLFW_KEY_RIGHT])			models[0]->positions[0].x -= 0.05f;
		if (keyStatus[GLFW_KEY_UP])				models[0]->positions[0].y += 0.05f;
		if (keyStatus[GLFW_KEY_DOWN])			models[0]->positions[0].y -= 0.05f;
		if (keyStatus[GLFW_KEY_KP_ADD])			models[0]->positions[0].z += 0.10f;
		if (keyStatus[GLFW_KEY_KP_SUBTRACT]) {
			models[0]->positions[0].z -= 0.10f;  
		}
		if (keyStatus[GLFW_KEY_Q]) { models[0]->rotations[0].y += 0.10f; doorsMoved = true;}
		if (keyStatus[GLFW_KEY_E]) { models[0]->rotations[0].y -= 0.10f; doorsMoved = true; 
}
		if (keyStatus[GLFW_KEY_P]) {
			std::cout << "cam x: " << cameraPosition.x << endl;
			std::cout << "cam y: " << cameraPosition.y << endl;
			std::cout << "cam z: " << cameraPosition.z << endl;
			std::cout << "cam f x" << cameraFront.x << endl;
			std::cout << "cam f y: " << cameraFront.y << endl;
			std::cout << "cam f z: " << cameraFront.z << endl;

		}
		if (keyStatus[GLFW_KEY_F1]) {
			models[0]->positions[0] = glm::vec3(1.05f, 0.4f, 0.5);
			placeDoors();
		}
		if (keyStatus[GLFW_KEY_F2]) {
			tStages.reMat = true;
			tStages.deMat = false;
			tStages.stage8 = false;
			tStages.stage1 = true;
		}
		if (keyStatus[GLFW_KEY_F3]) {
			tStages.deMat = true;
			tStages.reMat = false;
			tStages.stage8 = false;
			tStages.stage1 = true;
			tardisLanded = false;
		}
		if (keyStatus[GLFW_KEY_F5]) {
			open = true;
			close = false;
		}
		if (keyStatus[GLFW_KEY_F6]) {
			open = false;
			close = true;
		}
		if (keyStatus[GLFW_KEY_F7]) {

			lockCameraBool = true;
		}
		if (keyStatus[GLFW_KEY_F8]) {

			lockCameraBool = false;
		}

		
	}
	else if(movingDalek){
		for (int i = 0; i < models[3]->numModels; i++) {
			if (keyStatus[GLFW_KEY_LEFT])			models[3]->positions[i].x -= 0.05f;
			if (keyStatus[GLFW_KEY_RIGHT])			models[3]->positions[i].x += 0.05f;
			if (keyStatus[GLFW_KEY_UP])				models[3]->positions[i].y += 0.05f;
			if (keyStatus[GLFW_KEY_DOWN])			models[3]->positions[i].y -= 0.05f;
			if (keyStatus[GLFW_KEY_KP_ADD])			models[3]->positions[i].z += 0.05f;
			if (keyStatus[GLFW_KEY_KP_SUBTRACT])	models[3]->positions[i].z -= 0.05f;
			if (keyStatus[GLFW_KEY_E]) { models[3]->rotations[i].y += 0.05f;}
			if (keyStatus[GLFW_KEY_Q])				models[3]->rotations[i].y -= 0.05f;
			if (keyStatus[GLFW_KEY_P]) {
				std::cout << i << ". x: " << models[3]->positions[i].x <<endl;
				std::cout << i <<". y: " << models[3]->positions[i].y <<endl;
				std::cout << i << ". z:" << models[3]->positions[i].z <<endl;
			}
			if (keyStatus[GLFW_KEY_F1] && dalekArkStages.deMat == false) {
				dalekArkStages.deMat = true;
				dalekArkStages.stage1 = true;
				dalekArkStages2.stage1 = true;
				std::cout << "working " << endl;
			}

			if (keyStatus[GLFW_KEY_F2]) {
				dalekArkStages.deMat = false;
				dalekArkStages.stage1 = true;
				models[3]->arkAngle = 6.28319f;
			}
			if (keyStatus[GLFW_KEY_F3]) {
				dalekArkStages.stage3 = true;
			}
		}
	}
	else if (cameraMode) {
		if (keyStatus[GLFW_KEY_F1]) {
			cameraPosition.y = models[3]->positions[0].y + 0.5;
			cameraPosition.x = models[3]->positions[0].x;
			cameraPosition.z = models[3]->positions[0].z + 5;

			cameraFront.y = 0;
			cameraFront.x = 0;
			cameraFront.z = -1;
		}
		if (keyStatus[GLFW_KEY_F2]) {
			cameraPosition.y = 0.812664;
			cameraPosition.x = 1.5303;
			cameraPosition.z = -1.432;

			cameraFront.y = -0.0322828;
			cameraFront.x = -0.256152;
			cameraFront.z = 0.966097;
		}
		if (keyStatus[GLFW_KEY_F3]) {
			cameraPosition.y = 0.700109;
			cameraPosition.x = -0.186071;
			cameraPosition.z = 4.56459;

			cameraFront.y = -0.0845474;
			cameraFront.x = 0.00347796;
			cameraFront.z = 0.996413;
		}
		if (keyStatus[GLFW_KEY_F5]) {
			cameraPosition.y = 18.45;
			cameraPosition.x = -11.97;
			cameraPosition.z = 10.01;

			cameraFront.y = -0.79069;
			cameraFront.x = 0.462397;
			cameraFront.z = -0.401247;
		}



	}
	else {							// moving light displacement x y z
		
			if (keyStatus[GLFW_KEY_LEFT])			lights.lightDisp[lights.index].x -= 0.05f;
			if (keyStatus[GLFW_KEY_RIGHT])			lights.lightDisp[lights.index].x += 0.05f;
			if (keyStatus[GLFW_KEY_UP])				lights.lightDisp[lights.index].y += 0.05f;
			if (keyStatus[GLFW_KEY_DOWN])			lights.lightDisp[lights.index].y -= 0.05f;
			if (keyStatus[GLFW_KEY_KP_ADD])			lights.lightDisp[lights.index].z += 0.05f;
			if (keyStatus[GLFW_KEY_KP_SUBTRACT])	lights.lightDisp[lights.index].z -= 0.05f;
			if (keyStatus[GLFW_KEY_F1]) {
				if (lights.index < LightConstant) {
					lights.index++;
				}
				else
				{
					lights.index = 0;
				}
			}
			if (keyStatus[GLFW_KEY_P]) {
				std::cout << ". x: " << lights.lightDisp[lights.index].x << endl;
				std::cout << ". y: " << lights.lightDisp[lights.index].y << endl;
				std::cout << ". z:" << lights.lightDisp[lights.index].z << endl;
			}
		}
	
}

/*checks booleans and other variables, outputs the scene based on the result, repsonsible
for the activation of the animation functions*/
void render(GLfloat currentTime) {
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebufferTexture, 0);

	glViewport(0, 0, windowWidth, windowHeight);

	glm::vec4 backgroundColor = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f); glClearBufferfv(GL_COLOR, 0, &backgroundColor[0]);
	glEnable(GL_DEPTH_TEST);
	static const GLfloat one = 1.0f; glClearBufferfv(GL_DEPTH, 0, &one);
	glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glm::mat4 viewMatrix = glm::lookAt(cameraPosition,					// eye
		cameraPosition + cameraFront,	// centre
		cameraUp);


	if (dalekArkStages.deMat) {
		dalekArk1(currentTime);
		dalekArk2(currentTime);
	}
	if (doorsMoved) {
		//placeDoors();
	}
	if (open) {
		openDoors(currentTime);
	}
	if (close) {
		closeDoors(currentTime);
	}
	if (lockCameraBool) {
		lockCamera();
	}
	
	
	for (int i = 0; i < objNum; i++) {
		if ((tStages.deMat || tStages.reMat) && i == 0) {
			attachLights(models[i]);
			attachSpots(models[i]);
			attachTextures(models[i]);
			tardisFade(models[i], viewMatrix, currentTime);

		}
		else {
			attachLights(models[i]);
			attachSpots(models[i]);
			attachTextures(models[i]);
			drawModels(models[i], viewMatrix);
		}
	}

	if (tardisLanded) {
		for (int i = 0; i < 3; i++)
		{
			attachLights(noDoors[i]);
			attachSpots(noDoors[i]);
			attachTextures(noDoors[i]);
			SpawnWithDoors(noDoors[i], viewMatrix);
			
		}
	}
	UpdateLights();
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);	// Disable rendering framebuffer to texture - aka render normally.
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST); //not needed as we are just displaying a single quad
	
	glUseProgram(display.program);
	glUniform1f(glGetUniformLocation(display.program, "width"), windowWidth);
	glUniform1f(glGetUniformLocation(display.program, "height"), windowHeight);
	glBindVertexArray(display.vao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, framebufferTexture);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	
	prevTime = currentTime;
}

/*function to define how manyy vertices the tardis model will be drawn with*/
void tardisFade(modelData *obj, glm::mat4 viewMatrix, GLfloat currentTime) {

	if (tStages.deMat) {
		tardisDeMat(&(*obj), currentTime);

	}
	else if(tStages.reMat)
	{
		tardisReMat(&(*obj));
	}



	for (int i = 0; i < (*obj).numModels; i++) {
		glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3((*obj).positions[i].x, (*obj).positions[i].y, (*obj).positions[i].z));// modelDisp.z));
		modelMatrix = glm::translate(modelMatrix, modelAngle);
		modelMatrix = glm::rotate(modelMatrix, modelAngle.x + (*obj).rotations[i].x, glm::vec3(1.0f, 0.0f, 0.0f));
		modelMatrix = glm::rotate(modelMatrix, modelAngle.y + (*obj).rotations[i].y, glm::vec3(0.0f, 1.0f, 0.0f));
		modelMatrix = glm::scale(modelMatrix, (*obj).scale);
		glm::mat4 mv_matrix = viewMatrix * modelMatrix;

		glUniformMatrix4fv(glGetUniformLocation((*obj).program, "model_matrix"), 1, GL_FALSE, &modelMatrix[0][0]);
		glUniformMatrix4fv(glGetUniformLocation((*obj).program, "view_matrix"), 1, GL_FALSE, &viewMatrix[0][0]);

		glDrawArrays(GL_TRIANGLES, 0, (int)tardisDec);
	}
}

/*function for animation of tardis reappearing*/
void tardisReMat(modelData *obj) {
	if (tStages.stage1) {
		cout << tardisDec << endl;
		tardisDec = 210;
		(*obj).transparency = 0.05;
		tStages.stage2 = true;
		tStages.stage1 = false;
	}
	else if (tStages.stage2)
	{
		tardisDec *= 1.1;
		(*obj).transparency *= 1.03;

		if (tardisDec >= 610)
		{
			tardisDec = 610;
			tStages.stage3 = true;
			tStages.stage2 = false;
		}


	}
	else if (tStages.stage3)
	{

		tardisDec *= 0.9;
		(*obj).transparency *= 0.9;
		if (tardisDec <= 210)
		{
			tardisDec = 210;
			tStages.stage4 = true;
			tStages.stage3 = false;
		}
	}
	else if (tStages.stage4)
	{
		tardisDec *= 1.05;
		(*obj).transparency *= 1.04;
		if (tardisDec >= 1000)
		{
			tardisDec = 1000;
			tStages.stage5 = true;
			tStages.stage4 = false;
		}

	}
	else if (tStages.stage5)
	{
		tardisDec *= 0.9;
		(*obj).transparency *= 0.9;
		if (tardisDec <= 210)
		{
			tardisDec = 210;
			tStages.stage6 = true;
			tStages.stage5 = false;
		}
	}
	else if (tStages.stage6)
	{
		tardisDec *= 1.05;
		(*obj).transparency *= 1.03;
		if (tardisDec >= 1900)
		{
			tardisDec = 1500;
			tStages.stage7 = true;
			tStages.stage6 = false;
		}

	}
	else if (tStages.stage7)
	{
		tardisDec *= 0.9;
		(*obj).transparency *= 0.9;
		if (tardisDec <= 210)
		{
			tardisDec = 210;
			tStages.stage8 = true;
			tStages.stage7 = false;
		}
	}
	else if (tStages.stage8)
	{
		tardisDec *= 1.05;
		(*obj).transparency *= 1.06;
		if (tardisDec >= (*obj).out_vertices.size())
		{
			tardisDec = 0;
			tardisLanded = true;

		}
	}
}

/*spawns the tardis with the doors*/
void SpawnWithDoors(modelData *obj, glm::mat4 viewMatrix) {

	for (int i = 0; i < (*obj).numModels; i++) {
		glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3((*obj).positions[i].x, (*obj).positions[i].y, (*obj).positions[i].z));// modelDisp.z));
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

/*function for the animation of the tardis disappearing*/
void tardisDeMat(modelData *obj, GLfloat currentTime) {
	updateTime = currentTime - prevTime;
	if (tStages.stage1) {
		cout << tardisDec << endl;
		tardisDec = (*obj).out_vertices.size();;
		(*obj).transparency = 0.75;
		tStages.stage2 = true;
		tStages.stage1 = false;
	}
	else if (tStages.stage2)
	{
		cout << tardisDec << endl;
		tardisDec *= 0.95;
		(*obj).transparency *= 0.95;
		cout <<updateTime << endl;

		if (tardisDec <= 1500)
		{
			tardisDec = 1500;
			tStages.stage3 = true;
			tStages.stage2 = false;
		}


	}
	else if (tStages.stage3)
	{

		tardisDec *= 1.05 ;
		(*obj).transparency *= 1.01;
		if (tardisDec >= (*obj).out_vertices.size())
		{
			tardisDec = (*obj).out_vertices.size();
			tStages.stage4 = true;
			tStages.stage3 = false;
		}
	}
	else if (tStages.stage4)
	{
		tardisDec *= 0.95 ;
		(*obj).transparency *= 0.99;
		if (tardisDec <= 800)
		{
			tardisDec = 1000;
			tStages.stage5 = true;
			tStages.stage4 = false;
		}

	}
	else if (tStages.stage5)
	{
		tardisDec *= 1.05;
		(*obj).transparency *= 1.01 ;
		if (tardisDec >= 1500)
		{
			tardisDec = 1500;
			tStages.stage6 = true;
			tStages.stage5 = false;
		}
	}
	else if (tStages.stage6)
	{
		tardisDec *= 0.95;
		(*obj).transparency *= 0.99 ;
		if (tardisDec <= 410)
		{
			tardisDec = 610;
			tStages.stage7 = true;
			tStages.stage6 = false;
		}

	}
	else if (tStages.stage7)
	{
		tardisDec *= 1.05 ;
		(*obj).transparency *= 1.01;
		if (tardisDec >= 1000)
		{
			tardisDec = 1000;
			tStages.stage8 = true;
			tStages.stage7 = false;
		}
	}
	else if (tStages.stage8)
	{
		tardisDec *= 0.95;
		(*obj).transparency *= 0.99;
		if (tardisDec <= 200)
		{
			tardisDec = 0;

		}
	}
}

/*calculates an ark based on angle and distance*/
float *calc_ark(modelData *obj, float retVal[]) {
	double x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;

	float h = (*obj).arkRad;
	float a = glm::degrees(glm::abs((*obj).arkAngle));

	x = h*sin(a);
	y = sqrt((h*h) - (x*x));

	cout << "ark x :" << a << endl;

	retVal[0] =  x;
	retVal[1] = y;
	return retVal;
}

/*animation function for first squad of daleks landing ark*/
void dalekArk1(GLfloat currentTime) {
	updateTime = currentTime - prevTime;
	float newVals[2];
	float xAdd = 0.0f;
	float yAdd = 0.0f;
	if (dalekArkStages.stage1) {
		for (int i = 0; i < models[3]->numModels - 6; i++) {
			models[3]->positions[i].z += 1.0;
			models[3]->positions[i].y += 20.5;
			arkStartPos[i].x = models[3]->positions[i].z;
			arkStartPos[i].y = models[3]->positions[i].y;
			models[3]->rotations[i].x = 4.71239;
			dalekModel.arkRad = 4.5;

		}
		//arkStartPos = models[3]->positions;
		dalekArkStages.stage1 = false;
		dalekArkStages.stage2 = true;
	}
	else if(dalekArkStages.stage2){
		for (int i = 0; i < models[3]->numModels - 6; i++) {
			if (models[3]->positions[i].y <= 4.5) {
				arkStartPos[i].x = models[3]->positions[i].z;
				arkStartPos[i].y = models[3]->positions[i].y;
				dalekArkStages.stage2 = false;
				dalekArkStages.stage3 = true;
			}
			if (dalekArkStages.stage2) {
				models[3]->positions[i].y -= 5.0 * updateTime;
			}
		}

	}
	else if(dalekArkStages.stage3) {
		newVals[0] = calc_ark(models[3], newVals)[0];
		newVals[1] = calc_ark(models[3], newVals)[1];
		xAdd = newVals[0] - arkStartPos[0].z;
		yAdd = arkStartPos[0].y - newVals[1];
		models[3]->positions[0].z = xAdd;
		models[3]->positions[0].y = yAdd;
		models[3]->rotations[0].x += 1.0 * updateTime;

		for (int i = 0; i < 5; i++) {
			
			
			if ((i + 1) < 3) {

				models[3]->positions[i + 1] = glm::vec3((models[3]->positions[0].x - 0.5) + (1 * i), models[3]->positions[0].y, models[3]->positions[0].z + 0.5);
			}
			if ((i + 1) >= 3) {

				models[3]->positions[i + 1] = glm::vec3((models[3]->positions[0].x - 1.0) + (1 * (i - 2)), models[3]->positions[0].y, models[3]->positions[0].z + 1.0);
			}
			models[3]->rotations[i + 1].x = models[3]->rotations[0].x;
			

			if (models[3]->positions[i].y <= 0.5f) {
				dalekArkStages.stage3 = false;
				dalekArkStages.stage4 = true;
			}

			
		}
		models[3]->arkAngle += 0.009 * updateTime;
	}
	else if (dalekArkStages.stage4) {
		for (int i = 0; i < models[3]->numModels - 6; i++) {
			models[3]->rotations[i].x += 1.0 * updateTime;
			models[3]->positions[i].z -= 2.0 * updateTime;
			int tempDeg = glm::degrees(models[3]->rotations[i].x);
			cout << tempDeg % 360 << endl;

			if ((tempDeg % 360) < 3) {
				models[3]->rotations[i].x = 0.0f;
				dalekArkStages.stage4 = false;
				dalekArkStages.stage5 = true;
			}
		}
	}
	else if (dalekArkStages.stage5) {
		
		for (int i = 0; i < models[3]->numModels - 6; i++) {
			models[3]->positions[i].z -= 0.8 * updateTime;
			models[3]->positions[i].y -= 0.1 * updateTime;

			if (models[3]->positions[i].y <= 0.1) {
				models[3]->arkAngle = 6.28319f;
				models[3]->positions[i].y = 0.1;
				dalekArkStages.stage5 = false;
				dalekArkStages.stage6 = true;
				patrolStages1.stage1 = true;		

			}
		}
	}
	else if (dalekArkStages.stage6) {
		dalekPatrol1(currentTime);
	}


}

/*animation function for first squad of daleks landing ark*/
void dalekArk2(GLfloat currentTime) {
	updateTime = currentTime - prevTime;
	float newVals[2];
	float xAdd = 0.0f;
	float yAdd = 0.0f;
	if (dalekArkStages2.stage1) {
		for (int i = 0; i < models[3]->numModels - 6; i++) {
			models[3]->positions[i + 6].z -= 1.0;
			models[3]->positions[i + 6].y += 20.5;
			arkStartPos[i + 6].x = models[3]->positions[i + 6].z;
			arkStartPos[i + 6].y = models[3]->positions[i + 6].y;
			models[3]->rotations[i + 6].x = 1.5708;
			dalekModel.arkRad = 4.5;

		}
		//arkStartPos = models[3]->positions;
		dalekArkStages2.stage1 = false;
		dalekArkStages2.stage2 = true;
	}
	else if (dalekArkStages2.stage2) {
		for (int i = 0; i < models[3]->numModels -6; i++) {
			if (models[3]->positions[i + 6].y <= 4.5) {
				arkStartPos[i + 6].x = models[3]->positions[i +6].z;
				arkStartPos[i + 6].y = models[3]->positions[i +6].y;
				dalekArkStages2.stage2 = false;
				dalekArkStages2.stage3 = true;
				cout << "stage 2 end" << endl;
			}
			if (dalekArkStages2.stage2) {
				
				models[3]->positions[i + 6].y -= 5.0 * updateTime;
			}
		}

	}
	else if (dalekArkStages2.stage3) {
		newVals[0] = calc_ark(models[3], newVals)[0];
		newVals[1] = calc_ark(models[3], newVals)[1];
		xAdd = arkStartPos[6].z + newVals[0];
		yAdd = arkStartPos[6].y - newVals[1];
		models[3]->positions[6].z = 0 + (0 - xAdd);
		models[3]->positions[6].y = yAdd;
		models[3]->rotations[6].x -= 1.0 * updateTime;

		for (int i = 0; i <5; i++) {


			if ((i + 1) < 3) {

				models[3]->positions[i + 7] = glm::vec3((models[3]->positions[6].x - 0.5) + (1 * i), models[3]->positions[6].y, models[3]->positions[6].z - 0.5);
			}
			if ((i + 1) >= 3) {

				models[3]->positions[i + 7] = glm::vec3((models[3]->positions[6].x - 1.0) + (1 * (i - 2)), models[3]->positions[6].y, models[3]->positions[6].z - 1.0);
			}
			models[3]->rotations[i + 7].x = models[3]->rotations[6].x;


			if (models[3]->positions[i + 7].y <= 0.5f) {
				dalekArkStages2.stage3 = false;
				dalekArkStages2.stage4 = true;
			}


		}
		models[3]->arkAngle += 0.03 * updateTime;
	}
	else if (dalekArkStages2.stage4) {
		for (int i = 0; i < models[3]->numModels - 6; i++) {
			models[3]->rotations[i + 6].x -= 1.0 * updateTime;
			models[3]->positions[i + 6].z += 2.0 * updateTime;
			int tempDeg = glm::degrees(models[3]->rotations[i + 6].x);
			cout << tempDeg % 360 << endl;

			if ((tempDeg % 360) < 3) {
				models[3]->rotations[i + 6].x = 0.0f;
				dalekArkStages2.stage4 = false;
				dalekArkStages2.stage5 = true;
			}
		}
	}
	else if (dalekArkStages2.stage5) {

		for (int i = 0; i < models[3]->numModels - 6; i++) {
			models[3]->positions[i + 6].z += 0.8 * updateTime;
			models[3]->positions[i + 6].y -= 0.1 * updateTime;

			if (models[3]->positions[i + 6].y <= 0.1) {
				models[3]->arkAngle = 6.28319f;
				models[3]->positions[i].y = 0.1;
				dalekArkStages2.stage5 = false;
				dalekArkStages2.stage6 = true;
				patrolStages2.stage1 = true;
				models[3]->rotations[6].y = 6.28319;
				

			}
		}
	}
	else if (dalekArkStages2.stage6) {
				
		dalekPatrol2(currentTime);
	}


}

/*places the doors in the correct postion*/
void placeDoors() 
{
	tardisDoorLeft.positions[0] = glm::vec3(tardisNoDoors.positions[0].x + 0.12, tardisModel.positions[0].y + 0.029f, tardisModel.positions[0].z - 0.125);
	tardisDoorRight.positions[0] = glm::vec3(tardisNoDoors.positions[0].x - 0.12, tardisModel.positions[0].y + 0.032f, tardisModel.positions[0].z - 0.125);
}

/*animation function for first squad of daleks patrolling*/
void dalekPatrol1(GLfloat currentTime) {
	updateTime = currentTime - prevTime;
	if (patrolStages1.stage1) {
		models[3]->positions[0].z -= 0.8 * updateTime;

		for (int i = 0; i < 5; i++) {

			if ((i + 1) < 3) {

				models[3]->positions[i + 1] = glm::vec3((models[3]->positions[0].x - 0.5) + (1 * i), models[3]->positions[0].y, models[3]->positions[0].z + 0.5);
			}
			if ((i + 1) >= 3) {

				models[3]->positions[i + 1] = glm::vec3((models[3]->positions[0].x - 1.0) + (1 * (i - 2)), models[3]->positions[0].y, models[3]->positions[0].z + 1.0);
			}
			
		}
		if (models[3]->positions[0].z <= -5.0)
		{
			patrolStages1.stage1 = false;
			patrolStages1.stage2 = true;
		}
	}
	else if (patrolStages1.stage2) {
		models[3]->rotations[0].y += 0.5 * updateTime;
		for (int i = 0; i < 5; i++) {

			if ((i + 1) < 3) {

				models[3]->rotations[i + 1].y = models[3]->rotations[0].y;
			}
			if ((i + 1) >= 3) {

				models[3]->rotations[i + 1].y = models[3]->rotations[0].y;
			}

		}

		if (int(glm::degrees(models[3]->rotations[0].y)) % 270 == 0) {
			patrolStages1.stage2 = false;
			patrolStages1.stage3 = true;
		}
	}else if (patrolStages1.stage3) {
		models[3]->positions[0].x -= 0.8 * updateTime;

		for (int i = 0; i < 5; i++) {

			if ((i + 1) < 3) {

				models[3]->positions[i + 1] = glm::vec3((models[3]->positions[0].x - 0.5) + (1 * i), models[3]->positions[0].y, models[3]->positions[0].z + 0.5);
			}
			if ((i + 1) >= 3) {

				models[3]->positions[i + 1] = glm::vec3((models[3]->positions[0].x - 1.0) + (1 * (i - 2)), models[3]->positions[0].y, models[3]->positions[0].z + 1.0);
			}

		}
		if (models[3]->positions[0].x <= -4.5)
		{
			patrolStages1.stage3 = false;
			patrolStages1.stage4 = true;
		}
	}
	else if (patrolStages1.stage4) {
		models[3]->rotations[0].y += 0.5 * updateTime;
		for (int i = 0; i < 5; i++) {

			if ((i + 1) < 3) {

				models[3]->rotations[i + 1].y = models[3]->rotations[0].y;
			}
			if ((i + 1) >= 3) {

				models[3]->rotations[i + 1].y = models[3]->rotations[0].y;
			}

		}

		if (int(glm::degrees(models[3]->rotations[0].y)) % 360 == 0) {
			patrolStages1.stage4 = false;
			patrolStages1.stage5 = true;
		}
	}
	else if (patrolStages1.stage5) {
		models[3]->positions[0].z += 0.8 * updateTime;

		for (int i = 0; i < 5; i++) {

			if ((i + 1) < 3) {

				models[3]->positions[i + 1] = glm::vec3((models[3]->positions[0].x - 0.5) + (1 * i), models[3]->positions[0].y, models[3]->positions[0].z + 0.5);
			}
			if ((i + 1) >= 3) {

				models[3]->positions[i + 1] = glm::vec3((models[3]->positions[0].x - 1.0) + (1 * (i - 2)), models[3]->positions[0].y, models[3]->positions[0].z + 1.0);
			}

		}
		if (models[3]->positions[0].z >= 3.5)
		{
			patrolStages1.stage5 = false;
			patrolStages1.stage6 = true;
		}
	}else if (patrolStages1.stage6) {
		models[3]->rotations[0].y += 0.5 * updateTime;
		for (int i = 0; i < 5; i++) {

			if ((i + 1) < 3) {

				models[3]->rotations[i + 1].y = models[3]->rotations[0].y;
			}
			if ((i + 1) >= 3) {

				models[3]->rotations[i + 1].y = models[3]->rotations[0].y;
			}

		}

		if (int(glm::degrees(models[3]->rotations[0].y)) % 450 == 0) {
			patrolStages1.stage6 = false;
			patrolStages1.stage7 = true;
		}
	}
	else if (patrolStages1.stage7) {
		models[3]->positions[0].x += 0.8 * updateTime;

		for (int i = 0; i < 5; i++) {

			if ((i + 1) < 3) {

				models[3]->positions[i + 1] = glm::vec3((models[3]->positions[0].x - 0.5) + (1 * i), models[3]->positions[0].y, models[3]->positions[0].z + 0.5);
			}
			if ((i + 1) >= 3) {

				models[3]->positions[i + 1] = glm::vec3((models[3]->positions[0].x - 1.0) + (1 * (i - 2)), models[3]->positions[0].y, models[3]->positions[0].z + 1.0);
			}

		}
		if (models[3]->positions[0].x >= 4.2)
		{
			patrolStages1.stage7 = false;
			patrolStages1.stage8 = true;
		}
	}
	else if (patrolStages1.stage8) {
		models[3]->rotations[0].y += 0.5 * updateTime;
		for (int i = 0; i < 5; i++) {

			if ((i + 1) < 3) {

				models[3]->rotations[i + 1].y = models[3]->rotations[0].y;
			}
			if ((i + 1) >= 3) {

				models[3]->rotations[i + 1].y = models[3]->rotations[0].y;
			}

		}

		if (int(glm::degrees(models[3]->rotations[0].y)) % 180 == 0) {
			patrolStages1.stage8 = false;
			patrolStages1.stage1 = true;
			models[3]->rotations[0].y = 3.14159;
		}
	}

}

/*animation function for second squad of daleks patrolling*/
void dalekPatrol2(GLfloat currentTime) {
	updateTime = currentTime - prevTime;
	if (patrolStages2.stage1) {
		models[3]->positions[6].z += 0.8 * updateTime;

		for (int i = 0; i < 5; i++) {

			if ((i + 1) < 3) {

				models[3]->positions[i + 7] = glm::vec3((models[3]->positions[6].x - 0.5) + (1 * i), models[3]->positions[6].y, models[3]->positions[6].z - 0.5);
			}
			if ((i + 1) >= 3) {

				models[3]->positions[i + 7] = glm::vec3((models[3]->positions[6].x - 1.0) + (1 * (i - 2)), models[3]->positions[6].y, models[3]->positions[6].z - 1.0);
			}

		}
		if (models[3]->positions[6].z >= 4.0)
		{
			cout << "true" << endl;
			patrolStages2.stage1 = false;
			patrolStages2.stage2 = true;
		}
	}
	else if (patrolStages2.stage2) {
		models[3]->rotations[6].y += 0.5 * updateTime;
		for (int i = 0; i < 5; i++) {

			if ((i + 1) < 3) {

				models[3]->rotations[i + 7].y = models[3]->rotations[6].y;
			}
			if ((i + 1) >= 3) {

				models[3]->rotations[i + 7].y = models[3]->rotations[6].y;
			}

		}
		if (int(glm::degrees(models[3]->rotations[6].y)) % 450 == 0) {
			
			patrolStages2.stage2 = false;
			patrolStages2.stage3 = true;
		}
	}
	else if (patrolStages2.stage3) {
		models[3]->positions[6].x += 0.8 * updateTime;

		for (int i = 0; i < 5; i++) {

			if ((i + 1) < 3) {

				models[3]->positions[i + 7] = glm::vec3((models[3]->positions[6].x - 0.5) + (1 * i), models[3]->positions[6].y, models[3]->positions[6].z - 0.5);
			}
			if ((i + 1) >= 3) {

				models[3]->positions[i + 7] = glm::vec3((models[3]->positions[6].x - 1.0) + (1 * (i - 2)), models[3]->positions[6].y, models[3]->positions[6].z - 1.0);
			}

		}
		if (models[3]->positions[6].x >= 4.0)
		{
			cout << "true" << endl;
			patrolStages2.stage3 = false;
			patrolStages2.stage4 = true;
			
		}
	}
	else if (patrolStages2.stage4) {
		models[3]->rotations[6].y += 0.5 * updateTime;
		for (int i = 0; i < 5; i++) {

			if ((i + 1) < 3) {

				models[3]->rotations[i + 7].y = models[3]->rotations[6].y;
			}
			if ((i + 1) >= 3) {

				models[3]->rotations[i + 7].y = models[3]->rotations[6].y;
			}

		}
		if (int(glm::degrees(models[3]->rotations[6].y)) % 180 == 0) {

			patrolStages2.stage4 = false;
			patrolStages2.stage5 = true;
			models[3]->rotations[6].y = 3.14159;
		}
	}
	else if (patrolStages2.stage5) {
		
		models[3]->positions[6].z -= 0.8 * updateTime;

		for (int i = 0; i < 5; i++) {

			if ((i + 1) < 3) {

				models[3]->positions[i + 7] = glm::vec3((models[3]->positions[6].x - 0.5) + (1 * i), models[3]->positions[6].y, models[3]->positions[6].z - 0.5);
			}
			if ((i + 1) >= 3) {

				models[3]->positions[i + 7] = glm::vec3((models[3]->positions[6].x - 1.0) + (1 * (i - 2)), models[3]->positions[6].y, models[3]->positions[6].z - 1.0);
			}

		}
		if (models[3]->positions[6].z <= -4.5)
		{
			cout << "true" << endl;
			patrolStages2.stage5 = false;
			patrolStages2.stage6 = true;

		}
		
	}
	else if (patrolStages2.stage6) {
		models[3]->rotations[6].y += 0.5 * updateTime;
		for (int i = 0; i < 5; i++) {

			if ((i + 1) < 3) {

				models[3]->rotations[i + 7].y = models[3]->rotations[6].y;
			}
			if ((i + 1) >= 3) {

				models[3]->rotations[i + 7].y = models[3]->rotations[6].y;
			}

		}
		if (int(glm::degrees(models[3]->rotations[6].y)) % 270 == 0) {

			patrolStages2.stage6 = false;
			patrolStages2.stage7 = true;
		}
	}
	else if (patrolStages2.stage7) {
		models[3]->positions[6].x -= 0.8 * updateTime;

		for (int i = 0; i < 5; i++) {

			if ((i + 1) < 3) {

				models[3]->positions[i + 7] = glm::vec3((models[3]->positions[6].x - 0.5) + (1 * i), models[3]->positions[6].y, models[3]->positions[6].z - 0.5);
			}
			if ((i + 1) >= 3) {

				models[3]->positions[i + 7] = glm::vec3((models[3]->positions[6].x - 1.0) + (1 * (i - 2)), models[3]->positions[6].y, models[3]->positions[6].z - 1.0);
			}

		}
		if (models[3]->positions[6].x <= -4.5)
		{
			cout << "true" << endl;
			patrolStages2.stage7 = false;
			patrolStages2.stage8 = true;

		}
	}
	else if (patrolStages2.stage8) {
		models[3]->rotations[6].y += 0.5 * updateTime;
		for (int i = 0; i < 5; i++) {

			if ((i + 1) < 3) {

				models[3]->rotations[i + 7].y = models[3]->rotations[6].y;
			}
			if ((i + 1) >= 3) {

				models[3]->rotations[i + 7].y = models[3]->rotations[6].y;
			}

		}
		if (int(glm::degrees(models[3]->rotations[6].y)) % 360 == 0) {

			patrolStages2.stage8 = false;
			patrolStages2.stage1 = true;
			models[3]->rotations[6].y = 6.28319;
		}
	}

}

/*animation function for opening doors of the tardis*/
void openDoors(GLfloat currentTime) {
	updateTime = currentTime - prevTime;
	if (open) {
		if (tardisDoorLeft.rotations[0].y <= 4.537) {
			tardisDoorLeft.rotations[0].y += 5.0 * updateTime;
		}
		if (tardisDoorRight.rotations[0].y >= -4.53) {
			tardisDoorRight.rotations[0].y -= 5.0 * updateTime;
		}
	}
}

/*animation function for opening doors of the tardis*/
void closeDoors(GLfloat currentTime) {
	updateTime = currentTime - prevTime;
	if (close) {
		if (tardisDoorLeft.rotations[0].y >= 3.14159) {
			tardisDoorLeft.rotations[0].y -= 5.0 * updateTime;
		}
		if (tardisDoorRight.rotations[0].y <= -3.14159) {
			tardisDoorRight.rotations[0].y += 5.0 * updateTime;
		}
	}
}

/*locks camera to a dalek*/
void lockCamera() {
	cameraPosition.y = models[3]->positions[0].y + 0.5;
	cameraPosition.x = models[3]->positions[0].x;
	cameraPosition.z = models[3]->positions[0].z + 5;

	cameraFront.y = 0;
	cameraFront.x = 0;
	cameraFront.z = -1;
}





