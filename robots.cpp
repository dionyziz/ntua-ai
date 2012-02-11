/*
    National Technical University of Athens
    Developer: Dionysis "dionyziz" Zindros <dionyziz@gmail.com>
*/

#include <vector>
#include <cstdio>
#include <cassert>

#include "astar.h"
#include "visualizer.h"

using namespace std;

int main() {
    Point mapSize;
    vector< vector< bool > > obstacle;
    Point A, B, target;

    scanf( "%i %i %i %i %i %i %i %i\n",
            &mapSize.x, &mapSize.y, &A.x, &A.y, &B.x, &B.y, &target.x, &target.y );

    // make coordinates 0-based
    --A.x; --A.y; --B.x; --B.y; --target.x; --target.y;

    assert( mapSize.x > 0 );
    assert( mapSize.y > 0 );
    assert( mapSize.x <= 400000 );
    assert( mapSize.y <= 400000 );

    assert( validPoint( A, mapSize ) );
    assert( validPoint( B, mapSize ) );
    assert( validPoint( target, mapSize ) );

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
    list< Edge > APath = aStar( target, A, obstacle, mapSize );
    printf( "Running A* for robot B:\n" );
    list< Edge > BPath = aStar( target, B, obstacle, mapSize );

    printf( "Robot A path:\n" );
    for ( list< Edge >::iterator it = APath.begin(); it != APath.end(); ++it ) {
        printf( "(%i, %i)\n", it->from.x + 1, it->from.y + 1 );
    }

    printf( "\nRobot B path:\n" );
    for ( list< Edge >::iterator it = BPath.begin(); it != BPath.end(); ++it ) {
        printf( "(%i, %i)\n", it->from.x + 1, it->from.y + 1 );
    }
    
    printf( "\n" );

    visualize( APath, BPath, obstacle, mapSize );

    return 0;
}
