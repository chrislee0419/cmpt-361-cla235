
# Updated by Luwei Yang
# Jan 2016
# Sample code for OpenGL tutorials of CMPT 361

1 demo_linux folder description
1.1 You can find three folders: cube, include, square. The applications and MakeFile are in folder "cube" and "square". You do not need to modify anything inside folder "include".

2. Folder cube and square
2.1 There are three applications in folder "cube". They are "example1", "example2" and "example3". Each of them has its corresponding shader files (*.glsl). Three applicaitons in folder "square" are "square", "rotate" and "translate".

2.2 In either directory, you can find and open the MakeFile, find the following lines:

# If you have more source files add them here 
SOURCE= example3.cpp ../include/InitShader.cpp

You can change "example3.cpp" to "exmpale1.cpp" or "example2.cpp"

2.3 Now open a terminal and "cd" into the directory of "cube". 

2.4 Type "make" and an application program "demo" should appear in the same folder.

2.5 run the applcaition by typing "./demo" in the same directory.

2.6 Every time you changed the MakeFile, you should run "make clean" and then "make" the new application.

2.7 Feel free to change the code and explore. Enjoy.
