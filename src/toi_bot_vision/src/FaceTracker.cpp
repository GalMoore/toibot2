#include "/home/gal/toibot_ws/src/ToiBot1/src/toi_bot_vision/include/toi_bot_vision/FaceTracker.h"


FaceTracker::FaceTracker(){


    deserialize("/home/gal/toibot_ws/src/ToiBot1/src/toi_bot_vision/models/shape_predictor_68_face_landmarks.dat") >> pose_model_;
    //deserialize("/home/toilab/toibot_ws/src/toi_bot_vision/models/shape_predictor_5_face_landmarks.dat") >> sp;

    //face recognition
    deserialize("/home/gal/toibot_ws/src/ToiBot1/src/toi_bot_vision/models/dlib_face_recognition_resnet_model_v1.dat") >> net;




}



void FaceTracker::trackeOverFaces(State& state,const Mat& frame){

    //cout<<"inisied tracking over face "<<endl;

    std::vector<std::vector<cv::Point> > landmarksFaces;
    std::vector<cv::Rect> facesRects;
    Mat tmp = frame.clone();
    cv_image<bgr_pixel> cimg(frame);

    // Detect faces
    std::vector<dlib::rectangle> faces = detector_(cimg);
    // Find the pose of each face.
    std::vector<full_object_detection> shapes;
    for (unsigned long i = 0; i < faces.size(); ++i){
        dlib:: rectangle r = faces[i];
        dlib::point tl = r.tl_corner();
        int x = tl.x();
        int y = tl.y();
        cv::Rect cvRect = Rect(x,y,r.width(),r.height());
        cv::rectangle(tmp,cvRect,cv::Scalar(0,255,0),2,8,0);
        facesRects.push_back(cvRect);
        int scale = 1;
        shapes.push_back(pose_model_(cimg, faces[i]));
        std::vector<cv::Point>  landmarks;
        for(unsigned int j = 0; j< shapes[i].num_parts();++j)
            {
                landmarks.push_back(Point(shapes[i].part(j).x()*(1/scale),shapes[i].part(j).y()*(1/scale)));
        }
        landmarksFaces.push_back(landmarks);
    }
    for(int i = 0; i < landmarksFaces.size(); i++){
        std::vector<cv::Point>  landmarks = landmarksFaces[i];

        for (int j = 0; j < landmarks.size(); j++){
            cv::Point p = landmarks[j];
            circle(tmp, p,1, Scalar(0,0,0), -1, 8, 0);
        }


    }
    imshow("tracker",tmp);
    waitKey(1);

    const int close_face_threshold = 7000;

    if(facesRects.size() == 0 ){

        canRecognizeAgain_ = true;
        state = tracking;
        return;
    }


    ///if biggest rect is good publish command (delta x, delta y
    /// else publish "no close-face"

    if( canRecognizeAgain_ == true ){

        for(int i =0; i < facesRects.size(); i++){
            Rect r = facesRects[i];
            double area = r.area();
            if( r.area() > close_face_threshold ){
                state = recognition;
                return;

            }
        }

    }

     state = tracking;



}

string FaceTracker::recognizeFace(State &state,const Mat& frame){

        canRecognizeAgain_ = false;

        /// folder is empty

        string path = "/home/gal/toibot_ws/src/ToiBot1/src/toi_bot_vision/faces";
        if( checkIfFolderIsEmpty(path) == true){


            state = tracking;
            return "hellow";
        }

        ////calc cuurent face

        cv_image<bgr_pixel> image(frame);
        matrix<rgb_pixel> imgOriginal;
        assign_image(imgOriginal, image);

        std::vector<matrix<rgb_pixel>> facesOriginal;
        for (auto face : detector_(imgOriginal)){

           auto shape = pose_model_(imgOriginal, face);
           matrix<rgb_pixel> face_chip;
           extract_image_chip(imgOriginal, get_face_chip_details(shape,150,0.25), face_chip);
           facesOriginal.push_back(move(face_chip));

        }

        std::vector<matrix<float,0,1>> faceOrig_descriptors;
        faceOrig_descriptors = net(facesOriginal);


        ///run over all the folders
        std::vector<string> names = getAllNamesOfFoldersInDir(path);

        string bestName;
        double minError = 1;
        for( int i =0; i < names.size(); i++){

           string pathName = (path) +"/"+ names[i];
           double error = checkMatch(faceOrig_descriptors,pathName);
            if( error <  minError){

                bestName = names[i];
                minError = error;


            }

        }

    state = tracking;
    return "hello "+bestName;




}

double FaceTracker::checkMatch(std::vector<matrix<float,0,1>> faceOrig_descriptors ,
                                string faceFolderPath){


    ////calc img from folder
    cv::String folder = faceFolderPath+"/*.jpg";
    std::vector<cv::String> picturesInFolder;
    cv::glob(folder, picturesInFolder);
    Mat imgFromFolder = imread(picturesInFolder[0],1);


    cv_image<bgr_pixel> image(imgFromFolder);
    matrix<rgb_pixel> imgFolder;
    assign_image(imgFolder, image);



    std::vector<matrix<rgb_pixel>> faceFolder;
    for (auto face : detector_(imgFolder)){

       auto shape = pose_model_(imgFolder, face);
       matrix<rgb_pixel> face_chip;
       extract_image_chip(imgFolder, get_face_chip_details(shape,150,0.25), face_chip);
       faceFolder.push_back(move(face_chip));

    }
    std::vector<matrix<float,0,1>> faceFolder_descriptors;
    faceFolder_descriptors = net(faceFolder);


    double l = length(faceOrig_descriptors[0]-faceFolder_descriptors[0]) ;


    return l;


}

std::vector<string> FaceTracker::getAllNamesOfFoldersInDir(string path) const {


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
Mat FaceTracker::getCropFace(const Mat& frame){


    std::vector<cv::Rect> facesRects;
    Mat tmp = frame.clone();
    cv_image<bgr_pixel> cimg(frame);

    // Detect faces
    std::vector<dlib::rectangle> faces = detector_(cimg);
    // Find the pose of each face.
    std::vector<full_object_detection> shapes;

    if( faces.size() == 1){
        for (unsigned long i = 0; i < faces.size(); ++i){
            dlib:: rectangle r = faces[i];
            dlib::point tl = r.tl_corner();
            int x = tl.x();
            int y = tl.y();
            cv::Rect cvRect = Rect(x,y,r.width(),r.height());
            //cv::rectangle(tmp,cvRect,cv::Scalar(0,255,0),2,8,0);
             if(cvRect.x > 0 && cvRect.y > 0 &&
                     cvRect.x + cvRect.width < tmp.cols && cvRect.y +cvRect.height < tmp.rows  ){
                 cv::Mat croppedImage = tmp(cvRect);

                 resize(croppedImage,croppedImage,cv::Size(310,310));
                 return croppedImage;
             }


        }
    } else {
        Mat empty(0,0, CV_8U, cv::Scalar(0));
        return empty;
    }






}

void FaceTracker::rememberMe(State &state, string name ,const Mat &frame){

    // detect the face and make crop
    std::vector<cv::Point> landmarks;
    std::vector<cv::Rect> facesRects;
    Mat tmp = frame.clone();
    cv_image<bgr_pixel> cimg(frame);

    // Detect faces
    std::vector<dlib::rectangle> faces = detector_(cimg);
    // Find the pose of each face.
    std::vector<full_object_detection> shapes;

    if( faces.size() == 1  ){

        dlib:: rectangle r = faces[0];
        dlib::point tl = r.tl_corner();
        int x = tl.x();
        int y = tl.y();
        cv::Rect cvRect = Rect(x,y,r.width(),r.height());
        Mat faceCrop = frame(cvRect).clone();


        /// create folder with name
        string path = "/home/gal/toibot_ws/src/ToiBot1/src/toi_bot_vision/faces";

        string pathName = (path) +"/"+name;

        createFolderWithName(name, path);
        waitKey(1000);

        imwrite(pathName+"/"+name+".jpg",frame);

        state = init;

    }
     else {

        cout<<" cant detect face !!  "<<endl;
        state = init;
    }



}

void FaceTracker::createFolderWithName(string folderName, string path) const {

    int status;
    string newPath = path+"/"+ folderName;

    cout<<newPath<<endl;

    char *finalPath = new char[newPath.length() + 1];
    std::strcpy(finalPath,newPath.c_str());

    ///create folder
    status = mkdir(finalPath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

    delete [] finalPath;

}

bool FaceTracker::checkIfFolderIsEmpty(string path) const {

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




