#include <Windows.h>
#include <Xinput.h>
#include <fstream>
#include <iostream>
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

using namespace std;
using namespace cv;

Mat hwnd2mat(HWND hwnd) {
  HDC hwindowDC, hwindowCompatibleDC;

  int height, width, srcheight, srcwidth;
  HBITMAP hbwindow;
  Mat src;
  BITMAPINFOHEADER bi;

  hwindowDC = GetDC(hwnd);
  hwindowCompatibleDC = CreateCompatibleDC(hwindowDC);
  SetStretchBltMode(hwindowCompatibleDC, COLORONCOLOR);

  RECT windowsize;  // get the height and width of the screen
  GetClientRect(hwnd, &windowsize);

  srcheight = windowsize.bottom;
  srcwidth = windowsize.right;
  height = windowsize.bottom / 1;
  width = windowsize.right / 1;

  src.create(height, width, CV_8UC4);

  // create a bitmap
  hbwindow = CreateCompatibleBitmap(hwindowDC, width, height);
  bi.biSize = sizeof(BITMAPINFOHEADER);
  bi.biWidth = width;
  bi.biHeight = -height;
  bi.biPlanes = 1;
  bi.biBitCount = 32;
  bi.biCompression = BI_RGB;
  bi.biSizeImage = 0;
  bi.biXPelsPerMeter = 0;
  bi.biYPelsPerMeter = 0;
  bi.biClrUsed = 0;
  bi.biClrImportant = 0;

  // use the previously created device context with the bitmap
  SelectObject(hwindowCompatibleDC, hbwindow);
  // copy from the window device context to the bitmap device context
  StretchBlt(hwindowCompatibleDC, 0, 0, width, height, hwindowDC, 0, 0,
             srcwidth, srcheight, SRCCOPY);
  GetDIBits(hwindowCompatibleDC, hbwindow, 0, height, src.data,
            (BITMAPINFO *)&bi, DIB_RGB_COLORS);

  DeleteObject(hbwindow);
  DeleteDC(hwindowCompatibleDC);
  ReleaseDC(hwnd, hwindowDC);

  return src;
}

bool checkA() {
  XINPUT_STATE state;
  memset(&state, 0, sizeof(XINPUT_STATE));
  XInputGetState(0, &state);
  if (state.Gamepad.wButtons & XINPUT_GAMEPAD_A) {
    return true;
  } else {
    return false;
  }
}

int printControls() {
  XINPUT_STATE state;
  memset(&state, 0, sizeof(XINPUT_STATE));
  XInputGetState(0, &state);

  float LX = state.Gamepad.sThumbLX;
  float LY = state.Gamepad.sThumbLY;

  const int INPUT_DEADZONE = 10000;

  float magnitude = sqrt(LX * LX + LY * LY);
  // determine the direction the controller is pushed
  float normalizedLX = LX / magnitude;
  float normalizedLY = LY / magnitude;

  float normalizedMagnitude = 0;
  // check if the controller is outside a circular dead zone
  if (magnitude > INPUT_DEADZONE) {
    if (LX > 0) {
      // cout << "RIGHT " << LX / 32767 << "\n";
      cout << "1\n";
      return 1;
    } else {
      // cout << "LEFT " << LX / 32768 << "\n";
      cout << "-1\n";
      return -1;
    }
  } else {
    cout << "0\n";
    return 0;
  }
}

int main() {
  // try to load existing training data
  /* 2d array like this:
   * -----screen------
   * [255, 240, 12...]
   * [233, 220, 20...]
   */
  vector<uchar> trainingData;
  ofstream data;
  data.open("sample/screen.peach", ios::binary | ios::app);

  LPCTSTR windowName =
      "Dolphin 5.0 | JIT64 DC | OpenGL | HLE | FPS: 60 - VPS: 60 - 100%";  // terrible
                                                                           // hack
                                                                           // since
                                                                           // window
                                                                           // title
                                                                           // can
                                                                           // change
  HWND dolphinWindow = FindWindowEx(0, 0, 0, windowName);
  if (!dolphinWindow) {
    return -1;
  }
  namedWindow("output", WINDOW_KEEPRATIO);  // keep ratio not working...

  // MASK
  // crop to get region of interest
  Rect roi;
  roi.x = 0;
  roi.y = 15;
  roi.width = 60;
  roi.height = 60 - roi.y;

  int key = 0;
  int count = 0;
  bool astate = false;
  while (key != 27) {
    bool prevAstate = astate;
    astate = checkA();
    if (!astate) {
      if (prevAstate) {
        break;
      }
      continue;
    } else {
      Mat screen = hwnd2mat(dolphinWindow);
      float control = printControls();
      count++;

      cvtColor(screen, screen, CV_BGR2GRAY);
      resize(screen, screen, Size(60, 60), 0, 0);

      // bitwise_and(screen, mask, screen);
      // crop using region of interest
      Mat screen2 = screen(roi);

      cout << screen2.cols << "\n";
      cout << screen2.rows << "\n";

      vector<uchar> screenArray;
      if (screen2.isContinuous())
        screenArray.assign(screen2.datastart, screen2.dataend);
      else {
        // not continuous in memory
        for (int i = 0; i < screen2.rows; ++i)
          screenArray.insert(screenArray.end(), screen2.ptr<uchar>(i),
                             screen2.ptr<uchar>(i) + screen2.cols);
      }
      trainingData.insert(trainingData.end(), screenArray.begin(),
                          screenArray.end());

      imshow("output", screen2);

      // Write data to disk every 500 "frames"
      if (count % 500 == 0 && count > 0) {
        int size = trainingData.size();
        data.write((const char *)&size, sizeof(int));
        data.write((const char *)&trainingData[0], size * sizeof(uchar));
        cout << "Wrote to file!\n";
        trainingData.clear();
      }

      key = waitKey(500);  // reduce fps by waiting 500 ms on loop
    }
  }

  int size = trainingData.size();
  data.write((const char *)&size, sizeof(int));
  data.write((const char *)&trainingData[0], size * sizeof(uchar));
  cout << "Final write\n"
       << "Wrote " << count << " frames!\n";
  data.close();
  cout << "Press enter to exit";
  std::cin.ignore();
}