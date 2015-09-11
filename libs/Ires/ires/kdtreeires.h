
#ifndef KDTREEIRES_H
#define KDTREEIRES_H

#include "ires.h"

#ifdef _WIN32
#include "Eigen\Eigen"  // For math-related stuff.
#else
#include "Eigen/Eigen"
#endif



namespace ires
{
    // The vector type we use throughout IRES.
    typedef Eigen::Vector3f Vec3;

    // Our KD-Tree. TO-DO: Add functionality (get K-nearest points/blocks, etc.)
    class KdTree
    {
        struct Node
        {
            Vec3 point;
            unsigned int blockIndex;
            unsigned short axis;
            Node *left, *right;

            Node()
            {
                blockIndex = axis = 0;
                left = NULL;
                right = NULL;
            }

            Node(Vec3 point_, int blockIndex_, int axis_)
            {
                point = point_;
                blockIndex = blockIndex_;
                axis = axis_;
                left = NULL;
                right = NULL;
            }
        };

        struct Point
        {
            Vec3 point;
            unsigned int blockIndex;

            Point(Vec3 point_, int index_)
            {
                point = point_;
                blockIndex = index_;
            }
        };






    public:
        KdTree(const std::vector<int>& blockIndexList, const ires::Ires& ires);
        ~KdTree();

        void getKNearestBlockIds(const F32 point[3], U32 k, std::vector<U32>& nearestBlocks) const;

    private:
        KdTree(const KdTree&) ;             // Declarations only; prevent copy-construction
        KdTree& operator=(const KdTree&) ;  // and copy-assignment.

        Node* buildTree(std::vector<Point*>& points, int depth);

        void getKNearestHelper(const Node* currentNode, const Vec3& searchPoint,
            U32 k, std::vector<U32>& nearestBlocks, std::vector<F32>& distances) const;

        void checkCurrentNode(const Node* currentNode, const Vec3& searchPoint,
            U32 k, std::vector<U32>& nearestBlocks, std::vector<F32>& distances) const;

        F32 distanceSquared(const Vec3& first, const Vec3& second) const;

        Node* m_root;
        std::vector<Node> m_nodes;
        std::size_t m_size;
    };


}


#endif
 
