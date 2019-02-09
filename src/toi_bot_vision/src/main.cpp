
#include "/home/gal/toibot_ws/src/ToiBot1/src/toi_bot_vision/include/toi_bot_vision/personManager.h"
#include <ctime>


using namespace dlib;
using namespace std;


void initState(){

    string fullPath ="/home/gal/toibot_ws/src/ToiBot1/src/toi_bot_vision/command.txt";
    remove(fullPath.c_str());

    //calc the new avg

    ofstream myfile;
    myfile.open ("/home/gal/toibot_ws/src/ToiBot1/src/toi_bot_vision/command.txt");
    myfile<<1<<endl;

}
void getName(string& name){

    ifstream nameFile;
    nameFile.open("/home/gal/toibot_ws/src/ToiBot1/src/toi_bot_vision/name.txt");
    std::string   line;

    while(std::getline(nameFile, line)){
        std::stringstream   linestream(line);

        nameFile.close();
        name = line;
        return;

    }



}

int main()
{
    int count = 0;
    cv::VideoCapture cap(0);
    cv::Mat frame;
    State state = init;

    PersonManager personManager_;

    while(true){

        count++;

        cap >> frame;

        personManager_.getCallback(state);

        if( state == init){
            state = tracking;
        }

        if( state == tracking ){

            personManager_.track(state,frame);
            if( state == recognition){

                cout<<" iniside recognition "<<endl;

                string name = personManager_.recognize(state,frame);
                cv::putText(frame,name,
                            cv::Point(frame.rows/2,frame.cols/2),1, 3, cv::Scalar(0,255,0),3,8);
                imshow("frame",frame);
                waitKey(1);

            }
        }
        if( state == memorization){
            cout<<" iniside remember me "<<endl;
            string name;
            getName(name);
            personManager_.rememberMe(state,name, frame);
            initState();


        }

        if( state == emotionRecognition){

            string emotion = personManager_.detectEmotion(state, frame);
            cout<<"emotion: "<<emotion<<endl;
            cv::putText(frame,emotion,
                        cv::Point(frame.rows/2,frame.cols/2),1, 3, cv::Scalar(0,255,0),3,8);
            imshow("frame",frame);
            waitKey(1);
            initState();

         }

    }


  return 0;
}


