#include "TEXTRecognizer.h"

TEXTRecognizer::TEXTRecognizer()
{
    _vocabulary = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
}

TEXTRecognizer::~TEXTRecognizer()
{
}

void TEXTRecognizer::initialize()
{
    vector<string> lexicon;
    initialize( lexicon );
}

void TEXTRecognizer::initialize( vector<string> lexicon )
{
    _ocrProc = cv::text::loadOCRHMMClassifierCNN("OCRBeamSearch_CNN_model_data.xml.gz");

    if( lexicon.size() > 0 ){
        cv::text::createOCRHMMTransitionsTable( _vocabulary, lexicon, _transition_p);
    }else{
        // An alternative would be to load the default generic language model
        //    (created from ispell 42869 english words list)
        std::string filename = "OCRHMM_transitions_table.xml";
        cv::FileStorage fs(filename, cv::FileStorage::READ);
        fs["transition_probabilities"] >> _transition_p;
        fs.release();
    }

    cv::Mat emission_p = cv::Mat::eye( 62, 62, CV_64FC1 );
    _txtProc = cv::text::OCRBeamSearchDecoder::create(
                cv::text::loadOCRBeamSearchClassifierCNN("OCRBeamSearch_CNN_model_data.xml.gz"),
                _vocabulary, _transition_p, emission_p, cv::text::OCR_DECODER_VITERBI, 50);
}

void TEXTRecognizer::textDecode(cv::Mat roiMat, std::string &text, float &confidence)
{
    string output;
    vector<cv::Rect> boxes;
    vector<std::string> words;
    vector<float> confidences;
    _txtProc->run( roiMat, output, &boxes, &words, &confidences, cv::text::OCR_LEVEL_WORD );

    text = words[0];
    confidence = confidences[0];
}

void TEXTRecognizer::charDecode( Mat roiMat, string &character, double &confidence )
{
    vector<int> out_classes;
    vector<double> out_confidences;
    _ocrProc->eval( roiMat, out_classes, out_confidences);

    character = _vocabulary[out_classes[0]];
    confidence = out_confidences[0];
}

