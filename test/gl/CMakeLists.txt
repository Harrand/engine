add_tz_test(NAME tz_headless_triangle_test
        SOURCE_FILES headless_triangle_test.cpp
        SHADER_SOURCES
            test/gl/triangle_test.vertex.tzsl
            test/gl/triangle_test.fragment.tzsl
        )

add_tz_test(NAME tz_mesh_test
        SOURCE_FILES mesh_test.cpp
        )

add_tz_test(NAME tz_renderer_test
        SOURCE_FILES renderer_test.cpp
        SHADER_SOURCES
            test/gl/shader_test.vertex.glsl
            test/gl/shader_test.fragment.glsl
        )

add_tz_test(NAME tz_shader_test
        SOURCE_FILES shader_test.cpp
        SHADER_SOURCES
            test/gl/shader_test.vertex.glsl
            test/gl/shader_test.fragment.glsl
        )
