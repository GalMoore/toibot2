

#include "/home/gal/toibot_ws/src/ToiBot1/src/toi_bot_vision/include/toi_bot_vision/personManager.h"

PersonManager::PersonManager(){

}

void PersonManager::getCallback(State &state){

    ifstream commandFile;
    commandFile.open("/home/gal/toibot_ws/src/ToiBot1/src/toi_bot_vision/command.txt");
    std::string   line;

    while(std::getline(commandFile, line)){
        std::stringstream   linestream(line);

        int numState = atoi(line.c_str());
        state = static_cast<State>(numState);

    }
    commandFile.close();
    //cout<<" state is "<<state<<endl;


}


void PersonManager::track(State &state,const Mat& frame){

    faceTracker_.trackeOverFaces(state,frame);

}

string PersonManager::recognize(State &state,const Mat &frame){


    return faceTracker_.recognizeFace(state,frame);
}

void PersonManager::rememberMe(State &state, string name ,const Mat &frame){

        faceTracker_.rememberMe(state, name, frame);

}

string PersonManager::detectEmotion(State& state,const Mat& frame){

    Mat cropFace =  faceTracker_.getCropFace(frame);

        if(cropFace.rows > 0 ){

            string emotion = emotionDetector_.detectEmotion(cropFace);
            return emotion;
        }
        else {
           return "there is no face ";
        }





}





