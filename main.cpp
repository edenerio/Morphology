#include <iostream>
#include <fstream>
using namespace std;

class Morphology{
    public:
        int numImgRows, numImgCols, imgMin, imgMax,numStructRows;
        int numStructCols, structMin, structMax, rowOrigin, colOrigin;
        int rowFrameSize, colFrameSize, extraRows, extraCols, rowSize, colSize;
        int **zeroFramedAry;
        int **morphAry;
        int **tempAry;
        int **structAry;
    
    Morphology(int imgRows, int imgCols, int structRows, int structCols){
        numImgRows = imgRows;
        numImgCols = imgCols;
        numStructRows = structRows;
        numStructCols = structCols;
        rowFrameSize = numStructRows/2;
        colFrameSize = numStructCols/2;
        extraRows = numStructCols*2;
        extraCols = colFrameSize*2;
        rowSize = numImgRows+extraRows;
        colSize = numImgCols+extraCols;
    }

    ~Morphology(){
        delete[] zeroFramedAry;
        delete[] morphAry;
        delete[] tempAry;
        delete[] structAry;
        for(int i=0; i<numImgRows; i++){
            delete[] zeroFramedAry[i];
            delete[] morphAry[i];
            delete[] tempAry[i];
            delete[] structAry[i];
        }
    }

    int **zero2DAry(int **ary, int nRows, int nCols){
        for(int i=0; i<nRows; i++){
            for(int j=0; j<nCols; j++){
                ary[i][j] = 0;
            }
        }
        return ary;
    }

    void loadImg(){
        //load imgFile to zeroFramedAry inside of frame,
        //begins at (rowOrigin, colOrigin)
    }

    void loadStruct(){
        //load structFile to structAry
    }

    void computeDilation(int **inAry, int **outAry){
        for(int i=rowFrameSize; i<rowSize; i++){
            for(int j=colFrameSize; j<colSize; j++){
                if(inAry[i][j] > 0){
                    //onePixelDilation(i, j, inAry, outAry)
                    //^ only processing one pixel inAry[i][j]
                }
            }
        }
    }

    void computeErosion(int **inAry, int **outAry){
        for(int i=rowFrameSize; i<rowSize; i++){
            for(int j=colFrameSize; j<colSize; j++){
                if(inAry[i][j] > 0){
                    //onePixelErosion(i, j, inAry,outAry)
                }
            }
        }
    }

    int **onePixelErosion(int i, int j, int **inAry, int **outAry){
        int iOffset = i-rowOrigin;
        int jOffset = j-colOrigin;
        bool matchFlag = true; 
        int rIndex=0, cIndex=0;
        while((matchFlag==true)&&(rIndex<numStructRows)){
            while((matchFlag==true)&&(cIndex<numStructCols)){
                if((structAry[rIndex][cIndex]>0) && (inAry[iOffset+rIndex][jOffset+cIndex]<=0)){
                    matchFlag=false;
                }
                cIndex++;
            }
            rIndex++;
        }

        if(matchFlag==true){
            outAry[i][j] = 1;
            return outAry;
        }
        outAry[i][j]=0;
        return outAry;
    }
};

int main(int argc, char *argv[]){

    ifstream input1;
    input1.open(argv[1]);

    ifstream input2;
    input2.open(argv[2]);


    return 0;
}