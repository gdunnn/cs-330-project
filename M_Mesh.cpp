
/*
[!] Please note that this module can easily become a Class instead of M_MESH struct. Should be done asap for a better code! 
*/

#include <iostream>         // cout, cerr
#include <cstdlib>          // EXIT_FAILURE
#include <GL/glew.h>        // GLEW library
#include <GLFW/glfw3.h>     // GLFW library

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include "M_Mesh.h"


#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>      // Image loading Utility functions


/*
Initialize a M_MESH with the arguments passed.
The aMesh passed must be already allocated.
The Mesh must be destroyed after use with M_DestroyMesh().
The WorldPosition member will be set to 0,0,0 (default start).
The matrices will be initialized with default(zero) values.
Note that this initialize the mesh with NO texture layers,so you can't render the mesh like this,you need to add at least 1 texture layer with the 
apposite function M_MeshAddTextureLayer().
*/
void M_InitMesh(M_MESH* aMesh,GLfloat* aVertices,int aNumVertex)
{
	const GLuint floatsPerVertex = 3;
    const GLuint floatsPerNormal = 3;
	const GLuint floatsPerUV = 2;
	int c0;	

	// Default RenderMode is SOLID
	aMesh->RenderMode = 0;

	aMesh->nVertices = aNumVertex;
	//aMesh->nIndices = aNumIndices;

	// Reset texture layers (this is the default start state,no layers active)
	aMesh->BindedTexturesNum = 0;			// This is the number of active layers
	for (c0=0;c0<M_MESH_MAXTEXTURELAYERS;c0++)
	{
		aMesh->TexturesID[c0] = 0;
	}

	// Set the WorldPosition to default (0)
	aMesh->WorldPosition[0] = 0.0f;
	aMesh->WorldPosition[1] = 0.0f;
	aMesh->WorldPosition[2] = 0.0f;

	// Set matrices to default values (all zero)
	aMesh->scale = glm::scale(glm::vec3(1.0f,1.0f,1.0f));
	aMesh->rotation = glm::rotate(0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
	aMesh->translation = glm::translate(glm::vec3(0.0f, 0.0f, 0.0f));		// This will be the WorldPosition member 

	// Create a VAO (Vertex Array) that will contain the vertex buffer for this mesh
	glGenVertexArrays(1, &(aMesh->vao));												// we can also generate multiple VAOs or buffers at the same time
	glBindVertexArray(aMesh->vao);

	// Create VBO (only 1 for vertexes)
	glGenBuffers(1,aMesh->vbos);
	glBindBuffer(GL_ARRAY_BUFFER,aMesh->vbos[0]);		// Activates the buffer
	glBufferData(GL_ARRAY_BUFFER, (sizeof(GLfloat) * (floatsPerVertex + floatsPerUV + floatsPerNormal)) * aNumVertex, aVertices, GL_STATIC_DRAW);

	// Create the Vertex Attribute Pointer.
	// We need to tell OpenGL how to read the VBO since there could be more than one attribute for each vertex

	// Strides between vertex coordinates 
	GLint stride = sizeof(float) * (floatsPerVertex + floatsPerUV + floatsPerNormal);// The number of floats before each
	
	// Vertex Data Attribute 0 = vertex position
	glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
	glEnableVertexAttribArray(0);

	// Vertex Data Attribute 1 - Vertex Normals
    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

	// Vertex Data Attribute 2 = vertex UV coords
	glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
	glEnableVertexAttribArray(2);
}


void M_DestroyMesh(M_MESH* aMesh)
{
	glDeleteVertexArrays(1, &(aMesh->vao));
	glDeleteBuffers(1, aMesh->vbos);
}

/*
Add the texture aTextureID to the first unused layer (ie. if no layers active the 1st slot free will be the 0) of aMesh.
You pass the textureid previously created with M_LoadTexture(), this will be an added layer of textures that will be rendered. Note that
every active layer will be automatically rendered in the render function. 
The layer wont be added if the max layers number is reached.
Note: after creating a new mesh with M_InitMesh() you must add at least 1 layer to the mesh calling this function.
*/
void M_MeshAddTextureLayer(M_MESH* aMesh,GLuint aTextureID)
{
	if (aMesh->BindedTexturesNum>=M_MESH_MAXTEXTURELAYERS)
		return;

	// We save the texturesid in the mesh layers (in the struct)
	aMesh->TexturesID[aMesh->BindedTexturesNum] = aTextureID;
	aMesh->BindedTexturesNum++;
}


// This set the current Mesh's Scale,you may call this function anytime you want,this set the internal object's scale matrix. 
// This will have effect the next time M_RenderMesh() its called.
void M_SetMeshScale(M_MESH* aMesh,GLfloat aX,GLfloat aY,GLfloat aZ)
{
	aMesh->scale = glm::scale(glm::vec3(aX,aY,aZ));
}

// This set the current Mesh's Rotation,you may call this function anytime you want,this set the internal object's rotation matrix. 
// This will have effect the next time M_RenderMesh() its called.
// Rotated by aAngleDegree on the axis chosen,i.e.: 45.0f,1.0f,0.0f,0.0f = Rotate around X axis by 45 degree
void M_SetMeshRotation(M_MESH* aMesh,GLfloat aAngleDegree,GLfloat aX, GLfloat aY, GLfloat aZ)
{
	aMesh->rotation = glm::rotate(aAngleDegree, glm::vec3(aX,aY,aZ));
}


// This set the current Mesh's WorldPosition,you may call this function anytime you want,this set the internal object's world position. 
// This will have effect the next time M_RenderMesh() its called.
void M_SetMeshWorldPosition(M_MESH* aMesh, GLfloat aX, GLfloat aY, GLfloat aZ)
{
	aMesh->WorldPosition[0] = aX;
	aMesh->WorldPosition[1] = aY;
	aMesh->WorldPosition[2] = aZ;
}

/*
Render the aMesh. Must be called once per frame(inside render section).
This also generate the Model matrix based on the currently set: WorldPosition,Scale and Rotation of the object. In other words this also TRANSFORM 
the Mesh from local to world space based on its current transform values.
Note that it render the mesh based on the member RenderMode,valid only for this mesh!!
The mesh will be rendered with currently active texture layers,to add layers use the apposite function.
Notice that the function supports M_MESH_MAXTEXTURELAYERS max texture layers.
*/
void M_RenderMesh(M_MESH* aMesh,GLuint aShaderProgramId)
{
	// Security check,can't render the mesh if there are no texture layers active
	if (aMesh->BindedTexturesNum==0)
		return;

	// We set the render mode as the member RenderMode,this will be valid only for this rendering
	if (aMesh->RenderMode==0)
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	else if (aMesh->RenderMode==1)
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

	// The VAO to be rendered (inside there is the mesh VertexBufferObject)
	// Activating a VAO activate all the VBO inside of it!
	glBindVertexArray(aMesh->vao);

	// Here we pass info to the fragment shader for rendering the texture layers

	// Num of active textures (the texture layers the shader will draw)
	glUniform1i( glGetUniformLocation(aShaderProgramId, "uNumActiveTextures"),aMesh->BindedTexturesNum );

	// Here we setup and activate every single texture layer (only the active ones)
	// Texture0 (always present)
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,aMesh->TexturesID[0]);
	glUniform1i(glGetUniformLocation(aShaderProgramId, "uTexture0"),0);

	// Texture1
	if (aMesh->BindedTexturesNum>1)
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D,aMesh->TexturesID[1]);
		glUniform1i(glGetUniformLocation(aShaderProgramId, "uTexture1"),1);
	}

	// Texture2
	if (aMesh->BindedTexturesNum>2)
	{
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D,aMesh->TexturesID[2]);
		glUniform1i(glGetUniformLocation(aShaderProgramId, "uTexture2"),2);
	}

	// Texture3
	if (aMesh->BindedTexturesNum>3)
	{
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D,aMesh->TexturesID[3]);
		glUniform1i(glGetUniformLocation(aShaderProgramId, "uTexture3"),3);
	}

	// Apply actual WorldPosition to the translation matrix
	aMesh->translation = glm::translate(glm::vec3(aMesh->WorldPosition[0], aMesh->WorldPosition[1], aMesh->WorldPosition[2]));

	// Model matrix: transformations are applied right-to-left order
	// This is the FINAL TRANSFORM MATRIX for this Mesh,wich concatenate TRANSLATION,ROTATION,SCALE 
	// This matrix will transform the object from LOCAL SPACE to WORLD SPACE
	glm::mat4 model = aMesh->translation * aMesh->rotation * aMesh->scale;

	GLint modelLoc = glGetUniformLocation(aShaderProgramId, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    // Draws the triangles
//	glDrawElements(GL_TRIANGLES,aMesh->nIndices,GL_UNSIGNED_SHORT,NULL);
    glDrawArrays(GL_TRIANGLES,0,aMesh->nVertices);

	// Deactivate the VAO
	glBindVertexArray(0);

	// Restore the PolygonMode to fill(default state) 
	// --- ITS MORE CORRECT TO SAVE IT WITH glGet() and RESTORE HERE
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
}


/*
Load the image file aFilePathName(path+filename), and if its the correct format create a new texture with a new textureid.
This function only handles images with three and four channels (RGB and RGBA). 
Note that texturesid are not automatically destroyed when the mesh is destroyed,you must do it manually calling M_DestroyTexture().
> NewTextureID: Will return here the new textureid created,if success. You have to destroy the returned textureid with M_DestroyTexture()
(true) Success,false if error.
*/
bool M_LoadTexture(const char* aFilePathName,GLuint& NewTextureID)
{
    int width, height, channels;

    unsigned char *image = stbi_load(aFilePathName, &width, &height, &channels, 0);

    if (image)
    {
        M_flipImageVertically(image, width, height, channels);

        glGenTextures(1,&NewTextureID);						// Here the textureId will be filled with the new id

		// Setup this texture 
        glBindTexture(GL_TEXTURE_2D,NewTextureID);

        // Set the texture wrapping parameters.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // Set texture filtering parameters.
		// Filtering parameters are configured to perform linear interpolation. When we read the texture we will not have integer texture coordinates, 
		// so what texel do we access for texture coordinates `(5.2, 12.8)`? GL_LINEAR tells OpenGL to do a bilinear interpolation with the four 
		// closest neighboring texels: `(5, 12)`, `(5, 13)`, `(6, 12)`, and `(6, 13)`.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Here we transfers the image to the GPU
        if (channels == 3)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        else if (channels == 4)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        else
        {
            std::cout << "Not implemented to handle image with " << channels << " channels" << std::endl;
            return false;
        }

		// Generates mipmap images
		// Mipmaps are "intermediate levels" between one resolution and another, which will be used by the GPU based on the distance of view
        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(image);
        glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture.

        return true;
    }

    // Error loading the image
    return false;
}


void M_DestroyTexture(GLuint aTextureID)
{
    glDeleteTextures(1,&aTextureID);
}

// Images are loaded with Y axis going down, but OpenGL's Y axis goes up, so let's flip it
void M_flipImageVertically(unsigned char *image, int width, int height, int channels)
{
    for (int j = 0; j < height / 2; ++j)
    {
        int index1 = j * width * channels;
        int index2 = (height - 1 - j) * width * channels;

        for (int i = width * channels; i > 0; --i)
        {
            unsigned char tmp = image[index1];
            image[index1] = image[index2];
            image[index2] = tmp;
            ++index1;
            ++index2;
        }
    }
}
