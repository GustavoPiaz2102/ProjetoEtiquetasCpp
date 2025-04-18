Primeira vez que estou escrevendo esse codigo, entao nao sei se vai seguir assim
diagrama:

1. Model
1.1 library 'sao os imports para a controller, importa tudo que tiver na model e manda pra controller ou detect
1.2 ocr 'sera a parte que vai receber a imagem (ja preprocessada e vai retornar o texto detectado)'
1.3 preprocessor 'a parte que recebe a imagem e faz o preprocessamento para melhorar a deteccao'
1.4 validation 'a parte que recebe o texto e valida com a base no texto de referencia'
1.5 capture 'faz a captura da imagem e retorna'

2. controller
2.1 controller 'inicia e segue a sequencia de passos , recebe os valores da interface e gerencia para a deteccao ou mudanca de padrao'
2.2 detector 'a parte que controlla a deteccao,faz a captura ,preprocessa,faz o ocr e valida, constantemente e retornando o resultado'

3.view
3.1 inteface 'faz a interacao com o usuario e retorna valores selecionados para a controller'