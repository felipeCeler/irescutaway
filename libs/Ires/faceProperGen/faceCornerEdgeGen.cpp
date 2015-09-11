///////////////////////////////////////////////////////////////////////////////
/// Face Propierty Generator for Ires 2.1 flexible Grids //////////////////////
///////////////////////////////////////////////////////////////////////////////
/// This file will save the faces that are corner using binary code for each
/// pair of vertices in the following order |ab|bc|cd|da.
/// Example: if the edge bc and da are corner then bc = 4 da = 1; 5 will be
///          the value of the face prop
///
/// One way to know wich edge are corners given a value V is:
///         if ( V >= 8 ) then ab is corner and V -= 8
///         if ( V >= 4 ) then bc is corner and V -= 4
///         if ( V >= 2 ) then cd is corner and V -= 2
///         if ( V == 1 ) then da is corner
///
/////////////
///
/// In addition, it creates a face prop based on in it geometry, using 1
/// if the face is a fault face and 0 if it is not.
///
/////////////////////////////////////////////////////////////////////////////

#include <ires.h>

#include <vector>
#include <iostream>
#include <cassert>

 struct Line
 {
     ires::U32 a, b;


     Line (  )
     {
	   this->a = 0;
	   this->b = 0;
     }
     Line( const Line& l )
     {
	     *this = l ;
     }

     Line& operator=( const Line& l )
     {
	     this->a = l.a;
	     this->b = l.b;

	     return (*this);
     }

     // Makes sure the smallest vertex is 'a'
     inline void set(const ires::U32 _a, const ires::U32 _b)
     {
         // Make sure smallest int in this line is _a_
         // This makes comparing with other lines easier

         if (_a <= _b)
         {
             a = _a;
             b = _b;
         }
         else
         {
             a = _b;
             b = _a;
         }
     }


     // Equality operator.
     inline bool operator==(const Line& rhs) const
     {
         return ( (a == rhs.a) && (b == rhs.b) );
     }
     inline bool operator!=(const Line& rhs) const
     {
         return ( !( (*this) == rhs ) );
     }
 };

 float getFaceLines( ires::Face face , std::vector<ires::Face> neighbors )
{
     std::vector<Line> lineNeighbor;

     for( ires::U32 i = 0 ; i < neighbors.size() ; ++i )
     {
         if( neighbors[i] != face  )
         {
             if( neighbors[i].isExtern )
             {
        	     Line tmp;
        	     tmp.set( neighbors[i].a , neighbors[i].b );
        	     lineNeighbor.push_back(tmp);

        	     tmp.set( neighbors[i].b , neighbors[i].c );
        	     lineNeighbor.push_back(tmp);

        	     tmp.set( neighbors[i].c , neighbors[i].d );
        	     lineNeighbor.push_back(tmp);

        	     tmp.set( neighbors[i].d , neighbors[i].a );
        	     lineNeighbor.push_back(tmp);
             }
         }

     }
     ires::F32 result = 0;

     Line lineFace;
     bool hasCommomLine = false;
     lineFace.set(face.a,face.b);
     for( ires::U32 i = 0 ; i < lineNeighbor.size() ; ++i )
     {
         hasCommomLine = hasCommomLine || ( lineNeighbor[i] == lineFace ) ;
     }
     if(hasCommomLine) result += 8 ;

     hasCommomLine = false;
     lineFace.set(face.b,face.c);
     for( ires::U32 i = 0 ; i < lineNeighbor.size() ; ++i )
     {
         hasCommomLine = hasCommomLine || ( lineNeighbor[i] == lineFace ) ;
     }
     if(hasCommomLine) result += 4 ;

     hasCommomLine = false;
     lineFace.set(face.c,face.d);
     for( ires::U32 i = 0 ; i < lineNeighbor.size() ; ++i )
     {
         hasCommomLine = hasCommomLine || ( lineNeighbor[i] == lineFace ) ;
     }
     if(hasCommomLine) result += 2 ;

     hasCommomLine = false;
     lineFace.set(face.d,face.a);
     for( ires::U32 i = 0 ; i < lineNeighbor.size() ; ++i )
     {
         hasCommomLine = hasCommomLine || ( lineNeighbor[i] == lineFace ) ;
     }
     if(hasCommomLine) result += 1 ;

     return result;

 }

int main(int argc , char* argv[])
{

    std::string name1("Face Corner") ;

    std::string name2("Geometric Fault") ;

    const char* namefile = "../models/zmap_ires2.ires";
    if( argc > 1 ) namefile = argv[1];

    ires::Ires iresOBJ(true);

    bool readFileResult = false;

    readFileResult = iresOBJ.readFile(namefile);
    std::cerr << "Result of reading file is " <<  (readFileResult?"true":"false") << std::endl ;

    std::vector<ires::Face> quadFacesList ;
    iresOBJ.generateFaceList( quadFacesList ) ;

    unsigned int numOfbl = iresOBJ.getNumTotalBlocks();

    std::vector<ires::F32> faceProper(6*numOfbl,0);


    for( unsigned i = 0 ; i < quadFacesList.size() - 6 ; i += 6 )
    {

         assert( (5 * quadFacesList.at( i + ires::Face::BOTTOM ).id) ==
                    ( quadFacesList.at( i + ires::Face::TOP  ).id +
                      quadFacesList.at( i + ires::Face::LEFT ).id +
                      quadFacesList.at( i + ires::Face::RIGHT).id +
                      quadFacesList.at( i + ires::Face::FRONT).id +
                      quadFacesList.at( i + ires::Face::BACK ).id));

        std::vector<ires::Face> cubeExternFaces;
        for( ires::U32 pos = 0 ; pos < 6 ; pos++ )
        {
            ires::Face faceTmp = quadFacesList.at( i + pos ) ;
            if( faceTmp.isExtern && faceTmp.faceBlockRelPos >= 0 )
                cubeExternFaces.push_back( faceTmp );
        }

        if( cubeExternFaces.size() > 1 )
        {
            for(  ires::U32 j = 0 ; j < cubeExternFaces.size() ; ++j )
            {
                ires::Face faceTmp = cubeExternFaces[j] ;
                ires::F32 value = getFaceLines( faceTmp , cubeExternFaces );
                faceProper[ 6*faceTmp.id + faceTmp.faceBlockRelPos ] = value;
            }
        }

    }

    bool addedProp = iresOBJ.addFaceProp( name1 , faceProper );

    std::cerr << "The result of adding " <<  name1 << " is " <<  (addedProp?"true":"false") << std::endl ;

    std::vector<ires::F32> faceProper2(6*numOfbl,0);


    ires::U32 numi, numj, numk;
      iresOBJ.getNumIJK(numi, numj, numk);
      ires::U32 numberOfBlocks = numi * numj * numk;

    //  Gives vertices ordered v0.x,v0.y,v0.z, v1.x ...
    //  where vertex numbering is like so:
    //
    //      v7-------v6                      +Y / -K
    //      /  Top   /|                      |
    //     /        / |                      |
    //    v4-------v5 v2                     ---------+X / +I
    //    | Front  | /                      /
    //    |        |/                      /
    //    v0-------v1                    +Z / +J

    for( unsigned i = 0 ; i < quadFacesList.size() ; i++ )
    {
            ires::Face tmpFace = quadFacesList[i];

            int index = tmpFace.id ;

            unsigned int idI = 0 , idJ = 0 , idK = 0;
            iresOBJ.getIJKfromIndex( index , idI , idJ , idK ) ;

        if( tmpFace.isExtern && idI != 0 && idI != numi-1 && idJ != 0 && idJ != numj-1 && idK != 0 && idK != numk-1)
        {
            switch ( tmpFace.faceBlockRelPos ) {
            case ires::Face::BOTTOM:
                idK++;
                break;
            case ires::Face::TOP:
                idK--;
                break;
            case ires::Face::LEFT:
                idI++;
                break;
            case ires::Face::RIGHT:
                idI--;
                break;
            case ires::Face::FRONT:
                idJ--;
                break;
            case ires::Face::BACK:
                idJ++;
                break;
            default:
                break;
            }

            ires::F32 fault = 0 ;
            if ( iresOBJ.isValidBlock(idI , idJ , idK) ) fault = 1.0;
            faceProper2[ 6*tmpFace.id + tmpFace.faceBlockRelPos ] = fault;
        }
    }

     addedProp = iresOBJ.addFaceProp( name2 , faceProper2 );

     std::cerr << "The result of adding " <<  name2 << " is " <<  (addedProp?"true":"false") << std::endl ;


    std::string newNamefile(namefile);
    unsigned found = newNamefile.find_last_of(".");
    newNamefile = newNamefile.substr(0,found);
    newNamefile +=  "_NEW01_FaceProp" ;

    bool result_write = iresOBJ.writeFile( newNamefile );
    std::cerr << "Result of write file is " <<  (result_write?"true":"false") << std::endl ;
}

