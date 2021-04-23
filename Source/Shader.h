//
//  Shader.h
//  NewProject - App
//
//  Created by Duke Quarcoo on 06/03/2021.
//

#ifndef Shader_h
#define Shader_h
#include <JuceHeader.h>
#include <stdio.h>
#include <istream>
#define SHADERS_FROM_BINARY_DATA 1
using namespace juce;


//==============================================================================
// This class just manages the uniform values that the fragment shader uses.
struct Uniforms
{
    Uniforms (OpenGLContext& openGLContext, OpenGLShaderProgram& shaderProgram)
    {
        totalScope.reset (createUniform (openGLContext, shaderProgram, "totalScope"));
        offset.reset (createUniform (openGLContext, shaderProgram, "offset"));

        resolution.reset (createUniform (openGLContext, shaderProgram, "resolution"));
        audioSampleData.reset (createUniform (openGLContext, shaderProgram, "audioSampleData"));
        tex.reset (createUniform (openGLContext, shaderProgram, "tex"));
        windowForLog.reset (createUniform(openGLContext, shaderProgram, "windowForLog"));
        reverse.reset (createUniform(openGLContext, shaderProgram, "reverse"));
    }
    ~Uniforms(){
        totalScope.reset();
        offset.reset();
        resolution.reset();
        audioSampleData.reset();
        tex.reset();
        windowForLog.reset();
        reverse.reset();
    }
    std::unique_ptr<OpenGLShaderProgram::Uniform> totalScope, offset, resolution, audioSampleData, tex, windowForLog, reverse;
    
private:
    static OpenGLShaderProgram::Uniform* createUniform (OpenGLContext& openGLContext,
                                                        OpenGLShaderProgram& shaderProgram,
                                                        const char* uniformName)
    {
        if (openGLContext.extensions.glGetUniformLocation (shaderProgram.getProgramID(), uniformName) < 0)
            return nullptr;
        
        return new OpenGLShaderProgram::Uniform (shaderProgram, uniformName);
    }
};

class Shader{
    GLuint vs, fs, program;
    GLuint loadShader(std::string &source, GLuint mode);
    void loadFile(const char* fn, std::string &str, GLuint);
    void removeShader();
    String shaderPath = (SystemStats::getOperatingSystemType() & SystemStats::MacOSX)
		? "/Users/Duke/Documents/GitHub/JUCE/Orbish/resources/shaders"
		: (SystemStats::getOperatingSystemType() & SystemStats::Windows)
		      ? "C:\\Users\\quarc\\source\\repos\\orfeoPreto\\Orbish\\resources\\shaders"
		      : "";
public:
    Shader(const char* sname, std::shared_ptr<OpenGLContext> openGLContext);
    ~Shader();
    void use();
    unsigned int getProgramId();
    std::shared_ptr<OpenGLContext> openGLContext;
    std::unique_ptr<OpenGLShaderProgram> shaderProgram;
    std::unique_ptr<Uniforms> uniforms;

};

#endif /* Shader_h */
