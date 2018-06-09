/*
CPE/CSC 474 Lab base code Eckhardt/Dahl
based on CPE/CSC 471 Lab base code Wood/Dunn/Eckhardt
*/

#include <iostream>
#include <glad/glad.h>
#include <windows.h>
#include <mmsystem.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"

#include "WindowManager.h"
#include "Shape.h"
#include "Line.h"
#include "bone.h"
#include "Character.h"
#include "Team.h"
#include "Board.h"

//text stuff

#define RUN_ANIMATION 0
#define AXE_SWING_ANIMATION 1
#define AXE_UNSHEATHE_ANIMATION 2
#define DODGE_ANIMATION 3

using namespace std;
using namespace glm;

shared_ptr<Shape> shape;
shared_ptr<Shape> plane;
shared_ptr<Shape> brick;

//temp vars to move char
static double moveCharX = 0;
static double moveCharY = 0;

//current team
static int activeTeam;
static int turnNumber;
static int teamEndTurn;//check if you want to end the turn, 1 for true end turn, 0 for false
static int teamSumMoves;

mat4 linint_between_two_orientations(vec3 ez_aka_lookto_1, vec3 ey_aka_up_1, vec3 ez_aka_lookto_2, vec3 ey_aka_up_2, float t);


//*************************************************************************************************
//
//			TEST YOUR ANIMATION
//
//*************************************************************************************************
mat4 test_animation()
	{
	static float t = 0.0;
	t += 0.01;
	//float f = (sin(t)+1.)/2.0;//
	mat4 mt;
	vec3 ez1, ey1, ez2, ey2, ez3, ey3, ez4, ey4;

	ez1 = vec3(0, 0, 1);
	ey1 = vec3(0, 1, 0);

	ez2 = vec3(0.157114, -0.0207484, 0.987362);
	ey2 = vec3(-0.978864, 0.129268, 0.158478);

	ez3 = vec3(0.799965, -0.0758681, 0.595231);
	ey3 = vec3(-0.356247, 0.73818, 0.572869);

	ez4 = vec3(0.883246, -0.0758681, -0.462732);
	ey4 = vec3(0.361728, 0.73818, 0.569423);



	if (t <= 1)					mt = linint_between_two_orientations(ez1, ey1, ez2, ey2, t);
	else if (t <= 2)			mt = linint_between_two_orientations(ez2, ey2, ez3, ey3, t - 1.);
	else if (t <= 3)			mt = linint_between_two_orientations(ez3, ey3, ez4, ey4, t - 2.);
	else if (t <= 4)			mt = linint_between_two_orientations(ez4, ey4, ez1, ey1, t - 3.);
	else
		{
		t = 0;
		mt = linint_between_two_orientations(ez1, ey1, ez2, ey2, t);
		}


	return mt;
	}
//*************************************************************************************************
//
//			END - TEST YOUR ANIMATION
//
//*************************************************************************************************

mat4 linint_between_two_orientations(vec3 ez_aka_lookto_1, vec3 ey_aka_up_1, vec3 ez_aka_lookto_2, vec3 ey_aka_up_2, float t)
	{
	mat4 m1, m2;
	quat q1, q2;
	vec3 ex, ey, ez;
	ey = ey_aka_up_1;
	ez = ez_aka_lookto_1;
	ex = cross(ey, ez);
	m1[0][0] = ex.x;		m1[0][1] = ex.y;		m1[0][2] = ex.z;		m1[0][3] = 0;
	m1[1][0] = ey.x;		m1[1][1] = ey.y;		m1[1][2] = ey.z;		m1[1][3] = 0;
	m1[2][0] = ez.x;		m1[2][1] = ez.y;		m1[2][2] = ez.z;		m1[2][3] = 0;
	m1[3][0] = 0;			m1[3][1] = 0;			m1[3][2] = 0;			m1[3][3] = 1.0f;
	ey = ey_aka_up_2;
	ez = ez_aka_lookto_2;
	ex = cross(ey, ez);
	m2[0][0] = ex.x;		m2[0][1] = ex.y;		m2[0][2] = ex.z;		m2[0][3] = 0;
	m2[1][0] = ey.x;		m2[1][1] = ey.y;		m2[1][2] = ey.z;		m2[1][3] = 0;
	m2[2][0] = ez.x;		m2[2][1] = ez.y;		m2[2][2] = ez.z;		m2[2][3] = 0;
	m2[3][0] = 0;			m2[3][1] = 0;			m2[3][2] = 0;			m2[3][3] = 1.0f;
	q1 = quat(m1);
	q2 = quat(m2);
	quat qt = slerp(q1, q2, t); //<---
	qt = normalize(qt);
	mat4 mt = mat4(qt);
	//mt = transpose(mt);		 //<---
	return mt;
	}




double get_last_elapsed_time()
{
    static double lasttime = glfwGetTime();
    double actualtime = glfwGetTime();
    double difference = actualtime - lasttime;
    lasttime = actualtime;
    return difference;
}
class camera
{
public:
    glm::vec3 pos, rot;
    int w, a, s, d;
    camera()
    {
        w = a = s = d = 0;
        pos = rot = glm::vec3(0, 0, 0);
    }
    glm::mat4 process(double ftime)
    {
        float speed = 0;
        if (w == 1)
        {
            speed = 10 * ftime;
        }
        else if (s == 1)
        {
            speed = -10 * ftime;
        }
        float yangle = 0;
        if (a == 1)
            yangle = -3 * ftime;
        else if (d == 1)
            yangle = 3 * ftime;
        rot.y += yangle;
        glm::mat4 R = glm::rotate(glm::mat4(1), rot.y, glm::vec3(0, 1, 0));
        glm::vec4 dir = glm::vec4(0, 0, speed, 1);
        dir = dir * R;
        pos += glm::vec3(dir.x, dir.y, dir.z);
        glm::mat4 T = glm::translate(glm::mat4(1), pos);
		glm::mat4 Rx = glm::rotate(glm::mat4(1), rot.x, glm::vec3(1, 0, 0)); //Tilt the camera on x-axis based on rot.x, set in initGeom
        return Rx * R * T;
    }
};

camera mycam;
Board board;
static Character* activeUnit; //current selected unit, global
vector<Character *> Nteam1, Nteam2;
class Application : public EventCallbacks
{

public:

	WindowManager * windowManager = nullptr;

	// Our shader program
	std::shared_ptr<Program> bonesprog, psky, pplane, bricks, billboards, swordUnits, spearUnits, axeUnits, magicUnits;

	// Contains vertex information for OpenGL
	GLuint VertexArrayID, BillboardVAOID;

	// Data necessary to give our box to OpenGL
	GLuint VertexBufferID, VertexBufferIDimat, VertexNormDBox, VertexTexBox, IndexBufferIDBox;
    GLuint BillboardVertexBufferID, BillboardNormBufferID, BillboardTexBufferID, BillboardIndexBufferID;
	GLuint swrdVertexBufferID, swrdNormBufferID, swrdTexBufferID, swrdIndexBufferID;

	//texture data
    GLuint Texture, Texture1, Texture2, Texture3, bgmountains1;
	GLuint TexHector, TexMarth;
	GLuint swrdTex, spearTex, axeTex, magicTex;
	GLuint guiTeam1Tex, guiTeam2Tex; //Gui textures
	//line
	Line linerender;
	Line smoothrender;
	vector<vec3> line;
	
	//animation matrices:
	mat4 animmat[200];
	int animmatsize=0;

	//Current camera pos 
	int curcamPos = 0; //init to 1 so first keypress works, if set to 0 2x key presses to work
	bool zoomCam = FALSE;

	//animated hud variables
	int boolTeamHUD;
	float animateHudTeam1, animateHudTeam2;
    

	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		
		if (key == GLFW_KEY_W && action == GLFW_PRESS)
		{
			mycam.w = 1;
		}
		if (key == GLFW_KEY_W && action == GLFW_RELEASE)
		{
			mycam.w = 0;
		}
		if (key == GLFW_KEY_S && action == GLFW_PRESS)
		{
			mycam.s = 1;
		}
		if (key == GLFW_KEY_S && action == GLFW_RELEASE)
		{
			mycam.s = 0;
		}
		if (key == GLFW_KEY_A && action == GLFW_PRESS)
		{
			mycam.a = 1;
		}
		if (key == GLFW_KEY_A && action == GLFW_RELEASE)
		{
			mycam.a = 0;
		}
		if (key == GLFW_KEY_D && action == GLFW_PRESS)
		{
			mycam.d = 1;
		}
		if (key == GLFW_KEY_D && action == GLFW_RELEASE)
		{
			mycam.d = 0;
		}
		//Active Unit Movement
		if (key == GLFW_KEY_UP && action == GLFW_PRESS)
		{
			// only move charaters if they are isCharacter = 1 otherwise regard them as gameplay erased from gameboard
			if (activeUnit[0].moves > 0)
			{
				activeUnit[0].moves -= 1;
				activeUnit[0].position.z -= 1;
			}
			//activeUnit[0].position.z -= 1;

		}
		if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
		{	
			if (activeUnit[0].moves > 0)
			{
				activeUnit[0].moves -= 1;
				activeUnit[0].position.z += 1;
			}
		}
		if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
		{
			if (activeUnit[0].moves > 0)
			{
				activeUnit[0].moves -= 1;
				activeUnit[0].position.x -= 1;
			}
		}
		if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
		{
			if (activeUnit[0].moves > 0)
			{
				activeUnit[0].moves -= 1;
				activeUnit[0].position.x += 1; 
			}
		}
		//Active Unit Selection
		if (key == GLFW_KEY_1 && action == GLFW_PRESS)
		{
			if (activeTeam == 1)
			{
				activeUnit = Nteam1.at(0);
				//upload the current activeUnit to the shader, need to find an efficent way to do this
				//glUniform1i(spearUnits->getUniform("activeUnit"), (*activeUnit).team);

				//activeUnit = &board.characters[0];
			}
			else if (activeTeam == 2)
			{
				activeUnit = Nteam2.at(0);
				//activeUnit = &board.characters[0+4];
			}
		}
		if (key == GLFW_KEY_2 && action == GLFW_PRESS)
		{
			if (activeTeam == 1)
			{
				activeUnit = Nteam1.at(1);
				//activeUnit = &board.characters[1];
			}
			else if (activeTeam == 2)
			{
				activeUnit = Nteam2.at(1);
				//activeUnit = &board.characters[1 + 4];
			}
		}
		if (key == GLFW_KEY_3 && action == GLFW_PRESS)
		{
			if (activeTeam == 1)
			{
				activeUnit = Nteam1.at(2);
				//activeUnit = &board.characters[2];
			}
			else if (activeTeam == 2)
			{
				activeUnit = Nteam2.at(2);
				//activeUnit = &board.characters[2 + 4];
			}
		}
		if (key == GLFW_KEY_4 && action == GLFW_PRESS)
		{
			if (activeTeam == 1)
			{
				activeUnit = Nteam1.at(3);
				//activeUnit = &board.characters[3];
			}
			else if (activeTeam == 2)
			{
				activeUnit = Nteam2.at(3);
				//activeUnit = &board.characters[3 + 4];
			}
		}
		//end current team's turn
		if (key == GLFW_KEY_M && action == GLFW_PRESS) //update the global so the turn is changed
		{
			
			if (teamEndTurn == 0)
			{
				teamEndTurn = 1; //turn the boolean int to true
			}
		}
		//switch teams
		if (key == GLFW_KEY_L && action == GLFW_PRESS)
		{
			if (activeTeam == 1)
			{
				activeTeam = 2;
			}
			else if (activeTeam == 2)
			{
				activeTeam = 1;
			}
		}
		
		if (key == GLFW_KEY_R && action == GLFW_PRESS)  // switch the camera position
		{
			curcamPos = 0;
			moveCameraScene();
		}
		if (key == GLFW_KEY_F && action == GLFW_PRESS)  // switch the camera position
		{
			curcamPos = 1;
			moveCameraScene();
		}
		if (key == GLFW_KEY_T && action == GLFW_RELEASE)
		{
			zoomCam = TRUE;
			////Make the overhead cam pan in
			//if (curcamPos == 0)
			//{
			//	double zoom = 0;
			//	//overhead orientation
			//	//Call func to update camera pos in render
			//	for (zoom = 0; zoom < 5; zoom += 1)
			//	{
			//		cout << zoom;
			//		mycam.pos = glm::vec3(-0.75, -10 , -9) + vec3(0, zoom ,0);
			//	}
			//	//mycam.pos = glm::vec3(-0.75, -10, -9);
			//	mycam.rot.x = 1; // Camera orientaion, 1 will look nearly straight down
			//	mycam.rot.y = 0;
			//	//curcamPos = 1; //switch for next press
			//}
			//else
			//{
			//	//nothing
			//}
		}

	
		
		if (key == GLFW_KEY_C && action == GLFW_RELEASE)
		{
			if (smoothrender.is_active())
				smoothrender.reset();
			else
				{
				vector<vec3> cardinal;
				cardinal_curve(cardinal, line, 5, 1.0);
				smoothrender.re_init_line(cardinal);
				}
		}
	}

	// callback for the mouse when clicked move the triangle when helper functions
	// written
	void mouseCallback(GLFWwindow *window, int button, int action, int mods)
	{
		double posX, posY;
		float newPt[2];
		if (action == GLFW_PRESS)
		{
			glfwGetCursorPos(window, &posX, &posY);
			std::cout << "Pos X " << posX <<  " Pos Y " << posY << std::endl;

			//change this to be the points converted to WORLD
			//THIS IS BROKEN< YOU GET TO FIX IT - yay!
			newPt[0] = 0;
			newPt[1] = 0;

			std::cout << "converted:" << newPt[0] << " " << newPt[1] << std::endl;
			glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID);
			//update the vertex array with the updated points
			glBufferSubData(GL_ARRAY_BUFFER, sizeof(float)*6, sizeof(float)*2, newPt);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
	}

	//if the window is resized, capture the new size and reset the viewport
	void resizeCallback(GLFWwindow *window, int in_width, int in_height)
	{
		//get the window size - may be different then pixels for retina
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
	}

	/*Note that any gl calls must always happen after a GL state is initialized */
	bone *root = NULL;
	int size_stick = 0;
	all_animations all_animation;
	void initGeom(const std::string& resourceDirectory)
	{
		//Game Logic Stuff///
		activeTeam = 1;
		turnNumber = 0; //
		teamEndTurn = 0; //set the initial turn not to end

		//// Hud stuff ////
		animateHudTeam1 = 0;
		animateHudTeam2 = 0;
		boolTeamHUD = 0;

		//// Geometery Stuff ////
        brick = make_shared<Shape>();
        brick->loadMesh(resourceDirectory + "/cube2.obj");
        brick->resize();
        brick->init();

		for (int ii = 0; ii < 200; ii++)
			animmat[ii] = mat4(1);
		
		//readtobone("test.fbx",&all_animation,&root);  // old load 
		readtobone("fbxAnimations/run_Char00.fbx", &all_animation, &root); //82 frames
		readtobone("fbxAnimations/axeSwing_1Char00.fbx", &all_animation, NULL);  //92 frames
		readtobone("fbxAnimations/axeUnsheatheChar00.fbx", &all_animation, NULL);  
		readtobone("fbxAnimations/dodgeChar00.fbx", &all_animation, NULL);  
		
		root->set_animations(&all_animation,animmat,animmatsize);

		cout << "root name " << root->name << endl;

		// Initialize the Camera Position and orientation
		moveCameraScene();
		//mycam.pos = glm::vec3(-0.75, -10, -9); //Set initial Cam pos cented above the map plane
		//mycam.rot.x = 1; //rotate the camera to look down at the map plane
		
			
		// Initialize mesh.
		shape = make_shared<Shape>();
		shape->loadMesh(resourceDirectory + "/skybox.obj");
		shape->resize();
		shape->init();

		plane = make_shared<Shape>();
		plane->loadMesh(resourceDirectory + "/FA18.obj");
		plane->resize();
		plane->init();

		//generate the VAO
		glGenVertexArrays(1, &VertexArrayID);
		glBindVertexArray(VertexArrayID);

		//generate vertex buffer to hand off to OGL
		glGenBuffers(1, &VertexBufferID);
		//set the current state to focus on our vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID);
		
		vector<vec3> pos;
		vector<unsigned int> imat;
		root->write_to_VBOs(vec3(0, 0, 0), pos, imat);
		size_stick = pos.size();
		//actually memcopy the data - only do this once
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)*pos.size(), pos.data(), GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		//indices of matrix:
		glGenBuffers(1, &VertexBufferIDimat);
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferIDimat);
		glBufferData(GL_ARRAY_BUFFER, sizeof(uint)*imat.size(), imat.data(), GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, 0, (void*)0);

        // billboard VAO and VBO
        glGenVertexArrays(1, &BillboardVAOID);
        glBindVertexArray(BillboardVAOID);

        //generate vertex buffer to hand off to OGL
        glGenBuffers(1, &BillboardVertexBufferID);
        //set the current state to focus on our vertex buffer
        glBindBuffer(GL_ARRAY_BUFFER, BillboardVertexBufferID);

        GLfloat cube_vertices[] = {
            // front
            -1.0, 0.0,  1.0,//LD
            1.0, 0.0,  1.0,//RD
            1.0,  2.0,  1.0,//RU
            -1.0,  2.0,  1.0,//LU
        };
        //make it a bit smaller
        for (int i = 0; i < 12; i++)
            cube_vertices[i] *= 0.5;
        //actually memcopy the data - only do this once
        glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_DYNAMIC_DRAW);

        //we need to set up the vertex array
        glEnableVertexAttribArray(0);
        //key function to get up how many elements to pull out at a time (3)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	
        //color
        GLfloat cube_norm[] = {
            // front colors
            0.0, 0.0, 1.0,
            0.0, 0.0, 1.0,
            0.0, 0.0, 1.0,
            0.0, 0.0, 1.0,

        };
        glGenBuffers(1, &BillboardNormBufferID);
        //set the current state to focus on our vertex buffer
        glBindBuffer(GL_ARRAY_BUFFER, BillboardNormBufferID);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cube_norm), cube_norm, GL_STATIC_DRAW);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        //color
        glm::vec2 cube_tex[] = {
            // front colors
            glm::vec2(0.0, 1.0),
            glm::vec2(1.0, 1.0),
            glm::vec2(1.0, 0.0),
            glm::vec2(0.0, 0.0),

        };
        glGenBuffers(1, &BillboardTexBufferID);
        //set the current state to focus on our vertex buffer
        glBindBuffer(GL_ARRAY_BUFFER, BillboardTexBufferID);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cube_tex), cube_tex, GL_STATIC_DRAW);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glGenBuffers(1, &BillboardIndexBufferID);
        //set the current state to focus on our vertex buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BillboardIndexBufferID);
        GLushort cube_elements[] = {

            // front
            0, 1, 2,
            2, 3, 0,
        };
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_elements), cube_elements, GL_STATIC_DRAW);
        glBindVertexArray(0);


		int width, height, channels;
		char filepath[1000];

		//-- ANIMATED SPRITE TEXTURES --//
		//sword units
		string str = resourceDirectory + "/swordMaster-spritesheet.png"; // actually get the first sprite texture
		strcpy(filepath, str.c_str());
		unsigned char* data = stbi_load(filepath, &width, &height, &channels, 4);
		glGenTextures(1, &swrdTex);
		//glGenTextures(1, &swrdTex2);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, swrdTex);
		//glBindTexture(GL_TEXTURE_2D, swrdTex2);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // changed from GL_MIRRORED_REPEAT
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST); //Use nearest_nearest or linear_nearest
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);


		//[TWOTEXTURES] sword units
		//set the 2 textures to the correct samplers in the fragment shader:
		GLuint Tex1Location = glGetUniformLocation(swordUnits->pid, "tex");//tex, tex2... sampler in the fragment shader
		GLuint Tex2Location = glGetUniformLocation(swordUnits->pid, "tex2");
		// Then bind the uniform samplers to texture units:
		glUseProgram(swordUnits->pid);
		glUniform1i(Tex1Location, 0);
		glUniform1i(Tex2Location, 1);
		

		//spear units
		str = resourceDirectory + "/tempLancer.png"; // actually get the first sprite texture
		strcpy(filepath, str.c_str());
		data = stbi_load(filepath, &width, &height, &channels, 4);
		glGenTextures(1, &spearTex);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, spearTex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST); //Use nearest_nearest or linear_nearest
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		//[TWOTEXTURES] sword units
		//set the 2 textures to the correct samplers in the fragment shader:
		Tex1Location = glGetUniformLocation(spearUnits->pid, "tex");//tex, tex2... sampler in the fragment shader
		Tex2Location = glGetUniformLocation(spearUnits->pid, "tex2");
		// Then bind the uniform samplers to texture units:
		glUseProgram(spearUnits->pid);
		glUniform1i(Tex1Location, 0);
		glUniform1i(Tex2Location, 1);


		//Axe Units
		str = resourceDirectory + "/tempAxe.png"; // actually get the first sprite texture
		strcpy(filepath, str.c_str());
		data = stbi_load(filepath, &width, &height, &channels, 4);
		glGenTextures(1, &axeTex);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, axeTex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST); //Use nearest_nearest or linear_nearest
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		//[TWOTEXTURES] axe units
		//set the 2 textures to the correct samplers in the fragment shader:
		Tex1Location = glGetUniformLocation(axeUnits->pid, "tex");//tex, tex2... sampler in the fragment shader
		Tex2Location = glGetUniformLocation(axeUnits->pid, "tex2");
		// Then bind the uniform samplers to texture units:
		glUseProgram(axeUnits->pid);
		glUniform1i(Tex1Location, 0);
		glUniform1i(Tex2Location, 1);

		//Magic Units
		str = resourceDirectory + "/tempMage.png"; // actually get the first sprite texture
		strcpy(filepath, str.c_str());
		data = stbi_load(filepath, &width, &height, &channels, 4);
		glGenTextures(1, &magicTex);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, magicTex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST); //Use nearest_nearest or linear_nearest
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		//[TWOTEXTURES] magic units
		//set the 2 textures to the correct samplers in the fragment shader:
		Tex1Location = glGetUniformLocation(magicUnits->pid, "tex");//tex, tex2... sampler in the fragment shader
		Tex2Location = glGetUniformLocation(magicUnits->pid, "tex2");
		// Then bind the uniform samplers to texture units:
		glUseProgram(magicUnits->pid);
		glUniform1i(Tex1Location, 0);
		glUniform1i(Tex2Location, 1);


		//-- GUI and HUD Textures --//
		//team 1 turn
		str = resourceDirectory + "/Team1turn.png";
		strcpy(filepath, str.c_str());
		data = stbi_load(filepath, &width, &height, &channels, 4);
		glGenTextures(1, &guiTeam1Tex);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, guiTeam1Tex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST); //Use nearest_nearest or linear_nearest
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		//One Texture HUD
		//set the 2 textures to the correct samplers in the fragment shader:
		Tex1Location = glGetUniformLocation(billboards->pid, "tex");//tex, tex2... sampler in the fragment shader
		// Then bind the uniform samplers to texture units:
		glUseProgram(billboards->pid);
		glUniform1i(Tex1Location, 0);

		//team 2 turn 
		str = resourceDirectory + "/Team2turn.png";
		strcpy(filepath, str.c_str());
		data = stbi_load(filepath, &width, &height, &channels, 4);
		glGenTextures(1, &guiTeam2Tex);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, guiTeam2Tex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST); //Use nearest_nearest or linear_nearest
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		//One Texture HUD
		//set the 2 textures to the correct samplers in the fragment shader:
		Tex1Location = glGetUniformLocation(billboards->pid, "tex");//tex, tex2... sampler in the fragment shader
																	// Then bind the uniform samplers to texture units:
		glUseProgram(billboards->pid);
		glUniform1i(Tex1Location, 0);


		//OLD SPRITE TEXTURES
		//str = resourceDirectory + "/lyn.png"; // actually get the first sprite texture
		//strcpy(filepath, str.c_str());
		//data = stbi_load(filepath, &width, &height, &channels, 4);
		//glGenTextures(1, &Texture);
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, Texture);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		//glGenerateMipmap(GL_TEXTURE_2D);

  //      str = resourceDirectory + "/camilla.png"; // actually get the second sprite texture
  //      strcpy(filepath, str.c_str());
  //      data = stbi_load(filepath, &width, &height, &channels, 4);
  //      glGenTextures(1, &Texture1);
  //      glActiveTexture(GL_TEXTURE0);
  //      glBindTexture(GL_TEXTURE_2D, Texture1);
  //      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
  //      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
  //      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  //      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  //      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
  //      glGenerateMipmap(GL_TEXTURE_2D);

		//str = resourceDirectory + "/lahector.png"; // actually get the third sprite texture
		//strcpy(filepath, str.c_str());
		//data = stbi_load(filepath, &width, &height, &channels, 4);
		//glGenTextures(1, &TexHector);
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, TexHector);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST); //TEST DIFFERENT PARAMETER FOR HIGHER QUALITY SPRITE RENDERING
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		//glGenerateMipmap(GL_TEXTURE_2D);

		//str = resourceDirectory + "/marth.png"; // actually get the third sprite texture
		//strcpy(filepath, str.c_str());
		//data = stbi_load(filepath, &width, &height, &channels, 4);
		//glGenTextures(1, &TexMarth);
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, TexMarth);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		//glGenerateMipmap(GL_TEXTURE_2D);

		//texture 2
		str = resourceDirectory + "/skyBox.jpg"; 
		strcpy(filepath, str.c_str());
		data = stbi_load(filepath, &width, &height, &channels, 4);
		glGenTextures(1, &Texture2);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, Texture2);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

        //standard block texture
		str = resourceDirectory + "/TexTerrain/GrassTextureA.png";
        strcpy(filepath, str.c_str());
        data = stbi_load(filepath, &width, &height, &channels, 4);
        glGenTextures(1, &Texture3);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, Texture3);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

		//Mountains Texture
		str = resourceDirectory + "/BrokenTopThreeSistersV2Connor.png";
		strcpy(filepath, str.c_str());
		data = stbi_load(filepath, &width, &height, &channels, 4);
		glGenTextures(1, &bgmountains1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, bgmountains1);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST); //minifcation tex param
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); //magnification tex param
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		//Skeleton
		//[TWOTEXTURES]
		//set the 2 textures to the correct samplers in the fragment shader:
		Tex1Location = glGetUniformLocation(bonesprog->pid, "tex");//tex, tex2... sampler in the fragment shader
		Tex2Location = glGetUniformLocation(bonesprog->pid, "tex2");
        GLuint Tex3Location = glGetUniformLocation(bonesprog->pid, "bricks");
		// Then bind the uniform samplers to texture units:
		glUseProgram(bonesprog->pid);
		glUniform1i(Tex1Location, 0);
		glUniform1i(Tex2Location, 1);
        glUniform1i(Tex3Location, 1);
		smoothrender.init();
		linerender.init();
		line.push_back(vec3(0,0,-3));
		line.push_back(vec3(5,-1,-3));
		line.push_back(vec3(5,3,-8));
		line.push_back(vec3(2,2,-10));
		line.push_back(vec3(-3,-2,-15));
		line.push_back(vec3(0,1.5,-20));
		linerender.re_init_line(line);

        // Generate map foundation blocks
        int mapWidth = 10;
        int mapHeight = 10;
        vector<vec3> mapBlocks;
        vector<vector<Character>> charPos(mapWidth, vector<Character>(mapHeight));
        for (int i = 0; i < mapWidth; i++) {
            for (int j = 0; j < mapHeight; j++) {
                vec3 blockPos = vec3(i, 0, j) - vec3(mapWidth / 2, 0 , mapHeight / 2);
                mapBlocks.push_back(blockPos);
            }
        }
        
		//// SET UP THE CHARACTERS TO SEND TO THE GAME BOARD ////

		vector<vector<Character>> team1, team2;
        // team 1 test chars
        weapon curweapon = sword;
        vec3 default = vec3(0, 0, 0);
		//
		teamSumMoves = 100000; // just set this initial value so it doesnt switch turns on the first render loop, it gets recomputed on each render loop
		//NEW UNITS
		//Team 1
		charPos.at(0).at(0) = Character("Sword Lord T1", default, sword, false, 25, swrdTex, 1, 3);
		charPos.at(0).at(1) = Character("Spear Wielder T1", default, spear, false, 25, spearTex, 1, 3);
		charPos.at(0).at(2) = Character("Axe Master T1", default, axe, false, 25, axeTex, 1, 3);
		charPos.at(0).at(3) = Character("Mage Tactician T1", default, magic, false, 25, magicTex, 1, 2);

		//Team 2
		charPos.at(6).at(0) = Character("Sword Lord T2", default, sword, false, 25, swrdTex, 2, 3);
		charPos.at(6).at(1) = Character("Spear Wielder T2", default, spear, false, 25, spearTex, 2, 3);
		charPos.at(6).at(2) = Character("Axe Master T2", default, axe, false, 25, axeTex, 2, 3);
		charPos.at(6).at(3) = Character("Mage Tactician T2", default, magic, false, 25, magicTex, 2, 2);

		////OLD TEMP UNITS
  //      charPos.at(0).at(1) = Character("Lyn", default, spear, true, 20, Texture, 1);  // load the character spite textures
  //      curweapon = axe;
  //      charPos.at(0).at(2) = Character("Camilla", default, axe, false, 25, Texture1, 1);   // load the character spite textures
		//charPos.at(0).at(3) = Character("Hector", default, axe, false, 25, TexHector, 1); //test char3
		//charPos.at(0).at(4) = Character("Marth", default, axe, false, 25, TexMarth, 2); //test char4
		////team 2 test chars
		//charPos.at(6).at(4) = Character("Hector", default, axe, false, 25, TexHector, 1); //test char3
		//charPos.at(6).at(3) = Character("Camilla", default, axe, false, 25, Texture1, 2);   // load the character spite textures
		//charPos.at(6).at(2) = Character("Hector", default, axe, false, 25, TexHector, 2); //test char3
		//charPos.at(6).at(1) = Character("Marth", default, axe, false, 25, TexMarth, 2); //test char4

        // send to board
        board = Board(mapBlocks, team1, team2, charPos, mapWidth, mapHeight);        

        //cout << "CONVERT CHAR TO PHYSICAL COORDINATES\n";
        // call this to convert to map coordinates
        board.convertCharToPhysicalCoordinates();

		////Send to teams for active unit selection, doing this doesnt do anything ;(
		Nteam1.push_back(&board.characters[0]);
		Nteam1.push_back(&board.characters[1]);
		Nteam1.push_back(&board.characters[2]);
		Nteam1.push_back(&board.characters[3]);
		Nteam2.push_back(&board.characters[4]);
		Nteam2.push_back(&board.characters[5]);
		Nteam2.push_back(&board.characters[6]);
		Nteam2.push_back(&board.characters[7]);

        //PlaySound(TEXT("../resources/lostinthots.wav"), NULL, SND_FILENAME | SND_ASYNC);
        //PlaySound(TEXT("../resources/nge-thanatos.wav"), NULL, SND_FILENAME | SND_NOSTOP);
	}

	//General OGL initialization - set OGL state here
	void init(const std::string& resourceDirectory)
	{
		GLSL::checkVersion();

		// Set background color.
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

		
		// Enable z-buffer test.
		glEnable(GL_DEPTH_TEST);
		//glDisable(GL_DEPTH_TEST);
		// Initialize the GLSL program.
		bonesprog = std::make_shared<Program>();
		bonesprog->setVerbose(true);
		bonesprog->setShaderNames(resourceDirectory + "/skeleton_vertex.glsl", resourceDirectory + "/skeleton_fragment.glsl");
		if (!bonesprog->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		bonesprog->addUniform("P");
		bonesprog->addUniform("V");
		bonesprog->addUniform("M");
		bonesprog->addUniform("Manim");
		bonesprog->addUniform("campos");
		bonesprog->addAttribute("vertPos");
		bonesprog->addAttribute("vertimat");

		//// UNIT SHADERS ////
		//Sword Unit Sprite Shader
		swordUnits = std::make_shared<Program>();
		swordUnits->setVerbose(true);
		swordUnits->setShaderNames(resourceDirectory + "/5x7sprite_vertex.glsl", resourceDirectory + "/5x7sprite_fragment.glsl");
		if (!swordUnits->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		swordUnits->addUniform("P");
		swordUnits->addUniform("V");
		swordUnits->addUniform("M");
		//swrdlrd->addUniform("Manim");
		swordUnits->addUniform("campos");
		swordUnits->addAttribute("vertPos");
		swordUnits->addAttribute("vertNor");
		swordUnits->addAttribute("vertTex");
		//swrdlrd->addAttribute("vertimat");
		//add uniforms for interpolation
		swordUnits->addUniform("offset1");
		swordUnits->addUniform("offset2");
		swordUnits->addUniform("t");
		swordUnits->addUniform("team");
		swordUnits->addUniform("activeUnit");

		//spear unit shader //
		spearUnits = std::make_shared<Program>();
		spearUnits->setVerbose(true);
		spearUnits->setShaderNames(resourceDirectory + "/5x7sprite_vertex.glsl", resourceDirectory + "/5x7sprite_fragment.glsl");
		if (!spearUnits->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		spearUnits->addUniform("P");
		spearUnits->addUniform("V");
		spearUnits->addUniform("M");
		spearUnits->addUniform("campos");
		spearUnits->addAttribute("vertPos");
		spearUnits->addAttribute("vertNor");
		spearUnits->addAttribute("vertTex");
		//add uniforms for interpolation
		spearUnits->addUniform("offset1");
		spearUnits->addUniform("offset2");
		spearUnits->addUniform("t");
		spearUnits->addUniform("team");
		spearUnits->addUniform("activeUnit");

		//axe units shader
		axeUnits = std::make_shared<Program>();
		axeUnits->setVerbose(true);
		axeUnits->setShaderNames(resourceDirectory + "/5x7sprite_vertex.glsl", resourceDirectory + "/5x7sprite_fragment.glsl");
		if (!axeUnits->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		axeUnits->addUniform("P");
		axeUnits->addUniform("V");
		axeUnits->addUniform("M");
		axeUnits->addUniform("campos");
		axeUnits->addAttribute("vertPos");
		axeUnits->addAttribute("vertNor");
		axeUnits->addAttribute("vertTex");
		//add uniforms for interpolation
		axeUnits->addUniform("offset1");
		axeUnits->addUniform("offset2");
		axeUnits->addUniform("t");
		axeUnits->addUniform("team");
		axeUnits->addUniform("activeUnit");

		//magic units shader
		magicUnits = std::make_shared<Program>();
		magicUnits->setVerbose(true);
		magicUnits->setShaderNames(resourceDirectory + "/5x7sprite_vertex.glsl", resourceDirectory + "/5x7sprite_fragment.glsl");
		if (!magicUnits->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		magicUnits->addUniform("P");
		magicUnits->addUniform("V");
		magicUnits->addUniform("M");
		magicUnits->addUniform("campos");
		magicUnits->addAttribute("vertPos");
		magicUnits->addAttribute("vertNor");
		magicUnits->addAttribute("vertTex");
		//add uniforms for interpolation
		magicUnits->addUniform("offset1");
		magicUnits->addUniform("offset2");
		magicUnits->addUniform("t");
		magicUnits->addUniform("team");
		magicUnits->addUniform("activeUnit");


		// skybox shader //
		psky = std::make_shared<Program>();
		psky->setVerbose(true);
		psky->setShaderNames(resourceDirectory + "/skyvertex.glsl", resourceDirectory + "/skyfrag.glsl");
		if (!psky->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		psky->addUniform("P");
		psky->addUniform("V");
		psky->addUniform("M");
		psky->addUniform("campos");
		psky->addAttribute("vertPos");
		psky->addAttribute("vertNor");
		psky->addAttribute("vertTex");

		//unused?
		/*pplane = std::make_shared<Program>();
		pplane->setVerbose(true);
		pplane->setShaderNames(resourceDirectory + "/plane_vertex.glsl", resourceDirectory + "/plane_frag.glsl");
		if (!pplane->init())
			{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
			}
		pplane->addUniform("P");
		pplane->addUniform("V");
		pplane->addUniform("M");
		pplane->addUniform("campos");
		pplane->addAttribute("vertPos");
		pplane->addAttribute("vertNor");
		pplane->addAttribute("vertTex");*/

		//Terrain Shader
		bricks = std::make_shared<Program>();
		bricks->setVerbose(true);
		bricks->setShaderNames(resourceDirectory + "/brick_vertex.glsl", resourceDirectory + "/brick_fragment.glsl");
		bricks->init();
		bricks->addUniform("P");
		bricks->addUniform("V");
		bricks->addUniform("M");
		bricks->addUniform("camPos");
		bricks->addAttribute("vertPos");
		bricks->addAttribute("vertNor");
		bricks->addAttribute("vertTex");

		//Repurposed for background textures and animated elements such as damage sprites or moving background elements
		billboards = std::make_shared<Program>();
		billboards->setVerbose(true);
		billboards->setShaderNames(resourceDirectory + "/vert_billboard.glsl", resourceDirectory + "/fragment_billboard.glsl");
		if (!billboards->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		billboards->addUniform("P");
		billboards->addUniform("V");
		billboards->addUniform("M");
		billboards->addUniform("campos");
		billboards->addAttribute("vertPos"); //send vertice pos to the shader
		billboards->addAttribute("vertNor"); //send vertice norms to the shader
		billboards->addAttribute("vertTex"); //send vertice texture to the shader
	}

	double overheadZoomLevel = 0;
	void zoomCamera() //Pressing T will set a boolean so this gets called in the render function
	{

		if (curcamPos == 0) //only do zoom when in the overhead pos
		{
			overheadZoomLevel += .1;
			if (overheadZoomLevel < 5)
			{
				mycam.pos = glm::vec3(-0.75, -10, -9) + vec3(0, overheadZoomLevel, overheadZoomLevel / 2);
				//send something to the shader to make the scene darker
			}
			else
			{
				zoomCam = FALSE; //stop zooming
			}
			mycam.rot.x = 1; // Camera orientaion, 1 will look nearly straight down
			mycam.rot.y = 0;

		}

	}

	void updateHUDteams(int team, float frametime) //0 for off, 1 for team 1 hud, 2 for team 2 hud
	{
		if (team == 1)
		{
			if (animateHudTeam2 > -6) //move team 2 hud out 
			{
				animateHudTeam2 -= (1.7 * frametime); //incrementally move the hud
			}
			if (animateHudTeam1 < 3)
			{
				animateHudTeam1 += (1.7 * frametime ); //incrementally move the hud
			}
			else
			{
				boolTeamHUD = 0; //turn off the update when the billboard moves so far
			}
			
		}
		else if (team == 2)
		{
			cout << "HIT TEAM 2" << endl;
			if (animateHudTeam1 > -6) //move team one hud in
			{
				animateHudTeam1 -= (1.7 * frametime); //incrementally move the hud
			}
			if (animateHudTeam2 < 3) //make this team2
			{
				animateHudTeam2 += (1.7 * frametime); //incrementally move the hud
			}
			else
			{
				boolTeamHUD = 0; //turn off the update when the billboard moves so far
			}
		}

		
	}

	void moveCameraScene() //calling this function alters the current camera scene, same functionality as pressing R
	{
		//moving the camera to the overhead scene will cause the current team's # to fly on screen
		if (curcamPos == 0)
		{
			//overhead orientation
			//mycam.pos = glm::vec3(-0.75, -10, -9);
			mycam.pos = glm::vec3(-0.75, -8.5, -8);
			mycam.rot.x = 1; // Camera orientaion, 1 will look nearly straight down
			mycam.rot.y = 0;
			//curcamPos = 1; //switch for next press

			//update the team gui elements to fly in
			if (activeTeam == 1)
			{
				boolTeamHUD = 1;
			}
			else if (activeTeam == 2) //update team2's hud
			{
				boolTeamHUD = 2;
			}
			

		}
		if (curcamPos == 1)
		{
			//up close combat orientation
			mycam.pos = glm::vec3(6, -3, 11);
			mycam.rot.x = 0.3; // Camera orientaion
			mycam.rot.y = 1;
			//curcamPos = 0; //switch for next press, this line breaks the comabt animation playback FIX HOW CAMERA SCENE WORKS SO IT ONLY CATCHES IF AND DOESNT CHANGE THE VALUE, EXTERNAL
			//THINGS LIKE THE COMBAT STARTING AND COMBAT ENDING SHOULD CHANGE THE CURRENT CAM POS
		}
	}

	void updateTurn() //function that continously checks if all the 
	{	
		if (activeTeam == 1) //check if you want to end team 1's turn
		{
			//cout << "team 1 moves left: " << teamSumMoves << endl;
			//check if the sum characters movements have been exhausted, sum the characters movements and check if the sum == 0;
			if (teamEndTurn == 1) //check to see if the player has pressed the end turn keys
			{
				teamEndTurn = 0;
				activeTeam = 2;
				activeUnit = Nteam2.at(0); //change the active unit

				//reset the moves for all the new current team's units
				for (int ii = 0; ii < Nteam2.size(); ii++)
				{
					if ( (Nteam2.at(ii)[0]).weaponclass != magic)
					{
						(Nteam2.at(ii)[0]).moves = 3; //reset unit movements
					}
					else if ((Nteam2.at(ii)[0]).weaponclass == magic)
					{
						(Nteam2.at(ii)[0]).moves = 2; //reset unit movements
					}
				}

			}
			else if (teamSumMoves <= 0) // check if the sum of movements on this team is 0
			{
				activeTeam = 2;
				activeUnit = Nteam2.at(0); //change the active unit

				//reset the moves for all the new current team's units
				for (int ii = 0; ii < Nteam2.size(); ii++)
				{
					if ((Nteam2.at(ii)[0]).weaponclass != magic)
					{
						(Nteam2.at(ii)[0]).moves = 3; //reset unit movements
					}
					else if ((Nteam2.at(ii)[0]).weaponclass == magic)
					{
						(Nteam2.at(ii)[0]).moves = 2; //reset unit movements
					}
				}
			}
		}
		else if (activeTeam == 2) //check if you want to end team 2's turn
		{
			//cout << "team 2 moves left: " << teamSumMoves << endl;
			//check if the sum characters movements have been exhausted, sum the characters movements and check if the sum == 0;
			if (teamEndTurn == 1) //check to see if the player has pressed the end turn keys
			{
				teamEndTurn = 0;
				activeTeam = 1;
				activeUnit = Nteam1.at(0); //change the active unit

				//reset the moves for all the new current team's units
				for (int ii = 0; ii < Nteam2.size(); ii++)
				{
					if ((Nteam1.at(ii)[0]).weaponclass != magic)
					{
						(Nteam1.at(ii)[0]).moves = 3; //reset unit movements
					}
					else if ((Nteam1.at(ii)[0]).weaponclass == magic)
					{
						(Nteam1.at(ii)[0]).moves = 2; //reset unit movements
					}
				}
			}
			else if (teamSumMoves <= 0) // check if the sum of movements on this team is 0 
			{
				activeTeam = 1;
				activeUnit = Nteam1.at(0); //change the active unit

				//reset the moves for all the new current team's units
				for (int ii = 0; ii < Nteam2.size(); ii++)
				{
					if ((Nteam1.at(ii)[0]).weaponclass != magic)
					{
						(Nteam1.at(ii)[0]).moves = 3; //reset unit movements
					}
					else if ((Nteam1.at(ii)[0]).weaponclass == magic)
					{
						(Nteam1.at(ii)[0]).moves = 2; //reset unit movements
					}
				}
			}

		}


	}

	void bindActiveUnit(int i, std::shared_ptr<Program> prog)
	{
		if ( (activeUnit[0].team == activeTeam) && (activeUnit[0].name == board.characters[i].name) ) //send a uniform to the shader indicating if the unit is selected
		{
			//cout << "active unit's team: " << activeUnit[0].team << endl;
			glUniform1i(prog->getUniform("activeUnit"), 1); //true, highlight
		}
		else //send a uniform to the shader
		{
			glUniform1i(prog->getUniform("activeUnit"), 0); //false, dont highlight
		}
	}

	/****DRAW
	This is the most important function in your program - this is where you
	will actually issue the commands to draw any geometry you have set up to
	draw
	********/
	void render()
	{
        static bool playingSong1 = true;
        static bool playingSong2 = false;
        if (playingSong1) {
            playingSong2 = PlaySound(TEXT("../resources/lostinthots.wav"), NULL, SND_FILENAME | SND_NOSTOP | SND_ASYNC);
            if (playingSong2) {
                cout << "Playing Lost In Thoughts" << endl;
                playingSong1 = false;
            }  
        }
        else if (playingSong2){
            playingSong1 = PlaySound(TEXT("../resources/nge-thanatos.wav"), NULL, SND_FILENAME | SND_NOSTOP | SND_ASYNC);
            if (playingSong1) {
                cout << "Playing NGE - Thanatos" << endl;
                playingSong2 = false;
            }
        }
            
		//// Things to initialize for the first render loop ////
		static float t_swrd = 0, t_spear = 0, t_axe = 0, t_magic = 0;
		static vec2 offset1swrd, offset2swrd, offset1spear, offset2spear, offset1axe, offset2axe, offset1magic, offset2magic;
		static int firstLoop = 0; //use to initialize the offsets once

		//these work for 5x7 sprite sheets
		float updateX = 1. / 5.;
		float updateY = 1. / 7.;

		//Things to setup on the first loop of render
		if (firstLoop == 0)
		{
			activeUnit = Nteam1.at(0); //set the iniatial unit to the sword unit on team 1, this isnt

			//offsets 
			offset1swrd.x = 0; offset1swrd.y = 0;
			offset2swrd.x = updateX; offset2swrd.y = 0;
			offset1spear.x = 0; offset1spear.y = 0;
			offset2spear.x = updateX; offset2spear.y = 0;
			offset1axe.x = 0; offset1axe.y = 0;
			offset2axe.x = updateX; offset2axe.y = 0;
			offset1magic.x = 0; offset1magic.y = 0;
			offset2magic.x = updateX; offset2magic.y = 0;

			firstLoop += 1;
		}

		//// Game Logic Stuff ////
		updateTurn(); //function to check if the turn should be changed



		//// Graphics Stuff ////
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		double frametime = get_last_elapsed_time();
		static double totaltime_ms=0;
		totaltime_ms += frametime*1000.0;
		static double totaltime_untilframe_ms = 0;
		totaltime_untilframe_ms += frametime*1000.0;

		static int anim_num = 0;

		for (int ii = 0; ii < 200; ii++)
			animmat[ii] = mat4(1);


		//animation frame system
		int keyframe_length = root->animation[anim_num]->keyframes.size();
		int ms_length = root->animation[anim_num]->duration;
		int anim_step_width_ms = ms_length / keyframe_length;
		static int frame = 0;  
		static float play_anim_t = 0; // interpolation value between 2 different animations
		int num_animations = 2;
		int framezerocount = 0;
		
		if ((totaltime_untilframe_ms >= anim_step_width_ms) && (curcamPos == 1)) //new condition, only update when you enter the combat sc
		{
			totaltime_untilframe_ms = 0;
			frame++;
		}
		//if (frame > keyframe_length)  //Catch the end of the current animation
		if (!root->myplayanimation(frame, RUN_ANIMATION, AXE_SWING_ANIMATION, play_anim_t))
		{
			totaltime_untilframe_ms = 0;
			frame = 0;
			framezerocount++;
			anim_num++; //go from animation 0 to 1

			if (anim_num > 1) 
			{
				curcamPos = 0; //update the camera to move to the overhead scene
				moveCameraScene(); //move the camera back after combat is over
				play_anim_t = 0;
				anim_num = 0; //reset the animation loop for the next time combat begins, exception will be thrown if this line is not here
			}

		}

		//root->play_animation(frame,"axisneurontestfile_Avatar00");	//name of current animation, comment out to make code build faster
		//root->play_animation(frame, "avatar_0_fbx_tmp"); //play back our animation instead of test one 
		//root->myplayanimation(frame, RUN_ANIMATION, AXE_SWING_ANIMATION, play_anim_t);
		if (anim_num == AXE_SWING_ANIMATION && play_anim_t < 1) {
			play_anim_t += frametime;
		}
		else if (anim_num == RUN_ANIMATION && play_anim_t > 0) {
			play_anim_t -= frametime;
		}
	

		// Get current frame buffer size.
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		float aspect = width/(float)height;
		glViewport(0, 0, width, height); //use the entire window

		// Clear framebuffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Create the matrix stacks - please leave these alone for now
		glm::mat4 V, M, P; //View, Model and Perspective matrix
		V = mycam.process(frametime);
		M = glm::mat4(1);
		// Apply orthographic projection....
		P = glm::ortho(-1 * aspect, 1 * aspect, -1.0f, 1.0f, -2.0f, 100.0f);		
		if (width < height)
			{
			P = glm::ortho(-1.0f, 1.0f, -1.0f / aspect,  1.0f / aspect, -2.0f, 100.0f);
			}
		// ...but we overwrite it (optional) with a perspective projection.
		P = glm::perspective((float)(3.14159 / 4.), (float)((float)width/ (float)height), 0.1f, 1000.0f); //so much type casting... GLM metods are quite funny ones
		
		

		//Zoom the camera if T is pressed
		if (zoomCam == TRUE )
		{
			zoomCamera();
		}
		

		//Sky Stuff 
		float sangle = 3.1415926 / 2.;
		glm::vec3 camp = -mycam.pos;
		glm::mat4 TransSky = glm::translate(glm::mat4(1.0f), camp);
		glm::mat4 SSky = glm::scale(glm::mat4(1.0f), glm::vec3(0.8f, 0.8f, 0.8f));

		M = TransSky * SSky;

		// Draw the sky using GLSL.
		psky->bind();		
		glUniformMatrix4fv(psky->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(psky->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(psky->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glUniform3fv(psky->getUniform("campos"), 1, &mycam.pos[0]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture2);
		glDisable(GL_DEPTH_TEST);
		shape->draw(psky, false);			//render!!!!!!!
		glEnable(GL_DEPTH_TEST);	
		psky->unbind();

		//// Draw the plane using GLSL.
		//glm::mat4 TransPlane = glm::translate(glm::mat4(1.0f), vec3(0,0,-3));
		//glm::mat4 SPlane = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 2.0f));
		//sangle = -3.1415926 / 2.;
		//glm::mat4 RotateXPlane = glm::rotate(glm::mat4(1.0f), sangle, vec3(1,0,0));
		//
		//mat4 anim = test_animation();
		//M = TransPlane*anim*RotateXPlane;

		//pplane->bind();
		//glUniformMatrix4fv(pplane->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		//glUniformMatrix4fv(pplane->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		//glUniformMatrix4fv(pplane->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		//glUniform3fv(pplane->getUniform("campos"), 1, &mycam.pos[0]);
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, Texture2);
		////plane->draw(pplane, false);			//render!!!!!!!
		//pplane->unbind();


		//draw the lines for the bones		
		bonesprog->bind();
		//send the matrices to the shaders
		glUniformMatrix4fv(bonesprog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(bonesprog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(bonesprog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glUniform3fv(bonesprog->getUniform("campos"), 1, &mycam.pos[0]);	
		glBindVertexArray(VertexArrayID);
		//actually draw from vertex 0, 3 vertices
		//glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
		mat4 Vi = glm::transpose(V);
		Vi[0][3] = 0;
		Vi[1][3] = 0;
		Vi[2][3] = 0;
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);

		////------------------------ COMBAT DRAWING, MAKE THIS GRID DRAW A LARGE TERRAIN FOR BATTLES INSTEAD, CAN PUSH WHOLE COMBAT SCENE FURTHER AWAY FROM OVERHEAD SCENE IF NEEDED --------------------------------////
		//-- lets swap the grid to a heightmaped mesh or a billboard

		//draw right set of bones
		glm::mat4 TransBones = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.0f, -15.0f)); //translate the bones back into the combat scene
		sangle = 3.1415926 / 2.;
		glm::mat4 rotXBones = glm::rotate(glm::mat4(1.0f), sangle, glm::vec3(0, 1, 0)); //rotate the bones
		glm::mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(0.01f, 0.01f, 0.01f)); //scale the bones
		M = TransBones * rotXBones* S;
		glUniformMatrix4fv(bonesprog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glUniformMatrix4fv(bonesprog->getUniform("Manim"), 200, GL_FALSE, &animmat[0][0][0]);
		glDrawArrays(GL_LINES, 4, size_stick-4);

		//draw left set of bones
		TransBones = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 0.0f, -15.0f)); //translate the bones back into the combat scene
		sangle = -3.1415926 / 2.;
		rotXBones = glm::rotate(glm::mat4(1.0f), sangle, glm::vec3(0, 1, 0)); //rotate the bones
		S = glm::scale(glm::mat4(1.0f), glm::vec3(0.01f, 0.01f, 0.01f)); //scale the bones
		M = TransBones * rotXBones* S;
		glUniformMatrix4fv(bonesprog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glUniformMatrix4fv(bonesprog->getUniform("Manim"), 200, GL_FALSE, &animmat[0][0][0]);
		glDrawArrays(GL_LINES, 4, size_stick - 4);

		bonesprog->unbind();
		

		//Draw the billboards that have the background textures on them for the combat scene
		billboards->bind();
		glUniformMatrix4fv(billboards->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(billboards->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glm::mat4 backgroundTranslation = glm::translate(glm::mat4(1.0f), glm::vec3(9.0f, -2.0f, -21.5f)); //translate to behind the combat plane, xyz are fine, make y absloute( x + 10 ) to keep the bg in a proper location
		sangle = - 3.1415926 / 3.; //angle to rotate by
		glm::mat4 rotMtns = glm::rotate(glm::mat4(1.0f), sangle, glm::vec3(0, 1, 0)); //axis to apply rotation to
		float Hratio = 2.839961; //horizontal / vertical aspect ratio, old one 3.725
		float Vscale = 10; //mult this by Hratio to get horizontal scale
		glm::mat4 Smtn = glm::scale(glm::mat4(1.0f), glm::vec3(Vscale * Hratio, Vscale,1.f)); //scale the mtns to the correct size and aspect ratio 
		M = backgroundTranslation * rotMtns * Smtn;
		glUniformMatrix4fv(billboards->getUniform("M"), 1, GL_FALSE, &M[0][0]); //upload the model matrix
		glBindVertexArray(BillboardVAOID); //use the square billboard, its ok since scale fixes aspect ratio problems
		//bind texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, bgmountains1);
		//draw the bilboars in here
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void*)0); //actually draw the billboard (has 6 verts)
		billboards->unbind();


		//render combat plane, change to draw a different 
		bricks->bind();
		glUniformMatrix4fv(bricks->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(bricks->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(bricks->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture3);
		S = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));
		for (int i = 0; i < board.getBoardWidth() * board.getBoardHeight(); i++) {
			vec3 newPos = board.mapBlocks[i];
			newPos.x *= -1;
			newPos.z *= -1;
			TransBones = glm::translate(glm::mat4(1.0f), newPos);
			mat4 TransPos = glm::translate(glm::mat4(1.0f), vec3(0.25f, -1.0f, -15.0f));
			M = TransPos * TransBones * S;
			glUniformMatrix4fv(bricks->getUniform("M"), 1, GL_FALSE, &M[0][0]);
			brick->draw(bricks, FALSE);
		}
		bricks->unbind();
        

        glBindVertexArray(0);
		
        
        

		////-------------------------- OVERHEAD VIEW DRAWING ---------------------------------------////



		//draw the overhead game grid
        bricks->bind();
        glUniformMatrix4fv(bricks->getUniform("P"), 1, GL_FALSE, &P[0][0]);
        glUniformMatrix4fv(bricks->getUniform("V"), 1, GL_FALSE, &V[0][0]);
        glUniformMatrix4fv(bricks->getUniform("M"), 1, GL_FALSE, &M[0][0]);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Texture3);
        S = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));
        for (int i = 0; i < board.getBoardWidth() * board.getBoardHeight(); i++) { //draw the game grid block by block
            vec3 newPos = board.mapBlocks[i];
            newPos.x *= -1;
            newPos.z *= -1;
			glm::mat4 TranstoMap = glm::translate(glm::mat4(1.0f), newPos); //translate to the pos stored in the gameboard
			glm:mat4 TransPos = glm::translate(glm::mat4(1.0f), vec3(0, -0.0005, 0.0)); //additional transformations to what the game board stores
            M = TransPos * TranstoMap * S;
            glUniformMatrix4fv(bricks->getUniform("M"), 1, GL_FALSE, &M[0][0]);
            brick->draw(bricks, FALSE);
        }
        
        bricks->unbind();

		//-- draw gui and hud elements --//

		updateHUDteams(boolTeamHUD, frametime); //check if any of the Team HUD elements need to be updated

		//team 1 turn indicator
		billboards->bind();
		glUniformMatrix4fv(billboards->getUniform("P"), 1, GL_FALSE, &P[0][0]); //send p and v matrices to the shader
		glUniformMatrix4fv(billboards->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		//do transformations and upload model matrix to shader
		glm::mat4 TranstoMap = glm::translate(glm::mat4(1.0f), board.mapBlocks[board.mapBlocks.size()/2]); //translate to the pos stored in the middle of the gameboard
		glm::mat4 TransPos = glm::translate(glm::mat4(1.0f), vec3(-5.5, 1.5, 10.5)); //additional transformations to what the game board stores
		//set up a variable that gets set at the start of this teams turn after combat so this flies in
		
		glm::mat4 TanimatedFlyIn = glm::translate(glm::mat4(1.0f), vec3(animateHudTeam1, 0, 0));
		float rotAmount = -3.1415926/2; //the angle of roation, rotate 90 degress to face the camera
		glm::mat4 RotHud = glm::rotate(glm::mat4(1.0f), rotAmount, glm::vec3(1, 0, 0)); //rotate the hud so it faces the cam, define what axis rotAmount works on
		glm::mat4 scaleHud = glm::scale(glm::mat4(1.0f), glm::vec3(1));
		M = TransPos * TranstoMap * TanimatedFlyIn * RotHud * scaleHud; //model matrix, order of operations: T * R * S
		glUniformMatrix4fv(billboards->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glBindVertexArray(BillboardVAOID);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, guiTeam1Tex);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void*)0); //actually draw the billboard (has 6 verts)

		billboards->unbind();

		//team 2 turn indicator
		billboards->bind();
		glUniformMatrix4fv(billboards->getUniform("P"), 1, GL_FALSE, &P[0][0]); //send p and v matrices to the shader
		glUniformMatrix4fv(billboards->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		//do transformations and upload model matrix to shader
		TranstoMap = glm::translate(glm::mat4(1.0f), board.mapBlocks[board.mapBlocks.size() / 2]); //translate to the pos stored in the middle of the gameboard
		TransPos = glm::translate(glm::mat4(1.0f), vec3(-5.5, 1.5, 10.5)); //additional transformations to what the game board stores
																					 //set up a variable that gets set at the start of this teams turn after combat so this flies in

		glm::mat4 T2animatedFlyIn = glm::translate(glm::mat4(1.0f), vec3(animateHudTeam2, 0, 0));
		rotAmount = -3.1415926 / 2; //the angle of roation, rotate 90 degress to face the camera
		RotHud = glm::rotate(glm::mat4(1.0f), rotAmount, glm::vec3(1, 0, 0)); //rotate the hud so it faces the cam, define what axis rotAmount works on
		scaleHud = glm::scale(glm::mat4(1.0f), glm::vec3(1));
		M = TransPos * TranstoMap * T2animatedFlyIn * RotHud * scaleHud; //model matrix, order of operations: T * R * S
		glUniformMatrix4fv(billboards->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glBindVertexArray(BillboardVAOID);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, guiTeam2Tex);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void*)0); //actually draw the billboard (has 6 verts)

		billboards->unbind();

		//sangle = -3.1415926 / 3.; //angle to rotate by
		//glm::mat4 rotMtns = glm::rotate(glm::mat4(1.0f), sangle, glm::vec3(0, 1, 0)); //axis to apply rotation to


		//-- Draw the new animated sprites based on Weapon class --//
		//check to see if any of the units have 0 health, if they do remove themfrom the gameboard
		
		teamSumMoves = 0; //recompute the total number of moves on the team each render loop
		for (int i = 0; i < board.characters.size(); i++) //Check every character on the game board
		{
			
			glm::mat4 TransSprites = glm::translate(glm::mat4(1.0f), board.characters[i].position + vec3(0, 0, 0)); //draw all the sprites
			//Game Logic Stuff
			//Add to the sum of the teams movement
			if ((activeTeam == activeUnit[0].team) )
			{
				teamSumMoves += board.characters[i].moves;
			}
			//draw each weapon class
			if (board.characters[i].weaponclass == sword) //draw sword units to the board
			{
				swordUnits->bind();
				glUniformMatrix4fv(swordUnits->getUniform("P"), 1, GL_FALSE, &P[0][0]);
				glUniformMatrix4fv(swordUnits->getUniform("V"), 1, GL_FALSE, &V[0][0]);
				glUniformMatrix4fv(swordUnits->getUniform("M"), 1, GL_FALSE, &M[0][0]);
				glBindVertexArray(BillboardVAOID);

				//cout << "found char with sword" << "\n";
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, board.characters[i].texture);	
			

				//UPDATE THE SPRITE
				t_swrd += 0.25; //interpolation value for sword Units
				if (t_swrd >= 1)
				{
					t_swrd = 0; //reset t
					//update the 1st offset
					offset1swrd.x += updateX;
					if (offset1swrd.x >= 1.00)
					{
						offset1swrd.x = 0;
						offset1swrd.y += updateY;
						if (offset1swrd.y >= 1.00)
						{
							offset1swrd.y = 0;
						}
					}
					//update the 2nd offset
					offset2swrd.x += updateX;
					if (offset2swrd.x >= 1.00)
					{
						offset2swrd.x = 0;
						offset2swrd.y += updateY;
						if (offset2swrd.y >= 1.00)
						{
							offset2swrd.y = 0;
						}
					}

				}
				//bind uniforms to the shader
				glUniform1f(swordUnits->getUniform("t"), t_swrd);
				glUniform2fv(swordUnits->getUniform("offset1"), 1, &offset1swrd[0]);
				glUniform2fv(swordUnits->getUniform("offset2"), 1, &offset2swrd[0]);
				glUniform1i(swordUnits->getUniform("team"), board.characters[i].team);

				//glm::mat4 TransSprites = glm::translate(glm::mat4(1.0f), board.characters[i].position + glm::vec3(0, 0, 0));
				//board.moveCharacter(board.characters[i].position.x, board.characters[i].position.y, board.characters[i].position.x + 1, board.characters[i].position.y + 1 );
				//int moveCharacter(int charX, int charY, int destX, int destY);
				M = TransSprites * Vi;
				glUniformMatrix4fv(swordUnits->getUniform("M"), 1, GL_FALSE, &M[0][0]);
				//call bind func here
				bindActiveUnit(i, swordUnits); //bind the active unit true/false
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void*)0); //actually draw the billboard (has 6 verts)

				swordUnits->unbind();
			}
			if (board.characters[i].weaponclass == spear) //Draw Spear Units to the board
			{
				spearUnits->bind();
				glUniformMatrix4fv(spearUnits->getUniform("P"), 1, GL_FALSE, &P[0][0]);
				glUniformMatrix4fv(spearUnits->getUniform("V"), 1, GL_FALSE, &V[0][0]);
				glUniformMatrix4fv(spearUnits->getUniform("M"), 1, GL_FALSE, &M[0][0]);
				glBindVertexArray(BillboardVAOID);

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, board.characters[i].texture);

				//UPDATE THE SPRITE
				t_spear += 0.25; //interpolation value for sword Units
				if (t_spear >= 1)
				{
					t_spear = 0; //reset t
					//update the 1st offset
					offset1spear.x += updateX;  //WE PROBABLY NEED PER SHADER UPDATES
					if (offset1spear.x >= 1.00)
					{
						offset1spear.x = 0;
						offset1spear.y += updateY;
						if (offset1spear.y >= 1.00)
						{
							offset1spear.y = 0;
						}
					}
					//update the 2nd offset
					offset2spear.x += updateX;
					if (offset2spear.x >= 1.00)
					{
						offset2spear.x = 0;
						offset2spear.y += updateY;
						if (offset2spear.y >= 1.00)
						{
							offset2spear.y = 0;
						}
					}

				}
				//bind uniforms to the shader
				glUniform1f(spearUnits->getUniform("t"), t_spear);
				glUniform2fv(spearUnits->getUniform("offset1"), 1, &offset1spear[0]);
				glUniform2fv(spearUnits->getUniform("offset2"), 1, &offset2spear[0]);
				glUniform1i(spearUnits->getUniform("team"), board.characters[i].team);

				M = TransSprites * Vi;
				glUniformMatrix4fv(spearUnits->getUniform("M"), 1, GL_FALSE, &M[0][0]);
				//call bind func here
				bindActiveUnit(i, spearUnits); //bind the active unit true/false
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void*)0); //actually draw the billboard (has 6 verts)

				spearUnits->unbind();

			}
			if (board.characters[i].weaponclass == axe)
			{
				axeUnits->bind();
				glUniformMatrix4fv(axeUnits->getUniform("P"), 1, GL_FALSE, &P[0][0]);
				glUniformMatrix4fv(axeUnits->getUniform("V"), 1, GL_FALSE, &V[0][0]);
				glUniformMatrix4fv(axeUnits->getUniform("M"), 1, GL_FALSE, &M[0][0]);
				glBindVertexArray(BillboardVAOID);

				//cout << "found char with spear" << "\n";
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, board.characters[i].texture);

				//UPDATE THE SPRITE
				t_axe += 0.25; //interpolation value for sword Units
				if (t_axe >= 1)
				{
					t_axe = 0; //reset t
					//update the 1st offset
					offset1axe.x += updateX;  //WE PROBABLY NEED PER SHADER UPDATES
					if (offset1axe.x >= 1.00)
					{
						offset1axe.x = 0;
						offset1axe.y += updateY;
						if (offset1axe.y >= 1.00)
						{
							offset1axe.y = 0;
						}
					}
					//update the 2nd offset
					offset2axe.x += updateX;
					if (offset2axe.x >= 1.00)
					{
						offset2axe.x = 0;
						offset2axe.y += updateY;
						if (offset2axe.y >= 1.00)
						{
							offset2axe.y = 0;
						}
					}

				}
				//bind uniforms to the shader
				glUniform1f(axeUnits->getUniform("t"), t_axe);
				glUniform2fv(axeUnits->getUniform("offset1"), 1, &offset1axe[0]);
				glUniform2fv(axeUnits->getUniform("offset2"), 1, &offset2axe[0]);
				glUniform1i(axeUnits->getUniform("team"), board.characters[i].team);

				//glm::mat4 TransSprites = glm::translate(glm::mat4(1.0f), board.characters[i].position + glm::vec3(0, 0, 0.)); //Our y and z planes are swapped, add the vector to get the sprites from intersecting with the board
				//board.moveCharacter(board.characters[i].position.x, board.characters[i].position.y, board.characters[i].position.x + 1, board.characters[i].position.y + 1 );
				//int moveCharacter(int charX, int charY, int destX, int destY);
				M = TransSprites * Vi;
				glUniformMatrix4fv(axeUnits->getUniform("M"), 1, GL_FALSE, &M[0][0]);
				//call bind func here
				bindActiveUnit(i, axeUnits); //bind the active unit true/false
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void*)0); //actually draw the billboard (has 6 verts)

				axeUnits->unbind();

			}
			if (board.characters[i].weaponclass == magic)
			{
				magicUnits->bind();
				glUniformMatrix4fv(magicUnits->getUniform("P"), 1, GL_FALSE, &P[0][0]);
				glUniformMatrix4fv(magicUnits->getUniform("V"), 1, GL_FALSE, &V[0][0]);
				glUniformMatrix4fv(magicUnits->getUniform("M"), 1, GL_FALSE, &M[0][0]);
				glBindVertexArray(BillboardVAOID);

				//cout << "found char with spear" << "\n";
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, board.characters[i].texture);

				//UPDATE THE SPRITE
				t_magic += 0.25; //interpolation value for sword Units
				if (t_magic >= 1)
				{
					t_magic = 0; //reset t
							   //update the 1st offset
					offset1magic.x += updateX;  //WE PROBABLY NEED PER SHADER UPDATES
					if (offset1magic.x >= 1.00)
					{
						offset1magic.x = 0;
						offset1magic.y += updateY;
						if (offset1magic.y >= 1.00)
						{
							offset1magic.y = 0;
						}
					}
					//update the 2nd offset
					offset2magic.x += updateX;
					if (offset2magic.x >= 1.00)
					{
						offset2magic.x = 0;
						offset2magic.y += updateY;
						if (offset2magic.y >= 1.00)
						{
							offset2magic.y = 0;
						}
					}

				}
				//bind uniforms to the shader
				glUniform1f(magicUnits->getUniform("t"), t_magic);
				glUniform2fv(magicUnits->getUniform("offset1"), 1, &offset1magic[0]);
				glUniform2fv(magicUnits->getUniform("offset2"), 1, &offset2magic[0]);
				glUniform1i(magicUnits->getUniform("team"), board.characters[i].team);

				//glm::mat4 TransSprites = glm::translate(glm::mat4(1.0f), board.characters[i].position + glm::vec3(0, 0, 0)); //Our y and z planes are swapped, add the vector to get the sprites from intersecting with the board
				//board.moveCharacter(board.characters[i].position.x, board.characters[i].position.y, board.characters[i].position.x + 1, board.characters[i].position.y + 1 );
				//int moveCharacter(int charX, int charY, int destX, int destY);
				M = TransSprites * Vi;
				glUniformMatrix4fv(magicUnits->getUniform("M"), 1, GL_FALSE, &M[0][0]);
				bindActiveUnit(i, magicUnits); //bind the active unit true/false
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void*)0); //actually draw the billboard (has 6 verts)

				

				magicUnits->bind();

			}

			//Check to see if any characters are overlapping, theres probably a more efficent way to do this
			for (int j = 0; j < board.characters.size(); j++)
			{
				//Check if units have the same postion and are on different teams
				if ( (board.characters[i].position == board.characters[j].position) && (board.characters[i].team != board.characters[j].team) )
				   //( board.characters[i].name != board.characters[j].name ) ) //Old shit used to be && with the above if
				{
					Character defendingUnit = board.characters[j];


					//do game logic to determine new health for units

					//add the zoom to game board then transition to battle scene instead of jump cuts
					curcamPos = 1; //Set up the camera to move to the combat scene
					moveCameraScene(); //make it so this function kicks off the battle scene animation
					board.characters[i].position.x = board.characters[i].position.x - 1; //move a character so they are no longer overlapping with the enemy
				}
			}
			
			
		} //end loop through all character on board
		


	}

};
//******************************************************************************************
int main(int argc, char **argv)
{
	std::string resourceDir = "../resources"; // Where the resources are loaded from
	if (argc >= 2)
	{
		resourceDir = argv[1];
	}

	Application *application = new Application();
    
	/* your main will always include a similar set up to establish your window
		and GL context, etc. */
	WindowManager * windowManager = new WindowManager();
	windowManager->init(1920, 1080);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;

	/* This is the code that will likely change program to program as you
		may need to initialize or set up different data and state */
	// Initialize scene.
	application->init(resourceDir);
	application->initGeom(resourceDir);

	// Loop until the user closes the window.
	while(! glfwWindowShouldClose(windowManager->getHandle()))
	{
		// Render scene.
		application->render();

		// Swap front and back buffers.
		glfwSwapBuffers(windowManager->getHandle());
		// Poll for and process events.
		glfwPollEvents();
	}

	// Quit program.
	windowManager->shutdown();
	return 0;
}
