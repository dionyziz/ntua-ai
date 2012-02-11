#include "astar.h"

void enqueue(
        int a,
        vector< Edge > const &E,
        Point mapSize,
        vector< vector< bool > > const &obstacle,
        set< Point > const &visited,
        set< int, CompareEdges > &frontier
    ) {
    Edge e = E[ a ];

    if ( !validPoint( e.to, mapSize ) ) {
        // to is out of bounds
        printf( "Out of bounds.\n" );
        return;
    }
    if ( obstacle[ e.to.x ][ e.to.y ] ) {
        // to is obstacle
        printf( "Obstacle.\n" );
        return;
    }
    if ( visited.find( e.to ) != visited.end() ) {
        // to is already visited
        printf( "Already visited.\n" );
        return;
    }
    printf( "Enqueued (%i, %i) with id %i.\n", e.to.x + 1, e.to.y + 1, a );
    assert( e.distance >= 0 );
    assert( e.distance <= mapSize.x * mapSize.y );
    assert( e.heuristic >= 0 );
    assert( validPoint( e.from, mapSize ) );

    frontier.insert( a );

    assert( !frontier.empty() );
}

list< Edge > aStar(
        Point source,
        Point target,
        vector< vector< bool > > const &obstacle,
        Point mapSize
    ) {
    list< Edge > ret;
    int iterations = 0;
    vector< Edge > E;
    set< int, CompareEdges > frontier( E );
    set< Point > visited;

    // printf( "Target is (%i, %i)\n", target.x + 1, target.y + 1 );
    assert( E.empty() );
    assert( frontier.empty() );
    assert( visited.empty() );

    assert( validPoint( source, mapSize ) );
    assert( validPoint( target, mapSize ) );

    // seed the frontier with a circular edge connecting the source with the source
    E.push_back( Edge( NULL, source, source, manhattan( source, target ), 0 ) );
    frontier.insert( 0 );

    while ( !frontier.empty() ) {
        // printf( "Frontier contains %i edges.\n", frontier.size() );
        set< int, CompareEdges >::iterator it = frontier.begin();
        int a = *it;
        Edge e = E[ a ];
        assert( validPoint( e.from, mapSize ) );
        assert( validPoint( e.to, mapSize ) );
        frontier.erase( it );
        if ( visited.find( e.to ) != visited.end() ) {
            // Edge "to" has already been visited.
            // Because the heuristic we ues is admissible,
            // the past visit is always going to better than this one,
            // so skip it.
            // printf( "(%i, %i) has already been visited.\n", e.to.x, e.to.y );
            continue;
        }
        printf( "At (%i, %i).\n", e.to.x + 1, e.to.y + 1 );
        if ( e.to == target ) {
            // arrived on target
            // clean-up and return

            while ( e.from != e.to ) {
                ret.push_back( e );
                e = E[ e.parent ];
            }
            ret.push_back( e );

            E.clear();
            visited.clear();
            frontier.clear();

            printf( "A* algorithm completed in %i steps.\n", iterations );

            return ret;
        }
        visited.insert( e.to );
        // enqueue all cells adjacent to q
        for ( int x = e.to.x - 1; x <= e.to.x + 1; x += 2 ) {
            Point next = makePoint( x, e.to.y );
            printf( "(%i, %i) => (%i, %i): ", e.to.x + 1, e.to.y + 1, x + 1, e.to.y + 1 );
            E.push_back( Edge( a, e.to, next, manhattan( next, target ), e.distance + 1 ) );
            enqueue( E.size() - 1, E, mapSize, obstacle, visited, frontier );
        }
        for ( int y = e.to.y - 1; y <= e.to.y + 1; y += 2 ) {
            Point next = makePoint( e.to.x, y );
            printf( "(%i, %i) => (%i, %i): ", e.to.x + 1, e.to.y + 1, e.to.x + 1, y + 1 );
            E.push_back( Edge( a, e.to, next, manhattan( next, target ), e.distance + 1 ) );
            enqueue( E.size() - 1, E, mapSize, obstacle, visited, frontier );
        }
        // it's impossible to iterate more than the map size if the algorithm is implemented correctly
        ++iterations;
        assert( iterations <= mapSize.x * mapSize.y );
    }

    assert( false ); // target should not be unreachable
}
