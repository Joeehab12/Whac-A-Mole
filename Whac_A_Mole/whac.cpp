// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GL/glfw.h>

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

int main( void )
{
	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		return -1;
	}

	glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 4);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 2);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 1);

	// Open a window and create its OpenGL context
	if( !glfwOpenWindow( 1024, 768, 0,0,0,0, 32,0, GLFW_WINDOW ) )
	{
		fprintf( stderr, "Failed to open GLFW window.\n" );
		glfwTerminate();
		return -1;
	}

	// Initialize GLEW
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

    glfwSetWindowTitle( "Whac-A-Mole" );

	// Ensure we can capture the escape key being pressed below
	glfwEnable( GLFW_STICKY_KEYS );
	glfwSetMousePos(1024/2, 768/2);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS); 

	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders( "StandardShading.vertexshader", "StandardShading.fragmentshader" );

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
    GLuint TextureID  = glGetUniformLocation(programID, "myTextureSampler");

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

	do{

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);

        glm::vec3 lightPos = glm::vec3(0,9,-8);
        glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);

        computeMatricesFromInputs();
        glm::mat4 ProjectionMatrix = getProjectionMatrix();
        glm::mat4 ViewMatrix = getViewMatrix();

        // MVP & drawing for table
        //***************************

        glm::mat4 RotationMatrix_Table = eulerAngleYXZ(0.0f, 0.0f,0.0f);
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

        glDrawArrays(GL_TRIANGLES, 0, vertices_table.size() );

        glDisableVertexAttribArray(vertexPosition_modelspaceID);
        glDisableVertexAttribArray(vertexUVID);
        glDisableVertexAttribArray(vertexNormal_modelspaceID);

        //***************************

        // MVP & drawing for Mole
        //***************************

        glm::mat4 RotationMatrix_Mole = eulerAngleYXZ(0.0f, 0.0f,0.0f);
        glm::mat4 TranslationMatrix_Mole = translate(mat4(), vec3(0.0f, 2.0f, -9.0f));
        glm::mat4 ScalingMatrix_Mole = scale(mat4(), vec3(1.5f, 1.5f, 1.5f));
        glm::mat4 ModelMatrix_Mole = TranslationMatrix_Mole * RotationMatrix_Mole * ScalingMatrix_Mole;

        glm::mat4 MVP_Mole = ProjectionMatrix * ViewMatrix * ModelMatrix_Mole;

        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP_Mole[0][0]);
        glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix_Mole[0][0]);
        glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Texture_Mole);

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

        glDrawArrays(GL_TRIANGLES, 0, vertices_mole.size() );

        glDisableVertexAttribArray(vertexPosition_modelspaceID);
        glDisableVertexAttribArray(vertexUVID);
        glDisableVertexAttribArray(vertexNormal_modelspaceID);

        //***************************

        // MVP & drawing for Evil Mole
        //***************************

        glm::mat4 RotationMatrix_evilMole= eulerAngleYXZ(0.0f, 0.0f,0.0f);
        glm::mat4 TranslationMatrix_evilMole = translate(mat4(), vec3(-3.5f, 1.8f, -5.5f));
        glm::mat4 ScalingMatrix_evilMole = scale(mat4(), vec3(0.014f, 0.014f, 0.014f));
        glm::mat4 ModelMatrix_evilMole = TranslationMatrix_evilMole * RotationMatrix_evilMole * ScalingMatrix_evilMole;

        glm::mat4 MVP_evilMole = ProjectionMatrix * ViewMatrix * ModelMatrix_evilMole;

        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP_evilMole[0][0]);
        glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix_evilMole[0][0]);
        glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Texture_evilMole);

        glUniform1i(TextureID, 0);

        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(vertexPosition_modelspaceID);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer_evilMole);
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
        glBindBuffer(GL_ARRAY_BUFFER, uvbuffer_evilMole);
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
        glBindBuffer(GL_ARRAY_BUFFER, normalbuffer_evilMole);
        glVertexAttribPointer(
            vertexNormal_modelspaceID,    // The attribute we want to configure
            3,                            // size
            GL_FLOAT,                     // type
            GL_FALSE,                     // normalized?
            0,                            // stride
            (void*)0                      // array buffer offset
        );

        glDrawArrays(GL_TRIANGLES, 0, vertices_evilMole.size() );

        glDisableVertexAttribArray(vertexPosition_modelspaceID);
        glDisableVertexAttribArray(vertexUVID);
        glDisableVertexAttribArray(vertexNormal_modelspaceID);

        //***************************

        // MVP & drawing for hammer
        //***************************
	
		
	//Code for Linking Mouse Motion with Hammer
        //***************************
        static double lastTime = glfwGetTime();

        float mouseSpeed = 3.0f;
        double currentTime = glfwGetTime();
        float deltaTime = float(currentTime - lastTime);
        int xpos,ypos;
        glfwGetMousePos(&xpos, &ypos);
        float xhammer,yhammer;
        xhammer =  (1024/2-xpos)*deltaTime*mouseSpeed;
        yhammer =  (768/2-ypos)*deltaTime*mouseSpeed;
        if (xhammer < -2){
            xhammer = -2.0f;
            glfwSetMousePos(1024/2-xhammer/(deltaTime*mouseSpeed),ypos);
        }
        else if (xhammer > 9){
            xhammer = 9.0f;
            glfwSetMousePos(1024/2-xhammer/(deltaTime*mouseSpeed),ypos);
        }
        else if (yhammer > 8){
            yhammer = 8.0f;
            glfwSetMousePos(xpos,768/2-yhammer/(deltaTime*mouseSpeed));
        }
        else if (yhammer <-6){
            yhammer = -6.0f;
            glfwSetMousePos(xpos,768/2-yhammer/(deltaTime*mouseSpeed));
        }



        glm::mat4 RotationMatrix_Hammer = eulerAngleYXZ(0.0f,0.0f,1.0f);
        glm::mat4 TranslationMatrix_Hammer = translate(mat4(), vec3(6.0f - xhammer, 3.0f, -9.0f - yhammer));
        glm::mat4 ScalingMatrix_Hammer = scale(mat4(), vec3(0.06f, 0.06f, 0.06f));
        glm::mat4 ModelMatrix_Hammer = TranslationMatrix_Hammer * RotationMatrix_Hammer * ScalingMatrix_Hammer;


        glm::mat4 MVP_Hammer = ProjectionMatrix * ViewMatrix * ModelMatrix_Hammer;
        

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

        glDrawArrays(GL_TRIANGLES, 0, vertices_hammer.size() );

		glDisableVertexAttribArray(vertexPosition_modelspaceID);
		glDisableVertexAttribArray(vertexUVID);
		glDisableVertexAttribArray(vertexNormal_modelspaceID);

        //***************************

		// Swap buffers
		glfwSwapBuffers();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey( GLFW_KEY_ESC ) != GLFW_PRESS &&
		   glfwGetWindowParam( GLFW_OPENED ) );

	// Cleanup VBO and shader
    glDeleteProgram(programID);

    //Clean up hammer
    glDeleteBuffers(1, &vertexbuffer_hammer);
    glDeleteBuffers(1, &uvbuffer_hammer);
    glDeleteBuffers(1, &normalbuffer_hammer);
    glDeleteTextures(1, &Texture_Hammer);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

