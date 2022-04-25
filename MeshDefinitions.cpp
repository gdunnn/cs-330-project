
#include <GL/glew.h>        // GLEW library
#include <GLFW/glfw3.h>     // GLFW library


#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>


// ===========================================================================================================================================
// PlaneTable mesh
GLfloat PlaneTableverts[] = {
         //Vertex Positions				//Normals							//Texture Coordinates
		// Rear face
        -0.5f, -0.5f, -0.5f,			0.0f,0.0f,-1.0f,					0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,			0.0f,0.0f,-1.0f,					1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,			0.0f,0.0f,-1.0f,					1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,			0.0f,0.0f,-1.0f,					1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,			0.0f,0.0f,-1.0f,					0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,			0.0f,0.0f,-1.0f,					0.0f, 0.0f,

		// Front face
        -0.5f, -0.5f,  0.5f,			0.0f,0.0f,1.0f,						0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,			0.0f,0.0f,1.0f,						1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,			0.0f,0.0f,1.0f,						1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,			0.0f,0.0f,1.0f,						1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,			0.0f,0.0f,1.0f,						0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,			0.0f,0.0f,1.0f,						0.0f, 0.0f,

		// Left face
        -0.5f,  0.5f,  0.5f,			-1.0f,0.0f,0.0f,					1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,			-1.0f,0.0f,0.0f,					1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,			-1.0f,0.0f,0.0f,					0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,			-1.0f,0.0f,0.0f,					0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,			-1.0f,0.0f,0.0f,					0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,			-1.0f,0.0f,0.0f,					1.0f, 0.0f,

		// Right face
         0.5f,  0.5f,  0.5f,			1.0f,0.0f,0.0f,						1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,			1.0f,0.0f,0.0f,						1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,			1.0f,0.0f,0.0f,						0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,			1.0f,0.0f,0.0f,						0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,			1.0f,0.0f,0.0f,						0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,			1.0f,0.0f,0.0f,						1.0f, 0.0f,

		// Bottom face
        -0.5f, -0.5f, -0.5f,			0.0f,-1.0f,0.0f,					0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,			0.0f,-1.0f,0.0f,					1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,			0.0f,-1.0f,0.0f,					1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,			0.0f,-1.0f,0.0f,					1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,			0.0f,-1.0f,0.0f,					0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,			0.0f,-1.0f,0.0f,					0.0f, 1.0f,

		// Top face
        -0.5f,  0.5f, -0.5f,			0.0f,1.0f,0.0f,						0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,			0.0f,1.0f,0.0f,						1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,			0.0f,1.0f,0.0f,						1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,			0.0f,1.0f,0.0f,						1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,			0.0f,1.0f,0.0f,						0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,			0.0f,1.0f,0.0f,						0.0f, 1.0f
};
// ===========================================================================================================================================


// ===========================================================================================================================================
// Speaker mesh
GLfloat SpeakerMeshverts[] = {
         //Vertex Positions				//Normals							//Texture Coordinates
		 // Rear face on the negative Z - this face use the right side of the texture
        -0.5f, -0.5f, -0.5f,			0.0f,0.0f,-1.0f,					0.5f, 1.0f,							// 0
         0.5f, -0.5f, -0.5f,			0.0f,0.0f,-1.0f,					1.0f, 1.0f,							// 1
         0.5f,  0.5f, -0.5f,			0.0f,0.0f,-1.0f,					1.0f, 0.0f,							// 2
         0.5f,  0.5f, -0.5f,			0.0f,0.0f,-1.0f,					1.0f, 0.0f,							// 3
        -0.5f,  0.5f, -0.5f,			0.0f,0.0f,-1.0f,					0.5f, 0.0f,							// 4
        -0.5f, -0.5f, -0.5f,			0.0f,0.0f,-1.0f,					0.5f, 1.0f,							// 5

		 // Front face on the positive Z - this face use the LEFT side of the texture
        -0.5f, -0.5f,  0.5f,			0.0f,0.0f,1.0f,						0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,			0.0f,0.0f,1.0f,						0.5f, 1.0f,
         0.5f,  0.5f,  0.5f,			0.0f,0.0f,1.0f,						0.5f, 0.0f,
         0.5f,  0.5f,  0.5f,			0.0f,0.0f,1.0f,						0.5f, 0.0f,
        -0.5f,  0.5f,  0.5f,			0.0f,0.0f,1.0f,						0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,			0.0f,0.0f,1.0f,						0.0f, 1.0f,							// 11

		// Left face on the negative X - this face use the right side of the texture
        -0.5f,  0.5f,  0.5f,			-1.0f,0.0f,0.0f,					0.5f, 1.0f,	
        -0.5f,  0.5f, -0.5f,			-1.0f,0.0f,0.0f,					1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,			-1.0f,0.0f,0.0f,					1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,			-1.0f,0.0f,0.0f,					1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,			-1.0f,0.0f,0.0f,					0.5f, 0.0f,
        -0.5f,  0.5f,  0.5f,			-1.0f,0.0f,0.0f,					0.5f, 1.0f,

		// Right face on the positive X - this face use the right side of the texture
         0.5f,  0.5f,  0.5f,			1.0f,0.0f,0.0f,						0.5f, 1.0f,
         0.5f,  0.5f, -0.5f,			1.0f,0.0f,0.0f,						1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,			1.0f,0.0f,0.0f,						1.0f, 0.0f,
         0.5f, -0.5f, -0.5f,			1.0f,0.0f,0.0f,						1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,			1.0f,0.0f,0.0f,						0.5f, 0.0f,
         0.5f,  0.5f,  0.5f,			1.0f,0.0f,0.0f,						0.5f, 1.0f,

		// Botton face on the negative Y - this face use the right side of the texture
        -0.5f, -0.5f, -0.5f,			0.0f,-1.0f,0.0f,					0.5f, 1.0f,
         0.5f, -0.5f, -0.5f,			0.0f,-1.0f,0.0f,					1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,			0.0f,-1.0f,0.0f,					1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,			0.0f,-1.0f,0.0f,					1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,			0.0f,-1.0f,0.0f,					0.5f, 0.0f,
        -0.5f, -0.5f, -0.5f,			0.0f,-1.0f,0.0f,					0.5f, 1.0f,

		// Top face on the positive Y  - this face use the right side of the texture
        -0.5f,  0.5f, -0.5f,			0.0f,1.0f,0.0f,						0.5f, 1.0f,
         0.5f,  0.5f, -0.5f,			0.0f,1.0f,0.0f,						1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,			0.0f,1.0f,0.0f,						1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,			0.0f,1.0f,0.0f,						1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,			0.0f,1.0f,0.0f,						0.5f, 0.0f,
        -0.5f,  0.5f, -0.5f,			0.0f,1.0f,0.0f,						0.5f, 1.0f
};
// ===========================================================================================================================================



// ===========================================================================================================================================
// Screen mesh
GLfloat ScreenMeshverts[] = {
         //Vertex Positions				//Normals							//Texture Coordinates
		 // Rear face on the negative Z - this face use the right side of the texture
        -0.5f, -0.5f, -0.5f,			0.0f,0.0f,-1.0f,					0.5f, 1.0f,							// 0
         0.5f, -0.5f, -0.5f,			0.0f,0.0f,-1.0f,					1.0f, 1.0f,							// 1
         0.5f,  0.5f, -0.5f,			0.0f,0.0f,-1.0f,					1.0f, 0.0f,							// 2
         0.5f,  0.5f, -0.5f,			0.0f,0.0f,-1.0f,					1.0f, 0.0f,							// 3
        -0.5f,  0.5f, -0.5f,			0.0f,0.0f,-1.0f,					0.5f, 0.0f,							// 4
        -0.5f, -0.5f, -0.5f,			0.0f,0.0f,-1.0f,					0.5f, 1.0f,							// 5

		 // Front face on the positive Z - this face use the LEFT side of the texture
        -0.5f, -0.5f,  0.5f,			0.0f,0.0f,1.0f,						0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,			0.0f,0.0f,1.0f,						0.5f, 1.0f,
         0.5f,  0.5f,  0.5f,			0.0f,0.0f,1.0f,						0.5f, 0.0f,
         0.5f,  0.5f,  0.5f,			0.0f,0.0f,1.0f,						0.5f, 0.0f,
        -0.5f,  0.5f,  0.5f,			0.0f,0.0f,1.0f,						0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,			0.0f,0.0f,1.0f,						0.0f, 1.0f,							// 11

		// Left face on the negative X - this face use the right side of the texture
        -0.5f,  0.5f,  0.5f,			-1.0f,0.0f,0.0f,					0.5f, 1.0f,	
        -0.5f,  0.5f, -0.5f,			-1.0f,0.0f,0.0f,					1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,			-1.0f,0.0f,0.0f,					1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,			-1.0f,0.0f,0.0f,					1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,			-1.0f,0.0f,0.0f,					0.5f, 0.0f,
        -0.5f,  0.5f,  0.5f,			-1.0f,0.0f,0.0f,					0.5f, 1.0f,

		// Right face on the positive X - this face use the right side of the texture
         0.5f,  0.5f,  0.5f,			1.0f,0.0f,0.0f,						0.5f, 1.0f,
         0.5f,  0.5f, -0.5f,			1.0f,0.0f,0.0f,						1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,			1.0f,0.0f,0.0f,						1.0f, 0.0f,
         0.5f, -0.5f, -0.5f,			1.0f,0.0f,0.0f,						1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,			1.0f,0.0f,0.0f,						0.5f, 0.0f,
         0.5f,  0.5f,  0.5f,			1.0f,0.0f,0.0f,						0.5f, 1.0f,

		// Botton face on the negative Y - this face use the right side of the texture
        -0.5f, -0.5f, -0.5f,			0.0f,-1.0f,0.0f,					0.5f, 1.0f,
         0.5f, -0.5f, -0.5f,			0.0f,-1.0f,0.0f,					1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,			0.0f,-1.0f,0.0f,					1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,			0.0f,-1.0f,0.0f,					1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,			0.0f,-1.0f,0.0f,					0.5f, 0.0f,
        -0.5f, -0.5f, -0.5f,			0.0f,-1.0f,0.0f,					0.5f, 1.0f,

		// Top face on the positive Y  - this face use the right side of the texture
        -0.5f,  0.5f, -0.5f,			0.0f,1.0f,0.0f,						0.5f, 1.0f,
         0.5f,  0.5f, -0.5f,			0.0f,1.0f,0.0f,						1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,			0.0f,1.0f,0.0f,						1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,			0.0f,1.0f,0.0f,						1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,			0.0f,1.0f,0.0f,						0.5f, 0.0f,
        -0.5f,  0.5f, -0.5f,			0.0f,1.0f,0.0f,						0.5f, 1.0f
};
// ===========================================================================================================================================




// ===========================================================================================================================================
// ScreenArm1 mesh (this is a simple cube,with standard UV mapping,so it can be multiple used for many mesh)
GLfloat ScreenArm1verts[] = {
         //Vertex Positions				//Normals							//Texture Coordinates
        -0.5f, -0.5f, -0.5f,			0.0f,0.0f,-1.0f,					0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,			0.0f,0.0f,-1.0f,					1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,			0.0f,0.0f,-1.0f,					1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,			0.0f,0.0f,-1.0f,					1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,			0.0f,0.0f,-1.0f,					0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,			0.0f,0.0f,-1.0f,					0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,			0.0f,0.0f,1.0f,						0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,			0.0f,0.0f,1.0f,						1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,			0.0f,0.0f,1.0f,						1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,			0.0f,0.0f,1.0f,						1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,			0.0f,0.0f,1.0f,						0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,			0.0f,0.0f,1.0f,						0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,			-1.0f,0.0f,0.0f,					1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,			-1.0f,0.0f,0.0f,					1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,			-1.0f,0.0f,0.0f,					0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,			-1.0f,0.0f,0.0f,					0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,			-1.0f,0.0f,0.0f,					0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,			-1.0f,0.0f,0.0f,					1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,			1.0f,0.0f,0.0f,						1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,			1.0f,0.0f,0.0f,						1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,			1.0f,0.0f,0.0f,						0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,			1.0f,0.0f,0.0f,						0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,			1.0f,0.0f,0.0f,						0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,			1.0f,0.0f,0.0f,						1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,			0.0f,-1.0f,0.0f,					0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,			0.0f,-1.0f,0.0f,					1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,			0.0f,-1.0f,0.0f,					1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,			0.0f,-1.0f,0.0f,					1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,			0.0f,-1.0f,0.0f,					0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,			0.0f,-1.0f,0.0f,					0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,			0.0f,1.0f,0.0f,						0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,			0.0f,1.0f,0.0f,						1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,			0.0f,1.0f,0.0f,						1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,			0.0f,1.0f,0.0f,						1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,			0.0f,1.0f,0.0f,						0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,			0.0f,1.0f,0.0f,						0.0f, 1.0f
};
// ===========================================================================================================================================