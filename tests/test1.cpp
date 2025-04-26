/*
 * Programa de Captura Contínua de Câmera com OCR
 * Detecta frames da câmera e aplica OCR periodicamente
 * Pressione 'q' para sair
 */

 #include <opencv2/opencv.hpp>
 #include <tesseract/baseapi.h>
 #include <leptonica/allheaders.h>
 #include <iostream>
 #include <chrono>
 #include <thread>
 
 using namespace cv;
 using namespace std;
 using namespace chrono;
 
 // Constantes de configuração
 const int CAMERA_INDEX = 0;
 const Size CAMERA_RESOLUTION(1280, 720);
 const Size WINDOW_SIZE(800, 600);
 const int OCR_INTERVAL_MS = 1;
 const string OCR_LANGUAGES = "eng+por";
 
 // Função para pré-processar a imagem antes do OCR
 Mat preprocessImageForOCR(const Mat& input) {
     Mat gray, blurred, thresholded;
     
     // Converter para escala de cinza
     cvtColor(input, gray, COLOR_BGR2GRAY);
     
     // Aplicar desfoque para reduzir ruído
     GaussianBlur(gray, blurred, Size(3, 3), 0);
     
     // Binarização adaptativa
     adaptiveThreshold(blurred, thresholded, 255, ADAPTIVE_THRESH_GAUSSIAN_C, 
                      THRESH_BINARY, 11, 2);
     
     return thresholded;
 }
 
 int main() {
     // Inicializar a captura de vídeo
     VideoCapture cap(CAMERA_INDEX);
     
     if (!cap.isOpened()) {
         cerr << "Erro ao abrir a câmera!" << endl;
         return -1;
     }
     
     // Configurar a resolução da câmera
     cap.set(CAP_PROP_FRAME_WIDTH, CAMERA_RESOLUTION.width);
     cap.set(CAP_PROP_FRAME_HEIGHT, CAMERA_RESOLUTION.height);
     
     // Inicializar o Tesseract OCR
     tesseract::TessBaseAPI *ocr = new tesseract::TessBaseAPI();
     
     if (ocr->Init(NULL, OCR_LANGUAGES.c_str())) {
         cerr << "Erro ao inicializar o Tesseract OCR!" << endl;
         return -1;
     }
     
     // Configurar modo de segmentação de página
     ocr->SetPageSegMode(tesseract::PSM_AUTO);
     
     // Configurar janela de exibição
     namedWindow("Camera", WINDOW_NORMAL);
     resizeWindow("Camera", WINDOW_SIZE.width, WINDOW_SIZE.height);
     
     cout << "Pressione 'q' para sair..." << endl;
     
     auto last_ocr_time = steady_clock::now();
     
     while (true) {
         Mat frame;
         cap >> frame; // Capturar frame da câmera
         
         if (frame.empty()) {
             cerr << "Frame vazio recebido!" << endl;
             break;
         }
         
         // Exibir o frame original
         imshow("Camera", frame);
         
         // Verificar se é hora de processar OCR
         auto now = steady_clock::now();
         auto elapsed_ms = duration_cast<milliseconds>(now - last_ocr_time).count();
         
         if (elapsed_ms >= OCR_INTERVAL_MS) {
             last_ocr_time = now;
             
             // Pré-processar e executar OCR
             Mat processed = preprocessImageForOCR(frame);
             ocr->SetImage(processed.data, processed.cols, processed.rows, 
                           1, processed.step);
             
             // Obter texto reconhecido
             char* text = ocr->GetUTF8Text();
             
             if (text && strlen(text) > 0) {
                 cout << "\nTexto reconhecido:\n" << text << endl;
                 
                 // Adicionar marcação no frame original
                 putText(frame, "Texto detectado!", Point(20, 50), 
                         FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 0), 2);
                 
                 // Mostrar imagem processada
                 imshow("Processado", processed);
             }
             
             // Liberar memória do texto
             delete[] text;
         }
         
         // Verificar se o usuário pressionou 'q' para sair
         if (waitKey(1) == 'q') {
             break;
         }
     }
     
     // Liberar recursos
     ocr->End();
     delete ocr;
     cap.release();
     destroyAllWindows();
     
     return 0;
 }