#include "random_utils.h"
 /**
    * codes from : https://github.com/rflynn/c/blob/master/rand-normal-distribution.c
    * following gaussian distribution
    */
    double rand_normal(){
        double x,y;
        for(int i=0;i<(rand()%10)+1;i++){
         x = (double)random() / RAND_MAX;
        }
        for(int i=0;i<(rand()%10)+1;i++){
         y = (double)random() / RAND_MAX;
        }
        double z = sqrt((double)(-2 * log(x))) * cos((double)(2 * M_PI * y));
        return z;
    }
    double rand_pure(){
        double r;
        for(int i=0;i<(rand()%10)+1;i++){
             r = (double)random()/RAND_MAX;
        }
        return r;
    }
    /**
    * codes from https://jasoncrease.medium.com/zipf-54912d5651cc
    */
    double rand_zipfian(){
        const int _N = SHRT_MAX;
        double N = (double)_N;
        double s = rand_pure();
        double k = (double)((rand()%(_N))+1);
        double a = (pow(k, 1 - s) - 1) / (1 - s) + 0.5 + pow(k, -s) / 2 + s / 12 - pow(k, -1 - s) * s / 12;
        double b = (pow(N, 1 - s) - 1) / (1 - s) + 0.5 + pow(N, -s) / 2 + s / 12 - pow(N, -1 - s) * s / 12;

        return a / b;
    }
    
    int getRandomValue(unsigned char random_distribution_type){
        double (*func)()  = rand_pure;
        switch(random_distribution_type){
            case RANDOM_DISTRIBUTION_TYPE_NORMAL:{
                func = rand_normal;
                break;
            }
            case RANDOM_DISTRIBUTION_TYPE_ZIPFIAN:{
                func = rand_zipfian;
                break;
            }
            default:{
                func = rand_pure;
                break;
            }
        }
        return 5*SHRT_MAX+(SHRT_MAX*func());
    }