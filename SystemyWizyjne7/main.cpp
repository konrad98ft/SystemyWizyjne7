#include<opencv2/opencv.hpp>
#define IMAGES_PATH ""
#define DST_W 100
#define DST_H 140

#define STEP 3

void wait(int time = 0)
{
	if (cv::waitKey(time) == 'q') exit(0);
}

void konturowanie(const cv::Mat& img_c, std::vector<std::vector<cv::Point>>& contours, std::vector<cv::Vec4i>& hierarchy)
{
	cv::Mat img_gr;
	cv::cvtColor(img_c, img_gr, cv::COLOR_BGR2GRAY);
	cv::threshold(img_gr, img_gr, 250, 255, cv::THRESH_BINARY_INV);
	cv::findContours(img_gr, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
}

void testObecnosci(cv::Mat img)
{
	int w = img.cols;
	int h = img.rows;
	cv::Rect r[] =
	{
		cv::Rect(0, 0, w / 4, h / 5),
		cv::Rect(w * 3 / 8, 0, w / 4, h / 5),
		cv::Rect(w * 3 / 8, h * 4 / 10, w / 4, h / 5),
		cv::Rect(w * 3 / 8, h * 4 / 5, w / 4, h / 5)
	};
	double sz[4];
	cv::Mat gray;
	for (int i = 0; i < 4; ++i) cv::rectangle(img, r[i], cv::Scalar(0, 255, 0));
	cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
	cv::threshold(gray, gray, 250, 255, cv::THRESH_BINARY_INV);
	for (int j = 0; j < 4; ++j)
	{
		sz[j] = cv::mean(cv::Mat(gray, r[j]) != 0)[0];
		sz[j] /= 255;
		std::cout << sz[j] << "; ";
	}
	std::cout << std::endl;
	if (sz[2] < 0.15) 
	{
		if (sz[0] > 0.8)
		std::cout << "D"<<std::endl;
		else
		std::cout << "O" << std::endl;
	}
	else
	{
		if (sz[1] < 0.2)
		{
			std::cout << "H" << std::endl;
		}
		else
		{
			if (sz[3] < 0.2)
			std::cout << "F" << std::endl;
			else
			std::cout << "B" << std::endl;

		}
	}
		


}

void analiza(const cv::Mat& img_c, const std::vector<std::vector<cv::Point>>& contours, const std::vector<cv::Vec4i>& hierarchy)
{
	for (int i = 0; i < contours.size(); ++i)
	{
		if (hierarchy[i][3] < 0) //element nadrz�dny
		{
			cv::RotatedRect r_rect = cv::minAreaRect(contours[i]);
			cv::Vec<cv::Point2f, 4> points;
			r_rect.points(&points[0]);
			cv::Mat img_t = img_c.clone();
			for (int i = 0; i < 4; ++i)
			{
				std::stringstream ss;
				ss << i;
				cv::line(img_t, points[i], points[(i + 1) % 4], cv::Scalar(0, 0, 255));
				cv::putText(img_t, ss.str(), cv::Point(points[i]), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar::all(0));
			}
			cv::imshow("Test", img_t);
#if STEP==0
			cv::Point2f src_p[] = { points[0],points[1],points[2] };
#endif
#if STEP>0
			int FREE_W = 0, FREE_H = 0;
			//cv::namedWindow("Test2", cv::WINDOW_NORMAL);
			int offset = r_rect.size.aspectRatio() > 1 ? 1 : 0;
			cv::Point2f src_p[] = { points[0 + offset],points[1 + offset],points[2 + offset] };
#endif
			cv::Point2f dst_p[] = { cv::Point2f(0,DST_H - 1),cv::Point2f(0,0),cv::Point2f(DST_W - 1,0) };
			cv::Mat transform = cv::getAffineTransform(src_p, dst_p);
			cv::Mat dstImg;
			cv::warpAffine(img_c, dstImg, transform, cv::Size(DST_W, DST_H));
#if STEP==2
			std::vector<cv::Point> tr_point;
			cv::transform(contours[i], tr_point, transform);
			cv::Mat img_cont(DST_H, DST_W, CV_8UC1);
			img_cont = 0;
			std::vector<std::vector<cv::Point>> c2{ tr_point };
			//cv::drawContours(img_cont, c2, 0, cv::Scalar::all(255), 1);
			//cv::drawContours(img_cont, std::vector<std::vector<cv::Point>>(1,tr_point), 0, cv::Scalar::all(255), 1);
			cv::polylines(img_cont, tr_point, true, cv::Scalar::all(255),1);
			cv::imshow("img_cont", img_cont);
#endif
#if STEP==3
			testObecnosci(dstImg);
#endif
#if STEP==4
			if (r_rect.size.aspectRatio() < 1)
			{
				FREE_W = r_rect.size.width;
				FREE_H = r_rect.size.height;
			}
			else
			{
				FREE_W = r_rect.size.height;
				FREE_H = r_rect.size.width;
			}
			cv::Point2f dst2_p[] = { cv::Point2f(0,FREE_H - 1),cv::Point2f(0,0),cv::Point2f(FREE_W - 1,0) };
			cv::Mat transform2 = cv::getAffineTransform(src_p, dst2_p);
			cv::Mat dstImg;
			cv::warpAffine(img_c, dstImg, transform2, cv::Size(FREE_W, FREE_H));
			std::cout << FREE_W <<" "<< FREE_H << std::endl;
			
			
#endif


			cv::imshow("Test2", dstImg);
			
			wait();
			cv::destroyAllWindows();
		}
	}
}

int main()
{
	for (int i = 1; i <= 3; ++i)
	{
		std::stringstream ss;
		ss << IMAGES_PATH << "" << i << ".png";
		cv::Mat img = cv::imread(ss.str());
		cv::imshow("Test", img);
		std::vector<std::vector<cv::Point>> contours;
		std::vector<cv::Vec4i> hierarchy;
		konturowanie(img, contours, hierarchy);
		analiza(img, contours, hierarchy);
		wait();
	}
}