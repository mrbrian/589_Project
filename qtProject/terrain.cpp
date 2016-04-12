#include "terrain.h"
#include "tests.h"

vector<TreeSimulation *> *Terrain::addTreesToTerrain(std::vector<QVector3D> controlPoints)
{
    m_trees.clear(); //NOTE: remove trees every time this is run for Performance
    Simulation * sim = new Simulation();
    std::vector<TreeSimulation *> toMake = sim->simulate(controlPoints);
    std::cout << "Made " << toMake.size() << " new trees \n";
    //turn those into rev surfaces

/*
    for (int i = 0; i < m_controlMesh.size(); i++)
    {
        QVector3D p = m_controlMesh[i];
        std::cout << p[0] << ",asd " << p[1] << ", " << p[2] << endl;
    }
*/
    std::vector<TreeSimulation *> *result = new std::vector<TreeSimulation *>();

    for (int i = 0; i < toMake.size(); i++)
    {
        result->push_back(toMake[i]);
    }
    delete sim;
    return result;
}

int Terrain::get_meshWidth()
{
    return m_meshWidth;
}

int Terrain::get_meshHeight()
{
    return m_meshHeight;
}

std::vector<TreeSimulation *> Terrain::simulateTreeGrowth(std::vector<QVector3D> controlPoints)
{

}

// x, z = not normalized values
float Terrain::get_y_height(float x, float z)
{
    return get_y_height(x, z, m_meshWidth, m_meshHeight, m_controlMesh);
}

// x, z = not normalized values
float Terrain::get_y_height(float x, float z, int width, int height, vector<QVector3D> pts)
{
    /*if (x > 1)
        throw std::invalid_argument( "x > 1" );
    if (z > 1)
        throw std::invalid_argument( "z > 1" );*/
    int vert_width = width;

    int x_min = floor(x);
    int x_max = x_min + 1;

    int z_min = floor(z);
    int z_max = z_min + 1;

    int idx1 = z_min * vert_width + x_min;
    int idx2 = z_min * vert_width + x_max;
    int idx3 = z_max * vert_width + x_min;
    int idx4 = z_max * vert_width + x_max;

    QVector3D a = pts[idx1];
    QVector3D b = pts[idx2];
    QVector3D c = pts[idx4];
    QVector3D d = pts[idx3];

    // index of triangles  vert Idx 1, +1

    float x_pos = x;
    float z_pos = z;

    float xOffs = x_pos - x_min;
    float zOffs = z_pos - z_min;

    bool topTri = (z_pos) < (z_max - xOffs);    // is it above or below the diagonal?
    int triIdx = -1;

    if (topTri)
    {
//        triIdx = 2 * idx1;


        QVector3D xDir = b - a;
        QVector3D zDir = d - a;

        QVector3D final_pos = a + ((xOffs * xDir) + (zOffs * zDir));

        return final_pos[1];
    }
    else
    {
    //    triIdx = 2 * idx1 + 1;

        QVector3D xDir = d - c;
        QVector3D zDir = b - c;

        QVector3D final_pos = c + (((1 - xOffs) * xDir) + ((1 - zOffs) * zDir));

        return final_pos[1];
    }
}

QImage *Terrain::getImage()
{
    return m_uvImage;
}

int low_res_modifier = 30;

ObjModel *Terrain::getObjModel()
{
    ObjModel *result = new ObjModel();
    result->num_tris = m_numQuads * 2;
    result->num_xyz = m_controlMesh.size();

    for (int i = 0; i < m_controlMesh.size(); i++ )
    {
        result->m_vertices.push_back(
                    vec3(m_controlMesh[i][0],
                         m_controlMesh[i][1],
                         m_controlMesh[i][2]));
    }

    for (int j = 0; j < m_meshHeight; j++ )
    {
        for (int i = 0; i < m_meshWidth; i++)
        {
            result->texs.push_back(
                        vec2((float)i / m_meshWidth,
                             (float)j / m_meshHeight));
        }
    }

    for (int j = 0; j < m_meshHeight - 1; j++ )
    {
        for (int i = 0; i < m_meshWidth - 1; i++)
        {
            GLuint a,b,c,d;

            a = j * m_meshWidth + i;
            b = j * m_meshWidth + i + 1;
            c = (j + 1) * m_meshWidth + i + 1;
            d = (j + 1) * m_meshWidth + i;

            tri t1, t2;

            t1.index_xyz[0] = a;
            t1.index_xyz[1] = d;
            t1.index_xyz[2] = b;

            t2.index_xyz[0] = d;
            t2.index_xyz[1] = c;
            t2.index_xyz[2] = b;

            t1.index_uv[0] = a;
            t1.index_uv[1] = d;
            t1.index_uv[2] = b;

            t2.index_uv[0] = d;
            t2.index_uv[1] = c;
            t2.index_uv[2] = b;

            result->tris.push_back(t1);
            result->tris.push_back(t2);
        }
    }
    return result;
}

void Terrain::createControlMesh(QImage * heightMap, float heightToAreaScale)
{
    m_meshWidth = heightMap->width();
    m_meshHeight = heightMap->height();

    for (int j = 0; j < m_meshHeight; j++ )
    {
        for (int i = 0; i < m_meshWidth; i++)
        {
            QColor pixel = QColor(heightMap->pixel(i,j));
            float height = (float)pixel.red() / 255.0f * heightToAreaScale;// * heightToAreaScale;

            m_controlMesh.push_back(QVector3D((float)i, height, (float)j));

            //std::cout << i << ", height " << height << ", " << j << endl;
            if(i % low_res_modifier== 0 && j % low_res_modifier == 0)
            {
                m_selectableControlMesh.push_back(QVector3D((float)i + (i*j*0.000001), height, (float)j + (i*j*0.00001)));
            }

            //m_controlMesh.push_back(QVector3D((float)i / (float)m_meshWidth, height,(float)j / (float)m_meshHeight));
        }
    }


}

void Terrain::populateVAO()
{
    //initialize sizing data
    m_numQuads = (m_meshWidth-1) * (m_meshHeight-1);
    m_numVerts = m_controlMesh.size();

    m_sizeVerts = m_numQuads * 4 * 3;
    m_sizeUVs = m_numQuads * 4 * 2;
    m_sizeColors = m_numQuads * 4 * 3;
    m_sizeNorms = m_numQuads * 4 * 3;

    //create arrays
    m_verts = new float[m_sizeVerts];
    m_colors = new float[m_sizeColors];
    m_uvs = new float[m_sizeUVs];
    m_faceNorms = new float[m_sizeNorms];

    //make mesh white for now
    QVector3D faceColor(1,1,1);

    //fill arrays
    for (int j = 0; j < m_meshHeight - 1; j++ )
    {
        int heightOffset = j * (m_meshWidth - 1) * 4;

        for (int i = 0; i < m_meshWidth - 1; i++)
        {
            int widthOffset = i * 4;
            int offset = heightOffset * 3 + widthOffset * 3;
            /*  a----------b
             *  ------------
             *  ------------
             *  d----------c
             */
            QVector3D a,b,c,d;
            a = m_controlMesh[j * m_meshWidth + i];
            d = m_controlMesh[j * m_meshWidth + i + 1];
            c = m_controlMesh[(j + 1) * m_meshWidth + i + 1];
            b = m_controlMesh[(j + 1) * m_meshWidth + i];

            faceNormal = QVector3D::normal(b-a, d-a);

            //fill vertice array
            m_verts[offset + 0 ] = a.x();
            m_verts[offset + 1 ] = a.y();
            m_verts[offset + 2 ] = a.z();
            m_verts[offset + 3 ] = b.x();
            m_verts[offset + 4 ] = b.y();
            m_verts[offset + 5 ] = b.z();
            m_verts[offset + 6 ] = c.x();
            m_verts[offset + 7 ] = c.y();
            m_verts[offset + 8 ] = c.z();
            m_verts[offset + 9 ] = d.x();
            m_verts[offset + 10] = d.y();
            m_verts[offset + 11] = d.z();

            //fill color and normal arrays. same color and normal for all points of the quad
            for (int c = 0; c < 4; c++)
            {

                m_colors[offset + 0] = faceColor.x();
                m_colors[offset + 1] = faceColor.y();
                m_colors[offset + 2] = faceColor.z();

                m_faceNorms[offset + 0] = faceNormal.x();
                m_faceNorms[offset + 1] = faceNormal.y();
                m_faceNorms[offset + 2] = faceNormal.z();

                offset+=3;
            }

            //fill uv coords
            offset = heightOffset * 2 + widthOffset * 2;
            m_uvs[offset + 0] = i / (float)(m_meshWidth - 1);
            m_uvs[offset + 1] = j / (float)(m_meshHeight - 1);
            m_uvs[offset + 2] = (i + 1) / (float)(m_meshWidth - 1);
            m_uvs[offset + 3] = j / (float)(m_meshHeight - 1);
            m_uvs[offset + 4] = (i + 1) / (float)(m_meshWidth - 1);
            m_uvs[offset + 5] = (j + 1) / (float)(m_meshHeight - 1);
            m_uvs[offset + 6] = i / (float)(m_meshWidth - 1);
            m_uvs[offset + 7] = (j + 1) / (float)(m_meshHeight - 1);
        }
    }

}

int Terrain::getControlMeshSize()
{
    return m_selectableControlMesh.size();
}
