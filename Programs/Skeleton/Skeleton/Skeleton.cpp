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
// Nev    : Szlovak Anna
// Neptun : OPOFGK
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

class PointCollection {
public: std::vector<GLfloat> vertices;

	  void add(float cX, float cY) {// add the clicked point to the vertices
		  vertices.push_back(cX);
		  vertices.push_back(cY);
		  printf("Point added at coordinates: %f, %f\n", cX, cY);
	  }

	  void findClosest(float cX, float cY, PointCollection &selected) {
		  //find the closest point to the clicked location
		  float threshold = 0.05f; // this can be adjusted
		  int closestPointIndex = -1;
		  for (int i = 0; i < vertices.size(); i += 2) {
			  float dx = vertices[i] - cX;
			  float dy = vertices[i + 1] - cY;
			  if (dx * dx + dy * dy < pow(threshold, 2)) {
				  closestPointIndex = i;
				  break;
			  }
		  }
		  if (closestPointIndex != -1) {
			  selected.vertices.push_back(vertices[closestPointIndex]);
			  selected.vertices.push_back(vertices[closestPointIndex + 1]);
		  }
	  }

	  void drawPoints(float size, float R, float G, float B, int &location) {
		  glUniform3f(location, R, G, B); // 3 floats
		  //bind buffer
		  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
		  glPointSize(size);

		  glDrawArrays(GL_POINTS, 0, vertices.size() / 2);
	  }
};

class Line {
public: float m, b;
	  bool vertical;
	  float x_val;


	  Line(float m, float b) {
		  this->m = m;
		  this->b = b;
	  };

	  Line(float x1, float y1, float x2, float y2) {
		  if (x1 == x2) {
			  vertical = true;
			  x_val = x1;

		}
		  else {
			  vertical = false;
			  m = (y2 - y1) / (x2 - x1);
			  b = ((-(y2 - y1) / (x2 - x1)) * x1) + y1;
		  }
	  };

};

class LineCollection {
public: std::vector<Line> lineCollection;
		Line* selectedline = nullptr;

	  void add(Line &l) {
		  lineCollection.push_back(l);
	  };

	  void drawLines(float R, float G, float B, int &location) {
		  std::vector<float> points;
		  for (int line = 0; line < lineCollection.size(); line++) {
			  if (lineCollection[line].vertical == false) {
				  points.push_back(-1.0f);
				  points.push_back(lineCollection[line].m * (-1.0f) + lineCollection[line].b);
				  points.push_back(1.0f);
				  points.push_back(lineCollection[line].m * (1.0f) + lineCollection[line].b);
			  }
			  else {
				  points.push_back(lineCollection[line].x_val);
				  points.push_back(1.0f);
				  points.push_back(lineCollection[line].x_val);
				  points.push_back(-1.0f);
			  }

		  }
		  glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(float), &points[0], GL_STATIC_DRAW);
		  glUniform3f(location, R, G, B); // 3 floats
		  glDrawArrays(GL_LINES, 0, points.size());
	  };

	  int getSize() {
		  return lineCollection.size();
	  };

	  void selectClickedLine(float cX, float cY) {
		  for (Line &l : lineCollection) {
			  if (abs(l.m * cX + l.b - cY) < 0.01f) {
				  printf("Line selected at point %f %f\n", cX, cY); //for debug
				  selectedline = &l;
				  break;
			  }
		 }
	  };

	  void dragSelectedLine(float cX, float cY) {
		  if (selectedline) { 
			  selectedline->b = cY - selectedline->m * cX; //modify the +b part of the equation, slope stays the same
		  }
	  };

	  void selectTwoLines(float cX, float cY, LineCollection& selected) {
			  for (Line& l : lineCollection) {
				  if (abs(l.m * cX + l.b - cY) < 0.05f) {
					  printf("Line selected at point %f %f\n", cX, cY); //for debug
					  selected.add(l);
					  break;
				  }
			  }
		  
	  };

	  void addPointOfIntersection(Line l1, Line l2, PointCollection& pointcollection) {
		  if (l1.m == l2.m) { //they are parallel, no intersection
			  return; //do nothing
		  }
		  float x = (l2.b - l1.b) / (l1.m - l2.m);
		  float y = l1.m * x + l1.b;
		  pointcollection.add(x, y);
	  }
};

GPUProgram gpuProgram; // vertex and fragment shaders
unsigned int vao, vboPoints;  // virtual world on the GPU

bool lKeyPressed = false;
bool pKeyPressed = false;
bool mKeyPressed = false;
bool iKeyPressed = false;

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

	// create program for the GPU
	gpuProgram.create(vertexSource, fragmentSource, "outColor");
}
//GLOBAL VARIABLES TO STORE OUR LINES AND VERTICES
PointCollection points; //here I store the coordinates x and y of every drawn point
PointCollection selected; //these will be the points of the lines

LineCollection collection;
LineCollection selectedLines;

// Window has become invalid: Redraw
void onDisplay() {

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f); // Set clear color to grey
	glClear(GL_COLOR_BUFFER_BIT); // clear frame buffer

	// Set color to (1, 0, 0) = red
	int colorloc = glGetUniformLocation(gpuProgram.getId(), "color");
	glUniform3f(colorloc, 1.0f, 0.0f, 0.0f); // 3 floats

	float MVPtransf[4][4] = { 1, 0, 0, 0,    // MVP matrix, 
							  0, 1, 0, 0,    // row-major!
							  0, 0, 1, 0,
							  0, 0, 0, 1 };

	int location = glGetUniformLocation(gpuProgram.getId(), "MVP");	// Get the GPU location of uniform variable MVP
	glUniformMatrix4fv(location, 1, GL_TRUE, &MVPtransf[0][0]);	// Load a 4x4 row-major float matrix to the specified location

	glBindVertexArray(vao);

		// Draw points
		glBindBuffer(GL_ARRAY_BUFFER, vboPoints);  // Draw call
		points.drawPoints(10.0f, 1.0f, 0.0f, 0.0f, colorloc);

		if (lKeyPressed) { //we dont want to add lines when we drag an existing one
			// Draw lines 
			if (selected.vertices.size() >= 4 ) {
			
					for (int i = 0; i < selected.vertices.size(); i += 4) { //we add every 2 pair of points, but only when we indeed have 2 pairs selected
						if (i + 4 == selected.vertices.size()) {
							Line line(selected.vertices[i], selected.vertices[i + 1], selected.vertices[i + 2], selected.vertices[i + 3]);
							collection.add(line);
							printf("Equation of the line: y = %f x + %f\n", line.m, line.b);
						}
					}
			}
		}
		
			if (selectedLines.getSize() == 2) {
				selectedLines.addPointOfIntersection(selectedLines.lineCollection[0], selectedLines.lineCollection[1], points);
				selectedLines.lineCollection.clear();
				printf("Point of intersection is added if it existed.");
			}
		
		glLineWidth(3.0f);		
		collection.drawLines(0.0f, 1.0f, 1.0f, colorloc);
			
		glutSwapBuffers(); // exchange buffers for double buffering
}

// Key of ASCII code pressed
void onKeyboard(unsigned char key, int pX, int pY) {
	switch (key) {
	case 'l':
		printf("Select points to draw lines\n");
		lKeyPressed = true;
		pKeyPressed = false;
		mKeyPressed = false;
		iKeyPressed = false;
		break;
	case 'p':
		printf("Draw points\n");
		pKeyPressed = true;
		lKeyPressed = false;
		mKeyPressed = false;
		iKeyPressed = false;
		break;
	case 'm':
		printf("Select line to drag\n");
		mKeyPressed = true;
		lKeyPressed = false;
		pKeyPressed = false;
		iKeyPressed = false;
		break;
	case 'i':
		printf("Select two lines to find intersection\n");
		iKeyPressed = true;
		lKeyPressed = false;
		pKeyPressed = false;
		mKeyPressed = false;
		break;
	default:
		printf("Wrong key\n");
		break;
	}
}

// Mouse click event
void onMouse(int button, int state, int pX, int pY) {
	// Convert to normalized device space
	float cX = 2.0f * pX / windowWidth - 1;    // flip y axis
	float cY = 1.0f - 2.0f * pY / windowHeight;

	if (state == GLUT_UP) { //so when we let go of the selected line, at the next click we can select a new one and forget the previously selected
		collection.selectedline = nullptr;
	}
	else if (state == GLUT_DOWN) {
		if (pKeyPressed) {
			points.add(cX, cY);
		}
		else if (lKeyPressed) {
			points.findClosest(cX, cY, selected);
		}
		else if (mKeyPressed) {
			collection.selectClickedLine(cX, cY);
		}
		else if (iKeyPressed) {
			collection.selectTwoLines(cX, cY, selectedLines); //twice because we need two lines
			glutPostRedisplay();
		}
	}
}

// Move mouse with key pressed
void onMouseMotion(int pX, int pY) {	// pX, pY are the pixel coordinates of the cursor in the coordinate system of the operation system
	// Convert to normalized device space
	float cX = 2.0f * pX / windowWidth - 1;	// flip y axis
	float cY = 1.0f - 2.0f * pY / windowHeight;
	//printf("Mouse moved to (%3.2f, %3.2f)\n", cX, cY);

	collection.dragSelectedLine(cX, cY);
	glutPostRedisplay();
}

// Key of ASCII code released
void onKeyboardUp(unsigned char key, int pX, int pY) {
}

// Idle event indicating that some time elapsed: do animation here
void onIdle() {
	long time = glutGet(GLUT_ELAPSED_TIME); // elapsed time since the start of the program
}