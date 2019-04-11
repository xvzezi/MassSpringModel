//
// Created by xvzezi on 2019/4/10.
//

#ifndef MASSSPRINGMODEL_MS_UI_H
#define MASSSPRINGMODEL_MS_UI_H

#include "MSNetwork.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "qx/qxheader.h"


// gui init and render
void gui_init(QX_World& world)
{
    // set up the dear gui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls

    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(world.main_window, true);
    ImGui_ImplOpenGL3_Init(nullptr);
}

void gui_clean(QX_World& world)
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

// play data
bool is_ready = false;
bool auto_play = false;
float time_step = 0.1f;
bool go_next = false;

// param
int rows = 5;
int cols = 10;
float rest_len = 10;
float mass = 1;
float gravity = 10;
float spring_k = 20;
float damping_d = 0;
int mode = 0;

// accessibility
bool show_method1 = true;

MS_Network* net = nullptr;

void load_simulated_objects1(QX_World& world)
{
    QX_Object* prim = world.get_object("prim");
    // generate an origin point
    QX_Object* origin = prim->duplicate("origin");
    origin->scale(0.1, 0.1, 0.1)->enable();
    world.add_object(origin);

    // create
}

void check_param(QX_World* world)
{
    if(rows < 3) rows = 3;
    if(rows > 10) rows = 10;
    if(cols < 3) cols = 3;
    if(cols > 20) cols = 20;
    if(rest_len < 5) rest_len = 5;
    if(mass < 1) mass = 1;
    if(time_step < 0.001f) time_step = 0.001f;
}

void ready(QX_World* world)
{
    // do sth

    //
    is_ready = true;
}

void unready(QX_World* world)
{
    // do sth

    //
    auto_play = false;
    is_ready = false;
}

void matrix_settings(QX_World* world)
{
    static double timer = glfwGetTime();    // seconds
    // the gun


    // the three objects:
    if(auto_play)
    {
//        if(glfwGetTime() - timer > time_step)
//        {
//            go_next = true;
//            timer = glfwGetTime();
//        }
        net->solve_next(mode, glfwGetTime() - timer);
        timer = glfwGetTime();
    }
    if(go_next)
    {
        // recalculate the 3 objects
        std::cout << "Next Framed called" << std::endl;
        net->solve_next(mode, time_step);
        //
        go_next = false;
    }

    // end
}

void gui_render1(QX_World* world)
{
    check_param(world);
    // ready to draw
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
//     ImGui::ShowDemoWindow();
    // gui design
    {
        // titles
        ImGui::Begin("Control Panel");
        ImGui::Text("Mass Spring");
        if(!net)
        {
            ImGui::SliderInt("Rows", &rows, 3, 10);
            ImGui::SliderInt("Columns", &cols, 3, 20);
            ImGui::InputFloat("Rest Length(m)", &rest_len, 0.01f, 1.0f, "%f");
            ImGui::SameLine();
            ImGui::TextDisabled("(?)");
            if (ImGui::IsItemHovered())
            {
                ImGui::BeginTooltip();
                ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
                ImGui::TextUnformatted("The original length of a spring");
                ImGui::PopTextWrapPos();
                ImGui::EndTooltip();
            }
            ImGui::InputFloat("Mass(kg)", &mass, 0.01f, 1.0f, "%f");
            ImGui::InputFloat("Gravity(m/s^2)", &gravity, 0.01f, 1.0f, "%f");
            ImGui::InputFloat("K(N/m)", &spring_k, 0.01f, 1.0f, "%f");
            ImGui::InputFloat("Damping Cof(N*s/m)", &damping_d, 0.01f, 1.0f, "%f");
            if(ImGui::Button("Create"))
            {
                QX_Object* prim = world->get_object("prim");
                net = new MS_Network(rows, cols, rest_len, mass, prim, world);
                net->set_param(gravity, spring_k, damping_d);
                prim->enable();
            }
        }
        else if(!is_ready)
        {
            ImGui::RadioButton("Implicit", &mode, 0);
            ImGui::RadioButton("Explicit", &mode, 1);
            ImGui::RadioButton("Combine", &mode, 2);
            if (ImGui::Button("ready"))
            {
                ready(world);
            }
            if (ImGui::Button("Destroy"))
            {
                delete net;
                net = nullptr;
            }
        }
        else
        {
            ImGui::SliderFloat("Time Step(s)", &time_step, 0.005, 0.5);
            ImGui::RadioButton("Implicit", &mode, 0);
            ImGui::RadioButton("Explicit", &mode, 1);
            ImGui::RadioButton("Combine", &mode, 2);
            ImGui::Checkbox("Auto Play", &auto_play);ImGui::SameLine();
            if (ImGui::Button("Next Frame"))
                go_next = true;
            if (ImGui::Button("reset"))
                unready(world);
        }


        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    }
    // print out

    // Rendering
    ImGui::Render();
    int display_w, display_h;
    glfwMakeContextCurrent(world->main_window);
    glfwGetFramebufferSize(world->main_window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // after work
    matrix_settings(world);
}

void print_out(QX_World* world)
{

}
#endif //MASSSPRINGMODEL_MS_UI_H
