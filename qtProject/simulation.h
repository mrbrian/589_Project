#ifndef SIMULATION_H
#define SIMULATION_H

#pragma once
#include "QVector2D"
#include "QVector3D"
#include <vector>

class TreeSimulation
{
public:

    enum TreeType
    {
        eAspen,
        eBalsam,
    };

    TreeSimulation(QVector2D origin, TreeType type)
    {
        m_origin = origin;
        m_type = type;
        m_age = m_trunkDiameter = m_height = m_crownDiameter = 0;
        m_toDie = false;

        assignGrowthAttributes();

    }

    TreeSimulation(QVector2D origin, int type)
    {
       TreeSimulation(origin, type == 0 ? eAspen : eBalsam);

    }

    ~TreeSimulation()
    {

    }

    void assignGrowthAttributes()
    {
        float range;
        switch(m_type)
        {

            case (eAspen):
            {
                // height 20-25m
                range = (rand() % 6);
                m_maxHeight = 20 + range;
                //trunk diameter .2 - .8 meters
                range = (rand() % 7) / 10.f;
                m_maxTrunkDiameter = .2 + range;
                //age 40-150 yrs
                range = (rand() % 111);
                m_maxAge = 100 + range;
                //crown width 6-9 meters in diameter
                range = (rand() % 4);
                m_maxCrownDiameter = 6 + range;
                break;
            }
            case (eBalsam):
            {
                // height 15-21m
                range = (rand() % 7);
                m_maxHeight = 15 + range;
                //trunk diameter .3 - .8 meters
                range = (rand() % 6) / 10.f;
                m_maxTrunkDiameter = .3 + range;
                //age 150-200 yrs
                range = (rand() % 51);
                m_maxAge = 150 + range;
                //crown width 3.3-6.7 meters in diameter
                range = (rand() % 35) / 5.f;
                m_maxCrownDiameter = 3.3 + range;
                break;
            }

        }
    }

    void grow(float years)
    {
        float yearsPassed = years; // dtMillis * 10;
        m_height += m_maxHeight / m_maxAge * yearsPassed;
        m_crownDiameter += m_maxCrownDiameter / m_maxAge * yearsPassed;
        m_trunkDiameter += m_maxTrunkDiameter / m_maxAge * yearsPassed;
        m_age+= (int)yearsPassed;
        if (m_age > m_maxAge)
            setToDie();
    }

    void setToDie() { m_toDie = true; }
    bool isAlive() {return !m_toDie;}
    float getCrownRadius(float scale) {return m_crownDiameter / 2 / scale ; }
    QVector2D getOrigin() {return m_origin;}

private:

    QVector2D m_origin;
    TreeType m_type;
    float m_height;
    float m_maxHeight;

    float m_trunkDiameter;
    float m_maxTrunkDiameter;

    float m_crownDiameter;
    float m_maxCrownDiameter;

    float m_age;
    float m_maxAge;

    bool m_toDie;


};


class Simulation
{
public:

    Simulation(float terrainScale = 100, int arrayDimensions = 100)
    {
        std::cout << "created Simulation \n";
        mGrowthArray = new int[arrayDimensions * arrayDimensions];
        for (int i = 0; i < arrayDimensions * arrayDimensions; i++)
        {
            mGrowthArray[i] = 0;
        }
        mArrayDimensions = arrayDimensions;
        mTerrainScale = terrainScale;
    }

    ~Simulation()
    {

        //delete mGrowthArray;
        //delete[] mGrowthArray;

        for (uint i = 0; i < mGrowingTrees.size(); i++)
        {
            delete mGrowingTrees[i];
        }
        mGrowingTrees.clear();
    }

    std::vector<TreeSimulation *> simulate(std::vector<QVector3D> controlPoints)
    {
        std::cout << "Simulating tree growth \n";
        traceBoundingVolume(controlPoints);
        fillBoundingVolume();
        runSimulation(60 + rand() % 60);
        return mGrowingTrees;

    }

private://members
    int * mGrowthArray;
    int mArrayDimensions;
    float mTerrainScale;
    std::vector<QVector2D> mNewTrees;
    std::vector<TreeSimulation *> mGrowingTrees;
private:
    void throwSeeds(int numSeeds)
    {
        for (uint i = 0; i < numSeeds; i++)
        {
            mNewTrees.push_back(
               QVector2D((float)(rand() % (mArrayDimensions-1)) / mArrayDimensions, (float)(rand() % (mArrayDimensions- 1)) / mArrayDimensions));


        }
    }

    void cullNewSeeds()
    {
        for (uint i = 0; i < mNewTrees.size(); i++)
            {
                int index = (int)(mNewTrees[i].y() * (mArrayDimensions) * (mArrayDimensions) + mNewTrees[i].x() * (mArrayDimensions));

                if (mGrowthArray[index] == 2)

                {

                    mGrowingTrees.push_back(new TreeSimulation(mNewTrees[i], rand() % 2));

                } else {
                    //std::cout << ".. Failed\n";
                }
            }
            mNewTrees.clear();
    }

    void runSimulation(float numIterations)
    {

        //std::cout << "Running simulation...."<< std::endl;
        while (numIterations-- > 0)
            {
                throwSeeds(3000);
                //std::cout << "culled seeds\n";
                cullNewSeeds();
                //std::cout << "growing trees\n";
                float yearsPassed = 14;
                for (unsigned int i = 0; i < mGrowingTrees.size(); i++)
                {
                    mGrowingTrees[i]->grow(yearsPassed);
                    for (unsigned int j = 0; j < mGrowingTrees.size(); j++)
                    {
                        if (i != j){
                            float distance = (mGrowingTrees[i]->getOrigin() - mGrowingTrees[j]->getOrigin()).length();
                            //std::cout << distance << std::endl;
                            //std::cout<< "tree 1 radius : " << m_growingTrees[i]->getCrownRadius(SCALE) << " tree 2 radius :" << m_growingTrees[j]->getCrownRadius(SCALE) << std::endl;
                            if ((mGrowingTrees[i]->getCrownRadius(mTerrainScale) + mGrowingTrees[j]->getCrownRadius(mTerrainScale)) > distance )
                            {
                                mGrowingTrees[mGrowingTrees[i]->getCrownRadius(mTerrainScale) > mGrowingTrees[j]->getCrownRadius(mTerrainScale) ? j : i]->setToDie();
                            }
                        }

                    }
                }

                for (unsigned int i = 0; i < mGrowingTrees.size(); i++)
                {
                    if (!mGrowingTrees[i]->isAlive()){
                        //std::cout << "deleted a tree\n";
                        delete  mGrowingTrees[i];
                        mGrowingTrees.erase(mGrowingTrees.begin() + i);
                    }
                }

            }
            //std::cout << "Finished running simulation...."<< std::endl;
    }

    void traceBoundingVolume(std::vector<QVector3D> points)
    {
       // std::cout << "tracing bounding volume \n";
        if (points.size() < 2 ) return;
            for (uint i = 0 ; i < points.size(); i++)
            {
                QVector2D  point1 = QVector2D(points[i].x(), points[i].z()), point2;
                if (i == points.size() - 1)
                    point2 = QVector2D(points[0].x(), points[0].z());
                else
                    point2 =  QVector2D(points[i+1].x(), points[i+1].z());
                float x1 = point1.x()* mArrayDimensions,
                x2 = point2.x()* mArrayDimensions,
                y1 = point1.y()* mArrayDimensions,
                y2 = point2.y()* mArrayDimensions;



                if (point1.x() == point2.x())
                {
                    int lower, upper;
                    if (point1.y() < point2.y()){
                        lower = point1.y();
                        upper = point2.y();
                    } else {
                        lower = point2.y();
                        upper = point1.y();
                    }
                    for (int i = lower; i <= upper; i++)
                    {
                        mGrowthArray[i * mArrayDimensions + (int)point1.x()] = 1;
                    }
                } else
                {

                    const bool steep = (fabs(y2 - y1) > fabs(x2 - x1));
                    if(steep)
                    {
                        std::swap(x1, y1);
                        std::swap(x2, y2);
                    }

                    if(x1 > x2)
                    {
                        std::swap(x1, x2);
                        std::swap(y1, y2);
                    }

                    const float dx = x2 - x1;
                    const float dy = fabs(y2 - y1);

                    float error = dx / 2.0f;
                    const int ystep = (y1 < y2) ? 1 : -1;
                    int y = (int)y1;

                    const int maxX = (int)x2;

                    for(int x=(int)x1; x<maxX; x++)
                    {
                        if(steep)
                        {
                            mGrowthArray[x * mArrayDimensions + y] = 1;
                        }
                        else
                        {
                            mGrowthArray[y * mArrayDimensions + x] = 1;
                        }
                        error -= dy;
                        if(error < 0)
                        {
                            y += ystep;
                            error += dx;
                        }
                    }
                }
            }
    }

    void fillBoundingVolume()
    {
        //std::cout << "filling bounding volume \n";
        bool inVolume = false;
            bool inBorder = false;
            std::vector<int> tally;
            for (int j = 0; j < mArrayDimensions; j++)
            {
                inVolume = false;
                tally.clear();
                for (int i = 0; i < mArrayDimensions; i++)
                {
                    switch(mGrowthArray[j * mArrayDimensions + i])
                    {
                        case (0):
                            if(inBorder) //crossed over the border
                            {
                                inBorder = false;
                                if (inVolume){ //exited the volume
                                    inVolume = false;
                                } else {
                                    inVolume = true; //entering the volume
                                }
                            }
                            if (inVolume) tally.push_back(i);
                            break;
                        case (1):
                            if (!inBorder) //entered a border
                            {
                                inBorder = true;
                                for (int t = 0; t < tally.size(); t++)
                                {
                                    mGrowthArray[j * mArrayDimensions + tally[t]] = 2;
                                }
                                tally.clear();
                            }
                            break;
                        default:
                            break;
                    }


                }
            }
    }
};

#endif // SIMULATION_H
