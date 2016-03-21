#include "terrain.h"

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
