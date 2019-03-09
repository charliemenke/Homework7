/*
 *  BitMap.cpp
 *  HW 3
 */

#include "BitMap.hpp"
#include <stdio.h>
#include <iostream>
#include <cmath>
#include <thread>


BitMap* BitMap::DetectEdges(int th) const {
    
    // create bm that allocates same amount of space for new edge image
    BitMap* bm = new BitMap(width, height);
    std::thread** tharr = new std::thread*[th];
    
    for(int i = 0; i < th; i++) {
        
        int start = (height/th) * i;
        int end = (height/th)*(i+1);
        //std::cout << height << "\n";
        //if(!(width*height % th == 0) && i == th - 1) {
           // end = width*height;
        //}
        
        std::thread* th = new std::thread(&BitMap::calcEdge, *this, start, end, bm);
        tharr[i] = th;
        
    }
    
    for(int i = 0; i < th; i++) {
        tharr[i]->join();
    }
    delete [] tharr;
    
    return bm;
    
}

void BitMap::calcEdge(int start, int end, BitMap *bm) const {
    int Vx;
    int Vy;
    
    for(int x = 1; x < width - 1; x++) {
        for(int y = start; y < end; y++) {
            //std::cout << y << "\n";
            calcV(x, y, Vx, Vy);
            int edgeScale = edgeValue(Vx, Vy);
            bm->SetPixel(x, y, edgeScale, edgeScale, edgeScale, 0);
        }
    }
}

void BitMap::calcV(int x, int y, int &Vx, int &Vy) const {
    uint8_t r, g, b, a;
    
    //top
    uint8_t topGS;
    if(y != height - 1) {
        GetPixel(x, y + 1, r, g, b, a);
        topGS = grayScale(r, g, b);
    } else {
        topGS = 0;
    }
    
    //botom
    uint8_t bottomGS;
    if(y != 0) {
        GetPixel(x, y - 1, r, g, b, a);
        bottomGS = grayScale(r, g, b);
    } else {
        bottomGS = 0;
    }
    
    //left
    uint8_t leftGS;
    if(x != 0) {
        GetPixel(x - 1, y, r, g, b, a);
        leftGS = grayScale(r, g, b);
    } else {
        leftGS = 0;
    }
    
    // right
    uint8_t rightGS;
    if(x != width - 1) {
        GetPixel(x + 1, y, r, g, b, a);
        rightGS = grayScale(r, g, b);
    } else {
        rightGS = 0;
    }
    
    //top left
    uint8_t tlGS;
    if(x != 0 && y != height - 1) {
        GetPixel(x - 1, y + 1, r, g, b, a);
        tlGS = grayScale(r, g, b);
    } else {
        tlGS = 0;
    }
    
    //top right
    uint8_t trGS;
    if(x != width - 1 && y != height - 1) {
        GetPixel(x + 1, y + 1, r, g, b, a);
        trGS = grayScale(r, g, b);
    } else {
        trGS = 0;
    }
    
    //bottom left
    uint8_t blGS;
    if(x != 0 + 1 && y != 0) {
        GetPixel(x - 1, y - 1, r, g, b, a);
        blGS = grayScale(r, g, b);
    } else {
        blGS = 0;
    }
    
    // bottom right
    uint8_t brGS;
    if(x != width - 1 && y != 0) {
        GetPixel(x + 1, y - 1, r, g, b, a);
        brGS = grayScale(r, g, b);
    } else {
        brGS = 0;
    }
    
    // calc Vx
    int topSum = ((tlGS * -1) + (topGS * -2) + (trGS * -1));
    int bottomSum = ((blGS * 1) + (bottomGS * 2) + (brGS * 1));
    //std::cout << "TopSum: " << (int)topSum << "\n";
    //std::cout << "BottomSum: " << (int)bottomSum << "\n";
    Vx = topSum + bottomSum;
    //std::cout << "Vx: " << (int)Vx << "\n";
    
    // calc Vy
    int leftSum = ((tlGS * - 1) + (leftGS * -2) + (blGS * -1));
    int rightSum = ((trGS * 1) + (rightGS * 2) + (brGS * 1));
    //std::cout << "LeftSum: " << (int)leftSum << "\n";
    //std::cout << "RightSum: " << (int)rightSum << "\n";
    Vy = leftSum + rightSum;
    //std::cout << "Vy: " << (int)Vy << "\n";
}

uint8_t BitMap::grayScale(uint8_t r, uint8_t g, uint8_t b) const {
    uint8_t gs;
    //gs = ((0.3 * r) + (0.59 * g) + (0.11 * b));
    gs = (r + g + b)/3;
    return gs;
}

int BitMap::edgeValue(int Vx, int Vy) const {
    int r = sqrt((Vx * Vx) + (Vy * Vy));
    r = 255 - r;
    //std::cout << "edgeGS: " << (int)r << "\n";
    return r;
}

BitMap::BitMap(int w, int h):width(w), height(h)
{
    image = new uint8_t[width*height*4];
}

BitMap::BitMap(const char* file)
{
    bool loadFailed = false;
    FILE *f = fopen(file, "rb");
    BitMapHeader header;
    
    if (f == 0)
    {
        std::cout << "Unable to find file\n";
        loadFailed = true;
    }
    else
    {
        uint16_t bfType;
        fread(&bfType, sizeof(bfType), 1, f);
        if (bfType != 19778)
        {
            std::cout << "Unable to load file properly\n";
            fclose(f);
            loadFailed = true;
        }
        else
        {
            fread(&header, sizeof(BitMapHeader), 1, f);
            if (header.biBitCount < 24)
            {
                std::cout << "We only support 24 & 32-bit files\n";
                fclose(f);
                loadFailed = true;
            }
            else
            {
                std::cout << ":" << header.biBitCount << "bit image\n";
                
                if (header.biCompression != 0)
                {
                    std::cout << "We don't support compressed files\n";
                    fclose(f);
                    loadFailed = true;
                }
                else
                {
                    height = header.biHeight;
                    bool reverseHeight = false;
                    if ((int32_t)header.biHeight < 0)
                    {
                        std::cout << ":height reversed\n";
                        height = -height;
                        reverseHeight = true;
                    }
                    
                    width = header.biWidth;
                    image = new uint8_t[height*width*4];
                    
                    fseek(f, header.bfOffBits, SEEK_SET);
                    
                    if (header.biBitCount == 32)
                    {
                        for (int x = 0; x < height; x++)
                        {
                            if (reverseHeight)
                                fread(&image[(height-x-1)*width*4], sizeof(char), width*4, f);
                            else
                                fread(&image[x*width*4], sizeof(char), width*4, f);
                        }
                    }
                    else if (header.biBitCount == 24)
                    {
                        bool padding = false;
                        for (int x = 0; x < height; x++)
                        {
                            int bytesRead = 0;
                            for (int y = 0; y < width; y++)
                            {
                                if (reverseHeight)
                                    bytesRead += fread(&image[(height-x-1)*width*4+y*4], sizeof(char), 3, f);
                                else
                                    bytesRead += fread(&image[x*width*4+y*4], sizeof(char), 3, f);
                                image[x*width*4+y*4+3] = 0;
                            }
                            while (0 != bytesRead%4)
                            {
                                char zero[4] = {0, 0, 0, 0};
                                bytesRead += fread(zero, sizeof(char), 1, f);
                                padding = true;
                            }
                        }
                        if (padding)
                            std::cout << ":padding necessary\n";
                    }
                    fclose(f);
                }
            }
        }
    }
    
    if(loadFailed)
    {
        width = 10;
        height = 10;
        image = new uint8_t[width*height*4];
        for(int i=0; i < width*height*4; i++)
        {
            image[i] = 255;
        }
    }
    
    
}

BitMap::BitMap(const BitMap& b):width(b.width), height(b.height)
{
    image = new uint8_t[width*height*4];
    
    for(int i=0; i < width*height*4; i++)
    {
        image[i] = b.image[i];
    }
}

BitMap::~BitMap()
{
    width = 0;
    height = 0;
    if(image)
    {
        delete[] image;
        image = NULL;
    }
}

BitMap& BitMap::operator=(const BitMap& b)
{
    if(this == &b)
        return *this;
    
    width = b.width;
    height = b.height;
    
    delete[] image;
    image = new uint8_t[width*height*4];
    
    for(int i=0; i < width*height*4; i++)
    {
        image[i] = b.image[i];
    }
    
    return *this;
}

void BitMap::Save(const char *file)
{
    FILE *f = fopen(file, "w+");
    
    if (f == 0)
        return;
    
    BitMapHeader header;
    header.biWidth = width;
    header.biHeight = height;
    
    header.bfSize = sizeof(BitMapHeader)+2+(width)*height*4;
    header.biSizeImage = (width)*height*4;
    uint16_t bfType = 19778; // 0x4D42
    fwrite(&bfType, sizeof(bfType), 1, f);
    fwrite(&header, sizeof(header), 1, f);
    for (int x = 0; x < height; x++)
    {
        fwrite(&image[x*width*4], sizeof(char), width*4, f);
    }
    fclose(f);
}


void BitMap::SetPixel(int x, int y, uint8_t redByte, uint8_t greenByte, uint8_t blueByte, uint8_t alphaByte )
{
    // BGRA
    image[y*width*4+x*4+0] = blueByte;
    image[y*width*4+x*4+1] = greenByte;
    image[y*width*4+x*4+2] = redByte;
    image[y*width*4+x*4+3] = alphaByte;
}

void BitMap::GetPixel(int x, int y, uint8_t &redByte, uint8_t &greenByte, uint8_t &blueByte, uint8_t &alphaByte) const
{
    blueByte = image[y*width*4+x*4+0];
    greenByte = image[y*width*4+x*4+1];
    redByte = image[y*width*4+x*4+2];
    alphaByte = image[y*width*4+x*4+3];
}





