#define _USE_MATH_DEFINES

// header
#include "timer.hpp"

// stdlib
#include <cmath>
#include <iostream>
#include <string>

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_GLCOREARB

const char *VERTEX_SHADER = ""
                            "#version 410 core\n"
                            "in vec4 in_Position;\n"
                            "out vec2 texCoords;\n"
                            "void main(void) {\n"
                            "    gl_Position = vec4(in_Position.xy, 0, 1);\n"
                            "    texCoords = in_Position.zw;\n"
                            "}\n";

const char *FRAGMENT_SHADER = ""
                              "#version 410 core\n"
                              "precision highp float;\n"
                              "uniform sampler2D tex;\n"
                              "uniform vec4 color;\n"
                              "in vec2 texCoords;\n"
                              "out vec4 fragColor;\n"
                              "void main(void) {\n"
                              "    fragColor = vec4(1, 1, 1, texture(tex, texCoords).r) * color;\n"
                              "}\n";

bool Timer::init()
{
    seconds = 0;
    minutes = 0;
    fprintf(stderr, "Vertex Shader - \n %s", FRAGMENT_SHADER);
    return true;
}

// release all graphics resources
void Timer::destroy()
{
    glDeleteBuffers(1, &mesh.vbo);
    glDeleteBuffers(1, &m_instance_vbo);

    effect.release();
}

void Timer::update(float ms)
{
    // TODO Update timer
    if ((int)glfwGetTime() == 0)
    {
        seconds = 0;
        minutes = 0;
    }

    int secs_in_min = 60;
    seconds = (int)glfwGetTime();
    minutes = seconds / secs_in_min;
    seconds = seconds % secs_in_min;

    // fprintf(stderr, " timer - %02i:%02i \n",minutes, seconds);
}

void Timer::draw(const mat3 &projection)
{
    GLuint texture{0}, sampler{0};
    GLuint vbo{0}, vao{0};
    GLuint vs{0}, fs{0}, program{0};
    FT_Library ft_lib{nullptr};
    FT_Face face{nullptr};

    auto cleanup = [&]() {
        FT_Done_Face(face);
        FT_Done_FreeType(ft_lib);
        glDeleteTextures(1, &texture);
        glDeleteSamplers(1, &sampler);
        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);
        glDeleteShader(vs);
        glDeleteShader(fs);
        glDeleteProgram(program);
    };

    // Initialize and load our freetype face
    if (FT_Init_FreeType(&ft_lib) != 0)
    {
        std::cerr << "Couldn't initialize FreeType library\n";
        // cleanup();
        // return 1;
    }

    if (FT_New_Face(ft_lib, textures_path("fonts/arcade.TTF"), 0, &face) != 0)
    {
        std::cerr << "Unable to load FreeSans.ttf\n";
        // cleanup();
        // return 1;
    }

    const int WIDTH = 1200;
    const int HEIGHT = 800;
    const double SCALEX = 2.0 / WIDTH;
    const double SCALEY = 2.0 / HEIGHT;

    // auto window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL Text Rendering", nullptr, nullptr);
    // glfwMakeContextCurrent(window);
    // glViewport(0, 0, WIDTH, HEIGHT);

    // Initialize our texture and VBOs
    glGenBuffers(1, &vbo);
    glGenVertexArrays(1, &vao);
    glGenTextures(1, &texture);
    glGenSamplers(1, &sampler);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Initialize shader
    vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &VERTEX_SHADER, 0);
    glCompileShader(vs);

    fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &FRAGMENT_SHADER, 0);
    glCompileShader(fs);

    program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    // Set some initialize GL state
    glEnable(GL_BLEND);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glClearColor(0, 0, 0.4, 0);

    // Get shader uniforms
    glUseProgram(program);
    glBindAttribLocation(program, 0, "in_Position");
    GLuint texUniform = glGetUniformLocation(program, "tex");
    GLuint colorUniform = glGetUniformLocation(program, "color");

    // while(glfwWindowShouldClose(window) != GL_TRUE) {
    // glfwMakeContextCurrent(window);
    // glClear(GL_COLOR_BUFFER_BIT);

    // Bind stuff
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindSampler(0, sampler);
    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glUseProgram(program);
    glUniform4f(colorUniform, 1, 1, 1, 0.7);
    glUniform1i(texUniform, 0);

    FT_Set_Pixel_Sizes(face, 0, 50);
    std::string secs;
    if (seconds < 10)
        secs = "0" + std::to_string(seconds);
    else if (seconds >= 10)
        secs = std::to_string(seconds);
    std::string mins;
    if (minutes < 10)
        mins = "0" + std::to_string(minutes);
    else if (seconds >= 10)
        mins = std::to_string(minutes);

    render_text(mins + ":" + secs, face, -0.2, 0.85, SCALEX, SCALEY);

    // glfwSwapBuffers(window);
    glfwPollEvents();
    // }

    cleanup();
}

void Timer::render_text(const std::string &str, FT_Face face, float x, float y, float sx, float sy)
{
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    const FT_GlyphSlot glyph = face->glyph;

    for (auto c : str)
    {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER) != 0)
            continue;

        glTexImage2D(GL_TEXTURE_2D, 0, GL_R8,
                     glyph->bitmap.width, glyph->bitmap.rows,
                     0, GL_RED, GL_UNSIGNED_BYTE, glyph->bitmap.buffer);

        const float vx = x + glyph->bitmap_left * sx;
        const float vy = y + glyph->bitmap_top * sy;
        const float w = glyph->bitmap.width * sx;
        const float h = glyph->bitmap.rows * sy;

        struct
        {
            float x, y, s, t;
        } data[6] = {
            {vx, vy, 0, 0},
            {vx, vy - h, 0, 1},
            {vx + w, vy, 1, 0},
            {vx + w, vy, 1, 0},
            {vx, vy - h, 0, 1},
            {vx + w, vy - h, 1, 1}};

        glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), data, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        x += (glyph->advance.x >> 6) * sx;
        y += (glyph->advance.y >> 6) * sy;
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
}
