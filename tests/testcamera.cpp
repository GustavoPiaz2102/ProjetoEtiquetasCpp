#include <iostream>
#include <memory>
#include <chrono>

#include <libcamera/libcamera.h>

using namespace libcamera;
using namespace std;

int main()
{
    // Cria uma instância do gerenciador de câmera
    unique_ptr<CameraManager> manager = make_unique<CameraManager>();
    
    try {
        // Inicia o gerenciador de câmera
        manager->start();
        
        // Verifica se há câmeras disponíveis
        if (manager->cameras().empty()) {
            cerr << "Nenhuma câmera encontrada!" << endl;
            manager->stop();
            return EXIT_FAILURE;
        }
        
        // Pega a primeira câmera disponível
        shared_ptr<Camera> camera = manager->get(manager->cameras()[0]->id());
        if (!camera) {
            cerr << "Falha ao obter câmera" << endl;
            manager->stop();
            return EXIT_FAILURE;
        }
        
        // Configura a câmera
        if (camera->acquire()) {
            cerr << "Falha ao adquirir câmera" << endl;
            manager->stop();
            return EXIT_FAILURE;
        }
        
        cout << "Câmera adquirida: " << camera->id() << endl;
        
        // Configuração básica da câmera
        vector<StreamRole> roles = { StreamRole::Viewfinder };
        unique_ptr<CameraConfiguration> config = camera->generateConfiguration(roles);
        if (!config) {
            cerr << "Falha ao gerar configuração" << endl;
            camera->release();
            manager->stop();
            return EXIT_FAILURE;
        }
        
        // Aplica a configuração
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
        
        // Cria um alocador de buffer
        FrameBufferAllocator *allocator = new FrameBufferAllocator(camera);
        
        // Aloca buffers para o stream
        Stream *stream = config->at(0).stream();
        if (allocator->allocate(stream) < 0) {
            cerr << "Falha ao alocar buffers" << endl;
            delete allocator;
            camera->release();
            manager->stop();
            return EXIT_FAILURE;
        }
        
        // Cria uma requisição
        unique_ptr<Request> request = camera->createRequest();
        if (!request) {
            cerr << "Falha ao criar requisição" << endl;
            delete allocator;
            camera->release();
            manager->stop();
            return EXIT_FAILURE;
        }
        
        // Adiciona buffers à requisição
        const vector<unique_ptr<FrameBuffer>> &buffers = allocator->buffers(stream);
        for (const unique_ptr<FrameBuffer> &buffer : buffers) {
            if (request->addBuffer(stream, buffer.get()) < 0) {
                cerr << "Falha ao adicionar buffer à requisição" << endl;
                delete allocator;
                camera->release();
                manager->stop();
                return EXIT_FAILURE;
            }
        }
        
        // Inicia a câmera
        if (camera->start()) {
            cerr << "Falha ao iniciar câmera" << endl;
            delete allocator;
            camera->release();
            manager->stop();
            return EXIT_FAILURE;
        }
        
        cout << "Câmera iniciada com sucesso!" << endl;
        cout << "Pressione Enter para sair..." << endl;
        
        // Envia a requisição para captura
        if (camera->queueRequest(request.get()) < 0) {
            cerr << "Falha ao enfileirar requisição" << endl;
            camera->stop();
            delete allocator;
            camera->release();
            manager->stop();
            return EXIT_FAILURE;
        }
        
        // Aguarda entrada do usuário para sair
        cin.get();
        
        // Limpeza
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