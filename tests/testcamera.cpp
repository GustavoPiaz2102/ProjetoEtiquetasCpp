#include <opencv2/opencv.hpp>
#include <iostream>
#include <thread>
#include <cstdlib>
#include <csignal>
#include <unistd.h>
#include <sys/wait.h>  // <<== essa linha que faltava

pid_t pipe_pid = -1;

void start_stream_process() {
    const char* cmd = 
        "libcamera-vid -t 0 --codec yuv420 --width 640 --height 480 -o - | "
        "ffmpeg -loglevel quiet -f rawvideo -pix_fmt yuv420p -s 640x480 -i - -f v4l2 /dev/video10";

    pipe_pid = fork();
    if (pipe_pid == 0) {
        execl("/bin/sh", "sh", "-c", cmd, (char*)NULL);
        std::cerr << "Erro ao executar comando.\n";
        std::exit(1);
    }
}

void stop_stream_process() {
    if (pipe_pid > 0) {
        kill(pipe_pid, SIGTERM);
        waitpid(pipe_pid, nullptr, 0);  // agora compilador reconhece
        std::cout << "Pipeline encerrado.\n";
    }
}

int main() {
    std::cout << "Iniciando pipeline...\n";
    start_stream_process();

    std::this_thread::sleep_for(std::chrono::seconds(2));

    cv::VideoCapture cap("/dev/video10");
    if (!cap.isOpened()) {
        std::cerr << "Erro ao abrir /dev/video10\n";
        stop_stream_process();
        return 1;
    }

    cv::Mat frame;
    while (true) {
        cap >> frame;
        if (frame.empty()) break;

        cv::imshow("Camera", frame);
        if (cv::waitKey(1) == 27) break; // ESC para sair
    }

    stop_stream_process();
    cap.release();
    cv::destroyAllWindows();
    return 0;
}
