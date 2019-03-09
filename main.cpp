/*
 *  main.cpp
 *  HW 7
 */

#include <iostream>
#include "BitMap.hpp"
#include <thread>

int main(int argc, const char * argv[])
{
    const char * image1 = "large.bmp";
    const char * image2 = "mountain.bmp";
    const char * image3 = "train.bmp";
    
    const char * image1out = "large new.bmp";
    const char * image2out = "mountain new.bmp";
    const char * image3out = "train new.bmp";
    
    const char *input[3] = {image1, image2, image3};
    const char *output[3] = {image1out, image2out, image3out};
    
    
    for (int x = 0; x < 3; x++)
    {
        std::cout << "Processing " << input[x] << "\n";
        BitMap* bm = new BitMap(input[x]);
        if (bm == 0)
        {
            std::cout << "Load of " << input[x] << " failed\n";
        }
        else {
            std::cout << "Using 1 thread\n";
            BitMap* newBitmap = nullptr;
            double elapsedTime;
            std::chrono::high_resolution_clock::time_point startTime;
            startTime = std::chrono::high_resolution_clock::now();
            
            newBitmap = bm->DetectEdges(1);//std::thread::hardware_concurrency());
            
            auto stopTime =  std::chrono::high_resolution_clock::now();
            auto difference = stopTime - startTime;
            std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(difference);
            elapsedTime = time_span.count();
            std::cout << "Time: " << elapsedTime <<"s\n";
            delete newBitmap;
            newBitmap = nullptr;
            
            std::cout << "Using " << std::thread::hardware_concurrency() << " threads\n";
            startTime = std::chrono::high_resolution_clock::now();
            newBitmap = bm->DetectEdges(std::thread::hardware_concurrency());
            stopTime =  std::chrono::high_resolution_clock::now();
            difference = stopTime - startTime;
            time_span = std::chrono::duration_cast<std::chrono::duration<double>>(difference);
            elapsedTime = time_span.count();
            std::cout << "Time: " << elapsedTime <<"s\n";
            newBitmap->Save(output[x]);
            delete newBitmap;
            
            delete bm;
        }
    }
    
    return 0;
}
