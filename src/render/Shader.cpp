#include <GL/glew.h>
#include <iostream>
#include "render/Shader.hpp"
#include <fstream>
#include <sstream>

Shader::Shader()
: mVertexShader(0)
, mFragShader(0)
, mShaderProgram(0)
{
}

Shader::~Shader()
{
}

bool Shader::Load(const std::string& name)
{
	// Use the two-parameter version with constructed paths
	return Load(name + ".vert", name + ".frag");
}

bool Shader::Load(const std::string& vertShaderPath, const std::string& fragShaderPath)
{
	// Compile vertex and fragment shaders with separate paths
	if (!CompileShader(vertShaderPath, GL_VERTEX_SHADER, mVertexShader) ||
		!CompileShader(fragShaderPath, GL_FRAGMENT_SHADER, mFragShader))
	{
		return false;
	}

	// Now create a shader program that
	// links together the vertex/frag shaders
	mShaderProgram = glCreateProgram();
	glAttachShader(mShaderProgram, mVertexShader);
	glAttachShader(mShaderProgram, mFragShader);
	glLinkProgram(mShaderProgram);

	// Verify that the program linked successfully
	if (!IsValidProgram())
	{
		return false;
	}
	
	// Gather all uniforms from the shader
	GatherUniforms();
	
	return true;
}

void Shader::Unload()
{
	// Delete the program/shaders
	glDeleteProgram(mShaderProgram);
	glDeleteShader(mVertexShader);
	glDeleteShader(mFragShader);

	mShaderProgram = 0;
	mVertexShader = 0;
	mFragShader = 0;
}

void Shader::SetActive() const
{
	// Set this program as the active one
	glUseProgram(mShaderProgram);
}

void Shader::SetVectorUniform(const char* name, const Vector2& vector) const
{
    // Find the uniform by this name
    GLint loc = glGetUniformLocation(mShaderProgram, name);

    // Send the vector data to the uniform
    glUniform2fv(loc, 1, vector.GetAsFloatPtr());
}

void Shader::SetVectorUniform(const char* name, const Vector3& vector) const
{
	// Find the uniform by this name
	GLint loc = glGetUniformLocation(mShaderProgram, name);

	// Send the vector data to the uniform
	glUniform3fv(loc, 1, vector.GetAsFloatPtr());
}

void Shader::SetVectorUniform(const char* name, const Vector4& vector) const
{
    // Find the uniform by this name
    GLint loc = glGetUniformLocation(mShaderProgram, name);

    // Send the vector data to the uniform
    glUniform4fv(loc, 1, vector.GetAsFloatPtr());
}

void Shader::SetMatrixUniform(const char* name, const Matrix4& matrix) const
{
	// Find the uniform by this name
	GLuint loc = glGetUniformLocation(mShaderProgram, name);

    // Send the matrix data to the uniform
	glUniformMatrix4fv(loc, 1, GL_FALSE, matrix.GetAsFloatPtr());
}

void Shader::SetFloatUniform(const char *name, float value) const
{
    // Find the uniform by this name
    GLuint loc = glGetUniformLocation(mShaderProgram, name);

    // Send the float data to the uniform
    glUniform1f(loc, value);
}

void Shader::SetIntegerUniform(const char *name, int value) const
{
	GLint uTexture = glGetUniformLocation(mShaderProgram, name);
	glUniform1i(uTexture, value);
}

bool Shader::CompileShader(const std::string& fileName, GLenum shaderType, GLuint& outShader)
{
	// Open file
	std::ifstream shaderFile(fileName);
	if (shaderFile.is_open())
	{
		// Read all of the text into a string
		std::stringstream sstream;
		sstream << shaderFile.rdbuf();
		std::string contents = sstream.str();
		const char* contentsChar = contents.c_str();

		// Create a shader of the specified type
		outShader = glCreateShader(shaderType);

        // Set the source characters and try to compile
		glShaderSource(outShader, 1, &(contentsChar), nullptr);
		glCompileShader(outShader);

		if (!IsCompiled(outShader))
		{
			std::cerr << "Failed to compile shader: " << fileName << std::endl;
			return false;
		}
	}
	else
	{
		std::cerr << "Shader file not found: " << fileName << std::endl;
		return false;
	}

	return true;
}

bool Shader::IsCompiled(GLuint shader)
{
	GLint status = 0;

    // Query the compile status
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

	if (status != GL_TRUE)
	{
		char buffer[512];
		memset(buffer, 0, 512);
		glGetShaderInfoLog(shader, 511, nullptr, buffer);
		std::cerr << "Shader compilation error: " << buffer << std::endl;
		return false;
	}

	return true;
}

bool Shader::IsValidProgram() const
{

	GLint status = 0;
	// Query the link status
	glGetProgramiv(mShaderProgram, GL_LINK_STATUS, &status);
	if (status != GL_TRUE)
	{
		char buffer[512];
		memset(buffer, 0, 512);
		glGetProgramInfoLog(mShaderProgram, 511, nullptr, buffer);
		std::cerr << "Shader program linking error: " << buffer << std::endl;
		return false;
	}

	return true;
}

void Shader::GatherUniforms()
{
	// Clear existing uniforms
	mUniforms.clear();
	
	// Get the number of active uniforms
	GLint numUniforms = 0;
	glGetProgramiv(mShaderProgram, GL_ACTIVE_UNIFORMS, &numUniforms);
	
	// Iterate through all active uniforms
	for (GLint i = 0; i < numUniforms; i++)
	{
		GLchar uniformName[256];
		GLsizei length = 0;
		GLint size = 0;
		GLenum type = 0;
		
		glGetActiveUniform(mShaderProgram, i, sizeof(uniformName), &length, &size, &type, uniformName);
		
		// Store the uniform name
		mUniforms.insert(std::string(uniformName));
	}
}

bool Shader::HasUniform(const std::string& name) const
{
	return mUniforms.find(name) != mUniforms.end();
}
