#include <cstdio>
#include <cmath>
#include <set>
#include <vector>
using namespace std;
struct Edge {
    public:
        int parent;
        pair< int, int > from;
        pair< int, int > to;
        int heuristic; // approximation of how far the "to" end of Edge is from the target
        int distance; // actual distance between the "to" end of the Edge and the source
        Edge( int parent, pair< int, int > from, pair< int, int > to, int heuristic, int distance ):
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
pair< int, int > mapSize;
set< int, CompareEdges > frontier;
set< pair< int, int > > visited;
vector< vector< bool > > obstacle;
int manhattan( pair< int, int > source, pair< int, int > target ) {
    return abs( ( double )source.first - target.first ) + abs( ( double )source.second - target.second );
}
void enqueue( int a ) {
    Edge e = E[ a ];
    if ( e.to.first < 0 || e.to.second < 0 || e.to.first >= mapSize.first || e.to.second >= mapSize.second || obstacle[ e.to.first ][ e.to.second ] || visited.find( e.to ) != visited.end() ) {
        return;
    }
    frontier.insert( a );
}
Edge aStar( pair< int, int > source, pair< int, int > target ) {
    E.push_back( Edge( NULL, source, source, manhattan( source, target ), 0 ) );
    frontier.insert( 0 );
    while ( !frontier.empty() ) {
        set< int, CompareEdges >::iterator it = frontier.begin();
        int a = *it;
        Edge e = E[ a ];
        frontier.erase( it );
        if ( visited.find( e.to ) != visited.end() ) {
            continue;
        }
        if ( e.to == target ) {
            E.clear(); visited.clear(); frontier.clear();
            return e;
        }
        visited.insert( e.to );
        for ( int x = e.to.first - 1; x <= e.to.first + 1; x += 2 ) {
            pair< int, int > next = make_pair( x, e.to.second );
            E.push_back( Edge( a, e.to, next, manhattan( next, target ), e.distance + 1 ) );
            enqueue( E.size() - 1 );
        }
        for ( int y = e.to.second - 1; y <= e.to.second + 1; y += 2 ) {
            pair< int, int > next = make_pair( e.to.first, y );
            E.push_back( Edge( a, e.to, next, manhattan( next, target ), e.distance + 1 ) );
            enqueue( E.size() - 1 );
        }
    }
}
int main() {
    pair< int, int > A, B, target;
    scanf( "%i %i %i %i %i %i %i %i\n", &mapSize.first, &mapSize.second, &A.first, &A.second, &B.first, &B.second, &target.first, &target.second );
    --A.first; --A.second; --B.first; --B.second; --target.first; --target.second;
    for ( int x = 0; x < mapSize.first; ++x ) {
        obstacle.push_back( vector< bool >( mapSize.second ) );
    }
    for ( int y = 0; y < mapSize.second; ++y ) {
        for ( int x = 0; x < mapSize.first; ++x ) {
            char c;
            scanf( "%c", &c );
            obstacle[ x ][ y ] = c == 'X';
        }
        scanf( "\n" );
    }
    Edge APath = aStar( target, A );
    Edge BPath = aStar( target, B );
    while ( APath.from != APath.to ) {
        printf( "(%i, %i)\n", APath.from.first + 1, APath.from.second + 1 );
        APath = E[ APath.parent ];
    }
    printf( "\n" );
    while ( BPath.from != BPath.to ) {
        printf( "(%i, %i)\n", BPath.from.first + 1, BPath.from.second + 1 );
        BPath = E[ BPath.parent ];
    }
    printf( "\n" );
    return 0;
}
