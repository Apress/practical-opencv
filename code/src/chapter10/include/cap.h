#include <opencv2/opencv.hpp>
#include "interface/mmal/mmal.h"
#include "interface/mmal/util/mmal_default_components.h"
#include "interface/mmal/util/mmal_connection.h"
#include "interface/mmal/util/mmal_util.h"
#include "interface/mmal/util/mmal_util_params.h"

class PiCapture {
    private:
        MMAL_COMPONENT_T *camera;
        MMAL_COMPONENT_T *preview;
        MMAL_ES_FORMAT_T *format;
        MMAL_STATUS_T status;
        MMAL_PORT_T *camera_preview_port, *camera_video_port, *camera_still_port;
        MMAL_PORT_T *preview_input_port;
        MMAL_CONNECTION_T *camera_preview_connection;
        bool color;
    public:
        static cv::Mat image;
        static int width, height;
        static MMAL_POOL_T *camera_video_port_pool;
        static void set_image(cv::Mat _image) {image = _image;}
        PiCapture(int, int, bool);
        cv::Mat grab() {return image;}
};

static void color_callback(MMAL_PORT_T *, MMAL_BUFFER_HEADER_T *);
static void color_callback(MMAL_PORT_T *, MMAL_BUFFER_HEADER_T *);
