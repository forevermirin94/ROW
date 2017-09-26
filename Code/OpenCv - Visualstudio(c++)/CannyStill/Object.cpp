 /*
 © ROW TEAM 3 2016/2017
 
 Team members: Lamar Stein, Hung To, Rick de Jong and Jonathan Ratnavel
 
 Description of this program:
 This is a color recognition program which recognize the red color. 
 The program takes the streaming feed from the Raspberry pi 
 and make use of the frames in this program to recognize the color
 */


#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<iostream>

int main() {
	cv::VideoCapture capWebcam(0); // Activate the camera function
	capWebcam.open("http://172.24.1.1:8080/?action=stream");// Raspberry PI camera stream.

	if (capWebcam.isOpened() == false) {
		std::cout << "error: capWebcam not accessed successfully\n\n";	// IF NOT system output error.
		return(0);
	}

	cv::Mat imgOriginal;
	cv::Mat imgHSV;
	cv::Mat redColor;
	//cv::Mat blueColor;
	//cv::Mat colorCombi;

	char charCheckForEscKey = 0;

	while (charCheckForEscKey != 27 && capWebcam.isOpened()) {		// Houdt de connectie tot stand totdat er op ESC(ASCII 27) gedrukt wordt.
		bool FrameReadSuccessfully = capWebcam.read(imgOriginal);		// Haalt frames op met .read functie. > imgOriginal

		if (!FrameReadSuccessfully || imgOriginal.empty()) {		// Als .read FALSE meegeeft > leeg.
			std::cout << "error: frame not read from webcam\n";
			break;
		}

		cv::cvtColor(imgOriginal, imgHSV, CV_BGR2HSV); // Zet BGR frame over in HSV colorspace.

		inRange(imgOriginal, cv::Scalar(45, 15, 180), cv::Scalar(90, 60, 235), redColor);
		//inRange(imgOriginal, cv::Scalar(90, 60, 235), cv::Scalar(45, 15, 180), redColor); // Kleurbreedte van de kleur rood
																						 //inRange(imgOriginal, cv::Scalar(170, 100, 40), cv::Scalar(170, 120, 60), blueColor);	// Kleurbreedte van de kleur blauw

																						 //cv::GaussianBlur(redColor, redColor, cv::Size(3, 3), 0);
		cv::medianBlur(redColor, redColor, CV_32F);

		cv::Mat structuringElement = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));

		//~Verwijden en vernauwen van pixels

		//~Rood
			//~Verwijden van pixels
		cv::dilate(redColor, redColor, structuringElement);
			//~Vernauwen van pixels
		cv::erode(redColor, redColor, structuringElement);

		std::vector< std::vector<cv::Point> > contours;
		std::vector<cv::Point> points;
		cv::findContours(redColor, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
		for (size_t i = 0; i < contours.size(); i++) {
			for (size_t j = 0; j < contours[i].size(); j++) {
				cv::Point p = contours[i][j];
				points.push_back(p);
			}
		}

		// *Bepaalt de contouren van de kleur*
		// Tekent rechthoeken om de gedetecteerde kleur heen
		if (points.size() > 0) {
			cv::Rect brect = cv::boundingRect(cv::Mat(points).reshape(2));
			cv::rectangle(imgOriginal, brect.tl(), brect.br(), cv::Scalar(5, 100, 30), 2, CV_AA);
			cv::putText(imgOriginal, "OH LOOK WHAT WE HAVE HERE", brect.br(), CV_FONT_HERSHEY_SIMPLEX, 1, (128, 0, 0), 1, CV_AA);
		}

		// Definieer windows
		cv::namedWindow("imgOriginal", CV_WINDOW_AUTOSIZE);
		//cv::namedWindow("Red", CV_WINDOW_NORMAL);
		cv::namedWindow("imgHSV", CV_WINDOW_AUTOSIZE);

		cv::imshow("imgOriginal", imgOriginal);	// Laat originele frame zien
		cv::imshow("Red", redColor); // Laat frame met kleurdetectie rood + blauw
									 //cv::imshow("imgOriginal", imgHSV);

		charCheckForEscKey = cv::waitKey(1);			// Check of ESC toets wordt ingedrukt met een delay van 1 milliseconde.
	}

	return(0);
}