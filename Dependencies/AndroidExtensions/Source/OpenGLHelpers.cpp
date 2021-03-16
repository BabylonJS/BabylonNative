#include <AndroidExtensions/OpenGLHelpers.h>

namespace android::OpenGLHelpers
{
    GLuint LoadShader(GLenum shader_type, const char* shader_source)
    {
        GLuint shader{ glCreateShader(shader_type) };
        if (!shader)
        {
            throw std::runtime_error{"Failed to create shader"};
        }

        glShaderSource(shader, 1, &shader_source, nullptr);
        glCompileShader(shader);
        GLint compileStatus{ GL_FALSE };
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);

        if (compileStatus != GL_TRUE)
        {
            GLint infoLogLength{};

            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
            if (!infoLogLength)
            {
                throw std::runtime_error{"Unknown error compiling shader"};
            }

            std::string infoLog;
            infoLog.resize(static_cast<size_t>(infoLogLength));
            glGetShaderInfoLog(shader, infoLogLength, nullptr, infoLog.data());
            glDeleteShader(shader);
            throw std::runtime_error("Error compiling shader: " + infoLog);
        }

        return shader;
    }

    GLuint CreateShaderProgram(const char* vertShaderSource, const char* fragShaderSource)
    {
        GLuint vertShader{ LoadShader(GL_VERTEX_SHADER, vertShaderSource) };
        GLuint fragShader{ LoadShader(GL_FRAGMENT_SHADER, fragShaderSource) };

        GLuint program{ glCreateProgram() };
        if (!program)
        {
            throw std::runtime_error{"Failed to create shader program"};
        }

        glAttachShader(program, vertShader);
        glAttachShader(program, fragShader);

        glLinkProgram(program);
        GLint linkStatus{ GL_FALSE };
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);

        glDetachShader(program, vertShader);
        glDeleteShader(vertShader);
        glDetachShader(program, fragShader);
        glDeleteShader(fragShader);

        if (linkStatus != GL_TRUE)
        {
            GLint infoLogLength{};
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
            if (!infoLogLength)
            {
                throw std::runtime_error{"Unknown error linking shader program"};
            }

            std::string infoLog;
            infoLog.resize(static_cast<size_t>(infoLogLength));
            glGetProgramInfoLog(program, infoLogLength, nullptr, infoLog.data());
            glDeleteProgram(program);
            throw std::runtime_error("Error linking shader program: " + infoLog);
        }

        return program;
    }
}