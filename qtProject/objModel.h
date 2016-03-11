/*
 * CPSC 453 - Introduction to Computer Graphics
 * Assignment 3
 *
 * objModel - Class that handles and stores an OBJ model
 *
 * Note: The provided files are based off MD2 models, which have
 * specific animations as listed. A solitary OBJ file is not
 * animated, but the series of files provided have been generated
 * for each frame of the animation, in order
 */

#ifndef OBJMODEL_H
#define OBJMODEL_H
//#include <GL/glew.h>
#include <QOpenGLFunctions_4_2_Core>
#include <glm/glm.hpp>
#include <vector>

using namespace std;
using namespace glm;


// triangle
typedef struct
{
    GLuint index_xyz[3];    // indexes to triangle's vertices
    GLuint index_uv[3];     // indexes to triangles's coordinates
} tri;

// animation
typedef struct
{
        int first_frame;    // first frame of the animation
        int last_frame;     // number of frames
        int fps;            // number of frames per second

} anim_t;

// animation list
typedef enum {
        STAND,
        RUN,
        ATTACK,
        PAIN_A,
        PAIN_B,
        PAIN_C,
        JUMP,
        FLIP,
        SALUTE,
        FALLBACK,
        WAVE,
        POINT2,
        CROUCH_STAND,
        CROUCH_WALK,
        CROUCH_ATTACK,
        CROUCH_PAIN,
        CROUCH_DEATH,
        DEATH_FALLBACK,
        DEATH_FALLFORWARD,
        DEATH_FALLBACKSLOW,
        BOOM,

        MAX_ANIMATIONS

} animType_t;



class ObjModel {
	public:
        // constructor, destructor
        ObjModel ();
        ObjModel (const char *filename);
        virtual ~ObjModel() {};

        // loading model
        // TODO: You may want to update the LoadModel function to scale the model
        // to within a visible range
        bool LoadModel(const char *filename);

        // TODO: You may want to create and call the following methods during OBJ
        // loading, etc. You'll need to add the necessary member variables as well...
        //void calculateVertexNormals ();
        //void calculateFaceNormals ();
        //void draw (GLuint vertexLocation, GLuint normalLocation);

        // member variables
        // Note: This is poor formatting, as they should not be public
        int num_xyz;        // number of vertices
        int num_tris;       // number of triangles

        int skin_width;     // width of texture
        int skin_height;    // height of texture image

        vector<vec3> m_vertices; // vertex array
        vector<vec2> texs;  // texture coordinates
        vector<tri> tris;   // triangles

        static anim_t animlist[21]; // static array, listing the animations in the provided models

};

#endif
