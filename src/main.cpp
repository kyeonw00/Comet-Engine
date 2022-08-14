#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;

struct ShaderProgramSource
{
    string VertexSource;
    string FragmentSource;
};

static ShaderProgramSource ParseShader(const char* shader_file_path)
{
    // TODO: Specification for shader file that contains both of Vertex and Fragment shader

    enum class EShaderType
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1,
    };

    ifstream ShaderFileStream(shader_file_path, ios::in);
    stringstream ShaderCodeStream[2];
    string line;
    EShaderType CurrentShaderType = EShaderType::NONE;

    while (getline(ShaderFileStream, line))
    {
        if (line.find("#shader") != string::npos)
        {
            if (line.find("vertex") != string::npos)
            {
                // set mode to vertex
                CurrentShaderType = EShaderType::VERTEX;
            }
            else if (line.find("fragment") != string::npos)
            {
                // set mode to fragment
                CurrentShaderType = EShaderType::FRAGMENT;
            }
        }
        else
        {
            ShaderCodeStream[(int)CurrentShaderType] << line << "\n";
        }
    }

    return { ShaderCodeStream[0].str(), ShaderCodeStream[1].str() };
}

//static string LoadShaderCode(const char* shader_file_path)
//{
//    string ShaderCode;
//    ifstream ShaderStream(shader_file_path, ios::in);
//
//    if (ShaderStream.is_open())
//    {
//        stringstream StringStream;
//        StringStream << ShaderStream.rdbuf();
//        ShaderCode = StringStream.str();
//        ShaderStream.close();
//    }
//    else
//    {
//        cout << "Impossible to open " << shader_file_path << ". Check the given source path." << endl;
//        getchar();
//        return "";
//    }
//
//    return ShaderCode;
//}

static unsigned int CompileShader(const unsigned int type, const string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    // TODO: Error handling for syntax error.

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);

    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);

        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);

        cout << "Failed to compile shader!" << endl;
        cout << "ShaderType:" << (type == GL_VERTEX_SHADER ? "vertex shader" : "fragment shader") << endl;
        cout << message << endl;

        glDeleteShader(id);
        return 0;
    }

    return id;
}

static int CreateShader(const string& vertexShader, const string& fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
    {
        return -1;
    }

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Comet Engine", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
    {
        cout << "Error with initializin glew" << endl;
        return -1;
    }

    cout << "OpenGL version " << glGetString(GL_VERSION) << endl;

    float positions[6] = {
        -0.5f, -0.5f,
        0.0f, 0.5f,
        0.5f, -0.5f
    };

    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

    ShaderProgramSource ShaderSource = ParseShader("res\\shaders\\Shader.shader");
    cout << "Vertex" << endl;
    cout << ShaderSource.VertexSource << endl;
    cout << "Fragment" << endl;
    cout << ShaderSource.FragmentSource << endl;

    unsigned int shader = CreateShader(ShaderSource.VertexSource, ShaderSource.FragmentSource);

    glUseProgram(shader);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteProgram(shader);

    glfwTerminate();
    return 0;
}