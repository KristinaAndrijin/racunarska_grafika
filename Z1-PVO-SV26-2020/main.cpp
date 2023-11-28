// Autor: Nedeljko Tesanovic
// Opis: V3 Z1

#define _CRT_SECURE_NO_WARNINGS

#define CRES 30 // Circle Resolution = Rezolucija kruga


#include <iostream>
#include <fstream>
#include <sstream>

#include <GL/glew.h>   
#include <GLFW/glfw3.h>

#include <random>
#include <vector>

#include <chrono>
#include <ctime>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

unsigned int compileShader(GLenum type, const char* source);
unsigned int createShader(const char* vsSource, const char* fsSource);
static unsigned loadImageToTexture(const char* filePath); //Ucitavanje teksture, izdvojeno u funkciju
std::vector<float> generateHelicopters();
int generateRandomTime();

const int n = 20;


int main(void)
{


    if (!glfwInit())
    {
        std::cout << "GLFW Biblioteka se nije ucitala! :(\n";
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window;
    unsigned int wWidth = 1700;
    unsigned int wHeight = 900;
    const char wTitle[] = "Kristina Andrijin [SV26/2020]";
    window = glfwCreateWindow(wWidth, wHeight, wTitle, NULL, NULL);


    if (window == NULL)
    {
        std::cout << "Prozor nije napravljen! :(\n";
        glfwTerminate();
        return 2;
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
    {
        std::cout << "GLEW nije mogao da se ucita! :'(\n";
        return 3;
    }

    unsigned int mapShader = createShader("map_shaders/basic_map.vert", "map_shaders/basic_map.frag");
    unsigned int greenShader = createShader("green_shield_shaders/green_shield.vert", "green_shield_shaders/green_shield.frag");
    unsigned int diodShader = createShader("diod_shaders/basic.vert", "diod_shaders/basic.frag");
    unsigned int circleShader = createShader("circle_shaders/circle.vert", "circle_shaders/circle.frag");
    unsigned int hideMapShader = createShader("map_shaders/hide_map.vert", "map_shaders/hide_map.frag");
    unsigned int indicatorShader = createShader("indicator_shaders/indicator.vert", "indicator_shaders/indicator.frag");
    unsigned int helicopterShader = createShader("point_shaders/helicopter.vert", "point_shaders/helicopter.frag");
    unsigned int dronShader = createShader("point_shaders/dron.vert", "point_shaders/dron.frag");



    //float green_shield_vertices[] =
    //{
    //     1.0, 1.0,
    //    -1.0, 1.0,
    //   1.0, -1.0,
    //    -1.0, -1.0,
    //};

    float green_shield_vertices[] =
    {
        -0.7, -0.9,
        0.9, -0.9,
       -0.7, 0.7,
        0.9, 0.7,
    };

 /*   float green_shield_vertices[] =
    {
         1.0, 1.0,
        -1.0, 1.0,
       1.0, -1.0,
        -1.0, -1.0,
    };*/

    unsigned int green_shield_stride = 2 * sizeof(float);

    unsigned int VAO[n];
    glGenVertexArrays(n, VAO);

    unsigned int VBO[n];
    glGenBuffers(n, VBO);

    glBindVertexArray(VAO[0]);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(green_shield_vertices), green_shield_vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, green_shield_stride, (void*)0);
    glEnableVertexAttribArray(0);

    float map_vertices[] = {
       -0.7, -0.9,     0.0, 1.0,       //1.0, 1.0, 0.0, 0.5,
        0.9, -0.9,     1.0, 1.0,       //1.0, 1.0, 0.0, 0.5,
       -0.7,   0.7,      0.0, 0.0,       //1.0, 1.0, 0.0, 0.5,
        0.9, 0.7,      1.0, 0.0,       //1.0, 1.0, 0.0, 0.5,
    };

    //unsigned int green_shield_stride = (2 + 2) * sizeof(float);

    unsigned int map_stride = (2 + 2) * sizeof(float);

    unsigned int map_indices[] =
    {
        0, 1, 2,
        1, 2, 3,
    };


    glBindVertexArray(VAO[1]);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(map_vertices), map_vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, map_stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, map_stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(map_indices), map_indices, GL_STATIC_DRAW);

    float vertices[] =
    {
         -0.97f,  0.2f,
         -0.85f,  0.2f,
         -0.91f, 0.6f
    };

    glBindVertexArray(VAO[2]);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, green_shield_stride, (void*)0);
    glEnableVertexAttribArray(0);

    double aspectRatio = double(wWidth) / double(wHeight);
    double scaleX = 1.0;
    double scaleY = aspectRatio;

    float circle[CRES * 2 + 4]; // +4 je za x i y koordinate centra kruga, i za x i y od nultog ugla
    float r = 0.05; //poluprecnik

    circle[0] = -0.2; //Centar X0
    circle[1] = -0.6; //Centar Y0
    int i;
    for (i = 0; i <= CRES; i++)
    {
        double angle = (i * 360.0 / CRES) * (3.141592 / 180.0);
        double x = circle[0] + r * cos(angle) * scaleX;
        double y = circle[1] + r * sin(angle) * scaleY;

        circle[2 + 2 * i] = x;
        circle[2 + 2 * i + 1] = y;

        //circle[2 + 2 * i] = -0.2 + r * cos((3.141592 / 180) * (i * 360 / CRES)); //Xi
        //circle[2 + 2 * i + 1] = -0.6 + r * sin((3.141592 / 180) * (i * 360 / CRES)); //Yi
    }
    //Crtali smo od "nultog" ugla ka jednom pravcu, sto nam ostavlja prazno mjesto od poslednjeg tjemena kruznice do prvog,
    //pa da bi ga zatvorili, koristili smo <= umjesto <, sto nam dodaje tjeme (cos(0), sin(0))

    glBindVertexArray(VAO[3]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(circle), circle, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    float hide_map_vertices[] =
    {
         -0.7, -0.9,
        0.9, -0.9,
       -0.7, 0.7,
        0.9, 0.7
    };

    glBindVertexArray(VAO[4]);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[4]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(hide_map_vertices), hide_map_vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, green_shield_stride, (void*)0);
    glEnableVertexAttribArray(0);

    float indicator_vertices[] = {
        -0.7, 0.85,       1.0, 0.0, 0.0, 0.5,
        -0.7, 0.95,       1.0, 0.0, 0.0, 0.5,
        0.0, 0.85,       1.0, 0.0, 0.0, 0.5,
        0.0, 0.95,       1.0, 0.0, 0.0, 0.5,
    };


    unsigned int indicator_stride = 6 * sizeof(float);

    glBindVertexArray(VAO[5]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[5]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(indicator_vertices), indicator_vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, indicator_stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, indicator_stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    float hide_indicator_vertices[] = {
        -0.7, 0.75,       1.0, 0.0, 0.0, 0.5,
        -0.7, 0.95,       1.0, 0.0, 0.0, 0.5,
        -1.0, 0.75,       1.0, 0.0, 0.0, 0.5,
        -1.0, 0.95,       1.0, 0.0, 0.0, 0.5,
    };

    glBindVertexArray(VAO[6]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[6]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(hide_indicator_vertices), hide_indicator_vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, indicator_stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, indicator_stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    float indicator_divisions[] = {

        -0.6f, 0.85f,           0.7, 0.0, 0.0, 1.0,
        -0.6f, 0.95f,           0.7, 0.0, 0.0, 1.0,

        -0.5f, 0.95f,           0.7, 0.0, 0.0, 1.0,
        -0.5f, 0.85f,           0.7, 0.0, 0.0, 1.0,
                                
        -0.4f, 0.85f,           0.7, 0.0, 0.0, 1.0,
        -0.4f, 0.95f,           0.7, 0.0, 0.0, 1.0,
                                
        -0.3f, 0.95f,           0.7, 0.0, 0.0, 1.0,
        -0.3f, 0.85f,           0.7, 0.0, 0.0, 1.0,
                                
        -0.2f, 0.95f,           0.7, 0.0, 0.0, 1.0,
        -0.2f, 0.85f,           0.7, 0.0, 0.0, 1.0,

        -0.1f, 0.95f,           0.7, 0.0, 0.0, 1.0,
        -0.1f, 0.85f,           0.7, 0.0, 0.0, 1.0

    };

    glBindVertexArray(VAO[7]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[7]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(indicator_divisions), indicator_divisions, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, (2 + 4) * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, (2 + 4) * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    float çenter[] = {
        0.1, 0.0
    };

    float helicopter_point_vertices[] = {
        0.0, 0.0
    };

    unsigned int helicopter_point_stride = 2 * sizeof(float);

    std::vector<float> helicopters = generateHelicopters();

    float helicopters_currents[10] = { 0,0,0,0,0,0,0,0,0,0 };

    for (int i = 0; i < 5; i++) {
        helicopter_point_vertices[0] = helicopters[2 * i];
        helicopters_currents[2*i] = helicopters[2 * i];
        helicopter_point_vertices[1] = helicopters[2 * i + 1];
        helicopters_currents[2*i+1] = helicopters[2 * i + 1];
        glBindVertexArray(VAO[8+i]);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[8+i]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(helicopter_point_vertices), helicopter_point_vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, helicopter_point_stride, (void*)0);
        glEnableVertexAttribArray(0);
    }

    float dron_point_vertices[] = {
        -0.2, -0.6
    };

    unsigned int dron_point_stride = 2 * sizeof(float);


    for (int i = 0; i < 7; i++) {
        glBindVertexArray(VAO[13 + i]);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[13 + i]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(dron_point_vertices), dron_point_vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, dron_point_stride, (void*)0);
        glEnableVertexAttribArray(0);
    }


    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //Tekstura
    unsigned checkerTexture = loadImageToTexture("res/mapa.png"); //Ucitavamo teksturu
    glBindTexture(GL_TEXTURE_2D, checkerTexture); //Podesavamo teksturu
    glGenerateMipmap(GL_TEXTURE_2D); //Generisemo mipmape 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);   //GL_NEAREST, GL_LINEAR
    glBindTexture(GL_TEXTURE_2D, 0);


    unsigned uTexLoc = glGetUniformLocation(mapShader, "uTex");
    glUniform1i(uTexLoc, 0);

    glUseProgram(diodShader);

    glUniform4f(glGetUniformLocation(diodShader, "triangleColor"), 0.2, 0.3, 0.3, 1.0);

    bool mapa = false;
    bool start = true;

    double indicator_x_position = 0.0;
    int indicator_pressed = 0;
    bool canShoot = true;

    glUseProgram(indicatorShader);
    glUniform2f(glGetUniformLocation(indicatorShader, "uPos"), indicator_x_position, 0);

    glClearColor(0.7, 0.7, 0.7, 1.0);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glPointSize(15);

    unsigned int uAh = glGetUniformLocation(helicopterShader, "uA");
    unsigned int uRh = glGetUniformLocation(helicopterShader, "uR");
    unsigned int uPosLocH = glGetUniformLocation(helicopterShader, "uPos");
    unsigned int uAd = glGetUniformLocation(dronShader, "uA");
    unsigned int uPosLocD = glGetUniformLocation(dronShader, "uPos");

    bool helicopters_moving[] = { true, false, false, false, false };
    bool helicopters_dead[] = { false, false, false, false, false };
    float helicopter_changes[10] = { 0,0,0,0,0,0,0,0,0,0 };
    float x = 0.0;
    float y = 0.0;
    float rnX = 0.0;
    float rnY = 0.0;
    bool started_moving = true;

    float dron_changes[2] = { 0.0, 0.0 };
    float dron_currents[2] = { -0.2, -0.6 };

    std::chrono::system_clock::time_point starting_point = std::chrono::system_clock::now();
    std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
    std::chrono::duration<double> duration = end - starting_point;

    float helicopter_acc = 0.00035;
    float dron_acc = 0.00035*3;

    double dur = duration.count();

    int p = 0;

    int dron_counter = 0;

    bool is_dron_launched = false;
    bool space_pressed = false;
    bool is_dron_alive = false;
    bool should_kill_dron = false;

    int s = generateRandomTime();



    while (!glfwWindowShouldClose(window))
    {

        glfwPollEvents();


        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GL_TRUE);

        /*if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
            glClearColor(1.0, 1.0, 1.0, 1.0);
        else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            glClearColor(0.25, 0.25, 0.25, 1.0);*/


        glClear(GL_COLOR_BUFFER_BIT);

        if (start || mapa || glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
            mapa = true;
            start = false;
            //std::cout << "mapa"
            glUseProgram(mapShader);
            glBindVertexArray(VAO[1]);

            glActiveTexture(GL_TEXTURE0); //tekstura koja se bind-uje nakon ovoga ce se koristiti sa SAMPLER2D uniformom u sejderu koja odgovara njenom indeksu
            glBindTexture(GL_TEXTURE_2D, checkerTexture);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
           // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(0 * sizeof(unsigned int)));
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        /*glUseProgram(hideMapShader);
        glBindVertexArray(VAO[4]);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawArrays(GL_TRIANGLES, 1, 3);*/

        if ((glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS && is_dron_alive && is_dron_launched) || should_kill_dron) {
            dron_counter++;
            canShoot = true;
            glUseProgram(diodShader);
            glUniform4f(glGetUniformLocation(diodShader, "triangleColor"), 0.2, 0.3, 0.3, 1.0);
            is_dron_alive = false;
            is_dron_launched = false;
            dron_changes[0] = 0.0;
            dron_changes[1] = 0.0;
            dron_currents[0] = -0.2;
            dron_currents[1] = -0.6;
            should_kill_dron = false;
        }


        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glUseProgram(greenShader);
        glBindVertexArray(VAO[0]);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawArrays(GL_TRIANGLES, 1, 3);
        glDisable(GL_BLEND);

        if (!mapa || glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
            mapa = false;

            //std::cout << "hej";
            glUseProgram(hideMapShader);
            glBindVertexArray(VAO[4]);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            glDrawArrays(GL_TRIANGLES, 1, 3);
        }


        glUseProgram(diodShader);
        glBindVertexArray(VAO[2]);    
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glUseProgram(circleShader);
        glBindVertexArray(VAO[3]);
        glDrawArrays(GL_TRIANGLE_FAN, 0, sizeof(circle) / (2 * sizeof(float)));

        glUseProgram(indicatorShader);
        glBindVertexArray(VAO[5]);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, sizeof(indicator_vertices) / indicator_stride);


        // sakrij indikator
        glUseProgram(hideMapShader);
        glBindVertexArray(VAO[6]);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, sizeof(indicator_vertices) / indicator_stride);

        space_pressed = false;

        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && canShoot) {
            //std::cout << "cao" << std::endl;
            indicator_pressed++;
            if (indicator_pressed > 7) {
                std::cout << "ne moze" << std::endl;
                canShoot = false;
            }
            else {
                glUseProgram(diodShader);
                glUniform4f(glGetUniformLocation(diodShader, "triangleColor"), 0.0, 0.0, 1.0, 1.0);
                indicator_x_position = -0.1 * indicator_pressed;
                glUseProgram(indicatorShader);
                glUniform2f(glGetUniformLocation(indicatorShader, "uPos"), indicator_x_position, 0);
                canShoot = false;


                glUseProgram(dronShader);
                glBindVertexArray(VAO[13 + dron_counter]);
                glUniform2f(glGetUniformLocation(dronShader, "uPos"), dron_currents[0], dron_currents[1]);
                glUniform1f(glGetUniformLocation(dronShader, "uA"), 0);
                glDrawArrays(GL_POINTS, 0, sizeof(dron_point_vertices) / dron_point_stride);
                is_dron_launched = true;
                space_pressed = true;
                is_dron_alive = true;
                should_kill_dron = false;

                //glUniform1f(glGetUniformLocation(helicopterShader, "uA"), abs(sin(10 * (0.01 + abs(changes[2 * i]) + abs(changes[2 * i + 1])) * pulse)));
            }
        }

        if (is_dron_alive) {

            if (is_dron_launched && !space_pressed) {
                if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
                {
                    if (dron_currents[1] <= 0.68) {
                        dron_currents[1] += dron_acc;
                        dron_changes[1] += dron_acc;
                    }
                    else {
                        should_kill_dron = true;
                      /*  is_dron_alive = false;
                        is_dron_launched = false;*/
                    }
                }
                if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
                {
                    if (dron_currents[1] > -0.88) {
                        dron_currents[1] -= dron_acc;
                        dron_changes[1] -= dron_acc;
                    }
                    else {
                        should_kill_dron = true;
                     /*   is_dron_alive = false;
                        is_dron_launched = false;*/
                    }
                }
                if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
                {
                    if (dron_currents[0] <= 0.89) {
                        dron_currents[0] += dron_acc;
                        dron_changes[0] += dron_acc;
                    }
                    else {
                        should_kill_dron = true;
                     /*   is_dron_alive = false;
                        is_dron_launched = false;*/
                    }
                }
                if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
                {
                    if (dron_currents[0] >= -0.69) {
                        dron_currents[0] -= dron_acc;
                        dron_changes[0] -= dron_acc;
                    }
                    else {
                        should_kill_dron = true;
               /*         is_dron_alive = false;
                        is_dron_launched = false;*/
                    }
                }

                glUseProgram(dronShader);
                glBindVertexArray(VAO[13 + dron_counter]);
                glUniform2f(glGetUniformLocation(dronShader, "uPos"), dron_currents[0], dron_currents[1]);
                glUniform1f(glGetUniformLocation(dronShader, "uA"), abs(sin(10 * (0.01 + abs(dron_changes[0]) + abs(dron_changes[1])) * 2)));
                glDrawArrays(GL_POINTS, 0, sizeof(dron_point_vertices) / dron_point_stride);
            }
        }

        //for (int i = 0; i < 7; i++) {
        //    glUseProgram(dronShader);
        //    glBindVertexArray(VAO[13 + i]);
        //    /*glUniform2f(uPosLoc, changes[2 * i], changes[2 * i + 1]);
        //    glUniform1f(glGetUniformLocation(helicopterShader, "uA"), 0);*/
        //    glDrawArrays(GL_POINTS, 0, sizeof(dron_point_vertices) / dron_point_stride);
        //}

        glUseProgram(hideMapShader);
        glLineWidth(2.5);//debljina linije
        glBindVertexArray(VAO[7]);
        glDrawArrays(GL_LINES, 0, 12);


        for (int i = 0; i < 5; i++) {

            glBindVertexArray(VAO[8+i]);

            if (helicopters_moving[i] && !helicopters_dead[i]) {
                if (dur < s) {
                    s = generateRandomTime();
                    end = std::chrono::system_clock::now();
                    duration = end - starting_point;
                    dur = duration.count();
                    glUseProgram(helicopterShader);
                    glUniform2f(uPosLocH, helicopter_changes[2 * i], helicopter_changes[2 * i + 1]);
                    if (helicopters_currents[2 * i] >= 0.89 || helicopters_currents[2 * i] <= -0.69 || helicopters_currents[2 * i + 1] >= 0.69 || helicopters_currents[2 * i + 1] <= -0.89) {
                        glUniform1f(glGetUniformLocation(helicopterShader, "uA"), 0.7);
                        glUniform1f(glGetUniformLocation(helicopterShader, "uR"), 0.7);
                    }
                    else {
                        glUniform1f(glGetUniformLocation(helicopterShader, "uA"), 0);
                        glUniform1f(glGetUniformLocation(helicopterShader, "uR"), 1.0);
                    }
                    glDrawArrays(GL_POINTS, 0, sizeof(helicopter_point_vertices) / helicopter_point_stride);
                    continue;
                }
                //std::cout << i << std::endl;
                if (started_moving) {
                    //std::cout << "reset pozicija" << std::endl;
                    rnX = helicopters[2 * i];
                    rnY = helicopters[2 * i + 1];
                    started_moving = false;
                }
                if (rnX < 0.11) {
                    x += helicopter_acc;
                    rnX += helicopter_acc;
                }
                if (rnY < 0.01) {
                    y += helicopter_acc;
                    rnY += helicopter_acc;
                }

                if (rnX > 0.11) {
                    x -= helicopter_acc;
                    rnX -= helicopter_acc;
                }
                if (rnY > 0.01) {
                    y -= helicopter_acc;
                    rnY -= helicopter_acc;
                }
                helicopter_changes[2 * i] = x;
                helicopter_changes[2 * i + 1] = y;
                //glBindVertexArray(VAO[8 + i]);
                glUseProgram(helicopterShader);
                glUniform2f(uPosLocH, x, y);
                std::chrono::system_clock::time_point pulse_point = std::chrono::system_clock::now();
                std::chrono::duration<double> durationP = pulse_point - starting_point;

                double pulse = durationP.count();
                if (helicopters_currents[2 * i] >= 0.89 || helicopters_currents[2 * i] <= -0.69 || helicopters_currents[2 * i + 1] >= 0.69 || helicopters_currents[2 * i + 1] <= -0.89) {
                    glUniform1f(glGetUniformLocation(helicopterShader, "uA"), 0.7);
                    glUniform1f(glGetUniformLocation(helicopterShader, "uR"), 0.7);
                }
                else {
                    glUniform1f(glGetUniformLocation(helicopterShader, "uA"), abs(sin(10 * (0.01 + abs(helicopter_changes[2 * i]) + abs(helicopter_changes[2 * i + 1])) * pulse)));
                    glUniform1f(glGetUniformLocation(helicopterShader, "uR"), 1.0);
                }
                glDrawArrays(GL_POINTS, 0, sizeof(helicopter_point_vertices) / helicopter_point_stride);
                p++;

                helicopters_currents[2 * i] = rnX;
                helicopters_currents[2 * i + 1] = rnY;

                if (abs(rnX - 0.11) < 0.01 && abs(rnY) < 0.01) {
                    //std::cout << i << "\t" << p << std::endl;
                    p = 0;
                    helicopter_changes[2 * i] = x;
                    helicopter_changes[2 * i + 1] = y;
                    x = 0;
                    y = 0;
                    //started_moving = true;
                    helicopters_moving[i] = false;
                    /*if (i < 4) {
                        helicopters_moving[i + 1] = true;
                    }*/

                    glUseProgram(helicopterShader);
                    if (helicopters_currents[2 * i] >= 0.89 || helicopters_currents[2 * i] <= -0.69 || helicopters_currents[2 * i + 1] >= 0.69 || helicopters_currents[2 * i + 1] <= -0.89) {
                        glUniform1f(glGetUniformLocation(helicopterShader, "uA"), 0.7);
                        glUniform1f(glGetUniformLocation(helicopterShader, "uR"), 0.7);
                    }
                    else {
                        glUniform1f(glGetUniformLocation(helicopterShader, "uA"), 0);
                        glUniform1f(glGetUniformLocation(helicopterShader, "uR"), 1);
                    }
                    starting_point = std::chrono::system_clock::now();
                    end = std::chrono::system_clock::now();
                    duration = end - starting_point;
                    dur = duration.count();
                }


                //for (int j = 0; j < 5; j++) {
                    if (!helicopters_dead[i] && abs(helicopters_currents[2 * i] - dron_currents[0]) < 0.035 && abs(helicopters_currents[2 * i + 1] - dron_currents[1]) < 0.035) {
                        std::cout << "sudar!" << std::endl;
                        should_kill_dron = true;
                        helicopters_dead[i] = true;
                        p = 0;
                        x = 0;
                        y = 0;
                        started_moving = true;
                        helicopters_moving[i] = false;
                        if (i < 4) {
                            helicopters_moving[i + 1] = true;
                            //helicopters_dead[i + 1] = false;
                        }
                        starting_point = std::chrono::system_clock::now();
                        end = std::chrono::system_clock::now();
                        duration = end - starting_point;
                        dur = duration.count();
                    }
                    /*else {
                        std::cout << "\t" << helicopters_currents[2 * i] << "\n" << helicopters_currents[2 * i + 1] << std::endl;
                        std::cout << "\t" << dron_currents[0] << "\n" << dron_currents[1] << std::endl;
                    }*/
                //}
                    
            } else if (!helicopters_moving[i] && !helicopters_dead[i]) {
                /*glBindVertexArray(VAO[8 + i]);*/
                glUseProgram(helicopterShader);
                glUniform2f(uPosLocH, helicopter_changes[2 * i], helicopter_changes[2 * i + 1]);
                //std::cout << helicopters_currents[2 * i] << "\t" << helicopters_currents[2 * i + 1] << std::endl;
                if (helicopters_currents[2 * i] >= 0.89 || helicopters_currents[2 * i] <= -0.69 || helicopters_currents[2 * i + 1] >= 0.69 || helicopters_currents[2 * i + 1] <= -0.89) {
                    glUniform1f(glGetUniformLocation(helicopterShader, "uA"), 0.7);
                    glUniform1f(glGetUniformLocation(helicopterShader, "uR"), 0.7);
                }
                else {
                    glUniform1f(glGetUniformLocation(helicopterShader, "uA"), 0);
                    glUniform1f(glGetUniformLocation(helicopterShader, "uR"), 1);
                }
                glDrawArrays(GL_POINTS, 0, sizeof(helicopter_point_vertices) / helicopter_point_stride);
                if (!helicopters_dead[i] && abs(helicopters_currents[2 * i] - dron_currents[0]) < 0.03 && abs(helicopters_currents[2 * i + 1] - dron_currents[1]) < 0.03) {
                    std::cout << "sudar!" << std::endl;
                    should_kill_dron = true;
                    helicopters_dead[i] = true;
                    started_moving = true;
                    //helicopters_moving[i] = false;
                    if (i < 4) {
                        helicopters_moving[i + 1] = true;
                        //helicopters_dead[i + 1] = false;
                    }
                    //starting_point = std::chrono::system_clock::now();
                    end = std::chrono::system_clock::now();
                    duration = end - starting_point;
                    dur = duration.count();
                }
            } 
            else if (helicopters_dead[i]){
      /*          glUseProgram(helicopterShader);
                glUniform2f(uPosLocH, helicopter_changes[2 * i], helicopter_changes[2 * i + 1]);
                glUniform1f(glGetUniformLocation(helicopterShader, "uA"), 0.7);
                glDrawArrays(GL_POINTS, 0, sizeof(helicopter_point_vertices) / helicopter_point_stride);*/
                /*p = 0;
                x = 0;
                y = 0;*/
                //started_moving = true;
                //helicopters_moving[i] = false;
                //if (i < 4) {
                //    helicopters_moving[i + 1] = true;
                //    //helicopters_dead[i + 1] = false;
                //}
                //starting_point = std::chrono::system_clock::now();
                end = std::chrono::system_clock::now();
                duration = end - starting_point;
                dur = duration.count();
            }
        }

        //for (int i = 0; i < 7; i++) {
        //    glUseProgram(dronShader);
        //    glBindVertexArray(VAO[13 + i]);
        //    /*glUniform2f(uPosLoc, changes[2 * i], changes[2 * i + 1]);
        //    glUniform1f(glGetUniformLocation(helicopterShader, "uA"), 0);*/
        //    glDrawArrays(GL_POINTS, 0, sizeof(dron_point_vertices) / dron_point_stride);
        //}

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glUseProgram(0);


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteBuffers(n, VBO);
    glDeleteVertexArrays(n, VAO);
    glDeleteProgram(diodShader);
    glDeleteProgram(greenShader);
    glDeleteProgram(mapShader);
    glDeleteProgram(circleShader);
    glDeleteProgram(hideMapShader);
    glDeleteProgram(indicatorShader);
    glDeleteProgram(helicopterShader);
    glDeleteProgram(dronShader);

    glfwTerminate();
    return 0;
}

unsigned int compileShader(GLenum type, const char* source)
{
    //Uzima kod u fajlu na putanji "source", kompajlira ga i vraca sejder tipa "type"
    //Citanje izvornog koda iz fajla
    std::string content = "";
    std::ifstream file(source);
    std::stringstream ss;
    if (file.is_open())
    {
        ss << file.rdbuf();
        file.close();
        std::cout << "Uspjesno procitao fajl sa putanje \"" << source << "\"!" << std::endl;
    }
    else {
        ss << "";
        std::cout << "Greska pri citanju fajla sa putanje \"" << source << "\"!" << std::endl;
    }
     std::string temp = ss.str();
     const char* sourceCode = temp.c_str(); //Izvorni kod sejdera koji citamo iz fajla na putanji "source"

    int shader = glCreateShader(type); //Napravimo prazan sejder odredjenog tipa (vertex ili fragment)
    
    int success; //Da li je kompajliranje bilo uspjesno (1 - da)
    char infoLog[512]; //Poruka o gresci (Objasnjava sta je puklo unutar sejdera)
    glShaderSource(shader, 1, &sourceCode, NULL); //Postavi izvorni kod sejdera
    glCompileShader(shader); //Kompajliraj sejder

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success); //Provjeri da li je sejder uspjesno kompajliran
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog); //Pribavi poruku o gresci
        if (type == GL_VERTEX_SHADER)
            printf("VERTEX");
        else if (type == GL_FRAGMENT_SHADER)
            printf("FRAGMENT");
        printf(" sejder ima gresku! Greska: \n");
        printf(infoLog);
    }
    return shader;
}
unsigned int createShader(const char* vsSource, const char* fsSource)
{
    //Pravi objedinjeni sejder program koji se sastoji od Vertex sejdera ciji je kod na putanji vsSource

    unsigned int program; //Objedinjeni sejder
    unsigned int vertexShader; //Verteks sejder (za prostorne podatke)
    unsigned int fragmentShader; //Fragment sejder (za boje, teksture itd)

    program = glCreateProgram(); //Napravi prazan objedinjeni sejder program

    vertexShader = compileShader(GL_VERTEX_SHADER, vsSource); //Napravi i kompajliraj vertex sejder
    fragmentShader = compileShader(GL_FRAGMENT_SHADER, fsSource); //Napravi i kompajliraj fragment sejder

    //Zakaci verteks i fragment sejdere za objedinjeni program
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program); //Povezi ih u jedan objedinjeni sejder program
    glValidateProgram(program); //Izvrsi provjeru novopecenog programa

    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_VALIDATE_STATUS, &success); //Slicno kao za sejdere
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(program, 512, NULL, infoLog);
        std::cout << "Objedinjeni sejder ima gresku! Greska: \n";
        std::cout << infoLog << std::endl;
    }

    //Posto su kodovi sejdera u objedinjenom sejderu, oni pojedinacni programi nam ne trebaju, pa ih brisemo zarad ustede na memoriji
    glDetachShader(program, vertexShader);
    glDeleteShader(vertexShader);
    glDetachShader(program, fragmentShader);
    glDeleteShader(fragmentShader);

    return program;
}

static unsigned loadImageToTexture(const char* filePath) {
    int TextureWidth;
    int TextureHeight;
    int TextureChannels;
    unsigned char* ImageData = stbi_load(filePath, &TextureWidth, &TextureHeight, &TextureChannels, 0);
    if (ImageData != NULL)
    {
        //Slike se osnovno ucitavaju naopako pa se moraju ispraviti da budu uspravne
        //stbi__vertical_flip(ImageData, TextureWidth, TextureHeight, TextureChannels);

        // Provjerava koji je format boja ucitane slike
        GLint InternalFormat = -1;
        switch (TextureChannels) { 
        case 1: InternalFormat = GL_RED; break;
        case 3: InternalFormat = GL_RGB; break;
        case 4: InternalFormat = GL_RGBA; break;
        default: InternalFormat = GL_RGB; break;
        }

        unsigned int Texture;
        glGenTextures(1, &Texture);
        glBindTexture(GL_TEXTURE_2D, Texture);
        glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, TextureWidth, TextureHeight, 0, InternalFormat, GL_UNSIGNED_BYTE, ImageData);
        glBindTexture(GL_TEXTURE_2D, 0);
        // oslobadjanje memorije zauzete sa stbi_load posto vise nije potrebna
        stbi_image_free(ImageData);
        return Texture;
    }
    else
    {
        std::cout << "Textura nije ucitana! Putanja texture: " << filePath << std::endl;
        stbi_image_free(ImageData);
        return 0;
    }
}



std::vector<float> generateHelicopters() {
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_real_distribution<float> disDown(-1.0f, -0.93f);
    std::uniform_real_distribution<float> disLeft(-0.82f, -0.75f);
    std::uniform_real_distribution<float> disUp(0.75f, 0.82f);
    std::uniform_real_distribution<float> disRight(0.93f, 1.0f);
    std::uniform_real_distribution<float> disAll(-1.0, 1.0f);

    std::vector<float> randomNumbers;

    // leva traka * 2
    float randomFloat = disLeft(gen);
    randomNumbers.push_back(randomFloat);
    randomFloat = disAll(gen);
    randomNumbers.push_back(randomFloat);

    randomFloat = disLeft(gen);
    randomNumbers.push_back(randomFloat);
    randomFloat = disAll(gen);
    randomNumbers.push_back(randomFloat);

    // donja traka
    randomFloat = disAll(gen);
    randomNumbers.push_back(randomFloat);
    randomFloat = disDown(gen);
    randomNumbers.push_back(randomFloat);

    // desna traka
    randomFloat = disRight(gen);
    randomNumbers.push_back(randomFloat);
    randomFloat = disAll(gen);
    randomNumbers.push_back(randomFloat);

    // gornja traka
    randomFloat = disAll(gen);
    randomNumbers.push_back(randomFloat);
    randomFloat = disUp(gen);
    randomNumbers.push_back(randomFloat);

    //std::random_device rd2;

    //std::mt19937 g(rd2());

    //std::shuffle(randomNumbers.begin(), randomNumbers.end(), g);


    return randomNumbers;
}


int generateRandomTime() {
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_real_distribution<float> dis(2.0, 15.0);

    //std::cout << (int)dis(gen) << std::endl;
    return (int) dis(gen);
}