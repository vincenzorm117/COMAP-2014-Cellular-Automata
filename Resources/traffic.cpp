//
//  main.cpp
//  Traffic
//
//  Created by Vincenzo Marconi on 2/8/14.
//  Copyright (c) 2014 Vincenzo Marconi. All rights reserved.
//

#include <iostream>
#include <math.h>
#include <cstdlib>
#include <fstream>

using namespace std;

#define P_SPAWNING 50
#define P_SLOWING 1
#define LANE 6
#define LANESIZE 128
#define CYCLES 10000
#define START_METRICS 200
#define MAX_VEL 5
#define P_PASS_L 15
#define P_PASS_R 80
#define P_PASS_FAST 80
#define P_PASS_NO 1
#define LOOKBACK 2 //greater than 0!
#define PASSRESET 3
#define RH_RULE 0
#define NO_RULE 1
#define FAST_RULE 0
#define P_EXIT 1
#define P_ENTRANCE 80


struct block {
    bool occupied;
    int velocity;
    int passReset;
};
typedef struct block block;




int main(int argc, char const *argv[]) {
    
    
    srand((unsigned int)time(NULL));
    ofstream f;
    f.open("traffic.in");
    
    block freeway[LANE][LANESIZE];
    int i,j;
    int runCycles = CYCLES;
    int numCars[LANE];
    float passLeftVector[MAX_VEL+1];
    double avgFlowRate = 0;
    double avgVelocity = 0;
    double avgVelocityPerCycle[CYCLES+1];

    for (i = 0; i < CYCLES; i++) {
        avgVelocityPerCycle[i] = 0;
    }

    passLeftVector[0] = 0;
    for (i =1; i < MAX_VEL+1; i++) {
        passLeftVector[i] = i * P_PASS_L + P_PASS_L;
    }

    
    for(i = 0; i < LANE; i++){
        numCars[i] = 0;
        for(j = 0; j < LANESIZE; j++){
            if(j < LANESIZE/2 && P_SPAWNING > (rand() % 100)){
                freeway[i][j].occupied = true;
                freeway[i][j].velocity = (rand() % MAX_VEL) + 1;
                freeway[i][j].passReset = PASSRESET;
                numCars[i]++;
            }
            else{
                freeway[i][j].occupied = false;
                freeway[i][j].velocity = 0;
                freeway[i][j].passReset = 0;
            }
            
        }
    }
    
    
    f << LANE << endl;
    f << LANESIZE << endl;
    
    int totalCars = 0;
    for (i=0; i < LANE; i++) {
        cout << "numCars in LANE " << i << " " << numCars[i];
        cout << endl;
        totalCars += numCars[i];
    }
    cout << "total cars " << totalCars;
    cout << endl;
    
    
    while(runCycles-- > 0){
        // if (freeway[LANE-1][0].occupied && P_EXIT > rand()%100) {
        //     freeway[LANE-1][0].velocity = 0;
        //     freeway[LANE-1][0].occupied = false;
        //     freeway[LANE-1][0].passReset = 0;
        // }
        // else if (!freeway[LANE-1][0].occupied && P_ENTRANCE > rand()%100) {
        //     freeway[LANE-1][0].velocity = 3;
        //     freeway[LANE-1][0].occupied = true;
        //     freeway[LANE-1][0].passReset = 3;
        // }

        // if (freeway[LANE-1][LANESIZE-1].occupied && P_EXIT > rand()%100) {
        //     freeway[LANE-1][LANESIZE-1].velocity = 0;
        //     freeway[LANE-1][LANESIZE-1].occupied = false;
        //     freeway[LANE-1][LANESIZE-1].passReset = 0;
        // }
        // else if (!freeway[LANE-1][LANESIZE-1].occupied && P_ENTRANCE > rand()%100) {
        //     freeway[LANE-1][LANESIZE-1].velocity = 3;
        //     freeway[LANE-1][LANESIZE-1].occupied = true;
        //     freeway[LANE-1][LANESIZE-1].passReset = 3;
        // }
        // else {}
        //Step 0: Increase each cars velocity by 1 unit
        for(i = 0; i < LANE; i++){
            for(j = 0; j < LANESIZE; j++){
                if(freeway[i][j].occupied){
                    freeway[i][j].velocity = (int)fmin(freeway[i][j].velocity+1, MAX_VEL);
                    freeway[i][j].passReset = (int)fmax(freeway[i][j].passReset-1, 0);
                }
            }
        }
        
        int dist;
        int k;
        //Step 1: Passing Phase
        if (FAST_RULE) { //if fast rule being applied
            if (LANE > 1 && P_PASS_FAST > 0) { //if there are multiple lanes
                //pass to the left phase
                for(i = 1; i < LANE; i++) { //begin on the second to leftmost lane and proceed to the right most lane
                    for(j = 0; j < LANESIZE; j++){
                        if (freeway[i][j].occupied && freeway[i][j].passReset == 0) {
                            dist = 0;
                            k = (j+1)%LANESIZE;
                            while(!freeway[i][k].occupied && dist < freeway[i][j].velocity) { //determine gap
                                k = (k+1)%LANESIZE;
                                dist++;
                            }
                            if (dist < freeway[i][j].velocity) { //if needs to slow down
                                dist = 0;

                                k = (j+1)%LANESIZE;
                                while(!freeway[i-1][k].occupied && dist < freeway[i][j].velocity) { //determine gap
                                    k = (k+1)%LANESIZE;
                                    dist++;
                                }

                                if (dist == freeway[i][j].velocity) {
                                    dist = 0;
                                    k = j;
                                    while(!freeway[i-1][k].occupied && dist < LOOKBACK) { //check lefter lane by lookback distance
                                        k = ((k-1)%LANESIZE < 0)?( (k-1)%LANESIZE + LANESIZE ):( (k-1)%LANESIZE );
                                        dist++;
                                    }
                                    
                                    if (dist == LOOKBACK && passLeftVector[freeway[i][j].velocity] >= rand()%100) {
                                        freeway[i-1][j].velocity = freeway[i][j].velocity;
                                        freeway[i-1][j].passReset = 3;
                                        freeway[i-1][j].occupied = true;
                                                                                
                                        freeway[i][j].occupied = false;
                                        freeway[i][j].passReset = 0;
                                        freeway[i][j].velocity = 0;
                                    }
                                }
                            }
                        }
                    }
                }
                //pass to the right phase
                for(i = LANE-2; i >= 0; i--) { 
                    for(j = 0; j < LANESIZE; j++){
                        if (freeway[i][j].occupied && freeway[i][j].passReset == 0) {
                            dist = 0;
                            k = (j+1)%LANESIZE;
                            while(!freeway[i][k].occupied && dist < freeway[i][j].velocity) { //determine gap
                                k = (k+1)%LANESIZE;
                                dist++;
                            }
                            if (dist < freeway[i][j].velocity) { //if needs to slow down
                                dist = 0;
                                k = j;
                                while(!freeway[i+1][k].occupied && dist < freeway[i][j].velocity) { //determine gap
                                    k = (k+1)%LANESIZE;
                                    dist++;
                                }

                                if (dist == freeway[i][j].velocity) {//if at least more velocity in left lane
                                    dist = 0;
                                    k = j;
                                    while(!freeway[i+1][k].occupied && dist < LOOKBACK) { //check lefter lane by lookback distance
                                        k = ((k-1)%LANESIZE < 0)?( (k-1)%LANESIZE + LANESIZE ):( (k-1)%LANESIZE );
                                        dist++;
                                    }
                                    
                                    if (dist == LOOKBACK && passLeftVector[freeway[i][j].velocity] >= rand()%100) {
                                        freeway[i+1][j].velocity = freeway[i][j].velocity;
                                        freeway[i+1][j].passReset = 3;
                                        freeway[i+1][j].occupied = true;
                                        
                                        freeway[i][j].occupied = false;
                                        freeway[i][j].passReset = 0;
                                        freeway[i][j].velocity = 0;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        else if (RH_RULE) { //if rh rule is being used
            //Step 1.1: Passing Phase Left
            if (LANE > 1 && P_PASS_L > 0) { //if there are multiple lanes
                for(i = 1; i < LANE; i++) { //begin on the second to leftmost lane and proceed to the right most lane
                    for(j = 0; j < LANESIZE; j++){
                        if (freeway[i][j].occupied && freeway[i][j].passReset == 0) {
                            dist = 0;
                            k = (j+1)%LANESIZE;
                            while(!freeway[i][k].occupied && dist < freeway[i][j].velocity) { //determine gap
                                k = (k+1)%LANESIZE;
                                dist++;
                            }
                            if (dist < freeway[i][j].velocity) { //if needs to slow down
                                dist = 0;

                                k = (j+1)%LANESIZE;
                                while(!freeway[i-1][k].occupied && dist < freeway[i][j].velocity) { //determine gap
                                    k = (k+1)%LANESIZE;
                                    dist++;
                                }

                                if (dist == freeway[i][j].velocity) {
                                    dist = 0;
                                    k = j;
                                    while(!freeway[i-1][k].occupied && dist < LOOKBACK) { //check lefter lane by lookback distance
                                        k = ((k-1)%LANESIZE < 0)?( (k-1)%LANESIZE + LANESIZE ):( (k-1)%LANESIZE );
                                        dist++;
                                    }
                                    
                                    if (dist == LOOKBACK && passLeftVector[freeway[i][j].velocity] >= rand()%100) {
                                        freeway[i-1][j].velocity = freeway[i][j].velocity;
                                        freeway[i-1][j].passReset = 3;
                                        freeway[i-1][j].occupied = true;
                                                                                
                                        freeway[i][j].occupied = false;
                                        freeway[i][j].passReset = 0;
                                        freeway[i][j].velocity = 0;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            //Step 1.2: Passing Phase Right
            if (LANE > 1 && P_PASS_R > 0) { //if there are multiple lanes
                for(i = LANE-2; i >= 0; i--) { //begin on the second to rightmost lane and proceed to the leftmost lane.
                    for(j = 0; j < LANESIZE; j++) {
                        if(freeway[i][j].occupied && freeway[i][j].passReset == 0) { //if hasn't passed in a while
                            dist = 0;
                            k=j;
                            while(!freeway[i+1][k].occupied && dist < LOOKBACK) {
                                k = ((k-1)%LANESIZE < 0)?( (k-1)%LANESIZE + LANESIZE ):( (k-1)%LANESIZE );
                                dist++;
                            }
                            if(dist == LOOKBACK && P_PASS_R >= rand()%100) {
                                freeway[i+1][j].velocity = freeway[i][j].velocity;
                                freeway[i+1][j].passReset = PASSRESET;
                                freeway[i+1][j].occupied = true;
                                                                
                                freeway[i][j].occupied = false;
                                freeway[i][j].passReset = 0;
                                freeway[i][j].velocity = 0;
                            }
                        }
                    }
                }
            }
        }
        else if (NO_RULE) {
            if (LANE > 1 && P_PASS_NO > 0) { //if there are multiple lanes
                //pass to the left phase
                for(i = 1; i < LANE; i++) { //begin on the second to leftmost lane and proceed to the right most lane
                    for(j = 0; j < LANESIZE; j++){
                        if (freeway[i][j].occupied && freeway[i][j].passReset == 0) {
                            dist = 0;
                            k = j;

                            while(!freeway[i-1][k].occupied && dist < LOOKBACK) { //check lefter lane by lookback distance
                                k = ((k-1)%LANESIZE < 0)?( (k-1)%LANESIZE + LANESIZE ):( (k-1)%LANESIZE );
                                dist++;
                            }
                            
                            if (dist == LOOKBACK && P_PASS_NO >= rand()%100) {
                                freeway[i-1][j].velocity = freeway[i][j].velocity;
                                freeway[i-1][j].passReset = 3;
                                freeway[i-1][j].occupied = true;
                                
                                freeway[i][j].occupied = false;
                                freeway[i][j].passReset = 0;
                                freeway[i][j].velocity = 0;
                            }
                        }
                    }
                }
                //pass to the right phase
                for(i = LANE-2; i >= 0; i--) { //begin on the second to leftmost lane and proceed to the right most lane
                    for(j = 0; j < LANESIZE; j++){
                        if (freeway[i][j].occupied && freeway[i][j].passReset == 0) {
                            dist = 0;
                            k = j;

                            while(!freeway[i+1][k].occupied && dist < LOOKBACK) { //check lefter lane by lookback distance
                                k = ((k-1)%LANESIZE < 0)?( (k-1)%LANESIZE + LANESIZE ):( (k-1)%LANESIZE );
                                dist++;
                            }
                            
                            if (dist == LOOKBACK && P_PASS_NO >= rand()%100) {
                                freeway[i+1][j].velocity = freeway[i][j].velocity;
                                freeway[i+1][j].passReset = 3;
                                freeway[i+1][j].occupied = true;
                                
                                freeway[i][j].occupied = false;
                                freeway[i][j].passReset = 0;
                                freeway[i][j].velocity = 0;
                            }
                        }
                    }
                }
            }
        }
        else {}
        
        
        
        
        //Step 2: Slow down if some one is in the way
        for(i = 0; i < LANE; i++){
            for(j = 0; j < LANESIZE; j++){
                if(freeway[i][j].occupied){
                    
                    dist = 0;
                    k = (j + 1)%LANESIZE;
                    while(!freeway[i][k].occupied && dist < freeway[i][j].velocity) {
                        k = (k+1)%LANESIZE;
                        dist++;
                    }
                    
                    //for (k = j+1; !freeway[i][k].occupied; k = (k + 1)%LANESIZE )
                    //    dist++;
                    
                    
                    freeway[i][j].velocity = (int)fmin(freeway[i][j].velocity, dist);
                    
                }
            }
        }
        
        
        
        
        
        //Step 3: Randomization of slowing down
        for(i = 0; i < LANE; i++){
            for(j = 0; j < LANESIZE; j++){
                if(freeway[i][j].occupied && freeway[i][j].velocity > 0){
                    if(P_SLOWING >= ( rand() % 100) ){
                        freeway[i][j].velocity--;
                    }
                }
            }
        }
        
        
        
        
        int vel;
        int numCars;
        //Step 4: Update Position
        for(i = 0; i < LANE ; i++){
            numCars = 0;
            for (j = 0; j < LANESIZE; j++) {
                if (freeway[i][j].occupied) {
                    numCars++;
                }
            }
            j = 0;
            while(numCars > 0){
                if(!freeway[i][j].occupied) j = (j+1)%LANESIZE;
                else if(freeway[i][j].velocity > 0){  //occupied
                    vel = freeway[i][j].velocity;
                    
                    freeway[i][ (j+vel)%LANESIZE ].velocity = freeway[i][j].velocity;
                    freeway[i][ (j+vel)%LANESIZE ].passReset = freeway[i][j].passReset;
                    freeway[i][ (j+vel)%LANESIZE ].occupied = true;
                    
                    
                    freeway[i][j].occupied = false;
                    freeway[i][j].passReset = 0;
                    freeway[i][j].velocity = 0;
                    
                    j=(j+vel+1)%LANESIZE;
                    numCars--;
                }
                else {//occupied but velocity 0
                    j = (j+1)%LANESIZE;
                    numCars--;
                }
            }
        }
        
        //Step 5: Statistics
        //calc density and average velocity
        if (CYCLES - runCycles > START_METRICS) {
            int velocity = 0;
            for (i = 0; i < LANE; i++) {
                for (j = LANESIZE/2-MAX_VEL; j < LANESIZE/2+MAX_VEL; j++) {
                    if (freeway[i][j].occupied)
                        velocity += freeway[i][j].velocity;
                }
            }

            double flowRate = (float)velocity / (LANE * 2 * MAX_VEL);
            avgFlowRate += flowRate;

            //calculate global average velocity
            velocity = 0;
            int numberCars = 0;
            for (i = 0; i < LANE; i++) {
                for (j = 0; j < LANESIZE; j++) {
                    if (freeway[i][j].occupied) {
                        velocity += freeway[i][j].velocity;
                        numberCars++;
                    }
                }
            }
            avgVelocityPerCycle[CYCLES-runCycles] = (float)velocity / numberCars;
            avgVelocity += (((float)velocity) / numberCars);
        }

        
        //print to file
        for(i = 0; i < LANE; i++){
            for(j = 0; j < LANESIZE; j++){
                f << freeway[i][j].occupied;
                f << " ";
                f << freeway[i][j].velocity;
                f << " ";
            }
            f << endl;
        }
    }

    for (i = 0; i < LANE; i++) {
        numCars[i] = 0;
        for(j = 0; j < LANESIZE; j++) {
            if (freeway[i][j].occupied) numCars[i]++;
        }   
    }
    totalCars = 0;
    for (i=0; i < LANE; i++) {
        cout << "numCars in LANE " << i << " " << numCars[i];
        cout << endl;
        totalCars += numCars[i];
    }

    avgFlowRate /= CYCLES;
    avgVelocity /= CYCLES;

    float varianceVel = 0;
    for (i = 0; i < CYCLES-START_METRICS; i++) {
        varianceVel += (avgVelocityPerCycle[i] - avgVelocity)*(avgVelocityPerCycle[i] - avgVelocity);
    }
    varianceVel /= (float)(CYCLES-START_METRICS);

    cout << "total cars " << totalCars << endl;
    cout << "Average Flow rate " << avgFlowRate << endl;
    cout << "Average Velocity " << avgVelocity << endl;
    cout << "Variance of Velocity " << varianceVel << endl;
    cout << "Standard Deviation of Velocity " << sqrt(varianceVel) << endl;
    f.close();
    return 0;
}
















