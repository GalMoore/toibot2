
#include "/home/gal/toibot_ws/src/ToiBot1/src/toi_bot_vision/include/toi_bot_vision/emotionDetection.h"

EmotionDetection::EmotionDetection(){


    deserialize("/home/gal/toibot_ws/src/ToiBot1/src/toi_bot_vision/models/emotion.dnn") >> net;




}

dlib::rectangle EmotionDetection::make_random_cropping_rect_resnet(
    const matrix<rgb_pixel>& img,
    dlib::rand& rnd
)
{
    // figure out what rectangle we want to crop from the image
    double mins = 0.466666666, maxs = 0.875;
    auto scale = mins + rnd.get_random_double()*(maxs-mins);
    auto size = scale*std::min(img.nr(), img.nc());
    dlib::rectangle rect(size, size);
    // randomly shift the box around
    point offset(rnd.get_random_32bit_number()%(img.nc()-rect.width()),
                 rnd.get_random_32bit_number()%(img.nr()-rect.height()));
    return move_rect(rect, offset);
}

// ----------------------------------------------------------------------------------------

void EmotionDetection::randomly_crop_images (
    const matrix<rgb_pixel>& img,
    dlib::array<matrix<rgb_pixel>>& crops,
    dlib::rand& rnd,
    long num_crops
)
{
    std::vector<chip_details> dets;
    for (long i = 0; i < num_crops; ++i)
    {
        auto rect = make_random_cropping_rect_resnet(img, rnd);
        dets.push_back(chip_details(rect, chip_dims(227,227)));
    }

    extract_image_chips(img, dets, crops);

    for (auto&& img : crops)
    {
        // Also randomly flip the image
        if (rnd.get_random_double() > 0.5)
            img = fliplr(img);

        // And then randomly adjust the colors.
        apply_random_color_offset(img, rnd);
    }
}
// ----------------------------------------------------------------------------------------
string EmotionDetection::detectEmotion(const Mat& faceImg){

    Mat resultMat =    faceImg.clone();

    cv_image<bgr_pixel> image(faceImg);
    matrix<rgb_pixel> img;
    assign_image(img, image);


    softmax<anet_typeEnotion::subnet_type> snet;
    snet.subnet() = net.subnet();

    dlib::array<matrix<rgb_pixel>> images;
    matrix<rgb_pixel> crop;

    dlib::rand rnd;
    image_window win;


    const int num_crops = 1;//16;
    // Grab 16 random crops from the image.  We will run all of them through the
    // network and average the results.
    randomly_crop_images(img, images, rnd, num_crops);
    // p(i) == the probability the image contains object of class i.
    clock_t begin = clock();



    matrix<float,1,4> p = sum_rows(mat(snet(images.begin(), images.end())))/num_crops;

    clock_t end = clock();
    double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    cout<<elapsed_secs<<endl;

    int best;
    // Print the 5 most probable labels
    for (int k = 0; k < 5; ++k)
    {
        unsigned long predicted_label = index_of_max(p);
        //cout << p(predicted_label) << ": " << labels[predicted_label] << endl;
        p(predicted_label) = 0;
        if( k == 0 ){
            best =  atoi(labels[predicted_label].c_str());;
            if(best == 1){
                return "happy";
            }
            if(best == 2){
                return "neutral";
            }
            if(best == 3){
                return "sad";
            }
            if(best == 4){
                return "surprised";
            }
        }

    }

    return "nothing";









}


