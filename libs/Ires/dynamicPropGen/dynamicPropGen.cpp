///////////////////////////////////////////////////////////////////////////////
//// dynamic Propierty Generator for Ires 2.1 flexible Grids  ////////////////
/////////////////////////////////////////////////////////////////////////////

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

    bool treeInitResult = false;
    treeInitResult = iresOBJ.initializeKdTree();
    std::cerr << "Result of initializing kdtree is " <<  (treeInitResult?"true":"false") << std::endl ;

    std::string name1("Dynamic Test 01") ;
    std::string name2("Dynamic Test 02") ;
    std::vector<std::string> vnames;
    vnames.push_back(name1);
    vnames.push_back(name2);

    unsigned int numOfTS1 = 93;
    unsigned int numOfTS2 = 134;

    std::vector<unsigned int> vnumberOfTimeSteps;
    vnumberOfTimeSteps.push_back(numOfTS1);
    vnumberOfTimeSteps.push_back(numOfTS2);

    unsigned int numOfbl = iresOBJ.getNumTotalBlocks();

    std::vector<float> vals(numOfbl,100);

    int numOfFires = 17;
    for( int kk = 1 ; kk < numOfFires ; ++kk ) vals[ ((kk * numOfbl) / (numOfFires-1))-1 ] = 500;


    std::vector<float> vvals;
    for( unsigned int j = 0 ; j < numOfbl ; ++j ) vvals.push_back(std::log(vals[j]));


//    std::cout << "set up  01 numOfbl = " << numOfbl << "\n" ;

    for( unsigned int i = 0 ; i < numOfTS1 ; ++i )
    {
        std::vector<float> valsOld(vals) ;
        for( unsigned int j = 0 ; j < numOfbl ; ++j )
        {
            float h = valsOld[j]/7;

            unsigned int idI = 0 , idJ = 0 , idK = 0;
            iresOBJ.getIJKfromIndex( j , idI , idJ , idK ) ;

//				std::cerr << "j = " << j << "   idI = " << idI << "  idJ = " << idJ << "   idK = " << idK << std::endl;

            int index = iresOBJ.getBlockIndex(idI + 1 , idJ , idK) ;
            if( index > -1 ) h += valsOld[index]/7;
            else h += valsOld[j]/7;

            index = iresOBJ.getBlockIndex(idI - 1, idJ , idK) ;
            if( index > -1 ) h += valsOld[index]/7;
            else h += valsOld[j]/7;

            index = iresOBJ.getBlockIndex(idI , idJ + 1 , idK) ;
            if( index > -1 ) h += valsOld[index]/7;
            else h += valsOld[j]/7;

            index = iresOBJ.getBlockIndex(idI , idJ - 1 , idK) ;
            if( index > -1 ) h += valsOld[index]/7;
            else h += valsOld[j]/7;

            index = iresOBJ.getBlockIndex(idI , idJ , idK + 1 ) ;
            if( index > -1 ) h += valsOld[index]/7;
            else h += valsOld[j]/7;

            index = iresOBJ.getBlockIndex(idI , idJ , idK - 1 ) ;
            if( index > -1 ) h += valsOld[index]/7;
            else h += valsOld[j]/7;

            vals[j] = h ;
        }
        for( int kk = 1 ; kk < numOfFires ; ++kk ) vals[ ((kk * numOfbl) / (numOfFires-1))-1 ] = 500;
        for( unsigned int j = 0 ; j < numOfbl ; ++j ) vvals.push_back(std::log(vals[j]));
    }

    bool addedProp = iresOBJ.addDynamicProp( name1 , vvals ) ;


    std::cerr << "Result add prop 1 is " <<  (addedProp?"true":"false") << std::endl ;

    vvals.clear();
//    std::cout << "vvals.size() = " <<  vvals.size() << std::endl ;
//    std::cout << "set up  02\n" ;

    vals = std::vector<float> (numOfbl,100);
    std::vector<unsigned> blockIds;
    unsigned blockID, k = 128;

    for( int i = 1 ; i < numOfFires ; ++i )
    {
        blockID = ((i*numOfbl)/(numOfFires))-1;

        std::cout << "BlockID =  = " << blockID << "\n" ;


        iresOBJ.getKNearestBlockIds(blockID, k, blockIds);

        for (auto it = blockIds.begin(); it != blockIds.end(); ++it) vals[*it] = 500 ;
    }

    for( unsigned int j = 0 ; j < numOfbl ; ++j ) vvals.push_back(std::log(vals[j]));


    for( unsigned int i = 0 ; i < numOfTS2-1 ; ++i )
    {
        std::vector<float> valsOld(vals) ;
        for( unsigned int j = 0 ; j < numOfbl ; ++j )
        {
            float h = valsOld[j]/3;

            unsigned int idI = 0 , idJ = 0 , idK = 0;
            iresOBJ.getIJKfromIndex( j , idI , idJ , idK ) ;

            int index = iresOBJ.getBlockIndex(idI + 1 , idJ , idK) ;
            if( index > -1 ) h += valsOld[index]/9;
            else h += valsOld[j]/9;
            index = iresOBJ.getBlockIndex(idI - 1, idJ , idK) ;
            if( index > -1 ) h += valsOld[index]/9;
            else h += valsOld[j]/9;

            index = iresOBJ.getBlockIndex(idI , idJ + 1 , idK) ;
            if( index > -1 ) h += valsOld[index]/9;
            else h += valsOld[j]/9;
            index = iresOBJ.getBlockIndex(idI , idJ - 1 , idK) ;
            if( index > -1 ) h += valsOld[index]/9;
            else h += valsOld[j]/9;

            index = iresOBJ.getBlockIndex(idI , idJ , idK + 1 ) ;
            if( index > -1 ) h += valsOld[index]/9;
            else h += valsOld[j]/9;
            index = iresOBJ.getBlockIndex(idI , idJ , idK - 1 ) ;
            if( index > -1 ) h += valsOld[index]/9;
            else h += valsOld[j]/9;

            vals[j] = h ;
            vvals.push_back(std::log(vals[j]));
        }
    }

    std::cout << "vvals.size() = " <<  vvals.size() << std::endl ;

    addedProp = iresOBJ.addDynamicProp( name2 , vvals ) ;


    std::cerr << "Result add prop 2 is " <<  (addedProp?"true":"false") << std::endl ;


//    iresOBJ.setDynamicProps(vnames,vnumberOfTimeSteps,vvals);

    std::string newNamefile(namefile);
    unsigned found = newNamefile.find_last_of(".");
    newNamefile = newNamefile.substr(0,found);
    newNamefile +=  "_DynamicProp" ;

    bool result_write = iresOBJ.writeFile( newNamefile );
    std::cerr << "Result of write file is " <<  (result_write?"true":"false") << std::endl ;

    return 0;
}
