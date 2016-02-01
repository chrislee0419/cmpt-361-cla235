#version 130

out vec4  fColor;

void
main()
{	
	//Option 1: Assign red color to the square
        //fColor = vec4( 1.0, 0.0, 0.0, 1.0 );
	
	//Option 2: Assign color according to the Fragment position
	//mix performs a linear interpolation between x and y using a to weight between them. The return 	 //value is computed as x*(1-a) + y*a 
	float lerpValue = gl_FragCoord.y / 512.0f;
	fColor = mix(vec4( 0.0, 0.0, 0.0, 1.0 ),
				vec4( 1.0, 0.0, 1.0, 1.0 ), lerpValue);
}
