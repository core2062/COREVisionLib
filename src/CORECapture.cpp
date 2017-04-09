#include "CORECapture.h"

using namespace CORE;
using namespace cv;
using namespace std;

CORECapture::captureSettings::captureSettings(int _width, int _height, int _FPS, int _brightness, int _contrast,
                                              int _saturation, int _exposure) :
        FPS(_FPS), brightness(_brightness), contrast(_contrast), saturation(_saturation), exposure(_exposure) {
    resolution[0] = _width;
    resolution[1] = _height;
}

CORECapture::captureSettings::captureSettings(int *_resolution, int _FPS, int _brightness, int _contrast,
                                              int _saturation, int _exposure) :
        FPS(_FPS), brightness(_brightness), contrast(_contrast), saturation(_saturation), exposure(_exposure) {
    resolution[0] = _resolution[0];
    resolution[1] = _resolution[1];
}

void CORECapture::Init() {
    m_runCaptureThread = true;
    m_newFrameReady = false;
    m_useCrop = false;
    m_isOpen = false;
    m_frameCount = 0;
    m_frame.first = Mat(m_settings.resolution[1], m_settings.resolution[0], CV_8UC1);
    m_frame.first.setTo(Scalar(255));
    m_lastTimestamp = time(0);
    m_frame.second = time(0);
    if(!m_manualImageMode) {
        cout << "Initializing Camera" << endl;
        //m_initCameraThread = thread(&CORECapture::InitializeCamera, this, deviceNumber);
        InitializeCamera(m_deviceNumber);
    }
}

CORECapture::CORECapture(int deviceNumber, CORECapture::captureSettings settings, bool manualImageMode,
                         bool useThreading) : m_settings(settings), m_manualImageMode(manualImageMode),
                                              m_deviceNumber(deviceNumber) {
    Init();
}

CORECapture::CORECapture(int deviceNumber, int * resolution, bool manualImageMode, bool useThreading) :
        m_settings(resolution, -1, -1, -1, -1, -1), m_manualImageMode(manualImageMode),  m_deviceNumber(deviceNumber)  {
    Init();
}

CORECapture::CORECapture(int deviceNumber, int width, int height, bool manualImageMode, bool useThreading) :
        m_settings(width, height, -1, -1, -1, -1, -1), m_manualImageMode(manualImageMode),
        m_deviceNumber(deviceNumber)  {
    Init();
}

pair<Mat, time_t> CORECapture::GetFrame() {
    if(m_manualImageMode) {
        auto image = imread(m_inputFilePath, CV_LOAD_IMAGE_COLOR);
        if(!image.data) {
            cout << "Error reading manual image!" << endl;
        }
        pair<Mat, time_t> frame;
        frame.first = image;
        frame.second = time(0);
        return frame;
    } else {
        if(m_waitForNew) {
            while(!m_newFrameReady) {
                //TODO: Something here
            }
        }
        m_newFrameReady = false;
        camera.lock();
        auto frame = m_frame;
        camera.unlock();
        return frame;
    }
}

bool CORECapture::NewFrameReady() {
    return m_newFrameReady;
}

void CORECapture::WaitForNewFrame(bool wait) {
    m_waitForNew = wait;
}

bool CORECapture::SetFPS(int FPS) {
    m_settings.FPS = FPS;
    m_camera->set(CAP_PROP_FPS, m_settings.FPS);
    return m_camera->get(CAP_PROP_FPS) == m_settings.FPS;
}

bool CORECapture::SetResolution(int *resolution) {
    m_settings.resolution[0] = resolution[0];
    m_settings.resolution[1] = resolution[1];
    m_camera->set(CAP_PROP_FRAME_WIDTH, m_settings.resolution[0]);
    m_camera->set(CAP_PROP_FRAME_HEIGHT, m_settings.resolution[1]);
    return m_camera->get(CAP_PROP_FRAME_WIDTH) == m_settings.resolution[0] &&
           m_camera->get(CAP_PROP_FRAME_HEIGHT) == m_settings.resolution[1];
}

bool CORECapture::SetResolution(int width, int height) {
    m_settings.resolution[0] = width;
    m_settings.resolution[1] = height;
    m_camera->set(CAP_PROP_FRAME_WIDTH, m_settings.resolution[0]);
    m_camera->set(CAP_PROP_FRAME_HEIGHT, m_settings.resolution[1]);
    return m_camera->get(CAP_PROP_FRAME_WIDTH) == m_settings.resolution[0] &&
           m_camera->get(CAP_PROP_FRAME_HEIGHT) == m_settings.resolution[1];
}

bool CORECapture::SetResolutionWidth(int width) {
    m_settings.resolution[0] = width;
    m_camera->set(CAP_PROP_FRAME_WIDTH, m_settings.resolution[0]);
    return m_camera->get(CAP_PROP_FRAME_WIDTH) == m_settings.resolution[0];
}

bool CORECapture::SetResolutionHeight(int resolutionY) {
    m_settings.resolution[1] = resolutionY;
    m_camera->set(CAP_PROP_FRAME_HEIGHT, m_settings.resolution[1]);
    return m_camera->get(CAP_PROP_FRAME_HEIGHT) == m_settings.resolution[1];
}

bool CORECapture::SetBrightness(int brightness) {
    m_settings.brightness = brightness;
    if(!m_manualImageMode) {
        m_camera->set(CAP_PROP_BRIGHTNESS, m_settings.brightness);
        return m_camera->get(CAP_PROP_BRIGHTNESS) == m_settings.brightness;
    } else {
        return true;
    }
}

bool CORECapture::SetContrast(int contrast) {
    m_settings.contrast = contrast;
    m_camera->set(CAP_PROP_CONTRAST, m_settings.contrast);
    return m_camera->get(CAP_PROP_CONTRAST) == m_settings.contrast;
}

bool CORECapture::SetSaturation(int saturation) {
    m_settings.saturation = saturation;
    m_camera->set(CAP_PROP_SATURATION, m_settings.saturation);
    return m_camera->get(CAP_PROP_SATURATION) == m_settings.saturation;
}

bool CORECapture::SetExposure(int exposure) {
    m_settings.exposure = exposure;
    m_camera->set(CAP_PROP_EXPOSURE, m_settings.exposure);
    return m_camera->get(CAP_PROP_EXPOSURE) == m_settings.exposure;
}

bool CORECapture::SetCameraSettings(CORECapture::captureSettings settings) {
    m_settings = settings;
    bool error = false;
    if(m_settings.resolution[0] != -1 && m_settings.resolution[1] != -1) {
        m_camera->set(CAP_PROP_FRAME_WIDTH, m_settings.resolution[0]);
        m_camera->set(CAP_PROP_FRAME_HEIGHT, m_settings.resolution[1]);
        if(m_camera->get(CAP_PROP_FRAME_WIDTH) != m_settings.resolution[0]
           || m_camera->get(CAP_PROP_FRAME_HEIGHT) != m_settings.resolution[1]) {
            cout << "Error setting resolution!" << endl;
            error = true;
        }
    }
    if(m_settings.FPS != -1) {
        m_camera->set(CAP_PROP_FPS, m_settings.FPS);
        if(m_camera->get(CAP_PROP_FPS) != m_settings.FPS) {
            cout << "Error setting FPS!" << endl;
            error = true;
        }
    }
    if(m_settings.brightness != -1) {
        m_camera->set(CAP_PROP_BRIGHTNESS, m_settings.brightness);
        if(m_camera->get(CAP_PROP_BRIGHTNESS) != m_settings.brightness) {
            cout << "Error setting brightness!" << endl;
            error = true;
        }
    }
    if(m_settings.contrast != -1) {
        m_camera->set(CAP_PROP_CONTRAST, m_settings.contrast);
        if(m_camera->get(CAP_PROP_CONTRAST) != m_settings.contrast) {
            cout << "Error setting contrast!" << endl;
            error = true;
        }
    }
    if(m_settings.saturation != -1) {
        m_camera->set(CAP_PROP_SATURATION, m_settings.saturation);
        if(m_camera->get(CAP_PROP_SATURATION) != m_settings.saturation) {
            cout << "Error setting saturation!" << endl;
            error = true;
        }
    }
    if(m_settings.exposure != -1) {
        m_camera->set(CAP_PROP_EXPOSURE, m_settings.exposure);
        if(m_camera->get(CAP_PROP_EXPOSURE) != m_settings.exposure) {
            cout << "Error setting exposure!" << endl;
            error = true;
        }
    }
    return error;
}

void CORECapture::SetCrop(int X, int Y, int width, int height) {
    m_crop = new Rect(X, Y, width, height);
    m_useCrop = true;
}

void CORECapture::DisableCrop(bool disableCrop) {
    m_useCrop = !disableCrop;
}

double CORECapture::GetFPS() {
    return m_camera->get(CAP_PROP_FPS);
}

double CORECapture::GetRealFPS() {
    return m_fps;
    /*camera.lock();
    auto currentTimestamp = m_frame.second;
    auto lastTimestamp = m_lastTimestamp;
    int frames = m_frameCount;
    m_frameCount = 0;
    camera.unlock();
    return ((double)frames / difftime(currentTimestamp, lastTimestamp));*/
}


double CORECapture::GetResolutionWidth() {
    return m_camera->get(CAP_PROP_FRAME_WIDTH);
}

double CORECapture::GetResolutionHeight() {
    return m_camera->get(CAP_PROP_FRAME_HEIGHT);
}

double CORECapture::GetBrightness() {
    return m_camera->get(CAP_PROP_BRIGHTNESS);
}

double CORECapture::GetContrast() {
    return m_camera->get(CAP_PROP_CONTRAST);
}

double CORECapture::GetSaturation() {
    return m_camera->get(CAP_PROP_SATURATION);
}

double CORECapture::GetExposure() {
    return m_camera->get(CAP_PROP_EXPOSURE);
}

CORECapture::captureSettings CORECapture::GetCameraSettings() {
    return m_settings;
}

void CORECapture::UseFileInput(bool manualImageMode) {
    m_manualImageMode = manualImageMode;
}

void CORECapture::SetFileInputLocation(String path, fileInputType inputType) {
    m_inputFilePath = path;
    m_inputType = inputType;
}

bool CORECapture::IsOpen() {
    return m_isOpen;
}

void CORECapture::Close() {
    m_runCaptureThread = false;
    m_camera->release();
}

void CORECapture::InitializeCamera(int deviceNumber) {
    m_camera = new VideoCapture(deviceNumber);
    while(!m_camera->isOpened()) {
        //TODO: Possibly sleep here?
    }
    cout << "Camera Opened" << endl;
    bool error = SetCameraSettings(m_settings);
    if(error) {
        cout << "Error Setting Camera Settings!" << endl;
    }
    cout << "Camera Initialized!" << endl;
    while(!m_camera->isOpened()) {
        //TODO: Sleep?
    }
    m_isOpen = true;
    m_runCaptureThread = true;
    m_cameraCaptureThread = thread(&CORECapture::CameraCaptureThread, this);
    cout << "Started Capture" << endl;
}

void CORECapture::CameraCaptureThread() {
    Mat frame;
    while(m_runCaptureThread) {
        if(!m_manualImageMode) {
            *m_camera >> frame;
            auto currentTimestamp = time(0);
            if(m_useCrop)
                frame = frame(*m_crop);
            else
                frame = frame;
            camera.lock();
            m_frame.first = frame;
            m_frame.second = currentTimestamp;
            camera.unlock();
            double deltaTime = difftime(currentTimestamp, m_lastTimestamp);
            if(deltaTime >= 1) {
                m_fps = ((double) m_frameCount) / deltaTime;
                m_lastTimestamp = currentTimestamp;
                m_frameCount = 0;
            } else {
                m_frameCount++;
            }
            m_newFrameReady = true;
        }
    }
    m_isOpen = false;
}

CORECapture::~CORECapture() {
    Close();
}
