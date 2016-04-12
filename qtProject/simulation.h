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

    TreeSimulation(QVector2D origin, int type, float scale)    {
        m_simScale = scale;
        //std::cout << m_simScale<< endl;
        m_origin = origin;

        m_type = type == 0 ? eAspen : eBalsam;
        m_age = m_trunkDiameter = m_height = m_crownDiameter = 0;
        m_toDie = false;
        //std::cout<< m_origin.x() <<"," << m_origin.y() << endl;

        assignGrowthAttributes();
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

            m_maxCrownDiameter /=2.0f;
            m_maxTrunkDiameter /=2.0f;


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
    float getCrownRadius()
    {
        if (m_simScale == 0)
        {
            std::cout << "error\n";
            return 0;
        } else
             return m_crownDiameter/ m_simScale;
    }

    QVector2D getOrigin() {return m_origin;}
    float getHeight() { return m_height / m_simScale; }
    float getTrunkRadius() { return m_maxTrunkDiameter /m_simScale; }

private:

    float m_simScale;
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

#define RANGE_CONVERSION_SCALAR_AMOUNT 100000
class Simulation
{
public:

    Simulation(float terrainScale = 100, int arrayDimensions = 10000)
    {

//        std::cout << "created Simulation \n";
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
        delete[] mGrowthArray;



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
        runSimulation(500 + rand() % 60);
        return mGrowingTrees;

    }

private://members
    int * mGrowthArray;
    int mArrayDimensions;
    float mTerrainScale;
    float mBoundingXmin;
    float mBoundingYmin;
    int mBoundingXRange;
    int mBoundingYRange;

    std::vector<QVector2D> mNewTrees;
    std::vector<TreeSimulation *> mGrowingTrees;
private:
    void throwSeeds(int numSeeds)
    {
        for (uint i = 0; i < numSeeds; i++)
        {
            float x = mBoundingXmin + (float)(rand() % mBoundingXRange) / RANGE_CONVERSION_SCALAR_AMOUNT;
            float y = mBoundingYmin + (float)(rand() % mBoundingYRange) / RANGE_CONVERSION_SCALAR_AMOUNT;
            //std::cout << x <<"," << y << endl;
            mNewTrees.push_back(
               QVector2D(x,y));


        }
    }

    void cullNewSeeds()
    {
        for (uint i = 0; i < mNewTrees.size(); i++)
            {
                int index = (int)(mNewTrees[i].y() * (mArrayDimensions) * (mArrayDimensions) + mNewTrees[i].x() * (mArrayDimensions));

                if (mGrowthArray[index] == 2)

                {
                    mGrowingTrees.push_back(new TreeSimulation(mNewTrees[i], 0, mTerrainScale));

                } else {
                    //std::cout << ".. Failed\n";
                }
            }
            mNewTrees.clear();

/*            for (unsigned int i = 0; i < mGrowingTrees.size(); i++)
            {
                std::cout << "growing trees: " << mGrowingTrees[i]->getOrigin().x() << "," << mGrowingTrees[i]->getOrigin().y() << endl;

            }*/
    }

    void runSimulation(float numIterations)
    {

        //std::cout << "Running simulation...."<< std::endl;
        while (numIterations-- > 0)
        {
            throwSeeds(4500);
            //std::cout << "culled seeds\n";
            cullNewSeeds();
            //std::cout << "growing trees\n";
            float yearsPassed = 1;
            for (unsigned int i = 0; i < mGrowingTrees.size(); i++)
            {
                mGrowingTrees[i]->grow(yearsPassed);
                for (unsigned int j = 0; j < mGrowingTrees.size(); j++)
                {
                    if (i != j){
                        double distance = (mGrowingTrees[i]->getOrigin() - mGrowingTrees[j]->getOrigin()).length();
                        //std::cout << distance << std::endl;
                        //std::cout<< "tree 1 radius : " << mGrowingTrees[i]->getCrownRadius(mTerrainScale) << " tree 2 radius :" << mGrowingTrees[j]->getCrownRadius(mTerrainScale) << std::endl;
                        if ((mGrowingTrees[i]->getCrownRadius() + mGrowingTrees[j]->getCrownRadius()) > distance )
                        {
                            mGrowingTrees[mGrowingTrees[i]->getCrownRadius() > mGrowingTrees[j]->getCrownRadius() ? j : i]->setToDie();
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

        float boundingXmin = 1;
        float boundingXmax = 0;
        float boundingYmin = 1;
        float boundingYmax = 0;

        for (uint i = 0 ; i < points.size(); i++)
        {
//                std::cout << points[i].x() << "," << points[i].z() << std::endl;
            QVector2D  point1 = QVector2D(points[i].x(), points[i].z()), point2;
            boundingXmin = point1.x() < boundingXmin ? point1.x() : boundingXmin;
            boundingXmax = point1.x() > boundingXmax ? point1.x() : boundingXmax;

            boundingYmin = point1.y() < boundingYmin ? point1.y() : boundingYmin;
            boundingYmax = point1.y() > boundingYmax ? point1.y() : boundingYmax;


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
                    //std::cout << "trace point \n";
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
        mBoundingXmin = boundingXmin;
        mBoundingYmin = boundingYmin;
        mBoundingXRange = (boundingXmax - boundingXmin) * RANGE_CONVERSION_SCALAR_AMOUNT;
        mBoundingYRange = (boundingYmax - boundingYmin) * RANGE_CONVERSION_SCALAR_AMOUNT;
    }

    void fillBoundingVolume()
    {
        std::cout << "filling bounding volume \n";
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
