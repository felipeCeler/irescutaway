
#include "kdtreeires.h"

#include <vector>


namespace ires
{
    KdTree::~KdTree()
    {
        if (m_root != NULL)
        {
            m_root = NULL;  // The data pointed to is freed by the m_nodes vector.
        }

        m_size = 0;
    }

    KdTree::KdTree(const std::vector<int>& blockIndexList, const ires::Ires& ires)
        : m_root(NULL), m_nodes(), m_size(0)
    {
        ProgressOutput() << "\nIRES: > Building KD-Tree...\n";

        // Construct vector of Points using centroids
        std::vector<Point*> allValidPointsPointers;
        std::vector<Point> allValidPoints;


        std::size_t numBlocks = blockIndexList.size();

        ProgressOutput() << "IRES: > Reserving memory...\n";

        allValidPointsPointers.reserve(numBlocks / 2);
        allValidPoints.reserve(numBlocks / 2);

        // For storing the results of getCentroid()
        F32 centroid[3];

        ProgressOutput() << "IRES: > Building centroids...\n";

        for (std::size_t i = 0; i < numBlocks; ++i)
        {
            if (blockIndexList[i] > -1)
            {
                ires.getCentroid(i, centroid);

                // Not using 'new' because we won't need this data after this method returns.
                allValidPoints.push_back(Point(Vec3(centroid[0], centroid[1], centroid[2]), i));
            }
        }

        
        allValidPoints.shrink_to_fit();

        for (auto it = allValidPoints.begin(); it != allValidPoints.end(); ++it)
        {
            allValidPointsPointers.push_back(&(*it));
        }

        allValidPointsPointers.shrink_to_fit();

        m_nodes.resize(allValidPoints.size());  // Nodes will be set up by buildTree().

        ProgressOutput() << "IRES: > Building tree structure...\n";

        // Call buildTree with depth 0 and the Point list just built
        m_root = buildTree(allValidPointsPointers, 0);

        ProgressOutput() << "IRES: > Done building KD-tree.\n";
    }
    

    KdTree::Node* KdTree::buildTree(std::vector<Point*>& points, int depth)
    {
        // Sorting functions. We have three sorting functions; each
        // sorts along a different axis.
        struct
        {
            inline bool operator() (Point* lhs, Point* rhs)
            {
                return (lhs->point[0] < rhs->point[0]);
            }

        } SortFuncX;

        struct
        {
            inline bool operator() (Point* lhs, Point* rhs)
            {
                return (lhs->point[1] < rhs->point[1]);
            }

        } SortFuncY;

        struct
        {
            inline bool operator() (Point* lhs, Point* rhs)
            {
                return (lhs->point[2] < rhs->point[2]);
            }

        } SortFuncZ;




        const unsigned short axis = depth % 3;


        // End of recursion
        if (points.empty())
        {
            return NULL;
        }
        else if (points.size() == 1)
        {
            m_nodes[m_size].point = (*points[0]).point;
            m_nodes[m_size].blockIndex = (*points[0]).blockIndex;
            m_nodes[m_size].axis = axis;
            ++m_size;

            return &m_nodes[m_size - 1];
        }

        
        std::size_t median = (std::size_t) std::floor(points.size() / 2.0f);

        // Do a partial sort; we only need the median to be in the correct spot,
        // and have all elements that are smaller than the median should be before
        // the median, and all the elements that are greater than the median should be
        // after it. This is a lot more efficient than sorting the entire list.
        switch (axis)
        {
        case 0:
            std::nth_element(points.begin(), points.begin() + median, points.end(), SortFuncX);
            break;
        case 1:
            std::nth_element(points.begin(), points.begin() + median, points.end(), SortFuncY);
            break;
        case 2:
            std::nth_element(points.begin(), points.begin() + median, points.end(), SortFuncZ);
            break;
        default:
            assert(false);  // Should never get here!
            break;
        }

        
        m_nodes[m_size].point = points[median]->point;
        m_nodes[m_size].blockIndex = points[median]->blockIndex;
        m_nodes[m_size].axis = axis;
        Node* splitter = &m_nodes[m_size];
        ++m_size;

        std::vector<Point*> leftVector = std::vector<Point*>(points.cbegin(), points.cbegin() + median);
        std::vector<Point*> rightVector = std::vector<Point*>(points.cbegin() + median + 1, points.cend());

        // Assign left + right children to the result of recursive calls on buildTree with depth+1
        splitter->left = buildTree(leftVector, depth + 1);
        splitter->right = buildTree(rightVector, depth + 1);


        
        // Return root of this tree
        return splitter;
    }

    void KdTree::getKNearestBlockIds(const F32 point[3], U32 k, std::vector<U32>& nearestBlocks) const
    {
        std::vector<F32> distances;
        distances.reserve(k);
        
        Vec3 searchPoint(point);

        getKNearestHelper(m_root, searchPoint, k, nearestBlocks, distances);
    }

    void KdTree::getKNearestHelper(const Node* currentNode, const Vec3& searchPoint,
        U32 k, std::vector<U32>& nearestBlocks, std::vector<F32>& distances) const
    {
        unsigned short axis = currentNode->axis;

        if ((currentNode->left == NULL) && (currentNode->right == NULL))
        {
            checkCurrentNode(currentNode, searchPoint, k, nearestBlocks, distances);
        }
        else if (searchPoint[axis] <= currentNode->point[axis])
        {
            if (currentNode->left != NULL)
            {
                getKNearestHelper(currentNode->left, searchPoint, k, nearestBlocks, distances);
            }

            checkCurrentNode(currentNode, searchPoint, k, nearestBlocks, distances);

            if (currentNode->right != NULL)
            {
                float diff = currentNode->point[axis] - searchPoint[axis];
                diff *= diff;

                bool checkOtherSide = false;

                for (auto it = distances.cbegin(); it != distances.cend(); ++it)
                {
                    if ((*it) >= diff)
                    {
                        checkOtherSide = true;
                        break;
                    }
                }

                if (checkOtherSide)
                {
                    getKNearestHelper(currentNode->right, searchPoint, k, nearestBlocks, distances);
                }
            }
        }
        else
        {
            if (currentNode->right != NULL)
            {
                getKNearestHelper(currentNode->right, searchPoint, k, nearestBlocks, distances);
            }

            checkCurrentNode(currentNode, searchPoint, k, nearestBlocks, distances);

            if (currentNode->left != NULL)
            {
                float diff = searchPoint[axis] - currentNode->point[axis];
                diff *= diff;

                bool checkOtherSide = false;

                for (auto it = distances.cbegin(); it != distances.cend(); ++it)
                {
                    if ((*it) >= diff)
                    {
                        checkOtherSide = true;
                        break;
                    }
                }

                if (checkOtherSide)
                {
                    getKNearestHelper(currentNode->left, searchPoint, k, nearestBlocks, distances);
                }
            }
        }
    }

    void KdTree::checkCurrentNode(const Node* currentNode, const Vec3& searchPoint,
            U32 k, std::vector<U32>& nearestBlocks, std::vector<F32>& distances) const
    {
        float currentDistance = distanceSquared(currentNode->point, searchPoint);

        std::size_t numPoints = distances.size();

        if (numPoints < k)
        {
            distances.push_back(currentDistance);
            nearestBlocks.push_back(currentNode->blockIndex);
        }
        else
        {
            bool best = false;

            for (std::size_t i = 0; i < numPoints; ++i)
            {
                if (currentDistance < distances[i])
                {
                    best = true;
                    break;
                }
            }

            if (best)
            {
                float worst = 0.0f;
                std::size_t worstIndex = 0;

                for (std::size_t i = 0; i < numPoints; ++i)
                {
                    if (worst < distances[i])
                    {
                        worst = distances[i];
                        worstIndex = i;
                    }
                }

                distances.erase(distances.begin() + worstIndex);
                nearestBlocks.erase(nearestBlocks.begin() + worstIndex);

                distances.push_back(currentDistance);
                nearestBlocks.push_back(currentNode->blockIndex);
            }
        }
    }


    F32 KdTree::distanceSquared(const Vec3& first, const Vec3& second) const
    {
        float xd = second[0] - first[0];
        float yd = second[1] - first[1];
        float zd = second[2] - first[2];

        return (xd*xd + yd*yd + zd*zd);
    }
}
 
