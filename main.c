#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "src/lua.h"
#include "src/lualib.h"
#include "src/lauxlib.h"

// MATEMATICA \o/
typedef struct { double x, y, z; } Vec3;

Vec3 vec3_new( double x, double y, double z ) { return ( Vec3 ){x, y, z}; }
Vec3 vec3_add( Vec3 a, Vec3 b ) { return vec3_new( a.x + b.x, a.y + b.y, a.z + b.z ); }
Vec3 vec3_sub( Vec3 a, Vec3 b ) { return vec3_new( a.x - b.x, a.y - b.y, a.z - b.z ); }
Vec3 vec3_scale( Vec3 v, double s ) { return vec3_new( v.x * s, v.y * s, v.z * s ); }
double vec3_dot( Vec3 a, Vec3 b ) { return a.x * b.x + a.y * b.y + a.z * b.z; }
Vec3 vec3_normalize( Vec3 v ) {
    double len = sqrt( vec3_dot( v, v ) );
    return ( len > 0 ) ? vec3_scale( v, 1.0 / len ) : v;
}

// Funcaozinha utilitaria pra cor nao passar do limite
double clamp( double x, double min, double max ) {
    if ( x < min ) return min;
    if ( x > max ) return max;
    return x;
}

typedef struct { Vec3 origin; Vec3 direction; } Ray;
typedef struct { Vec3 center; double radius; } Sphere;

double hit_sphere( const Sphere *s, const Ray *r ) {
    Vec3 oc = vec3_sub( r->origin, s->center );
    double b = 2.0 * vec3_dot( oc, r->direction );
    double c = vec3_dot( oc, oc ) - s->radius * s->radius;
    double delta = b * b - 4 * c;
    return ( delta < 0 ) ? -1.0 : ( -b - sqrt( delta ) ) / 2.0;
}

Vec3 ray_color( Ray r, Sphere s, int depth ) {
    if ( depth <= 0 ) return vec3_new( 0, 0, 0 );

    double t_sph = hit_sphere( &s, &r );
    double t_floor = -1.0;

    if ( fabs( r.direction.y ) > 0.0001 ) {
        double t = ( -0.5 - r.origin.y ) / r.direction.y;
        if ( t > 0.0 ) t_floor = t;
    }

    int hit_type = 0;
    double t_closest = 999999.0;

    if ( t_sph > 0.0 && t_sph < t_closest ) { t_closest = t_sph; hit_type = 1; }
    if ( t_floor > 0.0 && t_floor < t_closest ) { t_closest = t_floor; hit_type = 2; }

    if ( hit_type == 1 ) {
        Vec3 hit_point = vec3_add( r.origin, vec3_scale( r.direction, t_closest ) );
        Vec3 normal = vec3_normalize( vec3_sub( hit_point, s.center ) );
        Vec3 reflected_dir = vec3_sub( r.direction, vec3_scale( normal, 2.0 * vec3_dot( r.direction, normal ) ) );

        Ray reflected_ray = {vec3_add( hit_point, vec3_scale( normal, 0.001 ) ), reflected_dir};
        Vec3 reflected_color = ray_color( reflected_ray, s, depth - 1 );
        
        Vec3 sun_dir = vec3_normalize( vec3_new( 1.0, 1.0, -1.0 ) );
        double specular = pow( fmax( 0.0, vec3_dot( reflected_dir, sun_dir ) ), 50.0 );
        Vec3 specular_color = vec3_scale( vec3_new( 1.0, 1.0, 1.0 ), specular );

        return vec3_add( vec3_scale( reflected_color, 0.9 ), specular_color );

    } else if ( hit_type == 2 ) {
        Vec3 hit_point = vec3_add( r.origin, vec3_scale( r.direction, t_closest ) );
        
        int cx = ( int )floor( hit_point.x * 2.0 );
        int cz = ( int )floor( hit_point.z * 2.0 );
        
        if ( ( cx + cz ) % 2 == 0 ) {
            return vec3_new( 0.3, 0.3, 0.3 );
        } else {
            return vec3_new( 0.8, 0.8, 0.8 );
        }
        
    } else {
        Vec3 unit_dir = vec3_normalize( r.direction );

        double t = 0.5 * ( unit_dir.y + 1.0 );
        
        Vec3 white = vec3_new( 1.0, 1.0, 1.0 );
        Vec3 blue = vec3_new( 0.4, 0.6, 1.0 );
        
        return vec3_add( vec3_scale( white, 1.0 - t ), vec3_scale( blue, t ) );
    }
}

static int l_executar_raytracing( lua_State *L ) {
    int width = ( int )luaL_checkinteger( L, 1 );
    int height = ( int )luaL_checkinteger( L, 2 );
    const char *filename = luaL_checkstring( L, 3 );

    FILE *f = fopen( filename, "w" );
    if ( !f ) return 0;

    fprintf( f, "P3\n%d %d\n255\n", width, height );

    Sphere s = {vec3_new( 0, 0, -1.5 ), 0.5};
    
    Vec3 origin = vec3_new( 0, 0, 1.0 ); // camera mais recuadinha para ver melhor o chao
    
    double aspect_ratio = ( double )width / height;
    Vec3 horizontal = vec3_new( 2.0 * aspect_ratio, 0, 0 );
    Vec3 vertical = vec3_new( 0, 2.0, 0 );
    Vec3 lower_left = vec3_sub( origin, vec3_scale( horizontal, 0.5 ) );
    lower_left = vec3_sub( lower_left, vec3_scale( vertical, 0.5 ) );
    lower_left = vec3_sub( lower_left, vec3_new( 0, 0, 1.5 ) );

    for ( int j = height - 1; j >= 0; --j ) {
        for ( int i = 0; i < width; ++i ) {
            double u = ( double )i / ( width - 1 );
            double v = ( double )j / ( height - 1 );
            
            Vec3 dir = vec3_add( lower_left, vec3_add( vec3_scale( horizontal, u ), vec3_scale( vertical, v ) ) );
            dir = vec3_sub( dir, origin );
            
            Ray r = {origin, vec3_normalize( dir )};

            Vec3 pixel_color = ray_color( r, s, 5 );

            int ir = ( int )( 255.999 * clamp( pixel_color.x, 0.0, 0.999 ) );
            int ig = ( int )( 255.999 * clamp( pixel_color.y, 0.0, 0.999 ) );
            int ib = ( int )( 255.999 * clamp( pixel_color.z, 0.0, 0.999 ) );

            fprintf( f, "%d %d %d\n", ir, ig, ib );
        }
    }
    fclose( f );
    lua_pushboolean( L, 1 );
    return 1;
}

int main() {
    lua_State *L = luaL_newstate();
    luaL_openlibs( L );

    lua_newtable( L );
    lua_pushcfunction( L, l_executar_raytracing );
    lua_setfield( L, -2, "gerar_imagem" );
    lua_setglobal( L, "meuraytracer" );

    if ( luaL_dofile( L, "interface.lua" ) != LUA_OK ) {
        printf( "Erro: %s\n", lua_tostring( L, -1 ) );
    }

    lua_close( L );
    return 0;
}