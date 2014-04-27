#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "constants.h"
#include "geometry.h"
#include "shaders.h"

void turn_view(GLFWwindow* window, point *cam_dir);
void move_position(GLFWwindow* window, point *cam_pos, point *cam_dir);

int main(void)
{
    // Initialize GLFW and a window
    GLFWwindow* window;
    if (!glfwInit())
    {
        printf("\nCouldn't initialize GLFW!\n");
        fflush(stdout);
        return -1;
    }

    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Terrain Renderer", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        printf("\nCouldn't create a window!\n");
        fflush(stdout);
        return -1;
    }

    // Set the current opengl context to the window
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    if (glewInit() != GLEW_OK)
    {
        printf("\nCouldn't initialize GLEW!\n");
        fflush(stdout);
        return -1;
    }

    // Set the viewport to the current window specifications
    glViewport(0, 0, (GLsizei)WINDOW_WIDTH, (GLsizei)WINDOW_HEIGHT);

    // Disable cursor
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    // Initialize application variables
    point cam_pos, cam_dir;
    cam_pos.x = -23.0;
    cam_pos.y = 37.0;
    cam_pos.z = -22.0;

    cam_dir.x = 0.653162;
    cam_dir.y = -0.532276;
    cam_dir.z = 0.538574;
    normalize(&cam_dir);

    // Clear the screen
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    // We don't need lighting yet
    glDisable(GL_LIGHTING);

    // Load and compile the shaders
    GLuint shader = shader_load("vertex.glsl", "fragment.glsl");
    glUseProgram(shader);

    // Wireframe mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Set up a grid of vertexes
    float *grid_vertices;
    int num_vertices = 0;
    grid_vertices = calloc(MAP_HEIGHT * MAP_WIDTH * 3, sizeof(float));

    for (int i=0; i<MAP_WIDTH; i++)
    {
        for (int j=0; j<MAP_HEIGHT; j++)
        {
            grid_vertices[num_vertices++] = i*GRID_UNIT_LENGTH;
            grid_vertices[num_vertices++] = 0;
            grid_vertices[num_vertices++] = j*GRID_UNIT_LENGTH;
        }
    }

    // Set up an index buffer
    GLushort *grid_indices;
    grid_indices = calloc(2*(MAP_HEIGHT)*(MAP_WIDTH-1) + (MAP_WIDTH-1), sizeof(GLushort));
    num_vertices = 0;

    for (int i=0; i<MAP_WIDTH-1; i++)
    {
        // If this is an even row (starting with 0)
        if (i%2 == 0)
        {
            for (int j=0; j<MAP_HEIGHT; j++)
            {
                grid_indices[num_vertices++] = i * (MAP_HEIGHT) + j;
                grid_indices[num_vertices++] = (i+1) * (MAP_HEIGHT) + j;
            }
            grid_indices[num_vertices++] = (i+1) * (MAP_HEIGHT) + (MAP_HEIGHT-1);
        }
        else
        {
            for (int j=MAP_HEIGHT-1; j>=0; j--)
            {
                grid_indices[num_vertices++] = i * (MAP_HEIGHT) + j;
                grid_indices[num_vertices++] = (i+1) * (MAP_HEIGHT) + j;
            }
            grid_indices[num_vertices++] = (i+1)*MAP_HEIGHT;
        }
    }

    // Create a VAO
    GLuint gridVAO;
    glGenVertexArrays(1, &gridVAO);
    glBindVertexArray(gridVAO);

    // Create a VBO
    GLuint gridVBO;
    glGenBuffers(1, &gridVBO);

    glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
    glBufferData(GL_ARRAY_BUFFER, MAP_WIDTH * MAP_HEIGHT * sizeof(float) * 3, grid_vertices, GL_STATIC_DRAW);

    // We also need an IBO
    GLuint gridIBO;
    glGenBuffers(1, &gridIBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gridIBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, num_vertices * sizeof(GLushort), grid_indices, GL_STATIC_DRAW);

    // The position here is defined in the shaders with layout
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // Allocate all the different matrices we'll need
    GLfloat *matrix_stack = calloc(16, sizeof(GLfloat));
    GLfloat *tmp_transform_matrix = calloc(16, sizeof(GLfloat));
    GLfloat *tmp_multiplication_matrix = calloc(16, sizeof(GLfloat));
    GLfloat *tmp_switch_matrix;
    GLfloat *projection_matrix = calloc(16, sizeof(GLfloat));

    // The projection matrix is constant, so we can set it once and for all
    generate_projection_matrix(projection_matrix, FOV, (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.5, VIEW_DISTANCE);

    point e_y;
    e_y.x = 0;
    e_y.y = 1;
    e_y.z = 0;

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Reset the matrix stack
        reset_to_identity(matrix_stack);

        // Update camera position and direction
        turn_view(window, &cam_dir);
        move_position(window, &cam_pos, &cam_dir);
        // Reset the mouse to the middle of the screen
        glfwSetCursorPos(window, WINDOW_WIDTH/2.0, WINDOW_HEIGHT/2.0);

        // Generate translation matrix
        generate_translation_matrix(tmp_transform_matrix, &cam_pos);
        // Add it in the stack
        add_matrix_to_stack(matrix_stack, tmp_transform_matrix, tmp_multiplication_matrix);
        tmp_switch_matrix = matrix_stack;
        matrix_stack = tmp_multiplication_matrix;
        tmp_multiplication_matrix = tmp_switch_matrix;

        // Generate view matrix for rotation
        generate_view_matrix(tmp_transform_matrix, &cam_dir, &e_y);
        // Add it in the stack
        add_matrix_to_stack(matrix_stack, tmp_transform_matrix, tmp_multiplication_matrix);
        tmp_switch_matrix = matrix_stack;
        matrix_stack = tmp_multiplication_matrix;
        tmp_multiplication_matrix = tmp_switch_matrix;

        // Add in the projection matrix
        add_matrix_to_stack(matrix_stack, projection_matrix, tmp_multiplication_matrix);
        tmp_switch_matrix = matrix_stack;
        matrix_stack = tmp_multiplication_matrix;
        tmp_multiplication_matrix = tmp_switch_matrix;

        // Send off matrix stack to the graphics card
        glUniformMatrix4fv(glGetUniformLocation(shader, "matrix_stack"), 1, GL_FALSE, matrix_stack);

        // Clear screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw everything
        glDrawElements(GL_TRIANGLE_STRIP, num_vertices, GL_UNSIGNED_SHORT, 0);

        //Force display to be drawn now
        glFlush();
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}

void turn_view(GLFWwindow* window, point *cam_dir)
{
    double mouse_x, mouse_y;
    glfwGetCursorPos(window, &mouse_x, &mouse_y);

    // Get the vector from the center of the screen to current mouse position and normalize it relative to screen size
    mouse_x = (mouse_x - WINDOW_WIDTH/2.0) * HORIZONTAL_TURNSPEED / WINDOW_WIDTH;
    mouse_y = -(mouse_y - WINDOW_HEIGHT/2.0) * VERTICAL_TURNSPEED / WINDOW_HEIGHT;

    // Rotate the view vector in accordance
    double tmp;
    tmp = cam_dir->x;
    cam_dir->x += mouse_x * -cam_dir->z;
    cam_dir->y = sin(mouse_y + asin(cam_dir->y));
    cam_dir->z += mouse_x * tmp;

    normalize(cam_dir);
}

void move_position(GLFWwindow* window, point *cam_pos, point *cam_dir)
{
    int forward = 0, side = 0;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        forward += 1;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        forward -= 1;
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        side += 1;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        side -= 1;
    }

    if (forward != 0)
    {
        // Go forward/backward in the looking direction
        cam_pos->x += forward * CAMERA_SPEED * cam_dir->x;
        cam_pos->y += forward * CAMERA_SPEED * cam_dir->y;
        cam_pos->z += forward * CAMERA_SPEED * cam_dir->z;
    }

    if (side != 0)
    {
        // Go forward/backward in cross(looking direction, (0|1|0))
        cam_pos->x += -side * CAMERA_SPEED * cam_dir->z;
        cam_pos->z += side * CAMERA_SPEED * cam_dir->x;
    }
}

// Stale and outdated code, only still exists for reference

//void update_view(point cam_pos, point cam_dir)
//{
//    float length, xrot, yrot;
//    length = sqrt(1-(cam_dir.y*cam_dir.y));
//    xrot = asin(cam_dir.x/length);
//    yrot = asin(cam_dir.y);
//
//    xrot *= 180/M_PI;
//    yrot *= 180/M_PI;
//
//    if (cam_dir.z > 0)
//    {
//        xrot = 180-xrot;
//    }
//
//    glRotatef(xrot, 0.0, 1.0, 0.0);
//    glRotatef(yrot, -cam_dir.z, 0.0, cam_dir.x);
//    glTranslated(-cam_pos.x, -cam_pos.y, -cam_pos.z);
//}
