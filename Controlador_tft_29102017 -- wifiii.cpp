// no momento em que for atualizar novamente, verificar se tem como fazer a sincronizaçao dos obejos de acordo com a pagina que se encontra no momento na tela. para nao haver troca das variaveis. para tal, criar variaveis no nextion hmi auxiliares.





//#include <SoftwareSerial.h>
#include <Nextion.h>
#include <EEPROM.h>
#include <Wire.h>
#include "RTClib.h"
#include  "avr/wdt.h" 
#include <SoftwareSerial.h>


/////////////////////////////DIRETIVAS ///////////////////////
#define on 1
#define off 0

////////////////////////////DEFINE PORTS /////////////////////

#define ventuinha_RL1 2 
#define flap_RL1 12 
#define flap_RL2 11 
#define abafador_RL1 10     //5  
#define abafador_RL2 9
#define buzzer 8
#define sensor_temperatura A0
#define sensor_umidade A1

////////////////// OBJETOS DO DISPLAY /////////////////////////
typedef struct{
  uint32_t temp_ajst;                 // n2
  uint32_t umid_ajst;                 // n3
  uint32_t bt_ventoinha=1;            // bt3
  uint32_t bt_buzzer;                 // bt1
  uint32_t bt_config;                 // bt2
  uint32_t temp;                      // n0
  uint32_t umid;                      // n1
  char horario[5];                    // t2
  char alerta[13]; 
  char aviso[15];                     // t0
  //uint32_t bt_salvar;                 // bt0
  uint32_t bt_controle;
  uint32_t bt_s_flap;
  uint32_t bt_s_ventoinha;
  uint32_t bt_s_abafador;
  uint32_t bt_s_alarme;
  uint32_t bt_salva_hora;
  uint32_t hora_ajst;
  uint32_t minuto_ajst;
  
 // uint32_t dsegundo_ajst;
  //uint32_t bt_s_aux1;
  //uint32_t bt_s_aux2;

}TELA_TRABALHO;

 /* typedef struct{              //objetos wifi
  	char SSID [20];
  	char SENHA [20];
  	char NSERIE [6];
  	char SENHARM [6];
  	char STATUS [10];
  	uint32_t bt_salva_wifi;
  	
  }WIFI;

*/

/////////////////////////// OBETOS DO DISPLAY/////////////////

NexText dhora = NexText(1,8,"t2");                   // relogio do display  // (pagina, id , nome)
NexText text = NexText(7,2,"data"); 
NexText daviso = NexText(1,15,"t1");                 // campo de alertas no display
//NexDSButton dbtconfig = NexDSButton(1,13,"bt2");     // botao pagina 1 configuraçao.
NexDSButton dbtventoinha = NexDSButton(1,15,"bt3");   // botao da ventoinha
//NexDSButton dbtsalvar = NexDSButton(2,8,"bt0");    // botao salvar pagina ajuste umidade     
NexDSButton dbtalarme = NexDSButton(1,4,"bt1");      // botao alarme
NexDSButton dbtcontrole = NexDSButton(5,8,"bt6");    // Botao de controle de saidas, referenciapara manter-se da tela
NexDSButton dbtsalva_hora = NexDSButton(6,12,"bt15"); // display botao de controle para salvar horario no RTC
NexNumber dhoraajst = NexNumber(6,2,"n0");               // variaveis de tempo no ajuste do RTC
NexNumber dminutoajst = NexNumber(6,3,"n1");			 // variaveis de tempo no ajuste do RTC
NexNumber dsegundoajst = NexNumber(6,4,"n2");			 // variaveis de tempo no ajuste do RTC
NexDSButton dbtsflap = NexDSButton(5,2,"bt0");       // display botao de controle saida flap
NexDSButton dbtsventoinha = NexDSButton(5,4,"bt2");  // display botao de controle saida ventoinha
NexDSButton dbtsabafador = NexDSButton(5,3,"bt1");   // display botao de controle saida abafador
NexDSButton dbtsalarme = NexDSButton(5,5,"bt3");     // display botao de controle saida alarme
//NexDSButton dbtsaux1 = NexDSButton(5,6,"bt4");     // display botao de controle saida aux1
//NexDSButton dbtsaux2 = NexDSButton(5,7,"bt5");     // display botao de controle saida aux2
NexNumber dtemp = NexNumber(1,6,"n0");               // campo de temperatura pagina 1 
NexNumber dumid = NexNumber(1,7,"n1");               // campo de umidade pagina 1 
NexNumber dtempajst = NexNumber(1,11,"n2");          // campo de ajuste temperatura pagina 1 
NexNumber dumidajst = NexNumber(1,12,"n3");          // campo de ajuste umidade pagina 1 
/*
NexText dssid = NexText(8,4,"t5");
NexText dsenha = NexText(8,2,"t3");
NexText dNserie = NexText(8,3,"t4");
NexText dsenhaRM = NexText(8,5,"t6");
NexText dstatuswifi = NexText(8,6,"t7");
NexDSButton dbtsalva_wifi = NexDSButton(8,7,"bt7");      // botao de controle de wifi
*/

///////////////////////////////Variaveis globais /////////

byte _temp_ajst, _umid_ajst,alarme,active_alarme,time_alarme,sec,hora,minuto,time_controle,time_controle_saidas,time_enable_alarme,espp,timewifi;
long previousMillis = 0;
boolean alarme_status;


/////////////////////////////INICIA OBJETOS //////////////////
//SoftwareSerial nextion(6,7);
//Nextion tft(nextion, 9600);

RTC_DS1307 rtc;
DateTime rtcc;
SoftwareSerial esp(2, 3);
TELA_TRABALHO p1;              // Instancia da Estrutura
//WIFI wf;


///////////////////////////// SETUP //////////////////////////

void setup() {
  esp.begin(19200);
  delay(2000);

  pinMode(sensor_temperatura, INPUT);
  pinMode(sensor_umidade, INPUT);
  pinMode(ventuinha_RL1, OUTPUT);
  pinMode(flap_RL1, OUTPUT);
  pinMode(flap_RL2, OUTPUT);
  pinMode(abafador_RL1, OUTPUT);
  pinMode(abafador_RL2, OUTPUT);
  pinMode(buzzer, OUTPUT);


  p1.temp_ajst = EEPROM.read(0);_temp_ajst=p1.temp_ajst;
  p1.umid_ajst = EEPROM.read(1);_umid_ajst=p1.umid_ajst;




  Wire.begin();
  rtc.begin();
  nexInit();
  delay(3000);
  sendCommand("page 1");delay(20);
   dtempajst.setValue(p1.temp_ajst);delay(20);     // seta valor que esta gravado no ajuste
   dumidajst.setValue(p1.umid_ajst);delay(20);     // seta valor que esta gravado no ajuste
   //dbtalarme.setValue(p1.bt_buzzer);delay(20);
   dbtventoinha.setValue(p1.bt_ventoinha);delay(20);            //Sempre inicia com a ventoinha ligada.
   wdt_enable(WDTO_4S); //Função que ativa e altera o Watchdog;
 }

void atualisaDisplay(){          /// Seta valores no display

	if(p1.bt_controle)display_controle(); 
	if(p1.bt_salva_hora)display_ajuste_relogio();
	//if(wf.bt_salva_wifi)display_wifi();                     

  if(p1.aviso[0] !=' '){
    daviso.setText(p1.aviso);delay(60);
    sendCommand("vis t1,1");delay(25);}else{sendCommand("vis t1,0");delay(25);}

    if(p1.bt_buzzer){
      sendCommand("vis p1,1");delay(25);}else{sendCommand("vis p1,0");delay(25);}

      if(p1.bt_ventoinha){
        sendCommand("vis p2,1");delay(25);}else{sendCommand("vis p2,0");delay(25);}
          
        dhora.setText(p1.horario);delay(30);
        dtemp.setValue(p1.temp);delay(25);
        dumid.setValue(p1.umid);delay(25);

        p1.aviso[0]=' ';
}

void sincronizaDados(){      // Recebe valores do display


  dtempajst.getValue(&p1.temp_ajst);delay(50);
  dumidajst.getValue(&p1.umid_ajst);delay(50);
  //dbtconfig.getValue(&p1.bt_config); delay(50);              
  dbtalarme.getValue(&p1.bt_buzzer);delay(20);
  dbtventoinha.getValue(&p1.bt_ventoinha);delay(20);
  dbtcontrole.getValue(&p1.bt_controle);delay(20);
  dbtsalva_hora.getValue(&p1.bt_salva_hora);delay(20);
  //dbtsalva_wifi.getValue(&wf.bt_salva_wifi);delay(20);
  
  if(p1.temp_ajst > 165 ){
  	p1.temp_ajst = 165;								// Nao deixa ajustar temperatura acima de 165ºF;
  	dtempajst.setValue(p1.temp_ajst);delay(20);
  }

  if(_temp_ajst!=p1.temp_ajst){ 
    EEPROM.write(0,p1.temp_ajst);
    _temp_ajst=p1.temp_ajst; //compara se foi alterado o valor, entao se sim grava na eeprom. Serve para "economizar" as escritas e leituras;

    umidade_automatica();     //Ainda fazer funçao para selecionar quando quiser trabalhar com umidade automatica
    dumidajst.setValue(p1.umid_ajst);delay(20); 

    p1.bt_ventoinha = 1;            // Para quando voltar da tela de ajuste de temperatura a ventoinha voltar a ligar.
    dbtventoinha.setValue(1);    
  }	

  if(_umid_ajst!=p1.umid_ajst){
    EEPROM.write(1,p1.umid_ajst);
    _umid_ajst=p1.umid_ajst;
  }

}

void relogio(){
  if(!rtc.isrunning()) {
   setAviso(straviso(F("Falha Relogio")));  // seta falha em p1.aviso
   sec++;delay(1000);
   if(sec>9){ 
    sec=0 ;
    minuto ++;
   }
   if(minuto > 59){
    minuto=0;
   }
   return ; 
 }

 String time="";
 rtcc=rtc.now();
 hora=rtcc.hour();
 minuto=rtcc.minute();
 sec=rtcc.second();
    if (hora<10) {time+="0";time+=hora;} else{time+=hora;}      /// Arruma formato de hora para 00:00
    time+=":";
    if (minuto<10) {time+="0";time+=minuto;}else{time+=minuto;}
    strcpy(p1.horario,time.c_str());      // converte string em array de char
  }

  void leitura() {
    float _temp = 0;
    float _umid = 0;

    for(int i=0;i<80;i++){
        _temp += (analogRead(sensor_temperatura) * 0.9 + 32); // tira média de leitura para evitar erros
        _umid += (analogRead(sensor_umidade) * 0.9 + 32);
      }
      p1.temp = _temp/80;
      p1.umid = _umid/80;
      if(p1.temp <= 33) p1.temp=0;
      if(p1.umid <= 33) p1.umid=0;
    
  }

    void abafador(byte action){
      digitalWrite(abafador_RL1,action);
      digitalWrite(abafador_RL2,action);
    }

    void flap(byte action){
      digitalWrite(flap_RL1,action);
      digitalWrite(flap_RL2,action);
    }

void ventoinha(byte action){                                               // Se usar ventoinha acrescentar isso.
  digitalWrite(ventuinha_RL1,action);
}

void alarmee(){

  if(active_alarme ==on && p1.bt_buzzer == on && alarme_status == true){
    digitalWrite(buzzer, on);
  }else digitalWrite(buzzer,off);
  
  if(!p1.bt_buzzer){                                //alarme ta desligado ??  // funçao para ligar o alarme automaticamente após 8 minutos;
    if(!p1.bt_buzzer && time_enable_alarme == 0){
  	 time_enable_alarme = 1;
    }
    if(time_alarme != minuto){
      time_alarme=minuto;
      time_enable_alarme++;
    }
    if(time_enable_alarme >= 9){                 // alarme volta a ligar 7 minutos depois
      p1.bt_buzzer=1;
      dbtalarme.setValue(1);delay(20);
      time_enable_alarme = 0;
    }
  }else time_enable_alarme = 0;

}

  void controle(){
//#ALARME

   alarmee();
   if(time_controle!=sec ){                                 /// Atualiza saidas a cada segundo;
     time_controle = sec;
     time_controle_saidas++;
     if(time_controle_saidas > 4) time_controle_saidas=0;   //temporizador para evitar de ficar abrindo e fechando varias vezes o flap.
   }
   if(time_controle_saidas == 4 ){

      if (p1.temp+4 < p1.temp_ajst) { setAviso(straviso(F("Temp. BAIXA"))); active_alarme=on; //controle_PID=true;  
      }else if (p1.temp-7 > p1.temp_ajst) { setAviso(straviso(F("Temp. ALTA"))); active_alarme=on;                
      }else active_alarme=off;                                                                        

//#FLAP

     if (p1.umid > p1.umid_ajst){ flap(on);
     }
     if(p1.temp-10 > p1.temp_ajst){ flap(on);       /// Se a temperatura subir 11 graus acima do ajuste entao o flap abre para nao estragar fumo.
     }
     else if(p1.umid <= p1.umid_ajst ) flap(off);
 /*
//##### CONTROLE PID TEMPERATURA ######        // 5 graus abaixo do ajuste // Controle PID: se a temperatura abaixar muito, ficara mt tempo abafador e o falp ligado, 
                                              //ultrapassando muito acima quando a temp atingir o ajuste, entao irá desligar 2 graus antes de atingir o ajuste. 
                                             //é usado somente quando a temp baixar muito; NAO È USADO QUANDO O ALARME ESTA LIGADO, tem tempo de referencia, pois se a temp nao atingir em 8 minutos, volta controle normal;
if(controle_PID && p1.bt_buzzer == 0){                                   // desligou o alarme? entao PID nao é implementado.
     // setAviso(straviso(F("'PID' ON T.B.")));                         // informa quando PID esta ligado;
      setAviso(straviso(F("Temp. BAIXA_")));
      if(p1.temp >= p1.temp_ajst) controle_PID = false;               // se temperatura for maior ao ajuste, ou alarme voltar a ligar por tempo, PID é delsigado.
         if (p1.temp+2 >= p1.temp_ajst ){ ventoinha(off); abafador(off);
         }else{ abafador(on);
          if(p1.bt_ventoinha) ventoinha(on); }
}else {
  controle_PID = false;
*/
//#ABAFADOR

      if (p1.temp < p1.temp_ajst ){ abafador(on); 
      }else if (p1.temp >= p1.temp_ajst ) abafador(off); 

//#VENTOINHA 
          if (p1.temp+1 < p1.temp_ajst && p1.bt_ventoinha ==1){ventoinha(on);                      
          }else if (p1.temp >= p1.temp_ajst ) ventoinha(off);

//  } // end_else_pid
 } // end_controle_saidas
} // end function controle() 

void display_controle(){
  while(p1.bt_controle){
   wdt_reset();
   dbtcontrole.getValue(&p1.bt_controle);
   dbtsflap.getValue(&p1.bt_s_flap);delay(5); if(p1.bt_s_flap) flap(on);else flap(off);
   dbtsventoinha.getValue(&p1.bt_s_ventoinha);delay(5); if(p1.bt_s_ventoinha) digitalWrite(ventuinha_RL1,on);else digitalWrite(ventuinha_RL1,off);
   dbtsalarme.getValue(&p1.bt_s_alarme);delay(5); if(p1.bt_s_alarme) digitalWrite(buzzer,on);else digitalWrite(buzzer,off);
   dbtsabafador.getValue(&p1.bt_s_abafador);delay(5); if(p1.bt_s_abafador) abafador(on); else abafador(off);
	 //dbtsaux1.getValue(&p1.bt_s_aux1);delay(25);if (p1.bt_s_aux1); ?????
	 //dbtsaux2.getValue(&p1.bt_s_aux2);delay(25);if (p1.bt_s_aux2); ?????
 }
}

void display_ajuste_relogio(){

	dhoraajst.setValue(hora);//delay(20);
	dminutoajst.setValue(minuto);//delay(20);                        // seta valor nos campos do display
	dsegundoajst.setValue(sec);//delay(20);

	while(p1.bt_salva_hora){

  	   dhoraajst.getValue(&p1.hora_ajst);//delay(20);               // salva valor digitado
  	   dminutoajst.getValue(&p1.minuto_ajst);//delay(20);
  	   dbtsalva_hora.getValue(&p1.bt_salva_hora);//delay(20);       // referencia com variavel do display para se manter no loop ;

  	   wdt_reset();
     }
  	 if(p1.hora_ajst!= hora | p1.minuto_ajst != minuto) rtc.adjust(DateTime(2017,1,1,p1.hora_ajst,p1.minuto_ajst,0));delay(500);  // compara se foi alterado, caso sim entao grava

    }
/*void display_wifi(){

	while(wf.bt_salva_wifi){
	  
  	   dssid.getText(wf.SSID,sizeof(wf.SSID));delay(100);               // salva valor digitado
  	   dsenha.getText(wf.SENHA,sizeof(wf.SENHA));delay(100);
  	   dsenhaRM.getText(wf.SENHARM,sizeof(wf.SENHARM));delay(100);       
  	   dNserie.getText(wf.NSERIE,sizeof(wf.NSERIE));delay(100);
  	   dbtsalva_wifi.getValue(&wf.bt_salva_wifi);  				// referencia para se manter no loop infinito;
  	   wdt_reset();
  	   dstatuswifi.setText(wf.SSID);delay(50);
  	   
  	}
  	
  }*/
String straviso(String text){
   String strstring = text;
   return strstring;
}
void setAviso(String text){              // Seta como array de char o aviso, para mostrar no display
 
  strcpy(p1.aviso,text.c_str());
}

void umidade_automatica() {

  if (p1.temp_ajst <= 90 | p1.temp_ajst == 91) {
    p1.umid_ajst = 88;
  } else
  if (p1.temp_ajst == 92 | p1.temp_ajst == 93) {
    p1.umid_ajst = 90;
  } else
  if (p1.temp_ajst == 94 | p1.temp_ajst == 95) {
    p1.umid_ajst = 92;
  } else
  if (p1.temp_ajst == 96 | p1.temp_ajst == 97) {
    p1.umid_ajst = 94;
  } else
  if (p1.temp_ajst == 98 | p1.temp_ajst == 99) {
    p1.umid_ajst = 96;
  } else
  if (p1.temp_ajst == 100 | p1.temp_ajst == 101) {
    p1.umid_ajst = 97;
  } else
  if (p1.temp_ajst >= 102 && p1.temp_ajst <= 117) {
    p1.umid_ajst = 96;
  } else
  if (p1.temp_ajst == 118 | p1.temp_ajst == 119) {
    p1.umid_ajst = 97;
  } else
  if (p1.temp_ajst == 120 | p1.temp_ajst == 122) {
    p1.umid_ajst = 98;
  } else
  if (p1.temp_ajst == 123 | p1.temp_ajst == 124) {
    p1.umid_ajst = 99;
  } else
  if (p1.temp_ajst == 125 | p1.temp_ajst == 126) {
    p1.umid_ajst = 100;
  } else
  if (p1.temp_ajst == 127 | p1.temp_ajst == 128) {
    p1.umid_ajst = 101;
  } else
  if (p1.temp_ajst == 129 | p1.temp_ajst == 130) {
    p1.umid_ajst = 102;
  } else
  if (p1.temp_ajst == 131 | p1.temp_ajst == 132) {
    p1.umid_ajst = 103;
  } else
  if (p1.temp_ajst >= 133 && p1.temp_ajst <= 148) {
    p1.umid_ajst = 104;
  } else
  if (p1.temp_ajst >= 149 && p1.temp_ajst <= 152) {
    p1.umid_ajst = 105;
  } else
  if (p1.temp_ajst == 153 | p1.temp_ajst == 154) {
    p1.umid_ajst = 106;
  } else
  if (p1.temp_ajst == 155 | p1.temp_ajst == 156) {
    p1.umid_ajst = 108;
  } else
  if (p1.temp_ajst == 157 | p1.temp_ajst == 158) {
    p1.umid_ajst = 109;
  } else
  if (p1.temp_ajst == 159) {
    p1.umid_ajst = 110;
  } else
  if (p1.temp_ajst >= 160) {
    p1.umid_ajst = 112;
  }
}


void loop(){
  wdt_reset();

  relogio();                              //sequnecia 1
  controle();                             //sequnecia 2
  sincronizaDados();                      //sequencia 3
  enviawifi();
  
  unsigned long currentMillis = millis();
   if(currentMillis - previousMillis > 1000) {    /// Funçao para atualizar o display a cada segundo e fazer a leitura dos sensores a cada segundo
 
     leitura(); 
     atualisaDisplay();
     timewifi++;

     alarme_status =!alarme_status;          //inverte status do alarme,para nao ficar som continuo.
     previousMillis = currentMillis;
   }


 }


void enviawifi(){

if(timewifi > 10){

char txt[300];

  if(espp == 0)cmdESP(F("AT+CWMODE=1\r\n"),F("OK"),3000);

 if(espp == 1){ cmdESP(F("AT+CIPSTART=\"TCP\",\"sistemamonitoramento-net.umbler.net\",80\r\n"),F("CONNECT"),6000);
   
 }
  
  if(espp == 2) {
  sprintf(txt,"AT+CIPSENDEX=%d\r\n", sizeof(txt));
  cmdESP(txt,">",5000);
  }

  if (espp == 3){
sprintf(txt,"GET /crud.php?ns=9999&dados=%03d,%03d,%03d,%03d,%03d,%03d,%02d,%02d,%02d HTTP/1.1\nHost: sistemamonitoramento-net.umbler.net\n\n\\0",0,20,p1.temp,p1.umid,p1.temp_ajst,p1.umid_ajst,0,p1.bt_buzzer,p1.bt_ventoinha );
   cmdESP(txt,F("200"),4000);
}
if (espp ==4){
  timewifi = 0;
  espp = 0;
}
}if(timewifi > 30){
 cmdESP(F("AT+CIPCLOSE"),F("CLOSED"),4000);
}

}

byte cmdESP(String cmdAT ,String receive, int timeout) {
  
  esp.print(cmdAT);

  unsigned long _tempo = millis();
  boolean _continuar=true; 
  String S="";

  while (_continuar) { 
    if (esp.available()) {
      char c = esp.read();
  //Serial.print(c);
      S+=c;
      
      if( S.indexOf(receive)>0) {
        
        _continuar = false;   
        S="";
            espp ++;   
        }  
      }
    if (millis()-_tempo > timeout){ _continuar=false;
   
    espp = espp;

    }
  }  
}