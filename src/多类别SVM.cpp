#include<opencv2\opencv.hpp>
#include<iostream>

using namespace std;
using namespace cv;
#include <string>
#include<vector>
#include<direct.h>    //_mkdir  
#include<io.h> //_access
#include<memory>
#include<map>

void creatFolder(string pathname) {
	if (_access(pathname.c_str(), 0) == -1)
	{
		int i = _mkdir(pathname.c_str());
	}
}



void getFiles(const std::string & path, std::vector<std::string> & files)
{
	//文件句柄
	long long hFile = 0;
	//文件信息，_finddata_t需要io.h头文件
	struct _finddata_t fileinfo;
	std::string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//如果是目录,迭代之
			//如果不是,加入列表
			if ((fileinfo.attrib & _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)//不考虑隐藏文件夹“.”和“..”,不进行迭代文件夹“.”和“..”
					getFiles(p.assign(path).append("\\").append(fileinfo.name), files);//迭代得到同个目录下的所有文件
			}
			else
			{
				files.push_back(p.assign(path).append("\\").append(fileinfo.name));
			}
		} while (_findnext(hFile, &fileinfo) == 0);//查找hFile文件句柄下的同一级的下一个目录
		_findclose(hFile);
	}
}


struct filesplitname {
	string drive;
	string dir;
	string fname;
	string ext;

};


filesplitname spilitName(string filename) {
	char drive[_MAX_DRIVE] = { 0 };
	char dir[_MAX_DIR] = { 0 };
	char fname[_MAX_FNAME] = { 0 };
	char ext[_MAX_EXT] = { 0 };
	_splitpath_s(filename.c_str(), drive, dir, fname, ext);
	filesplitname s;

	s.drive = drive;
	s.dir = dir;
	s.fname = fname;
	s.ext = ext;

	return s;

}

void listFile(const std::string & path, vector<string> &files,int filetype=1) {
	//文件句柄
	long long hFile = 0;
	//文件信息，_finddata_t需要io.h头文件
	struct _finddata_t fileinfo;
	std::string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			if ((fileinfo.attrib & _A_SUBDIR))
			{
				if (filetype&&strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)//不考虑隐藏文件夹“.”和“..”,不进行迭代文件夹“.”和“..”
				{
					files.push_back(fileinfo.name);
				}
			}
			else {
				if (filetype == 0) {
					files.push_back(fileinfo.name);
				}
			}

		} while (_findnext(hFile, &fileinfo) == 0);//查找hFile文件句柄下的同一级的下一个目录
		_findclose(hFile);
	}
}

void main()
{
	vector<string> classlist = { 
		"aeroplane",
		"bicycle",
		"bird",
		"boat",
		"bottle",
		"bus",
		"car",
		"cat",
		"chair",
		"cow",
		"diningtable",
		"dog",
		"horse",
		"motorbike",
		"person",
		"pottedplant",
		"sheep",
		"sofa",
		"train",
		"tvmonitor"};
	/*vector<string> classlist = {
		"0",
		"1",
		"2",
		"3"};*/

	string path = ".\\data";
	int num;
	vector<string> files;
	listFile(path, files, 1);
	num = files.size();

	map<int,vector<Mat>> tmp;
	map<int, vector<Mat>> tmp_test;
	for (const auto &f : files) {
		//cout << "\"" << f << "\"" << "," << endl;
		vector<string>::iterator ite = find(classlist.begin(), classlist.end(), f);
		auto index = std::distance(begin(classlist), ite);
		//cout << index << endl;
		tmp[index] = vector<Mat>();
		string jpeg_path=path+"\\"+f;
		vector<string> jpeg_files;
		listFile(jpeg_path, jpeg_files,0);

		for (const auto &jf : jpeg_files) {
			//cout << jf << endl;
			Mat src = imread(jpeg_path + "\\" + jf);
			
			cvtColor(src, src, CV_RGB2HSV);
			cvtColor(src, src, CV_RGB2GRAY);
			src.convertTo(src, CV_32FC1);
			resize(src, src, Size(50, 50));
			src=src.reshape(0, 1);
			/*vector<float> descriptors;
			cv::HOGDescriptor hog(cv::Size(20, 20), cv::Size(16, 16), cv::Size(1, 1), cv::Size(8, 8), 8);
			hog.compute(src, descriptors);
			Mat dest(descriptors,true);*/

			tmp[index].push_back(src);
			
	    }
	}

	/*for (int i = 0; i < classlist.size(); i++) {
		int mid = int(tmp[i].size() * 3 / 5);
		for (int j = 0; j < mid; j++) {

		}
	}*/


	int totalnum = 0;
	int totalnum_test = 0;
	for (int i = 0; i < classlist.size(); i++) {
		for (int j = 0; j < tmp[i].size(); j++) {
			cout << classlist[i]<<" : "<<tmp[i][j].type()<<" "<<tmp[i][j].size() << endl;
		}
		totalnum += tmp[i].size();
		//totalnum_test += tmp_test[i].size();
	}
	cout <<"totalnum : "<< totalnum << endl;

	//Mat testdata = Mat(totalnum_test, tmp_test[0][0].cols, CV_32FC1);
	//Mat testlabel = Mat(totalnum_test, 1, CV_32SC1);
	//int count = 0;
	//for (int i = 0; i < classlist.size(); i++) {
	//	for (int j = 0; j < tmp_test[i].size(); j++) {
	//		for (int k = 0; k < tmp_test[0][0].cols; k++) {
	//			testdata.at<float>(count, k) = tmp_test[i][j].at<float>(0, k);
	//		}
	//		/*traindata.row(count) = tmp[i][j];*/
	//		testlabel.row(count) = i;
	//		count++;
	//	}
	//}

	Mat traindata = Mat(totalnum, tmp[0][0].cols, CV_32FC1);
	Mat trainlabel = Mat(totalnum, 1, CV_32SC1);
	int count = 0;
	for (int i = 0; i < classlist.size(); i++) {
		for (int j = 0; j < tmp[i].size(); j++) {
			for (int k=0; k < tmp[0][0].cols; k++) {
				traindata.at<float>(count, k) = tmp[i][j].at<float>(0, k);
			}
			/*traindata.row(count) = tmp[i][j];*/
			trainlabel.row(count) = i;
			count++;
		}
	}

	cout << "traindata" << " : " << traindata.type() << " " << traindata.size() << endl;
	cout << "trainlabel" << " : " << trainlabel.type() << " " << trainlabel.size() << endl;
	//cout << trainlabel << endl;
	//cout << traindata << endl;



	//cv::Ptr<cv::ml::SVM> svm = cv::ml::SVM::create();

	//svm->setType(cv::ml::SVM::C_SVC);

	//svm->setKernel(cv::ml::SVM::LINEAR);//RBF//LINEAR

	//svm->setTermCriteria(cv::TermCriteria(CV_TERMCRIT_ITER, 10000, FLT_EPSILON));

	////训练SVM
	//svm->train(traindata, cv::ml::ROW_SAMPLE, trainlabel);

	////保存训练好的分类器（其中包含了SVM的参数，支持向量，α和rho）
	//svm->save("model.xml");

	string modelpath = "model.xml";
	Ptr<ml::SVM> svm = Algorithm::load<ml::SVM>(modelpath);

	float TrueNum = 0;
	for (int i = 0; i < traindata.rows; i++)
	{
		int response = (int)svm->predict(traindata.row(i));
		cout << "true: "<<classlist[trainlabel.at<int>(i, 0)] << " vs. " <<" predict: " <<classlist[response] << endl;
		if (response == trainlabel.at<int>(i, 0))
		{
			TrueNum++;
		}
	}
	std::cout << "TrueNum: " << TrueNum << endl;
	std::cout << "TotalNum: " << traindata.rows << endl;
	std::cout << "Result: " << TrueNum / float(traindata.rows) * 100 << "%" << endl;


	//imshow("show", tmp[0][0]);
	//waitKey(0);
}
	
    
