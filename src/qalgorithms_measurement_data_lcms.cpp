// qalgorithms_measurement_data_lcms.cpp

// internal
#include "../include/qalgorithms_measurement_data_lcms.h"

// external
#include <fstream>
#include <sstream>

namespace q
{
    // usings
    using VariableType = DataType::MassSpectrum::variableType;
    using DataField = DataType::DataField;
    using MassSpectrum = DataType::MassSpectrum;
    using DataPoint = DataType::MassSpectrum::DataPoint;
    using DataPointVector = std::vector<std::unique_ptr<DataPoint>>;

    LCMSData::LCMSData()
    {
    }

    LCMSData::~LCMSData()
    {
    }

    void LCMSData::readCSV(std::string filename, int rowStart, int rowEnd, int colStart, int colEnd, char separator, std::vector<DataField> variableTypes)
    {
        // open the file
        std::ifstream file(filename);
        std::string line;
        std::vector<std::vector<std::string>> raw_data;

        // if rowEnd is -1, then set it to the maximum number of rows
        if (rowEnd == -1)
        {
            rowEnd = 1000000;
        }

        // if colEnd is -1, then set it to the maximum number of columns
        if (colEnd == -1)
        {
            colEnd = 1000000;
        }

        // find the DataFiled::SCANNUMBER as this is the key for the LCMSData map
        int scanNumberIndex = -1;
        for (int i = 0; i < variableTypes.size(); i++)
        {
            if (variableTypes[i] == DataField::SCANNUMBER)
            {
                scanNumberIndex = i;
                break;
            }
        }
        // check if scanNumberIndex is found
        if (scanNumberIndex == -1)
        {
            std::cerr << "The scan number is not found in the variable types" << std::endl;
            return;
        }

        // find the DataFiled::RETENTIONTIME
        int retentionTimeIndex = -1;
        for (int i = 0; i < variableTypes.size(); i++)
        {
            if (variableTypes[i] == DataField::RETENTIONTIME)
            {
                retentionTimeIndex = i;
                break;
            }
        }
        // check if retentionTimeIndex is found
        if (retentionTimeIndex == -1)
        {
            std::cerr << "The retention time is not found in the variable types" << std::endl;
            return;
        }

        // find the DataFiled::MZ
        int mzIndex = -1;
        for (int i = 0; i < variableTypes.size(); i++)
        {
            if (variableTypes[i] == DataField::MZ)
            {
                mzIndex = i;
                break;
            }
        }
        // check if mzIndex is found
        if (mzIndex == -1)
        {
            std::cerr << "The mz is not found in the variable types" << std::endl;
            return;
        }

        // find the DataFiled::INTENSITY
        int intensityIndex = -1;
        for (int i = 0; i < variableTypes.size(); i++)
        {
            if (variableTypes[i] == DataField::INTENSITY)
            {
                intensityIndex = i;
                break;
            }
        }
        // check if intensityIndex is found
        if (intensityIndex == -1)
        {
            std::cerr << "The intensity is not found in the variable types" << std::endl;
            return;
        }

        // read the file
        int rowCounter = 0;
        int colCounter = 0;
        while (std::getline(file, line) && rowCounter < rowEnd)
        {
            if (rowCounter >= rowStart)
            {
                std::vector<std::string> row;
                std::stringstream lineStream(line);
                std::string cell;
                colCounter = 0;
                while (std::getline(lineStream, cell, separator))
                {
                    if (colCounter >= colStart && colCounter < colEnd)
                    {
                        row.push_back(cell);
                    }
                    colCounter++;
                }
                raw_data.push_back(row);
            }
            rowCounter++;
        }
        file.close();

        // check if the raw data has same number of columns as the variableTypes
        if (raw_data[0].size() != variableTypes.size())
        {
            std::cerr << "The number of columns in the raw data does not match the number of variable types" << std::endl;
            return;
        }

        // transfere the raw data to the data map
        int data_id = 0; // data id is used to identify the data set
        maxKey = 0;
        for (int i = 0; i < raw_data.size(); i++)
        {
            // check if the scan number is already in the data map
            int scanNumber = std::stoi(raw_data[i][scanNumberIndex]);
            if (this->data.find(scanNumber) == this->data.end())
            { // scan number is not found in the data map; initialize a new MassSpectrum object and add meta information
                // add the MassSpectrum object to the data map
                this->data[scanNumber] = std::make_unique<MassSpectrum>();
                // add the scan number to the MassSpectrum object
                (*this->data[scanNumber]->metaData)[DataField::SCANNUMBER] = VariableType(scanNumber);
                // add the retention time to the MassSpectrum object
                (*this->data[scanNumber]->metaData)[DataField::RETENTIONTIME] = VariableType(std::stod(raw_data[i][retentionTimeIndex]));
                // update the maxKey
                if (scanNumber > maxKey)
                {
                    maxKey = scanNumber;
                }
            }
            // create a new DataPoint object and add it to the DataPoint Vector
            double intensity = std::stod(raw_data[i][intensityIndex]);
            int df = (intensity > 0) ? 1 : 0;
            this->data[scanNumber]->dataPoints.push_back(std::make_unique<DataPoint>(intensity, std::stod(raw_data[i][mzIndex]), df));
        }
    }

    void LCMSData::writeCSV(std::string filename)
    {
        // open the file
        std::ofstream file(filename);
        // write header using: ID, ScanNumber, RetentionTime, MZ, Intensity (ID is the key for the data map)
        file << "ID,ScanNumber,RetentionTime,MZ,Intensity" << std::endl;
        // iterate over all data sets
        for (auto it = this->data.begin(); it != this->data.end(); it++)
        {
            // iterate over all data points
            for (const auto& dp : it->second->dataPoints)
            {
                file << it->first << "," << std::get<int>((*it->second->metaData)[DataField::SCANNUMBER]) << "," << std::get<double>((*it->second->metaData)[DataField::RETENTIONTIME]) << "," << dp->mz << "," << dp->intensity << std::endl;
            }
        }
        //close the file
        file.close();
    }

    void LCMSData::zeroFilling()
    {
        // create a varDataType Object, which is a pointer to a map of mass spectra
        varDataType dataObject = &(this->data);
        this->MeasurementData::zeroFilling(dataObject, 8);
    }

    void LCMSData::cutData()
    {
        // create a varDataType Object, which is a pointer to a map of mass spectra
        varDataType dataObject = &(this->data);
        this->MeasurementData::cutData(dataObject, maxKey);
    }

    void LCMSData::filterSmallDataSets()
    {
        // create a varDataType Object, which is a pointer to a map of mass spectra
        varDataType dataObject = &(this->data);
        this->MeasurementData::filterSmallDataSets(dataObject);
    }

    void LCMSData::interpolateData()
    {
        // create a varDataType Object, which is a pointer to a map of mass spectra
        varDataType dataObject = &(this->data);
        this->MeasurementData::interpolateData(dataObject);
    }

    void LCMSData::print()
    {
        for (auto it = this->data.begin(); it != this->data.end(); it++)
        {
            std::cout << "Scan Number: " << it->first << std::endl;
            std::cout << "Retention Time: " << std::get<double>((*it->second->metaData)[DataField::RETENTIONTIME]) << std::endl;
            std::cout << "Number of Data Points: " << it->second->dataPoints.size() << std::endl;
            std::cout << "MZ values: ";
            for (const auto& dp : it->second->dataPoints)
            {
                std::cout << dp->mz << " ";
            }
            std::cout << std::endl;

            std::cout << "Intensity values: ";
            for (const auto& dp : it->second->dataPoints)
            {
                std::cout << dp->intensity << " ";
            }
            std::cout << std::endl;
            std::cout << "cuttingPoints: ";
            for (const auto& cp : it->second->cuttingPoints)
            {
                std::cout << *cp << " ";
            }
            std::cout << std::endl;
        }
    }

    void LCMSData::info()
    {
        std::cout << "Number of data sets: " << this->data.size() << std::endl;
    }

} // namespace q