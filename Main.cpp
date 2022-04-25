
/*
TEXTURE LAYERS
Inside structure M_MESH the TexturesID[] array contains the currently active textures, and BindedTexturesNum the number. For now we support up to 
M_MESH_MAXTEXTURELAYERS layers which are more than enough but can be extended easily with a few lines of code.
After initializing a mesh with M_InitMesh() you will add as many textures you want using the M_MeshAddTextureLayer() function, please note that 
there must be at least 1 for each mesh(see the code in Main module to understand it) else nothing will be drawn.
Therefore the implemented system in M_Mesh module supports simple and advanced blending of the different layers,as described below(all the currently
implemented features are described below).

PIXEL DISCARD TEXTURE OVERLAP
As a first feature the shader supports the texture overlap on the same mesh: when more textures overlap, the lower layer is always drawn, 
while for the upper ones each pixel(texel) is drawn only if its alpha IS NOT COMPLETELY TRANSPARENT, if instead the alpha is 100% transparent 
the pixel is discarded and as a consequence the pixel of the lower layer remains drawn. This is the simplest technique and is referred by somebody 
as "pixel discard".
*/

#include <iostream>         // cout, cerr
#include <cstdlib>          // EXIT_FAILURE
#include <GL/glew.h>        // GLEW library
#include <GLFW/glfw3.h>     // GLFW library

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnOpengl/camera.h> // Camera class

#include "Main.h"
#include "M_Mesh.h"

#include "MeshDefinitions.h"

using namespace std; // Standard namespace

/*Shader program Macro*/
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version " core \n" #Source
#endif

const char* const WINDOW_TITLE = "3D SCENE WITH LIGHTS"; 

// Variables for window width and height
const int WINDOW_WIDTH = 1200;
const int WINDOW_HEIGHT = 600;


/* Vertex Shader Source Code*/
const GLchar* vertexShaderSource = GLSL(440,

	layout(location = 0) in vec3 position; // Vertex data from Vertex Attrib Pointer 0
	layout (location = 1) in vec3 InNormal;
	layout (location = 2) in vec2 textureCoordinate;

	out vec2 IOvertexTextureCoordinate;
	out vec3 IONormal;
	out vec3 IOFragmentWorldPosition;

	//Global variables for the  transform matrices
	uniform mat4 model;
	uniform mat4 view;
	uniform mat4 projection;
 
	void main()
	{
		gl_Position = projection * view * model * vec4(position, 1.0f); // transforms vertices to clip coordinates

		// Get normal vectors in world space only and exclude normal translation properties
		IONormal = mat3(transpose(inverse(model))) * InNormal;	// This is gpu cycles heavy but necessary for now

		IOvertexTextureCoordinate = textureCoordinate;

		// We're going to do all the lighting calculations(in the fragment shader) in world space so we need to pass this vertex in world position 
		// only to the fragment shader(the the fragment shader will receive the 3 vertex interpolate and will output 1 single fragment in world position).
		// We can accomplish this by multiplying the vertex position by model only (not the view and projection matrix) to transform 
		// it to world space only. 
		IOFragmentWorldPosition = vec3(model * vec4(position, 1.0f));
	}
);

/* Fragment Shader Source Code*/
const GLchar* fragmentShaderSource = GLSL(440,

const int LIGHTSNUM = 4;					// !!!This must be identical to G_LIGHTSNUM inside the Main.cpp!

struct LIGHTSTRUCT
{
	vec3 Position;
	vec3 Color;
	float SpecularIntensity;
	float SpecularHighlightSize;
};

	in vec3 IONormal;  
	in vec3 IOFragmentWorldPosition;		// This is this fragment position but in world position only (not completely transformed)
	in vec2 IOvertexTextureCoordinate;
	out vec4 fragmentColor;

    uniform vec3 viewPosition;
	uniform LIGHTSTRUCT LightsArray[LIGHTSNUM];

	// We support max 4 texture layers for now
    uniform int uNumActiveTextures;
    uniform sampler2D uTexture0;
    uniform sampler2D uTexture1;
    uniform sampler2D uTexture2;
    uniform sampler2D uTexture3;

	void main()
	{
		// === Ambient lighting (this is 1 single light,the global ambient illumination) ===
		// For this implementation the ambient illumination its considered to add once per fragment (not like a light but like a global illumination)
        float ambientStrength = 0.0f; // Set ambient or global lighting strength
		vec3 ambientColor = vec3(1.0f,1.0f,1.0f);
        vec3 ambientCOMPONENT = ambientStrength * ambientColor; // Generate ambient light color

		// For each existing light we sum them together and obtain the final fragment (enlightened by all existing lights)
		vec3 FINALresultpixel = vec3(0.0f,0.0f,0.0f);
		vec4 texturePixel;
		for(int i = 0; i < LIGHTSNUM; i++)
		{
			// === Diffuse component ===
			// Here we calculate the direction vector between the light source (in world space) and the fragment's world position
			vec3 norm = normalize(IONormal);	// This is the normal of this fragment,should be already normalized but we normalize it
			vec3 lightDirection = normalize(LightsArray[i].Position-IOFragmentWorldPosition); 
			// Here we calculate the diffuse impact on the current fragment by taking the dot product between the fragment's normal and lightDirection
			// The resulting value is then multiplied with the light's color to get the diffuse component, resulting in a darker diffuse component 
			// the greater the angle between both vectors
			float diffuseimpact = max(dot(norm,lightDirection),0.0);		// We use max to never go negative values
			vec3 diffuseCOMPONENT = diffuseimpact * LightsArray[i].Color; 

			// === Specular component ===
			vec3 viewDir = normalize(viewPosition - IOFragmentWorldPosition); // Calculate view direction
			vec3 reflectDir = reflect(-lightDirection, norm);// Calculate reflection vector
			//Calculate specular component
			float specularC = pow(max(dot(viewDir, reflectDir), 0.0), LightsArray[i].SpecularHighlightSize);
			vec3 specularCOMPONENT = LightsArray[i].SpecularIntensity * specularC * LightsArray[i].Color;

			// === Here we sum the light components to obtain the final pixel color ===
			// Here we take the pixel from the texture,so the color without lighting applied yet
			// texture() takes as input a `Sampler2D` and a `vec2` with the texture coordinates, 
			// and returns the bilinearly interpolated texel value at those coordinates.
			// Notice that we have MULTILAYERED TEXTURES

			// Texture 1 (always present)
			texturePixel= texture(uTexture0,IOvertexTextureCoordinate);

			// Texture 2 (if enabled)
			if (uNumActiveTextures>=2)
			{
				vec4 extraTexture1 = texture(uTexture1,IOvertexTextureCoordinate);
				if (extraTexture1.a != 0.0)
					texturePixel = extraTexture1;
			}

			// Texture 3 (if enabled)
			if (uNumActiveTextures>=3)
			{
				vec4 extraTexture2 = texture(uTexture2,IOvertexTextureCoordinate);
				if (extraTexture2.a != 0.0)
					texturePixel = extraTexture2;
			}

			// Texture 4 (if enabled)
			if (uNumActiveTextures>=4)
			{
				vec4 extraTexture3 = texture(uTexture3,IOvertexTextureCoordinate);
				if (extraTexture3.a != 0.0)
					texturePixel = extraTexture3;
			}

			// Here we calculate the final color(pixel) with all the components multiplied by the texture color(pixel)
			vec3 resultpixel = (diffuseCOMPONENT + specularCOMPONENT) * texturePixel.xyz;

			// We add the final pixel FOR THIS LIGHT to the FINAL RESULT PIXEL
			FINALresultpixel += resultpixel;
		}

		// Now we add the ambient illumination (just once per fragment)
		FINALresultpixel += ambientCOMPONENT * texturePixel.xyz ;		// The ambient is added just ONCE per fragment 

		// Now we may sendout the final pixel(color) of this fragment
        fragmentColor = vec4(FINALresultpixel,1.0); 
	}

);


// ===================================================== GLOBAL VARIABLES

// Camera
Camera gCamera(glm::vec3(0.0f, 0.0f, 120.0f));					// This is the starting world position for the camera 
float gLastX = WINDOW_WIDTH / 2.0f;
float gLastY = WINDOW_HEIGHT / 2.0f;
bool gFirstMouse = true;

// Timing
float gDeltaTime = 0.0f; // time between current frame and last frame
float gLastFrame = 0.0f;

// Projection Type Switch
// The current projection type. 0=Perspective projection ; 1=Orthographic projection 
int gProjectionType = 0;			

// This is the array with all the existing lights. The number of elements (=G_LIGHTSNUM) MUST BE INIZIALIZED!!Because every element will be sent
// to the shader(ie. if G_LIGHTSNUM=2 you must inizialize both 2 elements in the array).
M_LIGHT gGlobalLights[G_LIGHTSNUM];

// ============================================================================================================================================

int main(int argc, char* argv[])
{
	GLuint M_ShaderProgramId;
	static bool M_PKeyPressed = false;

	GLuint Table_TextureId;
	GLuint Speaker_TextureId;
	GLuint ScreenTexture_TextureId;
	GLuint WhiteBluePlastic_TextureId;
	GLuint ScreenGlassLayer_TextureId;

	M_MESH Mesh_PlaneTable;
	M_MESH Mesh_SpeakerMesh1;
	M_MESH Mesh_SpeakerMesh2;

	// Monitor complex 3d object (those are the mesh who compose it)
	M_MESH Mesh_Screen;
	M_MESH Mesh_ScreenArm1;
	M_MESH Mesh_ScreenArm2;
	M_MESH Mesh_ScreenBase;


	// Main GLFW window
	GLFWwindow* window = nullptr;

	if (!UInitialize(argc, argv, &window))
		return EXIT_FAILURE;

	// =========================================================================================================================================
	// INIT
	// =========================================================================================================================================

	// Compile shader program
	if (!UCreateShaderProgram(vertexShaderSource, fragmentShaderSource, M_ShaderProgramId))
		return EXIT_FAILURE;

	// Setup camera
	gCamera.MovementSpeed = 60.0f;			// This is the start value
	gCamera.MouseSensitivity = 0.1f;

	// Load textures
	// These are the textures we will use in 3d objects in this program

	// Brick texture
	// Note that: the path its relative to the executable,so we come out from that dir and go inside the "DATA" dir
	if ( !M_LoadTexture("../DATA/tabletexture.png",Table_TextureId) )
	{
		cout << "Failed to load texture" << endl;
		return EXIT_FAILURE;
	}

	// Speaker texture
	if ( !M_LoadTexture("../DATA/speakertexture.png",Speaker_TextureId) )
	{
		cout << "Failed to load texture" << endl;
		return EXIT_FAILURE;
	}

	// White plastic texture
	if ( !M_LoadTexture("../DATA/screentexture.png",ScreenTexture_TextureId) )
	{
		cout << "Failed to load texture" << endl;
		return EXIT_FAILURE;
	}

	// White Blue plastic texture
	if ( !M_LoadTexture("../DATA/whiteblueplastictexture.png",WhiteBluePlastic_TextureId) )
	{
		cout << "Failed to load texture" << endl;
		return EXIT_FAILURE;
	}

	// Screen glass layer (this texture is half 100% transparent)
	if ( !M_LoadTexture("../DATA/screenglasslayer.png",ScreenGlassLayer_TextureId) )
	{
		cout << "Failed to load texture" << endl;
		return EXIT_FAILURE;
	}

	// === INIT LIGHTS ===
	// These are the initial values of all the existing lights in the program (they will be transferred to the shader as a copy)

	// Little green light on the right speaker
	gGlobalLights[0].Position = glm::vec3(36.0f,11.0f,25.0f);
	gGlobalLights[0].Color = glm::vec3(0.0f,0.6f,0.0f);
	gGlobalLights[0].SpecularIntensity = 1.0f;
	gGlobalLights[0].SpecularHighlightSize = 32.0f;

	// White light in the center of the table,light everything from behind
	gGlobalLights[1].Position = glm::vec3(1.8f,15.0f,-73.0f);
	gGlobalLights[1].Color = glm::vec3(1.0f,1.0f,1.0f);
	gGlobalLights[1].SpecularIntensity = 1.0f;
	gGlobalLights[1].SpecularHighlightSize = 8.0f;

	// Blue light in front of the monitor that light the scene from the front side
	gGlobalLights[2].Position = glm::vec3(1.8f,1.8f,74.0f);
	gGlobalLights[2].Color = glm::vec3(0.1f,0.1f,1.0f);
	gGlobalLights[2].SpecularIntensity = 1.0f;
	gGlobalLights[2].SpecularHighlightSize = 16.0f;

	// Little green light on the left speaker
	gGlobalLights[3].Position = glm::vec3(-36.0f,11.0f,25.0f);
	gGlobalLights[3].Color = glm::vec3(0.0f,0.6f,0.0f);
	gGlobalLights[3].SpecularIntensity = 1.0f;
	gGlobalLights[3].SpecularHighlightSize = 32.0f;

	// = PLANE TABLE OBJECT =
	// This is a single mesh 3d object wich represents the Table in the picture
	M_InitMesh(&Mesh_PlaneTable,PlaneTableverts,PLANETABLE_VERTEXNUM);
	M_MeshAddTextureLayer(&Mesh_PlaneTable,Table_TextureId);
	M_SetMeshScale(&Mesh_PlaneTable,150.0f,3.0f,100.0f);		// piu aumenti piu  ingrandisce
	M_SetMeshRotation(&Mesh_PlaneTable,0.0f, 1.0f, 0.0f, 0.0f);
	M_SetMeshWorldPosition(&Mesh_PlaneTable,0.0f,-14.0f,16.0f);
	Mesh_PlaneTable.RenderMode = 0;		// 0=Solid ; 1=Wireframe

	// = SPEAKER 1 3D OBJECT =
	// This is a single mesh 3d object wich represents one of the Speakers in the picture
	M_InitMesh(&Mesh_SpeakerMesh1,SpeakerMeshverts,SPEAKERMESH_VERTEXNUM);
	M_MeshAddTextureLayer(&Mesh_SpeakerMesh1,Speaker_TextureId);
	M_SetMeshScale(&Mesh_SpeakerMesh1,12.0f,15.0f,10.0f);		// piu aumenti piu  ingrandisce
	M_SetMeshRotation(&Mesh_SpeakerMesh1,0.0f, 1.0f, 0.0f, 0.0f);
	M_SetMeshWorldPosition(&Mesh_SpeakerMesh1,-35.0f,-4.2f,40.0f);
	Mesh_SpeakerMesh1.RenderMode = 0;		// 0=Solid ; 1=Wireframe

	// = SPEAKER 2 3D OBJECT =
	// This is a single mesh 3d object wich represents one of the Speakers in the picture
	M_InitMesh(&Mesh_SpeakerMesh2,SpeakerMeshverts,SPEAKERMESH_VERTEXNUM);
	M_MeshAddTextureLayer(&Mesh_SpeakerMesh2,Speaker_TextureId);
	M_SetMeshScale(&Mesh_SpeakerMesh2,12.0f,15.0f,10.0f);		// piu aumenti piu  ingrandisce
	M_SetMeshRotation(&Mesh_SpeakerMesh2,0.0f, 1.0f, 0.0f, 0.0f);
	M_SetMeshWorldPosition(&Mesh_SpeakerMesh2,35.0f,-4.2f,40.0f);
	Mesh_SpeakerMesh2.RenderMode = 0;		// 0=Solid ; 1=Wireframe

	// = MONITOR 3D COMPLEX OBJECT =
	// This is a Complex 3d Object in the meaning that is COMPOSED BY DIFFERENT MESH, it that represents the Monitor in the picture with the arm
	// and the supporting base.
	// Its built with all the following mesh

	// Screen Mesh (it is just the flat monitor)
	M_InitMesh(&Mesh_Screen,ScreenMeshverts,SCREENMESH_VERTEXNUM);
	M_MeshAddTextureLayer(&Mesh_Screen,ScreenTexture_TextureId);
	M_MeshAddTextureLayer(&Mesh_Screen,ScreenGlassLayer_TextureId);			// this layer will go upper the "normal" texture of the screen
	M_SetMeshScale(&Mesh_Screen,40.0f,27.0f,1.0f);
	M_SetMeshRotation(&Mesh_Screen,-0.1f, 1.0f, 0.0f, 0.0f);
	M_SetMeshWorldPosition(&Mesh_Screen,0.0f,6.0f,46.0f);
	Mesh_Screen.RenderMode = 0;		// 0=Solid ; 1=Wireframe

	// ScreenArm1 Mesh
	M_InitMesh(&Mesh_ScreenArm1,ScreenArm1verts,SCREENARM1_VERTEXNUM);
	M_MeshAddTextureLayer(&Mesh_ScreenArm1,WhiteBluePlastic_TextureId);
	M_SetMeshScale(&Mesh_ScreenArm1,13.0f,11.0f,10.0f);		// piu aumenti piu  ingrandisce
	M_SetMeshRotation(&Mesh_ScreenArm1,3.0f, 1.0f, 0.0f, 0.0f);
	M_SetMeshWorldPosition(&Mesh_ScreenArm1,0.0f,8.0f,39.5f);
	Mesh_ScreenArm1.RenderMode = 0;		// 0=Solid ; 1=Wireframe

	// ScreenArm2 Mesh
	M_InitMesh(&Mesh_ScreenArm2,ScreenArm1verts,SCREENARM1_VERTEXNUM);
	M_MeshAddTextureLayer(&Mesh_ScreenArm2,WhiteBluePlastic_TextureId);
	M_SetMeshScale(&Mesh_ScreenArm2,10.0f,17.0f,1.5f);		// piu aumenti piu  ingrandisce
	M_SetMeshRotation(&Mesh_ScreenArm2,0.0f, 1.0f, 0.0f, 0.0f);
	M_SetMeshWorldPosition(&Mesh_ScreenArm2,0.0f,-6.0f,39.5f);
	Mesh_ScreenArm2.RenderMode = 0;		// 0=Solid ; 1=Wireframe

	// ScreenBase Mesh
	M_InitMesh(&Mesh_ScreenBase,ScreenArm1verts,SCREENARM1_VERTEXNUM);
	M_MeshAddTextureLayer(&Mesh_ScreenBase,WhiteBluePlastic_TextureId);
	M_SetMeshScale(&Mesh_ScreenBase,15.0f,0.6f,20.0f);		// piu aumenti piu  ingrandisce
	M_SetMeshRotation(&Mesh_ScreenBase,0.0f, 1.0f, 0.0f, 0.0f);
	M_SetMeshWorldPosition(&Mesh_ScreenBase,0.0f,-12.0f,42.0f);
	Mesh_ScreenBase.RenderMode = 0;		// 0=Solid ; 1=Wireframe

	// =========================================================================================================================================
	// MAIN LOOP
	// =========================================================================================================================================
	while (!glfwWindowShouldClose(window))
	{
        float currentFrame = (float)glfwGetTime();
        gDeltaTime = currentFrame - gLastFrame;			// This is the deltatime: the time passed since last frame
        gLastFrame = currentFrame;

		// CHECK INPUT =======================================================================

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		// We use WASD to move the camera 
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			gCamera.ProcessKeyboard(FORWARD, gDeltaTime);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			gCamera.ProcessKeyboard(BACKWARD, gDeltaTime);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			gCamera.ProcessKeyboard(LEFT, gDeltaTime);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			gCamera.ProcessKeyboard(RIGHT, gDeltaTime);

		// We use Q and E to move camera upward and downward
		// Upward
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		{
			// What we do here its simple,we move the camera following the path of the Up vector
			// Wich is a prebuilt vector updated internally in the camera class,based on the camera orientation
			float velocity = gCamera.MovementSpeed * gDeltaTime;

			gCamera.Position += gCamera.Up * velocity;			// Position + (Upvector*velocity)
		}
		// Downward
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		{
			float velocity = gCamera.MovementSpeed * gDeltaTime;

			gCamera.Position -= gCamera.Up * velocity;
		}

		// P key change the current projection type
		if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
			M_PKeyPressed = true;

		// This avoid the flip flop of the key without using a callback,but its not the correct code approach,you should update it using callbacks
		if (  (M_PKeyPressed==true) && ((glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE))  )
		{
			M_PKeyPressed = false;
			gProjectionType++;
			gProjectionType = gProjectionType&1;		// With a single bitwise AND we can avoid the if (save cpu cicles)

			cout << "Current Projection type " <<  gProjectionType << endl;
		}

		// Test light movement control
		// This controls will move a single light to help you positioning it on the scene
		// just change the array index and read the console to have the final position,so you may use that position in the array initialization

		static float TestLight_MovementSpeed = 10.0f;
		static unsigned int TestLight_ArrayIndex = 0;		// change this to test a particular light index in the array gGlobalLights[]

		if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
		{
			gGlobalLights[TestLight_ArrayIndex].Position.x -= (TestLight_MovementSpeed * gDeltaTime);
			cout << "Current Test light Position X: " <<  gGlobalLights[TestLight_ArrayIndex].Position.x << endl;
			cout << "Current Test light Position Y: " <<  gGlobalLights[TestLight_ArrayIndex].Position.y << endl;
			cout << "Current Test light Position Z: " <<  gGlobalLights[TestLight_ArrayIndex].Position.z << endl;
		}

		if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
		{
			gGlobalLights[TestLight_ArrayIndex].Position.x += (TestLight_MovementSpeed * gDeltaTime);
			cout << "Current Test light Position X: " <<  gGlobalLights[TestLight_ArrayIndex].Position.x << endl;
			cout << "Current Test light Position Y: " <<  gGlobalLights[TestLight_ArrayIndex].Position.y << endl;
			cout << "Current Test light Position Z: " <<  gGlobalLights[TestLight_ArrayIndex].Position.z << endl;
		}

		if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
		{
			gGlobalLights[TestLight_ArrayIndex].Position.y += (TestLight_MovementSpeed * gDeltaTime);
			cout << "Current Test light Position X: " <<  gGlobalLights[TestLight_ArrayIndex].Position.x << endl;
			cout << "Current Test light Position Y: " <<  gGlobalLights[TestLight_ArrayIndex].Position.y << endl;
			cout << "Current Test light Position Z: " <<  gGlobalLights[TestLight_ArrayIndex].Position.z << endl;
		}

		if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
		{
			gGlobalLights[TestLight_ArrayIndex].Position.y -= (TestLight_MovementSpeed * gDeltaTime);
			cout << "Current Test light Position X: " <<  gGlobalLights[TestLight_ArrayIndex].Position.x << endl;
			cout << "Current Test light Position Y: " <<  gGlobalLights[TestLight_ArrayIndex].Position.y << endl;
			cout << "Current Test light Position Z: " <<  gGlobalLights[TestLight_ArrayIndex].Position.z << endl;
		}

		if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
		{
			gGlobalLights[TestLight_ArrayIndex].Position.z -= (TestLight_MovementSpeed * gDeltaTime);
			cout << "Current Test light Position X: " <<  gGlobalLights[TestLight_ArrayIndex].Position.x << endl;
			cout << "Current Test light Position Y: " <<  gGlobalLights[TestLight_ArrayIndex].Position.y << endl;
			cout << "Current Test light Position Z: " <<  gGlobalLights[TestLight_ArrayIndex].Position.z << endl;
		}

		if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		{
			gGlobalLights[TestLight_ArrayIndex].Position.z += (TestLight_MovementSpeed * gDeltaTime);
			cout << "Current Test light Position X: " <<  gGlobalLights[TestLight_ArrayIndex].Position.x << endl;
			cout << "Current Test light Position Y: " <<  gGlobalLights[TestLight_ArrayIndex].Position.y << endl;
			cout << "Current Test light Position Z: " <<  gGlobalLights[TestLight_ArrayIndex].Position.z << endl;
		}

		// RENDER =============================================================================

		// Enable Z-depth(its called ZBUFFER)
		glEnable(GL_DEPTH_TEST);

		// Here we clear the backbuffer (the next frame buffer in wich we will render now) and the z-buffer
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Set the shader to be used
		// We use global shaders so we set it only once 
		glUseProgram(M_ShaderProgramId);

		// ------------------------------------------------------------------------------------------
		// SETTING UP TRANSFORMATIONS
		// OPTIMIZATION - Many things here could be done once and not here inside the loop (not everyframe)

		// VIEW MATRIX - VIEW TRANSFORMATION - From World Space to ViewSpace
		// We have the camera,so we take from the camera the ready matrix
		glm::mat4 view = gCamera.GetViewMatrix();

		// PROJECTION MATRIX 
		// We use a different projection type based on the gProjectionType switch
		glm::mat4 projection;
		if (gProjectionType==0)
		{
			// Perspective projection (field of view, aspect ratio, near plane, and far plane are the four parameters)
			// Notice that we take parameters from the camera too 
			//glm::mat4 projection = glm::perspective(45.0f, (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
			projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 1000.0f);
		}
		else
		{
			// Orthographic projection
			// 	left,right,bottom,top,znear,zfar
			// Reference to this sources for further explanation(try to play with values to understand the ortho proj):
			// https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/glOrtho.xml
			// https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glViewport.xhtml
			projection = glm::ortho((GLfloat)-50.0f, (GLfloat)50.0f, (GLfloat)-50.0f, (GLfloat)50.0f, 0.1f, 1000.0f);
		}

		// Retrieves and passes transform matrices to the Shader program
		// Those are the global matrices that don't change during the runtime (neither per object)
		GLint viewLoc = glGetUniformLocation(M_ShaderProgramId, "view");
		GLint projLoc = glGetUniformLocation(M_ShaderProgramId, "projection");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		// ------------------------------------------------------------------------------------------


		// ------------------------------------------------------------------------------------------
		// == TRANSFER LIGHTS DATA TO THE SHADER ==
		// Notice that its done once per frame

		// Here we transfer all the light structs in this module to the array of struct inside the shader
		for (int idx=0;idx<G_LIGHTSNUM;idx++)
		{
			// Transfer 1 struct
			char string00[256];

			sprintf_s(string00,256,"LightsArray[%d].Position",idx);
			GLint lightPositionLoc = glGetUniformLocation(M_ShaderProgramId,string00);
			glUniform3f(lightPositionLoc, gGlobalLights[idx].Position.x, gGlobalLights[idx].Position.y, gGlobalLights[idx].Position.z);

			sprintf_s(string00,256,"LightsArray[%d].Color",idx);
			GLint lightColorLoc = glGetUniformLocation(M_ShaderProgramId,string00);
			glUniform3f(lightColorLoc,gGlobalLights[idx].Color.r,gGlobalLights[idx].Color.g,gGlobalLights[idx].Color.b);

			sprintf_s(string00,256,"LightsArray[%d].SpecularIntensity",idx);
			GLint LightSpecularIntensityLoc = glGetUniformLocation(M_ShaderProgramId,string00);
			glUniform1f(LightSpecularIntensityLoc,gGlobalLights[idx].SpecularIntensity);

			sprintf_s(string00,256,"LightsArray[%d].SpecularHighlightSize",idx);
			GLint highlightSizeLoc = glGetUniformLocation(M_ShaderProgramId,string00);
			glUniform1f(highlightSizeLoc,gGlobalLights[idx].SpecularHighlightSize);
		}

		// We pass the view position (in world space,because lighiting occurs in world space) to the shader
		// This must be done once per frame since the camera position don't update more (for better code should be done only when the camera
		// change position)
		GLint viewPositionLoc = glGetUniformLocation(M_ShaderProgramId, "viewPosition");
		const glm::vec3 cameraPosition = gCamera.Position;
		glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);

		// ------------------------------------------------------------------------------------------
		// == RENDER OBJECTS ==
		// Here you render all the mesh objects you have 
		// Note you may easily create an array and a loop 

		// PLANE TABLE 3D OBJECT
		M_RenderMesh(&Mesh_PlaneTable,M_ShaderProgramId);

		// SPEAKERS 3D OBJECTS
		M_RenderMesh(&Mesh_SpeakerMesh1,M_ShaderProgramId);
		M_RenderMesh(&Mesh_SpeakerMesh2,M_ShaderProgramId);

		// MONITOR 3D COMPLEX OBJECT
		// Its composed by different mesh,here we render them all
		M_RenderMesh(&Mesh_Screen,M_ShaderProgramId);
		M_RenderMesh(&Mesh_ScreenArm1,M_ShaderProgramId);
		M_RenderMesh(&Mesh_ScreenArm2,M_ShaderProgramId);
		M_RenderMesh(&Mesh_ScreenBase,M_ShaderProgramId);

		// =====================================================================================

		// FLIP (glfw: swap buffers)
		glfwSwapBuffers(window);							// Flips the the back buffer with the front buffer every frame.

		// Process IO events (keys pressed/released, mouse moved etc.)
		// Processes only those events that are already in the event queue and then returns immediately. 
		// Processing events will cause the window and input callbacks associated with those events to be called.
		glfwPollEvents();
	}

	M_DestroyTexture(ScreenGlassLayer_TextureId);
	M_DestroyTexture(WhiteBluePlastic_TextureId);
	M_DestroyTexture(ScreenTexture_TextureId);
	M_DestroyTexture(Speaker_TextureId);
	M_DestroyTexture(Table_TextureId);

	UDestroyShaderProgram(M_ShaderProgramId);

	M_DestroyMesh(&Mesh_ScreenBase);
	M_DestroyMesh(&Mesh_ScreenArm2);
	M_DestroyMesh(&Mesh_ScreenArm1);
	M_DestroyMesh(&Mesh_Screen);
	M_DestroyMesh(&Mesh_SpeakerMesh2);
	M_DestroyMesh(&Mesh_SpeakerMesh1);
	M_DestroyMesh(&Mesh_PlaneTable);

	exit(EXIT_SUCCESS); // Terminates the program successfully
}


// Initialize GLFW, GLEW, and create a window
bool UInitialize(int argc, char* argv[], GLFWwindow** window)
{
	// GLFW: initialize and configure (specify desired OpenGL version)
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// GLFW: window creation
	// ---------------------
	* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
	if (*window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(*window);
	glfwSetFramebufferSizeCallback(*window, UResizeWindow);
    glfwSetCursorPosCallback(*window, UMousePositionCallback);
    glfwSetScrollCallback(*window, UMouseScrollCallback);
    glfwSetMouseButtonCallback(*window, UMouseButtonCallback);

	// Here we give initial window position
	glfwSetWindowPos(*window,40,40) ;

    // tell GLFW to capture our mouse
    glfwSetInputMode(*window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


	// GLEW: initialize
	// ----------------
	// Note: if using GLEW version 1.13 or earlier
	glewExperimental = GL_TRUE;
	GLenum GlewInitResult = glewInit();

	if (GLEW_OK != GlewInitResult)
	{
		std::cerr << glewGetErrorString(GlewInitResult) << std::endl;
		return false;
	}

	// Displays GPU OpenGL version
	cout << "INFO: OpenGL Version: " << glGetString(GL_VERSION) << endl;

	return true;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void UResizeWindow(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

/*
Note how this function compiles each shader separately, then attaches them to a single shader program, to finally link this program. 
At runtime, you might want to switch shader programs. Shader programs normally implement materials and not all objects in a scene will 
use the same material. To tell OpenGL that you want to use a specific shader program, you need to call the function `glUseProgram`.
*/
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId)
{
	// Compilation and linkage error reporting
	int success = 0;
	char infoLog[512];

	// Create a Shader program object.
	programId = glCreateProgram();

	// Create the vertex and fragment shader objects
	GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

	// Retrive the shader source
	glShaderSource(vertexShaderId, 1, &vtxShaderSource, NULL);
	glShaderSource(fragmentShaderId, 1, &fragShaderSource, NULL);

	// Compile the vertex shader, and print compilation errors (if any)
	glCompileShader(vertexShaderId); // compile the vertex shader
	// check for shader compile errors
	glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShaderId, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;

		return false;
	}

	glCompileShader(fragmentShaderId); // compile the fragment shader
	// check for shader compile errors
	glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShaderId, sizeof(infoLog), NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;

		return false;
	}

	// Attached compiled shaders to the shader program
	glAttachShader(programId, vertexShaderId);
	glAttachShader(programId, fragmentShaderId);

	glLinkProgram(programId);   // links the shader program
	// check for linking errors
	glGetProgramiv(programId, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(programId, sizeof(infoLog), NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;

		return false;
	}

	return true;
}


void UDestroyShaderProgram(GLuint programId)
{
	glDeleteProgram(programId);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos)
{
	
	// This change the "lookat" of the camera,in other words the camera will look around based on mouse movements
    if (gFirstMouse)
    {
        gLastX = (float)xpos;
        gLastY = (float)ypos;
        gFirstMouse = false;
    }

    float xoffset = (float)xpos - gLastX;
    float yoffset = gLastY - (float)ypos; // reversed since y-coordinates go from bottom to top

    gLastX = (float)xpos;
    gLastY = (float)ypos;

    gCamera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
//	// It change the camera zoom
//    gCamera.ProcessMouseScroll((float)yoffset);

	const float CAMERA_MAXSPEED = 150.0f;
	const float CAMERA_MINSPEED = 1.0f;

	// We change the camera movement speed
	gCamera.MovementSpeed += (float) yoffset;
	// Here we check the limits min and max
	if (gCamera.MovementSpeed>CAMERA_MAXSPEED)
		gCamera.MovementSpeed = CAMERA_MAXSPEED;
	else if (gCamera.MovementSpeed<CAMERA_MINSPEED)
		gCamera.MovementSpeed = CAMERA_MINSPEED;

	cout << "Camera movement speed changed to: " << gCamera.MovementSpeed << endl;
}

// glfw: handle mouse button events
// --------------------------------
void UMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    switch (button)
    {
        case GLFW_MOUSE_BUTTON_LEFT:
        {
            if (action == GLFW_PRESS)
                cout << "Left mouse button pressed" << endl;
            else
                cout << "Left mouse button released" << endl;
        }
        break;

        case GLFW_MOUSE_BUTTON_MIDDLE:
        {
            if (action == GLFW_PRESS)
                cout << "Middle mouse button pressed" << endl;
            else
                cout << "Middle mouse button released" << endl;
        }
        break;

        case GLFW_MOUSE_BUTTON_RIGHT:
        {
            if (action == GLFW_PRESS)
                cout << "Right mouse button pressed" << endl;
            else
                cout << "Right mouse button released" << endl;
        }
        break;

        default:
            cout << "Unhandled mouse button event" << endl;
            break;
    }
}



