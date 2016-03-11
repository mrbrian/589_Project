// CPSC 453: Introduction to Computer Graphics
//
// Implementation file for a class to handle obj file data
//
// Created by Troy Alderson
// Department of Computer Science
// University of Calgary
//
// Used with permission for CPSC 453 HW3

#include "objModel.h"
#include <fstream>
#include <string>
#include <iostream>
#include <cstring>

using namespace std;
using namespace glm;

// Initialize the 21 model animations, for the provided models
anim_t ObjModel::animlist[ 21 ] =
{
        // first, last, fps

        {   0,  39,  9 },       // STAND
        {  40,  45, 10 },       // RUN
        {  46,  53, 10 },       // ATTACK
        {  54,  57,  7 },       // PAIN_A
        {  58,  61,  7 },       // PAIN_B
        {  62,  65,  7 },       // PAIN_C
        {  66,  71,  7 },       // JUMP
        {  72,  83,  7 },       // FLIP
        {  84,  94,  7 },       // SALUTE
        {  95, 111, 10 },       // FALLBACK
        { 112, 122,  7 },       // WAVE
        { 123, 134,  6 },       // POINT
        { 135, 153, 10 },       // CROUCH_STAND
        { 154, 159,  7 },       // CROUCH_WALK
        { 160, 168, 10 },       // CROUCH_ATTACK
        { 169, 172,  7 },       // CROUCH_PAIN
        { 173, 177,  5 },       // CROUCH_DEATH
        { 178, 183,  7 },       // DEATH_FALLBACK
        { 184, 189,  7 },       // DEATH_FALLFORWARD
        { 190, 197,  7 },       // DEATH_FALLBACKSLOW
        { 198, 198,  5 },       // BOOM
};

// default constructor
ObjModel::ObjModel () {

}

// file-loading constructor
ObjModel::ObjModel (const char *filename) {
    LoadModel(filename);
}

// loads the model, populating the data
bool ObjModel::LoadModel(const char *filename) {

    // clear prior data
    num_xyz = num_tris = skin_width = skin_height = 0;
    m_vertices.clear(); texs.clear(); tris.clear();

    // From http://www.opengl-tutorial.org/beginners-tutorials/tutorial-7-model-loading/

    printf("Loading OBJ file %s...\n", filename);

    // try to open the file
    FILE * file = fopen(filename, "r");
    if( file == NULL ){
        printf("Impossible to open the file! Do you have the correct filename?\n");
        getchar();
        return false;
    }

    // load values from the file
    while( true ){

        char lineHeader[128];
        // read the first word of the line
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF)
            break; // EOF = End Of File. Quit the loop.

        // parse lineHeader
        if ( strcmp( lineHeader, "v" ) == 0 ){
            // x, y, z: vertex coordinates
            glm::vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
            m_vertices.push_back(vertex);
            num_xyz++;

        }else if ( strcmp( lineHeader, "vt" ) == 0 ){
            // u, v: texture coordinates
            glm::vec2 uv;
            fscanf(file, "%f %f\n", &uv.x, &uv.y );
            uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
            texs.push_back(uv);
        }else if ( strcmp( lineHeader, "vn" ) == 0 ){
            // normal coordinates (ignored)
            glm::vec3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
            //normals.push_back(normal);

        }else if ( strcmp( lineHeader, "f" ) == 0 ){
            // face indices
            tri t;
            int matches = fscanf(file, "%d/%d %d/%d %d/%d\n", &(t.index_xyz[0]), &(t.index_uv[0]), &(t.index_xyz[1]), &(t.index_uv[1]), &(t.index_xyz[2]), &(t.index_uv[2]) );
            if (matches != 6){
                printf("File can't be read by our simple parser :-( Try exporting with other options\n");
                return false;
            }
            tris.push_back(t);
            num_tris++;

        }else{
            // Probably a comment, eat up the rest of the line
            char stupidBuffer[1000];
            fgets(stupidBuffer, 1000, file);
        }

    }

    return true;

}
