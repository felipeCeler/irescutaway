/****************
 * This file is intended to just test out the IRES v2.0 library.
 *
 *
 ****************/

#include "ires.h"

#include <vector>
#include <iostream>
#include <ctime>

#include <iomanip>      // std::setprecision

void testStaticProps( ires::Ires& gridIres )
{
    ires::U32 numi, numj, numk;
    gridIres.getNumIJK(numi, numj, numk);
    ires::U32 numberOfBlocks = numi * numj * numk;

    std::vector<ires::F32> valueStaticProp ;
    std::vector<std::string> names ;

    gridIres.getStaticPropertyNames( names ) ;

    ires::U32 nDP = names.size();

    if( nDP < 1 )
    {
        std::cout << "\nFile without STATIC PROPS \n";
        return;
    }

    std::cout << "\n//////////////////////////////////////////////////////////////////////////////////\n";
    std::cout << "//////////////////////////////// STATIC PROPS ///////////////////////////////////\n";
    std::cout << "/////////////////////////////////////////////////////////////////////////////////\n";

    bool isGetingRightIndex = true ;

    for( int i = 0 ; i < (int) nDP ; ++i )
    {
        isGetingRightIndex = isGetingRightIndex & ( gridIres.getStaticPropertyIndex( names[i] ) == i );
        std::cout <<  names[i] << "   -> names[i].size() = " << names[i].size() << std::endl;
    }

    isGetingRightIndex = isGetingRightIndex & ( gridIres.getStaticPropertyIndex("PQP^3@3#$dfee" ) == -1 ) ;

    std::cout << "isGetingRightIndex = " << (isGetingRightIndex?"true":"false") << std::endl ;

    for( ires::U32  j = 0 ; j < nDP ; j++ )
    {
        std::cout << "Static Prop " << j << " " << names[j] ;

        gridIres.getStaticPropertyValues( j , valueStaticProp );
        std::cout << "\t val[ 0 ] " << valueStaticProp[ 0 ] <<
                     "\t val[n/3] " << valueStaticProp[ numberOfBlocks/3 ] <<
                     "\t val[n/2] " << valueStaticProp[ numberOfBlocks/2 ] <<
                     "\t val[ n ] " << valueStaticProp[ numberOfBlocks-1 ] <<
                     std::endl;
    }
    std::cout.flush();
}

void testFaceProps( ires::Ires& gridIres )
{
    ires::U32 numi, numj, numk;
    gridIres.getNumIJK(numi, numj, numk);
    ires::U32 numberOfBlocks = numi * numj * numk;

    std::vector<ires::F32> valueFaceProp ;
    std::vector<std::string> names ;

    gridIres.getFaceNames( names ) ;

    ires::U32 nDP = names.size();
    if( nDP < 1 )
    {
        std::cout << "\nFile without FACE PROPS \n";
        return;
    }

    std::cout << "\n//////////////////////////////////////////////////////////////////////////////////\n";
    std::cout << "//////////////////////////////// FACE PROPS //////////////////////////////////////\n";
    std::cout << "/////////////////////////////////////////////////////////////////////////////////\n";

    bool isGetingRightIndex = true ;


    for( int i = 0 ; i < (int) nDP ; ++i )
    {
        isGetingRightIndex = isGetingRightIndex & ( gridIres.getFacePropertyIndex( names[i] ) == i );
        std::cout <<  names[i] << "   -> names[i].size() = " << names[i].size() << std::endl;
    }
    isGetingRightIndex = isGetingRightIndex & ( gridIres.getFacePropertyIndex("FDP^2@3#$dfee" ) == -1 ) ;

    std::cout << "isGetingRightIndex = " << (isGetingRightIndex?"true":"false") << std::endl ;

    for( ires::U32  j = 0 ; j < nDP ; j++ )
    {
        std::cout << "Face Prop " << j << " " << names[j] << std::endl ;

        gridIres.getFacePropertyValues( j , valueFaceProp );
        std::cout << "0 \tBottom " << valueFaceProp[ 0 + ires::Face::BOTTOM ] <<
                     "\tTop "   << valueFaceProp[ 0 + ires::Face::TOP ] <<
                     "\tRight " << valueFaceProp[ 0 + ires::Face::RIGHT ] <<
                     "\tLeft "  << valueFaceProp[ 0 + ires::Face::LEFT ] <<
                     "\tFront " << valueFaceProp[ 0 + ires::Face::FRONT ] <<
                     "\tBack "  << valueFaceProp[ 0 + ires::Face::BACK ] << std::endl;
        std::cout << "n/2 \tBottom " << valueFaceProp[ 6*numberOfBlocks/2 + ires::Face::BOTTOM ] <<
                     "\tTop " << valueFaceProp[ 6*numberOfBlocks/2  + ires::Face::TOP ] <<
                     "\tRight " << valueFaceProp[ 6*numberOfBlocks/2  + ires::Face::RIGHT ] <<
                     "\tLeft " << valueFaceProp[ 6*numberOfBlocks/2  + ires::Face::LEFT ] <<
                     "\tFront " << valueFaceProp[ 6*numberOfBlocks/2  + ires::Face::FRONT ] <<
                     "\tBack " << valueFaceProp[ 6*numberOfBlocks/2  + ires::Face::BACK ] << std::endl;
        std::cout << "n \tBottom " << valueFaceProp[ 6*(numberOfBlocks-1)/2 + ires::Face::BOTTOM ] <<
                     "\tTop " << valueFaceProp[ 6*(numberOfBlocks-1)/2  + ires::Face::TOP ] <<
                     "\tRight " << valueFaceProp[ 6*(numberOfBlocks-1)/2  + ires::Face::RIGHT ] <<
                     "\tLeft " << valueFaceProp[ 6*(numberOfBlocks-1)/2  + ires::Face::LEFT ] <<
                     "\tFront " << valueFaceProp[ 6*(numberOfBlocks-1)/2  + ires::Face::FRONT ] <<
                     "\tBack " << valueFaceProp[ 6*(numberOfBlocks-1)/2  + ires::Face::BACK ] << std::endl;
    }

    std::cout.flush();
}

void testDynamicProps( ires::Ires& gridIres )
{
    ires::U32 numi, numj, numk;
    gridIres.getNumIJK(numi, numj, numk);
    ires::U32 numberOfBlocks = numi * numj * numk;

    std::vector<ires::F32> valueDynamicProp;
    std::vector<std::string> names ;

    gridIres.getDynamicPropertyNames( names );

    ires::U32 nDP = names.size();

    if( nDP < 1 )
    {
        std::cout << "\nFile without DYNAMIC PROPS \n";
        return;
    }

    std::cout << "\n/////////////////////////////////////////////////////////////////////////////////\n";
    std::cout << "//////////////////////////////// DYNAMIC PROPS //////////////////////////////////\n";
    std::cout << "/////////////////////////////////////////////////////////////////////////////////\n";

    std::vector<ires::U32> numOfTimeSteps ;
    gridIres.getDynamicPropertyNumTimesteps( numOfTimeSteps );

    bool isGetingRightIndex = true ;

    for( int i = 0 ; i < (int) nDP ; ++i )
    {
        isGetingRightIndex = isGetingRightIndex & ( gridIres.getDynamicPropertyIndex( names[i] ) == i ) ;
        std::cout <<  names[i] << "   -> names[i].size() = " << names[i].size() << std::endl;
    }
    isGetingRightIndex = isGetingRightIndex & ( gridIres.getDynamicPropertyIndex("FDS@3#$dfee" ) == -1 ) ;

    std::cout << "isGetingRightIndex = " << (isGetingRightIndex?"true":"false") << std::endl ;

    for( ires::U32  j = 0 ; j < nDP ; j++ )
    {
        ires::U32 nTS = numOfTimeSteps[j] ;
        std::cout << "Dynamic Prop " << j << " " << names[j]
                  << "  has " << nTS << std::endl;

        for( ires::U32  timeStep = 0 ; timeStep < nTS ; timeStep += nTS/10 )
        {
            gridIres.getDynamicPropertyValues( j , timeStep , valueDynamicProp );
            std::cout << "Time Step = " << timeStep <<
                         "\t val[ 0 ] " << valueDynamicProp[ 0 ] <<
                         "\t val[n/3] " << valueDynamicProp[ numberOfBlocks/3 ] <<
                         "\t val[n/7] " << valueDynamicProp[ numberOfBlocks/7 ] <<
                         "\t val[n/5] " << valueDynamicProp[ numberOfBlocks/5 ] <<
                         "\t val[n/2] " << valueDynamicProp[ numberOfBlocks/2 ] <<
                         "\t val[ n ] " << valueDynamicProp[ numberOfBlocks-1 ] <<
                         std::endl;
        }
    }
    std::cout << "/////////////////////////////////////////////////////////////////////////////////\n";
    for( ires::U32  j = 0 ; j < nDP ; j++ )
    {
        ires::U32 nTS = numOfTimeSteps[j] ;
        std::cout << "Dynamic Prop " << j << "   = " << names[j]
                  << "  has " << nTS << std::endl;

        gridIres.getDynamicPropertyValues( j , -1 , valueDynamicProp );

        for( ires::U32  timeStep = 0 ; timeStep < nTS ; timeStep += nTS/10 )
        {
            ires::U32 tsIndex = timeStep*numberOfBlocks;
            std::cout << "Time Step = " << timeStep <<
                         "\t val[ 0 ] " << valueDynamicProp[ tsIndex ] <<
                         "\t val[n/3] " << valueDynamicProp[ tsIndex + numberOfBlocks/3 ] <<
                         "\t val[n/2] " << valueDynamicProp[ tsIndex + numberOfBlocks/2 ] <<
                         "\t val[ n ] " << valueDynamicProp[ tsIndex + numberOfBlocks-1 ] <<
                         std::endl;
        }
    }
    std::cout.flush();
}

void testFaceOutput( ires::Ires& gridIres , std::vector<ires::Face> faces , std::vector<ires::F32>  vertices )
{
    ires::U32 numberOfFaces = faces.size();

    for( ires::U32  i = 0 ; i < numberOfFaces ; i += numberOfFaces/13 )
    {
        ires::U32 blockID = faces[i].id;

        ires::U32 I = 0, J = 0 , K = 0 ;

        gridIres.getIJKfromIndex( blockID , I , J , K );

        std::cout << " face index = " << i <<
                     " \t blockID " << blockID <<
                     " \t ( I , J , K ) = ( " << I << ", "<< J << ", "<< K << " )"
                     " \ta = ( " << vertices[ 3*faces[i].a     ] <<
                     ", " << vertices[ 3*faces[i].a + 1 ] <<
                     ", " << vertices[ 3*faces[i].a + 2 ] <<
                     " ) d = ( " << vertices[ 3*faces[i].d     ] <<
                     ", " << vertices[ 3*faces[i].d + 1 ] <<
                     ", " << vertices[ 3*faces[i].d + 2 ] << " )" <<
                     std::endl;
    }


    std::cout << "/////////////////////////////////////////////////////////////////////////////////\n";

    std::vector<ires::F32>   valuefaceProp ;
    std::vector<std::string> names ;
    gridIres.getFaceNames( names ) ;

    if( names.size()>0)
    {
        for( unsigned j = 0 ; j < names.size() ; ++j )
        {
            std::cout << "\n++++++++++++++++ "  << names[j] << " ++++++++++++++++\n";

            gridIres.getFacePropertyValues( j , valuefaceProp );
            for( ires::U32  i = 0 ; i < numberOfFaces ; i += numberOfFaces/11 )
            {
                ires::Face::FACE_BLOCK_POS poss = faces[i].faceBlockRelPos;
                std::string possName = ires::Face::faceBlockPos2string(poss);
                ires::U32 blockID = faces[i].id;
                std::cout << "blockID = " << blockID <<
                             " \t val[" << possName << "] " << valuefaceProp[ blockID*6 + poss ] <<
                             " \t val[ FRONT ] " << valuefaceProp[ blockID*6 + ires::Face::FRONT  ] <<
                             " \t val[ BACK  ] " << valuefaceProp[ blockID*6 + ires::Face::BACK  ] <<
                             " \t val[ TOP   ] " << valuefaceProp[ blockID*6 + ires::Face::TOP  ] <<
                             std::endl;
            }
        }
    }
    else {
        std::cout << "No face proper to be tested \n";
    }

    std::cout << "/////////////////// FAULT TEST ////////////////////////////////////////\n";

    int index = gridIres.getFacePropertyIndex("Geometric Fault") ;
    if( index > 0 )
    {
        ires::U32 count = 0 ;
        gridIres.getFacePropertyValues( index , valuefaceProp );
        for( ires::U32  i = 0 ; i < numberOfFaces ; i++ )
        {
            ires::Face faceTMP = faces[i];
            ires::Face::FACE_BLOCK_POS poss = faceTMP.faceBlockRelPos;
            ires::U32 blockID = faceTMP.id;
            ires::F32 val = valuefaceProp[ blockID*6 + poss ] ;


            if( val > 0 ) count++;
        }
        std::cout << "This grid has " << count << " Faces marked as Fault in Prop  " << names[index] << std::endl ;
    }
    else {
        std::cout << "No prop Geometric Fault\n";
    }

    std::cout.flush();

}

void testFaceGenarate( ires::Ires& gridIres )
{
    std::cout << "\n/////////////////////////////////////////////////////////////////////////////////\n";
    std::cout << "//////////////////////////////// FACE GENERATE TOTAL /////////////////////////////\n";
    std::cout << "/////////////////////////////////////////////////////////////////////////////////\n";

    std::vector<ires::Face> faces;
    std::vector<ires::F32>  vertices = gridIres.getVertexList() ;

    gridIres.generateFaceList(faces);
    testFaceOutput( gridIres , faces , vertices ) ;

    std::cout << "\n/////////////////////////////////////////////////////////////////////////////////\n";
    std::cout << "//////////////////////////////// FACE GENERATE I J K /////////////////////////////\n";
    std::cout << "/////////////////////////////////////////////////////////////////////////////////\n";

    ires::U32 numi, numj, numk;
    gridIres.getNumIJK(numi, numj, numk);

    gridIres.generateFaceList( 0 , 0 , 0 , numi-1 , numj-1 , numk-1 , faces );
//    gridIres.generateFaceList( numi/4 , numj/4 , numk/4 , numi/2 , numj/2 , numk/2 , faces );
    testFaceOutput( gridIres , faces , vertices ) ;


    std::cout << "\n/////////////////////////////////////////////////////////////////////////////////\n";
    std::cout << "///////////////////////////// FACE GENERATE BLOCK INDEX ///////////////////////////\n";
    std::cout << "/////////////////////////////////////////////////////////////////////////////////\n";
    std::vector<ires::U32> blockIndex;
    ires::U32 numberOfBlocks = numi * numj * numk;

    for( ires::U32 i = (numberOfBlocks/500) ; i < (numberOfBlocks/250) ; ++i ) blockIndex.push_back(i);


    std::cout<< "numberOfBlocks = " << numberOfBlocks << " Range = " << (*blockIndex.begin()) << "  - " << (blockIndex.at( blockIndex.size()-1 ) ) << std::endl ;

    gridIres.generateFaceList( blockIndex , faces );

    std::cout<<"blockIndex.size() = " << blockIndex.size() << "  - faces.size() = " << faces.size() << std::endl ;


    testFaceOutput( gridIres , faces , vertices ) ;

}

void testKDTree( ires::Ires& gridIres )
{

    ires::U32 numi, numj, numk;
    gridIres.getNumIJK(numi, numj, numk);

    std::cout << "\n/////////////////////////////////////////////////////////////////////////////////\n";
    std::cout << "/////////////////////////////////// KDTREE ///////////////////////////////////////\n";
    std::cout << "/////////////////////////////////////////////////////////////////////////////////\n";

    bool treeInitResult = false;

    clock_t startTime = std::clock();
    treeInitResult = gridIres.initializeKdTree();
    clock_t endTime = std::clock();

    std::cout << "Result of initializing kdtree is  " << (treeInitResult?"true":"false")
              << "  in  " << ( endTime - startTime)/float(CLOCKS_PER_SEC) << " seconds.\n\n" ;;

    std::vector<unsigned> blockIds;
    unsigned blockID = 0, k = 0;
    int tmpBlockID = 0, tmpK = 0;

    float centroid[3];
    float verts[8*3];

    while (true)
    {
        std::cout << "\nEnter block index \n(if it is less than zero breaks the loop): ";
        std::cout.flush();
        std::cin >> tmpBlockID;

        if( tmpBlockID < 0 ) break ;
        blockID = (unsigned)tmpBlockID;

        while (!gridIres.getBlockVertices(blockID, verts))
        {
            std::cout << "\nInvalid block. Enter block index: ";
            std::cout.flush();
            std::cin >> blockID;
        }

        std::cout << "\nEnter K \n(if it is less than zero breaks the loop): ";
        std::cout.flush();
        std::cin >> tmpK;

        if( tmpK < 0 ) break;
        k = tmpK;

        gridIres.getCentroid(blockID, centroid);
        std::cout << "\nCentroid = ( " <<  centroid[0] << ", " << centroid[1] << ", " << centroid[2]  << " ). Getting blocks...\n\n" ;

        startTime = std::clock();
        gridIres.getKNearestBlockIds(blockID, k, blockIds);
        endTime = std::clock();

        std::cout << "Retrieval took  " << (endTime - startTime)/float(CLOCKS_PER_SEC) << "  seconds.\n\n" ;

        for (auto it = blockIds.begin(); it != blockIds.end(); ++it)
        {
            gridIres.getCentroid((*it), centroid);
            std::cout << "BlockID= "<< (*it) << "  centroid = ( " <<  centroid[0] << ", "<<  centroid[1] << ", "<<  centroid[2] << " )\n" ;
        }

        std::cout << "Retrieval took  " << (endTime - startTime)/float(CLOCKS_PER_SEC) << "  seconds.\n\n" ;

        std::cout.flush();
    }
}

int main(int argc , char* argv[])
{
    std::cout << std::setprecision(2);
    std::cout << std::fixed;

    ires::Ires gridIres(true);

    bool result;

    const char* namefile = "../models/zmap_ires2.ires";
    if( argc > 1 ) namefile = argv[1];

    result = gridIres.readFile( namefile );
    std::cout << "Result of reading file is "<< (result?"true":"false") << std::endl;

    testDynamicProps(gridIres) ;
    testStaticProps(gridIres) ;
    testFaceProps(gridIres) ;
    testFaceGenarate(gridIres);
//    testKDTree(gridIres);



    return 0;
}
