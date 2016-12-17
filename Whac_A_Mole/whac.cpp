// Include standard headers
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <time.h>       /* time */
#include <math.h>
#include <unistd.h>
#include <common/text2D.hpp>
#include <cstring>
// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GL/glfw.h>

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>//rotation
#include <glm/gtx/norm.hpp>//rotation
using namespace glm;

#include <common/shader.hpp>
#include <common/texture.hpp>
#include <common/controls.hpp>
#include <common/objloader.hpp>
#include <common/vboindexer.hpp>
#include <common/text2D.hpp>

struct Mole
{
	int type;
	int hit;
	float x;
	float y;
	float z;
};

struct Hammer
{
	int type;
	int hit;
	float x;
	float y;
	float z;
};

int main(void)
{
    ALuint buffer, source;
    ALuint state;

    // Initialize the environment
        alutInit(0, NULL);

	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 4);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 2);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 1);

	// Open a window and create its OpenGL context
	if (!glfwOpenWindow(1024, 768, 0, 0, 0, 0, 32, 0, GLFW_WINDOW))
	{
		fprintf(stderr, "Failed to open GLFW window.\n");
		glfwTerminate();
		return -1;
	}

	// Initialize GLEW
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	glfwSetWindowTitle("Whac-A-Mole");

	// Ensure we can capture the escape key being pressed below
	glfwEnable(GLFW_STICKY_KEYS);

	glfwSetMousePos(1024 / 2, 768 / 2);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders("StandardShading.vertexshader", "StandardShading.fragmentshader");

	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
	GLuint ModelMatrixID = glGetUniformLocation(programID, "M");

	// Get a handle for our buffers
	GLuint vertexPosition_modelspaceID = glGetAttribLocation(programID, "vertexPosition_modelspace");
	GLuint vertexUVID = glGetAttribLocation(programID, "vertexUV");
	GLuint vertexNormal_modelspaceID = glGetAttribLocation(programID, "vertexNormal_modelspace");

	// Load the textures for each model
	//***************************

	GLuint Texture_Table = loadBMP_custom("table.bmp");
	GLuint Texture_Mole = loadBMP_custom("montymole.bmp");
	GLuint Texture_evilMole = loadBMP_custom("evilmontymole.bmp");
	GLuint Texture_Hammer = loadBMP_custom("hammer.bmp");

	//***************************

	// Get a handle for our "myTextureSampler" uniform
	GLuint TextureID = glGetUniformLocation(programID, "myTextureSampler");

	// Read Table obj file
	//***************************

	std::vector<glm::vec3> vertices_table;
	std::vector<glm::vec2> uvs_table;
	std::vector<glm::vec3> normals_table;
	bool res_table = loadOBJ("table.obj", vertices_table, uvs_table, normals_table);

	//***************************

	// Read Mole obj file
	//***************************

	std::vector<glm::vec3> vertices_mole;
	std::vector<glm::vec2> uvs_mole;
	std::vector<glm::vec3> normals_mole;
	bool res_mole = loadOBJ("montymole.obj", vertices_mole, uvs_mole, normals_mole);

	//***************************

	// Read Evil Mole obj file
	//***************************

	std::vector<glm::vec3> vertices_evilMole;
	std::vector<glm::vec2> uvs_evilMole;
	std::vector<glm::vec3> normals_evilMole;
	bool res_evilMole = loadOBJ("evilmontymole.obj", vertices_evilMole, uvs_evilMole, normals_evilMole);

	//***************************

	// Read Hammer obj file
	//***************************

	std::vector<glm::vec3> vertices_hammer;
	std::vector<glm::vec2> uvs_hammer;
	std::vector<glm::vec3> normals_hammer;
	bool res = loadOBJ("hammer.obj", vertices_hammer, uvs_hammer, normals_hammer);

	//***************************

	//Load Table
	//***************************

	GLuint vertexbuffer_table;
	glGenBuffers(1, &vertexbuffer_table);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer_table);
	glBufferData(GL_ARRAY_BUFFER, vertices_table.size() * sizeof(glm::vec3), &vertices_table[0], GL_STATIC_DRAW);

	GLuint uvbuffer_table;
	glGenBuffers(1, &uvbuffer_table);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer_table);
	glBufferData(GL_ARRAY_BUFFER, uvs_table.size() * sizeof(glm::vec2), &uvs_table[0], GL_STATIC_DRAW);

	GLuint normalbuffer_table;
	glGenBuffers(1, &normalbuffer_table);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer_table);
	glBufferData(GL_ARRAY_BUFFER, normals_table.size() * sizeof(glm::vec3), &normals_table[0], GL_STATIC_DRAW);

	//***************************

	//Load Mole
	//***************************

	GLuint vertexbuffer_mole;
	glGenBuffers(1, &vertexbuffer_mole);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer_mole);
	glBufferData(GL_ARRAY_BUFFER, vertices_mole.size() * sizeof(glm::vec3), &vertices_mole[0], GL_STATIC_DRAW);

	GLuint uvbuffer_mole;
	glGenBuffers(1, &uvbuffer_mole);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer_mole);
	glBufferData(GL_ARRAY_BUFFER, uvs_mole.size() * sizeof(glm::vec2), &uvs_mole[0], GL_STATIC_DRAW);

	GLuint normalbuffer_mole;
	glGenBuffers(1, &normalbuffer_mole);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer_mole);
	glBufferData(GL_ARRAY_BUFFER, normals_mole.size() * sizeof(glm::vec3), &normals_mole[0], GL_STATIC_DRAW);

	//***************************

	//Load Evil Mole
	//***************************

	GLuint vertexbuffer_evilMole;
	glGenBuffers(1, &vertexbuffer_evilMole);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer_evilMole);
	glBufferData(GL_ARRAY_BUFFER, vertices_evilMole.size() * sizeof(glm::vec3), &vertices_evilMole[0], GL_STATIC_DRAW);

	GLuint uvbuffer_evilMole;
	glGenBuffers(1, &uvbuffer_evilMole);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer_evilMole);
	glBufferData(GL_ARRAY_BUFFER, uvs_evilMole.size() * sizeof(glm::vec2), &uvs_evilMole[0], GL_STATIC_DRAW);

	GLuint normalbuffer_evilMole;
	glGenBuffers(1, &normalbuffer_evilMole);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer_evilMole);
	glBufferData(GL_ARRAY_BUFFER, normals_evilMole.size() * sizeof(glm::vec3), &normals_evilMole[0], GL_STATIC_DRAW);

	//***************************

	//Load Hammer
	//***************************

	GLuint vertexbuffer_hammer;
	glGenBuffers(1, &vertexbuffer_hammer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer_hammer);
	glBufferData(GL_ARRAY_BUFFER, vertices_hammer.size() * sizeof(glm::vec3), &vertices_hammer[0], GL_STATIC_DRAW);

	GLuint uvbuffer_hammer;
	glGenBuffers(1, &uvbuffer_hammer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer_hammer);
	glBufferData(GL_ARRAY_BUFFER, uvs_hammer.size() * sizeof(glm::vec2), &uvs_hammer[0], GL_STATIC_DRAW);

	GLuint normalbuffer_hammer;
	glGenBuffers(1, &normalbuffer_hammer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer_hammer);
	glBufferData(GL_ARRAY_BUFFER, normals_hammer.size() * sizeof(glm::vec3), &normals_hammer[0], GL_STATIC_DRAW);

	//***************************

	// Get a handle for our "LightPosition" uniform
	glUseProgram(programID);
	GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");

	Mole m1;
	Hammer h;

	m1.hit = 0;
	m1.type = 0;
	m1.x = 0;
	m1.y = 2;
	m1.z = -9;

	std::vector<Mole> moles(9, Mole(m1));

	//	0	1	2
	//	O	O	O

	//	3	4	5
	//	O	O	O

	//	6	7	8
	//	O	O	O

    moles[0].x = -3.5, moles[0].y = -10, moles[0].z = -12.5;
    moles[1].x = 0, moles[1].y = -10, moles[1].z = -12.5;
    moles[2].x = 3.5, moles[2].y = -10, moles[2].z = -12.5;
    moles[3].x = -3.5, moles[3].y = -10, moles[3].z - 9;
    moles[4].x = 0, moles[4].y = -10, moles[4].z = -9;
    moles[5].x = 3.5, moles[5].y = -10, moles[5].z = -9;
    moles[6].x = -3.5, moles[6].y = -10, moles[6].z = -5.5;
    moles[7].x = 0, moles[7].y = -10, moles[7].z = -5.5;
    moles[8].x = 3.5, moles[8].y = -10, moles[8].z = -5.5;

	bool flag = true;
    bool flag1 = true;

	//Generation of Random Numbers to represent mole locations and mole types
	//************
	srand(time(NULL));
	static double currentTime = glfwGetTime();
	static double currentTime2 = currentTime;
	int mole_pos1 = rand() % 9;
	int mole_pos2 = rand() % 9;

	int mole_type = rand() % 2;
    int ys = 310;
    int xs = 50;
    int time_limit = 60;
    int hp = 50;
	int score = 0;
	int threshold1 = 100;
    int threshold2 = 200;

	moles[mole_pos1].hit = 1;
	double mole_speed = 1;
	double lastTime;
	//*** text
	initText2D("Holstein.tga");
	char msg1[] = "Score:";
	char msg2[] = "HP:";
	char *score_text = (char*)malloc(5);
	char msg3[] = ("You win with score:");
	char msg4[] = ("You lose with score:");
	char *hp_text = (char*)malloc(5);
	int time = 0;
	char *time_text = (char*)malloc(5);
	char msg5[] = ("Time:");

	//*************
	do {
		
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);
		glm::vec3 lightPos = glm::vec3(0, 9, -8);
		glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);

		computeMatricesFromInputs();
		glm::mat4 ProjectionMatrix = getProjectionMatrix();
		glm::mat4 ViewMatrix = getViewMatrix();

		// MVP & drawing for table
		//***************************

		glm::mat4 RotationMatrix_Table = eulerAngleYXZ(0.0f, 0.0f, 0.0f);
		glm::mat4 TranslationMatrix_Table = translate(mat4(), vec3(0.0f, 0.0f, 0.0f));
		glm::mat4 ScalingMatrix_Table = scale(mat4(), vec3(1.0f, 1.0f, 1.0f));
		glm::mat4 ModelMatrix_Table = TranslationMatrix_Table * RotationMatrix_Table * ScalingMatrix_Table;

		glm::mat4 MVP_Table = ProjectionMatrix * ViewMatrix * ModelMatrix_Table;

		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP_Table[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix_Table[0][0]);
		glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture_Table);

		glUniform1i(TextureID, 0);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(vertexPosition_modelspaceID);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer_table);
		glVertexAttribPointer(
			vertexPosition_modelspaceID,  // The attribute we want to configure
			3,                            // size
			GL_FLOAT,                     // type
			GL_FALSE,                     // normalized?
			0,                            // stride
			(void*)0                      // array buffer offset
			);

		// 2nd attribute buffer : UVs
		glEnableVertexAttribArray(vertexUVID);
		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer_table);
		glVertexAttribPointer(
			vertexUVID,                   // The attribute we want to configure
			2,                            // size : U+V => 2
			GL_FLOAT,                     // type
			GL_FALSE,                     // normalized?
			0,                            // stride
			(void*)0                      // array buffer offset
			);

		// 3rd attribute buffer : normals
		glEnableVertexAttribArray(vertexNormal_modelspaceID);
		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer_table);
		glVertexAttribPointer(
			vertexNormal_modelspaceID,    // The attribute we want to configure
			3,                            // size
			GL_FLOAT,                     // type
			GL_FALSE,                     // normalized?
			0,                            // stride
			(void*)0                      // array buffer offset
			);

		glDrawArrays(GL_TRIANGLES, 0, vertices_table.size());

		glDisableVertexAttribArray(vertexPosition_modelspaceID);
		glDisableVertexAttribArray(vertexUVID);
		glDisableVertexAttribArray(vertexNormal_modelspaceID);

		//***************************

		// hit = 0 --> mole doesn't exist
		// hit = 1 --> mole exists and not hit
		// hit = 2 --> mole exists and hit

		// Logic for appearing and disappearing of moles
		//****************************
		
		lastTime = glfwGetTime();

		time = (int)lastTime - currentTime2;
		if (lastTime - currentTime > mole_speed)
		{  // if mole appears for (mole_speed) sec.
			currentTime = glfwGetTime();          // reset currentTime
			moles[mole_pos1].hit = 0;
			mole_pos1 = rand() % 9;
			moles[mole_pos1].hit = 1;

			if (score <= threshold1)
			{
				moles[mole_pos1].type = mole_type;
				mole_type = rand() % 2;
			}
			else if (score <= threshold2)
			{
				moles[mole_pos2].hit = 0;
				mole_pos2 = rand() % 9;
				moles[mole_pos2].hit = 1;

				if (mole_type == 0)
				{
					moles[mole_pos2].type = 0;
				}
				else
				{
					moles[mole_pos2].type = 1;
				}
				mole_type = rand() % 2;
			}
			else
			{
				moles[mole_pos2].hit = 0;
				mole_pos2 = rand() % 9;
				moles[mole_pos2].hit = 1;

				if (mole_type == 0)
				{
					moles[mole_pos2].type = 0;
				}
				else
				{
					moles[mole_pos2].type = 1;
				}

				mole_type = rand() % 2;
				mole_speed = 0.5;
			}
			std::cout << "Score: " << score << " HP: " << hp << std::endl;

		}
		//*********************************

		for (int i = 0; i < 9; i++)
		{
			if (moles[i].hit == 1)
			{
                moles[i].y = 2;
				glm::mat4 TranslationMatrix_Mole = translate(mat4(), vec3(moles[i].x, moles[i].y, moles[i].z));
				glm::mat4 RotationMatrix_Mole = eulerAngleYXZ(0.0f, 0.0f, 0.0f);
				glm::mat4 ScalingMatrix_Mole = scale(mat4(), vec3(1.5f, 1.5f, 1.5f));
				glm::mat4 ModelMatrix_Mole = TranslationMatrix_Mole * RotationMatrix_Mole * ScalingMatrix_Mole;

				glm::mat4 MVP_Mole = ProjectionMatrix * ViewMatrix * ModelMatrix_Mole;

				glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP_Mole[0][0]);
				glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix_Mole[0][0]);
				glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

				glActiveTexture(GL_TEXTURE0);

				if (moles[i].type == 0)
					glBindTexture(GL_TEXTURE_2D, Texture_Mole);
				else
					glBindTexture(GL_TEXTURE_2D, Texture_evilMole);

				glUniform1i(TextureID, 0);

				// 1rst attribute buffer : vertices
				glEnableVertexAttribArray(vertexPosition_modelspaceID);
				glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer_mole);
				glVertexAttribPointer(
					vertexPosition_modelspaceID,  // The attribute we want to configure
					3,                            // size
					GL_FLOAT,                     // type
					GL_FALSE,                     // normalized?
					0,                            // stride
					(void*)0                      // array buffer offset
					);

				// 2nd attribute buffer : UVs
				glEnableVertexAttribArray(vertexUVID);
				glBindBuffer(GL_ARRAY_BUFFER, uvbuffer_mole);
				glVertexAttribPointer(
					vertexUVID,                   // The attribute we want to configure
					2,                            // size : U+V => 2
					GL_FLOAT,                     // type
					GL_FALSE,                     // normalized?
					0,                            // stride
					(void*)0                      // array buffer offset
					);

				// 3rd attribute buffer : normals
				glEnableVertexAttribArray(vertexNormal_modelspaceID);
				glBindBuffer(GL_ARRAY_BUFFER, normalbuffer_mole);
				glVertexAttribPointer(
					vertexNormal_modelspaceID,    // The attribute we want to configure
					3,                            // size
					GL_FLOAT,                     // type
					GL_FALSE,                     // normalized?
					0,                            // stride
					(void*)0                      // array buffer offset
					);

				glDrawArrays(GL_TRIANGLES, 0, vertices_mole.size());

				glDisableVertexAttribArray(vertexPosition_modelspaceID);
				glDisableVertexAttribArray(vertexUVID);
				glDisableVertexAttribArray(vertexNormal_modelspaceID);
			}
		}


		// MVP & drawing for hammer
		//***************************

		// Code for Linking Mouse Motion with Hammer
		// ***************************

		glfwDisable(GLFW_MOUSE_CURSOR);

		float mouseSpeed = 0.03f;

		int xpos, ypos;

		glfwGetMousePos(&xpos, &ypos);

		float xhammer, yhammer;

		xhammer = (1024 / 2 - xpos)*mouseSpeed;
		yhammer = (768 / 2 - ypos)*mouseSpeed;

		if (xhammer < -6)
		{
			xhammer = -6.0f;
			glfwSetMousePos(1024 / 2 - xhammer / mouseSpeed, ypos);
		}
		else if (xhammer > 6)
		{
			xhammer = 6.0f;
			glfwSetMousePos(1024 / 2 - xhammer / mouseSpeed, ypos);
		}
		else if (yhammer > 8)
		{
			yhammer = 8.0f;
			glfwSetMousePos(xpos, 768 / 2 - yhammer / mouseSpeed);
		}
		else if (yhammer <-6)
		{
			yhammer = -6.0f;
			glfwSetMousePos(xpos, 768 / 2 - yhammer / mouseSpeed);
		}

		h.x = 4.0f - xhammer;
		h.y = 4.0f;
		h.z = -6.0f - yhammer;

		glm::mat4 TranslationMatrix_Hammer = translate(mat4(), vec3(h.x, h.y, h.z));

		// ***************************

		// Rotation of Hammer when Smacking
		// ***************************

		glm::mat4 RotationMatrix_Hammer;

		if (glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		{

			h.y += 1.2f;
			RotationMatrix_Hammer = eulerAngleYXZ(-0.5f, 0.0f, 1.5f);
			TranslationMatrix_Hammer = translate(mat4(), vec3(h.x, h.y, h.z));

			for (int i = 0; i < 9; i++)
			{
                if (moles[i].hit == 1 && (h.y - 3.2 <= moles[i].y && (h.x - 4 <= moles[i].x + 1.75 && h.x - 4 >= moles[i].x - 1.75) && (h.z + 6 <= moles[i].z + 10.5 && h.z + 6 >= moles[i].z + 6.9)) && flag)
				{
                    if (moles[i].type == 0)
                    {
                        score += 10;
                    }
                    else
                    {
                        hp -= 10;
                    }

                    moles[i].hit = 0;
					break;
				}
			}

			flag = false;
		}
		else
		{
			RotationMatrix_Hammer = eulerAngleYXZ(-0.5f, 0.0f, 1.0f);
			flag = true;
		}

		// ***************************

		glm::mat4 ScalingMatrix_Hammer = scale(mat4(), vec3(0.06f, 0.06f, 0.06f));
		glm::mat4 ModelMatrix_Hammer = TranslationMatrix_Hammer * RotationMatrix_Hammer * ScalingMatrix_Hammer;

		glm::mat4 MVP_Hammer = ProjectionMatrix * ViewMatrix * ModelMatrix_Hammer;

		//*************************************************
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP_Hammer[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix_Hammer[0][0]);
		glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture_Hammer);

		glUniform1i(TextureID, 0);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(vertexPosition_modelspaceID);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer_hammer);

		glVertexAttribPointer(
			vertexPosition_modelspaceID,  // The attribute we want to configure
			3,                            // size
			GL_FLOAT,                     // type
			GL_FALSE,                     // normalized?
			0,                            // stride
			(void*)0                      // array buffer offset
			);

		// 2nd attribute buffer : UVs
		glEnableVertexAttribArray(vertexUVID);

		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer_hammer);
		glVertexAttribPointer(
			vertexUVID,                   // The attribute we want to configure
			2,                            // size : U+V => 2
			GL_FLOAT,                     // type
			GL_FALSE,                     // normalized?
			0,                            // stride
			(void*)0                      // array buffer offset
			);

		// 3rd attribute buffer : normals
		glEnableVertexAttribArray(vertexNormal_modelspaceID);
		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer_hammer);
		glVertexAttribPointer(
			vertexNormal_modelspaceID,    // The attribute we want to configure
			3,                            // size
			GL_FLOAT,                     // type
			GL_FALSE,                     // normalized?
			0,                            // stride
			(void*)0                      // array buffer offset
			);

		glDrawArrays(GL_TRIANGLES, 0, vertices_hammer.size());

		glDisableVertexAttribArray(vertexPosition_modelspaceID);
		glDisableVertexAttribArray(vertexUVID);
		glDisableVertexAttribArray(vertexNormal_modelspaceID);

		//***************************
        sprintf(score_text, "%d", score);
        sprintf(hp_text, "%d", hp);
        sprintf(time_text, "%d", time_limit - time);
        printText2D(msg1, 602, 550, 20);
        printText2D(score_text, 730, 550, 20);
        printText2D(msg2, 10, 550, 20);
        printText2D(hp_text, 70, 550, 20);
        printText2D(msg5, 296, 550, 20);
        printText2D(time_text, 400, 550, 20);

        if (hp == 0)
        {
            while(1)
            {
                //std::cout << "You lose with a score of " << score << std::endl;
                printText2D(msg4, 70 + xs, ys, 25);
                printText2D(score_text, 570 + xs, ys, 25);
                glfwSwapBuffers();

                if(glfwGetKey(GLFW_KEY_ESC) == GLFW_PRESS)
                {
                    flag1 = false;
                    break;
                }
            }
        }
        else if(lastTime - currentTime2 > time_limit)
        {
            while(1)
            {
                //std::cout << "You win with a score of " << score << std::endl;
                printText2D(msg3, 50 + xs, ys, 25);
                printText2D(score_text, 550 + xs, ys, 25);
                glfwSwapBuffers();

                if(glfwGetKey(GLFW_KEY_ESC) == GLFW_PRESS)
                {
                    flag1 = false;
                    break;
                }
            }
        }
		
		// Swap buffers
		glfwSwapBuffers();
		
	} // Check if the ESC key was pressed or the window was closed
    while (flag1 && glfwGetKey(GLFW_KEY_ESC) != GLFW_PRESS);

	// Cleanup VBO and shader
	glDeleteProgram(programID);

	//Clean up hammer
	glDeleteBuffers(1, &vertexbuffer_hammer);
	glDeleteBuffers(1, &uvbuffer_hammer);
	glDeleteBuffers(1, &normalbuffer_hammer);
	glDeleteTextures(1, &Texture_Hammer);
	cleanupText2D();
	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}
