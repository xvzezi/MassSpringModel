#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "qx/qxheader.h"
#include "MSNetwork.h"

#include "Eigen/Dense"


using  namespace std;
using Eigen::MatrixXd;

// load all the data
void load_shaders(QX_World& world)
{
    world.add_shader(new QX_Shader("basic", "../shader/basic.vert", "../shader/basic.frag"));
}

void load_textures(QX_World& world)
{
    world.add_texture(new QX_Texture("tex0", "../resources/container.jpg"));
}
MS_Network* net;
void load_objects(QX_World& world)
{
    // -- // data preparation
    float vertices[] = {
            // location
            0, 0, 5,
            0, 0, -5,
            200, 0, -5,
            200, 0, 5
    };
    float colors[] = {
            // color
            0.8, 0.8, 0.8,
            0.5, 0.5, 0.5,
            0.8, 0.8, 0.8,
            0.5, 0.5, 0.5,
    };
    unsigned int indices[] = {
            0, 1, 3,
            1, 2, 3
    };
    QX_Object* plane = new QX_Object("plane", vertices, colors, nullptr, 4, GL_STATIC_DRAW, indices, 6);
    plane->ref_shader(world.get_shader("basic"));
    plane->render_draw_elements(true);
    world.add_object(plane);

    QX_Object* xline = plane->duplicate("xline");
    xline->set_colors_rgb(0, 0, 1);
    xline->scale(1, 1, 0.1);
    world.add_object(xline);

    QX_Object* yline = xline->duplicate("yline");
    yline->scale(1, 1, 0.02)->rotate(glm::radians(90.0f), 0, 0, 1);
    world.add_object(yline);

    // a cube prim
    float cube_vs[] = {
            1, 0, 1,    1, 0, -1,   -1, 0, -1,      -1, 0, 1,
            1, 2, 1,    1, 2, -1,   -1, 2, -1,      -1, 2, 1
    };
    float cube_cols[] = {
            1, 0, 0,    1, 0, 0,    1, 0, 0,        1, 0, 0,
            1, 0, 0,    1, 0, 0,    1, 0, 0,        1, 0, 0,
    };
    unsigned int cube_i[] = {
            0, 2, 1,    0, 3, 2,    0, 5, 1,    0, 4, 5,    0, 7, 4,    0, 3, 7,
            6, 4, 7,    6, 5, 4,    6, 7, 3,    6, 3, 2,    6, 2, 1,    6, 1, 5
    };
    QX_Object* prim = new QX_Object("prim", cube_vs, cube_cols, nullptr, 8, GL_STATIC_DRAW, cube_i, 36);
    prim->ref_shader(world.get_shader("basic"));
    prim->render_draw_elements(true);
    prim->disable();
    world.add_object(prim);
//    MassPoint* my = new MassPoint("drag", prim, 0, 1, 0, 1);
//    my->enable();
//    world.add_object(my);
    net = new MS_Network(10, 20, prim, &world);
}


int main() {
    QX_World world("CA HW2 SIM Spring Motion", 1600, 800);
    load_shaders(world);
    load_textures(world);
    load_objects(world);
//    load_simulated_objects1(world);
//    gui_init(world);
//    world.render_gui(gui_render1);

    MatrixXd m(2,2);
    m << 3, 2.5, -1, 1.5;
    cout << m << endl;

    while(!world.should_close())
    {
        world.event_loop();
    }
//    print_out(&world);
    // clear the object
//    gui_clean(world);
    return 0;
}