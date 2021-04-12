//
//  Shader.cpp
//  NewProject - App
//
//  Created by Duke Quarcoo on 06/03/2021.
//

#include "Shader.h"

Shader::Shader(const char* sname, std::shared_ptr<OpenGLContext> openGLContext){
    this->openGLContext = openGLContext;
    std::string source, str;
    loadFile(sname, source, GL_VERTEX_SHADER);
    vs=loadShader(source, GL_VERTEX_SHADER);
    loadFile(sname, source, GL_FRAGMENT_SHADER);
    fs=loadShader(source, GL_FRAGMENT_SHADER);
    // Sets up pipeline of shaders and compiles the program
    shaderProgram->link();
    uniforms.release();
    uniforms.reset (new Uniforms (*openGLContext, *shaderProgram));
}
Shader::~Shader(){
    shaderProgram->release();
    uniforms.reset();
}
GLuint Shader::loadShader(std::string &source, GLuint mode){
    if(shaderProgram == nullptr){
        shaderProgram = std::make_unique<OpenGLShaderProgram> (*openGLContext);
    }
    switch(mode){
        case GL_VERTEX_SHADER:
            // Sets up pipeline of shaders and compiles the program
            shaderProgram->addVertexShader (OpenGLHelpers::translateVertexShaderToV3 (source));
            break;
        case GL_FRAGMENT_SHADER:
            // Sets up pipeline of shaders and compiles the program
            shaderProgram->addFragmentShader (OpenGLHelpers::translateFragmentShaderToV3 (source));
            break;
        default:
            break;
    }
    return 0;
}

void Shader::loadFile(const char* fn, std::string &str, GLuint mode){

    std::unique_ptr<MemoryInputStream> is;
    std::string extension;
    std::string fullName = fn;

    switch(mode){
        case GL_VERTEX_SHADER:
            // Sets up pipeline of shaders and compiles the program
            extension = "vs";
            break;
        case GL_FRAGMENT_SHADER:
            // Sets up pipeline of shaders and compiles the program
            extension = "fs";
            break;
        default:
            break;
    }
#ifdef SHADERS_FROM_BINARY_DATA
    auto s = String(fullName);
    s = s.removeCharacters("-");
    fullName = s.toStdString();
    fullName += "_" + extension;
    const char *CFullName = fullName.c_str();
    int size = int(fullName.size());
    auto shaderResource = BinaryData::getNamedResource(CFullName, size);
    is = std::make_unique<MemoryInputStream> (shaderResource,size, false); 
    str.clear();
    while (! is->isExhausted()) // [3]
           {
               auto line = is->readNextLine();
               str.append(line.toStdString());
           }
#else
    fullName += "." + extension;
    std::string location = shaderPath.toStdString();
    fullName = location + "/" + fullName;
    const char *CFullName = fullName.c_str();
    File f{CFullName};
    StringArray sa;
    f.readLines(sa);
              auto s = sa.joinIntoString("\n").toStdString();
    str.clear();
    str.append(s);
#endif

}

void Shader::use(){
    shaderProgram->use();
}

unsigned int Shader::getProgramId(){
    return shaderProgram->getProgramID();
}


