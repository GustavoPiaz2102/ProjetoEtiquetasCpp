#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <cstring>

using namespace std;

struct Buffer {
    void* start;
    size_t length;
};

int main() {
    const char* device = "/dev/video0";
    int fd = open(device, O_RDWR);
    if (fd < 0) {
        cerr << "Erro ao abrir dispositivo: " << device << endl;
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

    // Mapear os buffers
    Buffer* buffers = new Buffer[req.count];
    for (unsigned int i = 0; i < req.count; ++i) {
        v4l2_buffer buffer{};
        buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buffer.memory = V4L2_MEMORY_MMAP;
        buffer.index = i;

        if (ioctl(fd, VIDIOC_QUERYBUF, &buffer) < 0) {
            cerr << "Erro ao consultar buffer" << endl;
            close(fd);
            delete[] buffers;
            return 1;
        }

        buffers[i].length = buffer.length;
        buffers[i].start = mmap(nullptr, buffer.length, 
                               PROT_READ | PROT_WRITE, 
                               MAP_SHARED, 
                               fd, buffer.m.offset);

        if (buffers[i].start == MAP_FAILED) {
            cerr << "Erro ao mapear buffer" << endl;
            close(fd);
            delete[] buffers;
            return 1;
        }
    }

    // Enfileirar um buffer
    v4l2_buffer buf{};
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = 0;

    if (ioctl(fd, VIDIOC_QBUF, &buf) < 0) {
        cerr << "Erro ao enfileirar buffer" << endl;
        close(fd);
        for (unsigned int i = 0; i < req.count; ++i) {
            munmap(buffers[i].start, buffers[i].length);
        }
        delete[] buffers;
        return 1;
    }

    // Iniciar captura
    v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(fd, VIDIOC_STREAMON, &type) < 0) {
        cerr << "Erro ao iniciar captura" << endl;
        close(fd);
        for (unsigned int i = 0; i < req.count; ++i) {
            munmap(buffers[i].start, buffers[i].length);
        }
        delete[] buffers;
        return 1;
    }

    // Aguardar frame
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(fd, &fds);
    timeval tv{};
    tv.tv_sec = 5; // Timeout de 5 segundos

    int r = select(fd + 1, &fds, nullptr, nullptr, &tv);
    if (r <= 0) {
        cerr << "Timeout ou erro ao aguardar frame" << endl;
        close(fd);
        for (unsigned int i = 0; i < req.count; ++i) {
            munmap(buffers[i].start, buffers[i].length);
        }
        delete[] buffers;
        return 1;
    }

    // Capturar frame
    if (ioctl(fd, VIDIOC_DQBUF, &buf) < 0) {
        cerr << "Erro ao capturar frame" << endl;
        close(fd);
        for (unsigned int i = 0; i < req.count; ++i) {
            munmap(buffers[i].start, buffers[i].length);
        }
        delete[] buffers;
        return 1;
    }

    // Salvar imagem
    ofstream out("captura.jpg", ios::binary);
    out.write(static_cast<char*>(buffers[buf.index].start), buf.bytesused);
    out.close();

    cout << "Imagem salva como captura.jpg (" << buf.bytesused << " bytes)" << endl;

    // Limpeza
    close(fd);
    for (unsigned int i = 0; i < req.count; ++i) {
        munmap(buffers[i].start, buffers[i].length);
    }
    delete[] buffers;
    
    return 0;
}