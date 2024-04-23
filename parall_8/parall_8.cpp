// parall_3.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#include <iostream>
#include <opencv2/opencv.hpp>
#include <omp.h>
#include <vector>


using namespace cv;
using namespace std;
void proccesing(vector<Mat>& video) {
    CascadeClassifier faceC, smileC, eyeC;
    faceC.load("haarcascade_frontalface_alt.xml");
    smileC.load("haarcascade_smile.xml");
    eyeC.load("haarcascade_eye_tree_eyeglasses.xml");
    int id = omp_get_thread_num();
    for (int i = 0; i < video.size();i++) {
        Mat frame, gray;
        frame = video[i];
        vector<Rect> face, smile, eye;
        cvtColor(frame, gray, COLOR_BGR2GRAY);
        faceC.detectMultiScale(gray, face, 1.1, 5, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));
        for (size_t i = 0; i < face.size(); ++i) {
            rectangle(frame, face[i], Scalar(255, 0, 0), 2);

            Mat faceID = gray(face[i]);

            smileC.detectMultiScale(faceID, smile, 2, 15, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));
            for (size_t j = 0; j < smile.size(); ++j) {
                Point smile1(face[i].x + smile[j].x + smile[j].width, face[i].y + smile[j].y + smile[j].height);
                Point smile2(face[i].x + smile[j].x, face[i].y + smile[j].y);
                rectangle(frame, smile1, smile2, Scalar(0, 255, 0), 2);
            }
            eyeC.detectMultiScale(faceID, eye, 2, 5, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));
            for (size_t j = 0; j < eye.size(); ++j) {
                Point eye1(face[i].x + eye[j].x + eye[j].width, face[i].y + eye[j].y + eye[j].height);
                Point eye2(face[i].x + eye[j].x, face[i].y + eye[j].y);
                rectangle(frame, eye1, eye2, Scalar(0, 0, 255), 2);
            }


        }
        video[i] = frame;
        cout << "[" << id << "]: iter = " << i << endl;
    }
}

int main()
{
    VideoCapture video("ZUA.mp4");
    video.isOpened();
    int width = static_cast<int>(video.get(CAP_PROP_FRAME_WIDTH));
    int height = static_cast<int>(video.get(CAP_PROP_FRAME_HEIGHT));
    VideoWriter outputVideo("output_video1.avi", VideoWriter::fourcc('M', 'J', 'P', 'G'), 30, Size(width, height));
    namedWindow("edit", WINDOW_AUTOSIZE);

    int totalFrames = video.get(CAP_PROP_FRAME_COUNT);
    vector<Mat> first,second;
    for (int i = 0; i < totalFrames; i++) {
        Mat frame;
        video >> frame;
        if (frame.empty())
        {
            break;
        }
        if (i < totalFrames/2)
            first.push_back(frame);
        else 
            second.push_back(frame);
    }


#pragma omp parallel sections num_threads(2)
    {
#pragma omp section
        {
            proccesing(first);
        }
#pragma omp section
        {
            proccesing(second);
        }
    }
    for (int i = 0; i < first.size(); i++) {
        outputVideo.write(first[i]);
        imshow("edit", first[i]);
    }
    for (int i = 0; i < second.size(); i++) {
        outputVideo.write(second[i]);
        imshow("edit", second[i]);
    }


    cout << "size " << first.size()<<endl;
    video.release();
    outputVideo.release();
    destroyAllWindows();
    return 0;
}



// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
