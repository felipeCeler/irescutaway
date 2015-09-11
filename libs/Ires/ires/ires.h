
#ifndef IRES_H
#define IRES_H

#include  "primitivesires.h"

namespace ires
{
    const F32 IresVersionNumber = 2.1f;
    const std::string IresExtension(".ires");
    const unsigned long long MagicNumber = 0xAA495245530D0C0A;
    const U32 MaxStringLength = 64;
    ////////////////////////////////////////////////////////////////////////////////
    /// IRES COORDINATE SYSTEM INFORMATION
    ///
    /// The model uses a right-handed coordinate system,
    /// with X going to the right and Y going up.
    ///
    /// The I,J,K system is assigned as +I = +X, +J = +Z, +K = -Y
    ///
    /// When importing files to the IRES format, you may need to switch from
    /// a left-handed system to a right-handed system; make sure to also switch
    /// cell property ordering so that you don't mix up property associations.
    ///
    /////////////////////////////////////////////////////////////////////////////////

    // NOTE: Block indices go from 0-(numBlocks - 1), while I,J,K go from 1-numI,1-numJ,1-numK (i.e., block (0,0,0) is undefined)
    struct IresHeader
    {
        F32 version;
        U8 title[MaxStringLength];
        U32 runDate;	// Number giving the run date in the format of YYYYMMDD
        U32 numI;
        U32 numJ;
        U32 numK;
        U32 numTimestepsTotal;
        U32 numDynamicProps;
        U32 numStaticProps;
        U32 numFaceProps;
        F32 minX;
        F32 minY;
        F32 minZ;
        F32 maxX;
        F32 maxY;
        F32 maxZ;
        U64 geometryDataPos;
        U64 dynamicDataPos;
        U64 staticDataPos;
        U64 faceDataPos;

        IresHeader()
            : version(0.0f),
            runDate(0),
            numI(0),
            numJ(0),
            numK(0),
            numTimestepsTotal(0),
            numDynamicProps(0),
            numStaticProps(0),
            numFaceProps(0),
            minX(0.0),
            minY(0.0),
            minZ(0.0),
            maxX(0.0),
            maxY(0.0),
            maxZ(0.0),
            geometryDataPos(0),
            dynamicDataPos(0),
            staticDataPos(0),
            faceDataPos(0)
        {
            std::memset(title, '\0', MaxStringLength * sizeof(U8));
        }
    };

    // Forward-declaration.
    class KdTree;

    class Ires
    {
    public:
        // Ctor, Dtor functions.
        Ires( bool reportStatus = false );
        ~Ires();

        // Resets to a default/freshly initialized state.
        void reset();

        // Initializes the KD-Tree that is used for
        // finding which blocks are closest to a given point.
        bool initializeKdTree() ;

        // getVertices should take in the block index, and assign a 8*3 array of doubles representing
        // vertices in the correct order (see this file for diagram). If the block is null, the returned
        // value should be FALSE, and TRUE otherwise
        void buildVertexBlockLists(U32 numI, U32 numJ, U32 numK, bool (*getVertices)(U32, F32*));
        void buildVertexBlockLists(U32 numI, U32 numJ, U32 numK, std::function<bool(U32, F32*)> getVertices);

        // Enum to define the possition of one face in one cube

        /////////////////////////////////////////////////////////////////////////////////////
        /////////// Read & Write functions, defined in reader.cpp and writer.cpp ////////////
        /////////////////////////////////////////////////////////////////////////////////////
        bool readFile(const std::string& filename);

        // Clears memory as it writes (so we don't run into bad_allocs).
        bool writeFile(const std::string& filename);
        /////////////////////////////////////////////////////////////////////////////////////


        /////////////////////////////////////////////////////////////////////////////////////
        /////////// Getters /////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////
        
        // Convenience function.
        U32  getNumTotalBlocks( void ) const;
        void getKNearestBlockIds(U32 index, U32 k, std::vector<U32>& blockIds) const;

        void getKNearestBlockIds(const F32 point[3], U32 k, std::vector<U32>& blockIds) const;

        bool isValidBlock ( U32 index ) const;
        bool isValidBlock ( U32 I , U32 J , U32 K ) const;

        // Returns a copy of the header.
        IresHeader getHeader() const;

        // These two methods return -1 if the property was not found.
        int getDynamicPropertyIndex(const std::string& name) const;
        int getStaticPropertyIndex(const std::string& name) const;
        int getFacePropertyIndex(const std::string& name) const;

        // Pass a value < 0 for timestep to get all timesteps for a given property.
        void getDynamicPropertyValues(U32 propertyIndex, int timestep, std::vector<F32>& values) const;
        void getStaticPropertyValues(U32 propertyIndex, std::vector<F32>& values) const;
        void getFacePropertyValues(U32 propertyIndex, std::vector<F32>& values) const;

        void getDynamicPropertyNumTimesteps(std::vector<U32> &numTimesteps) const;

        void getDynamicPropertyNames(std::vector<std::string>& names) const;
        void getStaticPropertyNames(std::vector<std::string>& names) const;
        void getFaceNames(std::vector<std::string>& names) const;

        void getCentroid(U32 index, F32 centroid[3]) const;

        // Given a block index, returns the (I, J, K) coordinates
        void getIJKfromIndex(U32 index, U32 &i, U32 &j, U32 &k) const;

        // Given IJK coordinates, returns the block index
        // Returns -1 if is out of range
        int getBlockIndex(int i, int j, int k) const;


        //  Gives vertices ordered v0.x,v0.y,v0.z, v1.x ...
        //  where vertex numbering is like so:
        //
        //      v7-------v6                      +Y / -K
        //      /        /|                      |
        //     /        / |                      |
        //    v4-------v5 v2                     ---------+X / +I
        //    |        | /                      /
        //    |        |/                      /
        //    v0-------v1                    +Z / +J
        //
        //  You must allocate memory for double* vertices BEFORE calling.
        bool getBlockVertices(U32 index, F32* const vertices) const;
        

        void getNumIJK(U32 &i, U32 &j, U32 &k) const ;

        void getMinMax(F32 &minX, F32 &minY, F32 &minZ,
            F32 &maxX, F32 &maxY, F32 &maxZ) const;

        /// Returns the list with all vertices used by blocks and faces, one Vertex
        /// V with index I has coordinates:
        /// Vx = vertexlist[ 3*I + 0 ]
        /// Vy = vertexlist[ 3*I + 1 ]
        /// Vz = vertexlist[ 3*I + 2 ]
        std::vector<F32> getVertexList( void ) const ;

        std::vector<U32> getBlockList( void ) const ;
        std::vector<int> getBlockIndexList( void ) const;

        /////////////////////////////////////////////////////////////////////////////////////
        /////////// Setters /////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////
        void setHeaderData(const std::string& title, Date runDate);
        
        // Set the vertex list; vertexList will be invalid after the call.
        void setVertexList(std::vector<F32>& vertexList);

        // BlockList contains a list of indices into the Vertex List.
        // BlockIndexList contains a list of indices into the BlockList (indicating where v0 of
        // a given block is). [See private member section for more details]
        // blockList and blockIndexList will be invalid after the call.
        void setBlockList(std::vector<U32>& blockList, std::vector<int>& blockIndexList,
            U32 numI, U32 numJ, U32 numK);

        // Consumes the data passed to it!
        void setDynamicProps(std::vector<std::string>& names,
                             std::vector<U32> numTimesSteps,
                             std::vector<F32>& values);

        // Consumes the data passed to it!
        void setStaticProps(std::vector<std::string>& names,
                            std::vector<F32>& values);

        // Consumes the data passed to it!
        void setFaceProps(std::vector<std::string>& names ,
                           std::vector<F32>& values );

        /////////////////////////////////////////////////////////////////////////////////////
        
        /////////////////////////////////////////////////////////////////////////////////////
        /////////// 3D Visualization functions //////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////

        /// Returns a list of Quad Faces (quadFacesList), the whole model,
        /// the vertices a, b, c and, d of each face point to the vertex list,
        /// use getVertexList() to get the coordinates.
        void generateFaceList( std::vector<Face>& quadFacesList ) const ;

        /// Returns a list of Quad Faces (quadFacesList), the whole model,
        /// from blocks in the range [ IFrom, JFrom, KFrom ] X [ ITo,  JTo,  KTo ],
        /// the vertices a, b, c and, d of each face point to the vertex list,
        /// use getVertexList() to get the coordinates.
        void generateFaceList( U32 IFrom , U32 JFrom , U32 KFrom ,
                               U32 ITo   , U32 JTo   , U32 KTo   ,
                               std::vector<Face>& quadFacesList ) const ;

        /// Returns a list of Quad Faces (quadFacesList), the whole model,
        /// for blocks in blocksID vector,
        /// the vertices a, b, c and, d of each face point to the vertex list,
        /// use getVertexList() to get the coordinates.
        void generateFaceList( const std::vector<U32> blocksID  ,
                               std::vector<Face>& quadFacesList ) const ;
        
        /////////////////////////////////////////////////////////////////////////////////////

        /////////////////////////////////////////////////////////////////////////////////////
        ///////// Add Propers  //////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////

        /// Add a Face proper
        bool addFaceProp( std::string name , std::vector<F32> values );

        /// Add a Static proper
        bool addStaticProp( std::string name , std::vector<F32> values );

        /// Add a Dynamic proper
        bool addDynamicProp( std::string name , std::vector<F32> values );

        /////////////////////////////////////////////////////////////////////////////////////


    private:
        Ires(const Ires&) ;             // Declarations only; prevent copy-construction
        Ires& operator=(const Ires&) ;  // and copy-assignment.

        void init();
        void cleanup();

        void setFace( U32 i, U32 j, U32 k, U32 toK, U32 index, int blockListIndex, U32 mergeK,
                      bool skipping , bool preserveDuplicates, std::unordered_set<Face, Face, Face>& faceSet ,
                      std::vector<Face>& quadFaceList ) const ;


        // Member variables
        IresHeader m_header;

        bool m_reportProgress;
        bool m_initialized, m_verticesSet, m_blocksSet;

        std::vector<std::string> m_dynamicNames;
        std::vector<std::string> m_staticNames;
        std::vector<std::string> m_faceNames;

        std::vector<U32> m_dynamicNumTimesteps;

        std::vector<F32> m_dynamicData;
        std::vector<F32> m_staticData;

        /// m_faceData contains for each block 6 face property values
        /// in the following order: Bottom:v(2103); Top:v(4567);
        /// Left:v(0473); Right:v(2651); Front:v(1540); Back:v(3762).
        /// Example: a propertie K and a block index I are givem, them
        /// ( K*numofblocks + I*6 + ires::FRONT) = (K*numofblocks + I*6+4)
        /// returns the propertie K of the front face of block I
        std::vector<F32> m_faceData;

        std::vector<F32> m_vertexList;
        std::vector<U32> m_blockList;	    // Contains indices into the vertex list.
        std::vector<int> m_blockIndexList;	// Contains indices into the block list (-1 means null block).

        // Use both of the above to access block vertices. They are ordered from v0 to v7,
        // like so:
        //
        //      v7-------v6                      +Y / -K
        //      /        /|                      |
        //     /        / |                      |
        //    v4-------v5 v2                     ---------+X / +I
        //    |        | /                      /
        //    |        |/                      /
        //    v0-------v1                    +Z / +J
        //
        // So to get the values for Block 525's vertex 5, you'd index into the vertex list with:
        // m_vertexList[m_blockList[m_blockIndexList[525]]+5]. This would give you the X coordinate,
        // and the next 2 entries in the vertex list would be the y and z coordinates.
        // If block 100 were invalid, the entry for m_blockIndexList[100] would be -1.


        std::unique_ptr<KdTree> m_tree;
        bool m_treeBuilt;
    };
}

#endif

