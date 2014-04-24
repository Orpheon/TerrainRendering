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

void generate_projection_matrix(GLfloat *matrix, double half_fov, double aspect, double nearDist, double farDist)
{
    //
    // General form of the Projection Matrix
    //
    // uh = -Cot( fov/2 ) == -1/Tan(fov/2)
    // uw / uh = 1/aspect
    //
    //   uw         0       0       0
    //    0        uh       0       0
    //    0         0      f/(f-n)  1
    //    0         0    -fn/(f-n)  0

    double oneOverDepth = 1 / (farDist - nearDist);
    double oneOverTanFov = -1 / tan(half_fov);

//    matrix[0]  = oneOverTanFov / aspect;
//    matrix[1]  = 0;
//    matrix[2]  = 0;
//    matrix[3]  = 0;
//
//    matrix[4]  = 0;
//    matrix[5]  = oneOverTanFov;
//    matrix[6]  = 0;
//    matrix[7]  = 0;
//
//    matrix[8]  = 0;
//    matrix[9]  = 0;
//    matrix[10] = -farDist * oneOverDepth;
//    matrix[11] = -1;
//
//    matrix[12] = 0;
//    matrix[13] = 0;
//    matrix[14] = (-farDist * nearDist) * oneOverDepth;
//    matrix[15] = 0;

    // Source: http://www.terathon.com/gdc07_lengyel.pdf
    matrix[0]  = oneOverTanFov;
    matrix[1]  = 0;
    matrix[2]  = 0;
    matrix[3]  = 0;

    matrix[4]  = 0;
    matrix[5]  = oneOverTanFov / aspect;
    matrix[6]  = 0;
    matrix[7]  = 0;

    matrix[8]  = 0;
    matrix[9]  = 0;
    matrix[10] = -(farDist + nearDist) * oneOverDepth;
    matrix[11] = -1;

    matrix[12] = 0;
    matrix[13] = 0;
    matrix[14] = -(2 * farDist * nearDist) * oneOverDepth;
    matrix[15] = 0;
}

void generate_view_matrix(GLfloat *matrix, point *cam_pos, point *cam_dir)
{
    // http://www.opengl.org/sdk/docs/man2/xhtml/gluLookAt.xml
    // f = cam_dir, up = (0|1|0)
    // s = normalize(f X up)
    // u = s X f
    double x = cam_dir->x, y = cam_dir->y, z = cam_dir->z;
    double l = sqrt(z*z + x*x);
    double s_x = -z/l, s_z = x/l;
    double u_x = -s_z*y, u_y = s_z*x - s_x*z, u_z = s_x*y;
    // 4x4 matrix
    // 4th element is the translation
    // s[0]  s[1]  s[2]  x
    // u[0]  u[1]  u[2]  y
    // -f[0] -f[1] -f[2] z
    // 0     0     0     1

    matrix[0]  = s_x;
    matrix[1]  = u_x;
    matrix[2]  = -x;
    matrix[3]  = 0;

    matrix[4]  = 0;
    matrix[5]  = u_y;
    matrix[6]  = -y;
    matrix[7]  = 0;

    matrix[8]  = s_z;
    matrix[9]  = u_z;
    matrix[10] = -z;
    matrix[11] = 0;

//    matrix[0]  = 1;
//    matrix[1]  = 0;
//    matrix[2]  = 0;
//    matrix[3]  = 0;
//
//    matrix[4]  = 0;
//    matrix[5]  = 1;
//    matrix[6]  = 0;
//    matrix[7]  = 0;
//
//    matrix[8]  = 0;
//    matrix[9]  = 0;
//    matrix[10] = 1;
//    matrix[11] = 0;

    matrix[12] = cam_pos->x;
    matrix[13] = cam_pos->y;
    matrix[14] = cam_pos->z;
    matrix[15] = 1;
}

#endif

