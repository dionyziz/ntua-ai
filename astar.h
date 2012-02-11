#ifndef ASTAR_H
#define ASTAR_H

#include <list>
#include <vector>
#include <set>
#include <cassert>
#include <cstdio>
#include <cmath>

using namespace std;

struct Point {
    int x;
    int y;
};

inline bool validPoint( Point a, Point mapSize ) {
    return a.x >= 0 && a.y >= 0 && a.x < mapSize.x && a.y < mapSize.y;
}

inline bool operator <( Point a, Point b ) {
    if ( a.x != b.x ) {
        return a.x < b.x;
    }
    return a.y < b.y;
}

inline bool operator ==( Point a, Point b ) {
    return a.x == b.x && a.y == b.y;
}

inline bool operator !=( Point a, Point b ) {
    return !( a == b );
}

inline Point makePoint( int x, int y ) {
    Point ret;

    ret.x = x;
    ret.y = y;

    return ret;
}

inline int manhattan( Point source, Point target ) {
    return abs( ( double )source.x - target.x ) + abs( ( double )source.y - target.y );
}

struct Edge {
    public:
        int parent;
        Point from;
        Point to;
        int heuristic; // approximation of how far the "to" end of Edge is from the target
        int distance; // actual distance between the "to" end of the Edge and the source

        Edge( int parent, Point from, Point to, int heuristic, int distance ):
            parent( parent ), from( from ), to( to ),
            heuristic( heuristic ), distance( distance ) {};
};

class CompareEdges {
    private:
        vector< Edge > const & _E;
    public:
        CompareEdges( vector< Edge > const &E ) :_E( E ) {}
        inline bool operator() ( const int &a, const int &b ) {
            int diff = ( _E[ b ].distance + _E[ b ].heuristic ) - ( _E[ a ].distance + _E[ a ].heuristic );

            if ( diff == 0 ) {
                return a < b;
            }
            return diff > 0;
        }
};

void enqueue(
    int,
    vector< Edge > const &,
    Point,
    vector< vector< bool > > const &,
    set< Point > const &,
    set< int, CompareEdges > &
);

list< Edge > aStar(
    Point,
    Point,
    vector< vector< bool > > const &,
    Point
);

#endif
