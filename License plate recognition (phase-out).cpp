#include <opencv2/opencv.hpp>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <iostream>

using namespace cv;
using namespace std;

Mat detect_license_plate(Mat frame, tesseract::TessBaseAPI* ocr) {
    Mat gray, gray_filtered, edged;

    // 1. Convert to grayscale
    cvtColor(frame, gray, COLOR_BGR2GRAY);

    // 2. Denoise while preserving edges: Bilateral filter
    bilateralFilter(gray, gray_filtered, 11, 17, 17);

    // 3. Edge detection (Canny algorithm)
    Canny(gray_filtered, edged, 30, 200);

    // 4. Find contours
    vector<vector<Point>> contours;
    findContours(edged, contours, RETR_TREE, CHAIN_APPROX_SIMPLE);
    sort(contours.begin(), contours.end(),
         [](const vector<Point>& c1, const vector<Point>& c2) {
             return contourArea(c1, false) > contourArea(c2, false);
         });

    vector<Point> plate_contour;
    string license_plate_text;

    // 5. Traverse contours to find an approximate quadrilateral region
    for (size_t i = 0; i < min(contours.size(), size_t(10)); i++) {
        double perimeter = arcLength(contours[i], true);
        vector<Point> approx;
        approxPolyDP(contours[i], approx, 0.018 * perimeter, true);

        if (approx.size() == 4) {  // Approximates to a quadrilateral
            plate_contour = approx;

            Rect rect = boundingRect(approx);
            Mat license_plate_region = gray(rect);

            // 6. Apply thresholding
            Mat license_plate_thresh;
            threshold(license_plate_region, license_plate_thresh, 150, 255, THRESH_BINARY);

            // 7. Perform OCR recognition
            ocr->SetImage(license_plate_thresh.data, license_plate_thresh.cols,
                          license_plate_thresh.rows, 1, license_plate_thresh.step);
            char* outText = ocr->GetUTF8Text();
            license_plate_text = string(outText);
            delete[] outText;

            break;  // Exit after finding one candidate region
        }
    }

    // 8. Draw the detected license plate region and recognition result
    if (!plate_contour.empty()) {
        polylines(frame, plate_contour, true, Scalar(0, 255, 0), 3);
        Rect rect = boundingRect(plate_contour);
        putText(frame, license_plate_text, Point(rect.x, rect.y - 10),
                FONT_HERSHEY_SIMPLEX, 0.9, Scalar(0, 255, 0), 2);
    }

    return frame;
}

int main() {
    // Open the default camera
    VideoCapture cap(0);
    if (!cap.isOpened()) {
        cerr << "Unable to open the camera!" << endl;
        return -1;
    }

    // Initialize Tesseract OCR
    tesseract::TessBaseAPI ocr;
    if (ocr.Init(NULL, "eng", tesseract::OEM_LSTM_ONLY)) {
        cerr << "Unable to initialize the Tesseract OCR engine!" << endl;
        return -1;
    }
    ocr.SetPageSegMode(tesseract::PSM_SINGLE_LINE); // Equivalent to --psm 7 in Python

    // Create a resizable window
    namedWindow("License Plate Recognition - Video Stream", WINDOW_NORMAL);
    resizeWindow("License Plate Recognition - Video Stream", 800, 600);

    while (true) {
        Mat frame;
        cap >> frame;  // Read video frame
        if (frame.empty()) {
            cerr << "Unable to read video frame!" << endl;
            break;
        }

        // Detect license plate and recognize characters
        Mat processed_frame = detect_license_plate(frame, &ocr);

        // Display the result
        imshow("License Plate Recognition - Video Stream", processed_frame);

        // Exit when ESC key is pressed
        if (waitKey(1) == 27) {
            break;
        }
    }

    cap.release();
    destroyAllWindows();
    ocr.End();

    return 0;
}
