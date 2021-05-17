#include "Serializable.h"

#include <iostream>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include "utils.h"

class Jugador: public Serializable
{
public:
    Jugador(const char * _n, int16_t _x, int16_t _y):x(_x),y(_y)
    {
        strncpy(name, _n, SIZE_OF_NAME);
        _size = sizeof(int16_t) + sizeof(int16_t) + sizeof(char)*SIZE_OF_NAME;
    };

    virtual ~Jugador(){};

    void to_bin()
    {
        //First we alocate data
        alloc_data(_size);
        //Then we fill the internal buffer field by field
        memcpy(_data, name,SIZE_OF_NAME);
        memcpy(_data + SIZE_OF_NAME, &x, sizeof(int16_t));
        memcpy(_data + SIZE_OF_NAME+sizeof(int16_t), &y, sizeof(int16_t));
    }

    int from_bin(char * data)
    {
        //First we alocate data
        alloc_data(_size);
        //Then we re-construct field by field with param data contents
        memcpy(name, data,SIZE_OF_NAME);
        memcpy(&x, data + SIZE_OF_NAME, sizeof(int16_t));
        memcpy(&x, data + SIZE_OF_NAME+ sizeof(int16_t), sizeof(int16_t));
        return 0;
    }

    void print()
    {
      printf("Name: " "%s\n"
             "x: %d," "y: %d\n",name, x,y);
    }

public:
    static constexpr unsigned int SIZE_OF_NAME = 80;
    char name[SIZE_OF_NAME];

    int16_t x;
    int16_t y;
};

int main(int argc, char **argv)
{
//Apartado 2
    Jugador one_r("", 0, 0);
    Jugador one_w("Player_ONE", 12, 345);
    int file = open("player.bin", O_RDWR | O_CREAT, 0666); //Create if non existing and open in write only mode
    if(file==-1)
      handleError("open");
    one_w.to_bin();
    int bytes = write(file, one_w.data(),one_w.size());
    if(bytes==-1)
      handleError("write");
    close(file);
    //con od -sa podemos ver el contenido del archivo esperado en una representación con caracteres legibles

//Apartado 3
    file = open("player.bin", O_RDONLY);
    if(file==-1)
      handleError("open");
    char* buffer = (char*)malloc(one_r.size());
    bytes = read(file, buffer, one_r.size());
    if(bytes==-1)
      handleError("read");

    one_r.from_bin(buffer);
    one_r.print();
    return 0;
}

