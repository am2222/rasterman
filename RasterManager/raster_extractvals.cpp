#define MY_DLL_EXPORT

#include "gdal_priv.h"
#include "raster.h"
#include "rastermanager_interface.h"
#include "rastermanager_exception.h"
#include "gdalgrid.h"
#include "rastermanager.h"

#include <math.h>
#include <QFile>
#include <QStringList>

namespace RasterManager {

// Default nodata value (note it is a string)
QString EXTRACT_NO_DATA = "-9999";

int Raster::ExtractPoints(const char * sCSVInputSourcePath,
                          const char * sRasterInputSourcePath,
                          const char * sCSVOutputPath,
                          QString sXField,
                          QString sYField,
                          QString sNoData){

    // "" is a special case here. We can opt not to have a header line
    bool bHasXYField = true;
    if (sXField.compare("") == 0 ||
            sYField.compare("") == 0 )
        bHasXYField = false;

    if (sNoData.compare("") == 0)
        sNoData = EXTRACT_NO_DATA;

    // Validate that the files are there
    CheckFile(sCSVInputSourcePath, true);
    CheckFile(sRasterInputSourcePath, true);
    CheckFile(sCSVOutputPath, false);

    int xcol = -1;
    int ycol = -1;

    if (!bHasXYField){
        xcol = 0;
        ycol = 1;
    }

    // Set up our line buffer
    int nlinenumber = 0;

    // Open the input Raster
    // --------------------------------------------------
    RasterMeta rmRasterMeta(sRasterInputSourcePath);

    GDALDataset * pDS = (GDALDataset*) GDALOpen(sRasterInputSourcePath, GA_ReadOnly);
    if (pDS == NULL)
        throw RasterManagerException(INPUT_FILE_ERROR, "Couldn't open input raster file.");

    GDALRasterBand * pRBInput = pDS->GetRasterBand(1);

    // Our buffer only has to be one pixel
    double pInputPt;

    bool bHeaderRow = true;

    // Open the output file for writing
    QFile CSVOutputfile(sCSVOutputPath);
    if ( CSVOutputfile.open(QFile::WriteOnly|QFile::Append) )
    {
        // Write the header line output CSV
        // --------------------------------------------------
        if (bHasXYField){
            CSVWriteLine(&CSVOutputfile, QString("%1,%2,Value")
                         .arg(sXField)
                         .arg(sYField) );
        }
        else {
            CSVWriteLine(&CSVOutputfile, QString("X,Y,Value") );
        }


        // Iterate over lines in the CSV
        // --------------------------------------------------
        QFile file(sCSVInputSourcePath);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        {

            // Line loop
            LoopTimer rowLoop("Row");
            LoopTimer colLoop("Column");

            while (!file.atEnd())
            {
                nlinenumber++;
                QString csvline = file.readLine();
                QStringList lstLine = csvline.split(",");

                int csvX = -1;
                int csvY = -1;

                double dCSVX = -1;
                double dCSVY = -1;

                // Figure out if line one is a header line
                if (nlinenumber == 1){
                    for (int ncolnumber = 0; ncolnumber < lstLine.size(); ncolnumber++){
                        bool isNumeric = false;
                        QString sCell = lstLine.at(ncolnumber);
                        sCell.toDouble(&isNumeric);
                        if (isNumeric){
                            bHeaderRow = false;
                            break;
                        }
                    }
                }
                else {
                    bHeaderRow = false;
                }

                // Loop over cells
                for (int ncolnumber = 0; ncolnumber < lstLine.size(); ncolnumber++){

                    QString csvItem = lstLine.at(ncolnumber);
                    CSVCellClean(csvItem);

                    // For the first line read values if they apply
                    if ( bHeaderRow && nlinenumber == 1 ){
                        if (csvItem.compare(sXField) == 0){
                            xcol = ncolnumber;
                        }
                        else if (csvItem.compare(sYField) == 0){
                            ycol = ncolnumber;
                        };
                    }

                    // If it's not a header, just process it.
                    if (!bHeaderRow){
                        // Basic checking to make sure we have parameters
                        if (xcol == -1){
                            QString sErr = QString("X Field '%1' not found").arg(sXField);
                            throw RasterManagerException(MISSING_ARGUMENT, sErr);
                        }
                        else if (ycol == -1){
                            QString sErr = QString("Y Column '%1' not found").arg(sYField);
                            throw RasterManagerException(MISSING_ARGUMENT, sErr);
                        }

                        double dVal = csvItem.toDouble();
                        if (xcol == ncolnumber){
                            dCSVX = dVal;
                            csvX = (int) floor((dVal - rmRasterMeta.GetLeft() ) / rmRasterMeta.GetCellWidth());
                        }
                        else if (ycol == ncolnumber){
                            dCSVY = dVal;
                            csvY = (int) floor((rmRasterMeta.GetTop() - dVal) / rmRasterMeta.GetCellHeight() * -1);
                        }

                    }
                    colLoop.Tick(); //DEBUG Only
                }
                if (!bHeaderRow){
                    // here's where we need to get the correct row of the output. Replace
                    pRBInput->RasterIO(GF_Read, csvX,  csvY, 1, 1, &pInputPt, 1, 1, GDT_Float64, 0, 0);
                    QString value = sNoData;

                    if (pInputPt != rmRasterMeta.GetNoDataValue() )
                        value = QString("%1").arg(pInputPt, 0, 'f', 10, '0');

                    CSVWriteLine( &CSVOutputfile, QString("%1,%2,%3")
                                  .arg(dCSVX, 0, 'f', rmRasterMeta.GetHorizontalPrecision() )
                                  .arg(dCSVY, 0, 'f', rmRasterMeta.GetHorizontalPrecision() )
                                  .arg(value) );
                }
                rowLoop.Tick();
            }
            file.close();

            rowLoop.Output();
            colLoop.Output();
        }
        else{
            throw RasterManagerException(INPUT_FILE_ERROR, "Couldn't open input csv file.");
        }
    }
    else{
        throw RasterManagerException(OUTPUT_FILE_ERROR, "Couldn't open output csv file.");
    }
    GDALClose(pDS);

    return PROCESS_OK;
}

}
