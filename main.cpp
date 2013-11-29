#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_mixer.h"
#include <string>
#include <iostream>
#include <fstream>
#include "Personaje.h"

//Screen attributes
const int SCREEN_WIDTH = 790;
const int SCREEN_HEIGHT = 600;
const int SCREEN_BPP = 32;

//The surfaces
SDL_Surface *background = NULL;
SDL_Surface *message = NULL;
SDL_Surface *screen = NULL;
Mix_Music *music = NULL;

bool turno=true;

 int cursor_x=0;
 int cursor_y=0;

SDL_Event event;
TTF_Font *font = NULL;
SDL_Color textColor = { 0, 0, 0 };

std::string toString(int number)
{
    if (number == 0)
        return "0";
    std::string temp="";
    std::string returnvalue="";
    while (number>0)
    {
        temp+=number%10+48;
        number/=10;
    }
    for (int i=0;i<(int)temp.length();i++)
        returnvalue+=temp[temp.length()-i-1];
    return returnvalue;
}

SDL_Surface *load_image( std::string filename )
{
    return IMG_Load( filename.c_str() );
}

void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip = NULL )
{
    SDL_Rect offset;
    offset.x = x;
    offset.y = y;
    SDL_BlitSurface( source, clip, destination, &offset );
}

bool init()
{
    //Initialize all SDL subsystems
    if( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )
    {
        return false;
    }

    //Set up the screen
    screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE );

    //If there was an error in setting up the screen
    if( screen == NULL )
    {
        return false;
    }

    //Initialize SDL_ttf
    if( TTF_Init() == -1 )
    {
        return false;
    }

    //Set the window caption
    SDL_WM_SetCaption( "LABERINTO", NULL );

    //If everything initialized fine
    return true;
}

bool load_files()
{
    //Load the background image
    background = load_image( "tablero/background.png" );

    //Open the font
    font = TTF_OpenFont( "lazy.ttf", 30 );

    //If there was a problem in loading the background
    if( background == NULL )
    {
        return false;
    }

    //If there was an error in loading the font
    if( font == NULL )
    {
        return false;
    }

    //If everything loaded fine
    return true;
}

void clean_up()
{
    //Free the surfaces
    SDL_FreeSurface( background );

    //Close the font
    TTF_CloseFont( font );
    Mix_FreeMusic( music );

    //Quit SDL_ttf
    TTF_Quit();
    Mix_CloseAudio();
    //Quit SDL
    SDL_Quit();
}

bool puedoLLegar(char tablero[8][8],int x_actual, int y_actual,int pasos, int x_final, int y_final)
{
    //Casos base
    if(pasos<0)
        return false;

    if(x_actual>=8 || y_actual>=8 || x_actual<0 || y_actual<0)
       return false;

    if(tablero[y_actual][x_actual]=='#')
       return false;

    if(x_actual==x_final && y_actual==y_final)
       return true;

    if(tablero[y_actual][x_actual]=='O')
        pasos--;

    if(tablero[y_actual][x_actual]=='V')
        pasos++;

    if(tablero[y_actual][x_actual]=='V' && pasos<=0)
            pasos=1;
    pasos--;

    return puedoLLegar(tablero,x_actual+1,y_actual,pasos,x_final,y_final)
            || puedoLLegar(tablero,x_actual-1,y_actual,pasos,x_final,y_final)
            || puedoLLegar(tablero,x_actual,y_actual+1,pasos,x_final,y_final)
            || puedoLLegar(tablero,x_actual,y_actual-1,pasos,x_final,y_final);
}

void marcar(char tablero[8][8],char tablero_de_pasos[8][8],char rango_ataque[8][8],int x_actual, int y_actual,int pasos)
{
    if(pasos<0)
        return;

    if(x_actual>=8 || y_actual>=8  || x_actual<0 || y_actual<0)
        return;

    if(tablero[y_actual][x_actual]=='#')
       return;

    tablero_de_pasos[y_actual][x_actual]='P';

    if (pasos>=1&&tablero[y_actual][x_actual]==' ')
        rango_ataque[y_actual][x_actual]='A';

    if(tablero[y_actual][x_actual]=='O')
        pasos--;

    if(tablero[y_actual][x_actual]=='V')
        pasos++;
    if(tablero[y_actual][x_actual]=='V' && pasos<=0)
            pasos=1;
    pasos--;

    marcar(tablero,tablero_de_pasos,rango_ataque,x_actual+1,y_actual,pasos);
    marcar(tablero,tablero_de_pasos,rango_ataque,x_actual-1,y_actual,pasos);
    marcar(tablero,tablero_de_pasos,rango_ataque,x_actual,y_actual+1,pasos);
    marcar(tablero,tablero_de_pasos,rango_ataque,x_actual,y_actual-1,pasos);
}

void limpiar(char tablero_de_pasos[8][8], char rango_ataque[8][8])
{
    for(int x=0;x<8;x++)
        for(int y=0;y<8;y++)
        {
           tablero_de_pasos[y][x]=' ';
           rango_ataque[y][x]=' ';
        }

}

 SDL_Surface* personaje_surface = load_image("tablero/personaje.png");
 SDL_Surface* personaje2_surface = load_image("tablero/personaje2.png");
 Personaje personaje(2,3,personaje_surface);
 Personaje personaje2(6,1,personaje2_surface);

int main( int argc, char* args[] )
{
    if( Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 ) == -1 )
    {
        return 1;
    }

    music = Mix_LoadMUS( "music.mp3" );

    if( music == NULL )
    {
        return 1;
    }

    //Mix_PlayMusic(music, -1);

   //Quit flag
    bool quit = false;

    //Initialize
    if( init() == false )
    {
        return 1;
    }

    //Load the files
    if( load_files() == false )
    {
        return 1;
    }

    SDL_Surface* pasillo = load_image("tablero/pasillo.png");
    SDL_Surface* muro = load_image("tablero/muro.png");
    SDL_Surface* cursor = load_image("tablero/cursor.png");
    SDL_Surface* obstaculo = load_image("tablero/obstaculo.png");
    SDL_Surface* ventaja = load_image("tablero/ventaja.png");


    TTF_Font *font = TTF_OpenFont( "lazy.ttf", 24 );

    SDL_Surface * gameover = load_image("tablero/gameover.png");


    char tablero[8][8]=        {{' ','#',' ','O','V',' ','O','#'},
                                {' ','#',' ','#',' ','#',' ','#'},
                                {' ','#','V','#',' ',' ',' ','#'},
                                {' ','#',' ','#',' ','#',' ','#'},
                                {' ','#',' ','#',' ',' ','V','#'},
                                {'V','#',' ','#','V','#',' ','#'},
                                {' ',' ',' ','#',' ',' ',' ',' '},
                                {' ','O',' ',' ',' ','#',' ','#'}};

    char tablero_de_pasos[8][8]={{' ',' ',' ',' ',' ',' ',' ',' '},
                                 {' ',' ',' ',' ',' ',' ',' ',' '},
                                 {' ',' ',' ',' ',' ',' ',' ',' '},
                                 {' ',' ',' ',' ',' ',' ',' ',' '},
                                 {' ',' ',' ',' ',' ',' ',' ',' '},
                                 {' ',' ',' ',' ',' ',' ',' ',' '},
                                 {' ',' ',' ',' ',' ',' ',' ',' '},
                                 {' ',' ',' ',' ',' ',' ',' ',' '}};

    char rango_ataque[8][8]=   {{' ',' ',' ',' ',' ',' ',' ',' '},
                                {' ',' ',' ',' ',' ',' ',' ',' '},
                                {' ',' ',' ',' ',' ',' ',' ',' '},
                                {' ',' ',' ',' ',' ',' ',' ',' '},
                                {' ',' ',' ',' ',' ',' ',' ',' '},
                                {' ',' ',' ',' ',' ',' ',' ',' '},
                                {' ',' ',' ',' ',' ',' ',' ',' '},
                                {' ',' ',' ',' ',' ',' ',' ',' '}};

    SDL_Surface* pasos_surface = load_image("tablero/pasos.png");
    SDL_Surface* ataque = load_image("tablero/ataque.png");
    SDL_Surface * cursor_surface = load_image("tablero/cursor.png");

    SDL_Surface * msj1 = TTF_RenderText_Solid( font, "Personaje 1", textColor );
    SDL_Surface * msj2 = TTF_RenderText_Solid( font, "Personaje 2", textColor );

    //While the user hasn't quit
    while( quit == false )
    {
    SDL_Surface * vidas_surface = TTF_RenderText_Solid( font, toString(personaje.vida).c_str(), textColor );
    SDL_Surface * vidas_surface2 = TTF_RenderText_Solid( font, toString(personaje2.vida).c_str(), textColor );

        //If there's an event to handle
        if( SDL_PollEvent( &event ) )
        {
             if (cursor_x>7)
                cursor_x=7;
             if (cursor_y>7)
                cursor_y=7;
             if (cursor_x<0)
                cursor_x=0;
             if (cursor_y<0)
                cursor_y=0;
            //If a key was pressed
            if( event.type == SDL_KEYDOWN )
            {
                switch( event.key.keysym.sym )
                {
                    case SDLK_UP:
                          cursor_y--;
                    break;
                    case SDLK_DOWN:
                         cursor_y++;
                    break;
                    case SDLK_LEFT:
                         cursor_x--;
                    break;
                    case SDLK_RIGHT:
                         cursor_x++;
                    break;
                    case SDLK_RETURN:
                         if(turno)
                            {
                             if(puedoLLegar(tablero,personaje.x,personaje.y,3,cursor_x,cursor_y))
                             {
                                personaje.x=cursor_x;
                                personaje.y=cursor_y;

                                if (rango_ataque[personaje.y][personaje.x]=='A')
                                  personaje2.recibirAtaque();

                                limpiar(tablero_de_pasos,rango_ataque);
                                marcar(tablero,tablero_de_pasos,rango_ataque,personaje.x,personaje.y,3);

                                turno=false;
                             }

                            }
                            else if(!turno)
                            {
                               if(puedoLLegar(tablero,personaje2.x,personaje2.y,3,cursor_x,cursor_y))
                               {
                                personaje2.x=cursor_x;
                                personaje2.y=cursor_y;

                                if (rango_ataque[personaje2.y][personaje2.x]=='A')
                                  personaje.recibirAtaque();

                                limpiar(tablero_de_pasos,rango_ataque);
                                marcar(tablero,tablero_de_pasos,rango_ataque,personaje2.x,personaje2.y,3);

                                turno=true;
                               }
                            }
                    break;
                }
            }

            else if( event.type == SDL_QUIT )
            {
                quit = true;
            }

        }

        apply_surface( 0, 0, background, screen );

        if( message != NULL )
        {
            apply_surface( 0, 0, background, screen );
            apply_surface( ( SCREEN_WIDTH - message->w ) / 2, ( SCREEN_HEIGHT - message->h ) / 2, message, screen );
            message = NULL;
        }

        for(int x=0;x<8;x++)
            for(int y=0;y<8;y++)
            {
                if(tablero[y][x]==' ')
                    apply_surface(x*75,y*75,pasillo,screen);
                if(tablero[y][x]=='#')
                    apply_surface(x*75,y*75,muro,screen);
                if(tablero[y][x]=='O')
                    apply_surface(x*75,y*75,obstaculo,screen);
                if(tablero[y][x]=='V')
                    apply_surface(x*75,y*75,ventaja,screen);
            }

        for(int x=0;x<8;x++)
            for(int y=0;y<8;y++)
                if(tablero_de_pasos[y][x]=='P')
                    apply_surface(x*75,y*75,pasos_surface,screen);

        for(int x=0;x<8;x++)
         {
            for(int y=0;y<8;y++){
            if(tablero_de_pasos[y][x]=='P'){
            apply_surface(x*75,y*75,pasos_surface,screen);
            }
             if(rango_ataque[y][x]=='A'){
               apply_surface(x*75,y*75,ataque,screen);
             }
            }
         }

        apply_surface(cursor_x*75,cursor_y*75,cursor_surface,screen);

        apply_surface(635,30,msj1,screen);
        apply_surface(635,80,msj2,screen);
        apply_surface(635,50,vidas_surface,screen);
        apply_surface(635,100,vidas_surface2,screen);



        personaje.dibujar(screen) ;
        personaje2.dibujar(screen) ;

        if(personaje.vida==0 || personaje2.vida==0)
        {
             SDL_Rect offset;
             offset.x = 0;
             offset.y = 0;
             SDL_BlitSurface( gameover, NULL, screen, &offset );
        }

        //Update the screen
        if( SDL_Flip( screen ) == -1 )
        {
            return 1;
        }

    }

    clean_up();

    return 0;
}
