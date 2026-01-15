// main.cpp
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>

// Vertex shader source (shared)
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

void main()
{
    TexCoord = aTexCoord;
    gl_Position = vec4(aPos, 1.0);
}
)";

// Fragment shader for normal texture
const char* normalFragSource = R"(
#version 330 core
out vec4 FragColor;
in vec2 TexCoord;

void main()
{
    FragColor = vec4(0.8, 0.4, 0.2, 1.0); // Orange-ish
}
)";

// Fragment shader for "effect" (simple wave)
const char* waveFragSource = R"(
#version 330 core
out vec4 FragColor;
in vec2 TexCoord;
uniform float time;

void main()
{
    float wave = sin(TexCoord.x * 10.0 + time * 5.0) * 0.1;
    FragColor = vec4(0.2 + wave, 0.5, 1.0, 1.0); // bluish wavy effect
}
)";

// Check shader compilation
void checkCompile(unsigned int shader)
{
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "Shader compilation error:\n" << infoLog << std::endl;
    }
}

// Check program linking
void checkLink(unsigned int program)
{
    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cout << "Program linking error:\n" << infoLog << std::endl;
    }
}

int main()
{
    // Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Two Quads One Shader Effect", NULL, NULL);
    if (!window) { std::cout << "Failed to create GLFW window\n"; return -1; }
    glfwMakeContextCurrent(window);

    // Load OpenGL functions
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) { std::cout << "Failed to init GLAD\n"; return -1; }

    // ---------------------
    // Build shaders
    // ---------------------
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader); checkCompile(vertexShader);

    unsigned int normalFrag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(normalFrag, 1, &normalFragSource, NULL);
    glCompileShader(normalFrag); checkCompile(normalFrag);

    unsigned int waveFrag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(waveFrag, 1, &waveFragSource, NULL);
    glCompileShader(waveFrag); checkCompile(waveFrag);

    unsigned int normalShader = glCreateProgram();
    glAttachShader(normalShader, vertexShader);
    glAttachShader(normalShader, normalFrag);
    glLinkProgram(normalShader); checkLink(normalShader);

    unsigned int waveShader = glCreateProgram();
    glAttachShader(waveShader, vertexShader);
    glAttachShader(waveShader, waveFrag);
    glLinkProgram(waveShader); checkLink(waveShader);

    glDeleteShader(vertexShader);
    glDeleteShader(normalFrag);
    glDeleteShader(waveFrag);

    // ---------------------
    // Vertex data (two quads)
    // ---------------------
    float vertices[] = {
        // positions        // texcoords
        // Quad 1 (normal)
        -0.9f,  0.5f, 0.0f,  0.0f,1.0f, // top-left
        -0.9f,  0.0f, 0.0f,  0.0f,0.0f, // bottom-left
        -0.5f,  0.0f, 0.0f,  1.0f,0.0f, // bottom-right
        -0.5f,  0.5f, 0.0f,  1.0f,1.0f, // top-right

        // Quad 2 (wave)
         0.5f,  0.5f, 0.0f,  0.0f,1.0f, // top-left
         0.5f,  0.0f, 0.0f,  0.0f,0.0f, // bottom-left
         0.9f,  0.0f, 0.0f,  1.0f,0.0f, // bottom-right
         0.9f,  0.5f, 0.0f,  1.0f,1.0f  // top-right
    };

    unsigned int indices[] = {
        0,1,2, 0,2,3, // quad1
        4,5,6, 4,6,7  // quad2
    };

    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // TexCoord
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    // ---------------------
    // Render loop
    // ---------------------
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(VAO);

        // Draw first quad normally
        normalShader;
        glUseProgram(normalShader);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);

        // Draw second quad with wave effect
        glUseProgram(waveShader);
        float timeValue = glfwGetTime();
        int timeLoc = glGetUniformLocation(waveShader, "time");
        glUniform1f(timeLoc, timeValue);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(6 * sizeof(unsigned int)));

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(normalShader);
    glDeleteProgram(waveShader);

    glfwTerminate();
    return 0;
}
