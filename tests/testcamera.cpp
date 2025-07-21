#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>

int main() {
    int fd = open("/dev/video0", O_RDWR);
    if (fd == -1) {
        std::cerr << "Erro ao abrir a câmera" << std::endl;
        return 1;
    }

    v4l2_capability cap;
    if (ioctl(fd, VIDIOC_QUERYCAP, &cap) == -1) {
        std::cerr << "Erro ao consultar capacidades" << std::endl;
        close(fd);
        return 1;
    }

    std::cout << "Câmera aberta com sucesso!" << std::endl;
    std::cout << "Driver: " << cap.driver << std::endl;
    std::cout << "Card: " << cap.card << std::endl;
    std::cout << "Bus: " << cap.bus_info << std::endl;
    
    close(fd);
    return 0;
}