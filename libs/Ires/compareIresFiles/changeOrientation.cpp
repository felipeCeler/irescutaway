#include <ires.h>

#include <vector>
#include <iostream>

int main(int argc , char* argv[])
{
    const char* namefile = "../models/zmap_ires2.ires";
    if( argc > 1 ) namefile = argv[1];

    ires::Ires iresOBJ(true);

    bool readFileResult = false;

    readFileResult = iresOBJ.readFile(namefile);
    std::cerr << "Result of reading file is " <<  (readFileResult?"true":"false") << std::endl ;

    std::vector<ires::U32> blockList = iresOBJ.getBlockList();
    std::vector<int> blockIndexList = iresOBJ.getBlockIndexList();

    for( ires::U32 i = 0 ; i < blockIndexList.size() ; i++ )
    {
        int index = blockIndexList[i];
        if(index >= 0)
        {
            ires::U32 tmp1 = blockList[index+1];
            blockList[index+1] = blockList[index+3];
            blockList[index+3] = tmp1;

            ires::U32 tmp2 = blockList[index+5];
            blockList[index+5] = blockList[index+7];
            blockList[index+7] = tmp2;
        }
    }

    ires::U32 numi, numj, numk;
    iresOBJ.getNumIJK(numi, numj, numk);

    iresOBJ.setBlockList(blockList,blockIndexList,numi, numj, numk);

    std::string newNamefile(namefile);
    unsigned found = newNamefile.find_last_of(".");
    newNamefile = newNamefile.substr(0,found);
    newNamefile +=  "_newOrientation" ;

    bool result_write = iresOBJ.writeFile( newNamefile );
    std::cerr << "Result of write file is " <<  (result_write?"true":"false") << std::endl ;

    return 0;
}

