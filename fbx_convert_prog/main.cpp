/*
CPE/CSC 474 Lab base code Eckhardt/Dahl
based on CPE/CSC 471 Lab base code Wood/Dunn/Eckhardt
*/

#include <iostream>
#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"

#include "WindowManager.h"
#include "Shape.h"
#include "Line.h"
#include "bone.h"
using namespace std;
using namespace glm;
shared_ptr<Shape> shape;
shared_ptr<Shape> plane;
shared_ptr<Shape> brick;

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
        return R * T;
    }
};

camera mycam;

class Character {
public:
    vec3 position;
    enum weapon {
        sword,
        spear,
        axe,
        bow,
        magic
    };
    weapon weaponclass;
    bool hasShield;
    int health;
    string name;
    int isCharacter = 0; // is 1 when character actually exists

    // constructors
    Character();
    Character(string name, vec3 position, weapon weaponclass, bool hasShield, int health);
};

Character::Character() {
    isCharacter = 0;
}
Character::Character(string charName, vec3 charPos, weapon curWeapon, bool shield, int charHealth) {
    position = charPos;
    weaponclass = curWeapon;
    hasShield = shield;
    name = charName;
    health = charHealth;
    isCharacter = 1;
}

class Board {
public:
    // Board properties
    int boardWidth;
    int boardHeight;

    // Rendered Positions
    vector<vec3> mapBlocks;
    vector<int> characterIndices;
    vector<vec3> spritePositions;

    // Character Positions
    vector<vector<Character>> team1Pos;
    vector<vector<Character>> team2Pos;
    vector<vector<Character>> allCharPos;

    // Constructors
    Board();
    Board(vector<vec3> blockPos, vector<vector<Character>> team1, vector<vector<Character>> team2, vector<vector<Character>> charsPos, int width, int height);
    
    // Board methods
    int checkWin();
    void convertCharToPhysicalCoordinates();

    // Character management methods
    int addCharacter(Character c, int x, int y);
    int hasCharacter(int x, int y);
    Character getCharacter(int x, int y);
    int moveCharacter(int charX, int charY, int destX, int destY);
    void removeCharacter(int x, int y);
    int startFight(Character a, Character b); 

    // Extraneous methods
    int getBoardWidth();
    int getBoardHeight();
    
};

// Constructors
Board::Board() {}

Board::Board(vector<vec3> blockPos, vector<vector<Character>> team1, vector<vector<Character>> team2, vector<vector<Character>> charsPos, int width, int height) {
    mapBlocks = blockPos;
    allCharPos = charsPos;
    boardWidth = width;
    boardHeight = height;
    team1Pos = team1;
    team2Pos = team2;
}

// Board methods
int Board::checkWin() {
    // returns 0 if player 1 wins, 1 if player 2 wins
    // check number of units on each team
    // if either team number of units = 0, declare victory for other team
	return 0; //temp
}

void Board::convertCharToPhysicalCoordinates() {
    // uses the 2D array of character positions and sets the character indices array
    // used to get the correct position to draw each character
    int counter = 0;
    vector<int> characterIndices;
    vector<Character> characters;
    for (int i = 0; i < boardWidth; i++) {
        for (int j = 0; j < boardHeight; j++) {
            if (hasCharacter(i, j)) {
                characterIndices.push_back(counter); // gets indices of 2D and puts it into 1D array
                // TODO: convert to vec3 positions above each grid square and store in spritePositions
            }
            counter++;
        }
    }

}

// Character management methods
int Board::addCharacter(Character c, int x, int y) {
    // returns 1 if character is added successfully (i.e. no other unit exists in the same position), otherwise 0
	return 0; //temp
}


int Board::hasCharacter(int x, int y) {
    // returns 1 if a character exists at x,y, otherwise 0
    if (allCharPos.at(x).at(y).isCharacter == NULL) {
        return 0;
    }
    return 1;
}

Character Board::getCharacter(int x, int y) {
    // returns character at x,y
    return allCharPos.at(x).at(y);
}

int Board::moveCharacter(int charX, int charY, int destX, int destY) {
	// implementation needed
	// check if character exists at that (charX,charY)
	if (hasCharacter(charX, charY) == 1) {
		// if character exists
		// check if destination point has a character
		if (hasCharacter(destX, destY) == 1) {
			// if has character
			cout << "THERE WILL BE BLOOD" << endl;
			// just print something right now, or do something that shows that there will be a detection when selecting (start fight)
			return 1;
		}
		else {
			// if not have character
			// move current character to dest
			allCharPos.at(destX).at(destY) = getCharacter(charX, charY);
			removeCharacter(charX, charY);
			if (hasCharacter(charX, charY) != 0) {
				cout << "error removing character" << endl;
			}
			return 1;
		}
	}
	return 0;
}

void Board::removeCharacter(int x, int y) {
    // deletes character at that position
    allCharPos.at(x).at(y).isCharacter = 0;
}

int Board::startFight(Character a, Character b) {
    // return 0 if a wins, returns 1 if b wins
	return 0; //temp
}

// Extraneous methods
int Board::getBoardWidth() {
    return boardWidth;
}

int Board::getBoardHeight() {
    return boardHeight;
}

Board board;

class Application : public EventCallbacks
{

public:

	WindowManager * windowManager = nullptr;

	// Our shader program
	std::shared_ptr<Program> prog, psky, pplane, bricks;

	// Contains vertex information for OpenGL
	GLuint VertexArrayID;

	// Data necessary to give our box to OpenGL
	GLuint VertexBufferID, VertexBufferIDimat, VertexNormDBox, VertexTexBox, IndexBufferIDBox;

	//texture data
	GLuint Texture;
	GLuint Texture2;
    GLuint Texture3;
	//line
	Line linerender;
	Line smoothrender;
	vector<vec3> line;
	
	//animation matrices:
	mat4 animmat[200];
	int animmatsize=0;

    

    

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
        brick = make_shared<Shape>();
        brick->loadMesh(resourceDirectory + "/cube2.obj");
        brick->resize();
        brick->init();

		for (int ii = 0; ii < 200; ii++)
			animmat[ii] = mat4(1);
		
		readtobone("test.fbx",&all_animation,&root);
		root->set_animations(&all_animation,animmat,animmatsize);
		
			
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



		glBindVertexArray(0);

	

		int width, height, channels;
		char filepath[1000];

		//texture 1
		string str = resourceDirectory + "/explosion.jpg";
		strcpy(filepath, str.c_str());
		unsigned char* data = stbi_load(filepath, &width, &height, &channels, 4);
		glGenTextures(1, &Texture);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		//texture 2
		str = resourceDirectory + "/grid.jpg";
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

        str = resourceDirectory + "/brick-img512.png";
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

		//[TWOTEXTURES]
		//set the 2 textures to the correct samplers in the fragment shader:
		GLuint Tex1Location = glGetUniformLocation(prog->pid, "tex");//tex, tex2... sampler in the fragment shader
		GLuint Tex2Location = glGetUniformLocation(prog->pid, "tex2");
        GLuint Tex3Location = glGetUniformLocation(prog->pid, "bricks");
		// Then bind the uniform samplers to texture units:
		glUseProgram(prog->pid);
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
        vector<vector<Character>> charPos;
        for (int i = 0; i < mapWidth; i++) {
            for (int j = 0; j < mapHeight; j++) {
                vec3 blockPos = vec3(i, 0, j) - vec3(mapWidth / 2, 0 , mapHeight / 2);
                mapBlocks.push_back(blockPos);
            }
        }

        // Set size so can access
        charPos.resize(mapWidth);
        for (int i = 0; i < mapHeight; i++) {
            charPos.at(i).resize(10);
        }
		vector<vector<Character>> team1, team2;

        board = Board(mapBlocks, team1, team2, charPos, mapWidth, mapHeight);

        //charPos.at(0).at(1) = 10;
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
		prog = std::make_shared<Program>();
		prog->setVerbose(true);
		prog->setShaderNames(resourceDirectory + "/shader_vertex.glsl", resourceDirectory + "/shader_fragment.glsl");
		if (!prog->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		prog->addUniform("P");
		prog->addUniform("V");
		prog->addUniform("M");
		prog->addUniform("Manim");
		prog->addUniform("campos");
		prog->addAttribute("vertPos");
		prog->addAttribute("vertimat");


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

		pplane = std::make_shared<Program>();
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
		pplane->addAttribute("vertTex");

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
	}


	/****DRAW
	This is the most important function in your program - this is where you
	will actually issue the commands to draw any geometry you have set up to
	draw
	********/

	
	void render()
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		double frametime = get_last_elapsed_time();
		static double totaltime_ms=0;
		totaltime_ms += frametime*1000.0;
		static double totaltime_untilframe_ms = 0;
		totaltime_untilframe_ms += frametime*1000.0;

		for (int ii = 0; ii < 200; ii++)
			animmat[ii] = mat4(1);


		//animation frame system
		int anim_step_width_ms = 8490 / 204;
		static int frame = 0;
		if (totaltime_untilframe_ms >= anim_step_width_ms)
			{
			totaltime_untilframe_ms = 0;
			frame++;
			}
		root->play_animation(frame,"axisneurontestfile_Avatar00");	//name of current animation	


		// Get current frame buffer size.
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		float aspect = width/(float)height;
		glViewport(0, 0, width, height);

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

		// Draw the plane using GLSL.
		glm::mat4 TransPlane = glm::translate(glm::mat4(1.0f), vec3(0,0,-3));
		glm::mat4 SPlane = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 2.0f));
		sangle = -3.1415926 / 2.;
		glm::mat4 RotateXPlane = glm::rotate(glm::mat4(1.0f), sangle, vec3(1,0,0));
		
		mat4 anim = test_animation();
		M = TransPlane*anim*RotateXPlane;

		pplane->bind();
		glUniformMatrix4fv(pplane->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(pplane->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(pplane->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glUniform3fv(pplane->getUniform("campos"), 1, &mycam.pos[0]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture2);
		//plane->draw(pplane, false);			//render!!!!!!!
		pplane->unbind();


		//draw the lines

		
		prog->bind();
		//send the matrices to the shaders
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glUniform3fv(prog->getUniform("campos"), 1, &mycam.pos[0]);	
		glBindVertexArray(VertexArrayID);
		//actually draw from vertex 0, 3 vertices
		//glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
		mat4 Vi = glm::transpose(V);
		Vi[0][3] = 0;
		Vi[1][3] = 0;
		Vi[2][3] = 0;
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);

		

		glm::mat4 TransZ = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -8));
		glm::mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(0.01f, 0.01f, 0.01f));
		M = TransZ * S;
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glUniformMatrix4fv(prog->getUniform("Manim"), 200, GL_FALSE, &animmat[0][0][0]);
		glDrawArrays(GL_LINES, 4, size_stick-4);

        glBindVertexArray(0);
		prog->unbind();

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
            TransZ = glm::translate(glm::mat4(1.0f), newPos);
            mat4 TransPos = glm::translate(glm::mat4(1.0f), vec3(0.25, -1, 0.25));
            M = TransPos * TransZ * S;
            glUniformMatrix4fv(bricks->getUniform("M"), 1, GL_FALSE, &M[0][0]);
            brick->draw(bricks, FALSE);
        }
        bricks->unbind();

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
