#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <sstream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <random>

#include "shader.hpp"
#include "model.hpp"

#include "vertices.h"

const int n = 3;

const unsigned int wWidth = 1800;
const unsigned int wHeight = 900;
const char wTitle[] = "Kristina Andrijin [SV26/2020]";

static unsigned loadImageToTexture(const char* filePath);
unsigned int compileShader(GLenum type, const char* source);
unsigned int createShader(const char* vsSource, const char* fsSource);
float generateRandomHelicopterY();
float generateRandomHelicopterDownY();
float generateRandomX();
float generateRandomZ();


int main()
{

    if(!glfwInit())
    {
        std::cout << "GLFW fail!\n" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(wWidth, wHeight, wTitle, NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Window fail!\n" << std::endl;
        glfwTerminate();
        return -2;
    }
    glfwMakeContextCurrent(window);

    if (glewInit() !=GLEW_OK)
    {
        std::cout << "GLEW fail! :(\n" << std::endl;
        return -3;
    }

    Shader textureShader("textures/basic_2d.vert", "textures/basic_2d.frag");

    unsigned int VAO[n];
    glGenVertexArrays(n, VAO);

    unsigned int VBO[n];
    glGenBuffers(n, VBO);

    unsigned int EBO[2];
    glGenBuffers(1, &EBO[0]);

    float name_vertices[] = {
       -0.98, 0.6,     0.0, 1.0,       //1.0, 1.0, 0.0, 0.5,
       -0.7, 0.6,     1.0, 1.0,       //1.0, 1.0, 0.0, 0.5,
       -0.98,   0.9,      0.0, 0.0,       //1.0, 1.0, 0.0, 0.5,
       -0.7, 0.9,      1.0, 0.0,       //1.0, 1.0, 0.0, 0.5,
    };

    //float name_vertices[] = {
    //-0.9,  0.4, 0.0, 1.0, // Bottom-left
    //-0.6,  0.4, 1.0, 1.0, // Bottom-right
    //-0.9,  0.8, 0.0, 0.0, // Top-left
    //-0.6,  0.8, 1.0, 0.0  // Top-right
    //};


    //unsigned int green_shield_stride = (2 + 2) * sizeof(float);

    unsigned int name_stride = (2 + 2) * sizeof(float);

    unsigned int name_indices[] =
    {
        0, 1, 2,
        1, 2, 3,
    };


    glBindVertexArray(VAO[0]);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(name_vertices), name_vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, name_stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, name_stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &EBO[1]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(name_indices), name_indices, GL_STATIC_DRAW);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    unsigned nameTexture = loadImageToTexture("res/potpis.png"); //Ucitavamo teksturu
    glBindTexture(GL_TEXTURE_2D, nameTexture); //Podesavamo teksturu
    glGenerateMipmap(GL_TEXTURE_2D); //Generisemo mipmape 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);   //GL_NEAREST, GL_LINEAR
    glBindTexture(GL_TEXTURE_2D, 0);

    textureShader.setInt("uTex", 0);
    //unsigned uTexLoc = glGetUniformLocation(textureShader, "uTex");
    //glUniform1i(uTexLoc, 0);


    //Model lija("res/AIM120D.obj");
    Model helicopter("res/bixler.obj");
    //Tjemena i baferi su definisani u model klasi i naprave se pri stvaranju objekata

    Shader dronShader("basic.vert", "basic.frag");

    //Render petlja
    /*helicopterShader.use();
    helicopterShader.setVec3("uLightPos", 0, 1, 3);
    helicopterShader.setVec3("uViewPos", 0, 0, 5);
    helicopterShader.setVec3("uLightColor", 1, 1, 1);
    helicopterShader.setVec3("darkColor", 0.5, 0.0, 0.0);*/
    glm::mat4 projectionHelicopter = glm::perspective(glm::radians(45.0f), (float)wWidth / (float)wHeight, 0.1f, 100.0f);
    dronShader.setMat4("uP", projectionHelicopter);
    glm::mat4 viewHelicopter = glm::lookAt(glm::vec3(-0.0f, 0.5f, 2.0f), glm::vec3(-0.0f, 0.0f, 0.5f), glm::vec3(0.0f, 1.0f, 0.2f));
    dronShader.setMat4("uV", viewHelicopter);
    glm::mat4 modelHelicopter = glm::mat4(1.0f);

    glClearColor(0.5, 0.5, 0.5, 1.0);
    float x = generateRandomX();
    float y = generateRandomHelicopterY();
    float z = generateRandomX();
    glm::vec3 helicopterPosition(x, y, z);
    modelHelicopter = glm::mat4(1.0f); /// Reset the model matrix to identity
    modelHelicopter = glm::translate(modelHelicopter, glm::vec3(x, y, z)); // Set the new position
    modelHelicopter = glm::scale(modelHelicopter, glm::vec3(0.01f, 0.01f, 0.01f));

    dronShader.use();
    modelHelicopter = glm::rotate(modelHelicopter, glm::radians(1000.5f), glm::vec3(0.0f, 1.0f, 0.0f));
    dronShader.setMat4("uM", modelHelicopter);

    Model dron("res/bixler.obj");
    //Tjemena i baferi su definisani u model klasi i naprave se pri stvaranju objekata

    //Shader dronShader("basic.vert", "basic.frag");

    //Render petlja
    dronShader.use();
    dronShader.setVec3("uLightPos", 0, 1, 3);
    dronShader.setVec3("uViewPos", 0, 0, 5);
    dronShader.setVec3("uLightColor", 1, 1, 1);
    dronShader.setVec3("darkColor", 0.0, 0.0, 0.5);
    glm::mat4 projectionDron = glm::perspective(glm::radians(45.0f), (float)wWidth / (float)wHeight, 0.1f, 100.0f);
    dronShader.setMat4("uP", projectionDron);
    glm::mat4 viewDron = glm::lookAt(glm::vec3(-0.0f, 0.5f, 2.0f), glm::vec3(-0.0f, 0.0f, 0.5f), glm::vec3(0.0f, 1.0f, 0.2f));
    dronShader.setMat4("uV", viewDron);
    glm::mat4 modelDron = glm::mat4(1.0f);

    glClearColor(0.5, 0.5, 0.5, 1.0);

    modelDron = glm::mat4(1.0f); /// Reset the model matrix to identity
    modelDron = glm::translate(modelDron, glm::vec3(-0.335, 0.255, 1.835)); // Set the new position
    modelDron = glm::scale(modelDron, glm::vec3(0.01f, 0.01f, 0.01f));

    dronShader.use();
    modelDron = glm::rotate(modelDron, glm::radians(1000.5f), glm::vec3(0.0f, 1.0f, 0.0f));
    dronShader.setMat4("uM", modelDron);

    Model helicopterDown("res/bixler.obj");
    //Tjemena i baferi su definisani u model klasi i naprave se pri stvaranju objekata

    //Shader dronShader("basic.vert", "basic.frag");

    //Render petlja
    /*helicopterDownShader.use();
    helicopterDownShader.setVec3("uLightPos", 0, 1, 3);
    helicopterDownShader.setVec3("uViewPos", 0, 0, 5);
    helicopterDownShader.setVec3("uLightColor", 1, 1, 1);
    helicopterDownShader.setVec3("darkColor", 0.0, 0.5, 0.0);*/
    glm::mat4 projectionHelicopterDown = glm::perspective(glm::radians(45.0f), (float)wWidth / (float)wHeight, 0.1f, 100.0f);
    dronShader.setMat4("uP", projectionHelicopterDown);
    glm::mat4 viewHelicopterDown = glm::lookAt(glm::vec3(-0.0f, 0.5f, 2.0f), glm::vec3(-0.0f, 0.0f, 0.5f), glm::vec3(0.0f, 1.0f, 0.2f));
    dronShader.setMat4("uV", viewHelicopterDown);
    glm::mat4 modelHelicopterDown = glm::mat4(1.0f);

    glClearColor(0.2, 0.2, 0.2, 1.0);
    x = generateRandomX();
    y = generateRandomHelicopterDownY();
    z = generateRandomX();
    glm::vec3 helicopterDownPosition(x,y,z);
    modelHelicopterDown = glm::mat4(1.0f); /// Reset the model matrix to identity
    modelHelicopterDown = glm::translate(modelHelicopterDown, glm::vec3(x, y, z)); // Set the new position
    modelHelicopterDown = glm::scale(modelHelicopterDown, glm::vec3(0.01f, 0.01f, 0.01f));

    dronShader.use();
    modelHelicopterDown = glm::rotate(modelHelicopterDown, glm::radians(1000.5f), glm::vec3(0.0f, 1.0f, 0.0f));
    dronShader.setMat4("uM", modelHelicopterDown);


    unsigned int floor_VAO;
    glGenVertexArrays(1, &floor_VAO);
    glBindVertexArray(floor_VAO);

    unsigned int floor_VBO;
    glGenBuffers(1, &floor_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, floor_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floor_vertices), floor_vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, floor_stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, floor_stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, floor_stride, (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    Shader map3dShader("phong/phong.vert", "phong/phong.frag");
    map3dShader.use();
    map3dShader.setBool("hasText", true);
    //Teksture
    unsigned floorDiffuse = loadImageToTexture("res/mapa3d.png");
    glBindTexture(GL_TEXTURE_2D, floorDiffuse); //Podesavamo teksturu
    glGenerateMipmap(GL_TEXTURE_2D); //Generisemo mipmape 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);   //GL_NEAREST, GL_LINEAR
    glBindTexture(GL_TEXTURE_2D, 0);
    map3dShader.setInt("uMaterial.diffuse", 0);

    unsigned floorSpecular = loadImageToTexture("res/mapa_spec.png");
    glBindTexture(GL_TEXTURE_2D, floorSpecular);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 1);
    map3dShader.setInt("uMaterial.specular", 1);

    glm::mat4 modelMap = glm::mat4(1.0f);

    glm::mat4 viewMap = glm::mat4(1.0f);
    viewHelicopter = glm::lookAt(glm::vec3(-0.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 projectionP = glm::perspective(glm::radians(80.0f), (float)wWidth / (float)wHeight, 0.1f, 100.0f);

    map3dShader.use();

    map3dShader.setMat4("uM", modelMap);
    map3dShader.setMat4("uV", viewMap);
    map3dShader.setMat4("uP", projectionP);

    map3dShader.setVec3("uLight.pos", 1.0f, 1.0f, 1.0f);
    map3dShader.setVec3("uLight.kA", 0.2, 0.2, 0.2);
    map3dShader.setVec3("uLight.kD", 0.8, 0.8, 0.8);
    map3dShader.setVec3("uLight.kS", 1.0, 1.0, 1.0);

    map3dShader.setVec3("uViewPos", -0.9f, 0.6f, 0.7f);
    map3dShader.setFloat("uMaterial.shine", 42.0f);


    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);

    
    float movementSpeedHelicopter = 0.005f;
    float movementSpeedDron = movementSpeedHelicopter*3;
    float movementSpeedHelicopterDown = movementSpeedHelicopter/2;
    glm::vec3 dronPosition(-0.335, 0.255, 2.0);
    //float movementSpeedDron = movementSpeedHelicopter * 3;
    float sceneWidth = 2.0f;
    float sceneHeight = 1.0f;

    bool helicopterExist = true;
    bool helicopterDownExist = true;
    bool dronExist = false;

    int hel = 0;
    int helD = 0;
    int dronCount = 0;
    bool helicopters[3] = { true, false, false };
    bool helicoptersDown[2] = { true, false };
    bool drons[7] = { true, false, false, false, false, false, false };
    bool pressedX = false;
    bool canLaunchDron = true;

    while (!glfwWindowShouldClose(window))
    {

        float currentTime = glfwGetTime();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        dronShader.use();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //model = glm::rotate(model, glm::radians(0.1f), glm::vec3(-1.0f, 0.0f, 0.0f));
        //model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f)); // Adjust the scaling factor as needed
        //model = glm::translate(model, glm::vec3(0.0f, 0.0f, -1.0f));

        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && canLaunchDron) {
            dronExist = true;
        }
        
        if (dronExist && canLaunchDron) {
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            {
                dronShader.use();
                dronPosition.x -= movementSpeedDron;

                if (dronPosition.x < -1.5) {
                    dronPosition.x = -1.5;
                }

                //std::cout << "Helicopter X Position: " << helicopterPosition.y << std::endl;
            }
            else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            {
                dronShader.use();
                dronPosition.x += movementSpeedDron;

                if (dronPosition.x > sceneWidth - 0.2) {
                    dronPosition.x = sceneWidth - 0.2;
                }
            }
            else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            {
                dronShader.use();
                dronPosition.y -= movementSpeedDron;

                /*if (dronPosition.z > -1.0 && dronPosition.z < 0.0 && dronPosition.y < -0.5) {
                    dronPosition.y = -0.5;
                }
                else if (dronPosition.z > 0.0 && dronPosition.z < 1.0 && dronPosition.y < -0.1) {
                    dronPosition.y = -0.1;
                }
                else if (dronPosition.z > 1.0 && dronPosition.z < 2.0 && dronPosition.y < 0.0) {
                    dronPosition.y = -0.0;
                }*/

                //if (dronPosition.y < 1) {
                //    dronPosition.y = 1; // ovde treba da se uništi
                //}

                //std::cout << "Helicopter X Position: " << helicopterPosition.y << std::endl;
            }
            else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            {
                dronShader.use();
                dronPosition.y += movementSpeedDron;

                if (dronPosition.z > 0.5) {
                    dronPosition.z = 0.5;
                }

            }

            else if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
            {
                dronShader.use();
                dronPosition.z -= movementSpeedDron;

                /*if (dronPosition.z > -1.0 && dronPosition.z < 0.0 && dronPosition.y < -0.5) {
                    dronPosition.y = -0.5;
                }
                else if (dronPosition.z > 0.0 && dronPosition.z < 1.0 && dronPosition.y < -0.1) {
                    dronPosition.y = -0.1;
                }
                else if (dronPosition.z > 1.0 && dronPosition.z < 2.0 && dronPosition.y < 0.0) {
                    dronPosition.y = -0.0;
                }*/

                if (dronPosition.z < -2) {
                    dronPosition.z = -2;
                }

                //std::cout << "Helicopter X Position: " << helicopterPosition.y << std::endl;
            }
            else if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
            {
                dronShader.use();
                dronPosition.z += movementSpeedDron;

                /*if (dronPosition.z > -1.0 && dronPosition.z < 0.0 && dronPosition.y < -0.5) {
                    dronPosition.y = -0.5;
                }
                else if (dronPosition.z > 0.0 && dronPosition.z < 1.0 && dronPosition.y < -0.1) {
                    dronPosition.y = -0.1;
                }
                else if (dronPosition.z > 1.0 && dronPosition.z < 2.0 && dronPosition.y < 0.0) {
                    dronPosition.y = -0.0;
                }*/

                if (dronPosition.z > 2) {
                    dronPosition.z = 2;
                }
            }
        }

        if (dronExist && glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
        {
            if (dronCount < 7) {
                pressedX = false;
                drons[dronCount] = false;
                dronExist = false;
                dronCount++;
                if (!(dronCount < 7)) {
                    canLaunchDron = false;
                    dronPosition = glm::vec3(-0.335, 0.255, 2.0);
                }
                else {
                    if (dronCount < 6) {
                        drons[dronCount + 1] = true;
                    }
                    dronPosition = glm::vec3(-0.335, 0.255, 2.0);
                }
                std::cout << dronCount << std::endl;
            }
            else {
                canLaunchDron = false;
                pressedX = false;
                std::cout << "nema vise dronova!" << std::endl;
                dronExist = false;
            }
        }
        //std::cout << dronPosition.x << " " << dronPosition.y << " " << dronPosition.z << std::endl;
        /*std::cout << dronPosition.x << " " << dronPosition.y << " " << dronPosition.z << std::endl;
        std::cout << helicopterPosition.x << " " << helicopterPosition.y << " " << helicopterPosition.z << std::endl;
        std::cout << helicopterDownPosition.x << " " << helicopterDownPosition.y << " " << helicopterDownPosition.z << std::endl;*/
        modelDron = glm::mat4(1.0f);
        modelDron = glm::translate(modelDron, dronPosition);
        modelDron = glm::scale(modelDron, glm::vec3(0.01f, 0.01f, 0.01f));
        dronShader.use();
        dronShader.setVec3("darkColor", 0.0, 0.0, 0.5);
        dronShader.setFloat("uTime", currentTime);
        dronShader.setMat4("uM", modelDron);
        //model = glm::rotate(model, glm::radians(0.1f), glm::vec3(-1.0f, 0.0f, 0.0f));

        dronShader.use();
        if (helicopterExist) {
            helicopterPosition.x += movementSpeedHelicopter;
            //helicopterPosition.y += movementSpeedHelicopter;

            if (helicopterPosition.x > sceneWidth - 0.7) {
                helicopterPosition.x = sceneWidth - 0.7;
                movementSpeedHelicopter *= -1;
            } else if (helicopterPosition.x < -0.8) {
                helicopterPosition.x = -0.8;
                movementSpeedHelicopter *= -1;
            }
            //if (helicopterPosition.y > sceneHeight - 0.5) {
            //    helicopterPosition.y = sceneHeight - 0.5;
            //    //movementSpeedHelicopter *= -1;
            //}

            //std::cout << helicopterPosition.x << " " << helicopterPosition.y << " " << helicopterPosition.z << std::endl;

            modelHelicopter = glm::mat4(1.0f);
            modelHelicopter = glm::translate(modelHelicopter, helicopterPosition);
            modelHelicopter = glm::scale(modelHelicopter, glm::vec3(0.01f, 0.01f, 0.01f));

            dronShader.setVec3("darkColor", 0.5, 0.0, 0.0);
            dronShader.setFloat("uTime", currentTime);
            dronShader.setMat4("uM", modelHelicopter);
            helicopter.Draw(dronShader);
        }
       /* else {
            x = generateRandomX();
            y = generateRandomHelicopterY();
            z = generateRandomX();
            helicopterPosition = glm::vec3(x, y, z);
            helicopterExist = true;
        }*/

        dronShader.use();
        dronShader.setVec3("darkColor", 0.0, 0.0, 0.5);
        dronShader.setFloat("uTime", currentTime);
        dronShader.setMat4("uM", modelDron);
        dron.Draw(dronShader);

        if (helicopterDownExist) {
            helicopterDownPosition.x += movementSpeedHelicopterDown;
            //helicopterDownPosition.y += movementSpeedHelicopterDown;

            if (helicopterDownPosition.x > sceneWidth - 0.7) {
                helicopterDownPosition.x = sceneWidth - 0.7;
                movementSpeedHelicopterDown *= -1;
            } else if (helicopterDownPosition.x < -0.7) {
                helicopterDownPosition.x = - 0.7;
                movementSpeedHelicopterDown *= -1;
            }
            //if (helicopterDownPosition.y > -0.4) {
            //    helicopterDownPosition.y = -0.4;
            //    //movementSpeedHelicopterDown *= -1;
            //}

            /*if (helicopterDownPosition.x == sceneWidth - 0.5 && helicopterDownPosition.y == -0.4)
                movementSpeedHelicopterDown *= -1;*/
                //std::cout << "Helicopter X Position: " << helicopterPosition.y << std::endl;

            modelHelicopterDown = glm::mat4(1.0f);
            modelHelicopterDown = glm::translate(modelHelicopterDown, helicopterDownPosition);
            modelHelicopterDown = glm::scale(modelHelicopterDown, glm::vec3(0.01f, 0.01f, 0.01f));

            dronShader.use();
            dronShader.setVec3("darkColor", 0.0, 0.5, 0.0);
            dronShader.setFloat("uTime", currentTime);
            dronShader.setMat4("uM", modelHelicopterDown);
            helicopterDown.Draw(dronShader);
        }
        /*model = glm::rotate(model, glm::radians(0.1f), glm::vec3(0.0f, 1.0f, 0.0f));
        unifiedShader.setMat4("uM", model);
        lija.Draw(unifiedShader);*/

        if (abs(dronPosition.x - helicopterPosition.x) < 0.1 && abs(dronPosition.y - helicopterPosition.y) < 0.1 && abs(dronPosition.z - helicopterPosition.z) < 0.1) {
            std::cout << "sudar helikopter" << std::endl;
            helicopterExist = false;
            helicopters[hel] = false;
            drons[dronCount] = false;
            dronExist = false;
            if (hel < 2) {
                helicopters[hel + 1] = true;
                x = generateRandomX();
                y = generateRandomHelicopterY();
                z = generateRandomX();
                helicopterPosition = glm::vec3(x, y, z);
                helicopterExist = true;
                hel++;
            } else{
                std::cout << "nema vise helikoptera" << std::endl;
                helicopterExist = false;
            }

            if (dronCount < 7) {
                drons[dronCount + 1] = true;
                dronPosition = glm::vec3(-0.335, 0.255, 2.0);
                //dronExist = true;
                dronCount++;
            }
            else {
                std::cout << "nema vise dronova!" << std::endl;
                dronExist = false;
            }
        }
        if (abs(dronPosition.x - helicopterDownPosition.x) < 0.1 && abs(dronPosition.y - helicopterDownPosition.y) < 0.1 && abs(dronPosition.z - helicopterDownPosition.z) < 0.1) {
            std::cout << "sudar donji" << std::endl;
            helicopterDownExist = false;
            helicoptersDown[helD] = false;
            drons[dronCount] = false;
            dronExist = false;
            if (helD < 1) {
                helicoptersDown[helD + 1] = true;
                x = generateRandomX();
                y = generateRandomHelicopterDownY();
                z = generateRandomX();
                helicopterDownPosition = glm::vec3(x, y, z);
                helicopterDownExist = true;
                helD++;
            }
            else {
                std::cout << "nema vise donjih" << std::endl;
                helicopterDownExist = false;
            }

            if (dronCount < 7) {
                drons[dronCount + 1] = true;
                dronPosition = glm::vec3(-0.335, 0.255, 2.0);
                //dronExist = true;
                dronCount++;
            }
            else {
                std::cout << "nema vise dronova!" << std::endl;
                dronExist = false;
            }
        }



        textureShader.use();
        glBindVertexArray(VAO[0]);

        glActiveTexture(GL_TEXTURE0); //tekstura koja se bind-uje nakon ovoga ce se koristiti sa SAMPLER2D uniformom u sejderu koja odgovara njenom indeksu
        glBindTexture(GL_TEXTURE_2D, nameTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

       //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        map3dShader.use();
        glBindVertexArray(floor_VAO);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, floorDiffuse);
        //glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        // bind specular map
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, floorSpecular);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        map3dShader.use();
        //modelMap = glm::rotate(modelMap, glm::radians(-0.5f), glm::vec3(0.0f, 1.0f, 0.0f));
        //modelMap = glm::translate(modelMap, glm::vec3(0.0f, 0.0f, 1.0f));
        map3dShader.setMat4("uM", modelMap);

        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(0 * sizeof(unsigned int)));
        glBindTexture(GL_TEXTURE_2D, 0);

        glBindVertexArray(0);
        glUseProgram(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
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

        //std::cout << TextureChannels << std::endl;

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

unsigned int compileShader(GLenum type, const char* source)
{
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
    const char* sourceCode = temp.c_str();

    int shader = glCreateShader(type);

    int success;
    char infoLog[512];
    glShaderSource(shader, 1, &sourceCode, NULL);
    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
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
    unsigned int program;
    unsigned int vertexShader;
    unsigned int fragmentShader;

    program = glCreateProgram();

    vertexShader = compileShader(GL_VERTEX_SHADER, vsSource);
    fragmentShader = compileShader(GL_FRAGMENT_SHADER, fsSource);

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program);
    glValidateProgram(program);

    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_VALIDATE_STATUS, &success);
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(program, 512, NULL, infoLog);
        std::cout << "Objedinjeni sejder ima gresku! Greska: \n";
        std::cout << infoLog << std::endl;
    }

    glDetachShader(program, vertexShader);
    glDeleteShader(vertexShader);
    glDetachShader(program, fragmentShader);
    glDeleteShader(fragmentShader);

    return program;
}

float generateRandomHelicopterY() {
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_real_distribution<float> dis(0.1, 0.5);

    //std::cout << (int)dis(gen) << std::endl;
    return (float)dis(gen);
}

float generateRandomHelicopterDownY() {
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_real_distribution<float> dis(-0.3, -0.1);

    //std::cout << (int)dis(gen) << std::endl;
    float rad = (float)dis(gen);
    //std::cout << rad << std::endl;
    return rad;
}

float generateRandomZ() {
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_real_distribution<float> dis(-1.0, -0.5);

    //std::cout << (int)dis(gen) << std::endl;
    return (float)dis(gen);
    //return -0.5;
}

float generateRandomX() {
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_real_distribution<float> dis(-5.0, 5.0);
    float random = dis(gen);
    if (random > 0) {
        return -0.5;
    };
    return 0.5;
}
