
#include "ires.h"

#ifdef _WIN32
#include "snappy\snappy.h"  // For compression/decompression.
#else
#include "snappy/snappy.h"
#endif

#include <fstream>
#include <iostream>
#include <algorithm>

bool ires::Ires::writeFile(const std::string& filename)
{
    if ((!m_initialized) || (!m_verticesSet) || (!m_blocksSet))
    {
        if (!m_initialized)
        {
            ProgressOutput() << "IRES: > !!! ERROR: IRES not initialized before calling writeFile().\n";
        }

        if (!m_verticesSet)
        {
            ProgressOutput() << "IRES: > !!! ERROR: Vertices not set before calling writeFile().\n";
        }

        if (!m_blocksSet)
        {
            ProgressOutput() << "IRES: > !!! ERROR: Blocks not set before calling writeFile().\n";
        }

        return false;
    }

    std::string fnString(filename);
    fnString.append(IresExtension);

    std::ofstream output(fnString, std::ios::out | std::ios::binary | std::ios::trunc);

    if (!output.good())
    {
        ProgressOutput() << "IRES: > !!! ERROR: Could not open file for output when calling writeFile().\n";

        return false;
    }


    ProgressOutput() << "\nIRES: > Writing file '" << fnString << "'...\nIRES: > Writing magic number...\n";

    m_header.version = IresVersionNumber;

    //////////////// WRITE OUT MAGIC NUMBER ///////////////////
    char current = '\0';

    // The magic number is 8 bytes long. Have to do some funny stuff because little-endian architecture
    for (int i = 0; i < 8; ++i)
    {
        current = (char) (MagicNumber >> (8*7 - i*8));
        output.write(&current, 1);	// Write one byte at a time
    }

    ProgressOutput() << "IRES: > Writing out header...\n";

    //////////////// WRITE OUT HEADER ///////////////////
    output.write((const char*) &(m_header.version), sizeof(m_header.version));
    output.write((const char*) &(m_header.title), sizeof(m_header.title));
    output.write((const char*) &(m_header.runDate), sizeof(m_header.runDate));
    output.write((const char*) &(m_header.numI), sizeof(m_header.numI));
    output.write((const char*) &(m_header.numJ), sizeof(m_header.numJ));
    output.write((const char*) &(m_header.numK), sizeof(m_header.numK));
    output.write((const char*) &(m_header.numTimestepsTotal), sizeof(m_header.numTimestepsTotal));
    output.write((const char*) &(m_header.numDynamicProps), sizeof(m_header.numDynamicProps));
    output.write((const char*) &(m_header.numStaticProps), sizeof(m_header.numStaticProps));
    output.write((const char*) &(m_header.numFaceProps), sizeof(m_header.numFaceProps));
    output.write((const char*) &(m_header.minX), sizeof(m_header.minX));
    output.write((const char*) &(m_header.minY), sizeof(m_header.minY));
    output.write((const char*) &(m_header.minZ), sizeof(m_header.minZ));
    output.write((const char*) &(m_header.maxX), sizeof(m_header.maxX));
    output.write((const char*) &(m_header.maxY), sizeof(m_header.maxY));
    output.write((const char*) &(m_header.maxZ), sizeof(m_header.maxZ));
    std::streampos dataPositionTMP = output.tellp();
    output.write((const char*) &(m_header.geometryDataPos), sizeof(m_header.geometryDataPos));
    output.write((const char*) &(m_header.dynamicDataPos), sizeof(m_header.dynamicDataPos));
    output.write((const char*) &(m_header.staticDataPos), sizeof(m_header.staticDataPos));
    output.write((const char*) &(m_header.faceDataPos), sizeof(m_header.faceDataPos));

    ///////////////////////////////////////////////////////////////////////////
    /// GEOMETRIC DATA
    ///////////////////////////////////////////////////////////////////////////
    /////////////////// WRITE DYNAMIC PROPERTY HEAD //////////////////////
    // Record geometric property position
    std::streampos geoPos = output.tellp();
    output.seekp(dataPositionTMP);
    U64 geometricPosition = geoPos;   // Make sure it fits a U64 variable.
    m_header.dynamicDataPos = geometricPosition;
    output.write((const char*) &geometricPosition, sizeof(m_header.geometryDataPos));
    dataPositionTMP = output.tellp();	// Property position now points to spot to write static property position in file
    output.seekp(geoPos);

    ProgressOutput() << "IRES: > Compressing vertex data...\n";

    /////////////////// COMPRESS VERTEX DATA ///////////////////
    U32 vertexDataSize = m_vertexList.size() * sizeof(F32);

    U32 uncompressedSize = vertexDataSize + sizeof(U32);

    U32 compressedBytes = 0;

    snappy::string compressedString;

    std::vector<U8> uncompressedData(uncompressedSize);

    U32 numVertexDoubles = m_vertexList.size();

    memcpy(uncompressedData.data(), &numVertexDoubles, sizeof(U32));
    memcpy(uncompressedData.data() + sizeof(U32), m_vertexList.data(), vertexDataSize);

    compressedBytes = snappy::Compress((const char*) uncompressedData.data(), uncompressedSize, &compressedString);
    uncompressedData.clear();

    ProgressOutput() << "IRES: > Writing vertex data...\n";

    /////////////////// WRITE VERTEX DATA //////////////////////
    output.write((const char*) &compressedBytes, sizeof(U32));
    output.write(compressedString.data(), compressedBytes);

    compressedString.clear();
    

    ProgressOutput() << "IRES: > Compressing block data...\n";

    /////////////////// COMPRESS BLOCK DATA ///////////////////
    U32 blockListLengthPlusNullBlocks = m_blockList.size() + std::count(m_blockIndexList.cbegin(), m_blockIndexList.cend(), -1);

    U32 blockDataSize = blockListLengthPlusNullBlocks * sizeof(U32);

    uncompressedSize = blockDataSize + sizeof(U32);

    std::size_t index = 0;

    std::size_t numBlocks = m_blockIndexList.size();

    std::size_t blockSize = 8 * sizeof(U32);

    uncompressedData = std::vector<U8>(uncompressedSize);

    std::vector<U32> blockData(blockListLengthPlusNullBlocks);

    for (std::size_t i = 0; i < numBlocks; ++i)
    {
        if (m_blockIndexList[i] > -1)
        {
            memcpy(&(blockData[index]), &(m_blockList[m_blockIndexList[i]]), blockSize);

            index += 8;
        }
        else
        {
            blockData[index++] = (U32) -1;
        }
    }


    memcpy(uncompressedData.data(), &blockListLengthPlusNullBlocks, sizeof(U32));
    memcpy(uncompressedData.data() + sizeof(U32), blockData.data(), blockDataSize);

    blockData.clear(); // Clear blockData.

    compressedBytes = snappy::Compress((const char*) uncompressedData.data(), uncompressedSize, &compressedString);

    ProgressOutput() << "IRES: > Writing block data...\n";

    /////////////////// WRITE BLOCK DATA //////////////////////
    output.write((const char*) &compressedBytes, sizeof(U32));
    output.write(compressedString.data(), compressedBytes);
    compressedString.clear(); // Clear compressedString.
    uncompressedData.clear(); // Clear uncompressedData.

    ///////////////////////////////////////////////////////////////////////////
    /// DYNAMIC PROPERTIES
    ///////////////////////////////////////////////////////////////////////////
    /////////////////// WRITE DYNAMIC PROPERTY HEAD //////////////////////
    // Record dynamic property position
    std::streampos dynamicPos = output.tellp();
    output.seekp(dataPositionTMP);
    U64 dynamicPosition = dynamicPos;   // Make sure it fits a U64 variable.
    m_header.dynamicDataPos = dynamicPosition;
    output.write((const char*) &dynamicPosition, sizeof(m_header.dynamicDataPos));
    dataPositionTMP = output.tellp();	// Property position now points to spot to write static property position in file
    output.seekp(dynamicPos);

    ProgressOutput() << "IRES: > Preparing dynamic property data...\n";

    ////////////////// PREPARE DYNAMIC PROPERTY DATA //////////////
    uncompressedSize =   m_header.numDynamicProps * MaxStringLength
            + m_header.numDynamicProps * sizeof(U32)
            + numBlocks * m_header.numTimestepsTotal * sizeof(F32) ;

    uncompressedData = std::vector<U8>(uncompressedSize);

    for (std::size_t i = 0; i < m_dynamicNames.size(); ++i)
    {
        for (std::size_t j = 0; j < MaxStringLength; ++j)
        {
            if (j < m_dynamicNames[i].size())
            {
                uncompressedData[i * MaxStringLength + j] = m_dynamicNames[i][j];
            }
            else
            {
                uncompressedData[i * MaxStringLength + j] = 0;
            }
        }
    }

    std::size_t offset = m_header.numDynamicProps * MaxStringLength;

    memcpy(uncompressedData.data() + offset , m_dynamicNumTimesteps.data(),
           m_header.numDynamicProps * sizeof(U32));

    offset += m_header.numDynamicProps * sizeof(U32);

    memcpy(uncompressedData.data() + offset , m_dynamicData.data(),
           numBlocks * m_header.numTimestepsTotal * sizeof(F32));

    ProgressOutput() << "IRES: > Compressing dynamic property data...\n";

    ////////////////// COMPRESS DYNAMIC PROPERTY DATA //////////////
    compressedBytes = snappy::Compress((const char*) uncompressedData.data(), uncompressedSize, &compressedString);
    uncompressedData.clear();

    ProgressOutput() << "IRES: > Writing dynamic property data...\n";

    ////////////////// WRITE OUT COMPRESSED DYNAMIC PROPERTY SIZE //////////////
    output.write((const char*) &compressedBytes, sizeof(U32));

    ////////////////// WRITE OUT DYNAMIC PROPERTY DATA ///////////////
    output.write(compressedString.data(), compressedBytes);
    compressedString.clear();

    ///////////////////////////////////////////////////////////////////////////
    /// STATIC PROPERTIES
    ///////////////////////////////////////////////////////////////////////////
    /////////////////// WRITE STATIC PROPERTY HEAD //////////////////////
    // Record static property position
    std::streampos staticPos = output.tellp();
    output.seekp(dataPositionTMP);
    U64 staticPosition = staticPos;   // Make sure it fits a U64 variable.
    m_header.staticDataPos = staticPosition;
    output.write((const char*) &staticPosition, sizeof(m_header.staticDataPos));
    dataPositionTMP = output.tellp();	// Property position now points to spot to write static property position in file
    output.seekp(staticPos);


    ProgressOutput() << "IRES: > Preparing static property data...\n";

    ////////////////// PREPARE STATIC PROPERTY DATA //////////////
    uncompressedSize = m_header.numStaticProps * MaxStringLength
            + m_header.numStaticProps * numBlocks * sizeof(F32);

    uncompressedData = std::vector<U8> (uncompressedSize);

    for (std::size_t i = 0; i < m_staticNames.size(); ++i)
    {
        for (std::size_t j = 0; j < MaxStringLength; ++j)
        {
            if (j < m_staticNames[i].size())
            {
                uncompressedData[i * MaxStringLength + j] = m_staticNames[i][j];
            }
            else
            {
                uncompressedData[i * MaxStringLength + j] = 0;
            }
        }
    }

    memcpy(uncompressedData.data() + m_header.numStaticProps * MaxStringLength, m_staticData.data(),
           m_header.numStaticProps * numBlocks * sizeof(F32));

    ProgressOutput() << "IRES: > Compressing static property data...\n";

    ////////////////// COMPRESS STATIC PROPERTY DATA //////////////
    compressedBytes = snappy::Compress((const char*) uncompressedData.data(), uncompressedSize, &compressedString);

    uncompressedData.clear();

    ProgressOutput() << "IRES: > Writing static property data...\n";

    ////////////////// WRITE OUT COMPRESSED STATIC PROPERTY SIZE //////////////
    output.write((const char*) &compressedBytes, sizeof(U32));

    ////////////////// WRITE OUT STATIC PROPERTY DATA ///////////////
    output.write(compressedString.data(), compressedBytes);
    compressedString.clear();

    ///////////////////////////////////////////////////////////////////////////
    /// FACE PROPERTIES
    ///////////////////////////////////////////////////////////////////////////
    /////////////////// WRITE FACE PROPERTY HEAD //////////////////////
    // Record Face property position
    std::streampos facePropPos = output.tellp();
    output.seekp(dataPositionTMP);
    U64 facePosition = facePropPos;   // Make sure it fits a U64 variable.
    m_header.faceDataPos = facePosition;
    output.write((const char*) &facePosition, sizeof(m_header.faceDataPos));
    dataPositionTMP = output.tellp();	// Property position now points to spot to write static property position in file
    output.seekp(facePropPos);

    ProgressOutput() << "IRES: > Preparing Face property data...\n";

    ////////////////// PREPARE FACE PROPERTY DATA //////////////
    uncompressedSize =   m_header.numFaceProps * MaxStringLength
            +  6 * numBlocks * m_header.numFaceProps * sizeof(F32);

    uncompressedData = std::vector<U8>(uncompressedSize);

    for (std::size_t i = 0; i < m_faceNames.size(); ++i)
    {
        for (std::size_t j = 0; j < MaxStringLength; ++j)
        {
            if (j < m_faceNames[i].size())
            {
                uncompressedData[i * MaxStringLength + j] = m_faceNames[i][j];
            }
            else
            {
                uncompressedData[i * MaxStringLength + j] = 0;
            }
        }
    }


   offset = m_header.numFaceProps * MaxStringLength ;

    memcpy(uncompressedData.data() + offset , m_faceData.data(),
           6 * numBlocks * m_header.numFaceProps * sizeof(F32) ) ;

    ProgressOutput() << "IRES: > Compressing Face property data...\n";

    ////////////////// COMPRESS FACE PROPERTY DATA //////////////
    compressedBytes = snappy::Compress((const char*) uncompressedData.data(), uncompressedSize, &compressedString);
    uncompressedData.clear();

    ProgressOutput() << "IRES: > Writing Face property data...\n";

    ////////////////// WRITE OUT COMPRESSED FACE PROPERTY SIZE //////////////
    output.write((const char*) &compressedBytes, sizeof(U32));

    ////////////////// WRITE OUT FACE PROPERTY DATA ///////////////
    output.write(compressedString.data(), compressedBytes);
    compressedString.clear();

    // Flush data & close file
    output.close();

    ProgressOutput() << "IRES: > Done writing '" << fnString << "' to disk.\n";

    this->reset();

    return true;
}

