#ifndef TOPAZ_GL_IMPL_OGL_SHADER_HPP
#define TOPAZ_GL_IMPL_OGL_SHADER_HPP
#if TZ_OGL
#include "gl/api/shader.hpp"
#include "glad/glad.h"

namespace tz::gl
{
    class ShaderBuilderOGL : public IShaderBuilder
    {
    public:
        ShaderBuilderOGL() = default;
        virtual void set_shader_file(ShaderType type, std::filesystem::path shader_file) final;
        virtual void set_shader_source(ShaderType type, std::string source_code) final;
        virtual std::string_view get_shader_source(ShaderType type) const final;
        virtual bool has_shader(ShaderType type) const final;
    private:
        std::string vertex_shader_source;
        std::string fragment_shader_source;
    };

    class ShaderOGL
    {
    public:
        ShaderOGL(ShaderBuilderOGL builder);
        ShaderOGL(const ShaderOGL& copy) = delete;
        ShaderOGL(ShaderOGL&& move);
        ~ShaderOGL();

        ShaderOGL& operator=(const ShaderOGL& rhs) = delete;
        ShaderOGL& operator=(ShaderOGL&& rhs);
    private:
        static std::string get_info_log(GLuint shader);

        GLuint vertex_shader;
        GLuint fragment_shader;
    };
}

#endif // TZ_OGL
#endif // TOPAZ_GL_IMPL_OGL_SHADER_HPP