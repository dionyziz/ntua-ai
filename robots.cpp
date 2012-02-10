#include <cstdio>
#include <cassert>
#include <cmath>
#include <set>
#include <queue>

using namespace std;

class Point {
    public:
        int x;
        int y;

        Point() {};
        Point( int x, int y ):
            x( x ), y( y ) {};
};

bool operator <( Point a, Point b ) {
    if ( a.x != b.x ) {
        return a.x < b.x;
    }
    return a.y < b.y;
}

bool operator ==( Point a, Point b ) {
    return a.x == b.x && a.y == b.y;
}

class Node {
    public:
        Node* parent;
        Point location;
        int heuristic;
        int distance;

        Node( Point location, int heuristic, int distance ):
            location( location ), heuristic( heuristic ), distance( distance ) {};
};

bool operator <( Node a, Node b ) {
    // better nodes are the ones with the minimum heuristic value
    return a.heuristic > b.heuristic;
}

Point mapSize;
priority_queue< Node > frontier;
set< Point > visited;
vector< vector< bool > > obstacle;

int manhattan( Point source, Point target ) {
    return abs( ( double )source.x - target.x ) + abs( ( double )source.y - target.y );
}

void enqueue( Node from,
              Point to,
              Point target ) {
    if (    to.x < 0
         || to.y < 0
         || to.x >= mapSize.x
         || to.y >= mapSize.y // out of bounds
         || visited.find( to ) != visited.end()
         || obstacle[ to.y ][ to.x ] ) {
        return;
    }
    Node next(
        to,
        from.distance + 1 + manhattan( to, target ),
        from.distance + 1
    );
    next.parent = &from;
    frontier.push( next );
}

Node A_star( Point source, Point target ) {
    frontier.push( Node( source, manhattan( source, target ), 0 ) );

    while ( !frontier.empty() ) {
        Node q = frontier.top();
        frontier.pop();
        if ( q.location == target ) {
            // arrived on target
            return q;
        }
        visited.insert( q.location );
        for ( int x = q.location.x - 1; x <= q.location.x + 1; x += 2 ) {
            enqueue( q, Point( x, q.location.y ), target );
        }
        for ( int y = q.location.y - 1; y <= q.location.y + 1; y += 2 ) {
            enqueue( q, Point( q.location.x, y ), target );
        }
    }
}

int main() {
    int X, Y;
    Point mapSize, A, B, target;

    scanf( "%i %i %i %i %i %i %i %i\n",
            &mapSize.x, &mapSize.y, &A.x, &A.y, &B.x, &B.y, &target.x, &target.y );

    for ( int y = 0; y < mapSize.y; ++y ) {
        obstacle.push_back( vector< bool >() );
        for ( int x = 0; x < mapSize.x; ++x ) {
            char c;
            scanf( "%c", &c );
            obstacle[ y ].push_back( c == 'X' );
        }
        scanf( "\n" );
    }
    // make sure the robots start from an empty square position and that the target
    // is a free square as well
    assert( !obstacle[ target.x ][ target.y ] );
    assert( !obstacle[ A.x ][ A.y ] );
    assert( !obstacle[ B.x ][ B.y ] );

    A_star( A, target );
    A_star( B, target );

    return 0;
}
