#ifndef PRIMITIVESIRES_H
#define PRIMITIVESIRES_H

#include <cstdint>  // For typedefs, below.
#include <string>
#include <iostream>
#include <sstream>
#include <cmath>
#include <memory> // For smart pointers.
#include <vector>
#include <cstring>  // For memset.
#include <unordered_set>
#include <exception>
#include <functional> // For hash.
#include <cfloat>
#include <limits>

namespace ires
{

#ifdef _WIN32
    typedef std::uint32_t U32;
    typedef std::uint64_t U64;
#else
    typedef uint32_t U32;
    typedef uint64_t U64;
#endif
    typedef float F32;
    typedef unsigned char U8;
    typedef double D64;

    // This class is used only with the IresHeader
    // struct (converting Dates to/from integers).
    // We use this so that runDate maintains a sensible value
    // (so avoid touching runDate directly).
    class Date
    {
    public:
        struct Day
        {
            explicit Day(U32 day) : value(day)
            {
                if ((day > 31) || (day < 1))
                {
                    value = 1;
                }
            }

            U32 val()
            {
                return value;
            }

        private:
            U32 value;
        };

        struct Month
        {
            explicit Month(U32 month) : value(month)
            {
                if ((month > 12) || (month < 1))
                {
                    value = 1;
                }
            }

            U32 val()
            {
                return value;
            }

        private:
            U32 value;
        };

        struct Year
        {
            explicit Year(U32 year) : value(year)
            {
                if ((year > 3000) || (year < 1500))
                {
                    value = 1500;
                }
            }

            U32 val()
            {
                return value;
            }

        private:
            U32 value;
        };



        Date(Year y, Month m, Day d)
            : m_asInt(0), m_year(y), m_month(m), m_day(d)
        {
            m_asInt = m_year.val() * 10000
                    + m_month.val() * 100
                    + m_day.val();
        }

        explicit Date(U32 dateInt)
            : m_asInt(dateInt),
              m_year(0), m_month(0), m_day(0)
        {
            if ((dateInt < 15000101)
                    || (dateInt > 30001231))
            {
                m_asInt = 15000101;
            }
            else
            {
                m_year = Year(dateInt / 10000);
                m_month = Month((dateInt - m_year.val()) / 100);
                m_day = Day(dateInt - m_year.val() - m_month.val());
            }
        }

        inline U32 getAsInt()
        {
            return m_asInt;
        }

    private:
        U32 m_asInt;
        Year m_year;
        Month m_month;
        Day m_day;
    };

    // Use like ProgressOutput() << "Some Text";
    class ProgressOutput
    {
    private:
        static bool m_reportProgress;
        std::stringstream buffer;

    public:
        static void setProgressReporting(bool reportProgress)
        {
            m_reportProgress = reportProgress;
        }

        ProgressOutput() {}

        ~ProgressOutput()
        {
            if (m_reportProgress)
            {
                std::cout << buffer.rdbuf() << std::flush;
            }
        }

        template <typename T>
        ProgressOutput& operator<<(const T& t)
        {
            buffer << t;
            return *this;
        }
    };

    // Structs used by IRES.

    // Used by buildVertexBlockLists().
    struct Vertex
    {
        F32 v0[3];
        int index;

        // Equality operator
        inline bool operator==(const Vertex& x) const
        {
            if ((fabs(v0[0] - x.v0[0]) <= 1e-2)
                    && (fabs(v0[1] - x.v0[1]) <= 1e-2)
                    && (fabs(v0[2] - x.v0[2]) <= 1e-2))
            {
                return true;
            }

            return false;
        }



        // Hash-related functions
        inline std::size_t vectorHash(const F32 v[3]) const
        {
            char bs[128];

            double numberOfSignificantDigits = 10e5;

            long int d1( numberOfSignificantDigits * v[0] );
            long int d2( numberOfSignificantDigits * v[1] );
            long int d3( numberOfSignificantDigits * v[2] );

            std::sprintf(bs,"%ld%ld%ld",d1,d2,d3);

            std::string s(bs);

            static std::hash<std::string> hasher;
            return hasher(s);
        }

        inline std::size_t operator()(const Vertex& vertex) const
        {
            return vectorHash(vertex.v0);
        }

        inline bool operator()(const Vertex& lhs, const Vertex& rhs) const
        {
            return lhs == rhs;
        }
    };

    struct Face
    {
        enum FACE_BLOCK_POS { NOTDEFINED = -100000000 ,
                              BOTTOM = 0 ,
                              TOP    = 1 ,
                              LEFT   = 2 ,
                              RIGHT  = 3 ,
                              FRONT  = 4 ,
                              BACK   = 5 };

        U32 a , b , c , d ;
        int id ; // Block Id that generated this face
        int auxId ; // An auxiliary  varible to help data manipulation
        bool isExtern ;
        FACE_BLOCK_POS faceBlockRelPos ;

        // Ctors
        Face() : a(0), b(0), c(0), d(0), id(-1) , isExtern(false) , faceBlockRelPos(NOTDEFINED) { }

        Face(U32 a, U32 b, U32 c, U32 d) : id(-1) , isExtern(false) , faceBlockRelPos(NOTDEFINED)
        {
            set(a,b,c,d);
        }

        // Sets vertices, making sure smallest int in the face is 'a'
        void set(U32 _a, U32 _b, U32 _c, U32 _d)
        {
            // Make sure smallest int in this face is 'a'
            // This makes comparing with other faces much easier
            if ((_a <= _b) && (_a <= _c) && (_a <= _d))
            {
                a = _a;
                b = _b;
                c = _c;
                d = _d;
            }
            else if ((_b <= _a) && (_b <= _c) && (_b <= _d))
            {
                a = _b;
                b = _c;
                c = _d;
                d = _a;
            }
            else if ((_c <= _a) && (_c <= _b) && (_c <= _d))
            {
                a = _c;
                b = _d;
                c = _a;
                d = _b;
            }
            else
            {
                a = _d;
                b = _a;
                c = _b;
                d = _c;
            }
        }

        // Equality operator.
        // We know the smallest int in the face is a, the
        // only difference between the two faces might
        // be the ordering.
        inline bool operator==(const Face& rhs) const
        {
            return ( ((a == rhs.a) && (c == rhs.c))
                     && ( ((b == rhs.d) && (d == rhs.b)) ||
                          ((b == rhs.b) && (d == rhs.d)) ) );
        }
        inline bool operator!=(const Face& rhs) const
        {
            return !( (*this) == rhs );
        }
        // Hash-related functions
        inline std::size_t operator()(const Face& f) const
        {
            char bs[128];
            long int d1( f.a );
            long int d2( f.c );

            std::sprintf(bs,"%ld%ld",d1,d2);

            std::string s(bs);
            static std::hash<std::string> hasher;

            return hasher(s);
        }


        inline bool operator()(const Face& lhs, const Face& rhs) const
        {
            return lhs == rhs;
        }

        static std::string faceBlockPos2string( int pos )
        {
            std::string possName;
            switch (pos)
            {
            case ires::Face::BACK :
                possName = "BACK";
                break;
            case ires::Face::FRONT:
                possName = "FRONT";
                break;
            case ires::Face::TOP:
                possName = "TOP";
                break;
            case ires::Face::BOTTOM:
                possName = "BOTTOM";
                break;
            case ires::Face::LEFT:
                possName = "LEFT";
                break;
            case ires::Face::RIGHT:
                possName = "RIGHT";
                break;
            default:
                possName = "Not Defined";
                break;
            }
            return possName;
        }
    };

}

#endif // PRIMITIVESIRES_H
