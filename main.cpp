
#include <string>
#include <stdlib.h> 

#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include "Shader.h"
#include "Camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "SOIL2/SOIL2.h"

#define vis 0.005f

// Properties
const GLuint WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Function prototypes
void KeyCallback( GLFWwindow *window, int key, int scancode, int action, int mode );
void ScrollCallback( GLFWwindow *window, double xOffset, double yOffset );
void MouseCallback( GLFWwindow *window, double xPos, double yPos );
void DoMovement( );

// Camera
Camera  camera(glm::vec3( 0.0f, 0.0f, 3.0f ) );
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool keys[1024];
bool firstMouse = true;
static bool turn = true;
static bool pause = false;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

const GLuint N = 125;
const GLuint sq = 1;




GLfloat vertices1[N*N*6]; 
GLuint waterIndices[N*N*6];
glm::vec3 normal( 0.0f, 0.0f, 0.0f );
int p = 0;

struct f{
	float U[N*N][N*N];
};

f A,B;
f *l=&A,*n=&B;


int  makePlane(GLfloat n, GLfloat len){
    int k = 0;
    GLfloat step = (len *1.0f) / (n - 1.0f);
    for (int i = 0; i < (int)n; i++){
        for (int j = 0; j < (int)n; j++){
            vertices1[k] = i*step*1.0f;
            vertices1[k+1] = 0.0f;
            vertices1[k+2] = j*step*1.0f;
            vertices1[k+3] = 0.0f;
            vertices1[k+4] = 0.0f;
            vertices1[k+5] = 0.0f;
            k+=6;
            if (i<(n-1)){
                if (j<(n-1))
                {
                    waterIndices[p] = i*n + j;
                    waterIndices[p+1] = i*n + j + 1;
                    waterIndices[p+2] = (i + 1)*n + j;
                    waterIndices[p+3] = i* n + j + 1;
                    waterIndices[p+4] = (i + 1)*n + j;
                    waterIndices[p+5] = (i + 1)*n + j + 1;
                    p+=6;
                }
            }
        }
    
    }
    return p-6;
}

void time_step()
{
    int k = 0;
	int i,j,i1,j1;

	i1=rand()%110;
	j1=rand()%110;
	
    if((rand()&127)==0)
	for(i=-3;i<4;i++){
		for(j=-3;j<4;j++){
			float v=6.0f-i*i-j*j;
			if(v<0.0f) v=0.0f;
			n->U[i+i1+3][j+j1+3]-=v*0.004f;
		}
	}
	for(i=1;i<N-1;i++){
		for(j=1;j<N-1;j++){
            vertices1[k+1]=n->U[i][j];
			vertices1[k+4]=n->U[i-1][j]-n->U[i+1][j];
			vertices1[k+5]=n->U[i][j-1]-n->U[i][j+1];

			float laplas=(n->U[i-1][j]+
				          n->U[i+1][j]+
						  n->U[i][j+1]+
						  n->U[i][j-1])*0.25f-n->U[i][j];

			l->U[i][j]=((2.0f-vis)*n->U[i][j]-l->U[i][j]*(1.0f-vis)+laplas);
            k+=6;
		}
	}
 }
 
int main( )
{
    // Init GLFW
    glfwInit( );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
    glfwWindowHint( GLFW_RESIZABLE, GL_FALSE );
    glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
    
    GLFWwindow* window = glfwCreateWindow( WIDTH, HEIGHT, "А вот и задание #2! ", 0, 0 );
    
    glfwMakeContextCurrent( window );
    
    glfwGetFramebufferSize( window, &SCREEN_WIDTH, &SCREEN_HEIGHT );
    
    // Set the required callback functions
    glfwSetKeyCallback( window, KeyCallback );
    glfwSetCursorPosCallback( window, MouseCallback );
    glfwSetScrollCallback( window, ScrollCallback );
    
    // Options, removes the mouse cursor for a more immersive experience
    glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_DISABLED );
    
    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
    glewExperimental = GL_TRUE;
    // Initialize GLEW to setup the OpenGL Function pointers
    if ( GLEW_OK != glewInit( ) )
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return EXIT_FAILURE;
    }
    
    // Define the viewport dimensions
    glViewport( 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT );
    
    // Setup some OpenGL options
    glEnable( GL_DEPTH_TEST );
    
    // enable alpha support
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    
    // Setup and compile our shaders
    Shader ourShader( "res/shaders/core.vs", "res/shaders/core.frag" );
    Shader ourShaderObjInside( "res/shaders/core.vs", "res/shaders/core.frag" );
    Shader ourShaderWater( "res/shaders/core1.vs", "res/shaders/core1.frag" );

   int counter =  makePlane(N, sq);
    
    // Set up our vertex data (and buffer(s)) and attribute pointers
    GLfloat vertices[] =
    {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
       
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
       
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    };
     GLfloat verticesObjectInside[] =
    {
        -0.2f, -0.2f, -0.2f,  0.0f, 0.0f,
        0.2f, -0.2f, -0.2f,  1.0f, 0.0f,
        0.2f,  0.2f, -0.2f,  1.0f, 1.0f,
        0.2f,  0.2f, -0.2f,  1.0f, 1.0f,
        -0.2f,  0.2f, -0.2f,  0.0f, 1.0f,
        -0.2f, -0.2f, -0.2f,  0.0f, 0.0f,
        
        -0.2f, -0.2f,  0.2f,  0.0f, 0.0f,
        0.2f, -0.2f,  0.2f,  1.0f, 0.0f,
        0.2f,  0.2f,  0.2f,  1.0f, 1.0f,
        0.2f,  0.2f,  0.2f,  1.0f, 1.0f,
        -0.2f,  0.2f,  0.2f,  0.0f, 1.0f,
        -0.2f, -0.2f,  0.2f,  0.0f, 0.0f,
       
        -0.2f,  0.2f,  0.2f,  1.0f, 0.0f,
        -0.2f,  0.2f, -0.2f,  1.0f, 1.0f,
        -0.2f, -0.2f, -0.2f,  0.0f, 1.0f,
        -0.2f, -0.2f, -0.2f,  0.0f, 1.0f,
        -0.2f, -0.2f,  0.2f,  0.0f, 0.0f,
        -0.2f,  0.2f,  0.2f,  1.0f, 0.0f,
       
        0.2f,  0.2f,  0.2f,  1.0f, 0.0f,
        0.2f,  0.2f, -0.2f,  1.0f, 1.0f,
        0.2f, -0.2f, -0.2f,  0.0f, 1.0f,
        0.2f, -0.2f, -0.2f,  0.0f, 1.0f,
        0.2f, -0.2f,  0.2f,  0.0f, 0.0f,
        0.2f,  0.2f,  0.2f,  1.0f, 0.0f,
        
        -0.2f, -0.2f, -0.2f,  0.0f, 1.0f,
        0.2f, -0.2f, -0.2f,  1.0f, 1.0f,
        0.2f, -0.2f,  0.2f,  1.0f, 0.0f,
        0.2f, -0.2f,  0.2f,  1.0f, 0.0f,
        -0.2f, -0.2f,  0.2f,  0.0f, 0.0f,
        -0.2f, -0.2f, -0.2f,  0.0f, 1.0f,
       
        -0.2f,  0.2f, -0.2f,  0.0f, 1.0f,
        0.2f,  0.2f, -0.2f,  1.0f, 1.0f,
        0.2f,  0.2f,  0.2f,  1.0f, 0.0f,
        0.2f,  0.2f,  0.2f,  1.0f, 0.0f,
        -0.2f,  0.2f,  0.2f,  0.0f, 0.0f,
        -0.2f,  0.2f, -0.2f,  0.0f, 1.0f,
    };


    GLuint VBO, VAO;
    glGenVertexArrays( 1, &VAO );
    glGenBuffers( 1, &VBO );
    // Bind our Vertex Array Object first, then bind and set our buffers and pointers.
    glBindVertexArray( VAO );
    
    glBindBuffer( GL_ARRAY_BUFFER, VBO );
    glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_STATIC_DRAW );
    
    // Position attribute
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof( GLfloat ), ( GLvoid * )0 );
    glEnableVertexAttribArray( 0 );
    // TexCoord attribute
    glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof( GLfloat ), ( GLvoid * )( 3 * sizeof( GLfloat ) ) );
    glEnableVertexAttribArray( 2 );
    
    glBindVertexArray( 0 ); // Unbind VAO

   
    // Load and create a texture
    GLuint texture;
    // --== TEXTURE == --
    glGenTextures( 1, &texture );
    glBindTexture( GL_TEXTURE_2D, texture ); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object
    // Set our texture parameters
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    // Set texture filtering
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    // Load, create texture and generate mipmaps
    int width, height;
    unsigned char *image = SOIL_load_image( "res/images/111.jpg", &width, &height, 0, SOIL_LOAD_RGB );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image );
    glGenerateMipmap( GL_TEXTURE_2D );
    SOIL_free_image_data( image );
    glBindTexture( GL_TEXTURE_2D, 0 ); // Unbind texture when done, so we won't accidentily mess up our texture.
    
    GLuint VBOobjInside, VAOobjInside;
    glGenVertexArrays( 1, &VAOobjInside );
    glGenBuffers( 1, &VBOobjInside );
    // Bind our Vertex Array Object first, then bind and set our buffers and pointers.
    glBindVertexArray( VAOobjInside );
    
    glBindBuffer( GL_ARRAY_BUFFER, VBOobjInside );
    glBufferData( GL_ARRAY_BUFFER, sizeof( verticesObjectInside ), verticesObjectInside, GL_STATIC_DRAW );
    
    // Position attribute
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof( GLfloat ), ( GLvoid * )0 );
    glEnableVertexAttribArray( 0 );
    // TexCoord attribute
    glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof( GLfloat ), ( GLvoid * )( 3 * sizeof( GLfloat ) ) );
    glEnableVertexAttribArray( 2 );
    
    glBindVertexArray( 0 ); // Unbind VAO

    // Load and create a texture
    GLuint textureobjInside;
    // --== TEXTURE == --
    glGenTextures( 1, &textureobjInside );
    glBindTexture( GL_TEXTURE_2D, textureobjInside ); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object
    // Set our texture parameters
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    // Set texture filtering
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    // Load, create texture and generate mipmaps
    int Owidth, Oheight;
    unsigned char *imageobjInside = SOIL_load_image( "res/images/lol.jpg", &Owidth, &Oheight, 0, SOIL_LOAD_RGB );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, Owidth, Oheight, 0, GL_RGB, GL_UNSIGNED_BYTE, imageobjInside );
    glGenerateMipmap( GL_TEXTURE_2D );
    SOIL_free_image_data( imageobjInside );
    glBindTexture( GL_TEXTURE_2D, 0 ); // Unbind texture when done, so we won't accidentily mess up our texture.
    

      // Get the uniform locations
    GLint modelLocCube = glGetUniformLocation( ourShader.Program, "model" );
    GLint viewLocCube = glGetUniformLocation( ourShader.Program, "view" );
    GLint projLocCube = glGetUniformLocation( ourShader.Program, "projection" ); 

    GLint modelLocObjInside = glGetUniformLocation( ourShaderObjInside.Program, "model" );
    GLint viewLocObjInside = glGetUniformLocation( ourShaderObjInside.Program, "view" );
    GLint projLocObjInside = glGetUniformLocation( ourShaderObjInside.Program, "projection" ); 

         // Get the uniform locations
    GLint modelLoc = glGetUniformLocation( ourShaderWater.Program, "model" );
    GLint viewLoc = glGetUniformLocation( ourShaderWater.Program, "view" );
    GLint projLoc = glGetUniformLocation( ourShaderWater.Program, "projection" ); 
    GLint lightLoc = glGetUniformLocation( ourShaderWater.Program, "light" );

    while( !glfwWindowShouldClose( window ) ){
         if (!pause ){
             time_step();
         }
             
        GLuint VAO1, VBO1, EBO;
        glGenVertexArrays( 1, &VAO1 );
        glGenBuffers( 1, &VBO1 );
        // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
        glBindVertexArray( VAO1 );
    
        glBindBuffer( GL_ARRAY_BUFFER, VBO1 );
        glBufferData( GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW );

        glGenBuffers( 1, &EBO );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, EBO );
        glBufferData( GL_ELEMENT_ARRAY_BUFFER, counter, waterIndices, GL_STATIC_DRAW );
    
        // Position attribute
        glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof( GLfloat ), ( GLvoid * ) 0 );
        glEnableVertexAttribArray( 0 );
        // Normal attribute
    
        glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof( GLfloat ), ( GLvoid * )( 3 * sizeof( GLfloat ) ) );
        glEnableVertexAttribArray( 1 );
    
        glBindVertexArray( 0 );
        // Set frame time
        GLfloat currentFrame = glfwGetTime( );
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        // Check and call events
        glfwPollEvents( );
        DoMovement( );
        
        glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        glm::mat4 projection;
        projection = glm::perspective(camera.GetZoom( ), (GLfloat)SCREEN_WIDTH/(GLfloat)SCREEN_HEIGHT, 0.1f, 1000.0f);
        // Create camera transformation
        glm::mat4 view;
        view = camera.GetViewMatrix( );
       
        ourShader.Use( );
        // Bind Textures using texture units
        glActiveTexture( GL_TEXTURE0 );
        glBindTexture( GL_TEXTURE_2D, texture );
        glUniform1i( glGetUniformLocation( ourShader.Program, "ourTexture1" ), 0 );
         // Pass the matrices to the shader
        glUniformMatrix4fv( viewLocCube, 1, GL_FALSE, glm::value_ptr( view ) );
        glUniformMatrix4fv( projLocCube, 1, GL_FALSE, glm::value_ptr( projection ) );

        glBindVertexArray( VAO );
        glm::mat4 modelCube;
        modelCube = glm::translate( modelCube, glm::vec3(0.0f, 0.0f, 0.0f) );
        modelCube = glm::rotate(modelCube, 90.0f, glm::vec3(  1.0f, 1.0f, 1.0f ) );
        glUniformMatrix4fv( modelLocCube, 1, GL_FALSE, glm::value_ptr( modelCube ) );   
        glDrawArrays( GL_TRIANGLES, 0, 36 );
        glBindVertexArray( 0 );

        ourShaderObjInside.Use( );
        // Bind Textures using texture units
        glActiveTexture( GL_TEXTURE0 );
        glBindTexture( GL_TEXTURE_2D, textureobjInside );
        glUniform1i( glGetUniformLocation( ourShaderObjInside.Program, "ourTexture1" ), 0 );
         // Pass the matrices to the shader
        glUniformMatrix4fv( viewLocObjInside, 1, GL_FALSE, glm::value_ptr( view ) );
        glUniformMatrix4fv( projLocObjInside, 1, GL_FALSE, glm::value_ptr( projection ) );

        glBindVertexArray( VAOobjInside );
        glm::mat4 modelObjInside;
        modelObjInside = glm::translate( modelObjInside, glm::vec3(0.0f, 0.0f, -0.1f) );
        modelObjInside = glm::rotate(modelObjInside, 90.0f, glm::vec3(  1.0f, 1.0f, 1.0f ) );
        glUniformMatrix4fv( modelLocObjInside, 1, GL_FALSE, glm::value_ptr( modelObjInside ) ); 
        glDrawArrays( GL_TRIANGLES, 0, 36 );
        glBindVertexArray( 0 );
        
        ourShaderWater.Use();    
        // Pass the matrices to the shader
        glUniformMatrix4fv( viewLoc, 1, GL_FALSE, glm::value_ptr( view ) );
        glUniformMatrix4fv( projLoc, 1, GL_FALSE, glm::value_ptr( projection ) );
       // glUniform3f( normalLoc, normal.x, normal.y, normal.z );

        glBindVertexArray( VAO1 );
	    glm::mat4 model;
        model = glm::translate( model, glm::vec3(-0.5266f, -0.47f, 0.3f) );
        model = glm::rotate(model, 90.0f, glm::vec3( 1.0f, 1.0f, 1.0f ) );
        glUniformMatrix4fv( modelLoc, 1, GL_FALSE, glm::value_ptr( model ) );  
        glUniformMatrix4fv( lightLoc, 1, GL_FALSE, glm::value_ptr( model ) ); 

        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, counter*sizeof(GLuint),  waterIndices, GL_STATIC_DRAW);

        if(!turn) glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

        glDrawElements(GL_TRIANGLES, counter*sizeof(GLuint), GL_UNSIGNED_INT, 0);
        glDisable(GL_BLEND);
        glBindVertexArray( 0 );

        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

        f *sw=l;l=n;n=sw;
       
        glDeleteVertexArrays( 1, &VAO1 );
        glDeleteBuffers( 1, &EBO );
        glfwSwapBuffers( window );
    }
    
    // Properly de-allocate all resources once they've outlived their purpose
    glDeleteVertexArrays( 1, &VAO );
    glDeleteBuffers( 1, &VBO );
    glDeleteVertexArrays( 1, &VAOobjInside );
    glDeleteBuffers( 1, &VBOobjInside );
    glfwTerminate( );
    return 0;
}

// Moves/alters the camera positions based on user input
void DoMovement( ){
    // Camera controls
    if( keys[GLFW_KEY_W]) camera.ProcessKeyboard( FORWARD, deltaTime );
    
    if( keys[GLFW_KEY_S]) camera.ProcessKeyboard( BACKWARD, deltaTime );
    
    if( keys[GLFW_KEY_A]) camera.ProcessKeyboard( LEFT, deltaTime );
    
    if( keys[GLFW_KEY_D]) camera.ProcessKeyboard( RIGHT, deltaTime );

}

// Is called whenever a key is pressed/released via GLFW
void KeyCallback( GLFWwindow *window, int key, int scancode, int action, int mode ){
    if( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS ) glfwSetWindowShouldClose(window, GL_TRUE);
  
    if( key == GLFW_KEY_F5 && action == GLFW_PRESS ) pause = !pause;

     if( key == GLFW_KEY_Q && action == GLFW_PRESS )  turn = !turn;
    
    if ( key >= 0 && key < 1024 ){
        if( action == GLFW_PRESS )  keys[key] = true;
        else if( action == GLFW_RELEASE ) keys[key] = false;
    }
}

void MouseCallback( GLFWwindow *window, double xPos, double yPos ){
    if( firstMouse ){
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }
    
    GLfloat xOffset = xPos - lastX;
    GLfloat yOffset = lastY - yPos;  // Reversed since y-coordinates go from bottom to left
    
    lastX = xPos;
    lastY = yPos;
    
    camera.ProcessMouseMovement( xOffset, yOffset );
}	


void ScrollCallback( GLFWwindow *window, double xOffset, double yOffset ){
    camera.ProcessMouseScroll( yOffset );
}



