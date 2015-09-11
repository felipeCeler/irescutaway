
#include "ires.h"

#ifdef _WIN32
#include "snappy\snappy.h"  // For compression/decompression.
#else
#include "snappy/snappy.h"
#endif

#include <fstream>
#include <iostream>
#include <iomanip>
#include <cassert>

bool ires::Ires::readFile(const std::string& filename)
{
    reset();

    std::ifstream input(filename, std::ios::in | std::ios::binary);
    
    ProgressOutput() << "\nIRES: > Reading file '" << filename << "'...\n";

    if (!input.good())
    {
        ProgressOutput() << "IRES: > ERROR !!!: Failed to open '" << filename << "'\n";

        return false;
    }


    
    ProgressOutput() << "IRES: > Checking magic number...\n";

    //////////////// CHECK MAGIC NUMBER ///////////////////
    char current;

    U64 check = 0;

    // The magic number is 8 bytes long. Have to do some funny stuff because little-endian architecture
    for (int i = 0; i < 8; ++i)
    {
        input.read(&current, 1);
        check += ((U64)current) << (8*7 - i*8);
    }

    if (check != MagicNumber)
    {
        ProgressOutput() << "IRES: > ERROR !!!: '" << filename << "' is not a valid IRES v"
            << std::setw(3) << std::setprecision(1) << IresVersionNumber <<" file.\n";

        return false;	// Magic number did not match, invalid file.
    }





    ProgressOutput() << "IRES: > Reading header...\n";

    //////////////// READ IN HEADER /////////////////////
    input.read((char*) &(m_header.version), sizeof(m_header.version));
    input.read((char*) &(m_header.title), sizeof(m_header.title));
    input.read((char*) &(m_header.runDate), sizeof(m_header.runDate));
    input.read((char*) &(m_header.numI), sizeof(m_header.numI));
    input.read((char*) &(m_header.numJ), sizeof(m_header.numJ));
    input.read((char*) &(m_header.numK), sizeof(m_header.numK));
    input.read((char*) &(m_header.numTimestepsTotal), sizeof(m_header.numTimestepsTotal));
    input.read((char*) &(m_header.numDynamicProps), sizeof(m_header.numDynamicProps));
    input.read((char*) &(m_header.numStaticProps), sizeof(m_header.numStaticProps));
    if( m_header.version > 2.0 ) input.read((char*) &(m_header.numFaceProps), sizeof(m_header.numFaceProps));
    input.read((char*) &(m_header.minX), sizeof(m_header.minX));
    input.read((char*) &(m_header.minY), sizeof(m_header.minY));
    input.read((char*) &(m_header.minZ), sizeof(m_header.minZ));
    input.read((char*) &(m_header.maxX), sizeof(m_header.maxX));
    input.read((char*) &(m_header.maxY), sizeof(m_header.maxY));
    input.read((char*) &(m_header.maxZ), sizeof(m_header.maxZ));
    input.read((char*) &(m_header.geometryDataPos), sizeof(m_header.geometryDataPos));
    input.read((char*) &(m_header.dynamicDataPos), sizeof(m_header.dynamicDataPos));
    input.read((char*) &(m_header.staticDataPos), sizeof(m_header.staticDataPos));
    if( m_header.version > 2.0 ) input.read((char*) &(m_header.faceDataPos), sizeof(m_header.faceDataPos));

    ProgressOutput() << "IRES: > file version  = " << m_header.version << "\n";

    assert(((U64)input.tellg()) == (m_header.geometryDataPos));

    ProgressOutput() << "IRES: > Reading vertex data...\n";

    ///////////////////////////////////////////////////////////////////////////
    /// GEOMETRY DATA
    ///////////////////////////////////////////////////////////////////////////
    /////////////////// READ VERTEX DATA //////////////////////
    U32 compressedSize = 0;

    input.read((char*) &compressedSize, sizeof(U32));


    std::vector<U8> compressedData(compressedSize);

    input.read((char*) compressedData.data(), compressedSize);


    ProgressOutput() << "IRES: > Decompressing and setting vertex data...\n";

    /////////////////// DECOMPRESS & SET VERTEX DATA ///////////////////

    snappy::string uncompressedStr;
    snappy::Uncompress((const char*) compressedData.data(), compressedSize, &uncompressedStr);
    
    compressedData = std::vector<U8>(0);


    U8* uncompressedData = (U8*) uncompressedStr.data();

    U32 numVertexDoubles = 0;
    memcpy(&numVertexDoubles, uncompressedData, sizeof(U32));


    std::size_t vertexDataSize = 0;

    std::vector<F32> vList(numVertexDoubles);

    vertexDataSize = numVertexDoubles * sizeof(F32);

    memcpy(vList.data(), uncompressedData + sizeof(U32), vertexDataSize);

    setVertexList(vList);
    vList.clear();

    ProgressOutput() << "IRES: > Reading block data...\n";

    /////////////////// READ BLOCK DATA //////////////////////
    compressedSize = 0;

    input.read((char*) &compressedSize, sizeof(U32));


    compressedData.resize(compressedSize);

    input.read((char*) compressedData.data(), compressedSize);


    ProgressOutput() << "IRES: > Decompressing and setting block data...\n";

    /////////////////// DECOMPRESS & SET BLOCK DATA ///////////////////
    uncompressedStr = std::string();

    snappy::Uncompress((const char*) compressedData.data(), compressedSize, &uncompressedStr);
    
    compressedData.clear();

    uncompressedData = (U8*) uncompressedStr.data();

    U32 blockListLengthPlusNullBlocks = 0;
    memcpy(&blockListLengthPlusNullBlocks, uncompressedData, sizeof(U32));
    
    std::size_t blockDataSize = 0;
    
    blockDataSize = blockListLengthPlusNullBlocks * sizeof(U32);
    

    std::vector<U32> blockData(blockListLengthPlusNullBlocks);

    memcpy(blockData.data(), uncompressedData + sizeof(U32), blockDataSize);

    uncompressedData = NULL;
    uncompressedStr = std::string();
    


    std::vector<int> bIndexList;
    bIndexList.reserve(blockListLengthPlusNullBlocks);

    std::vector<U32> bList;
    bList.reserve(blockListLengthPlusNullBlocks);

    std::size_t blockDataIndex = 0; // Indexes into the read uncompressed block data.

    while (blockDataIndex < blockListLengthPlusNullBlocks)
    {
        if (blockData[blockDataIndex] != (U32) -1)
        {
            bIndexList.push_back(bList.size());

            for (int j = 0; j < 8; j++)
            {
                bList.push_back(blockData[blockDataIndex + j]);
            }

            blockDataIndex += 8;
        }
        else
        {
            bIndexList.push_back(-1);
            ++blockDataIndex;
        }
    }

    bList.shrink_to_fit();
    
    setBlockList(bList, bIndexList, m_header.numI, m_header.numJ, m_header.numK);

    blockData.clear();
    bIndexList.clear();
    bList.clear();

    ///////////////////////////////////////////////////////////////////////////
    /// DYNAMIC PROPERTIES
    ///////////////////////////////////////////////////////////////////////////
    assert(((U64)input.tellg()) == (m_header.dynamicDataPos));


    ProgressOutput() << "IRES: > Reading dynamic property data...\n";


    /////////////////// READ DYNAMIC PROPERTY DATA //////////////////////
    compressedSize = 0;

    input.read((char*) &compressedSize, sizeof(U32));

    compressedData = std::vector<U8>(0);
    compressedData.resize(compressedSize);

    input.read((char*) compressedData.data(), compressedSize);



    ProgressOutput() << "IRES: > Decompressing and setting dynamic property data...\n";

    /////////////////// DECOMPRESS DYNAMIC PROPERTY DATA ///////////////////
    snappy::Uncompress((const char*) compressedData.data(), compressedSize, &uncompressedStr);
    
    uncompressedData = (U8*) uncompressedStr.data();

    std::vector<std::string> names;

    names.resize(m_header.numDynamicProps);

    for (std::size_t i = 0; i < m_header.numDynamicProps; ++i)
    {
        char * nameTmp = new char [MaxStringLength];
        for (std::size_t j = 0; j < MaxStringLength; ++j)
        {
            nameTmp[j] = uncompressedData[i*MaxStringLength + j];
        }

        names[i] = std::string(nameTmp);
        delete[] nameTmp;
    }

    std::size_t offset = m_header.numDynamicProps * MaxStringLength;
    
    std::vector<U32> numTimesSteps;
    std::vector<F32> vals;

    if(m_header.version == 2.0 )
    {
        /////////////////// DECOMPRESS DYNAMIC PROPERTY DATA V2.0 //////////////////////
        numTimesSteps = std::vector<U32>( m_header.numDynamicProps , m_header.numTimestepsTotal ); // repeating the number of times steps to be compatible with the version 2.1

        vals.resize( m_blockIndexList.size() *  m_header.numDynamicProps * m_header.numTimestepsTotal );

        memcpy((char*)vals.data(), uncompressedData + offset,
               m_blockIndexList.size() * m_header.numTimestepsTotal * sizeof(F32));
    }
    else
    {
        numTimesSteps.resize( m_header.numDynamicProps );
        memcpy((char*)numTimesSteps.data(), uncompressedData + offset,
               m_header.numDynamicProps * sizeof(U32));

        offset += m_header.numDynamicProps * sizeof(U32);

        vals.resize( m_blockIndexList.size() * m_header.numTimestepsTotal );

        memcpy((char*)vals.data(), uncompressedData + offset,
               m_blockIndexList.size() * m_header.numTimestepsTotal * sizeof(F32));
    }

    setDynamicProps(names, numTimesSteps ,vals);

    names.clear();
    numTimesSteps.clear();
    vals.clear();
    compressedData.clear();
    uncompressedData = NULL;
    uncompressedStr = std::string();

    ///////////////////////////////////////////////////////////////////////////
    /// STATIC PROPERTIES
    ///////////////////////////////////////////////////////////////////////////
    assert(((U64)input.tellg()) == (m_header.staticDataPos));
    ProgressOutput() << "IRES: > Reading static property data...\n";


    /////////////////// READ STATIC PROPERTY DATA //////////////////////
    compressedSize = 0;

    input.read((char*) &compressedSize, sizeof(U32));

    compressedData.resize(compressedSize);

    input.read((char*) compressedData.data(), compressedSize);



    ProgressOutput() << "IRES: > Decompressing and setting static property data...\n";


    /////////////////// DECOMPRESS STATIC PROPERTY DATA ///////////////////

    snappy::Uncompress((const char*) compressedData.data(), compressedSize, &uncompressedStr);
    
    uncompressedData = (U8*) uncompressedStr.data();

    names.resize(m_header.numStaticProps);

    for (std::size_t i = 0; i < m_header.numStaticProps; ++i)
    {
        char * nameTmp = new char [MaxStringLength];
        for (std::size_t j = 0; j < MaxStringLength; ++j)
        {
            nameTmp[j] = uncompressedData[i*MaxStringLength + j];
        }

        names[i] = std::string(nameTmp);
        delete[] nameTmp;
    }

    
    offset = m_header.numStaticProps * MaxStringLength;

    vals.resize(m_header.numStaticProps * m_blockIndexList.size());

    memcpy((char*)vals.data(), uncompressedData + offset,
           m_header.numStaticProps * m_blockIndexList.size() * sizeof(F32));

    setStaticProps(names, vals);

    names.clear();
    vals.clear();
    compressedData.clear();
    uncompressedData = NULL;
    uncompressedStr = std::string();

    if( m_header.version > 2.0 )
    {
        ///////////////////////////////////////////////////////////////////////////
        /// FACE PROPERTIES
        ///////////////////////////////////////////////////////////////////////////
        assert(((U64)input.tellg()) == (m_header.faceDataPos));


        ProgressOutput() << "IRES: > Reading face property data...\n";


        /////////////////// READ FACE PROPERTY DATA //////////////////////
        compressedSize = 0;

        input.read((char*) &compressedSize, sizeof(U32));

        compressedData.resize(compressedSize);

        input.read((char*) compressedData.data(), compressedSize);

        ProgressOutput() << "IRES: > Decompressing and setting face property data...\n";

        /////////////////// DECOMPRESS FACE PROPERTY DATA ///////////////////
        snappy::Uncompress((const char*) compressedData.data(), compressedSize, &uncompressedStr);

        uncompressedData = (U8*) uncompressedStr.data();

        names.resize(m_header.numFaceProps);

        for (std::size_t i = 0; i < m_header.numFaceProps; ++i)
        {
            char * nameTmp = new char [MaxStringLength];
            for (std::size_t j = 0; j < MaxStringLength; ++j)
            {
                nameTmp[j] = uncompressedData[i*MaxStringLength + j];
            }
            names[i] = std::string(nameTmp);
            delete[] nameTmp;
        }



        offset = m_header.numFaceProps * MaxStringLength;

         vals.resize( 6 * m_header.numFaceProps * m_blockIndexList.size() );

        memcpy((char*)vals.data(), uncompressedData + offset,
               6 * m_header.numFaceProps * m_blockIndexList.size() * sizeof(F32));

        this->setFaceProps(names,vals);

        names.clear();
        vals.clear();
        uncompressedData = NULL;
        compressedData.clear();
        uncompressedStr.clear();
    }

    // Done reading
    input.close();
    
    ProgressOutput() << "IRES: > Done reading '" << filename << "'.\n";

    return true;
}
 
