#include <cstdio>
#include <cassert>
#include <cmath>
#include <set>
#include <queue>

using namespace std;

struct Point {
    int x;
    int y;
};

Point makePoint( int x, int y ) {
    Point ret;

    ret.x = x;
    ret.y = y;

    return ret;
}

bool operator <( Point a, Point b ) {
    if ( a.x != b.x ) {
        return a.x < b.x;
    }
    return a.y < b.y;
}

bool operator ==( Point a, Point b ) {
    return a.x == b.x && a.y == b.y;
}

bool operator !=( Point a, Point b ) {
    return !( a == b );
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

vector< Edge > E;

class CompareEdges {
    public:
        bool operator() ( const int &a, const int &b ) {
            int diff = ( E[ b ].distance + E[ b ].heuristic ) - ( E[ a ].distance + E[ a ].heuristic );

            if ( diff == 0 ) {
                return a < b;
            }
            return diff > 0;
        }
};

Point mapSize;
set< int, CompareEdges > frontier;
set< Point > visited;
vector< vector< bool > > obstacle;

bool validPoint( Point a ) {
    return a.x >= 0 && a.y >= 0 && a.x < mapSize.x && a.y < mapSize.y;
}

int manhattan( Point source, Point target ) {
    return 0; // abs( ( double )source.x - target.x ) + abs( ( double )source.y - target.y );
}

void enqueue( int a ) {
    Edge e = E[ a ];

    if ( !validPoint( e.to ) ) {
        // to is out of bounds
        printf( "Out of bounds.\n", e.to.x, e.to.y );
        return;
    }
    if ( obstacle[ e.to.x ][ e.to.y ] ) {
        // to is obstacle
        printf( "Obstacle.\n", e.to.x + 1, e.to.y + 1 );
        return;
    }
    if ( visited.find( e.to ) != visited.end() ) {
        // to is already visited
        printf( "Already visited.\n", e.to.x + 1, e.to.y + 1 );
        return;
    }
    printf( "Enqueued (%i, %i) with id %i.\n", e.to.x + 1, e.to.y + 1, a );
    assert( e.distance >= 0 );
    assert( e.distance <= mapSize.x * mapSize.y );
    assert( e.heuristic >= 0 );
    assert( validPoint( e.from ) );

    frontier.insert( a );

    assert( !frontier.empty() );
}

Edge aStar( Point source, Point target ) {
    int iterations = 0;

    printf( "Target is (%i, %i)\n", target.x + 1, target.y + 1 );

    assert( validPoint( source ) );
    assert( validPoint( target ) );

    // seed the frontier with a circular edge connecting the source with the source
    E.push_back( Edge( NULL, source, source, manhattan( source, target ), 0 ) );
    frontier.insert( 0 );

    while ( !frontier.empty() ) {
        printf( "Frontier contains %i edges.\n", frontier.size() );
        set< int, CompareEdges >::iterator it = frontier.begin();
        int a = *it;
        Edge e = E[ a ];
        assert( validPoint( e.from ) );
        assert( validPoint( e.to ) );
        frontier.erase( it );
        if ( visited.find( e.to ) != visited.end() ) {
            // Edge "to" has already been visited.
            // Because the heuristic we ues is admissible,
            // the past visit is always going to better than this one,
            // so skip it.
            printf( "(%i, %i) has already been visited.\n", e.to.x, e.to.y );
            continue;
        }
        printf( "At (%i, %i).\n", e.to.x + 1, e.to.y + 1 );
        if ( e.to == target ) {
            // arrived on target
            // clean-up and return

            E.clear();
            visited.clear();
            frontier.clear();

            return e;
        }
        visited.insert( e.to );
        // enqueue all cells adjacent to q
        for ( int x = e.to.x - 1; x <= e.to.x + 1; x += 2 ) {
            Point next = makePoint( x, e.to.y );
            printf( "(%i, %i) => (%i, %i): ", e.to.x + 1, e.to.y + 1, x + 1, e.to.y + 1 );
            E.push_back( Edge( a, e.to, next, manhattan( next, target ), e.distance + 1 ) );
            enqueue( E.size() - 1 );
        }
        for ( int y = e.to.y - 1; y <= e.to.y + 1; y += 2 ) {
            Point next = makePoint( e.to.x, y );
            printf( "(%i, %i) => (%i, %i): ", e.to.x + 1, e.to.y + 1, e.to.x + 1, y + 1 );
            E.push_back( Edge( a, e.to, next, manhattan( next, target ), e.distance + 1 ) );
            enqueue( E.size() - 1 );
        }
        // it's impossible to iterate more than the map size if the algorithm is implemented correctly
        ++iterations;
        assert( iterations <= mapSize.x * mapSize.y );
    }

    assert( false ); // target should not be unreachable
}

int main() {
    Point A, B, target;

    scanf( "%i %i %i %i %i %i %i %i\n",
            &mapSize.x, &mapSize.y, &A.x, &A.y, &B.x, &B.y, &target.x, &target.y );

    // make coordinates 0-based
    --A.x; --A.y; --B.x; --B.y; --target.x; --target.y;

    assert( mapSize.x > 0 );
    assert( mapSize.y > 0 );
    assert( mapSize.x <= 400000 );
    assert( mapSize.y <= 400000 );

    assert( validPoint( A ) );
    assert( validPoint( B ) );
    assert( validPoint( target ) );

    for ( int x = 0; x < mapSize.x; ++x ) {
        obstacle.push_back( vector< bool >( mapSize.y ) );
    }

    for ( int y = 0; y < mapSize.y; ++y ) {
        for ( int x = 0; x < mapSize.x; ++x ) {
            // make sure we have allocated this cell's memory correctly
            assert( obstacle[ x ][ y ] || !obstacle[ x ][ y ] );
            char c;
            scanf( "%c", &c );
            assert( c == 'X' || c == 'O' );
            obstacle[ x ][ y ] = c == 'X';
        }
        scanf( "\n" );
    }
    // make sure the robots start from an empty square position and that the target
    // is a free square as well
    assert( !obstacle[ target.x ][ target.y ] );
    assert( !obstacle[ A.x ][ A.y ] );
    assert( !obstacle[ B.x ][ B.y ] );

    printf( "Running A* for robot A:\n" );
    Edge APath = aStar( target, A );
    printf( "Running A* for robot B:\n" );
    Edge BPath = aStar( target, B );

    assert( APath.to == A );
    assert( BPath.to == B );

    printf( "Robot A path:\n" );
    while ( APath.from != APath.to ) {
        printf( "(%i, %i)\n", APath.from.x + 1, APath.from.y + 1 );
        APath = E[ APath.parent ];
    }

    printf( "Robot B path:\n" );
    while ( BPath.from != BPath.to ) {
        printf( "(%i, %i)\n", BPath.from.x + 1, BPath.from.y + 1 );
        BPath = E[ BPath.parent ];
    }

    return 0;
}
