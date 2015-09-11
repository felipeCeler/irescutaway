
#include "ires.h"
#include "kdtreeires.h"

#include <cstdio>
#include <cassert>
#include <cfloat>
#include <algorithm>
#include <utility>
#include <stdexcept>
#include <iomanip>

namespace ires
{
    bool ProgressOutput::m_reportProgress = false;

    Ires::Ires(bool reportProgress) : 
        m_header(),
        m_reportProgress(reportProgress),
        m_initialized(false),
        m_verticesSet(false),
        m_blocksSet(false),
        m_dynamicNames(),
        m_staticNames(),
        m_dynamicData(),
        m_staticData(),
        m_vertexList(),
        m_blockList(),
        m_blockIndexList(),
        m_tree(),
        m_treeBuilt(false)
    {
        // Do size checks
        if (!((sizeof(U8) == 1)
            && (sizeof(F32) == sizeof(U32))
            && (sizeof(D64) == sizeof(U64))))
        {
            std::printf("IRES: > !!! ERROR: IRES assumes 4-byte floats and 8-byte doubles,\nwhich is not the case on this machine!\n");
            throw std::runtime_error("IRES: > !!! ERROR: IRES assumes 4-byte floats and 8-byte doubles, which is not the case on this machine!");
        }

        ProgressOutput::setProgressReporting(m_reportProgress);

        init();
    }

    Ires::~Ires()
    {
        cleanup();
    }

    void Ires::reset()
    {
        cleanup();
        init();
    }

    bool Ires::initializeKdTree()
    {
        if ((!m_blocksSet) || (m_blockIndexList.size() < 3))
        {
            return false;
        }

        
        if (m_treeBuilt)
        {
            // KdTree is already initialized.
            return true;
        }

        m_tree.reset(new KdTree(m_blockIndexList, *this));

        if (m_tree != NULL )
        {
            m_treeBuilt = true;
            return true;
        }

        return false;
    }


        // getVertices should take in the block index, and assign a 8*3 array of doubles representing
    // vertices in the correct order (see this file for diagram). If the block is null, the returned
    // value should be FALSE, and TRUE otherwise
    void Ires::buildVertexBlockLists(U32 numI, U32 numJ, U32 numK, bool (*getVertices)(U32, F32*))
    {
        buildVertexBlockLists(numI, numJ, numK, std::function<bool(U32, F32*)>(getVertices));
    }

    void Ires::buildVertexBlockLists(U32 numI, U32 numJ, U32 numK, std::function<bool(U32, F32*)> getVertices)
    {
        if ((numI < 1) || (numJ < 1) || (numK < 1))
        {
            throw std::runtime_error("IRES: > !!! ERROR: Bad argument when calling buildVertexBlockLists()");
        }

       
        std::vector<U32> bList;

        std::size_t numBlocks = numI * numJ * numK;
        std::vector<int> bIndexList;

        F32 vertices[8*3];

        

        bList.reserve(numBlocks);
        bIndexList.reserve(numBlocks);

        U32 index = 0;
        U32 listIndex = 0;
        Vertex v;

        
        ProgressOutput() << "\nIRES: > Building vertex and block lists...\nIRES: >    % of blocks processed...";
        {
            std::vector<F32> vList;
            vList.reserve(numBlocks*3);
            std::unordered_set<Vertex, Vertex, Vertex> vertexSet(numBlocks*4);

            for (std::size_t i = 0; i < numBlocks; ++i)
            {
                if (!getVertices(i, vertices))
                {
                    bIndexList.push_back(-1);
                }
                else
                {
                    bIndexList.push_back(listIndex);
                    listIndex += 8;


                    // Set v0
                    v.v0[0] = vertices[0*3 + 0];
                    v.v0[1] = vertices[0*3 + 1];
                    v.v0[2] = vertices[0*3 + 2];

                    if (vertexSet.count(v) == 0)
                    {
                        v.index = index;
                        vertexSet.insert(v);

                        bList.push_back(index++);

                        vList.push_back(v.v0[0]);
                        vList.push_back(v.v0[1]);
                        vList.push_back(v.v0[2]);
                    }
                    else
                    {
                        bList.push_back((vertexSet.find(v))->index);
                    }


                    // Set v1
                    v.v0[0] = vertices[1*3 + 0];
                    v.v0[1] = vertices[1*3 + 1];
                    v.v0[2] = vertices[1*3 + 2];

                    if (vertexSet.count(v) == 0)
                    {
                        v.index = index;
                        vertexSet.insert(v);

                        bList.push_back(index++);

                        vList.push_back(v.v0[0]);
                        vList.push_back(v.v0[1]);
                        vList.push_back(v.v0[2]);
                    }
                    else
                    {
                        bList.push_back((vertexSet.find(v))->index);
                    }

                    // Set v2
                    v.v0[0] = vertices[2*3 + 0];
                    v.v0[1] = vertices[2*3 + 1];
                    v.v0[2] = vertices[2*3 + 2];

                    if (vertexSet.count(v) == 0)
                    {
                        v.index = index;
                        vertexSet.insert(v);

                        bList.push_back(index++);

                        vList.push_back(v.v0[0]);
                        vList.push_back(v.v0[1]);
                        vList.push_back(v.v0[2]);
                    }
                    else
                    {
                        bList.push_back((vertexSet.find(v))->index);
                    }

                    // Set v3
                    v.v0[0] = vertices[3*3 + 0];
                    v.v0[1] = vertices[3*3 + 1];
                    v.v0[2] = vertices[3*3 + 2];

                    if (vertexSet.count(v) == 0)
                    {
                        v.index = index;
                        vertexSet.insert(v);

                        bList.push_back(index++);

                        vList.push_back(v.v0[0]);
                        vList.push_back(v.v0[1]);
                        vList.push_back(v.v0[2]);
                    }
                    else
                    {
                        bList.push_back((vertexSet.find(v))->index);
                    }

                    // Set v4
                    v.v0[0] = vertices[4*3 + 0];
                    v.v0[1] = vertices[4*3 + 1];
                    v.v0[2] = vertices[4*3 + 2];

                    if (vertexSet.count(v) == 0)
                    {
                        v.index = index;
                        vertexSet.insert(v);

                        bList.push_back(index++);

                        vList.push_back(v.v0[0]);
                        vList.push_back(v.v0[1]);
                        vList.push_back(v.v0[2]);
                    }
                    else
                    {
                        bList.push_back((vertexSet.find(v))->index);
                    }

                    // Set v5
                    v.v0[0] = vertices[5*3 + 0];
                    v.v0[1] = vertices[5*3 + 1];
                    v.v0[2] = vertices[5*3 + 2];

                    if (vertexSet.count(v) == 0)
                    {
                        v.index = index;
                        vertexSet.insert(v);

                        bList.push_back(index++);

                        vList.push_back(v.v0[0]);
                        vList.push_back(v.v0[1]);
                        vList.push_back(v.v0[2]);
                    }
                    else
                    {
                        bList.push_back((vertexSet.find(v))->index);
                    }

                    // Set v6
                    v.v0[0] = vertices[6*3 + 0];
                    v.v0[1] = vertices[6*3 + 1];
                    v.v0[2] = vertices[6*3 + 2];

                    if (vertexSet.count(v) == 0)
                    {
                        v.index = index;
                        vertexSet.insert(v);

                        bList.push_back(index++);

                        vList.push_back(v.v0[0]);
                        vList.push_back(v.v0[1]);
                        vList.push_back(v.v0[2]);
                    }
                    else
                    {
                        bList.push_back((vertexSet.find(v))->index);
                    }

                    // Set v7
                    v.v0[0] = vertices[7*3 + 0];
                    v.v0[1] = vertices[7*3 + 1];
                    v.v0[2] = vertices[7*3 + 2];

                    if (vertexSet.count(v) == 0)
                    {
                        v.index = index;
                        vertexSet.insert(v);

                        bList.push_back(index++);

                        vList.push_back(v.v0[0]);
                        vList.push_back(v.v0[1]);
                        vList.push_back(v.v0[2]);
                    }
                    else
                    {
                        bList.push_back((vertexSet.find(v))->index);
                    }
                }

                if (m_reportProgress && !(i % 500000))
                {
                    ProgressOutput() << "\rIRES: > " << std::setiosflags(std::ios::fixed)
                        << std::setiosflags(std::ios::right) << std::setw(3) << std::setprecision(0)
                        << (100.0f * static_cast<float>(i) / static_cast<float>(numBlocks))
                        << "% of blocks processed...";
                }
            }
        
            vList.shrink_to_fit();

            ProgressOutput() << "\rIRES: > Done processing blocks.    \n";

            assert(bList.size() == listIndex);

            std::size_t numVertices = vertexSet.size();

            assert(vList.size() == numVertices * 3);


            if (!vList.empty())
            {
                ProgressOutput() << "IRES: > Applying generated vertex list...\n";

                setVertexList(vList);
            }

            ProgressOutput() << "IRES: > Freeing memory...\n";
        }
        
        if (!bList.empty())
        {
            ProgressOutput() << "IRES: > Applying generated block list...\n";

            setBlockList(bList, bIndexList, numI, numJ, numK);
        }

        ProgressOutput() << "IRES: > Done building vertex and block lists.\n";
    }

    /////////////////////////////////////////////////////////////////////////////////////
    /////////// Getters /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////

    // Convenience function.
    void Ires::getKNearestBlockIds(U32 index, U32 k, std::vector<U32>& blockIds) const
    {
        F32 point[3];

        getCentroid(index, point);

        getKNearestBlockIds(point, k, blockIds);
    }

    void Ires::getKNearestBlockIds(const F32 point[3], U32 k, std::vector<U32>& blockIds) const
    {
        if (!m_treeBuilt)
        {
            ProgressOutput() << "\nIRES: > ERROR !!!: Before to use <<getKNearestBlockIds>> you should initialize KD-Tree when calling getKNearestBlockIds()!";
            throw std::runtime_error("\nIRES: > ERROR !!!: Before to use <<getKNearestBlockIds>> you should initialize KD-Tree when calling getKNearestBlockIds()!");
        }

        blockIds = std::vector<U32>(0);
        blockIds.reserve(k);

        m_tree->getKNearestBlockIds(point, k, blockIds);
    }

    U32 Ires::getNumTotalBlocks( void ) const
    {
        return m_blockIndexList.size();
    }

    // Returns a copy of the header.
    IresHeader Ires::getHeader() const
    {
        return m_header;
    }

    // Returns -1 if the property is not found.
    int Ires::getDynamicPropertyIndex(const std::string& name) const
    {
        if (name.empty())
        {
            throw std::runtime_error("IRES: > !!! ERROR: Bad arguments when calling getDynamicPropertyIndex()");
        }

        for (U32 i = 0; i < m_header.numDynamicProps; i++)
        {
            if (m_dynamicNames[i].compare(name) == 0)
            {
                return i;
            }
        }

        return -1;
    }

    // Returns -1 if the property is not found.
    int Ires::getStaticPropertyIndex(const std::string& name) const
    {
        if (name.empty())
        {
            throw std::runtime_error("IRES: > !!! ERROR: Bad arguments when calling getStaticPropertyIndex()");
        }

        for (U32 i = 0; i < m_header.numStaticProps; i++)
        {
            if (m_staticNames[i].compare(name) == 0)
            {
                return i;
            }
        }

        return -1;
    }

    // Returns -1 if the property is not found.
    int Ires::getFacePropertyIndex(const std::string& name) const
    {
        if (name.empty())
        {
            throw std::runtime_error("IRES: > !!! ERROR: Bad arguments when calling getFacePropertyIndex()");
        }

        for (U32 i = 0; i < m_header.numFaceProps; i++)
        {
            if (m_faceNames[i].compare(name) == 0)
            {
                return i;
            }
        }

        return -1;
    }

    void Ires::getDynamicPropertyValues(U32 propertyIndex, int timestep, std::vector<F32>& values) const
    {
        if ( (propertyIndex > m_header.numDynamicProps) || ((timestep > -1) && (U32(timestep) > m_dynamicNumTimesteps[propertyIndex])) )
        {
            throw std::runtime_error("IRES: > !!! ERROR: Bad arguments passed to getDynamicPropValues()");
        }
        values.clear();

        values = std::vector<F32>(0);

        std::size_t numBlocks = m_blockIndexList.size();
        std::size_t countTimeSteps = 0;

        for( U32 i = 0 ; i < propertyIndex ; ++i ) countTimeSteps += m_dynamicNumTimesteps[i];

        if (timestep > -1)
        {

            std::size_t start = numBlocks * countTimeSteps + timestep * numBlocks ;
            std::size_t end   = start + numBlocks ;

            std::vector<F32>::const_iterator itStart = m_dynamicData.begin() + start ;
            std::vector<F32>::const_iterator itEnd   = m_dynamicData.begin() + end ;
            values.assign( itStart , itEnd );

        }
        else
        {
            std::size_t start = numBlocks * countTimeSteps ;
            std::size_t end   = start + numBlocks * m_dynamicNumTimesteps[propertyIndex] ;

            std::vector<F32>::const_iterator itStart = m_dynamicData.begin() + start ;
            std::vector<F32>::const_iterator itEnd   = m_dynamicData.begin() + end ;
            values.assign( itStart , itEnd );
        }
    }

    void Ires::getStaticPropertyValues(U32 propertyIndex, std::vector<F32>& values) const
    {
        if (propertyIndex > m_header.numStaticProps)
        {
            throw std::runtime_error("IRES: > !!! ERROR: Bad argument passed to getStaticPropValues()");
        }
        values.clear();

        std::size_t numBlocks = m_blockIndexList.size();

        std::size_t start = numBlocks * propertyIndex ;
        std::size_t end   =  start + numBlocks ;

        std::vector<F32>::const_iterator itStart = m_staticData.begin() + start ;
        std::vector<F32>::const_iterator itEnd = m_staticData.begin() + end ;
        values.assign( itStart , itEnd );
    }

    void Ires::getFacePropertyValues(U32 propertyIndex, std::vector<F32>& values) const
    {
        if (propertyIndex > m_header.numFaceProps)
        {
            throw std::runtime_error("IRES: > !!! ERROR: Bad argument passed to getFacePropertyValues()");
        }
        values.clear();
        std::size_t numBlocks = m_blockIndexList.size();

        std::size_t start =  6*numBlocks * propertyIndex ;
        std::size_t end   =  start + 6*numBlocks ;

        std::vector<F32>::const_iterator itStart = m_faceData.begin() + start ;
        std::vector<F32>::const_iterator itEnd = m_faceData.begin() + end ;
        values.assign( itStart , itEnd );
    }

    void Ires::getDynamicPropertyNames(std::vector<std::string>& names) const
    {
        if (m_header.numDynamicProps < 1)
        {
            names = std::vector<std::string>(0);
            return;
        }

        names.reserve(m_header.numDynamicProps);

        for (auto it = m_dynamicNames.cbegin(); it != m_dynamicNames.cend(); ++it)
        {
            names.push_back(*it);
        }
    }

    void Ires::getStaticPropertyNames(std::vector<std::string>& names) const
    {
        if (m_header.numStaticProps < 1)
        {
            names = std::vector<std::string>(0);
            return;
        }

        names.reserve(m_header.numStaticProps);

        for (auto it = m_staticNames.cbegin(); it != m_staticNames.cend(); ++it)
        {
            names.push_back(*it);
        }
    }

    void Ires::getFaceNames(std::vector<std::string>& names) const
    {

        names.clear();

        if (m_header.numFaceProps < 1) return;

         names.reserve(m_header.numFaceProps);

         for ( unsigned i = 0 ;  i < m_faceNames.size() ; ++i)
         {
             names.push_back(m_faceNames[i]);
         }
    }

    void Ires::getDynamicPropertyNumTimesteps(std::vector<U32>& numTimesteps) const
    {
        numTimesteps = m_dynamicNumTimesteps;
    }

    void Ires::getCentroid(U32 index, F32 centroid[3]) const
    {
        if (!m_blocksSet || !m_verticesSet)
        {
            throw std::runtime_error("IRES: > !!! ERROR: Must set blocks and vertices before building centroids");
        }

        if ((centroid == NULL) || (index >= m_blockIndexList.size()))
        {
            throw std::runtime_error("IRES: > !!! ERROR: Bad arguments when calling getCentroid()");
        }

        if (m_blockIndexList[index] < 0)
        {
            return;
        }

        std::size_t listIndex = m_blockIndexList[index];

        Vec3 v0(m_vertexList[m_blockList[listIndex] * 3], m_vertexList[m_blockList[listIndex] * 3 + 1],
            m_vertexList[m_blockList[listIndex] * 3 + 2]);
        ++listIndex;
        Vec3 v1(m_vertexList[m_blockList[listIndex] * 3], m_vertexList[m_blockList[listIndex] * 3 + 1],
            m_vertexList[m_blockList[listIndex] * 3 + 2]);
        ++listIndex;
        Vec3 v2(m_vertexList[m_blockList[listIndex] * 3], m_vertexList[m_blockList[listIndex] * 3 + 1],
            m_vertexList[m_blockList[listIndex] * 3 + 2]);
        ++listIndex;
        Vec3 v3(m_vertexList[m_blockList[listIndex] * 3], m_vertexList[m_blockList[listIndex] * 3 + 1],
            m_vertexList[m_blockList[listIndex] * 3 + 2]);
        ++listIndex;
        Vec3 v4(m_vertexList[m_blockList[listIndex] * 3], m_vertexList[m_blockList[listIndex] * 3 + 1],
            m_vertexList[m_blockList[listIndex] * 3 + 2]);
        ++listIndex;
        Vec3 v5(m_vertexList[m_blockList[listIndex] * 3], m_vertexList[m_blockList[listIndex] * 3 + 1],
            m_vertexList[m_blockList[listIndex] * 3 + 2]);
        ++listIndex;
        Vec3 v6(m_vertexList[m_blockList[listIndex] * 3], m_vertexList[m_blockList[listIndex] * 3 + 1],
            m_vertexList[m_blockList[listIndex] * 3 + 2]);
        ++listIndex;
        Vec3 v7(m_vertexList[m_blockList[listIndex] * 3], m_vertexList[m_blockList[listIndex] * 3 + 1],
            m_vertexList[m_blockList[listIndex] * 3 + 2]);


        Vec3 center = (v0 + v1 + v2 + v3 + v4 + v5 + v6 + v7)/8.0;

        try
        {
            centroid[0] = center[0];
            centroid[1] = center[1];
            centroid[2] = center[2];
        }
        catch (std::exception& e)
        {
            std::printf("IRES: > !!! ERROR: getCentroid(): Memory for 'centroid' arg must be allocated by caller! Exception: %s\n", e.what());
            throw e;
        }
    }

    // Given a block index, returns the (I, J, K) coordinates.
    // Index, I, J, and K, all start at 1 (internally).
    void Ires::getIJKfromIndex(U32 index, U32 &i, U32 &j, U32 &k) const
    {
        if ((m_header.numI < 1) || (m_header.numJ < 1) || (m_header.numK < 1))
        {
            throw std::runtime_error("IRES: > !!! ERROR: Must set blocks before getting IJK from Index");
        }

        if (index >= m_blockIndexList.size())
        {
            throw std::runtime_error("IRES: > !!! ERROR: Index out of range when calling getIJK()");
        }

        i = (index % m_header.numI);
        j = ((index / m_header.numI) % m_header.numJ);
        k = (index / (m_header.numI * m_header.numJ));
    }

    bool Ires::isValidBlock ( U32 index ) const
    {
        if( index >= m_blockIndexList.size() )
            return false;
        return m_blockIndexList[index] >= 0;
    }

    bool Ires::isValidBlock ( U32 I , U32 J , U32 K ) const
    {
        int index = this->getBlockIndex( I , J , K ) ;
        if( index < 0 )
            return false ;
        return this->isValidBlock( (U32)index );
    }


    // Given IJK coordinates, returns the block index
    // Returns -1 if is out of range
    int Ires::getBlockIndex(int i, int j, int k) const
    {
        if ((i < 0 ) || (j < 0 ) || (k < 0 ) ||
            (i >= (int)m_header.numI) || (j >= (int)m_header.numJ) || (k >= (int)m_header.numK) )
        {
           return -1 ;
        }

        return i + j * m_header.numI + k * m_header.numI * m_header.numJ;
    }
    
    //  Gives vertices ordered v0.x,v0.y,v0.z, v1.x ...
    //  where vertex numbering is like so:
    //
    //      v7-------v6                      +Y
    //      /        /|                      |
    //     /        / |                      |
    //    v4-------v5 v2                     ---------+X
    //    |        | /                      /
    //    |        |/                      /
    //    v0-------v1                    +Z
    //
    //  You must allocate memory for double* vertices BEFORE calling.
    bool Ires::getBlockVertices(U32 index, F32* const vertices) const
    {
        if ((index >= m_blockIndexList.size()) || (vertices == NULL))
        {
            throw std::runtime_error("IRES: > !!! ERROR: Bad arguments when calling getBlockVertices()");
        }

        if (m_blockIndexList[index] < 0)
        {
            return false;
        }

        std::size_t listIndex = m_blockIndexList[index];

        for (int i = 0; i < 3; i++)
        {
            vertices[0*3 + i] = m_vertexList[m_blockList[listIndex] * 3 + i];
            vertices[1*3 + i] = m_vertexList[m_blockList[listIndex+1] * 3 + i];
            vertices[2*3 + i] = m_vertexList[m_blockList[listIndex+2] * 3 + i];
            vertices[3*3 + i] = m_vertexList[m_blockList[listIndex+3] * 3 + i];

            vertices[4*3 + i] = m_vertexList[m_blockList[listIndex+4] * 3 + i];
            vertices[5*3 + i] = m_vertexList[m_blockList[listIndex+5] * 3 + i];
            vertices[6*3 + i] = m_vertexList[m_blockList[listIndex+6] * 3 + i];
            vertices[7*3 + i] = m_vertexList[m_blockList[listIndex+7] * 3 + i];
        }

        return true;
    }
    
    void Ires::getNumIJK(U32 &i, U32 &j, U32 &k) const
    {
        i = m_header.numI;
        j = m_header.numJ;
        k = m_header.numK;
    }

    void Ires::getMinMax(F32 &minX, F32 &minY, F32 &minZ,
            F32 &maxX, F32 &maxY, F32 &maxZ) const
    {
        minX = m_header.minX;
        minY = m_header.minY;
        minZ = m_header.minZ;

        maxX = m_header.maxX;
        maxY = m_header.maxY;
        maxZ = m_header.maxZ;
    }

    /// Returns the list with all vertices used by blocks and faces, one Vertice
    /// V with index I has coordinates:
    /// Vx = vertexlist[ 3*I + 0 ]
    /// Vy = vertexlist[ 3*I + 1 ]
    /// Vz = vertexlist[ 3*I + 2 ]
    std::vector<F32> Ires::getVertexList( void ) const
    {
        return m_vertexList;
    }

    std::vector<U32> Ires::getBlockList( void ) const
    {
        return m_blockList;
    }

    std::vector<int> Ires::getBlockIndexList( void ) const
    {
        return m_blockIndexList;
    }

    /////////////////////////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////////////////////////
    /////////// Setters /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    void Ires::setHeaderData(const std::string& title, Date runDate)
    {
        size_t length = title.length();
        memcpy(m_header.title, title.c_str(), (length < MaxStringLength)? length : MaxStringLength);
        m_header.runDate = runDate.getAsInt();

    }

    void Ires::setVertexList(std::vector<F32>& vertexList)
    {
        if (vertexList.empty())
        {
            if (m_verticesSet)
            {
                m_verticesSet = false;
                m_vertexList = std::vector<F32>(0);
            }

            return;
        }

        m_vertexList.swap(vertexList);

        m_header.minX = FLT_MAX;
        m_header.minY = FLT_MAX;
        m_header.minZ = FLT_MAX;
        m_header.maxX = -FLT_MAX;
        m_header.maxY = -FLT_MAX;
        m_header.maxZ = -FLT_MAX;

        std::size_t size = m_vertexList.size();
        
        ProgressOutput() << "IRES: > Setting min/max values...\n";

        // Now set min/max values.
        for (std::size_t i = 0; i < size; i += 3)
        {
            if (m_vertexList[i] < m_header.minX)
                m_header.minX = m_vertexList[i];

            if (m_vertexList[i + 1] < m_header.minY)
                m_header.minY = m_vertexList[i + 1];

            if (m_vertexList[i + 2] < m_header.minZ)
                m_header.minZ = m_vertexList[i + 2];

            if (m_vertexList[i] > m_header.maxX)
                m_header.maxX = m_vertexList[i];

            if (m_vertexList[i + 1] > m_header.maxY)
                m_header.maxY = m_vertexList[i + 1];

            if (m_vertexList[i + 2] > m_header.maxZ)
                m_header.maxZ = m_vertexList[i + 2];
        }

        ProgressOutput() << "IRES: > Done setting min/max values.\n";

        m_verticesSet = true;
    }

    void Ires::setBlockList(std::vector<U32>& blockList, std::vector<int>& blockIndexList,
            U32 numI, U32 numJ, U32 numK)
    {
        if (blockList.empty() || blockIndexList.empty() || ((numI < 1) && (numJ < 1) && (numK < 1)))
        {
            if (m_blocksSet)
            {
                m_blocksSet = false;
                m_blockList = std::vector<U32>(0);
                m_blockIndexList = std::vector<int>(0);
                m_header.numI = 0;
                m_header.numJ = 0;
                m_header.numK = 0;
            }

            return;
        }


        if (!m_verticesSet || (numI < 1) || (numJ < 1) || (numK < 1))
        {
            throw std::runtime_error("IRES: > !!! ERROR: Bad arguments passed to setBlockList()");
        }

        m_header.numI = numI;
        m_header.numJ = numJ;
        m_header.numK = numK;

        m_blockList.swap(blockList);
        m_blockIndexList.swap(blockIndexList);
        
        m_blocksSet = true;

        m_treeBuilt = false;
        m_tree.reset();
    }

    void Ires::setDynamicProps(std::vector<std::string>& names,
                               std::vector<U32> numTimesSteps ,
                               std::vector<F32>& values)
    {
        m_dynamicNames = std::vector<std::string>(0);
        m_dynamicNumTimesteps = std::vector<U32>(0);
        m_dynamicData = std::vector<F32>(0);
        m_header.numDynamicProps = names.size();

        if ( names.empty() ) return;

        U32 sumNumTimeSteps = 0;

        for( U32 i = 0 ; i < numTimesSteps.size() ; ++i ) sumNumTimeSteps += numTimesSteps[i];

        U32 size = ( m_blockIndexList.size() * sumNumTimeSteps ) ;

        if ( values.empty() || values.size() != size)
        {
            throw std::runtime_error("IRES: > !!! ERROR: Bad arguments when calling setDynamicProps()");
        }

        m_header.numTimestepsTotal = sumNumTimeSteps;

        // Store NAMES
        m_dynamicNames.swap(names);

        // Store numbers of Timesteps
        m_dynamicNumTimesteps.swap(numTimesSteps);

        // Set VALUES
        m_dynamicData.swap(values);
    }

    void Ires::setFaceProps( std::vector<std::string>& names ,
                             std::vector<F32>& values )
    {
        m_faceData      = std::vector<F32>(0);
        m_faceNames     = std::vector<std::string>(0);
        m_header.numFaceProps = names.size();

        if ( names.empty() ) return ;

        U32 totalSize = names.size()*m_blockIndexList.size()*6 ;
        if ( values.empty() || values.size() != totalSize)
        {
            throw std::runtime_error("IRES: > !!! ERROR: Bad arguments when calling setFaceProps()");
        }

        // Store NAMES
        m_faceNames.swap(names);

        // Set VALUES
        m_faceData.swap(values);



    }

    void Ires::setStaticProps(std::vector<std::string>& names, std::vector<F32>& values)
    {
        if (names.empty())
        {
            m_staticNames = std::vector<std::string>(0);
            m_staticData = std::vector<F32>(0);

            return;
        }

        if (m_blockIndexList.empty())
        {
            throw std::runtime_error("IRES: > !!! ERROR: Must set blocks before setting static properties");
        }

        std::size_t size = m_blockIndexList.size() * names.size();

        if (values.empty() || values.size() != size)
        {
            throw std::runtime_error("IRES: > !!! ERROR: Bad arguments when calling setStaticProps()");
        }

        m_header.numStaticProps = names.size();

        m_staticNames = std::vector<std::string>(0);
        m_staticData = std::vector<F32>(0);

        // Store NAMES
        m_staticNames.swap(names);

        // Set VALUES
        m_staticData.swap(values);
    }

    /////////////////////////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////////////////////////
    /////////// 3D Visualization functions //////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////

    void Ires::setFace( U32 i, U32 j, U32 k, U32 toK, U32 index, int blockListIndex,
                        U32 mergeK, bool skipping , bool preserveDuplicates,
                        std::unordered_set<Face, Face, Face>& faceSet ,
                        std::vector<Face>& quadFaceList ) const
    {
        Face face;

        face.id = index;

        int nextIndex = -1;

        if (skipping)
        {
            if (k + mergeK < toK)
            {
                nextIndex = m_blockIndexList[getBlockIndex(i, j, k + mergeK)];
            }
            else if (k < toK - 1)
            {
                nextIndex = m_blockIndexList[getBlockIndex(i, j, toK - 1)];
            }
            else
            {
                nextIndex = 0;
            }

            if (nextIndex < 0)
            {
                nextIndex = 0;
            }
        }


        // BOTTOM
        if (skipping && nextIndex)
        {
            face.set(m_blockList[nextIndex+6], m_blockList[nextIndex+5],
                m_blockList[nextIndex+4], m_blockList[nextIndex+7]);
        }
        else
        {
            face.set(m_blockList[blockListIndex+2], m_blockList[blockListIndex+1],
                m_blockList[blockListIndex], m_blockList[blockListIndex+3]);
        }

        if ( faceSet.count(face) == 0 )
        {
            face.isExtern = true;
            face.auxId = (int)quadFaceList.size();
            face.faceBlockRelPos = Face::BOTTOM ;
            quadFaceList.push_back(face);
            faceSet.insert(face);
        }
        else if ( preserveDuplicates )
        {
            face.isExtern = false;
            face.auxId = (int)quadFaceList.size();
            face.faceBlockRelPos = Face::BOTTOM ;
            quadFaceList.push_back(face);
            U32 k = U32(faceSet.find(face)->auxId);
            quadFaceList[ k ].isExtern = false ;

        }
        else
        {
            faceSet.erase(face);
        }

        // TOP
        face.set(m_blockList[blockListIndex+4], m_blockList[blockListIndex+5],
            m_blockList[blockListIndex+6], m_blockList[blockListIndex+7]);

        if ( faceSet.count(face) == 0 )
        {
            face.isExtern = true;
            face.auxId = (int)quadFaceList.size();
            face.faceBlockRelPos = Face::TOP ;
            quadFaceList.push_back(face);
            faceSet.insert(face);
        }
        else if ( preserveDuplicates )
        {
            face.isExtern = false;
            face.auxId = (int)quadFaceList.size();
            face.faceBlockRelPos = Face::TOP ;
            quadFaceList.push_back(face);
            U32 k = U32(faceSet.find(face)->auxId);
            quadFaceList[ k ].isExtern = false ;
        }
        else
        {
            faceSet.erase(face);
        }


        // LEFT
        if (skipping && nextIndex)
        {
            face.set(m_blockList[nextIndex+4], m_blockList[blockListIndex+4],
                m_blockList[blockListIndex+7], m_blockList[nextIndex+7]);
        }
        else
        {
            face.set(m_blockList[blockListIndex], m_blockList[blockListIndex+4],
                m_blockList[blockListIndex+7], m_blockList[blockListIndex+3]);
        }

        if ( faceSet.count(face) == 0 )
        {
            face.isExtern = true;
            face.auxId = (int)quadFaceList.size();
            face.faceBlockRelPos = Face::LEFT ;
            quadFaceList.push_back(face);
            faceSet.insert(face);
        }
        else if ( preserveDuplicates )
        {
            face.isExtern = false;
            face.auxId = (int)quadFaceList.size();
            face.faceBlockRelPos = Face::LEFT ;
            quadFaceList.push_back(face);
            U32 k = U32(faceSet.find(face)->auxId);
            quadFaceList[ k ].isExtern = false ;
        }
        else
        {
            faceSet.erase(face);
        }

        // RIGHT
        if (skipping && nextIndex)
        {
            face.set(m_blockList[nextIndex+6], m_blockList[blockListIndex+6],
                m_blockList[blockListIndex+5], m_blockList[nextIndex+5]);
        }
        else
        {
            face.set(m_blockList[blockListIndex+2], m_blockList[blockListIndex+6],
                m_blockList[blockListIndex+5], m_blockList[blockListIndex+1]);
        }

        if ( faceSet.count(face) == 0 )
        {
            face.isExtern = true;
            face.auxId = (int)quadFaceList.size();
            face.faceBlockRelPos = Face::RIGHT ;
            quadFaceList.push_back(face);
            faceSet.insert(face);
        }
        else if ( preserveDuplicates )
        {
            face.isExtern = false;
            face.auxId = (int)quadFaceList.size();
            face.faceBlockRelPos = Face::RIGHT ;
            quadFaceList.push_back(face);
            U32 k = U32(faceSet.find(face)->auxId);
            quadFaceList[ k ].isExtern = false ;
        }
        else
        {
            faceSet.erase(face);
        }

        // FRONT
        if (skipping && nextIndex)
        {
            face.set(m_blockList[nextIndex+5], m_blockList[blockListIndex+5],
                m_blockList[blockListIndex+4], m_blockList[nextIndex+4]);
        }
        else
        {
            face.set(m_blockList[blockListIndex+1], m_blockList[blockListIndex+5],
                m_blockList[blockListIndex+4], m_blockList[blockListIndex]);
        }

        if ( faceSet.count(face) == 0 )
        {
            face.isExtern = true;
            face.auxId = (int)quadFaceList.size();
            face.faceBlockRelPos = Face::FRONT ;
            quadFaceList.push_back(face);
            faceSet.insert(face);
        }
        else if ( preserveDuplicates )
        {
            face.isExtern = false;
            face.auxId = (int)quadFaceList.size();
            face.faceBlockRelPos = Face::FRONT ;
            quadFaceList.push_back(face);
            U32 k = U32(faceSet.find(face)->auxId);
            quadFaceList[ k ].isExtern = false ;
        }
        else
        {
            faceSet.erase(face);
        }

        // BACK
        if (skipping && nextIndex)
        {
            face.set(m_blockList[nextIndex+7], m_blockList[blockListIndex+7],
                m_blockList[blockListIndex+6], m_blockList[nextIndex+6]);
        }
        else
        {
            face.set(m_blockList[blockListIndex+3], m_blockList[blockListIndex+7],
                m_blockList[blockListIndex+6], m_blockList[blockListIndex+2]);
        }

        if ( faceSet.count(face) == 0 )
        {
            face.isExtern = true;
            face.auxId = (int)quadFaceList.size();
            face.faceBlockRelPos = Face::BACK ;
            quadFaceList.push_back(face);
            faceSet.insert(face);
        }
        else if ( preserveDuplicates )
        {
            face.isExtern = false;
            face.auxId = (int)quadFaceList.size();
            face.faceBlockRelPos = Face::BACK ;
            quadFaceList.push_back(face);
            U32 k = U32(faceSet.find(face)->auxId);
            quadFaceList[ k ].isExtern = false ;
        }
        else
        {
            faceSet.erase(face);
        }
    }


    /// Returns a list of Quad Faces (quadFacesList), the whole model,
    /// the vertices a, b, c and, d of each face point to the vertex list,
    /// use getVertexList() to get the coordinates.
    void Ires::generateFaceList( std::vector<Face>& quadFacesList ) const
    {
        quadFacesList.clear();
        quadFacesList.reserve(6*m_blockIndexList.size());
        std::unordered_set<Face, Face, Face> faceSet; // Used for classifie the face as extern
        faceSet.reserve(6*m_blockIndexList.size());

        int count = 0 ;

        for ( U32 it =  0 ; it < m_blockIndexList.size() ; ++it)
        {
            int blockListIndex = -1;
            blockListIndex = m_blockIndexList[it];

            if ( blockListIndex >= 0 )
            {
                U32 i, j, k;
                this->getIJKfromIndex(it, i, j, k);
                this->setFace( i, j, k, 0, it , blockListIndex, 0 , false, true ,
                               faceSet , quadFacesList );

            }
            ++count;
//            if( ( it - (it/1024)*1024 ) == 0 ) std::cout << " Block = " << it << std::endl;
        }
        std::cout << " Block count = " << count << "    Face Total count = " << quadFacesList.size() << std::endl;
        quadFacesList.shrink_to_fit();
    }

    /// Returns a list of Quad Faces (quadFacesList), the whole model,
    /// From blocks in the range [ IFrom, JFrom, KFrom ] X [ ITo,  JTo,  KTo ],
    /// the vertices a, b, c and, d of each face point to the vertex list,
    /// use getVertexList() to get the coordinates.
    void Ires::generateFaceList( U32 IFrom , U32 JFrom , U32 KFrom ,
                                 U32 ITo   , U32 JTo   , U32 KTo   ,
                                 std::vector<Face>& quadFacesList  ) const
    {
        int IR = ITo - IFrom + 1 ;
        int JR = JTo - JFrom + 1 ;
        int KR = KTo - KFrom + 1 ;

        quadFacesList.clear();

        if( IR <= 0 || JR <= 0 || KR <= 0 ) return;


        quadFacesList.reserve(6*IR*JR*KR);
        std::unordered_set<Face, Face, Face> faceSet; // Used for classifie the face as extern
        faceSet.reserve(6*IR*JR*KR);

        for( U32 i = IFrom ; i <= ITo ; ++i )
        {
            for( U32 j = JFrom ; j <= JTo ; ++j )
            {
                for( U32 k = KFrom ; k <= KTo ; ++k )
                {
                    int index = getBlockIndex( i , j , k );
                    int blockListIndex = -1 ;

                    if( index >= 0 ) blockListIndex = m_blockIndexList[index];

                    if ( blockListIndex > -1 )
                    {
                        this->setFace( i, j, k, 0, index , blockListIndex, 0 , false, true ,
                                       faceSet , quadFacesList );
                    }
                }
            }
        }
        quadFacesList.shrink_to_fit();
    }

    /// Returns a list of Quad Faces (quadFacesList), the whole model,
    /// for blocks in blocksID vector,
    /// the vertices a, b, c and, d of each face point to the vertex list,
    /// use getVertexList() to get the coordinates.
    void Ires::generateFaceList( const std::vector<U32> blocksID  ,
                                 std::vector<Face>& quadFacesList ) const
    {
        quadFacesList.clear();
        quadFacesList.reserve(6*m_blockIndexList.size());
        std::unordered_set<Face, Face, Face> faceSet; // Used for classifie the face as extern
        faceSet.reserve(6*blocksID.size());

        for ( U32 it =  0 ; it < blocksID.size() ; ++it)
        {
            int blockListIndex = -1;
            if( blocksID[it] < m_blockIndexList.size() ) blockListIndex = m_blockIndexList[ blocksID[it] ];
            else ProgressOutput() << "IRES: ERRO --> in\n void Ires::generateFaceList( const std::vector<U32>"
                                  << " blocksID, std::vector<Face>& quadFacesList ) const\n OUT OF RANGE\n";

            if ( blockListIndex > -1 )
            {
                U32 i, j, k;
                this->getIJKfromIndex( blocksID[it] , i, j, k);
                this->setFace( i, j, k, 0, blocksID[it] , blockListIndex, 0 , false, true ,
                               faceSet , quadFacesList );
            }
        }
        quadFacesList.shrink_to_fit();
    }

    /////////////////////////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////////////////////////
    ///////// Add Propers  //////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////

    /// Add a Face proper
    bool Ires::addFaceProp( std::string name , std::vector<F32> values )
    {
        bool wasAdd = false ;
        if( values.size() == 6*m_blockIndexList.size() )
        {
            m_faceNames.push_back(name) ;
            m_faceData.reserve( m_faceData.size() + values.size() ) ;
            m_faceData.insert( m_faceData.end() , values.begin() , values.end() );
            m_header.numFaceProps += 1 ;

            wasAdd = true;
        }

        return wasAdd;
    }

    /// Add a Static proper
    bool Ires::addStaticProp( std::string name , std::vector<F32> values )
    {
        bool wasAdd = false ;
        if( values.size() == m_blockIndexList.size() )
        {
            m_staticNames.push_back(name) ;
            m_staticData.reserve( m_staticData.size() + values.size() ) ;
            m_staticData.insert( m_staticData.end() , values.begin() , values.end() );
            m_header.numStaticProps += 1;

            wasAdd = true;
        }

        return wasAdd;
    }


    /// Add a Dynamic proper
    bool Ires::addDynamicProp( std::string name , std::vector<F32> values )
    {
        U32 nOfTimeSteps = values.size()/m_blockIndexList.size();
        bool wasAdd = false ;
        if( values.size() == ( nOfTimeSteps * m_blockIndexList.size() ) )
        {
            m_dynamicNames.push_back(name) ;
            m_dynamicNumTimesteps.push_back( nOfTimeSteps ) ;

            std::cerr << "nOfTimeSteps = " << nOfTimeSteps << "   m_header.numTimestepsTotal =" <<  m_header.numTimestepsTotal << std::endl;

            m_dynamicData.reserve( m_dynamicData.size() + values.size() ) ;
            m_dynamicData.insert( m_dynamicData.end() , values.begin() , values.end() ) ;
            m_header.numTimestepsTotal += nOfTimeSteps ;
            m_header.numDynamicProps += 1 ;

            std::cerr << "m_header.numDynamicProps = " << m_header.numDynamicProps << "   m_header.numTimestepsTotal = " <<  m_header.numTimestepsTotal << std::endl;


            wasAdd = true;
        }

        return wasAdd;
    }


    /////////////////////////////////////////////////////////////////////////////////////
    /////////// Private functions ///////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    void Ires::init()
    {
        m_header.version = IresVersionNumber;
        std::memset(m_header.title, '\0', MaxStringLength * sizeof(U8));

        m_header.runDate = Date(0).getAsInt();

        m_header.numI = 0;
        m_header.numJ = 0;
        m_header.numK = 0;

        m_header.numDynamicProps    = 0;
        m_header.numStaticProps     = 0;
        m_header.numTimestepsTotal  = 0;

        m_header.minX = 0;
        m_header.minY = 0;
        m_header.minZ = 0;
        m_header.maxX = 0;
        m_header.maxY = 0;
        m_header.maxZ = 0;

        m_header.geometryDataPos = sizeof(MagicNumber) + sizeof(IresHeader);

        m_header.dynamicDataPos = 0;	// Gets set later
        m_header.staticDataPos  = 0;	// Gets set later

        m_dynamicNames = std::vector<std::string>(0);
        m_dynamicNumTimesteps = std::vector<U32>(0);
        m_staticNames  = std::vector<std::string>(0);
        m_dynamicData = std::vector<F32>(0);
        m_staticData = std::vector<F32>(0);

        m_vertexList = std::vector<F32>(0);
        m_blockList = std::vector<U32>(0);
        m_blockIndexList = std::vector<int>(0);

        m_verticesSet = false;
        m_blocksSet = false;
        m_initialized = true;

        m_tree.reset();
        m_treeBuilt = false;

    }

    void Ires::cleanup()
    {
        if (m_blocksSet)
        {
            m_blocksSet = false;

            m_blockList = std::vector<U32>(0);
            m_blockIndexList = std::vector<int>(0);
        }

        if (m_verticesSet)
        {
            m_verticesSet = false;
            m_vertexList = std::vector<F32>(0);
        }

        m_dynamicNames = std::vector<std::string>(0);
        m_staticNames = std::vector<std::string>(0);
        m_dynamicData = std::vector<F32>(0);
        m_staticData = std::vector<F32>(0);

        if (m_treeBuilt)
        {
            m_treeBuilt = false;
            m_tree.reset();
        }

        m_initialized = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////
}
 
