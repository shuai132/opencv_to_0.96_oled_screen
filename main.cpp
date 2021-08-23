#include <memory>
#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"
#include "RpcTask.h"

#define IMG_WIDTH 128
#define IMG_HEIGHT 64

#define CV_WINDOW_NAME "OLED"

static void printHex(const uint8_t* data, size_t size, const std::string& name = "img") {
    printf("%s: {", name.c_str());
    for (size_t i = 0; i < size; ++i) {
        printf("0x%X", data[i]);
        if (i < size - 1) {
            printf(", ");
        } else {
            printf("}\n");
        }
    }
}

int main() {
    using namespace cv;

    Mat img;
    const int binFrameSize = IMG_WIDTH / 8 * IMG_HEIGHT;
    auto binFrame = std::make_unique<uint8_t[]>(binFrameSize);
    auto clearBinFrame = [&]{ memset(binFrame.get(), 0, binFrameSize); };

    RpcTask rpcTask;
    while (true) {
        const char* imgName = "1.bmp";
        system("screencapture -m -t bmp -x 1.bmp");
        img = imread(imgName);
        if (img.empty()) break;

        cvtColor(img, img, COLOR_RGB2GRAY);
        resize(img, img, Size(128, 64), 0, 0, INTER_LINEAR);
        adaptiveThreshold(img, img, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 25, 10);

        clearBinFrame();
        for (int h = 0; h < IMG_HEIGHT; h++) {
            for (int w = 0; w < IMG_WIDTH; w++) {
                const uint8_t& imgData = img.data[h * IMG_WIDTH + w];
                uint8_t& binData = binFrame[h * IMG_WIDTH / 8 + w / 8];
                if (imgData > 0) {
                    binData |= 0b10000000 >> w % 8;
                }
            }
        }
        imshow(CV_WINDOW_NAME, img);
        rpcTask.onFrame(binFrame.get(), binFrameSize);
    }
    printf("finish!\n");
    return 0;
}
