/*This source code copyrighted by Lazy Foo' Productions (2004-2013)
and may not be redistributed without written permission.*/

//The headers
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_mixer.h"
#include <string>
#include <iostream>
#include <fstream>
#include "Personaje.h"


//Screen attributes
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;

//The surfaces
SDL_Surface *background = NULL;
SDL_Surface *message = NULL;
SDL_Surface *screen = NULL;
Mix_Music *BGM = NULL;

bool turno=true;

 int cursor_x=0;
 int cursor_y=0;


//The event structure
SDL_Event event;

//The font
TTF_Font *font = NULL;

//The color of the font
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
    //Holds offsets
    SDL_Rect offset;

    //Get offsets
    offset.x = x;
    offset.y = y;

    //Blit
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
    background = load_image( "background.png" );

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
     Mix_FreeMusic( BGM );

    //Quit SDL_ttf
    TTF_Quit();
    Mix_CloseAudio();
    //Quit SDL
    SDL_Quit();
}

bool puedoLLegar(char tablero[5][5],int x_actual, int y_actual,int pasos, int x_final, int y_final)
{
    //Casos base
    if(pasos<0)
        return false;

    if(x_actual>=5
       || y_actual>=5
       || x_actual<0
       || y_actual<0)
       return false;

    if(tablero[y_actual][x_actual]=='#')
       return false;

    if(x_actual==x_final
       && y_actual==y_final)
       return true;

    if(tablero[y_actual][x_actual]=='O')
        pasos--;

    pasos--;

//    if(tablero[y_actual][x_actual]=='V'
//       && pasos<=0)
//        pasos=1;

    return puedoLLegar(tablero,x_actual+1,y_actual,pasos,x_final,y_final)
            || puedoLLegar(tablero,x_actual-1,y_actual,pasos,x_final,y_final)
            || puedoLLegar(tablero,x_actual,y_actual+1,pasos,x_final,y_final)
            || puedoLLegar(tablero,x_actual,y_actual-1,pasos,x_final,y_final);
}

void marcar(char tablero[5][5],char tablero_de_pasos[5][5],int x_actual, int y_actual,int pasos)
{
    //Casos base
    if(pasos<0)
        return;

    if(x_actual>=5
       || y_actual>=5
       || x_actual<0
       || y_actual<0)
       return;

    if(tablero[y_actual][x_actual]=='#')
       return;

    tablero_de_pasos[y_actual][x_actual]='P';

    if(tablero[y_actual][x_actual]=='O')
        pasos--;

    pasos--;

//    if(tablero[y_actual][x_actual]=='V'
//       && pasos<=0)
//        pasos=1;

    marcar(tablero,tablero_de_pasos,x_actual+1,y_actual,pasos);
    marcar(tablero,tablero_de_pasos,x_actual-1,y_actual,pasos);
    marcar(tablero,tablero_de_pasos,x_actual,y_actual+1,pasos);
    marcar(tablero,tablero_de_pasos,x_actual,y_actual-1,pasos);
}

void limpiar(char tablero_de_pasos[5][5])
{
    for(int x=0;x<5;x++)
        for(int y=0;y<5;y++)
            tablero_de_pasos[y][x]=' ';
}

 SDL_Surface* personaje_surface = load_image("personaje.png");
 SDL_Surface* personaje2_surface = load_image("personaje2.png");
 Personaje personaje(2,3,personaje_surface);
 Personaje personaje2(2,1,personaje2_surface);


int main( int argc, char* args[] )
{
    if( Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 ) == -1 )
    {
        return 1;
    }

    BGM = Mix_LoadMUS( "BGM.mp3" );

    if( BGM == NULL )
    {
        return 1;
    }
    bool first_time = true, first_it = true;
    Mix_PlayMusic(BGM, -1);

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
    TTF_Font *font = TTF_OpenFont( "lazy.ttf", 30 );

    SDL_Surface * gameover = load_image("gameover.png");


    char tablero[5][5]={{' ','#',' ','O','V'},
                        {' ','#',' ','#',' '},
                        {'V','#',' ','#',' '},
                        {'V','#',' ','#','#'},
                        {' ','O','O',' ',' '}};

    char tablero_de_pasos[5][5]={{' ',' ',' ',' ',' '},
                                 {' ',' ',' ',' ',' '},
                                 {' ',' ',' ',' ',' '},
                                 {' ',' ',' ',' ',' '},
                                 {' ',' ',' ',' ',' '}};


    char tablero_de_pasos2[5][5]={{' ',' ',' ',' ',' '},
                                 {' ',' ',' ',' ',' '},
                                 {' ',' ',' ',' ',' '},
                                 {' ',' ',' ',' ',' '},
                                 {' ',' ',' ',' ',' '}};

    SDL_Surface* pasos_surface = load_image("pasos.png");
    SDL_Surface * cursor_surface = load_image("cursor.png");

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

            //If a key was pressed
            if( event.type == SDL_KEYDOWN )
            {
                /*   if(cursor_y>2)
                        cursor_y=2;

                     if(cursor_y<-2)
                        cursor_y=-2;

                     if(cursor_x>2)
                      cursor_x=2;

                     if(cursor_x<-2)
                        cursor_x=-2;*/

                //Set the proper message surface
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
                                personaje.setX(cursor_x);
                                personaje.setY(cursor_y);

                                 std::ofstream out("cursor.txt");
                                 out<<cursor_x<<" ";
                                 out<<cursor_y<<" ";

                                personaje2.recibirAtaque();

                                cursor_x=personaje.x;
                                cursor_y=personaje.y;

                                //actual.dibujar(screen);
                                limpiar(tablero_de_pasos);
                                marcar(tablero,tablero_de_pasos,personaje.x,personaje.y,3);

                                turno=false;

                             }

                            }
                            else if(!turno)
                            {
                               if(puedoLLegar(tablero,personaje2.x,personaje2.y,3,cursor_x,cursor_y))
                               {
                                personaje2.setX(cursor_x);
                                personaje2.setY(cursor_y);

                                personaje.recibirAtaque();

                                 std::ofstream out("cursor.txt");
                                 out<<cursor_x<<" ";
                                 out<<cursor_y<<" ";

                                cursor_x=personaje2.x;
                                cursor_y=personaje2.y;

                                //actual.dibujar(screen);
                                limpiar(tablero_de_pasos);
                                marcar(tablero,tablero_de_pasos2,personaje2.x,personaje2.y,3);

                                turno=true;

                               }
                            }
                    break;
                }
            }


            //If the user has Xed out the window
            else if( event.type == SDL_QUIT )
            {
                //Quit the program
                quit = true;
            }

        }

        //Apply the background
        apply_surface( 0, 0, background, screen );

        //If a message needs to be displayed
        if( message != NULL )
        {
            //Apply the background to the screen
            apply_surface( 0, 0, background, screen );

            //Apply the message centered on the screen
            apply_surface( ( SCREEN_WIDTH - message->w ) / 2, ( SCREEN_HEIGHT - message->h ) / 2, message, screen );

            //Null the surface pointer
            message = NULL;
        }

        for(int x=0;x<5;x++)
            for(int y=0;y<5;y++)
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

        for(int x=0;x<5;x++)
            for(int y=0;y<5;y++)
                if(tablero_de_pasos[y][x]=='P')
                    apply_surface(x*75,y*75,pasos_surface,screen);


        apply_surface(cursor_x*75,cursor_y*75,cursor_surface,screen);

        apply_surface(400,30,msj1,screen);
        apply_surface(400,80,msj2,screen);
        apply_surface(400,50,vidas_surface,screen);
        apply_surface(400,100,vidas_surface2,screen);

        if(personaje.vida==0 || personaje2.vida==0)
        {
             SDL_Rect offset;
             offset.x = 0;
             offset.y = 0;
             SDL_BlitSurface( gameover, NULL, screen, &offset );
           // apply_surface( -10, 0,gameover, screen );

        }


        personaje.dibujar(screen) ;
        personaje2.dibujar(screen) ;

        //Update the screen
        if( SDL_Flip( screen ) == -1 )
        {
            return 1;
        }

    }

    //Clean up
    clean_up();

    return 0;
}
