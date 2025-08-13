
//SD ** MOSI - pin 11 ** MISO - pin 12 ** CLK - pin 13 ** CS - pin 10
#include <EEPROM.h> //biblioteca pra gravar ultima figura na memória rom << sistema de troca com botão reset
#include <FastLED.h> //biblioteca de funcoes de controle dos leds
#include <SPI.h> //biblioteca de funcoes para leitura do cartao SD
#include <SD.h> //biblioteca de funcoes para leitura do cartao SD

#define NUM_LEDS 176 // Numero de Leds na strip
#define DATA_PIN 7 // Pino de dados

CRGB leds[NUM_LEDS];//cria a matriz de leds
File myFile; //cria a variavel do arquivo


//SETA PARAMETROS DO PROGRAMA
//--------------------------------------

int TempoDelay = 0;//delay entre uma linha e outra
int NumLinhas;//número de linhas da imagem
//--------------------------------------

int RegEEPROM;

void setup() {

RegEEPROM = EEPROM.read(0);//Resgata valor da última imagem da memória ROM
String NomeArq = String(RegEEPROM)+".bmp";//declara a variável com o nome completo do arquivo

  delay(200); //delay para prevenir que os leds queimem por oscilação de energia
  FastLED.addLeds<WS2812, DATA_PIN, RGB>(leds, NUM_LEDS);  //inicializa a biblioteca dos leds 
  
  Serial.begin(9600);//inicia a comunicação serial
  
  SD.begin(10);//--INICIALIZAÇÃO DO CARTÃO SD NO PINO 10

if (SD.exists(NomeArq)!=true)//Caso o arquivo da vez não exista vai procurar o arquivo 0.bmp
  {
    Serial.println ("Não Encontrou Arquivo e setou para zero");
    NomeArq = "0.bmp";//seta para o primeiro arquivo
    RegEEPROM = 0;//Atualiza a variável do valor, resetando o loop
    
    if (SD.exists(NomeArq)!=true) 
    {
      Serial.println ("Erro Não Achou Aquivo Zero");
      
      ErroSemZeroBMP();
    }//pula para função que pisca os dois primeiros leds caso não exista o arquivo 0.bmp
  }

myFile = SD.open(NomeArq);//ao arquivo existe, abrindo
EEPROM.write(0, RegEEPROM+1);//Grava na memória ROM o valor da próxima imagem a ser aberta se existir
NumLinhas = (myFile.size()-56)/528;//56 = cabeçalho + 2 bytes de parada // 528 = 176 pixels por linha x 3 bytes por pixel

}

void loop() {

      ContadorInicio(); // pisca o led do arduino sinalizando que iriá iniciar a imagem
  
      myFile.seek(54);//pula o cabeçalho e vai para o primeiro byte de cor
      Serial.println ("Achou o Arquivo");
      
byte b;
byte r;
byte g;
      
      for (int y = 0; y<NumLinhas;y++) {//lê o arquivo linha por linha
      
          for (int x = 0; x<NUM_LEDS/2; x++)
          {
            b = myFile.read(); r = myFile.read(); g = myFile.read();//lê os 3 bytes do pixel da primeira fita
            leds[x].r = r;
            leds[x].g = g;
            leds[x].b = b;

            b = myFile.read(); r = myFile.read(); g = myFile.read();//lê os 3 bytes do pixel da segunda fita
            leds[x+88].r = r;
            leds[x+88].g = g;
            leds[x+88].b = b;
        }
         
        FastLED.show();//mostra os pixels lidos
        //delay(TempoDelay); //paraliza o código para que os pixels sejam exibidos
      }   

          for (int x = 0; x<NUM_LEDS/2; x++)
          {
            //zera os leds
            leds[x].r = 0;
            leds[x].g = 0;
            leds[x].b = 0;
            leds[x+88].r = 0;
            leds[x+88].g = 0;
            leds[x+88].b = 0;
          }
  
}

void ErroSemZeroBMP ()
{
    Serial.println ("Erro Sem 0.bmp");
    
    while (0!=1)//Dead lock lindeza >.< pisca os leds para sinalizar erro
    {
      leds[0].g=60;
      leds[88].g=60;
      FastLED.show();
      delay(2000);

      leds[0].g=0;
      leds[88].g=0;
      FastLED.show();
      delay(2000); 
    } 
    
}

void ContadorInicio ()
{
  //ilumina de vermelho o numero de leds referentes ao arquivo atual
  for (int i = 88; i <= 88 + RegEEPROM; i++)
  {
    leds[i].g = 50;
  }
  for (int z=4; z>=0;z--)
  {
    leds[z+1].b=0;
    leds[z].b=50;
    
    FastLED.show();
    delay(1000);
  
  }
return;
}
