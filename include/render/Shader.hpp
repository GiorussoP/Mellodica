#pragma once
#include <GL/glew.h>
#include <string>
#include <unordered_set>
#include "Math.hpp"

class Shader
{
public:
	Shader();
	~Shader();

	// Load shader of the specified name, excluding
	// the .frag/.vert extension
	bool Load(const std::string& name);
	
	// Load shader with separate vertex and fragment shader paths
	bool Load(const std::string& vertShaderPath, const std::string& fragShaderPath);
	
	void Unload();

    // Set this as the active shader program
	void SetActive() const;

    // Sets a Vector / Matrix uniform
    void SetVectorUniform (const char* name, const Vector2& vector) const;
	void SetVectorUniform (const char* name, const Vector3& vector) const;
    void SetVectorUniform (const char* name, const Vector4& vector) const;
	void SetMatrixUniform(const char* name, const Matrix4& matrix) const;
    void SetFloatUniform(const char* name, float value) const;
    void SetIntegerUniform(const char *name, int value) const;
    
    // Check if shader has a specific uniform
    bool HasUniform(const std::string& name) const;

private:
	// Tries to compile the specified shader
	bool CompileShader(const std::string& fileName, GLenum shaderType, GLuint& outShader);

	// Tests whether shader compiled successfully
	bool IsCompiled(GLuint shader);
	// Tests whether vertex/fragment programs link
	bool IsValidProgram() const;
	
	// Gather all active uniforms in the shader program
	void GatherUniforms();

	// Store the shader object IDs
	GLuint mVertexShader;
	GLuint mFragShader;
	GLuint mShaderProgram;
	
	// Set of uniform names that this shader uses
	std::unordered_set<std::string> mUniforms;
};
