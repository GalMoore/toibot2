
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <map>
#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <iostream>
#include <dirent.h>
#include <stdio.h>
#include <string>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>


using namespace cv;
using namespace std;


class FoldersManagment{

public:
   
    FoldersManagment() {

    }

    ~FoldersManagment() {

    }

public:
   
    bool checkIfFolderIsEmpty(string path) const;

    void createNewPoolFolder(int folderNumber, string path) const;

    void putMapInRelevantFolder(Mat thirdEditPool,string folderNumber, string path) const;

    int  findNumberOfFilesInDirectory(std::string path) const;

    bool fileExistenceCheck(const std::string& name) const;

    int  findTheNumerOfImagesInFolder(string folderNumber, string path) const;

    std::vector<string> getAllNamesOfFoldersInDir(string path) const;

    std::vector<string> explode(const string& s, const char& c) const;

    void createFolderWithName(string folderName, string path) const;

};


