#include <cstdio>
#include <cassert>
#include <cmath>
#include <set>
#include <list>
#include <vector>

#include <cairo.h>

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
    return abs( ( double )source.x - target.x ) + abs( ( double )source.y - target.y );
}

void enqueue( int a ) {
    Edge e = E[ a ];

    if ( !validPoint( e.to ) ) {
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
    assert( validPoint( e.from ) );

    frontier.insert( a );

    assert( !frontier.empty() );
}

list< Edge > aStar( Point source, Point target ) {
    list< Edge > ret;
    int iterations = 0;

    // printf( "Target is (%i, %i)\n", target.x + 1, target.y + 1 );
    assert( E.empty() );
    assert( frontier.empty() );
    assert( visited.empty() );

    assert( validPoint( source ) );
    assert( validPoint( target ) );

    // seed the frontier with a circular edge connecting the source with the source
    E.push_back( Edge( NULL, source, source, manhattan( source, target ), 0 ) );
    frontier.insert( 0 );

    while ( !frontier.empty() ) {
        // printf( "Frontier contains %i edges.\n", frontier.size() );
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

const int LEGEND_PADDING = 10;
const int LEGEND_LINE_HEIGHT = 18;
const int SCALE_W = 30;
const int SCALE_H = 30;
const int LEGEND_H = 200;
const int BORDER_W = 1;

void drawLegend( cairo_t* cr, int y ) {
    // draw the legend
    int x = LEGEND_PADDING;

    cairo_set_line_width( cr, 1.0 );
    cairo_set_source_rgb( cr, 0, 0, 0 );
    cairo_move_to( cr, x, y );
    cairo_set_font_size( cr, 14.0 );
    cairo_text_path( cr, "National Technical University of Athens" );
    y += LEGEND_LINE_HEIGHT;
    cairo_move_to( cr, x, y );
    cairo_text_path( cr, "Artificial Intelligence 2011 - 2012" );
    y += LEGEND_LINE_HEIGHT;
    cairo_move_to( cr, x, y );
    cairo_text_path( cr, "Dionysis Zindros <dionyziz@gmail.com>" );
    cairo_fill( cr );

    y += LEGEND_LINE_HEIGHT;
    cairo_set_source_rgb( cr, 0, 0, 0 );
    cairo_rectangle( cr, x, y, LEGEND_LINE_HEIGHT, LEGEND_LINE_HEIGHT );
    cairo_fill( cr );
    cairo_set_source_rgb( cr, 0, 0, 0 );
    cairo_rectangle( cr, x, y, LEGEND_LINE_HEIGHT, LEGEND_LINE_HEIGHT );
    cairo_stroke( cr );
    cairo_move_to( cr, x + LEGEND_LINE_HEIGHT + 4, y + LEGEND_LINE_HEIGHT - 4 );
    cairo_text_path( cr, "Obstacle" );
    cairo_fill( cr );

    y += LEGEND_LINE_HEIGHT + 5;
    cairo_set_source_rgb( cr, 1, 0, 0 );
    cairo_rectangle( cr, x, y, LEGEND_LINE_HEIGHT, LEGEND_LINE_HEIGHT );
    cairo_fill( cr );
    cairo_set_source_rgb( cr, 0, 0, 0 );
    cairo_rectangle( cr, x, y, LEGEND_LINE_HEIGHT, LEGEND_LINE_HEIGHT );
    cairo_stroke( cr );
    cairo_move_to( cr, x + LEGEND_LINE_HEIGHT + 4, y + LEGEND_LINE_HEIGHT - 4 );
    cairo_set_source_rgb( cr, 0, 0, 0 );
    cairo_text_path( cr, "Robot 1" );
    cairo_fill( cr );

    y += LEGEND_LINE_HEIGHT + 5;
    cairo_set_source_rgb( cr, 0, 1, 0 );
    cairo_rectangle( cr, x, y, LEGEND_LINE_HEIGHT, LEGEND_LINE_HEIGHT );
    cairo_fill( cr );
    cairo_set_source_rgb( cr, 0, 0, 0 );
    cairo_rectangle( cr, x, y, LEGEND_LINE_HEIGHT, LEGEND_LINE_HEIGHT );
    cairo_stroke( cr );
    cairo_move_to( cr, x + LEGEND_LINE_HEIGHT + 4, y + LEGEND_LINE_HEIGHT - 4 );
    cairo_set_source_rgb( cr, 0, 0, 0 );
    cairo_text_path( cr, "Robot 2" );
    cairo_fill( cr );
}

void visualize( list< Edge > APath, list< Edge > BPath ) {
    set< Edge > common;
    const int W = mapSize.x * SCALE_W + 2 * BORDER_W;
    const int H = mapSize.y * SCALE_H + 2 * BORDER_W + LEGEND_H;

    cairo_surface_t* surface = cairo_image_surface_create( CAIRO_FORMAT_ARGB32, W, H );
    cairo_t* cr = cairo_create( surface );

    drawLegend( cr, H - LEGEND_H + LEGEND_PADDING + LEGEND_LINE_HEIGHT );

    // draw the border
    cairo_set_source_rgb( cr, 0.1, 0.2, 0.4 );
    cairo_set_line_width( cr, BORDER_W );
    cairo_rectangle( cr, BORDER_W / 2, BORDER_W / 2, W - BORDER_W, H - BORDER_W - LEGEND_H );
    cairo_stroke( cr );

    cairo_save( cr );
    cairo_translate( cr, BORDER_W, BORDER_W );
    cairo_scale( cr, SCALE_W, SCALE_H );

    // fill the image background with whiteness
    cairo_set_source_rgb( cr, 1, 1, 1 );
    cairo_rectangle( cr, 0, 0, mapSize.x, mapSize.y );
    cairo_fill( cr );

    // draw obstacles as black
    cairo_set_source_rgb( cr, 0, 0, 0 );

    for ( int x = 0; x < mapSize.x; ++x ) {
        for ( int y = 0; y < mapSize.y; ++y ) {
            if ( obstacle[ x ][ y ] ) {
                cairo_rectangle( cr, x, y, 1, 1 );
                cairo_fill( cr );
            }
        }
    }

    // draw the paths followed by the robots
    cairo_set_line_width( cr, 0.2 );

    // robot A is red
    cairo_set_source_rgb( cr, 1, 0, 0 );
    cairo_move_to( cr, APath.begin()->from.x + 0.5, APath.begin()->from.y + 0.5 );
    for ( list< Edge >::iterator it = APath.begin(); it != APath.end(); ++it ) {
        cairo_line_to( cr, it->to.x + 0.5, it->to.y + 0.5 );
    }
    cairo_stroke( cr );

    // robot B is green
    cairo_set_source_rgb( cr, 0, 1, 0 );
    cairo_move_to( cr, BPath.begin()->from.x + 0.5, BPath.begin()->from.y + 0.5 );
    for ( list< Edge >::iterator it = BPath.begin(); it != BPath.end(); ++it ) {
        cairo_line_to( cr, it->to.x + 0.5, it->to.y + 0.5 );
    }
    cairo_stroke( cr );

    cairo_surface_write_to_png( surface, "robots.png" );
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
    list< Edge > APath = aStar( target, A );
    printf( "Running A* for robot B:\n" );
    list< Edge > BPath = aStar( target, B );

    printf( "Robot A path:\n" );
    for ( list< Edge >::iterator it = APath.begin(); it != APath.end(); ++it ) {
        printf( "(%i, %i)\n", it->from.x + 1, it->from.y + 1 );
    }

    printf( "\nRobot B path:\n" );
    for ( list< Edge >::iterator it = BPath.begin(); it != BPath.end(); ++it ) {
        printf( "(%i, %i)\n", it->from.x + 1, it->from.y + 1 );
    }
    
    printf( "\n" );

    visualize( APath, BPath );

    return 0;
}
