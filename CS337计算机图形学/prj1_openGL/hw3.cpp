#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
float pi = 3.1415926;
const float radius = 0.2f;
const float leftox = -0.6f;
const float rightox = 0.6f;
const float oy = -0.6f;
const int tri_in_cir = 100;

//以下为glsl写的顶点着色器
const char *vertexShaderSource = "#version 410 core\n"
"layout (location = 0) in vec2 aPos;\n"
"layout (location = 1) in vec3 ColorIn;\n"
"uniform mat4 transform;\n"
"out vec3 elementColor;\n"
"void main()\n"
"{\n"
"   gl_Position = transform * vec4(aPos,0,1.0);\n"
"	elementColor = ColorIn;\n"
"}\0";

//以下为glsl写的片元着色器
const char *fragmentShaderSource = "#version 410 core\n"
"in vec3 elementColor;\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(elementColor,1.0);\n"
"}\n\0";

class Car {
private:
	const char *carVertexShader;
	const char *carFragmentShader;
	float *leftWheel,*rightWheel,*body;
	int leftWheelSize, rightWheelSize, bodySize;
	unsigned int VBO[3], VAO[3];
	int shaderProgram, vertexShader, fragmentShader;
public:
	Car(float lw[], int lws,float rw[], int rws, float b[], int bs, const char *s1, const char *s2)
	{
		leftWheel = lw;
		leftWheelSize = lws;
		rightWheel = rw;
		rightWheelSize = rws;
		body = b; 
		bodySize = bs;
		carVertexShader = s1;
		carFragmentShader = s2;
	}
	~Car() {
		glDeleteVertexArrays(3, VAO);
		glDeleteBuffers(3, VBO);
	}
	void setVertexShader(char *s) {
		carFragmentShader = s;
	}
	void setFragmemtShader(char *s) {
		carFragmentShader = s;
	}
	void buildCar() {
		// build and compile our shader program
		vertexShader = glCreateShader(GL_VERTEX_SHADER);

		glShaderSource(vertexShader, 1, &vertexShaderSource, NULL); //将glsl源码编译到点元着色器上

		glCompileShader(vertexShader);

		// check for shader compile errors

		int success;

		char infoLog[512];

		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

		if (!success)
		{
			glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
				<< infoLog << std::endl;
		}

		// fragment shader

		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);

		glCompileShader(fragmentShader);

		// check for shader compile errors

		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

		if (!success)
		{
			glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);

			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
				<< infoLog << std::endl;
		}

		// link shaders 通过着色器链接各个着色器

		shaderProgram = glCreateProgram();

		glAttachShader(shaderProgram, vertexShader);

		glAttachShader(shaderProgram, fragmentShader);

		glLinkProgram(shaderProgram);

		// check for linking errors

		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

		if (!success)
		{

			glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);

			std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
				<< infoLog << std::endl;
		}
		glDeleteShader(vertexShader); //删掉已经链接过的着色器
		glDeleteShader(fragmentShader);

		glGenVertexArrays(3, VAO);
		glGenBuffers(3, VBO);
		// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
		
		glBindVertexArray(VAO[0]);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, bodySize, body, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(2 * sizeof(float)));
		// 这个是针对的单个点反映的是怎么描述的这个点

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind

		//glBindBuffer(GL_ARRAY_BUFFER, 0);
		// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
		// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
		//glBindVertexArray(0);

		glBindVertexArray(VAO[1]);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		glBufferData(GL_ARRAY_BUFFER, leftWheelSize, leftWheel, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(2 * sizeof(float)));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glBindVertexArray(VAO[2]);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
		glBufferData(GL_ARRAY_BUFFER, rightWheelSize, rightWheel, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(2 * sizeof(float)));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
	}
	void drawCar(glm::mat4 transform) {
		unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
		glUseProgram(shaderProgram); //激活shader
		glBindVertexArray(VAO[0]);
		glDrawArrays(GL_TRIANGLES, 0, 9);
		glBindVertexArray(VAO[1]);
		glDrawArrays(GL_TRIANGLES, 0, tri_in_cir * 3);
		glBindVertexArray(VAO[2]);
		glDrawArrays(GL_TRIANGLES, 0, tri_in_cir * 3);
	}
};




int main()
{
	// glfw: initialize and configure
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X

	// glfw window creation
	GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "hw3", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); //窗口调整调用该函数

	// glad: load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// set up vertex data (and buffer(s)) and configure vertex attributes
	
	float vertices[] = {
		//pos			//color
		-0.5f, 0.5f, 0, 0, 1.0f,
		0.5f, 0.5f, 0, 0, 1.0f,
		0, 1.0f, 0, 0, 1.0f, //tri

		-0.8f, 0.5f, 1.0f, 0, 0,
		0.8f, 0.5f, 1.0f, 0, 0,
		-0.8f, -0.5f, 1.0f, 0, 0,

		0.8f, 0.5f, 1.0f, 0, 0,
		-0.8f, -0.5f, 1.0f, 0, 0,
		0.8f, -0.5f, 1.0f, 0, 0, //rec
	};
	float circle1[tri_in_cir * 15]; //left cir
	for (int i = 0; i < tri_in_cir; ++i)
	{
		circle1[i * 15] = leftox;
		circle1[i * 15 + 1] = oy;
		circle1[i * 15 + 2] = 0;
		circle1[i * 15 + 3] = 1.0f;
		circle1[i * 15 + 4] = 0;

		circle1[i * 15 + 5] = leftox + radius * float(sin(2 * pi / tri_in_cir * i));
		circle1[i * 15 + 6] = oy + radius * float(cos(2 * pi / tri_in_cir * i));
		circle1[i * 15 + 7] = 0;
		circle1[i * 15 + 8] = 1.0f;
		circle1[i * 15 + 9] = 0;

		circle1[i * 15 + 10] = leftox + radius * float(sin(2 * pi / tri_in_cir * (i + 1)));
		circle1[i * 15 + 11] = oy + radius * float(cos(2 * pi / tri_in_cir * (i + 1)));
		circle1[i * 15 + 12] = 0;
		circle1[i * 15 + 13] = 1.0f;
		circle1[i * 15 + 14] = 0;
	}
	float circle2[tri_in_cir * 15];
	for (int i = 0; i < tri_in_cir; ++i)
	{
		circle2[i * 15] = rightox;
		circle2[i * 15 + 1] = oy;
		circle2[i * 15 + 2] = 1.0f;
		circle2[i * 15 + 3] = 1.0f;
		circle2[i * 15 + 4] = 0;

		circle2[i * 15 + 5] = rightox + radius * float(sin(2 * pi / tri_in_cir * i));
		circle2[i * 15 + 6] = oy + radius * float(cos(2 * pi / tri_in_cir * i));
		circle2[i * 15 + 7] = 1.0f;
		circle2[i * 15 + 8] = 1.0f;
		circle2[i * 15 + 9] = 0;

		circle2[i * 15 + 10] = rightox + radius * float(sin(2 * pi / tri_in_cir * (i + 1)));
		circle2[i * 15 + 11] = oy + radius * float(cos(2 * pi / tri_in_cir * (i + 1)));
		circle2[i * 15 + 12] = 1.0f;
		circle2[i * 15 + 13] = 1.0f;
		circle2[i * 15 + 14] = 0;
	}

	
	// uncomment this call to draw in wireframe polygons.
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	Car myFirstCar(circle1, sizeof(circle1), circle2, sizeof(circle2), vertices, sizeof(vertices), vertexShaderSource, fragmentShaderSource);
	myFirstCar.buildCar();
	// render loop
	while (!glfwWindowShouldClose(window))
	{ //渲染循环防止直接退出

		// input
		processInput(window);

		// render
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glm::mat4 transform;
		transform = glm::translate(transform, glm::vec3(sin((float)glfwGetTime()), 0.0f, 1.0f));

		myFirstCar.drawCar(transform);
		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwSwapBuffers(window);

		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	
	// glfw: terminate, clearing all previously allocated GLFW resources.
	glfwTerminate();

	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly

// ---------------------------------------------------------------------------------------------------------

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and
	// height will be significantly larger than specified on retina displays.

	glViewport(0, 0, width, height);
}
