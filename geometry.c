#include "geometry.h"

#include <math.h>
#include <GL/glew.h>

void normalize(point *a)
{
    double length;
    length = sqrt((*a).x*(*a).x + (*a).y*(*a).y + (*a).z*(*a).z);
    a->x /= length;
    a->y /= length;
    a->z /= length;
}

void cross(point *a, point *b, point *result)
{
    result->x = a->y*b->z - a->z*b->y;
    result->y = a->z*b->x - a->x*b->z;
    result->z = a->x*b->y - a->y*b->x;
}

void reset_to_identity(GLfloat *matrix)
{
    // Column-major ordering, matrix[0*4 + 1] is right beneath matrix[0*4 + 0]
    for (int column=0; column<4; column++)
    {
        for (int row=0; row<4; row++)
        {
            if (column == row)
            {
                matrix[4*column + row] = 1.0;
            }
            else
            {
                matrix[4*column + row] = 0.0;
            }
        }
    }
}

void add_matrix_to_stack(GLfloat *matrix_stack, GLfloat *matrix, GLfloat *tmp_matrix)
{
    double sum;
    for (int column=0; column<4; column++)
    {
        for (int row=0; row<4; row++)
        {
            sum = 0.0;
            for (int i=0; i<4; i++)
            {
                sum += matrix_stack[4*column + i] * matrix[4*i + row];
            }
            tmp_matrix[4*column + row] = sum;
        }
    }
}

void generate_projection_matrix(GLfloat *matrix, double fovx, double aspect, double nearDist, double farDist)
{
    reset_to_identity(matrix);

    // Based on the gluPerspective implementation in java
    double deltaDist = farDist - nearDist;
    double cot = 1 / tan(fovx*M_PI / (2*aspect*180));

    // Column-major ordering, matrix[0*4 + 1] is right beneath matrix[0*4 + 0]
    matrix[0*4 + 0] = cot / aspect;

    matrix[1*4 + 1] = cot;

    matrix[2*4 + 2] = -(farDist + nearDist) / deltaDist;
    matrix[2*4 + 3] = -1;

    matrix[3*4 + 2] = -2 * nearDist * farDist / deltaDist;
    matrix[3*4 + 3] = 0;
}

void generate_view_matrix(GLfloat *matrix, point *cam_dir, point *up)
{
    reset_to_identity(matrix);

    // Assumption: cam_dir is normalized
    // Side = cam_dir X up
    point side;
    cross(cam_dir, up, &side);
    normalize(&side);

    // Rel up = side X cam_dir
    point relative_up;
    cross(&side, cam_dir, &relative_up);

    // Column-major ordering, matrix[0*4 + 1] is right beneath matrix[0*4 + 0]
    matrix[0*4 + 0] = side.x;
    matrix[0*4 + 1] = relative_up.x;
    matrix[0*4 + 2] = -cam_dir->x;

    matrix[1*4 + 0] = side.y;
    matrix[1*4 + 1] = relative_up.y;
    matrix[1*4 + 2] = -cam_dir->y;

    matrix[2*4 + 0] = side.z;
    matrix[2*4 + 1] = relative_up.z;
    matrix[2*4 + 2] = -cam_dir->z;
}

void generate_translation_matrix(GLfloat *matrix, point *cam_pos)
{
    reset_to_identity(matrix);

    // Column-major ordering, matrix[0*4 + 1] is right beneath matrix[0*4 + 0]
    matrix[3*4 + 0] = -cam_pos->x;
    matrix[3*4 + 1] = -cam_pos->y;
    matrix[3*4 + 2] = -cam_pos->z;
}
