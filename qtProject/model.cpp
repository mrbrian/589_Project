#include "model.h"

#define NORM_LINE_LENGTH 0.1

Model::~Model()
{
    delete[] verts;
    delete[] face_normals;
    delete[] vert_normals;
    delete[] uvs;
    delete[] colours;
    delete[] lines_face_normals;
    delete[] lines_vert_normals;
}

// this will return a scale value that will fit the model inside a unit cube
float Model::getNormalizeScale()
{
    QVector3D pos = getCenter();            // take position offset into account
    float max = fabs(bounds[0] - pos[0]);

    for (int i = 0; i < 6; i++)
    {
        float val = fabs(bounds[i] - pos[i % 3]);
        if (max < val)
            max = val;
    }

    return (1.0 / max);
}

// resets the local transform (if the model has no parent then use normalize scale & center position; otherwise just reset to identity)
void Model::resetLocalTransform()
{
    transform_ = QMatrix4x4();

    if (parent) // model has a parent so dont scale/translate it  (it will inherit parent transform)
    {
        scale = 1;
        return;
    }

    // so model is not a submodel
    QVector3D pos = getCenter();

    scale = getNormalizeScale();
    transform_.scale(scale);        // scale to fit inside a unit cube
    transform_.translate(-pos);     // center it's position
}

// find the min/max vertex bounds of model and store in bounds[] member
void Model::updateBounds()
{
    for (int i = 0; i < 6; i++)     // initialize min/max
        bounds[i] = verts[i];

    for (int i = 0; i < numVerts; i++)
    {
        for (int axis = 0; axis < 3; axis++)
        {
            float val = verts[i * 3 + axis];
            switch(axis)
            {
            case 0:
                if (val < bounds[0])    // check min x
                    bounds[0] = val;
                if (val > bounds[3])    // check max x
                    bounds[3] = val;
            break;
            case 1:

                if (val < bounds[1])    // check min y
                    bounds[1] = val;
                if (val > bounds[4])    // check max x
                    bounds[4] = val;
            break;
            case 2:

                if (val < bounds[2])    // check min z
                    bounds[2] = val;
                if (val > bounds[5])    // check max x
                    bounds[5] = val;
            break;
            }
        }
    }
}

// copy the uvs from the source ObjModel into array for VBO
float * Model::readUVs(ObjModel *source, long *size)
{
    int numFloats = source->num_tris * 3 * 2;       // 2 float values per vertex

    *size = numFloats * sizeof(float);

    float *uv_ = new float[numFloats];

    for (int i = 0; i < source->num_tris; i++)       // 3 verts a tri, 3 floats a vert
    {
        for (int vertNum = 0; vertNum < 3; vertNum++)
        {
            int vIdx = source->tris[i].index_uv[vertNum];
            for (int j = 0; j < 2; j++)
            {
                float val = source->texs[vIdx][j];
                int idx = i * 6 + vertNum * 2 + j;

                uv_[idx] = val;
            }
        }
    }
    return uv_;
}

// copy the vertex data from the ObjModel into array for VBO
float * Model::readVerts(ObjModel *source, long int *size)
{
    int numFloats = source->num_tris * 9;

    *size = numFloats * sizeof(float);

    float *v_ = new float[numFloats];

    for (int i = 0; i < source->num_tris; i++)       // 3 verts a tri, 3 floats a vert
    {
        for (int vertNum = 0; vertNum < 3; vertNum++)
        {
            float vIdx = source->tris[i].index_xyz[vertNum];
            for (int j = 0; j < 3; j++)
            {
                float val = source->m_vertices[vIdx][j];
                int idx = i * 9 + vertNum * 3 + j;

                v_[idx] = val;
            }
        }
    }
    numVerts = source->num_tris * 3;
    numTris = source->num_tris;
    return v_;
}

// generate a point list for all the face normal lines
float * Model::generateFaceNormalLines(long *size)
{
    float *v_ = new float[numTris * 3 * 2];     // allocate 2 verts (3 floats) for each face

    *size = numTris * 3 * 2 * sizeof(float);

    for (int tri = 0; tri < numTris; tri++)     // for each triangle
    {
        QVector3D p1, p2;
        for (int j = 0; j < 9; j++)         // read in the triangles 3 verts
        {
            p1[j % 3] += verts[tri * 9 + j];
        }
        p1 /= 3;                            // get the triangle center point

        // 9 floats per triangle
        int normIdx = tri * 3 * 3;
        QVector3D vec = QVector3D(face_normals[normIdx],
                                  face_normals[normIdx + 1],
                                  face_normals[normIdx + 2]);

        // compensate for the parent's scale
        vec = vec / getWorldScale() * NORM_LINE_LENGTH;

        p2 = p1 + vec;  // get 2nd point

        int pIdx     = tri * 3 * 2;     // will need 2 verts with 3 floats for each triangle

        v_[pIdx]     = p1[0];
        v_[pIdx + 1] = p1[1];
        v_[pIdx + 2] = p1[2];

        v_[pIdx + 3] = p2[0];
        v_[pIdx + 4] = p2[1];
        v_[pIdx + 5] = p2[2];
    }
    return v_;
}

// generate a point list for all the vertex normal lines
float * Model::generateVertexNormalLines(long *size)
{
    float *v_ = new float[numVerts * 3 * 2];        // allocate 2 verts (3 floats) for each vert

    *size = numVerts * 3 * 2 * sizeof(float);

    for (int vert = 0; vert < numVerts; vert++)     // for each triangle
    {
        QVector3D p1, p2;

        p1 = QVector3D(verts[vert * 3],
                       verts[vert * 3 + 1],
                       verts[vert * 3 + 2]);

        // 3 floats per vert
        int normIdx = vert * 3;
        QVector3D vec = QVector3D(vert_normals[normIdx],
                                  vert_normals[normIdx + 1],
                                  vert_normals[normIdx + 2]);

        // compensate for the parent's scale
        vec = vec / getWorldScale() * NORM_LINE_LENGTH;

        p2 = p1 + vec;  // get 2nd point

        int pIdx = vert * 3 * 2;     // will need 2 verts with 3 floats for each triangle

        v_[pIdx]     = p1[0];
        v_[pIdx + 1] = p1[1];
        v_[pIdx + 2] = p1[2];

        v_[pIdx + 3] = p2[0];
        v_[pIdx + 4] = p2[1];
        v_[pIdx + 5] = p2[2];
    }
    return v_;
}

// get the world transform (includes the parent(s) transforms)
QMatrix4x4 Model::getWorldTransform()
{
    if (parent)
        return parent->getWorldTransform() * transform_;
    else
        return transform_;
}

// get the local transform (doesn't include parent(s))
QMatrix4x4 Model::getLocalTransform()
{
    return transform_;
}

// sets the local transform
void Model::setLocalTransform(QMatrix4x4 m)
{
    transform_ = QMatrix4x4(m);
}

// finds the center point of the model, by averaging the min/max vertex bounds
QVector3D Model::getCenter()
{
    return QVector3D(
                (bounds[0] + bounds[3]) / 2,
                (bounds[1] + bounds[4]) / 2,
                (bounds[2] + bounds[5]) / 2
            );
}

// returns a list of vertex normals for the ObjModel (but not in VBO format- the vertices are shared between faces)
float * Model::getObjectVertexNormals(ObjModel *source)
{
    float *objNormals     = new float[source->num_xyz * 3]; // store the normals per obj vertex

    for (int i = 0; i < source->num_xyz * 3; i++)   // initialize normal array
    {
        objNormals[i] = 0;
    }

    // for each triangle..
    for (int tri = 0; tri < numTris; tri++)
    {
        int triOffset = tri * 9;        // 3 verts a tri, 3 floats a vert

        // get the vertices of the triangle..
        GLuint vertIndices[3] = {
                        source->tris[tri].index_xyz[0],
                        source->tris[tri].index_xyz[1],
                        source->tris[tri].index_xyz[2]
                    };

        // load the vertices into QVector3Ds..
        QVector3D p[3];
        for (int vert = 0; vert < 3; vert++)        // get the 3 vertices for this tri
        {
            int idx = triOffset + vert * 3;
            p[vert] = QVector3D(verts[idx], verts[idx + 1], verts[idx + 2]);
        }

        // get 2 vectors parallel to the triangle..
        QVector3D v[2] = {p[0] - p[1], p[0] - p[2]};
        // calculate the normal from those vectors
        QVector3D normal = QVector3D::crossProduct(v[0], v[1]);
        normal.normalize();

        // then add the normal to the 3 vertices of the face
        for (int vert = 0; vert < 3; vert++)
        {
            int vertIdx = vertIndices[vert] - 1;

            objNormals[vertIdx * 3]     += normal[0];
            objNormals[vertIdx * 3 + 1] += normal[1];
            objNormals[vertIdx * 3 + 2] += normal[2];
        }
    }

    // now normalize all of them and save
    for (int vert = 0; vert < source->num_xyz; vert++)
    {
        QVector3D normal;

        for (int i = 0; i < 3; i++)
        {
            normal[i] = objNormals[vert * 3 + i];
        }
        normal.normalize();     // make the normal unit length

        int idx = vert * 3;

        objNormals[idx]     = normal[0];    // store into objNormals array
        objNormals[idx + 1] = normal[1];
        objNormals[idx + 2] = normal[2];
    }

    return objNormals;
}

// calculate smooth vertex normals by adding up and then averaging the face normals
float * Model::calcVertexNormals(ObjModel *source, long *size)
{
    float *result = new float[numTris * 9];                 // final list of vertex normals for the VBO (all vertices are separated)

    *size = numTris * 9 * sizeof(float);            // update referenced size parameter

    for (int i = 0; i < numTris * 9; i++)           // initialize result array
    {
        result[i] = 0;
    }

    float *objNormals = getObjectVertexNormals(source);     // get the ObjModel vertex normals

    // now take the normals, separate each vertex then store into result array (VBO format)
    for (int tri = 0; tri < numTris; tri++)
    {
        GLuint vertIndices[3] = {source->tris[tri].index_xyz[0],
                              source->tris[tri].index_xyz[1],
                              source->tris[tri].index_xyz[2]};

        // copy normal value from objNormals[vertIdx] to result..
        for (int vert = 0; vert < 3; vert++)
        {
            int objVertIdx = vertIndices[vert] - 1;
            int resultIdx = tri * 9 + vert * 3;

            QVector3D v = QVector3D(objNormals[objVertIdx * 3], objNormals[objVertIdx * 3 + 1], objNormals[objVertIdx * 3 + 2]);

            result[resultIdx]     = v[0];
            result[resultIdx + 1] = v[1];
            result[resultIdx + 2] = v[2];
        }
    }
  //  delete[] objNormals;    // free up memory

    return result;
}

// gets the world scale (includes scale of parent(s))
float Model::getWorldScale()
{
    if (!parent)
        return scale;

    return parent->getWorldScale() * scale;
}

// calculate the surface normals for each face and save a copy for each vertex
float * Model::calcFaceNormals(long *size)
{
    float *n_ = new float[numTris * 9];

    *size = numTris * 9 * sizeof(float);

    // generate surface normal for each triangle and save it for each face vertex
    for (int tri = 0; tri < numTris; tri++)       // 3 verts a tri, 3 floats a vert
    {
        int triOffset = tri * 9;

        QVector3D p[3];
        for (int vert = 0; vert < 3; vert++)        // get the 3 vertices for this tri
        {
            int idx = triOffset + vert * 3;
            p[vert] = QVector3D(verts[idx], verts[idx + 1], verts[idx + 2]);
        }

        QVector3D v[2] = {p[0] - p[1], p[0] - p[2]};            // get 2 vectors
        QVector3D normal = QVector3D::crossProduct(v[0], v[1]); // get cross prod
        normal.normalize();

        for (int vert = 0; vert < 3; vert++)
        {
            int idx = triOffset + vert * 3;
            n_[idx] = normal[0];                // save normals for each vert of the triangle
            n_[idx + 1] = normal[1];
            n_[idx + 2] = normal[2];
            gNormal = normal;
        }
    }

    return n_;
}
/*
Model *Model::fromQuadList(vector<QVector3D*> *verts)
{
    Model *result;

    sizeVerts           = -1;
    sizeColours         = -1;
    sizeUvs             = -1;
    sizeVertNormals     = -1;
    sizeFaceNormals     = -1;
    sizeVertNormalLines = -1;
    sizeFaceNormalLines = -1;

    int numTris = verts->size() / 4;

    // so readVerts of quads
    // gen UV
    for (int i = 0; i < numQuads; i++)
    {

    }
}*/

Model::Model(ObjModel *m, Model *par)
{
    sizeVerts           = -1;
    sizeColours         = -1;
    sizeUvs             = -1;
    sizeVertNormals     = -1;
    sizeFaceNormals     = -1;
    sizeVertNormalLines = -1;
    sizeFaceNormalLines = -1;

    scale               = 1;
    parent              = par;                      // assign parent
    verts               = readVerts(m, &sizeVerts); // read in vertices
    uvs                 = readUVs(m, &sizeUvs);     // read in uvs

    // initialize colours
    int numFloats = m->num_tris * 9;
    float *c_ = new float[numFloats];

    for (int i = 0; i < numFloats; i += 3)  // initialize colours as white
    {
        c_[i]   = 1;
        c_[i+1] = 1;
        c_[i+2] = 1;
    }
    sizeColours = numFloats * sizeof(float);
    colours = c_;

    updateBounds();         // determine min/max bounds
    resetLocalTransform();  // initialize transform

    // calculate face/vertex normals
    face_normals        = calcFaceNormals(&sizeFaceNormals);
    vert_normals        = calcVertexNormals(m, &sizeVertNormals);

    // then calculate the normal lines
    lines_face_normals  = generateFaceNormalLines(&sizeFaceNormalLines);
    lines_vert_normals  = generateVertexNormalLines(&sizeVertNormalLines);
}

double Model::findIntersection(Ray ray)
{

        QVector3D ray_direction = ray.getRayDirection();
        QVector3D normal = gNormal;
        //Check whether the plane is parrell to the ray
        double a = ray_direction.dotProduct(ray_direction, normal);

        //parrell
        if (a == 0)
        {
            return -1;
        }
        else
        {
            //Normal dot (Ray Origin + (normal * distance))
            ray.getRayOrigin() += normal;
            float b = normal.dotProduct(normal, ray.getRayOrigin());
            return -1*b/a;
        }
}
