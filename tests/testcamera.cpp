#include <iostream>
#include <memory>
#include <vector>

#include <libcamera/libcamera.h>

using namespace libcamera;
using namespace std;

int main()
{
    unique_ptr<CameraManager> manager = make_unique<CameraManager>();
    
    try {
        manager->start();
        
        if (manager->cameras().empty()) {
            cerr << "Nenhuma câmera encontrada!" << endl;
            return EXIT_FAILURE;
        }
        
        shared_ptr<Camera> camera = manager->get(manager->cameras()[0]->id());
        if (!camera) {
            cerr << "Falha ao obter câmera" << endl;
            manager->stop();
            return EXIT_FAILURE;
        }
        
        if (camera->acquire()) {
            cerr << "Falha ao adquirir câmera" << endl;
            manager->stop();
            return EXIT_FAILURE;
        }
        
        cout << "Câmera adquirida: " << camera->id() << endl;
        
        vector<StreamRole> roles = { StreamRole::Viewfinder };
        unique_ptr<CameraConfiguration> config = camera->generateConfiguration(roles);
        if (!config) {
            cerr << "Falha ao gerar configuração" << endl;
            camera->release();
            manager->stop();
            return EXIT_FAILURE;
        }
        
        switch (config->validate()) {
            case CameraConfiguration::Valid:
                break;
            case CameraConfiguration::Adjusted:
                cout << "Configuração ajustada" << endl;
                break;
            case CameraConfiguration::Invalid:
                cerr << "Configuração inválida" << endl;
                camera->release();
                manager->stop();
                return EXIT_FAILURE;
        }
        
        if (camera->configure(config.get()) < 0) {
            cerr << "Falha ao configurar câmera" << endl;
            camera->release();
            manager->stop();
            return EXIT_FAILURE;
        }
        
        Stream *stream = config->at(0).stream();
        FrameBufferAllocator *allocator = new FrameBufferAllocator(camera);
        
        if (allocator->allocate(stream) < 0) {
            cerr << "Falha ao alocar buffers" << endl;
            delete allocator;
            camera->release();
            manager->stop();
            return EXIT_FAILURE;
        }
        
        // Criar uma única requisição por buffer
        vector<unique_ptr<Request>> requests;
        const vector<unique_ptr<FrameBuffer>> &buffers = allocator->buffers(stream);
        
        for (const unique_ptr<FrameBuffer> &buffer : buffers) {
            unique_ptr<Request> request = camera->createRequest();
            if (!request) {
                cerr << "Falha ao criar requisição" << endl;
                delete allocator;
                camera->release();
                manager->stop();
                return EXIT_FAILURE;
            }
            
            if (request->addBuffer(stream, buffer.get()) < 0) {
                cerr << "Falha ao adicionar buffer à requisição" << endl;
                delete allocator;
                camera->release();
                manager->stop();
                return EXIT_FAILURE;
            }
            
            requests.push_back(move(request));
        }
        
        if (camera->start()) {
            cerr << "Falha ao iniciar câmera" << endl;
            delete allocator;
            camera->release();
            manager->stop();
            return EXIT_FAILURE;
        }
        
        cout << "Câmera iniciada com sucesso!" << endl;
        
        // Enfileirar todas as requisições
        for (auto &request : requests) {
            if (camera->queueRequest(request.get()) < 0) {
                cerr << "Falha ao enfileirar requisição" << endl;
                camera->stop();
                delete allocator;
                camera->release();
                manager->stop();
                return EXIT_FAILURE;
            }
        }
        
        cout << "Pressione Enter para sair..." << endl;
        cin.get();
        
        camera->stop();
        delete allocator;
        camera->release();
        manager->stop();
        
    } catch (const exception &e) {
        cerr << "Erro: " << e.what() << endl;
        if (manager)
            manager->stop();
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}