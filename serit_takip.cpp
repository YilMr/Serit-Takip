#include<opencv2/core.hpp>
#include<opencv2/opencv.hpp>
#include<opencv2/cvconfig.h>
#include<opencv2/dnn.hpp>
#include<opencv2/features2d.hpp>
#include<opencv2/flann.hpp>
#include<opencv2/gapi.hpp>
#include<opencv2/highgui.hpp>
#include<opencv2/imgcodecs.hpp>
#include<opencv2/calib3d.hpp>
#include<opencv2/imgproc.hpp>
#include<opencv2/ml.hpp>
#include<opencv2/video.hpp>
#include<opencv2/videoio.hpp>
#include<iostream>
#include<Windows.h>
#include<stdlib.h>
using namespace std;
using namespace cv;


Point2f noktalar[] = { Point2f(440,683),Point2f(1210,652),Point(1600,856),Point2f(70,800) };  // 4 tane nokta belirledik dikdörtgen
Point2f donusum[] = { Point2f(0,0),Point2f(640,0),Point2f(640,480),Point2f(0,480) };   //4 nokta için sýnýr deðerlerini belirledik
vector<int>serit;
Mat ilgili_bolge;
Mat sonuc;
int solKonum, sagKonum;
int pid;

void renk_donusumu(Mat giris) {

	Mat cikis;
	cvtColor(giris, giris, COLOR_BGR2GRAY);
	threshold(giris, cikis, 150, 255, THRESH_BINARY);
	Canny(giris, giris,70, 175);
	add(giris, cikis, sonuc);
	//imshow("Sonuc", sonuc);
}

void serit_bul() {

	serit.resize(640);
	serit.clear();
	for (int i = 0; i < 640; i++) {           // BURAYA ÇOK DÝKKAT ÇALIÞMAZSA RECT DEÐERLERÝYLE OYNAMA YAP
		ilgili_bolge = sonuc(Rect(i, 100, 2, 150));       //-> ilgili bölge belirledik, ilgili bölgeyi 2 piksel 150 piksel uzunluðunda dikdörtgenlere ayýrdýk
		divide(255, ilgili_bolge, ilgili_bolge);          // tüm dikdörtgenleri yerleþtirdik
		serit.push_back((int)(sum(ilgili_bolge)[0]));     //ilgili bölgede bulunan seritleri serit vektörüne ekledik
	}
}

void serit_goster(Mat son) {

	vector<int>::iterator sol;
	sol = max_element(serit.begin(), serit.begin() + 150);                    //--> sol þeridin nerede olduðunu buluyoruz
	solKonum = distance(serit.begin(), sol);
	vector<int>::iterator sag;
	sag = max_element(serit.begin() + 350, serit.end());                    //--> sag þeridin nerede olduðunu buluyoruz
	sagKonum = distance(serit.begin(), sag);
	line(son, Point(solKonum, 0), Point(solKonum, 480), Scalar(0, 156, 157), 3);
	line(son, Point(sagKonum, 0), Point(sagKonum, 480), Scalar(0, 156, 157), 3);
	imshow("Serit Tespiti", son);
	waitKey(0);
}

void serit_merkez_bul(Mat son) {
	int seritMerkez = (sagKonum - solKonum) / 2 + solKonum;
	int kalibre = son.cols / 2 - 16;
	pid = seritMerkez - kalibre;
	line(son, Point(seritMerkez, 0), Point(seritMerkez, 480), Scalar(0, 0, 255), 3);
	line(son, Point(kalibre, 0), Point(kalibre, 480), Scalar(255, 0, 0));
	imshow("Son Hali", son);
}


int main() {

	VideoCapture serit("otoyol.mp4");
	if (!serit.isOpened()) cout << "Serit bulma islemi basarisiz!" << endl;
	else
	{
		while (true) {

			Mat oku;
			serit.read(oku);
			Mat pers = getPerspectiveTransform(noktalar, donusum);      //--> noktalar ' ý donusum içine perspektif olacak þekilde dönüþtürdük
			Mat son(480, 640, CV_8UC3);                              //--> yeni son hal için matrix oluþturdul
			warpPerspective(oku, son, pers, son.size());			//--> perspektifi alýnmýþ resmimizi son matrisine yazdýrdýk
			renk_donusumu(son);
			serit_bul();
			serit_goster(son);
			serit_merkez_bul(son);
			stringstream yazi;
			yazi.str(" ");
			yazi.clear();
			yazi << " PID degeri : " << pid;
			putText(oku, yazi.str(), Point(150, 150), FONT_HERSHEY_TRIPLEX, 2, Scalar(0, 125, 250), 3);
			waitKey(0);
			imshow("Serit", son);
			imshow("orijinal", oku);
			if (waitKey(50) == 32) break;
		}
	}

}
