/*Programme Root 2014 - Eurobot
 Description : Programme de omportmeent pour arduino Micro
 Auteur : Adrien BRACq
 Date : 11/05/2014
 */
/* Schéma des E/S de la carte :
 
            A5
D4          A4
D5          A3
D6          A2
D10  D7     A1
D11  D8     A0
D12  D9     D13
        USB
 
 */
// Déclaration des bibliothéques :
#include <Servo.h>
#include <Timer.h>
#include <Adafruit_NeoPixel.h>

#define PIN 11

// Déclaration des cnstantes :
//   Entrées / Sorties :
//     Capteurs :
const int CaptAdvAvant = A4;
const int CaptAdvArriere = A5;

const int CaptAdvDroite = A3;
const int CaptAdvGauche = A2;

const int CaptAvDroite = A1;
const int CaptAvGauche = A2;
const int CaptArHaut = 9; //correspond à la pin D9
const int CaptArBas = 10; //correspond à la pin D12

const int CaptPince = 8 ; //correspond à la pin D8

const int CaptFresque = 13;

//Seuil des capteurs

const int Seuil = 300 ;
const int SeuilCaptAdvAvant = 150 ;
const int SeuilCaptAdvDroite = 100 ;
const int SeuilCaptAdvGauche = 100 ;

const int SeuilCaptAdvArriere = 200;
const int SeuilCaptArHaut = 100;
const int SeuilCaptArBas = 100;

//Vitesse des moteurs

const int MDStop = 1454;
const int MDAv = 1000;
const int MDAr = 1800;

const int MGStop = 1483;
const int MGAv = 1800;
const int MGAr = 1000;

//    Bouttons :
const int Tirette = 4;
const int SelEquipe = 5;
const int IntMatch = 6; //patte d'interruption de match pour esclave

//  Constantes :
const boolean Jaune = true;
const boolean Rouge = false;

const int Avant = 0;
const int Arriere = 1;

// Comportements:
const int Stop = 0;
const int Deplacement = 1;
const int SuivreLigneAvant = 2;
const int SuivreLigneArriere = 7;
const int OuvrePince = 3;
const int FermePince = 4;
const int PrendPince = 5;
const int LanceBalle = 6;
const int DemiTourLigne = 8;
const int DemiTour = 9;
const int Fin = 69;

const int Ok = 1250;

// Déclaration des variables globales :
//   Variable tampons :
int Capt = 0 ;
volatile int Comportement = 0 ;
int Temp;

int SensActu = Avant;

boolean Equipe = 0;

// Déclaration du timer de temps de match

Timer TpsMatch ;

// Déclaration de la LED :
Adafruit_NeoPixel strip = Adafruit_NeoPixel(1, PIN, NEO_GRB + NEO_KHZ800);

//--------------------SETUP---------------------------------

void setup()
{

  // Initialisation des E/S
  pinMode(Tirette,INPUT);
  pinMode(SelEquipe,INPUT);
  pinMode(IntMatch,OUTPUT);
  pinMode(CaptFresque,INPUT_PULLUP);
  // Initialisation de la liaison Série
  Serial1.begin(9600);
  //Initialisation de la LED :
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  //Attente de début de match
  while(digitalRead(Tirette)==HIGH)
  {
    delay(50); // Anti rebond
    digitalWrite(IntMatch,HIGH);
    //Attendre le tirage de la tirette
    //Choisir l'équipe :
    CouleurEquipe();
  }
  while(digitalRead(Tirette)==LOW)
  {
    delay(50); // Anti rebond
    digitalWrite(IntMatch,LOW);
    //Attendre le tirage de la tirette
    //Choisir l'équipe :
    CouleurEquipe();
  }
  digitalWrite(IntMatch,HIGH);
  //Démarrage du Match
  //Démarrage du temps de match
  TpsMatch.after(90000,FinMatch);
  //Lecture de L'équipe
  Equipe = digitalRead(SelEquipe);
  //Validation du début par Led Verte
  strip.setPixelColor(0,strip.Color(0, 255, 0)); // Verte
  strip.show();
  delay(500);
  CouleurEquipe();

}

//------------------BOUCLE PRINICIPALE--------------------------
void loop()
{
  // Mise à jour du Temps de Match
  /*
  // Comportements:
const int Stop = 0;
const int Deplacement = 1;
const int SuivreLigne = 2;
const int OuvrePince = 3;
const int FermePince = 4;
const int PrendPince = 5;
const int LanceBalle = 6;*/

  //TestCapteur();

  TpsMatch.update() ;
  delay(200);
  //---------Suivre la ligne jusqu'au feu----------
  SensActu = Avant;
  EnvoiComportement(OuvrePince);
  delay(200);
  //Sort de la zone
  EnvoiVitesse(MDStop - 11 ,MGStop + 6);
  EnvoiComportement(Deplacement); 
  pause(500);
  //Suivre la ligne
  while (Capteur(CaptPince,10) < 450)
  {
    EnvoiComportement(SuivreLigneAvant);
    pause(1);
  }
  //--------------Se rapprocher du Feu---------------
  /*EnvoiComportement(Stop);
  delay(1000);*/
  //S'aligner sur le feu
  //Etat haut du capteur pince
  /*while (Capteur(CaptPince,20) < 500)
  {
    //Capteur Avant Droit
    if (Capteur(CaptAvDroite,20)>200)
    {
      EnvoiVitesse(MDStop ,MGStop + 6);
      EnvoiComportement(Deplacement); 
    }
    //Capteur Avant Gauche
    else if (Capteur(CaptAvGauche,20)>300)
    {
      EnvoiVitesse(MDStop - 11 ,MGStop);
      EnvoiComportement(Deplacement); 
    }
    else
    { 
      EnvoiVitesse(MDStop - 11 ,MGStop + 6);
      EnvoiComportement(Deplacement);  
    }
    pause(1);
  }*/
  /*Etat bas du capteur pince
  while (Capteur(CaptPince,10) > 420)
  {
    //Capteur Avant Droit
    if (Capteur(CaptAvDroite,20)>200)
    {
      EnvoiVitesse(MDStop ,MGStop + 6);
      EnvoiComportement(Deplacement); 
    }
    //Capteur Avant Gauche
    else if (Capteur(CaptAvGauche,20)>300)
    {
      EnvoiVitesse(MDStop - 11 ,MGStop);
      EnvoiComportement(Deplacement); 
    }
    else
    { 
      EnvoiVitesse(MDStop - 11 ,MGStop + 6);
      EnvoiComportement(Deplacement);  
    }
    pause(1);
  }*/
  //-----------------Pousser le Feu-----------------
  EnvoiComportement(Stop);
  delay(200);
  EnvoiComportement(FermePince);
  delay(800);
  
  //-----------------Tire les balles----------------
  if (Equipe == Jaune)
  {
    //---------Décallage de position si Jaune---------
    EnvoiComportement(SuivreLigneAvant);
    pause(200);
    EnvoiComportement(Stop);
    delay(100);
    //Tire les balles
    EnvoiComportement(LanceBalle);
    ReadOk();
    //pause(5000);
    //Tourne de 180 degrés
    EnvoiComportement(DemiTour);
    ReadOk();
    //Stop
    EnvoiComportement(Stop);
    pause(100);
    //-------------Lache le feu-------------
    EnvoiComportement(OuvrePince);
    pause(100);
    //Pousse le feu
    EnvoiComportement(SuivreLigneAvant); 
    pause(300); 
    //Stop
    EnvoiComportement(Stop);
    pause(100);
    //Tourne de 180 degrés
    EnvoiComportement(DemiTour);
    ReadOk();
    //Stop
    EnvoiComportement(Stop);
    pause(100);
    //Ferme Pince
    EnvoiComportement(FermePince);
    pause(100);
    
  }
  else
  {
    EnvoiVitesse(MDStop + 12 ,MGStop - 7);
    EnvoiComportement(Deplacement);  
    pause(200);
    //Tourne de 180 degrés
    EnvoiComportement(DemiTour);
    ReadOk();
    //Stop
    EnvoiComportement(Stop);
    pause(100);
    //Tire les balles
    EnvoiComportement(LanceBalle);
    ReadOk();
    //-------------Lache le feu-------------
    EnvoiComportement(OuvrePince);
    pause(100);
    //Pousse le feu
    EnvoiComportement(SuivreLigneAvant); 
    pause(300); 
    //Stop
    EnvoiComportement(Stop);
    pause(100);
    //Tourne de 180 degrés
    EnvoiComportement(DemiTour);
    ReadOk();
    //Stop
    EnvoiComportement(Stop);
    pause(100);
    //Ferme Pince
    EnvoiComportement(FermePince);
    pause(100);
  }
  
  
  
  //-----------------Va jusqu'à la fresque------------
  //Suivre la ligne
  SensActu = Avant;
  EnvoiComportement(SuivreLigneAvant);
  pause (500);
  while (digitalRead(CaptFresque) == HIGH)
  {
    pause(1);
    EnvoiComportement(SuivreLigneAvant);
    strip.setPixelColor(0,strip.Color(255, 255, 255)); // Blanc
    strip.show();
  }
  //Stop
  EnvoiComportement(Stop);
  delay(200);
  //Recul
  EnvoiVitesse(MDStop + 12 ,MGStop - 7);
  EnvoiComportement(Deplacement);  
  pause(500);
  //Demi-tour
  EnvoiComportement(DemiTour);
  ReadOk();
  //Stop
  EnvoiComportement(Stop);
  delay(200);
  //Pousse la fresque
  EnvoiVitesse(MDAr ,MGAr);
  EnvoiComportement(Deplacement);  
  delay(1000);
  SensActu = Avant;
  EnvoiVitesse(MDAv ,MGAv);
  EnvoiComportement(Deplacement); 
  pause(100);
  EnvoiComportement(Stop);
  
  
  //----------------Fin de match----------------------------
  FinMatch();
  
  
}

//------------------FIN DE MATCH--------------------------------

void FinMatch()
{
  Serial.print("Fin de match");
  EnvoiComportement(Fin);
  // Mise en boucle infinie
  strip.setPixelColor(0,strip.Color(0, 0, 255)); // Bleu
  strip.show();
  while (1==1){
    EnvoiComportement(Fin);
    digitalWrite(IntMatch,LOW);
  }
}

//-----------------Pause sans arrêt-----------------------------
// Cette fonction crée un délais tout en vérifiant la presence d'adversaire
// Elle renvoie '1' si le délais est dépassé
// Elle renvoie '0' si le délais a été interrompu par un adversaire
// Pour activer cette fonction mettre cette ligne à la place de delay :
//            if (pause(2000)==0) return; // Pause de 2000ms
// Cette fonction est utile pour des while également

int pause(int tps)
{
  int Temp = 0 ; // init de la variable "Temp" qui sera comparée à "tps"
  
  while(Temp < tps)
  {
    Temp += 1;  //Incrémente la variable de temps
    // Vérifie la presence d'un adversaire
    if (Adversaire(SensActu)) 
    {
      EnvoiComportement(Stop);
      delay(1000);
      return 0;                             // Fin de la fonction en cours
    }
    else
    {
      //Attendre une unité de temps
      delay(1);
      // Mise à jour du Temps de Match
      TpsMatch.update() ;
    }
  }
  // Fin du temps
  return 1;   
}

//--------------------Gestion de la couleur d'équipe-----------------
void CouleurEquipe()
{
  
   //Choix couleur d'équipe :
  if (digitalRead(SelEquipe) == Rouge)
  {
    strip.setPixelColor(0,strip.Color(255, 0, 0)); // Rouge
    strip.show();
    Equipe = Rouge;
  }
  else
  {
    strip.setPixelColor(0,strip.Color(255, 255, 0)); // Jaune
    strip.show();
    Equipe = Jaune;
  }
  
}

//-----------Envoi de comportement au second microcontrolleur-----------
void EnvoiComportement (int ComportementVoulu)
{
  Serial1.print("C");
  Serial1.println(ComportementVoulu); 
}

//-------------Envoi de vitesse au second microcontrolleur--------------
void EnvoiVitesse (int VitesseDroiteVoulue, int VitesseGaucheVoulue)
{
  Serial1.print("P");
  Serial1.print(VitesseDroiteVoulue); 
  Serial1.print(",");
  Serial1.println(VitesseGaucheVoulue); 
}

//----------------Fonction de detection adversaire----------------------
boolean Adversaire (int Sens)
{
  if (Sens == Avant)
  {
    if (Capteur(CaptAdvAvant,20) >= SeuilCaptAdvAvant || 
        Capteur(CaptAdvDroite,20) >= SeuilCaptAdvDroite ||
        Capteur(CaptAdvGauche,20)>= SeuilCaptAdvGauche )
        {
         //Adversaire devant
          strip.setPixelColor(0,strip.Color(255, 0, 0)); // Rouge
          strip.show(); 
          return true;
        }
  }
  else if (Sens == Arriere)
  {
     if (Capteur(CaptAdvArriere,20) >= SeuilCaptAdvArriere /*||
              Capteur(CaptArHaut,10) >= SeuilCaptArHaut*/)
        {
          //Adversaire derriere 
          strip.setPixelColor(0,strip.Color(255, 0, 0)); // Rouge
          strip.show();
          return true;
        }
  }
  
  strip.setPixelColor(0,strip.Color(0, 255, 0)); // Vert
  strip.show();
  return false;
        
}

//-------------Moyenne des mesures---------------
int Capteur(int Nom, int NbMesure)
{
 int Mesure = 0;
 int MesureTemp = 0;
 int MesurePrecedente = 0;
 for(int i=0 ; i<NbMesure ; i++)
 {
   MesureTemp = analogRead(Nom);
   if(i !=0)
   {
     if (abs(MesureTemp-MesurePrecedente) > 50)
     {MesureTemp = MesurePrecedente ;}
   }
   Mesure = Mesure + MesureTemp ;
   MesurePrecedente = MesureTemp;
 }
 Mesure = Mesure / NbMesure ;
 return Mesure;  
}

//-------------TestCapteur---------------

void TestCapteur()
{
  while(1==1)
  {
  Serial.print(Capteur(CaptAdvAvant,10));
  Serial.print(";");
  Serial.print(Capteur(CaptAdvDroite,10));
  Serial.print(";");
  Serial.println(Capteur(CaptAdvGauche,10));
  delay(50);
  }
}

//--------Fonction de Ack---------
boolean ReadOk()
{
  int inByte;
  while (inByte != 'O')
  {
    if (Serial1.available() > 0) 
    {
      inByte = Serial1.read();
      if (inByte == 'O')
      {return true;}
    }
    TpsMatch.update() ;
  }
}




