//
// Display a color square
//
// Colors are assigned to each vertex and then the rasterizer interpolates
//   those colors across the triangles.  We us an orthographic projection
//   as the default projection.

#include "../include/Angel.h"


// The rotation  around z axis
GLfloat  Theta = 0.0; // Angle (in degrees)
GLfloat  step = 0.005; // Incremental
GLuint  locTheta;  
enum { CW = 0, CCW = 1};
int direction = CW;  // Direction

//Scale along x and y axes
GLfloat ScaleFactor[2] = {1.0, 1.0};
GLuint locScale;


//----------------------------------------------------------------------------

// OpenGL initialization
void
init()
{
	// Vertices of a unit square centered at origin, sides aligned with axes
	vec4 points[4] = {
		vec4( -0.5, -0.5,  0, 1.0 ), //v1
		vec4(  0.5, -0.5,  0, 1.0 ), //v2
		vec4( -0.5,  0.5,  0, 1.0 ), //v3
		vec4(  0.5,  0.5,  0, 1.0 )  //v4
	};

	// RGBA colors
	vec4 colors[4] = {
		vec4( 1.0, 0.0, 0.0, 1.0 ),  // red
		vec4( 0.0, 1.0, 0.0, 1.0 ),  // green
		vec4( 0.0, 1.0, 0.0, 1.0 ),  // green
		vec4( 0.0, 0.0, 1.0, 1.0 ),  // blue
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
	GLuint program = InitShader( "vshader_rot.glsl", "fshader_rot.glsl" );
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

	// The location of shader uniform variables
	locTheta = glGetUniformLocation( program, "theta" );
	locScale = glGetUniformLocation( program, "scale" );

	glClearColor( 1.0, 1.0, 1.0, 1.0 );
}

//----------------------------------------------------------------------------

void
display( void )
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glShadeModel(GL_FLAT);

	glUniform1f( locTheta, Theta );
	glUniform2fv( locScale, 1, ScaleFactor );

	glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

	glutSwapBuffers();
}

//----------------------------------------------------------------------------
void
reshape( GLsizei w, GLsizei h )
{
	glViewport(0, 0, w, h);

	// Scale the square to avoid stretching
	if (w > h) ScaleFactor[0] = (float)h/w;
	if (w < h) ScaleFactor[1] = (float)w/h;
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
    case 'a':
        direction = CCW;
        break;
    case 'd':
        direction = CW;
        break;
    }
}

//----------------------------------------------------------------------------

void
mouse( int button, int state, int x, int y )
{
    if ( state == GLUT_DOWN ) {
		switch( button ) 
		{
		case GLUT_LEFT_BUTTON:    
			direction = CCW;  
			break;
		case GLUT_RIGHT_BUTTON:   
			direction = CW;  
			break;
		}
    }
}

//----------------------------------------------------------------------------

void
idle( void )
{
	// Animate the rotation
	if (direction == CW)	
		Theta += step;
	else
		Theta -= step;

	if ( Theta > 360.0 ) {
		Theta -= 360.0;
	}
    
	glutPostRedisplay();
}

//----------------------------------------------------------------------------

int
main( int argc, char **argv )
{
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
	glutInitWindowSize( 512, 512 );
	glutCreateWindow( "Rotating Color Square" );

	glewInit();
	init();

	glutDisplayFunc( display );
	glutReshapeFunc( reshape );
	glutKeyboardFunc( keyboard );
	glutMouseFunc( mouse );
	glutIdleFunc( idle );

	glutMainLoop();
	return 0;
}
