/**
 * Hey! This is the main source file for the engine!
 *
 * This whole project is just my way of testing my graphics programming
 * capabilities. I want to make a 3D engine from the ground up, like I did
 * during my semester with the Graphics Computing course, except this time
 * I would not be using the OpenGL Fixed Function Pipeline, and instead I'd
 * like to use shaders! That way I'd also get familiar with GLSL.
 *
 * I also want to try and build a 3D scene, both the file format and the parsing.
 * Above that, I'd like to be able to parse at least .obj files, if not other
 * formats in the future.
 *
 * We'll see how this goes.
 */

#include <iostream>
#include <string>
#include <sstream>

#ifdef __APPLE__
#include <GLUT/glut.h>

#else
#include <GL/glew.h>
#include <GL/glut.h>

#endif

#ifdef _WIN32
#include <windows.h>
typedef BOOL(WINAPI *PFNWGLSWAPINTERVALEXTPROC)(int);

#elif defined(__linux__)
#include <GL/glx.h>
typedef int (*PFNGLXSWAPINTERVALSGIPROC)(int);

#elif defined(__APPLE__)
#include <OpenGL/OpenGL.h>

#endif

#include "engine/shader_store.hpp"
#include "engine/object.hpp"
#include "engine/camera.hpp"

#include "math/vec3.hpp"
#include "math/mat4.hpp"

camera *cam;
float cam_fov = 90, cam_near = 0.1f, cam_far = 1000.0f;

bool key_states[256];

mat4 projection_matrix;

int prev_time = 0;

int win_width = 200;
int win_height = 200;
int init_win_pos_x = 100;
int init_win_pos_y = 100;

object *test_obj = nullptr;

bool draw_axis = true;

void disable_vsync()
{
#ifdef _WIN32
    PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT =
        (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");

    if (wglSwapIntervalEXT)
    {
        wglSwapIntervalEXT(0);
    }
#elif defined(__linux__)
    Display *dpy = glXGetCurrentDisplay();
    GLXDrawable drawable = glXGetCurrentDrawable();

    if (dpy && drawable)
    {
        PFNGLXSWAPINTERVALSGIPROC glXSwapIntervalSGI =
            (PFNGLXSWAPINTERVALSGIPROC)glXGetProcAddress((const GLubyte *)"glXSwapIntervalSGI");

        if (glXSwapIntervalSGI)
        {
            glXSwapIntervalSGI(0);
        }
    }
#elif defined(__APPLE__)
    CGLContextObj ctx = CGLGetCurrentContext();
    if (ctx)
    {
        GLint sync = 0;
        CGLSetParameter(ctx, kCGLCPSwapInterval, &sync);
    }
#endif
}

void change_window_size(int w, int h)
{
    if (h == 0)
        h = 1;

    cam->update_window_size(w, h);

    float ratio = w * 1.0 / h;

    projection_matrix = mat4::Projection(cam_fov, ratio, cam_near, cam_far);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMultMatrixf(projection_matrix);
    glMatrixMode(GL_MODELVIEW);

    glViewport(0, 0, w, h);
}

void render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
    glMultMatrixf(cam->get_view_matrix());

    if (draw_axis)
    {
        glBegin(GL_LINES);
        // x axis
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex3f(-100.0f, 0.0f, 0.0f);
        glVertex3f(100.0f, 0.0f, 0.0f);

        // y axis
        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex3f(0.0f, -100.0f, 0.0f);
        glVertex3f(0.0f, 100.0f, 0.0f);

        // z axis
        glColor3f(0.0f, 0.0f, 1.0f);
        glVertex3f(0.0f, 0.0f, -100.0f);
        glVertex3f(0.0f, 0.0f, 100.0f);
        glEnd();
    }

    test_obj->render(projection_matrix, cam->get_view_matrix());

    glutSwapBuffers();
    return;
}

void idle()
{
    int current_time = glutGet(GLUT_ELAPSED_TIME);
    int delta_time_ms = current_time - prev_time;
    prev_time = current_time;

    /* if (update_groups)
    {
        cfg_obj->update_groups(delta_time_ms);
        std::vector<vector3> l_pos = cfg_obj->query_group_postitions();
        cam->update_lock_positions(l_pos);
    } */

    cam->play_animations(delta_time_ms);
    cam->update_camera_position(key_states, delta_time_ms);
    glutPostRedisplay();
}

void processKeyPress(unsigned char c, int mouse_x, int mouse_y)
{
    static float zoom_delta = 1.0f;
    std::stringstream ss;
    switch (c)
    {
        /* case '1':
            draw_axis = !draw_axis;
            break;

        case '2':
            wire_mode = !wire_mode;
            break;

        case '3':
            draw_path = !draw_path;
            break;

        case '4':
            update_groups = !update_groups;
            break;

        case '5':
            draw_bounding_spheres = !draw_bounding_spheres;
            break;

        case '6':
            draw_frustum = !draw_frustum;
            break;

        case '7':
            frustum_cull = !frustum_cull;
            break;

        case '8':
            update_frustum_on_free_cam = !update_frustum_on_free_cam;
            break; */

    case 'f':
    case 'F':
        cam->switch_camera_mode();
        break;

    case 'p':
    case 'P':
        printer::print_tutorial(glGetString(GL_VENDOR), glGetString(GL_RENDERER), glGetString(GL_VERSION));
        break;

    case 'r':
    case 'R':
        cam->reset_camera();
        break;

    case 'c':
    case 'C':
        cam->cycle_target();
        break;

    case 'z':
    case 'Z':
        cam->add_to_target_radius(-zoom_delta);
        break;

    case 'x':
    case 'X':
        cam->add_to_target_radius(zoom_delta);
        break;

    case '+':
        zoom_delta += 0.1f;
        ss << "Zoom delta is now " << zoom_delta << ".";
        printer::print_info(ss.str());
        break;

    case '-':
        zoom_delta -= 0.1f;
        ss << "Zoom delta is now " << zoom_delta << ".";
        printer::print_info(ss.str());
        break;

    case 27:
        // this is esc!
        exit(0);
        break;

    default:
        // any key that has no special effect will instead be registered as being pressed down
        key_states[c] = true;
        break;
    }
}

void processKeyRelease(unsigned char c, int mouse_x, int mouse_y)
{
    // sets key state to being up (obvious, eh?)
    key_states[c] = false;
}

void processMouse(int x, int y)
{
    // x and y are the offset of the mouse from the previous frame
    cam->update_camera_direction(x, y);
}

int init(int &argc, char **argv)
{
    printer::setup();

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(init_win_pos_x, init_win_pos_y);
    glutInitWindowSize(win_width, win_height);
    glutCreateWindow("test");

    glutDisplayFunc(render);
    glutIdleFunc(idle);
    glutReshapeFunc(change_window_size);

    glutKeyboardFunc(processKeyPress);
    glutKeyboardUpFunc(processKeyRelease);
    glutPassiveMotionFunc(processMouse);

    disable_vsync();

    glFrontFace(GL_CCW);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

#ifndef __APPLE__
    glewInit();
#endif

    shader_store *instance = shader_store::get_instance();

    try
    {
        instance->load("./config/shaders.xml");
    }
    catch (const std::exception &e)
    {
        return 1;
    }

    return 0;
}

void test_function()
{
    test_obj = new object(std::string("C:\\Users\\azeve\\Desktop\\weird.obj"), "basic");
}

int main(int argc, char **argv)
{
    /**
     * For now, I will use hardcoded values.
     *
     *   * CHANGE LATER TO ACCEPT A SCENE FILE AS AN ENTRY POINT! *
     *   * MAYBE A CONFIGURATION FILE INSTEAD OF A SCENE, WE'LL SEE *
     *
     *  (whats a scene if not a config file, dummy)
     */
    if (init(argc, argv))
    {
        printer::print_exception("init failed!");
        return 1;
    }
    printer::print_info("init is ago!");

    try
    {
        test_function();
    }
    catch (FailedToParseException &e)
    {
        printer::print_exception(e.what(), e.caller);
        return 1;
    }
    catch (std::exception &e)
    {
        printer::print_exception(e.what());
        return 1;
    }

    printer::print_info("Test function is ago!");

    /*     glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0); */

    float ratio = win_width * 1.0 / win_height;
    projection_matrix = mat4::Projection(cam_fov, ratio, cam_near, cam_far);
    cam = new camera(vec3<float>(1.0f, 1.0f, 1.0f), vec3<float>(0.0f, 0.0f, 0.0f), vec3<float>(0.0f, 1.0f, 0.0f));

    glutMainLoop();

    return 0;
}