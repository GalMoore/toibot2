

#include <opencv2/opencv.hpp>


#include <dlib/opencv.h>
#include <opencv2/highgui/highgui.hpp>


#include <dlib/dnn.h>
#include <iostream>
#include <dlib/data_io.h>
#include <dlib/gui_widgets.h>
#include <dlib/image_transforms.h>

using namespace std;
using namespace dlib;
using namespace cv;



template <template <int,template<typename>class,int,typename> class blockEmotion, int N, template<typename>class BN, typename SUBNET>
using residual = add_prev1<blockEmotion<N,BN,1,tag1<SUBNET>>>;

template <template <int,template<typename>class,int,typename> class block, int N, template<typename>class BN, typename SUBNET>
using residual_down = add_prev2<avg_pool<2,2,2,2,skip1<tag2<block<N,BN,2,tag1<SUBNET>>>>>>;

template <int N, template <typename> class BN, int stride, typename SUBNET>
using blockEmotion  = BN<con<N,3,3,1,1,relu<BN<con<N,3,3,stride,stride,SUBNET>>>>>;

template <int N, typename SUBNET> using ares      = relu<residual<blockEmotion,N,affine,SUBNET>>;
template <int N, typename SUBNET> using ares_down = relu<residual_down<blockEmotion,N,affine,SUBNET>>;

template <typename SUBNET> using level1 = ares<512,ares<512,ares_down<512,SUBNET>>>;
template <typename SUBNET> using level2 = ares<256,ares<256,ares<256,ares<256,ares<256,ares_down<256,SUBNET>>>>>>;
template <typename SUBNET> using level3 = ares<128,ares<128,ares<128,ares_down<128,SUBNET>>>>;
template <typename SUBNET> using level4 = ares<64,ares<64,ares<64,SUBNET>>>;

using anet_typeEnotion = loss_multiclass_log<fc<4,avg_pool_everything<
                            level1<
                            level2<
                            level3<
                            level4<
                            max_pool<3,3,2,2,relu<affine<con<64,7,7,2,2,
                            input_rgb_image_sized<227>
                            >>>>>>>>>>>;


class EmotionDetection {

public:

    EmotionDetection();

    virtual ~EmotionDetection() {

    }

public:

    dlib::rectangle make_random_cropping_rect_resnet(
        const matrix<rgb_pixel>& img,
        dlib::rand& rnd
    );
    void randomly_crop_images (
        const matrix<rgb_pixel>& img,
        dlib::array<matrix<rgb_pixel>>& crops,
        dlib::rand& rnd,
        long num_crops
    );

    string detectEmotion(const Mat &faceImg);






private:

    std::vector<string> labels{"1","2","3","4"}; //1- happy, 4 - suprise
    anet_typeEnotion net;


};
