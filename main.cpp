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
    
    Morphology(int imgRows, int imgCols, int min, int max, int structRows, int structCols, int rowO, int colO){
        numImgRows = imgRows;
        numImgCols = imgCols;
        imgMin = min;
        imgMax = max;
        numStructRows = structRows;
        numStructCols = structCols;
        rowOrigin = rowO;
        colOrigin = colO;
        rowFrameSize = numStructRows/2;
        colFrameSize = numStructCols/2;
        extraRows = rowFrameSize*2;
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
        cout << "Deleted all arrays..." << endl;
    }

    void allocateArrs(){
        zeroFramedAry = new int*[rowSize];
        morphAry = new int*[rowSize];
        tempAry = new int*[rowSize];
        structAry = new int*[numStructRows];
        for(int i=0; i<rowSize; i++){
            zeroFramedAry[i] = new int[colSize]();
            morphAry[i] = new int[colSize]();
            tempAry[i] = new int[colSize]();
        }
        for(int i=0; i<numStructRows; i++){
            structAry[i] = new int[numStructCols]();
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

    void loadImg(ifstream& img){
        //load imgFile to zeroFramedAry inside of frame,
        //begins at (rowOrigin, colOrigin)
        int rowstart=rowFrameSize;
        int colstart=colFrameSize;

        if(img.is_open()){
            for(int i=rowstart; i<(rowSize-rowstart); i++){
                for(int j=colstart; j<(colSize-colstart); j++){
                    img >> zeroFramedAry[i][j];
                }
            }
        }
    }

    void loadStruct(ifstream& in){
        //load structFile to structAry
        for(int i=0; i<numStructRows; i++){
            for(int j=0; j<numStructCols; j++){
                in >> structAry[i][j];
            }
        }
    }

    void computeDilation(int **inAry, int **outAry){
        for(int i=rowFrameSize; i<rowSize; i++){
            for(int j=colFrameSize; j<colSize; j++){
                if(inAry[i][j] > 0){
                    onePixelDilation(i, j, inAry, outAry);
                }
            }
        }
    }

    void computeErosion(int **inAry, int **outAry){
        for(int i=rowFrameSize; i<rowSize; i++){
            for(int j=colFrameSize; j<colSize; j++){
                if(inAry[i][j] > 0){
                    onePixelErosion(i, j, inAry,outAry);
                }
            }
        }
    }

    void onePixelErosion(int i, int j, int **inAry, int **outAry){
        int iOffset = i-rowOrigin;
        int jOffset = j-colOrigin;
        bool matchFlag = true; 
        int rIndex=0, cIndex=0;
        while((matchFlag==true)&&(rIndex<numStructRows)){
            while((matchFlag==true)&&(cIndex<numStructCols)){
                if((structAry[rIndex][cIndex]>0) && (inAry[iOffset+rIndex][jOffset+cIndex])<=0){
                    matchFlag=false;
                }
                cIndex++;
            }
            rIndex++;
        }

        if(matchFlag==true){
            outAry[i][j] = 1;
        }else{
            outAry[i][j]=0;
        }
    }

    void onePixelDilation(int i, int j, int **inAry, int **outAry){
        int iOffset = i-rowOrigin;
        int jOffset = j-colOrigin;
        int rIndex = 0, cIndex = 0;
        while(rIndex < numStructRows){
            while(cIndex < numStructCols){
                if(structAry[rIndex][cIndex] > 0){
                    outAry[iOffset + rIndex][jOffset + cIndex] = 1;
                }
                cIndex++;
            }
            rIndex++;
        }
    }

    void computeClosing(int **zfa, int **ma, int **ta){
        computeDilation(zfa, ta);
        computeErosion(ta, ma);
    }

    void computeOpening(int **zfa, int **ma, int **ta){
        computeErosion(zfa, ta);
        computeDilation(ta, ma);
    }

    void prettyPrint(int **ary, ofstream& out){
        for(int i=rowFrameSize; i<(rowSize-rowFrameSize); i++){
            for(int j=colFrameSize; j<(colSize-colFrameSize); j++){
                if(ary[i][j] == 0){
                    out << ". ";
                }else{
                    out << "1 ";
                }
            }
            out << endl;
        }
    }

    void prettyPrintStruct(int **ary, ofstream& out){
        for(int i=0; i<numStructRows; i++){
            for(int j=0; j<numStructCols; j++){
                if(ary[i][j]==0){
                    out << ". ";
                }else{
                    out << "1 ";
                }
            }
            out << endl;
        }
    }

    void aryToFile(int **ary, ofstream& outFile){
        outFile << numImgRows << " ";
        outFile << numImgCols << " ";
        outFile << imgMin << " ";
        outFile << imgMax << " ";
        outFile << endl;
        int rowstart=rowFrameSize;
        int colstart=colFrameSize;
        for(int i=rowstart; i<(rowSize-rowstart); i++){
            for(int j=colstart; j<(colSize-colstart); j++){
                outFile << ary[i][j] << " ";
            }
            outFile << endl;
        }
    }

    void addBorder(ofstream& out){
        out << endl;
        for(int i=0; i<colSize; i++){
            out << "--";
        }
        out << endl;
    }
};

int main(int argc, char *argv[]){

    if(argc != 8){
        cout << "Invalid arguments, ending program..." << endl;
        return 1;
    }

    //Step 0
    ifstream imgFile;
    imgFile.open(argv[1]);

    ifstream structFile;
    structFile.open(argv[2]);

    ofstream dilateOutFile;
    dilateOutFile.open(argv[3]);

    ofstream erodeOutFile;
    erodeOutFile.open(argv[4]);

    ofstream closingOutFile;
    closingOutFile.open(argv[5]);

    ofstream openingOutFile;
    openingOutFile.open(argv[6]);

    ofstream prettyPrintFile;
    prettyPrintFile.open(argv[7]);

    //Step 1
    int imgRows, imgCols, imgMin, imgMax;
    if(imgFile.is_open()){
        imgFile >> imgRows;
        imgFile >> imgCols;
        imgFile >> imgMin;
        imgFile >> imgMax;
    }

    int strctRows, strctCols, strctMin, strctMax, rowOrigin, colOrigin;
    if(structFile.is_open()){
        structFile >> strctRows;
        structFile >> strctCols;
        structFile >> strctMin;
        structFile >> strctMax;
        structFile >> rowOrigin;
        structFile >> colOrigin;
    }

    //Step 2
    Morphology morph = Morphology(imgRows, imgCols, imgMin, imgMax, strctRows, strctCols, rowOrigin, colOrigin);
    morph.allocateArrs();

    //Step 3
    morph.zero2DAry(morph.zeroFramedAry, morph.rowSize, morph.colSize);

    //Step 4
    morph.loadImg(imgFile);
    prettyPrintFile << "Image File:" << endl;
    morph.prettyPrint(morph.zeroFramedAry, prettyPrintFile);
    morph.addBorder(prettyPrintFile);

    //setp 5
    morph.zero2DAry(morph.structAry, morph.numStructRows, morph.numStructCols);
    morph.loadStruct(structFile);
    prettyPrintFile << "Structure Element:" << endl;
    morph.prettyPrintStruct(morph.structAry, prettyPrintFile);
    morph.addBorder(prettyPrintFile);

    //step 6
    morph.zero2DAry(morph.morphAry, morph.rowSize, morph.colSize);
    morph.computeDilation(morph.zeroFramedAry, morph.morphAry);
    morph.aryToFile(morph.morphAry, dilateOutFile);
    prettyPrintFile << "Array after Dilation:" << endl;
    morph.prettyPrint(morph.morphAry, prettyPrintFile);
    morph.addBorder(prettyPrintFile);

    //step 7
    morph.zero2DAry(morph.morphAry, morph.rowSize, morph.colSize);
    morph.computeErosion(morph.zeroFramedAry, morph.morphAry);
    morph.aryToFile(morph.morphAry, erodeOutFile);
    prettyPrintFile << "Array after Erosion:" << endl;
    morph.prettyPrint(morph.morphAry, prettyPrintFile);
    morph.addBorder(prettyPrintFile);

    //step 8
    morph.zero2DAry(morph.morphAry, morph.rowSize, morph.colSize);
    morph.computeOpening(morph.zeroFramedAry, morph.morphAry, morph.tempAry);
    morph.aryToFile(morph.morphAry, openingOutFile);
    prettyPrintFile << "Opening:" << endl;
    morph.prettyPrint(morph.morphAry, prettyPrintFile);
    morph.addBorder(prettyPrintFile);

    //step 9
    morph.zero2DAry(morph.morphAry, morph.rowSize, morph.colSize);
    morph.computeClosing(morph.zeroFramedAry, morph.morphAry, morph.tempAry);
    morph.aryToFile(morph.morphAry, closingOutFile);
    prettyPrintFile << "Closing:" << endl;
    morph.prettyPrint(morph.morphAry, prettyPrintFile);
    morph.addBorder(prettyPrintFile);

    //step 10
    imgFile.close();
    structFile.close();
    dilateOutFile.close();
    erodeOutFile.close();
    openingOutFile.close();
    closingOutFile.close();
    prettyPrintFile.close();

    return 0;
}