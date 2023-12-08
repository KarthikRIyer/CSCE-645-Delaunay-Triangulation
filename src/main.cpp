#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <sstream>
#include <vector>
#include <fstream>
#include <string>
#include <chrono>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"
#include "Triangulation.h"

using namespace std;
using namespace glm;

shared_ptr<Program> progSimple;
shared_ptr<Program> progTri;
bool keyToggles[256] = {false}; // only for English keyboards!

GLFWwindow *window; // Main application window
string RESOURCE_DIR = ""; // Where the resources are loaded from

std::vector<glm::vec2> points;
Triangulation t;

static void error_callback(int error, const char *description)
{
	cerr << description << endl;
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
}

static void char_callback(GLFWwindow *window, unsigned int key)
{
	keyToggles[key] = !keyToggles[key];
	switch(key) {
        case 'c':
            points.clear();
            t.reset();
            break;
        case 't': {
            t.reset();
            auto start = std::chrono::high_resolution_clock::now();
            t.voronoi(points);
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            std::cout<<"Time taken: "<<duration.count()<<" microseconds\n";
            break;
        }
	    case 'g':
        {
            points.clear();
            int width, height;
            glfwGetWindowSize(window, &width, &height);
            float aspect = (float)width/height;
            double ymax = 100;
            double xmax = aspect*ymax;
            double yBound = ymax - 10;
            double xBound = xmax - 10;
            points.push_back(glm::vec2(-xBound, -yBound));
            points.push_back(glm::vec2(xBound, -yBound));
            points.push_back(glm::vec2(xBound, yBound));
            points.push_back(glm::vec2(-xBound, yBound));
            for (int i = 0; i < 100; i++) {
                float x = -xBound + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(2*xBound)));
                float y = -yBound + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(2*yBound)));
                points.push_back(glm::vec2(x,y));
            }
            t.reset();
            break;
        }
        case 'a':
        {
            points.clear();
            int width, height;
            glfwGetWindowSize(window, &width, &height);
            float aspect = (float)width/height;
            double ymax = 100;
            double xmax = aspect*ymax;
            double yBound = ymax - 10;
            double xBound = xmax - 10;

            float x = -10;
            float y = -10;
            points.push_back(glm::vec2(x,y));
            x = -20;
            y = -30;
            points.push_back(glm::vec2(x,y));
            x = 40;
            y = -20;
            points.push_back(glm::vec2(x,y));
            x = 30;
            y = 40;
            points.push_back(glm::vec2(x,y));
            x = 10;
            y = 10;
            points.push_back(glm::vec2(x,y));
            x = -20;
            y = 30;
            points.push_back(glm::vec2(x,y));
            x = -20;
            y = 20;
            points.push_back(glm::vec2(x,y));

            t.reset();
            break;
        }
        case 's':
        {
            points.clear();
            int width, height;
            glfwGetWindowSize(window, &width, &height);
            float aspect = (float)width/height;
            double ymax = 100;
            double xmax = aspect*ymax;
            double yBound = ymax - 10;
            double xBound = xmax - 10;

            float x = 0;
            float y = 0;
            points.push_back(glm::vec2(x+50,y));
            x = -133;
            y = 0;
            points.push_back(glm::vec2(x+50,y));
            x = -65;
            y = -1.5;
            points.push_back(glm::vec2(x+50,y));
            x = -68;
            y = -1.5;
            points.push_back(glm::vec2(x+50,y));

            t.reset();
            break;
        }
        case 'd':
        {
            points.clear();
            int width, height;
            glfwGetWindowSize(window, &width, &height);
            float aspect = (float)width/height;
            double ymax = 100;
            double xmax = aspect*ymax;
            double yBound = ymax - 10;
            double xBound = xmax - 10;
            int n = 20;
            for (float i = -xBound; i <= xBound; i += (xBound/n)) {
                for (float j = -yBound; j <= yBound; j += (yBound/n)) {
                    points.push_back(glm::vec2(i,j));
                }
            }

            t.reset();
            break;
        }
	}
}

static void cursor_position_callback(GLFWwindow* window, double xmouse, double ymouse)
{
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        // Get the current mouse position.
        double xmouse, ymouse;
        glfwGetCursorPos(window, &xmouse, &ymouse);

        // Get current window size.
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        // Convert from window coord to world coord assuming that we're
        // using an orthgraphic projection
        double aspect = (double)width/height;
        double ymax = 100;
        double xmax = aspect*ymax;
        double yBound = ymax - 10;
        double xBound = xmax - 10;
        glm::vec2 p;
        p.x = 2.0 * xmax * ((xmouse / width) - 0.5);
        p.y = 2.0 * ymax * (((height - ymouse) / height) - 0.5);

        if (p.x > xBound || p.x < -xBound || p.y > yBound || p.y < -yBound) return;

        points.push_back(p);
        t.reset();
        t.voronoi(points);
    }
}

static void init()
{
	GLSL::checkVersion();
	
	// Set background color
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	// Enable z-buffer test
//	glEnable(GL_DEPTH_TEST);
	
	keyToggles[(unsigned)'c'] = true;

    progSimple = make_shared<Program>();
    progSimple->setShaderNames(RESOURCE_DIR + "simple_vert.glsl", RESOURCE_DIR + "simple_frag.glsl");
    progSimple->setVerbose(true); // Set this to true when debugging.
    progSimple->init();
    progSimple->addUniform("P");
    progSimple->addUniform("MV");
    progSimple->setVerbose(false);

    progTri = make_shared<Program>();
    progTri->setShaderNames(RESOURCE_DIR + "tri_vert.glsl", RESOURCE_DIR + "tri_frag.glsl");
    progTri->setVerbose(true); // Set this to true when debugging.
    progTri->init();
    progTri->addUniform("P");
    progTri->addUniform("MV");
    progTri->addAttribute("aPos");
    progTri->addAttribute("col");
    progTri->setVerbose(false);

    t = Triangulation();
    t.init();

	// Initialize time.
	glfwSetTime(0.0);
	
	// If there were any OpenGL errors, this will print something.
	// You can intersperse this line in your code to find the exact location
	// of your OpenGL error.
	GLSL::checkError(GET_FILE_LINE);
}

void render()
{
	// Get current frame buffer size.
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	
	// Clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if(keyToggles[(unsigned)'c']) {
		glEnable(GL_CULL_FACE);
	} else {
		glDisable(GL_CULL_FACE);
	}
	if(keyToggles[(unsigned)'l']) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	} else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	
	auto P = make_shared<MatrixStack>();
	auto MV = make_shared<MatrixStack>();
	P->pushMatrix();
	MV->pushMatrix();
	
	// Apply camera transforms
	double aspect = (double)(width)/(height);
	double ymax = 100;
	double xmax = aspect*ymax;
	P->multMatrix(glm::ortho(-xmax, xmax, -ymax, ymax, -1.0, 1.0));
	double yBound = ymax - 10;
	double xBound = xmax - 10;
	// Draw grid
	progSimple->bind();
	glUniformMatrix4fv(progSimple->getUniform("P"), 1, GL_FALSE, glm::value_ptr(P->topMatrix()));
	glUniformMatrix4fv(progSimple->getUniform("MV"), 1, GL_FALSE, glm::value_ptr(MV->topMatrix()));
	// Draw grid lines
	glLineWidth(1.0f);
	glColor3d(0.0, 0.0, 0.0);
	glBegin(GL_LINES);
    glVertex2d( -xBound, -yBound);
    glVertex2d( xBound, -yBound);
    glVertex2d( xBound, -yBound);
    glVertex2d( xBound, yBound);
    glVertex2d( xBound, yBound);
    glVertex2d( -xBound, yBound);
    glVertex2d( -xBound, yBound);
    glVertex2d( -xBound, -yBound);
	glEnd();
	
	// Draw shape
	if (!points.empty()) {
        glPointSize(5);
        glBegin(GL_POINTS);
        glColor3d(0.3, 0.3, 0.3);
        for (glm::vec2 p: points) {
            glVertex2d(p.x, p.y);
        }
        glEnd();
	}
    progSimple->unbind();

//    progTri->bind();
//    glUniformMatrix4fv(progTri->getUniform("P"), 1, GL_FALSE, glm::value_ptr(P->topMatrix()));
//    glUniformMatrix4fv(progTri->getUniform("MV"), 1, GL_FALSE, glm::value_ptr(MV->topMatrix()));
	t.draw(progTri, progSimple, P, MV);
//    progTri->unbind();
//    progSimple->unbind();
	// Pop stacks
	MV->popMatrix();
	P->popMatrix();
	
	GLSL::checkError(GET_FILE_LINE);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        cout << "Please specify the resource directory." << endl;
        return 0;
    }
    RESOURCE_DIR = argv[1] + string("/");



    // Set error callback.
    glfwSetErrorCallback(error_callback);
    // Initialize the library.
    if (!glfwInit()) {
        return -1;
    }
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    // Create a windowed mode window and its OpenGL context.
    window = glfwCreateWindow(640, 480, "DELAUNAY TRIANGULATION", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    // Make the window's context current.
    glfwMakeContextCurrent(window);
    // Initialize GLEW.
    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        cerr << "Failed to initialize GLEW" << endl;
        return -1;
    }
    glGetError(); // A bug in glewInit() causes an error that we can safely ignore.
    cout << "OpenGL version: " << glGetString(GL_VERSION) << endl;
    cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
    // Set vsync.
    glfwSwapInterval(1);
    // Set keyboard callback.
    glfwSetKeyCallback(window, key_callback);
    // Set char callback.
    glfwSetCharCallback(window, char_callback);
    // Set cursor position callback.
    glfwSetCursorPosCallback(window, cursor_position_callback);
    // Set mouse button callback.
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    // Initialize scene.
    init();

    // Loop until the user closes the window.
    while (!glfwWindowShouldClose(window)) {
        if (!glfwGetWindowAttrib(window, GLFW_ICONIFIED)) {
            // Render scene.
            render();
            // Swap front and back buffers.
            glfwSwapBuffers(window);
        }
        // Poll for and process events.
        glfwPollEvents();
    }
    // Quit program.
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
