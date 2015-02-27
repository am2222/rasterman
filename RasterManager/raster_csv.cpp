#define MY_DLL_EXPORT

#include "gdal_priv.h"
#include "raster.h"
#include "rastermanager_interface.h"
#include "rastermanager_exception.h"
#include "gdalgrid.h"
#include "rastermanager.h"

#include <limits>
#include <math.h>
#include <QFile>
#include <QStringList>
#include <QTextStream>
#include <QFileInfo>

namespace RasterManager {

int Raster::CSVtoRaster(const char * sCSVSourcePath,
                         const char * sOutput,
                         double dTop,
                         double dLeft,
                         int nRows,
                         int nCols,
                         double dCellWidth,
                         double dNoDataVal,
                         const char * sXField,
                         const char * sYField,
                         const char * sDataField){

    double dCellHeight = dCellWidth * -1;
    const char * psDriver = GetDriverFromFileName(sOutput);
    GDALDataType nDType = GDT_Float32;
    RasterMeta inputRasterMeta(dTop, dLeft, nRows, nCols, &dCellHeight, &dCellWidth,
                               &dNoDataVal, psDriver, &nDType, NULL);

    CSVtoRaster(sCSVSourcePath, sOutput, sXField, sYField, sDataField, &inputRasterMeta);

    return PROCESS_OK;
}

int Raster::CSVtoRaster(const char * sCSVSourcePath,
                         const char * psOutput,
                         const char * sRasterTemplate,
                         const char * sXField,
                         const char * sYField,
                         const char * sDataField ){

    RasterMeta inputRasterMeta(sRasterTemplate);

    int eResult = CSVtoRaster(sCSVSourcePath, psOutput, sXField, sYField, sDataField, &inputRasterMeta);

    return eResult;

}

int Raster::CSVtoRaster(const char * sCSVSourcePath,
                         const char * psOutput,
                         const char * sXField,
                         const char * sYField,
                         const char * sDataField,
                         RasterMeta * p_rastermeta){





    //DELETE ME: FOR DEBUG ONLY
    if (QFileInfo(psOutput).exists()){
        QFile::remove(QFileInfo(psOutput).absoluteFilePath());
    }

    // Validate that the files are there
    CheckFile(sCSVSourcePath, true);
    CheckFile(psOutput, false);

    // Create the output dataset for writing
    GDALDataset * pDSOutput = CreateOutputDS(psOutput, p_rastermeta);

    int xcol=-1, ycol=-1, zcol=-1;

    // Read CSV file into 3 different arrays
    int nlinenumber = 0;
    QFile file(sCSVSourcePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        while (!file.atEnd())
        {
            nlinenumber++;
            QString csvline = file.readLine();
            QStringList lstLine = csvline.split(",");

            // prepare our buffers
            int csvX = -1;
            int csvY = -1;
            double csvDataVal = p_rastermeta->GetNoDataValue();

            for (int ncolnumber = 0; ncolnumber < lstLine.size(); ncolnumber++){

                QString csvItem = lstLine.at(ncolnumber);
                CSVCellClean(csvItem);

                //Strip the quotes off the
                // First line is the header
                if (nlinenumber == 1){
                    if (csvItem.compare(sXField) == 0){
                        xcol = ncolnumber;
                    }
                    else if (csvItem.compare(sYField) == 0){
                        ycol = ncolnumber;
                    }
                    else if (csvItem.compare(sDataField) == 0){
                        zcol = ncolnumber;
                    }
                }
                // Not the first line read values if they apply
                else{
                    // Basic checking to make sure we have parameters
                    if (xcol == -1){
                        QString sErr = QString("X Field '%1' not found").arg(sXField);
                        throw RasterManagerException(MISSING_ARGUMENT, sErr);
                    }
                    else if (ycol == -1){
                        QString sErr = QString("Y Column '%1' not found").arg(sYField);
                        throw RasterManagerException(MISSING_ARGUMENT, sErr);
                    }
                    else if (zcol == -1){
                        QString sErr = QString("Data Column '%1' not found").arg(sDataField);
                        throw RasterManagerException(MISSING_ARGUMENT, sErr);
                    }

                    // Assign our CSV values to an appropriate place in the raster
                    double dVal = csvItem.toDouble();
                    if (xcol == ncolnumber){
                        csvX = (int) floor((dVal - p_rastermeta->GetLeft() ) / p_rastermeta->GetCellWidth());
                    }
                    else if (ycol == ncolnumber){
                        csvY = (int) floor((p_rastermeta->GetTop() - dVal) / p_rastermeta->GetCellHeight() * -1);
                    }
                    else if (zcol == ncolnumber){
                        csvDataVal = dVal;
                    }


                }

            }
            // here's where we need to get the correct row of the output. Replace
            if (csvX >= 0 && csvX < p_rastermeta->GetCols()
                    && csvY >=0 && csvY < p_rastermeta->GetRows() ){
                pDSOutput->GetRasterBand(1)->RasterIO(GF_Write, csvX,  csvY, 1, 1, &csvDataVal, 1, 1, GDT_Float64, 0, 0);
            }

        }
        file.close();
    }
    else{
        throw RasterManagerException(INPUT_FILE_ERROR, "Couldn't open input csv file.");
    }

    CalculateStats(pDSOutput->GetRasterBand(1));
    GDALClose(pDSOutput);

    PrintRasterProperties(psOutput);

    return PROCESS_OK;

}

void Raster::CSVWriteLine(QString sCSVFullPath, QString sCSVLine){

    QFile csvFile(sCSVFullPath);

    if (csvFile.open(QFile::WriteOnly|QFile::Append))
    {
      QTextStream stream(&csvFile);

      stream << sCSVLine << "\n"; // this writes first line with two columns
      csvFile.close();
    }
    return;

}

void Raster::CSVCellClean(QString & value){

    // Strip whitespace outside the quotes
    value = value.simplified();

    // Then strip double quotes
    if (value.startsWith("\"") && value.endsWith("\"")){
        value=value.mid(1, value.length() -2);
    }

    // Now strip white space again instide the quotes
    value = value.simplified();
}

}
