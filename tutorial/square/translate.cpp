//
// Display a colored square
//
// Colors are assigned to each vertex and then the rasterizer interpolates
//   those colors across the triangles.  We us an orthographic projection
//   as the default projection.

#include "../include/Angel.h"


// Translation along each coordinate axis
GLfloat  Translation[4] = { 0.0, 0.0, 0.0, 0.0};
GLfloat  step = 0.1;

GLuint  locT;


//----------------------------------------------------------------------------

// OpenGL initialization
void
init()
{
	// Vertices of a unit cube centered at origin, sides aligned with axes
	vec4 points[6] = {
		vec4( -0.5, -0.5,  0, 1.0 ), //v1
		vec4(  0.5, -0.5,  0, 1.0 ), //v2
		vec4( -0.5,  0.5,  0, 1.0 ), //v3

		vec4( -0.5,  0.5,  0, 1.0 ), //v3
		vec4(  0.5, -0.5,  0, 1.0 ), //v2
		vec4(  0.5,  0.5,  0, 1.0 )  //v4
	};

	// RGBA colors
	vec4 colors[6] = {
		vec4( 1.0, 0.0, 0.0, 1.0 ),  // red
		vec4( 1.0, 0.0, 0.0, 1.0 ),  // red
		vec4( 1.0, 0.0, 0.0, 1.0 ),  // red

		vec4( 0.0, 1.0, 0.0, 1.0 ),  // green
		vec4( 0.0, 1.0, 0.0, 1.0 ),  // green
		vec4( 0.0, 1.0, 0.0, 1.0 )  // green
	};

	// Create a vertex array object
	GLuint vao;
	glGenVertexArrays( 1, &vao );
	glBindVertexArray( vao );

        // Create and initialize a buffer object
	GLuint buffer;
	glGenBuffers( 1, &buffer );
        glBindBuffer( GL_ARRAY_BUFFER, buffer );
        glBufferData( GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), NULL, GL_STATIC_DRAW );
	glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(points), points );
	glBufferSubData( GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors );

        // Load shaders and use the resulting shader program
	GLuint program = InitShader( "vshader_translate.glsl", "fshader_translate.glsl" );
	glUseProgram( program );

        // set up vertex arrays
	GLuint vPosition = glGetAttribLocation( program, "vPosition" );
	glEnableVertexAttribArray( vPosition );
	glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(0) );

	GLuint vColor = glGetAttribLocation( program, "vColor" );
	glEnableVertexAttribArray( vColor );
	glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(sizeof(points)) );

	// The location of the  shader uniform variable "deltaT"
        locT = glGetUniformLocation( program, "translation" );

	glClearColor( 1.0, 1.0, 1.0, 1.0 );
}

//----------------------------------------------------------------------------

void
display( void )
{
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glShadeModel(GL_FLAT);

	glUniform4fv( locT,1, Translation );

	glDrawArrays( GL_TRIANGLES, 0, 6 );

	glutPostRedisplay();
	glutSwapBuffers();
}

//----------------------------------------------------------------------------

void
keyboard( unsigned char key, int x, int y )
{
    switch( key ) {
	case 033: // Escape Key
	case 'q': case 'Q':
	    exit( EXIT_SUCCESS );
	    break;
    }
}


//----------------------------------------------------------------------------

void processSpecialKeys(int key, int x, int y)
{
	switch(key) {
	case GLUT_KEY_UP :
		Translation[1] += step;
		break;
	case GLUT_KEY_DOWN :
		Translation[1] -= step;
		break;
	case GLUT_KEY_LEFT :
		Translation[0] -= step;
		break;
	case GLUT_KEY_RIGHT:
		Translation[0] += step;
		break;
	}

	if (Translation[0] >  1) Translation[0] -= 2;
	if (Translation[0] < -1) Translation[0] += 2;
	if (Translation[1] >  1) Translation[1] -= 2;
	if (Translation[1] < -1) Translation[1] += 2;

	glutPostRedisplay();
}

//----------------------------------------------------------------------------


int
main( int argc, char **argv )
{
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE );
	glutInitWindowSize( 512, 512 );
	glutCreateWindow( "Move Color Square" );

	glewInit();
	init();

	glutDisplayFunc( display );
	glutKeyboardFunc( keyboard );
	glutSpecialFunc( processSpecialKeys );

	glutMainLoop();
	return 0;
}
