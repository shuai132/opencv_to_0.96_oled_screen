#include <memory>
#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"
#include "RpcTask.h"

#define IMG_WIDTH 128
#define IMG_HEIGHT 64

#define CV_WINDOW_NAME "OLED"
#define SCREENSHOT_NAME "screenshot.bmp"

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

static bool enableImgShow = true;
static bool useScreenShot = false;
static bool useCap = false;
static bool useFile = false;

int main(int argc, char* argv[]) {
    using namespace cv;

    auto cap = std::make_unique<VideoCapture>();
    if (argc == 1) {
        printf("0  ScreenShot\n1  Camera\nfile name of video, gif...\n");
        return 0;
    }
    if (argc >= 2) {
        std::string cmd(argv[1]);
        if (cmd == "0") {
            useScreenShot = true;
        }
        else if (cmd == "1") {
            useCap = true;
            cap->open(0);
            if (!cap->isOpened()) {
                printf("open failed\n");
                return 0;
            }
        }
        else {
            useFile = true;
            cap->open(cmd);
        }
    }
    if (argc >= 3) {
        std::string v(argv[2]);
        enableImgShow = v == "1" || v == "true";
    }

    Mat img;
    const int binFrameSize = IMG_WIDTH / 8 * IMG_HEIGHT;
    auto binFrame = std::make_unique<uint8_t[]>(binFrameSize);
    auto clearBinFrame = [&]{ memset(binFrame.get(), 0, binFrameSize); };

    RpcTask rpcTask;
    rpcTask.waitConnect();
    while (true) {
        if (useScreenShot) {
            system("screencapture -m -t bmp -x " SCREENSHOT_NAME);
            img = imread(SCREENSHOT_NAME);
        }
        else if (useCap || useFile) {
            cap->read(img);
            waitKey(1000 / cap->get(CAP_PROP_FPS));
        }
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
        if (enableImgShow) {
            imshow(CV_WINDOW_NAME, img);
        }
        rpcTask.onFrame(binFrame.get(), binFrameSize);
    }
    printf("press Ctrl+C to exit\n");
    waitKey();
    return 0;
}
