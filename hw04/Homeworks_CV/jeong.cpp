#include "jeong.h"

Jeong::Jeong()
{

}
Jeong::~Jeong()
{
}
Jeong::Jeong(const KImageColor& icImg){
    _jRow   = icImg.Row();
    _jCol   = icImg.Col();
    _jSize  = _jRow * _jCol;
    _icPtr  = &icImg;
}
Jeong::Jeong(const KImageGray& igImg){
    _jRow   = igImg.Row();
    _jCol   = igImg.Col();
    _jSize  = _jRow * _jCol;
    _igPtr  = &igImg;
}
void Jeong::SetPtr(const KImageColor &icImg)
{
    _jRow   = icImg.Row();
    _jCol   = icImg.Col();
    _jSize  = _jRow * _jCol;
    _icPtr  = &icImg;
}

void Jeong::SetPtr(const KImageGray &igImg)
{
    _jRow   = igImg.Row();
    _jCol   = igImg.Col();
    _jSize  = _jRow * _jCol;
    _igPtr  = &igImg;
}


KImageGray Jeong::LumiContrast(const int a, const int b, const int ap, const int bp) const
{
#ifdef _DEBUG
    assert(_igPtr != nullptr);
#endif
    KImageGray igTemp = KImageGray(*_igPtr);

    for(int i=0;i<igTemp.Row();i++){
        for(int j=0;j<igTemp.Col();j++){
            if(igTemp._ppA[i][j] < a)
                igTemp._ppA[i][j] = ap;
            else if(igTemp._ppA[i][j] >= a && igTemp._ppA[i][j] <=b){
                igTemp._ppA[i][j] = (bp-ap)/(b-a) * (igTemp._ppA[i][j] - a) + ap;
            }else{
                igTemp._ppA[i][j] = bp;
            }
        }
    }
    return igTemp;
}

KImageGray Jeong::LumiContrast(const KImageGray &igImg, const int a, const int b, const int ap, const int bp) const
{
    KImageGray igTemp = KImageGray(igImg);

    for(int i=0;i<igTemp.Row();i++){
        for(int j=0;j<igTemp.Col();j++){
            if(igTemp._ppA[i][j] < a)
                igTemp._ppA[i][j] = ap;
            else if(igTemp._ppA[i][j] >= a && igTemp._ppA[i][j] <=b){
                igTemp._ppA[i][j] = (bp-ap)/(b-a) * (igTemp._ppA[i][j] - a) + ap;
            }else{
                igTemp._ppA[i][j] = bp;
            }
        }
    }
    return igTemp;
}

unsigned int Jeong::OtsuBetween() const
{
#ifdef _DEBUG
    assert(_igPtr!=nullptr);
#endif

    // get histogram
    KHisto hTmp;
    hTmp.Histogram(*_igPtr);

    // variables
    double dq1_t = (hTmp[0])/_jSize;
    double dmu1_t = 0;
    double dmu1_tp1, dmu2_tp1, dq1_tp1;
    double dVar_tp1, dVarMax=0;
    unsigned int i_optimal_t; // optimal T. 아래 for문에서는 t+1임!!


    // global mean
    double dmu_g=0;
    for(int i=0;i<256;i++){
        dmu_g += i * (hTmp[i])/_jSize;
    }
    // searching for the optimal t(actually t+1)
    for(int t=0;t<254;t++){

        dq1_tp1 = dq1_t + (hTmp[t+1])/_jSize;
        if(dq1_tp1 == 0) // 아래 dmu1_tp1에서 분모가 dq1_tp1 인데, 0이면 무한대 됨.
            dq1_tp1 = 0.00001;

        dmu1_tp1 = ((dq1_t * dmu1_t) + (t+1)*(hTmp[t+1])/_jSize) / dq1_tp1;
        dmu2_tp1 = (dmu_g - dq1_tp1*dmu1_tp1) / (1-dq1_tp1);

        dVar_tp1 = dq1_tp1*(1-dq1_tp1)*(dmu1_tp1-dmu2_tp1)*(dmu1_tp1-dmu2_tp1);
        if(dVarMax<dVar_tp1){
            dVarMax = dVar_tp1;
            i_optimal_t = t+1;
        }
        // update
        dq1_t = dq1_tp1;
        dmu1_t = dmu1_tp1;
    }

    return i_optimal_t;
}

unsigned int Jeong::Otsubetween(const KImageGray& igImg) const
{
    // get histogram
    KHisto hTmp;
    hTmp.Histogram(igImg);

    // variables
    double dq1_t = (hTmp[0])/_jSize;
    double dmu1_t = 0;
    double dmu1_tp1, dmu2_tp1, dq1_tp1;
    double dVar_tp1, dVarMax=0;
    unsigned int i_optimal_t; // optimal T. 아래 for문에서는 t+1임!!


    // global mean
    double dmu_g=0;
    for(int i=0;i<256;i++){
        dmu_g += i * (hTmp[i])/_jSize;
    }
    // searching for the optimal t(actually t+1)
    for(int t=0;t<254;t++){

        dq1_tp1 = dq1_t + (hTmp[t+1])/_jSize;
        if(dq1_tp1 == 0) // 아래 dmu1_tp1에서 분모가 dq1_tp1 인데, 0이면 무한대 됨.
            dq1_tp1 = 0.00001;

        dmu1_tp1 = ((dq1_t * dmu1_t) + (t+1)*(hTmp[t+1])/_jSize) / dq1_tp1;
        dmu2_tp1 = (dmu_g - dq1_tp1*dmu1_tp1) / (1-dq1_tp1);

        dVar_tp1 = dq1_tp1*(1-dq1_tp1)*(dmu1_tp1-dmu2_tp1)*(dmu1_tp1-dmu2_tp1);
        if(dVarMax<dVar_tp1){
            dVarMax = dVar_tp1;
            i_optimal_t = t+1;
        }
        // update
        dq1_t = dq1_tp1;
        dmu1_t = dmu1_tp1;
    }

    return i_optimal_t;
}

KImageGray Jeong::Dilation(const KImageGray& igImg) const
{
    KImageGray  igTemp = igImg;

    // 무조건 흰색이 foreground
    for(int i=1,ii=this->Row()-2;ii;i++, ii--){
        for(int j=1, jj=this->Col()-2; jj; j++, jj--){
            if(igImg._ppA[i][j]==0) // 0인데, 주면에 있으면 늘려
                if(igImg._ppA[i-1][j-1]|| igImg._ppA[i-1][j]||igImg._ppA[i][j+1]||igImg._ppA[i][j-1]||
                        igImg._ppA[i][j+1]||igImg._ppA[i+1][j-1]||igImg._ppA[i+1][j]||igImg._ppA[i+1][j+1])
                    igTemp._ppA[i][j] = 255;
        }
    }
    return igTemp;
}

KImageGray Jeong::Erosion(const KImageGray& igImg) const
{
    KImageGray  igTemp = igImg;

    // 무조건 흰색이 foreground
    for(int i=1,ii=this->Row()-2;ii;i++, ii--){
        for(int j=1, jj=this->Col()-2; jj; j++, jj--){
            if(igImg._ppA[i][j]) // 255인데, 주변에 있으면 줄여
                if(igImg._ppA[i-1][j-1]==0|| igImg._ppA[i-1][j]==0||igImg._ppA[i][j+1]==0||igImg._ppA[i][j-1]==0||
                        igImg._ppA[i][j+1]==0||igImg._ppA[i+1][j-1]==0||igImg._ppA[i+1][j]==0||igImg._ppA[i+1][j+1]==0)
                    igTemp._ppA[i][j] = 0;
        }
    }

    return igTemp;
}

KImageGray Jeong::Opening(const KImageGray& igImg) const
{
    KImageGray  igTemp = igImg;

    // 1. Erosion
    igTemp = Erosion(igTemp);

    // 2. Dilation
    igTemp = Dilation(igTemp);

    return igTemp;
}

KImageGray Jeong::Closing(const KImageGray& igImg) const
{
    KImageGray  igTemp = igImg;

    // 1. Dilation
    igTemp = Dilation(igTemp);

    // 2. Erosion
    igTemp = Erosion(igTemp);

    return igTemp;
}

KImageWord Jeong::Labeling(const KImageGray &igImg, KImageColor *icpColor) const
{

    int             nRow = igImg.Row();
    int             nCol = igImg.Col();
    unsigned int    uCur, uUp, uLeft;
    unsigned short  wUpColor, wLeftColor, wIdx = 0;

    //init.
    KImageWord awColor(nRow,nCol);
    std::vector<std::pair<unsigned short, unsigned short>> diff;
    if(icpColor)
        icpColor->Create(nRow,nCol);

    // 0. 가장자리 0으로 만들어주기
    for(int i=0; i<nRow; i++){
        if(i==0 || i==nRow-1)
            for(int j=0;j<nCol;j++){
                awColor[i][j] = 0;
                igImg._ppA[i][j] = 0; // ??? 어떻게 이게 되지?
            }
        else{
            awColor[i][0] = 0;
            awColor[i][nCol-1] = 0;
            igImg._ppA[i][0] = 0; // ???
            igImg._ppA[i][nCol-1] = 0;
        }
    }

    // 1. 살펴보기
    for(int i=1, ii=nRow-2; ii; i++, ii--)
        for(int j=1, jj=nCol-2; jj; j++, jj--){
            uCur = igImg._ppA[i][j];
            uUp = igImg._ppA[i-1][j];
            uLeft = igImg._ppA[i][j-1];
            wUpColor = awColor[i-1][j];
            wLeftColor = awColor[i][j-1];

            if(uUp && uLeft && wUpColor!=wLeftColor){
                diff.push_back(std::make_pair(wUpColor,wLeftColor));
            }

            if(uCur){ // 현재가 1인 상태
                if(uUp||uLeft){ // 위, 좌 하나라도 있으면
                    if(uUp==0)
                        awColor[i][j] = wLeftColor;
                    else{
                        awColor[i][j] = wUpColor;
                    }
                }else{
                    awColor[i][j] = wIdx;
                    wIdx++;
                }
            }
        }


    // 2. Label 같은애들 다시 묶기
    for(int i=0; i<diff.size()-1; i++){
        unsigned short A = diff[i].first;
        unsigned short B = diff[i].second;
        if(B > A)
            for(int j=i+1; j<diff.size()-1; j++){
                unsigned short C = diff[j].first;
                unsigned short D = diff[j].second;
                if(C == B)
                    diff[j].first = A;
                if(D == B)
                    diff[j].second = A;
            }
    }

    for(int k=0; k<diff.size(); k++){
        unsigned short A = (diff[k].first < diff[k].second)?diff[k].first : diff[k].second;
        unsigned short B = (diff[k].first > diff[k].second)?diff[k].first : diff[k].second;

        for(int i=1, ii=nRow-1; ii; i++, ii--)
            for(int j=1, jj=nCol-1; jj; j++, jj--)
                if(awColor[i][j] == B)
                    awColor[i][j] = A;
    }


    // 3. 색칠하기
    //결과를 영상에 저장

        for(int i=0, ii=nRow; ii; i++, ii--)
            for(int j=0, jj=nCol; jj; j++, jj--){
                if(igImg._ppA[i][j] == 0){
                    (*icpColor)[i][j].r = 0;
                    (*icpColor)[i][j].g = 0;
                    (*icpColor)[i][j].b = 0;
                    continue;
                }
                switch(awColor[i][j]%9)
                {
                case 0:
                    (*icpColor)[i][j].r = 255;
                    break;
                case 1:
                    (*icpColor)[i][j].g = 255;
                    break;
                case 2:
                    (*icpColor)[i][j].b = 255;
                    break;
                case 3:
                    (*icpColor)[i][j].r = 100;
                    break;
                case 4:
                    (*icpColor)[i][j].g = 100;
                    break;
                case 5:
                    (*icpColor)[i][j].b = 100;
                    break;
                case 6:
                    (*icpColor)[i][j].r = 255;
                    (*icpColor)[i][j].g = 255;
                    break;
                case 7:
                    (*icpColor)[i][j].r = 255;
                    (*icpColor)[i][j].b = 255;
                    break;
                case 8:
                    (*icpColor)[i][j].g = 255;
                    (*icpColor)[i][j].b = 255;
                    break;
                }
            }
        return awColor;
}

void Jeong::HistoEqualize(const KImageColor &icMain, KImageColor& icOutput) const
{
    int         nRow = icMain.Row();
    int         nCol = icMain.Col();
    int         N    = nRow * nCol;
    double      dR=0, dG=0, dB=0, dpR[256]={0,}, dpG[256]={0,}, dpB[256]={0,};

    KImageGray R(nRow,nCol), G(nRow,nCol), B(nRow,nCol);
    for(int i=0, ii=nRow; ii; i++, ii--)
        for(int j=0, jj=nCol; jj; j++, jj--){
            R._ppA[i][j] = icMain._ppA[i][j].r;
            G._ppA[i][j] = icMain._ppA[i][j].g;
            B._ppA[i][j] = icMain._ppA[i][j].b;
        }
    KHisto hTmpR, hTmpG, hTmpB;
    hTmpR.Histogram(R);
    hTmpG.Histogram(G);
    hTmpB.Histogram(B);

    for(int i=0, ii=256; ii; i++, ii--){
        dR += hTmpR[i];
        dG += hTmpG[i];
        dB += hTmpB[i];
        dpR[i] = (double)(dR/N);
        dpG[i] = (double)(dG/N);
        dpB[i] = (double)(dB/N);
    }

    for(int i=0, ii=nRow; ii; i++, ii--)
        for(int j=0, jj=nCol; jj; j++, jj--){
            icOutput._ppA[i][j].r = dpR[R._ppA[i][j]] * 255;
            icOutput._ppA[i][j].g = dpG[G._ppA[i][j]] * 255;
            icOutput._ppA[i][j].b = dpB[B._ppA[i][j]] * 255;
        }
}

void Jeong::HistoMatch(const KImageColor &icOrigin, const KImageColor &icTarget, KImageColor &icOutput) const
{
    int             nRow = icOrigin.Row();
    int             nCol = icOrigin.Col();
    int             N    = nRow * nCol;
    double          dOriR=0, dOriG=0, dOriB=0, dTarR=0, dTarG=0, dTarB=0, dpOriR[256]={0,}, dpOriG[256]={0,}, dpOriB[256]={0,}, dpTarR[256]={0,}, dpTarG[256]={0,}, dpTarB[256]={0,};


    KImageGray      igOriR(nRow,nCol), igOriG(nRow,nCol), igOriB(nRow,nCol), igTarR(nRow,nCol), igTarG(nRow,nCol), igTarB(nRow,nCol);
    KHisto          hOriR, hOriG, hOriB, hTarR, hTarG, hTarB;

    for(int i=0, ii=nRow; ii; i++, ii--)
        for(int j=0, jj=nCol; jj; j++, jj--){
            igOriR._ppA[i][j] = icOrigin._ppA[i][j].r;  igOriG._ppA[i][j] = icOrigin._ppA[i][j].g;  igOriB._ppA[i][j] = icOrigin._ppA[i][j].b;
            igTarR._ppA[i][j] = icTarget._ppA[i][j].r;  igTarG._ppA[i][j] = icTarget._ppA[i][j].g;  igTarB._ppA[i][j] = icTarget._ppA[i][j].b;
        }
    hOriR.Histogram(igOriR);    hOriG.Histogram(igOriG);    hOriB.Histogram(igOriB);
    hTarR.Histogram(igTarR);    hTarG.Histogram(igTarG);    hTarB.Histogram(igTarB);

    for(int i=0, ii=256; ii; i++, ii--){
        dOriR += hOriR[i];      dOriG += hOriG[i];      dOriB += hOriB[i];
        dTarR += hTarR[i];      dTarG += hTarG[i];      dTarB += hTarB[i];
        dpOriR[i] = dOriR/N;    dpOriG[i] = dOriG/N;    dpOriB[i] = dOriB/N;
        dpTarR[i] = dTarR/N;    dpTarG[i] = dTarG/N;    dpTarB[i] = dTarB/N;
    }

    // r을 s로 대응시키기 (dPOriR, G, B를 재활용해서 그냥 담자)
    for(int i=0, ii=256; ii; i++, ii--){
        double          dOffsetMinR = 1000000, dOffsetMinG = 1000000, dOffsetMinB = 1000000;
        unsigned int    uMinIntR=0, uMinIntG=0, uMinIntB=0;
        for(int j=0, jj=256; jj; j++, jj--){
            if(dOffsetMinR>std::abs(dpOriR[i]-dpTarR[j])){
                dOffsetMinR = std::abs(dpOriR[i]-dpTarR[j]);
                uMinIntR = j;
            }
            if(dOffsetMinG>std::abs(dpOriG[i]-dpTarG[j])){
                dOffsetMinG = std::abs(dpOriG[i]-dpTarG[j]);
                uMinIntG = j;
            }
            if(dOffsetMinB>std::abs(dpOriB[i]-dpTarB[j])){
                dOffsetMinB = std::abs(dpOriB[i]-dpTarB[j]);
                uMinIntB = j;
            }
        }
        dpOriR[i] = (double)uMinIntR;
        dpOriG[i] = (double)uMinIntG;
        dpOriB[i] = (double)uMinIntB;
    }


    // 픽셀값 매칭된 값으로 바꿔주기
    for(int i=0, ii=nRow; ii; i++, ii--)
        for(int j=0, jj=nCol; jj; j++, jj--){
            icOutput._ppA[i][j].r = (unsigned int)dpOriR[icOrigin._ppA[i][j].r];
            icOutput._ppA[i][j].g = (unsigned int)dpOriG[icOrigin._ppA[i][j].g];
            icOutput._ppA[i][j].b = (unsigned int)dpOriB[icOrigin._ppA[i][j].b];
        }
}

int Jeong::getRandomNumber(int min, int max) const
{
    static const double fraction = 1.0 / (RAND_MAX + 1.0); // static used for efficiency, so we only calculate this value once
    // evenly distribute the random number across our range
    return min + static_cast<int>((max - min + 1) * (std::rand() * fraction));
}

void Jeong::SaltPepperNoise(const KImageColor icIn, KImageColor &icOut, double dPercent) const
{
    unsigned int nRow = icIn.Row(), nCol = icIn.Col();

    dPercent *= 0.01;
    double threshold = 500 * dPercent;
    int randomNum;

    for(int i=0, ii=nRow; ii; i++, ii--)
        for(int j=0, jj=nCol; jj; j++, jj--){
            randomNum = this->getRandomNumber(0,1000);
            if(randomNum<threshold) {
                icOut._ppA[i][j].r = 0;
                icOut._ppA[i][j].g = 0;
                icOut._ppA[i][j].b = 0;
            }else if(randomNum > 1000 - threshold){
                icOut._ppA[i][j].r = 255;
                icOut._ppA[i][j].g = 255;
                icOut._ppA[i][j].b = 255;
            }else{
                icOut._ppA[i][j].r = icIn._ppA[i][j].r;
                icOut._ppA[i][j].g = icIn._ppA[i][j].g;
                icOut._ppA[i][j].b = icIn._ppA[i][j].b;
            }
        }
}


void Jeong::GaussianNoise(const KImageColor icIn, KImageColor &icOut, double dMean, double dVar) const
{
    unsigned int    nRow = icIn.Row(), nCol = icIn.Col(), nSize = icIn.Size();
    double          dNewR = 0, dNewG = 0, dNewB = 0;

    // Gaussian 하나 만듬
    KGaussian gaussian(dMean, dVar);
    gaussian.OnRandom(nSize);

    // Noise 분배
    for(int i=0, ii=nRow; ii; i++, ii--)
        for(int j=0, jj=nCol; jj; j++, jj--){

            dNewR = icIn._ppA[i][j].r + gaussian.Generate();
            dNewG = icIn._ppA[i][j].g + gaussian.Generate();
            dNewB = icIn._ppA[i][j].b + gaussian.Generate();

            if(dNewR>255) icOut._ppA[i][j].r = 255;
            else if(dNewR<0) icOut._ppA[i][j].r = 0;
            else icOut._ppA[i][j].r = dNewR;

            if(dNewG>255) icOut._ppA[i][j].g = 255;
            else if(dNewG<0) icOut._ppA[i][j].g = 0;
            else icOut._ppA[i][j].g = dNewG;

            if(dNewB>255) icOut._ppA[i][j].b = 255;
            else if(dNewB<0) icOut._ppA[i][j].b = 0;
            else icOut._ppA[i][j].b = dNewB;

        }

}

void Jeong::MeanFilter(const KImageColor icIn, KImageColor &icOut, int kernelsize) const
{
    // 이렇게 const 인자 붙이면 이 함수 내에서는 icIn 바꿔도 바깥의 icIn은 바뀌지 않는다.


    // kernelsize shold be odd number
    #ifdef _DEBUG
        assert(kernelsize%2==1);
    #endif
        unsigned int    nRow = icIn.Row(), nCol = icIn.Col();
        int             nHalf = kernelsize/2;
        int             nSx = nHalf, nSy = nHalf, nEx = nCol-nHalf, nEy = nRow-nHalf;
        int*            kernel = new int[kernelsize];

        for(int i=0; i<kernelsize; i++)
            kernel[i] = 1;

        // convolution
        for(int i=nSy; i<nEy; i++)
            for(int j=nSx; j<nEx; j++){
                double dR=0, dB=0, dG=0;
                for(int r = -nHalf, rr=0; r<=nHalf; r++, rr++){
                    dR += icIn._ppA[i+r][j].r * kernel[rr];
                    dG += icIn._ppA[i+r][j].g * kernel[rr];
                    dB += icIn._ppA[i+r][j].b * kernel[rr];
                }
                icIn._ppA[i][j].r = dR/kernelsize < 255 ? dR/kernelsize : 255;
                icIn._ppA[i][j].g = dG/kernelsize < 255 ? dG/kernelsize : 255;
                icIn._ppA[i][j].b = dB/kernelsize < 255 ? dB/kernelsize : 255;
            }
        for(int i=nSy; i<nEy; i++)
            for(int j=nSx; j<nEx; j++){
                double dR=0, dB=0, dG=0;
                for(int r = -nHalf, rr=0; r<=nHalf; r++, rr++){
                    dR += icIn._ppA[i][j+r].r * kernel[rr];
                    dG += icIn._ppA[i][j+r].g * kernel[rr];
                    dB += icIn._ppA[i][j+r].b * kernel[rr];
                }
                icOut._ppA[i][j].r = dR/kernelsize < 255 ? dR/kernelsize : 255;
                icOut._ppA[i][j].g = dG/kernelsize < 255 ? dG/kernelsize : 255;
                icOut._ppA[i][j].b = dB/kernelsize < 255 ? dB/kernelsize : 255;
            }

        delete[] kernel;
}

void Jeong::GaussianFilter(const KImageColor icIn, KImageColor &icOut, double dSigma) const
{
    // 이렇게 const 인자 붙이면 이 함수 내에서는 icIn 바꿔도 바깥의 icIn은 바뀌지 않는다.

    // kernelsize shold be odd number
    #ifdef _DEBUG
        assert(kernelsize%2==1);
    #endif
        unsigned int    nRow = icIn.Row(), nCol = icIn.Col();
        int             nHalf = (int)(3.0*dSigma + 0.3);
        int             nSx = nHalf, nSy = nHalf, nEx = nCol-nHalf, nEy = nRow-nHalf;
        KMatrix         mMask(nHalf*2 + 1, nHalf*2 + 1);
        double          dScale = 0.0, dSigma2 = 2.0*_SQR(dSigma);
        double          dConst = 1.0/_2PI/_SQR(dSigma);

        for(int r = -nHalf, i=0; r <= nHalf; r++, i++)
            for(int c = -nHalf, j=0; c <= nHalf; c++, j++){
                mMask[i][j] = dConst * exp(-(r*r + c*c)/dSigma2);
                dScale      += mMask[i][j];
            }
        mMask /= dScale; // 이거 operator 정의되어있음. 원소 하나하나 dScale 나눈다.

        // Convolution
        for(int i=nSy; i<nEy; i++)
            for(int j=nSx; j<nEx; j++){
                for(int r=-nHalf, rr=0; r<=nHalf; r++, rr++)
                    for(int c=-nHalf, cc=0; c<=nHalf; c++, cc++){
                        icOut[i][j].r += icIn[i+r][j+c].r * mMask[rr][cc];
                        icOut[i][j].g += icIn[i+r][j+c].g * mMask[rr][cc];
                        icOut[i][j].b += icIn[i+r][j+c].b * mMask[rr][cc];
                    }
            }

}

void Jeong::GaussianFilter2(const KImageColor icIn, KImageColor &icOut, double dSigma) const
{
    // 이렇게 const 인자 붙이면 이 함수 내에서는 icIn 바꿔도 바깥의 icIn은 바뀌지 않는다.

    // kernelsize shold be odd number
    #ifdef _DEBUG
        assert(kernelsize%2==1);
    #endif
        unsigned int    nRow = icIn.Row(), nCol = icIn.Col();
        int             nHalf = (int)(3.0*dSigma + 0.3);
        int             nSx = nHalf, nSy = nHalf, nEx = nCol-nHalf, nEy = nRow-nHalf;
        double*         kernel = new double[nHalf*2+1];
        double          dScale = 0.0, dSigma2 = 2.0*_SQR(dSigma);
        double          dConst = 1.0/_2PI/_SQR(dSigma);

        for(int i=-nHalf, ii=0; i<=nHalf; i++, ii++){
            kernel[ii] = dConst * exp(-(i*i)/dSigma2);
            dScale += kernel[ii];
        }
        for(int i=-nHalf, ii=0; i<=nHalf; i++, ii++){
            kernel[ii] /= dScale;
        }


        // x Convolution
        for(int i=nSy; i<nEy; i++)
            for(int j=nSx; j<nEx; j++){
                double dRsum = 0, dGsum = 0, dBsum = 0;
                for(int r=-nHalf, rr=0; r<=nHalf; r++, rr++){
                    dRsum += icIn[i+r][j].r * kernel[rr];
                    dGsum += icIn[i+r][j].g * kernel[rr];
                    dBsum += icIn[i+r][j].b * kernel[rr];
                }
                icIn[i][j].r = dRsum;
                icIn[i][j].g = dGsum;
                icIn[i][j].b = dBsum;
            }

        // y Convolution
        for(int i=nSy; i<nEy; i++)
            for(int j=nSx; j<nEx; j++){
                for(int r=-nHalf, rr=0; r<=nHalf; r++, rr++){
                    icOut[i][j].r += icIn[i][j+r].r * kernel[rr];
                    icOut[i][j].g += icIn[i][j+r].g * kernel[rr];
                    icOut[i][j].b += icIn[i][j+r].b * kernel[rr];
                }
            }
}

