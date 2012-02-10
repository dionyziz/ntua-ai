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

class Edge {
    public:
        const Edge* parent;
        Point from;
        Point to;
        int heuristic; // approximation of how far the "to" end of Edge is from the target
        int distance; // actual distance between the "to" end of the Edge and the source

        Edge( const Edge* parent, Point from, Point to, int heuristic, int distance ):
            parent( parent ), from( Point( from ) ), to( Point( to ) ),
            heuristic( heuristic ), distance( distance ) {};
};

bool operator <( Edge a, Edge b ) {
    // better edges are the ones with the minimum estimated value
    return a.distance + a.heuristic > b.distance + b.heuristic;
}

Point mapSize;
priority_queue< Edge > frontier;
set< Point > visited;
vector< vector< bool > > obstacle;

bool validPoint( Point a ) {
    return a.x >= 0 && a.y >= 0 && a.x < mapSize.x && a.y < mapSize.y;
}

int manhattan( Point source, Point target ) {
    return 0; // abs( ( double )source.x - target.x ) + abs( ( double )source.y - target.y );
}

void enqueue( const Edge* e ) {
    if (    !validPoint( e->to ) // to is out of bounds
         || obstacle[ e->to.y ][ e->to.x ] // to is obstacle
         || visited.find( e->to ) != visited.end() ) { // to is already visited
        return;
    }
    assert( e->distance >= 0 );
    assert( e->distance <= mapSize.x * mapSize.y );
    assert( e->heuristic >= 0 );

    assert( validPoint( e->from ) );

    assert( e->parent != 0 );
    frontier.push( *e );

    assert( !frontier.empty() );
}

Edge aStar( Point source, Point target ) {
    int iterations = 0;

    assert( validPoint( source ) );
    assert( validPoint( target ) );

    // seed the frontier with a circular edge connecting the source with the source
    // FIXME: deallocate edges
    frontier.push( *( new Edge( NULL, source, source, manhattan( source, target ), 0 ) ) );

    while ( !frontier.empty() ) {
        const Edge* e = &frontier.top();
        assert( validPoint( e->from ) );
        assert( validPoint( e->to ) );
        frontier.pop();
        printf( "At (%i, %i).\n", e->to.x, e->to.y );
        if ( e->to == target ) {
            // arrived on target
            return *e;
        }
        visited.insert( e->to );
        // enqueue all cells adjacent to q
        // FIXME: deallocate edges
        for ( int x = e->to.x - 1; x <= e->to.x + 1; x += 2 ) {
            Point next = makePoint( x, e->from.y );
            enqueue( new Edge( e, e->to, next, manhattan( next, target ), e->distance + 1 ) );
        }
        for ( int y = e->to.y - 1; y <= e->to.y + 1; y += 2 ) {
            Point next = makePoint( e->from.x, y );
            enqueue( new Edge( e, e->to, next, manhattan( next, target ), e->distance + 1 ) );
        }
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

    Edge APath = aStar( target, A );
    Edge BPath = aStar( target, B );

    assert( APath.from == A );
    assert( BPath.from == B );

    printf( "Robot A path:\n" );
    while ( APath.from != APath.to ) {
        printf( "( %i, %i )\n", APath.from.x + 1, APath.from.y + 1 );
        assert( APath.parent != 0 );
        APath = *( APath.parent );
    }
    printf( "\nRobot B path:\n" );
    while ( BPath.from != BPath.to ) {
        printf( "( %i, %i )\n", BPath.from.x + 1, BPath.from.y + 1 );
        assert( BPath.parent != 0 );
        BPath = *( BPath.parent );
    }

    return 0;
}
