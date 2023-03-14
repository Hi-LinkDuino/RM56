/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef AW_CXX_DISTRIBUTED_UTILS_CSV_TRANSFORM_XML_H_
#define AW_CXX_DISTRIBUTED_UTILS_CSV_TRANSFORM_XML_H_

#include <iostream>
#include <vector>
#include <string>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <stdlib.h>

#define CSV_FILE_PATH "/data/test/"
#define XML_FILE_PATH "/data/test/"

class CsvTransformXml {
    std::vector<std::string> csvFiles_;
    std::vector<std::string> vecLines_;
    std::string csvFileName_ = "";
    std::string xmlFileName_ = "";
    std::string fileName_ = "";

public:

    explicit CsvTransformXml(std::string targetFile)
    {
        SetFileName(targetFile);
        SetCvsFileName();
        SetXmlFileName();
        FileOperate();
    }

    void FileOperate()
    {
        std::fstream csvIn = std::fstream(csvFileName_.c_str(), std::ios::in);
        std::fstream xmlOut = std::fstream(xmlFileName_.c_str(), std::ios::out);
        if (!xmlOut) {
            csvIn.close();
            xmlOut.close();
            std::cout << "file created fail!" << std::endl;
            return;
        }
        if (!csvIn) {
            csvIn.close();
            xmlOut.close();
            std::cout << "file not exit!" << std::endl;
            return;
        }
        std::string strLine;
        getline(csvIn, strLine);
        while (csvIn >> strLine) {
            SplitString(strLine, vecLines_, ",");
        }
        int testCaseSum = vecLines_.size() / 3; // one item case_result from csv includes 3 strings
        int failCaseSum = 0;
        for (std::string s : vecLines_) {
            if (!s.compare("FAILED")) {
                failCaseSum++;
            }
        }
        xmlOut << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
        xmlOut << "<testsuites tests=\""<< testCaseSum <<"\" failures=\""<< failCaseSum
               << "\" disabled=\"0\" errors=\"\" timestamp=\"\" time=\"\" name=\"AllTests\">"<<std::endl;
        xmlOut << "  <testsuite name=\""<< fileName_ << "\" tests=\"" << testCaseSum
               << "\" failures=\""<< failCaseSum << "\" disabled=\"0\" errors=\"0\" time=\"192.553\">" << std::endl;
        unsigned long i = 0;
        while (i < vecLines_.size()) {
            if (!(vecLines_.at(i + 2).compare("FAILED"))) {  // the result of every case intervals 2 string
                xmlOut << "    <testcase name=\""<< vecLines_.at(i)
                       <<"\" status=\"run\" time=\"\" classname=\"" << fileName_ << "\" level=\"3\">" << std::endl;
                xmlOut << "      <failure message=\"NULL\"></failure>"<< std::endl;
                xmlOut << "    </testcase>" << std::endl;
            } else {
                xmlOut <<"    <testcase name=\""<< vecLines_.at(i) <<"\" status=\"run\" time=\"0\" classname=\""
                       << fileName_ << "\" level=\"0\" />" << std::endl;
            }
            i = i + 3;  // one item case_result from csv includes 3 strings
        }
        xmlOut << "  </testsuite>" << std::endl;
        xmlOut << "</testsuites>" << std::endl;
        csvIn.close();
        xmlOut.close();
    }

    void SetFileName(std::string targetFile)
    {
        fileName_ = targetFile;
    }

    void SetCvsFileName()
    {
        getAllFiles(CSV_FILE_PATH);
        csvFileName_ = csvFiles_.at(0);
        std::cout << csvFileName_ << std::endl;
    }

    void SetXmlFileName()
    {
        std::string string1 = XML_FILE_PATH;
        string1 += fileName_ + ".xml";
        xmlFileName_ = string1;
    }

    void SplitString(const std::string& s, std::vector<std::string>& v, const std::string& c)
    {
        std::string::size_type pos1 = 0;
        std::string::size_type pos2 = 0;
        pos2 = s.find(c);
        pos1 = 0;
        while (std::string::npos != pos2) {
            v.push_back(s.substr(pos1, pos2 - pos1));
            pos1 = pos2 + c.size();
            pos2 = s.find(c, pos1);
        }
        if (pos1 != s.length()) {
            v.push_back(s.substr(pos1));
        }
    }

    void getAllFiles(std::string path)
    {
        std::string p;
        csvFiles_.push_back(p.assign(path).append("hulutest_result.csv"));
    }
};

#endif // AW_CXX_DISTRIBUTED_UTILS_CSV_TRANSFORM_XML_H_

