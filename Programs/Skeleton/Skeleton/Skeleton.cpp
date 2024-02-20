//=============================================================================================
// Mintaprogram: Zöld háromszög. Ervenyes 2019. osztol.
//
// A beadott program csak ebben a fajlban lehet, a fajl 1 byte-os ASCII karaktereket tartalmazhat, BOM kihuzando.
// Tilos:
// - mast "beincludolni", illetve mas konyvtarat hasznalni
// - faljmuveleteket vegezni a printf-et kiveve
// - Mashonnan atvett programresszleteket forrasmegjeloles nelkul felhasznalni es
// - felesleges programsorokat a beadott programban hagyni!!!!!!! 
// - felesleges kommenteket a beadott programba irni a forrasmegjelolest kommentjeit kiveve
// ---------------------------------------------------------------------------------------------
// A feladatot ANSI C++ nyelvu forditoprogrammal ellenorizzuk, a Visual Studio-hoz kepesti elteresekrol
// es a leggyakoribb hibakrol (pl. ideiglenes objektumot nem lehet referencia tipusnak ertekul adni)
// a hazibeado portal ad egy osszefoglalot.
// ---------------------------------------------------------------------------------------------
// A feladatmegoldasokban csak olyan OpenGL fuggvenyek hasznalhatok, amelyek az oran a feladatkiadasig elhangzottak 
// A keretben nem szereplo GLUT fuggvenyek tiltottak.
//
// NYILATKOZAT
// ---------------------------------------------------------------------------------------------
// Nev    : 
// Neptun : 
// ---------------------------------------------------------------------------------------------
// ezennel kijelentem, hogy a feladatot magam keszitettem, es ha barmilyen segitseget igenybe vettem vagy
// mas szellemi termeket felhasznaltam, akkor a forrast es az atvett reszt kommentekben egyertelmuen jeloltem.
// A forrasmegjeloles kotelme vonatkozik az eloadas foliakat es a targy oktatoi, illetve a
// grafhazi doktor tanacsait kiveve barmilyen csatornan (szoban, irasban, Interneten, stb.) erkezo minden egyeb
// informaciora (keplet, program, algoritmus, stb.). Kijelentem, hogy a forrasmegjelolessel atvett reszeket is ertem,
// azok helyessegere matematikai bizonyitast tudok adni. Tisztaban vagyok azzal, hogy az atvett reszek nem szamitanak
// a sajat kontribucioba, igy a feladat elfogadasarol a tobbi resz mennyisege es minosege alapjan szuletik dontes.
// Tudomasul veszem, hogy a forrasmegjeloles kotelmenek megsertese eseten a hazifeladatra adhato pontokat
// negativ elojellel szamoljak el es ezzel parhuzamosan eljaras is indul velem szemben.
//=============================================================================================
#include "framework.h"
#include <iostream>

// vertex shader in GLSL: It is a Raw string (C++11) since it contains new line characters
const char * const vertexSource = R"(
	#version 330				// Shader 3.3
	precision highp float;		// normal floats, makes no difference on desktop computers

	uniform mat4 MVP;			// uniform variable, the Model-View-Projection transformation matrix
	layout(location = 0) in vec2 vp;	// Varying input: vp = vertex position is expected in attrib array 0

	void main() {
		gl_Position = vec4(vp.x, vp.y, 0, 1) * MVP;		// transform vp from modeling space to normalized device space
	}
)";

// fragment shader in GLSL
const char * const fragmentSource = R"(
	#version 330			// Shader 3.3
	precision highp float;	// normal floats, makes no difference on desktop computers
	
	uniform vec3 color;		// uniform variable, the color of the primitive
	out vec4 outColor;		// computed color of the current pixel

	void main() {
		outColor = vec4(color, 1);	// computed color is the color of the primitive
	}
)";

GPUProgram gpuProgram; // vertex and fragment shaders
unsigned int vao, vboPoints;  // virtual world on the GPU
unsigned int vboSelected;  // Buffer for selected points

std::vector<GLfloat> vertices; //here I store the coordinates x and y
std::vector<GLfloat> selected;

bool lKeyPressed = false;
bool pKeyPressed = false;

// Initialization, create an OpenGL context
void onInitialization() {
	glViewport(0, 0, windowWidth, windowHeight);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vboPoints);	// Generate 1 buffer
	glBindBuffer(GL_ARRAY_BUFFER, vboPoints);
	glEnableVertexAttribArray(0);  // AttribArray 0
	glVertexAttribPointer(0,       // vbo -> AttribArray 0
		2, GL_FLOAT, GL_FALSE, // two floats/attrib, not fixed-point
		0, NULL); 		     // stride, offset: tightly packed
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glGenBuffers(1, &vboSelected);  // Generate buffer for selected points
	glBindBuffer(GL_ARRAY_BUFFER, vboSelected);

	// create program for the GPU
	gpuProgram.create(vertexSource, fragmentSource, "outColor");
}
GLint currently_binded_vbo;
// Window has become invalid: Redraw
void onDisplay() {

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f); // Set clear color to grey
	glClear(GL_COLOR_BUFFER_BIT); // clear frame buffer

	// Set color to (1, 0, 0) = red
	int location = glGetUniformLocation(gpuProgram.getId(), "color");
	glUniform3f(location, 1.0f, 0.0f, 0.0f); // 3 floats

	float MVPtransf[4][4] = { 1, 0, 0, 0,    // MVP matrix, 
							  0, 1, 0, 0,    // row-major!
							  0, 0, 1, 0,
							  0, 0, 0, 1 };

	location = glGetUniformLocation(gpuProgram.getId(), "MVP");	// Get the GPU location of uniform variable MVP
	glUniformMatrix4fv(location, 1, GL_TRUE, &MVPtransf[0][0]);	// Load a 4x4 row-major float matrix to the specified location

	glBindVertexArray(vao);

	std::cout << vboPoints << " id of the points" << std::endl;
	std::cout << vboSelected << " id of the selected points" << std::endl;
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &currently_binded_vbo);

		// Draw points
		glBindBuffer(GL_ARRAY_BUFFER, vboPoints);  // Draw call
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
		glPointSize(10.0f);
		glDrawArrays(GL_POINTS, 0 , vertices.size() / 2 );

		// Draw selected points/lines 
		if (selected.size() == 4) {
			glBufferData(GL_ARRAY_BUFFER, selected.size() * sizeof(float), &selected[0], GL_STATIC_DRAW);
			glDrawArrays(GL_LINES, 0, selected.size() / 2);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}

		glutSwapBuffers(); // exchange buffers for double buffering
		
}

// Key of ASCII code pressed
void onKeyboard(unsigned char key, int pX, int pY) {
	switch (key) {
	case 'l':
		printf("l key pressed\n");
		lKeyPressed = true;
		pKeyPressed = false;
		break;
	case 'p':
		printf("p key pressed\n");
		pKeyPressed = true;
		lKeyPressed = false;
		break;
	default:
		printf("Wrong key");
		break;
	}
}

// Mouse click event
void onMouse(int button, int state, int pX, int pY) {
	// Convert to normalized device space
	float cX = 2.0f * pX / windowWidth - 1;    // flip y axis
	float cY = 1.0f - 2.0f * pY / windowHeight;

	if (state == GLUT_DOWN) {
		if (pKeyPressed) {
			// Add the clicked point to the vertices
			vertices.push_back(cX);
			vertices.push_back(cY);
			
		}
		else if (lKeyPressed) {
			// Find the closest point to the clicked location
			float threshold = 0.05f; // Adjust this value as needed
			int closestPointIndex = -1;
			for (int i = 0; i < vertices.size(); i += 2) {
				float dx = vertices[i] - cX;
				float dy = vertices[i + 1] - cY;
				if (dx * dx + dy * dy < pow(threshold, 2)){
					closestPointIndex = i;
					break;
				}
			}
			if (closestPointIndex != -1) {
				// Clear the selected points if already present
				if (selected.size() > 4) {
					selected.clear();
				}
				// Add the clicked point to the selected points
				selected.push_back(vertices[closestPointIndex]);
				selected.push_back(vertices[closestPointIndex + 1]);
				std::cout << selected[0] << " " << selected[1];
			}
		}
	}
}

// Move mouse with key pressed
void onMouseMotion(int pX, int pY) {	// pX, pY are the pixel coordinates of the cursor in the coordinate system of the operation system
	// Convert to normalized device space
	float cX = 2.0f * pX / windowWidth - 1;	// flip y axis
	float cY = 1.0f - 2.0f * pY / windowHeight;
	printf("Mouse moved to (%3.2f, %3.2f)\n", cX, cY);
}

// Key of ASCII code released
void onKeyboardUp(unsigned char key, int pX, int pY) {
}

// Idle event indicating that some time elapsed: do animation here
void onIdle() {
	long time = glutGet(GLUT_ELAPSED_TIME); // elapsed time since the start of the program
}
