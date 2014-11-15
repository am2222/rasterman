#ifndef RASTERMANENGINE_H
#define RASTERMANENGINE_H

#include <QString>

namespace RasterManager {

class RasterManEngine
{
public:
    RasterManEngine(int argc, char * argv[]);


private:
    /**
     * @brief RasterProperties
     * @param argc
     * @param argv
     */
    void RasterProperties(int argc, char *argv[]);
    /**
     * @brief RasterCopy
     * @param argc
     * @param argv
     */
    void RasterCopy(int argc, char * argv[]);
    /**
     * @brief RasterAdd
     * @param argc
     * @param argv
     */
    void RasterAdd(int argc, char * argv[]);
    /**
     * @brief RasterSubtract
     * @param argc
     * @param argv
     */
    void RasterSubtract(int argc, char * argv[]);
    /**
     * @brief RasterDivide
     * @param argc
     * @param argv
     */
    void RasterDivide(int argc, char * argv[]);
    /**
     * @brief RasterMultiply
     * @param argc
     * @param argv
     */
    void RasterMultiply(int argc, char * argv[]);
    /**
     * @brief RasterPower
     * @param argc
     * @param argv
     */
    void RasterPower(int argc, char * argv[]);
    /**
     * @brief RasterSqrt
     * @param argc
     * @param argv
     */
    void RasterSqrt(int argc, char * argv[]);
    /**
     * @brief Mosaic
     * @param argc
     * @param argv
     */
    void Mosaic(int argc, char *argv[]);
    /**
     * @brief DoDThresholdPropError
     * @param argc
     * @param argv
     */

    void BiLinearResample(int argc, char * argv[]);

    /**
     * @brief Slope
     * @param argc
     * @param argv
     */
    void Slope(int argc, char * argv[]);
    /**
     * @brief Hillshade
     * @param argc
     * @param argv
     */
    void Hillshade(int argc, char *argv[]);

    /**
     * @brief CSVToRaster
     * @param argc
     * @param argv
     */
    void CSVToRaster(int argc, char * argv[]);

    /**
     * @brief Mask
     * @param argc
     * @param argv
     */
    void Mask(int argc, char *argv[]);

    /**
     * @brief CheckFile
     * @param sFilename
     * @param bMustExist
     */
    void CheckFile(const char *sFilename, bool bMustExist);
    /**
     * @brief CheckFile
     * @param argc
     * @param argv
     * @param nIndex
     * @param bMustExist
     */
    void CheckFile(int argc, char *argv[], int nIndex, bool bMustExist);

    /**
     * @brief GetInteger
     * @param argc
     * @param argv
     * @param nIndex
     * @return
     */
    int GetInteger(int argc, char * argv[], int nIndex);
    /**
     * @brief GetDouble
     * @param argc
     * @param argv
     * @param nIndex
     * @return
     */
    double GetDouble(int argc, char * argv[], int nIndex);
    /**
     * @brief GetOutputRasterProperties
     * @param fLeft
     * @param fTop
     * @param nRows
     * @param nCols
     * @param fCellSize
     * @param argc
     * @param argv
     */
    void GetOutputRasterProperties(double & fLeft, double & fTop, int & nRows, int & nCols, double & fCellSize, int argc, char * argv[], int nStartArg);
    /**
     * @brief MakeConcurrent
     * @param argc
     * @param argv
     */
    void MakeConcurrent(int argc, char *argv[]);
    /**
     * @brief isNumeric
     * @param pszInput
     * @param nNumberBase
     * @return
     */
    bool isNumeric(const char *pszInput, int nNumberBase);



};

}

#endif // RASTERMANENGINE_H
