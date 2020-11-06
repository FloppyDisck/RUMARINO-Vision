#include "aimod/aimod.hpp"

AI::AI(std::string input_path, bool record, std::string output_path) {
    //Set recording flag
    recording = record;

    std::string cfg, weights, names_path;

    //Identify the files inside the folder
    for (const auto & entry : std::experimental::filesystem::directory_iterator(input_path)) {
        if (entry.path().has_extension()) {
            if (entry.path().extension().string() == ".cfg") {
                cfg = entry.path().string();
            }
            else if (entry.path().extension().string() == ".names") {
                names_path = entry.path().string();
            }
        }
        else if (entry.path().stem().string() == "weights") {
            for (const auto & all_weights : std::experimental::filesystem::directory_iterator(entry.path())) {
                std::string weight_str = all_weights.path().stem();
                //TODO: for now we only look for the best weights, there must be a better way
                if (weight_str.substr(weight_str.length() - 5) == "_best") {
                    weights = all_weights.path().string();
                    break;
                }
            }
        }
    }

    //Get the names inside the file
    std::ifstream names_file;
    names_file.open(names_path.c_str());
    if(!names_file) {
        std::cout << "[AI] [ERROR] No .nammes file found";
    }
    else {
        std::string name;
        while(!names_file.eof()) {
            getline(names_file, name);
            names.push_back(name);
        }
    }

    // Load darknet
    darknet = new Detector(cfg, weights);

    if(recording) {
        //Start the CV Video Writer
        out_vid = cv::VideoWriter(output_path, cv::VideoWriter::fourcc('M','P','E','G'), 15, cv::Size(1920, 1080));
    }
}

std::vector<DetectedObject> AI::detect(Video_Frame &frame, float minimum_confidence) {
    //Create the resulting struct
    std::vector<DetectedObject> results;

    // Image where detected structures are written in
    cv::Mat annotated_img;
    if(recording)
        cv::cvtColor(frame.image, annotated_img, cv::COLOR_BGRA2BGR);

    //Predict items from the frame
    auto predictions = darknet->detect(frame.image, minimum_confidence);

    for(auto &prediction : predictions) {
        DetectedObject result;

        result.bounding_box = cv::Rect(prediction.x, prediction.y, prediction.w, prediction.h);
        result.id = prediction.obj_id;
        // Get the name of the detected object; leave it empty if not found
        if (prediction.obj_id < names.size()) {
            result.name = names[prediction.obj_id].c_str();
        }
        else {
            result.name = "";
        }


        //Get mid point from of the predicted image and get the distance from the depth image
        //Float taken from depth map is distance in millimeters (mm)
        cv::Point2f mid_point = cv::Point2f(prediction.x + prediction.w/2, prediction.y + prediction.h/2);
        result.distance = frame.depth_map.at<float>(mid_point);

        //Saving the 3D point on the struct
        result.location.x = frame.point_cloud.at<cv::Vec4f>(mid_point)[0];
        result.location.y = frame.point_cloud.at<cv::Vec4f>(mid_point)[1];
        result.location.z = frame.point_cloud.at<cv::Vec4f>(mid_point)[2];

        if(recording)
            cv::rectangle(annotated_img, result.bounding_box, cv::Scalar(0,255,0), 4, 8, 0);

        results.push_back(result);
    }

    if(recording){
        out_vid.write(annotated_img);
    }

    return results;
}

std::vector<DetectedObject> AI::detect(cv::Mat &frame, float minimum_confidence) {
    //Create the resulting struct
    std::vector<DetectedObject> results;

    // Image where detected structures are written in
    cv::Mat annotated_img;
    if(recording)
        annotated_img = frame;

    //Predict items from the frame
    auto predictions = darknet->detect(frame, minimum_confidence);

    for(auto &prediction : predictions) {
        DetectedObject result;

        result.bounding_box = cv::Rect(prediction.x, prediction.y, prediction.w, prediction.h);
        result.id = prediction.obj_id;
        result.name = "Empty";

        if(recording) {
            cv::rectangle(annotated_img, result.bounding_box, cv::Scalar(0,255,0), 1, 8, 0);
        }

        results.push_back(result);
    }

    if(recording){
        out_vid.write(annotated_img);
    }

    return results;
}

void AI::close() {
    if(recording){
        out_vid.release();
    }
}

AI::~AI() {
    AI::close();
}