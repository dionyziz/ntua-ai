/*
    National Technical University of Athens
    Developer: Dionysis "dionyziz" Zindros <dionyziz@gmail.com>
*/

#include "visualizer.h"

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

    y += LEGEND_LINE_HEIGHT + 5;
    cairo_set_source_rgb( cr, 0, 0, 1 );
    cairo_rectangle( cr, x, y, LEGEND_LINE_HEIGHT, LEGEND_LINE_HEIGHT );
    cairo_fill( cr );
    cairo_set_source_rgb( cr, 0, 0, 0 );
    cairo_rectangle( cr, x, y, LEGEND_LINE_HEIGHT, LEGEND_LINE_HEIGHT );
    cairo_stroke( cr );
    cairo_move_to( cr, x + LEGEND_LINE_HEIGHT + 4, y + LEGEND_LINE_HEIGHT - 4 );
    cairo_set_source_rgb( cr, 0, 0, 0 );
    cairo_text_path( cr, "Both robots" );
    cairo_fill( cr );
}

void drawArrow( cairo_t* cr, Point from, Point to ) {
    fPoint a;
    fPoint b;
    const float ARROW_HEAD_SIZE = 0.2;

    a.x = from.x; a.y = from.y;
    b.x = to.x; b.y = to.y;

    // move points to their respective block centers
    // so that it looks better
    a.x += 0.5;
    a.y += 0.5;
    b.x += 0.5;
    b.y += 0.5;

    cairo_move_to( cr, a.x, a.y );
    cairo_line_to( cr, b.x, b.y );

    if ( a.y == b.y ) { // horizontal arrow
        float arrowX = b.x - ( b.x - a.x ) * ARROW_HEAD_SIZE;
        cairo_move_to( cr, arrowX, b.y - ARROW_HEAD_SIZE );
        cairo_line_to( cr, b.x, b.y );
        cairo_move_to( cr, arrowX, b.y + ARROW_HEAD_SIZE );
        cairo_line_to( cr, b.x, b.y );
    }
    else { // vertial arrow
        float arrowY = b.y - ( b.y - a.y ) * ARROW_HEAD_SIZE;
        cairo_move_to( cr, b.x - ARROW_HEAD_SIZE, arrowY );
        cairo_line_to( cr, b.x, b.y );
        cairo_move_to( cr, b.x + ARROW_HEAD_SIZE, arrowY );
        cairo_line_to( cr, b.x, b.y );
    }
}

void visualize( list< Edge > APath, list< Edge > BPath, vector< vector< bool > > obstacle, Point mapSize ) {
    set< pair< Point, Point > > AEdges;
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

    cairo_set_line_width( cr, 1.0 / SCALE_W );

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
    // cairo_set_line_width( cr, 0.1 );

    // robot A is red
    cairo_set_source_rgb( cr, 1, 0, 0 );
    for ( list< Edge >::iterator it = APath.begin(); it != APath.end(); ++it ) {
        drawArrow( cr, it->to, it->from );
        AEdges.insert( make_pair( it->from, it->to ) );
    }
    cairo_stroke( cr );

    // robot B is green
    cairo_set_source_rgb( cr, 0, 1, 0 );
    for ( list< Edge >::iterator it = BPath.begin(); it != BPath.end(); ++it ) {
        if ( AEdges.find( make_pair( it->from, it->to ) ) != AEdges.end() ) {
            // common line
            cairo_set_source_rgb( cr, 0, 0, 1 );
        }
        else {
            // separate paths
            cairo_set_source_rgb( cr, 0, 1, 0 );
        }
        // printf( "Drawing line ( %i, %i ) -> ( %i, %i )\n", it->from.x, it->from.y, it->to.x, it->to.y );
        drawArrow( cr, it->to, it->from );
        cairo_stroke( cr );
    }

    cairo_surface_write_to_png( surface, "robots.png" );
}
