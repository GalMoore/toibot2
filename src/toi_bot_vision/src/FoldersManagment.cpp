
#include "/home/gal/toibot_ws/src/ToiBot1/src/toi_bot_vision/include/toi_bot_vision/FoldersManagment.h"


using namespace cv;
using namespace std;


bool FoldersManagment::checkIfFolderIsEmpty(string path) const {

    char cmd[1024];
    int status, exitcode;

    char *folder = new char[path.length() + 1];
    std::strcpy(folder,path.c_str());

    snprintf(cmd, 1024, "test $(ls -A \"%s\" 2>/dev/null | wc -l) -ne 0", folder);

    status = system(cmd);
    exitcode = WEXITSTATUS(status);

    delete [] folder;

    ///the folder is empty
    if (exitcode == 1){
        return true;

    }else{
        return false;
    }

}

std::vector<string> FoldersManagment::explode(const string& s, const char& c) const {
    string buff{ "" };
    std::vector<string> v;

    for (auto n : s)
    {
        if (n != c) buff += n; else
            if (n == c && buff != "") { v.push_back(buff); buff = ""; }
    }
    if (buff != "") v.push_back(buff);

    return v;
}

void FoldersManagment::createNewPoolFolder(int folderNumber, string path) const {
    int status;
    string newPath = path+ to_string(folderNumber);

    char *finalPath = new char[newPath.length() + 1];
    std::strcpy(finalPath,newPath.c_str());

    ///create folder
    status = mkdir(finalPath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

    delete [] finalPath;


}

void FoldersManagment::createFolderWithName(string folderName, string path) const {
    int status;
    string newPath = path+ folderName;
    cout<<newPath<<endl;

    char *finalPath = new char[newPath.length() + 1];
    std::strcpy(finalPath,newPath.c_str());

    ///create folder
    status = mkdir(finalPath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

    delete [] finalPath;



}

void FoldersManagment::putMapInRelevantFolder(
        Mat thirdEditPool,string folderNumber, string path) const {

    try {
        //string pathWithFolderNumber = path + to_string(folderNumber);
        string pathWithFolderNumber = path + (folderNumber);

        cv::String folder = pathWithFolderNumber+"/*.pgm";
        std::vector<cv::String> Pictures;
        cv::glob(folder, Pictures);

        /// this is the first img in the folder
        if (Pictures.size()  == 0){
            imwrite(pathWithFolderNumber+"/1.pgm",thirdEditPool);
            waitKey(100);
        }else{ /// add img to the list of imges
            int imgNum = Pictures.size() + 1;
            string imgNumber =  to_string(imgNum) ;
            imwrite(pathWithFolderNumber+"/"+imgNumber+".pgm",thirdEditPool);
            waitKey(100);

        }
    } catch (...) {
        
    }
}

int FoldersManagment::findNumberOfFilesInDirectory(std::string path) const {
    int possibleMax = 1000; //some number you can expect.
    int count = 0;
    for (int istarter = 1; istarter < possibleMax; istarter++){
        string fileName = to_string(istarter);
        bool status = fileExistenceCheck(path +"/"+fileName);
        string foundIndex;
        if (status == true)
        {
            count++;
        }
        else
        {
            break;
        }

    }

    return count;
}

std::vector<string> FoldersManagment::getAllNamesOfFoldersInDir(string path) const {
    std::vector<string> names;
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (path.c_str())) != NULL) {
      /* print all the files and directories within directory */
      while ((ent = readdir (dir)) != NULL) {
        //printf ("%s\n", ent->d_name);
        string s = ent->d_name;
        if (s.find("..") != std::string::npos
                || s.find(".") != std::string::npos) {
        }
        else{
           names.push_back(s);
        }
      }
      closedir (dir);
    }
    return names;

}

bool FoldersManagment::fileExistenceCheck(const std::string& name) const {
    struct stat buffer;
    return (stat(name.c_str(), &buffer) == 0);
}

int FoldersManagment::findTheNumerOfImagesInFolder(string folderNumber, string path) const {
    
    try {
        string pathWithFolderNumber = path + (folderNumber);
        cv::String folder = pathWithFolderNumber+"/*.JPG";
        std::vector<cv::String> Pictures;
        cv::glob(folder, Pictures);

        /// this is the first img in the folder
        return Pictures.size();
    } catch (...) {
        return 0;
    }

}
