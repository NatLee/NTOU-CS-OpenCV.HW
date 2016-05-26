#ifndef TEXTRECOGNIZER_H
#define TEXTRECOGNIZER_H
#include "opencv2/text.hpp"
#include "opencv2/core/utility.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

using namespace std;
using namespace cv;

#define LIB_DECL __declspec(dllexport)

class TEXTRecognizer
{
public:
    TEXTRecognizer();
    ~TEXTRecognizer();
    void initialize( );
    void initialize( std::vector<std::string> lexicon );

    void textDecode( Mat roiMat, string &text, float &confidence );
    void charDecode( Mat roiMat, string &character, double &confidence );

private:
    Ptr<cv::text::OCRHMMDecoder::ClassifierCallback> _ocrProc;
    Ptr<cv::text::OCRBeamSearchDecoder> _txtProc;
    string _vocabulary;
    Mat _transition_p;
};

#endif // TEXTRECOGNIZER_H
