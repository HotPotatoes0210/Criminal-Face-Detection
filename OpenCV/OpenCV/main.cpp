#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/face.hpp>
#include <string>
#include <vector>
#include <fstream>

using namespace cv;
using namespace std;

string path_list = "your_criminal_name_list_txt_file"; 

string path_face = "path_to_face_data";

void FaceDataCapture(VideoCapture camera_access, CascadeClassifier& face_cascade, string criminal_name) {
    namedWindow("Camera Feed", WINDOW_AUTOSIZE);

    for (int i = 1; i <= 10; ++i) {
        Mat frame;
        while (true) {
            camera_access.read(frame);
            if (frame.empty()) {
                cout << "Unable to read the frame" << endl;
                return;
            }

            Mat gray_frame;
            cvtColor(frame, gray_frame, COLOR_BGR2GRAY);
            vector<Rect> faces;
            face_cascade.detectMultiScale(gray_frame, faces, 1.1, 3, 0 | CASCADE_SCALE_IMAGE, Size(90, 90));

            for (const auto& face : faces) {
                Mat faceROI = gray_frame(face);
                Mat resizedFace;
                resize(faceROI, resizedFace, Size(100, 100), 1.0, 1.0, INTER_CUBIC);

                rectangle(frame, face, Scalar(0, 255, 0), 3);
                string filename = path_face + "/" + criminal_name + to_string(i) + ".png";
                imwrite(filename, resizedFace);

                imshow("Camera Feed", frame);
                cout << "Saved image: " << filename << endl;
                waitKey(500); //wait for 1s
                continue;
            }

            if (!faces.empty()) break;
        }
    }
    destroyWindow("Camera Feed");
}

void FaceRecognition(VideoCapture& camera_access, CascadeClassifier& face_cascade) {
    namedWindow("Camera Feed", WINDOW_AUTOSIZE);
    vector<Mat> images;
    vector<int> labels;

    // Load images for training
    bool data_loaded = false;
    fstream criminal_list_name;
    criminal_list_name.open(path_list);
    vector<string> name_list;
    string line;
    if(criminal_list_name.is_open()){
        cout << "Load the list successfully" << endl;
        while(getline(criminal_list_name,line)){
            name_list.push_back(line);
            data_loaded = true;
        }
    }
    else{
        cout << "Error loading the file" << endl;
    }
    
    Ptr<face::LBPHFaceRecognizer> recognizer = face::LBPHFaceRecognizer::create();
    
    if(data_loaded){
        for(int j = 0 ; j < name_list.size(); j++){
            for (int i = 1; i <= 10; ++i) {
                string filename = path_face + "/" + name_list[j] + to_string(i) + ".png";
                Mat img = imread(filename, IMREAD_GRAYSCALE);
                if (img.empty()) {
                    cout << "Unable to load image " << filename << endl;
                    return;
                }
                images.push_back(img);
                labels.push_back(j+1);
            }
        }
        recognizer->train(images, labels);
    }

    double threshold = 100.0;
    
    while (true) {
        Mat frame;
        camera_access.read(frame);
        if (frame.empty()) {
            cout << "Unable to read the frame" << endl;
            break;
        }

        Mat gray_frame;
        cvtColor(frame, gray_frame, COLOR_BGR2GRAY);
        vector<Rect> faces;
        face_cascade.detectMultiScale(gray_frame, faces, 1.1, 3, 0 | CASCADE_SCALE_IMAGE, Size(90, 90));
        
        //Making data prediction and recognize
        if(!path_face.empty()){
            for (const auto& face : faces) {
                Mat faceROI = gray_frame(face);
                Mat resizedFace;
                resize(faceROI, resizedFace, Size(100, 100), 1.0, 1.0, INTER_CUBIC);

                if(data_loaded){
                    int predicted_label = -1;
                    double confidence = 0.0;
                    recognizer->predict(resizedFace, predicted_label, confidence);

                    rectangle(frame, face, Scalar(0, 255, 0), 3);
                    if (confidence < threshold) {
                        putText(frame, "Criminal Detected: " + name_list[predicted_label - 1], Point(face.x, face.y - 10), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255), 2);
                        rectangle(frame, face, Scalar(0, 0, 255), 3);
                        cout << "Criminal Detected: " << confidence << endl;

                    } else {
                        putText(frame, "Innocence", Point(face.x, face.y - 10), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0), 2);
                        string conf = "Confidence: " + to_string(confidence);
                        putText(frame, conf, Point(face.x, face.y + face.height + 20), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0), 2);
                    }
                } else {
                    rectangle(frame, face, Scalar(0, 255, 0), 3);
                    putText(frame, "Innocent", Point(face.x, face.y - 10), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0), 2);
                }
            }
        }

        imshow("Camera Feed", frame);
        char key = waitKey(1);
        if (key == 27) {
            break;
        }
    }

    camera_access.release();
    destroyAllWindows();
}


int main() {
    VideoCapture camera_access(1);
    if (!camera_access.isOpened()) {
        cout << "Error: Could not open camera" << endl;
        return -1;
    }

    camera_access.set(CAP_PROP_FRAME_HEIGHT, 480);
    camera_access.set(CAP_PROP_FRAME_WIDTH, 650);
    camera_access.set(CAP_PROP_BUFFERSIZE, 3);
    camera_access.set(CAP_PROP_FPS, 60);

    CascadeClassifier face_cascade;
    if (!face_cascade.load("/Users/hungtran/Desktop/C++Project/OpenCV/Data/haarcascade_frontalface_default.xml")) {
        cout << "Error loading face cascade" << endl;
        return -1;
    }

    int command;
    cout << "Welcome to Face Detection and Face Recognition software" << endl;
    cout << "1. Scan new criminal data" << endl;
    cout << "2. Open Face Recognition" << endl;
    cout << "Please enter your option (1/2): ";
    cin >> command;
    string criminial_name;
    
    ofstream name_list;
    name_list.open(path_list, fstream::app);

    while (command != 1 && command != 2) {
        cout << "Invalid option. Please enter your option (1/2): ";
        cin >> command;
    }

    switch (command) {
        case 1:
            cout << "Enter the name of the criminal: ";
            cin >> criminial_name;
            name_list << criminial_name << "\n";
            FaceDataCapture(camera_access, face_cascade, criminial_name);
            break;
        case 2:
            FaceRecognition(camera_access, face_cascade);
            break;
    }

    return 0;
}
