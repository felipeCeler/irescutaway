///////////////////////////////////////////////////////////////////////////////
//// dynamic Propierty Generator for Ires 2.1 flexible Grids  ////////////////
/////////////////////////////////////////////////////////////////////////////

#include <ires.h>

#include <vector>
#include <iostream>
#include <cassert>

int main(int argc , char* argv[])
{
    const char* namefile1 = "../models/zmap_ires2.ires";
    const char* namefile2 = "../models/zmap_ires2.ires";
    if( argc > 2 )
    {
        namefile1 = argv[1];
        namefile2 = argv[2];
    }

    ires::Ires iresOBJ1(true);
    ires::Ires iresOBJ2(true);

    bool readFileResult = false;

    readFileResult = iresOBJ1.readFile(namefile1);
    std::cerr << "Result of reading file is " <<  (readFileResult?"true":"false") << std::endl ;

    bool treeInitResult = false;
    treeInitResult = iresOBJ1.initializeKdTree();
    std::cerr << "Result of initializing kdtree is " <<  (treeInitResult?"true":"false") << std::endl ;

    readFileResult = false;

    readFileResult = iresOBJ2.readFile(namefile2);
    std::cerr << "Result of reading file is " <<  (readFileResult?"true":"false") << std::endl ;

    treeInitResult = false;
    treeInitResult = iresOBJ2.initializeKdTree();
    std::cerr << "Result of initializing kdtree is " <<  (treeInitResult?"true":"false") << std::endl ;


    ires::U32 numi, numj, numk;
    iresOBJ1.getNumIJK(numi, numj, numk);
    ires::U32 numberOfBlocks1 = numi * numj * numk;

    iresOBJ2.getNumIJK(numi, numj, numk);
    ires::U32 numberOfBlocks2 = numi * numj * numk;

    assert( numberOfBlocks1 == numberOfBlocks2  );

    std::cout << "\n/////////////////////////////////////////////////////////////////////////////////\n";
    std::cout << "//////////////////////////////// FACE GENERATE /////////////////////////////////////\n";
    std::cout << "/////////////////////////////////////////////////////////////////////////////////\n";

    std::vector<ires::Face> faces1;
    std::vector<ires::F32>  vertices1;

    iresOBJ1.generateFaceList(faces1,vertices1);

    std::vector<ires::Face> faces2;
    std::vector<ires::F32>  vertices2;

    iresOBJ1.generateFaceList(faces2,vertices2);

    for( ires::U32  i = 0 ; i < 6*numberOfBlocks1 ; i++ )
    {
        ires::U32 blockID1 = faces1[i].id;
        ires::U32 blockID2 = faces2[i].id;

        if( blockID1 != blockID2 )
        {
            std::cout << blockID1 << " != " << blockID2 << std::endl;
        }
    }

}
