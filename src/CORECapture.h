#pragma once

#include "opencv2/opencv.hpp"
#include "CORETimer.h"
#include <memory>
#include <thread>
#include <math.h>
#include <atomic>
#include <mutex>

using namespace cv;
using namespace std;

namespace CORE {
    static const int DEFAULT = -1;
    enum fileInputType {
        VIDEO,
        SINGLE_IMAGE,
        MULTI_IMAGE
    };
    class CORECapture {
    public:
        class captureSettings {
        public:
            int FPS, contrast, saturation, exposure, brightness;
            int resolution[2];
            captureSettings(int _width, int _height, int _FPS, int _brightness, int _contrast, int _saturation,
                            int _exposure);
            captureSettings(int _resolution[2], int _FPS, int _brightness, int _contrast, int _saturation,
                            int _exposure);
        };
        CORECapture(int deviceNumber, captureSettings settings, bool manualImageMode = false, bool useThreading = true);
        CORECapture(int deviceNumber, int * resolution, bool manualImageMode = false, bool useThreading = true);
        CORECapture(int deviceNumber, int width, int height, bool manualImageMode = false, bool useThreading = true);
        virtual ~CORECapture();
        pair<Mat, time_t> GetFrame();
        bool NewFrameReady();
        void WaitForNewFrame(bool wait = true);
        bool SetFPS(int FPS);
        bool SetResolution(int resolution[2]);
        bool SetResolution(int width, int height);
        bool SetResolutionWidth(int width);
        bool SetResolutionHeight(int height);
        bool SetBrightness(int brightness);
        bool SetContrast(int contrast);
        bool SetSaturation(int saturation);
        bool SetExposure(int exposure);
        bool SetCameraSettings(captureSettings settings);
        void SetCrop(int X, int Y, int width, int height);
        void DisableCrop(bool disableCrop = true);
        double GetFPS();
        double GetRealFPS();
        double GetResolutionWidth();
        double GetResolutionHeight();
        double GetBrightness();
        double GetContrast();
        double GetSaturation();
        double GetExposure();
        captureSettings GetCameraSettings();
        void UseFileInput(bool manualImageMode);
        void SetFileInputLocation(String path, fileInputType inputType);
        bool IsOpen();
        void Close();
    private:
        VideoCapture * m_camera;
        captureSettings m_settings;
        String m_inputFilePath;
        fileInputType m_inputType;
        mutex camera;
        time_t m_lastTimestamp;
        atomic<double> m_fps;
        pair<Mat, time_t> m_frame;
        int m_frameCount;
        Rect * m_crop;
        bool m_runCaptureThread, m_newFrameReady, m_isOpen, m_useCrop, m_manualImageMode, m_waitForNew;
        int m_deviceNumber;
        thread m_initCameraThread, m_cameraCaptureThread;
        void InitializeCamera(int deviceNumber);
        void CameraCaptureThread();
        void Init();
    };
}
