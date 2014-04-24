#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <math.h>
#include <GL/glew.h>

typedef struct
{
    double x;
    double y;
    double z;
} point;

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

void generate_projection_matrix(GLfloat *matrix, double fovx, double aspect, double nearDist, double farDist)
{
    // Based on the gluPerspective implementation in java
    double deltaDist = farDist - nearDist;
    double cot = 1 / tan(fovx*M_PI / (2*aspect*180));

    // Column-major ordering, matrix[0*4 + 1] is right beneath matrix[0*4 + 0]
    matrix[0*4 + 0] = cot / aspect;
    matrix[0*4 + 1] = 0;
    matrix[0*4 + 2] = 0;
    matrix[0*4 + 3] = 0;

    matrix[1*4 + 0] = 0;
    matrix[1*4 + 1] = cot;
    matrix[1*4 + 2] = 0;
    matrix[1*4 + 3] = 0;

    matrix[2*4 + 0] = 0;
    matrix[2*4 + 1] = 0;
    matrix[2*4 + 2] = -(farDist + nearDist) / deltaDist;
    matrix[2*4 + 3] = -1;

    matrix[3*4 + 0] = 0;
    matrix[3*4 + 1] = 0;
    matrix[3*4 + 2] = -2 * nearDist * farDist / deltaDist;
    matrix[3*4 + 3] = 0;


    // DEBUGTOOL
//    matrix[0*4 + 0] = 1;
//    matrix[0*4 + 1] = 0;
//    matrix[0*4 + 2] = 0;
//    matrix[0*4 + 3] = 0;
//
//    matrix[1*4 + 0] = 0;
//    matrix[1*4 + 1] = 1;
//    matrix[1*4 + 2] = 0;
//    matrix[1*4 + 3] = 0;
//
//    matrix[2*4 + 0] = 0;
//    matrix[2*4 + 1] = 0;
//    matrix[2*4 + 2] = 1;
//    matrix[2*4 + 3] = 0;
//
//    matrix[3*4 + 0] = 0;
//    matrix[3*4 + 1] = 0;
//    matrix[3*4 + 2] = 0;
//    matrix[3*4 + 3] = 1;
}

void generate_view_matrix(GLfloat *matrix, point *cam_pos, point *cam_dir, point *up)
{
    // Assumption: cam_dir is normalized
    // Side = cam_dir X up
    point side;
    cross(cam_dir, up, &side);

    // Rel up = side X cam_dir
    point relative_up;
    cross(&side, cam_dir, &relative_up);

    // Column-major ordering, matrix[0*4 + 1] is right beneath matrix[0*4 + 0]
    matrix[0*4 + 0] = side.x;
    matrix[0*4 + 1] = relative_up.x;
    matrix[0*4 + 2] = -cam_dir->x;
    matrix[0*4 + 3] = 0;

    matrix[1*4 + 0] = side.y;
    matrix[1*4 + 1] = relative_up.y;
    matrix[1*4 + 2] = -cam_dir->y;
    matrix[1*4 + 3] = 0;

    matrix[2*4 + 0] = side.z;
    matrix[2*4 + 1] = relative_up.z;
    matrix[2*4 + 2] = -cam_dir->z;
    matrix[2*4 + 3] = 0;

    matrix[3*4 + 0] = 0;
    matrix[3*4 + 1] = 0;
    matrix[3*4 + 2] = 0;
    matrix[3*4 + 3] = 1;


    // DEBUGTOOL
//    matrix[0*4 + 0] = 1;
//    matrix[0*4 + 1] = 0;
//    matrix[0*4 + 2] = 0;
//    matrix[0*4 + 3] = 0;
//
//    matrix[1*4 + 0] = 0;
//    matrix[1*4 + 1] = 1;
//    matrix[1*4 + 2] = 0;
//    matrix[1*4 + 3] = 0;
//
//    matrix[2*4 + 0] = 0;
//    matrix[2*4 + 1] = 0;
//    matrix[2*4 + 2] = 1;
//    matrix[2*4 + 3] = 0;
//
//    matrix[3*4 + 0] = 0;
//    matrix[3*4 + 1] = 0;
//    matrix[3*4 + 2] = 0;
//    matrix[3*4 + 3] = 1;
}

#endif
