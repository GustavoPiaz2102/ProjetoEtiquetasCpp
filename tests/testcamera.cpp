#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>

using namespace std;

int main() {
    // Abrir dispositivo
    int fd = open("/dev/video0", O_RDWR);
    if (fd < 0) {
        cerr << "Erro ao abrir dispositivo" << endl;
        return 1;
    }

    // Configurar formato
    v4l2_format format{};
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    format.fmt.pix.width = 1280;
    format.fmt.pix.height = 720;
    format.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
    format.fmt.pix.field = V4L2_FIELD_NONE;

    if (ioctl(fd, VIDIOC_S_FMT, &format) < 0) {
        cerr << "Erro ao configurar formato" << endl;
        close(fd);
        return 1;
    }

    // Solicitar buffers
    v4l2_requestbuffers req{};
    req.count = 4;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if (ioctl(fd, VIDIOC_REQBUFS, &req) < 0) {
        cerr << "Erro ao solicitar buffers" << endl;
        close(fd);
        return 1;
    }

    // Capturar um frame
    v4l2_buffer buffer{};
    buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buffer.memory = V4L2_MEMORY_MMAP;
    buffer.index = 0;

    if (ioctl(fd, VIDIOC_QBUF, &buffer) < 0) {
        cerr << "Erro ao enfileirar buffer" << endl;
        close(fd);
        return 1;
    }

    // Iniciar captura
    v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(fd, VIDIOC_STREAMON, &type) < 0) {
        cerr << "Erro ao iniciar captura" << endl;
        close(fd);
        return 1;
    }

    // Aguardar frame
    if (ioctl(fd, VIDIOC_DQBUF, &buffer) < 0) {
        cerr << "Erro ao capturar frame" << endl;
        close(fd);
        return 1;
    }

    // Salvar imagem
    ofstream out("captura.jpg", ios::binary);
    out.write(static_cast<char*>(buffer.m.userptr), buffer.bytesused);
    out.close();

    cout << "Imagem salva como captura.jpg" << endl;

    // Limpeza
    close(fd);
    return 0;
}