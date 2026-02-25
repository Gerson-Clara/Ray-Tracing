#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "src/lua.h"
#include "src/lualib.h"
#include "src/lauxlib.h"

typedef struct {
    double x, y, z;
} Vec3;

Vec3 vec3_new( double x, double y, double z ) {
    Vec3 v = {x, y, z};
    return v;
}

Vec3 vec3_sub( Vec3 a, Vec3 b ) {
    return vec3_new( a.x - b.x, a.y - b.y, a.z - b.z );
}

Vec3 vec3_add( Vec3 a, Vec3 b ) {
    return vec3_new( a.x + b.x, a.y + b.y, a.z + b.z );
}

Vec3 vec3_scale( Vec3 v, double s ) {
    return vec3_new( v.x * s, v.y * s, v.z * s );
}

double vec3_dot( Vec3 a, Vec3 b ) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vec3 vec3_normalize( Vec3 v ) {
    double len_squared = vec3_dot( v, v );
    if ( len_squared > 0 ) {
        double invLen = 1.0 / sqrt( len_squared );
        return vec3_scale( v, invLen );
    }
    return v;
}

typedef struct {
    Vec3 origin;
    Vec3 direction;
} Ray;

typedef struct {
    Vec3 center;
    double radius;
    Vec3 color;
} Sphere;

double hit_sphere( const Sphere *s, const Ray *r ) {
    Vec3 oc = vec3_sub( r->origin, s->center );
    double b = 2.0 * vec3_dot( oc, r->direction );
    double c = vec3_dot( oc, oc ) - s->radius * s->radius;

    double discriminant = b * b - 4 * c;
    if ( discriminant < 0 ) {
        return -1.0;
    } else {
        return ( -b - sqrt( discriminant ) ) / 2.0;
    }
}

Vec3 calculate_pixel_color( const Ray *r, const Sphere *s ) {
    double t = hit_sphere( s, r );

    if ( t > 0.0 ) {

        Vec3 hit_point = vec3_new( 
            r->origin.x + t * r->direction.x,
            r->origin.y + t * r->direction.y,
            r->origin.z + t * r->direction.z );

        Vec3 normal = vec3_normalize( vec3_sub( hit_point, s->center ) );

        Vec3 light_dir = vec3_normalize( vec3_new( 1.0, 1.0, -1.0 ) );
        double diff = fmax( 0.0, vec3_dot( normal, light_dir ) );

        double light_intensity = diff + 0.2;

        return vec3_scale( s->color, light_intensity );
    } else {
        return vec3_new( 0.0, 0.0, 0.0 );
    }
}

static int l_executar_raytracing( lua_State *L ) {
    int width = ( int )luaL_checkinteger( L, 1 );
    int height = ( int )luaL_checkinteger( L, 2 );
    const char *filename = luaL_checkstring( L, 3 );

    printf( "[C] Iniciando motor de Ray Tracing nativo...\n" );
    printf( "[C] Resolucao: %dx%d. Saida: %s\n", width, height, filename );

    FILE *f = fopen( filename, "w" );
    if ( !f ) {
        lua_pushboolean( L, 0 );
        lua_pushstring( L, "Erro ao criar arquivo de imagem." );
        return 2;
    }

    fprintf( f, "P3\n%d %d\n255\n", width, height );

    Sphere s = {vec3_new( 0.0, 0.0, -1.0 ), 0.5, vec3_new( 0.7, 0.7, 0.7 )};

    Vec3 origin = vec3_new( 0.0, 0.0, 0.0 );
    double aspect_ratio = ( double )width / ( double )height;
    double viewport_height = 2.0;
    double viewport_width = aspect_ratio * viewport_height;
    double focal_length = 1.0;
    Vec3 horizontal = vec3_new( viewport_width, 0.0, 0.0 );
    Vec3 vertical = vec3_new( 0.0, viewport_height, 0.0 );
    Vec3 lower_left_corner = vec3_sub( origin, vec3_scale( horizontal, 0.5 ) );
    lower_left_corner = vec3_sub( lower_left_corner, vec3_scale( vertical, 0.5 ) );
    lower_left_corner = vec3_sub( lower_left_corner, vec3_new( 0, 0, focal_length ) );

    printf( "[C] Calculando pixels... ( isso pode levar alguns segundos )\n" );

    for ( int j = height - 1; j >= 0; --j ) {
        for ( int i = 0; i < width; ++i ) {
            double u = ( double )i / ( width - 1 );
            double v = ( double )j / ( height - 1 );

            Vec3 dir_temp = vec3_add( lower_left_corner, vec3_scale( horizontal, u ) );
            dir_temp = vec3_add( dir_temp, vec3_scale( vertical, v ) );
            Vec3 direction = vec3_sub( dir_temp, origin );

            Ray r = {origin, vec3_normalize( direction )};

            Vec3 pixel_color = calculate_pixel_color( &r, &s );

            int ir = ( int )( 255.999 * pixel_color.x );
            int ig = ( int )( 255.999 * pixel_color.y );
            int ib = ( int )( 255.999 * pixel_color.z );

            fprintf( f, "%d %d %d\n", ir, ig, ib );
        }
    }

    fclose( f );
    printf( "[C] Concluido.\n" );

    lua_pushboolean( L, 1 );
    return 1;
}

int main( int argc, char **argv ) {
    printf( "--- Iniciando Aplicacao Hibrida C/Lua ---\n" );

    lua_State *L = luaL_newstate(  );
    luaL_openlibs( L );

    lua_newtable( L );
    lua_pushcfunction( L, l_executar_raytracing );
    lua_setfield( L, -2, "gerar_imagem" );
    lua_setglobal( L, "meuraytracer" );
    printf( "[APP] Carregando script de interface Lua...\n" );

    if ( luaL_dofile( L, "interface.lua" ) != LUA_OK ) {
        fprintf( stderr, "[ERRO LUA] %s\n", lua_tostring( L, -1 ) );
        lua_pop( L, 1 );
        lua_close( L );
        return 1;
    }

    lua_close( L );
    printf( "--- Aplicacao Encerrada ---\n" );
    return 0;
}