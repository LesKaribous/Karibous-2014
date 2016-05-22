/*Programme Root 2014 - Eurobot
 Description : Programme de omportmeent pour arduino Micro
 Auteur : Adrien BRACq
 Date : 11/05/2014
 */
/* Schéma des E/S de la carte :
 
 USB
 D13        D9  D12
 A0         D8  D11 
 A1         D7  D10
 A2             D6
 A3             D5
 A4             D4
 A5
 
 */
// Déclaration des bibliothéques :
#include <Servo.h>
#include <Timer.h>

// Déclaration des variables globales :
// Déclaration des E/S
// Actionneurs :

const int PinMoteurDroit = 5 ;
const int PinMoteurGauche = 4 ;
const int PinPince = 6 ;

// Capteurs :

const int CaptAvDroit = A4 ;
const int CaptAvGauche = A3 ;

const int CaptPince = A2 ;

// Bouton :

const int Tirette = 10;

// Déclaration des Variables :

const int MDStop = 1460;
const int MDAv = 1000;
const int MDAr = 1800;

const int MGStop = 1483;
const int MGAv = 1800;
const int MGAr = 1000;

const int Ouvert = 180 ;
const int Ferme = 30 ;
const int Prend = 56 ;

//Variable tampons :
int Capt = 0 ;
volatile int Comportement = 0 ;
const int Ok = 1250 ; //Constante qui défini le Ok sur un comportement ( aka "fin")

// Déclaration des servomoteurs :

Servo MoteurDroit;
Servo MoteurGauche;
Servo Pince;

// Déclaration du timer de temps de match

Timer TpsMatch ;
Timer Attente ;

//---------------------------------------------------------
//SETUP
void setup()
{
  // Initialisation des E/S
  pinMode(Tirette,INPUT);
  // Initialisation de la liaison Série
  Serial.begin(9600);

  //Attente de début de match
  while(digitalRead(Tirette)==LOW)
  {
    delay(50); // Anti rebond
    //Attendre le tirage de la tirette
  }
  while(digitalRead(Tirette)==HIGH)
  {
    delay(50); // Anti rebond
    //Attendre le tirage de la tirette
  }
  //Démarrage du Match
  //Démarrage du temps de match
  TpsMatch.after(90000,FinMatch);
  //Démarrage des servomoteurs
  MoteurDroit.attach(PinMoteurDroit);
  MoteurGauche.attach(PinMoteurGauche);
  MoteurDroit.writeMicroseconds(MDStop);
  MoteurGauche.writeMicroseconds(MGStop);
  Pince.attach(PinPince); 


}

//---------------------------------------------------------
//            | BOUCLE PRINCIPALE |
//---------------------------------------------------------
void loop()
{
  // Mise à jour du Temps de Match
  TpsMatch.update() ;
  //Lecture du port Serie
  if (Serial.available() > 0) 
    {
      Comportement = Serial.parseInt() ;
      Serial.println(Comportement);
    }
  // Selection du Comportement
  switch (Comportement) 
  {
    case 0:
      Stop();
      break;
    case 1:
      Avant();
      break;
    case 2:
      Arriere();
      break;
    case 3:
      RotH();
      break;
    case 4:
      RotAH();
      break;
    case 666:
      RechercheFeu();
      break;
  }
}
//----------------------Fonction de test-----------------------
void Test ()
{
  if (pause(3000)==0) return; // Pause de 3000 ms
  Serial.println("Sortie de la boucle");
  Comportement = Ok ;
}
//----------------------Fonction de stop-----------------------
void Stop ()
{
  MoteurDroit.writeMicroseconds(MDStop);
  MoteurGauche.writeMicroseconds(MGStop);
}
//-----------------Fonction de marche avant-------------------
void Avant ()
{
  MoteurDroit.writeMicroseconds(MDAv);
  MoteurGauche.writeMicroseconds(MGAv);
}
//-----------------Fonction de marche arriere-------------------
void Arriere ()
{
  MoteurDroit.writeMicroseconds(MDAr);
  MoteurGauche.writeMicroseconds(MGAr);
}
//-----------------Fonction de rotation horaire-------------------
void RotH ()
{
  MoteurDroit.writeMicroseconds(MDAr);
  MoteurGauche.writeMicroseconds(MGAv);
}
//-----------------Fonction de rotation horaire-------------------
void RotAH ()
{
  MoteurDroit.writeMicroseconds(MDAv);
  MoteurGauche.writeMicroseconds(MGAr);
}

//---------------Fonction de recherche du feu------------------

void RechercheFeu()
{
  while (analogRead(CaptAvDroit)>150 || analogRead(CaptAvGauche)>150 || analogRead(CaptPince)>580)
  {
    //Capteur Avant Droit
    if (analogRead(CaptAvDroit)>150)
    {
      MoteurDroit.writeMicroseconds(MDStop);
    }
    //Capteur Avant Gauche
    if (analogRead(CaptAvGauche)>150)
    {
      MoteurGauche.writeMicroseconds(MGStop);
    }
    //Capteur Avant Pince
    if (analogRead(CaptPince)>580)
    {
      while(analogRead(CaptPince)>500)
      {
        Serial.println("approche");
        MoteurDroit.writeMicroseconds(MDStop - 10);
        MoteurGauche.writeMicroseconds(MGStop + 10);
        if (pause(1)==0) return; // Pause
      }
      MoteurDroit.writeMicroseconds(MDStop);
      MoteurGauche.writeMicroseconds(MGStop);
      Pince.write(Prend);
      if (pause(2000)==0) return; // Pause
      Pince.write(Ouvert);
      if (pause(2000)==0) return; // Pause
    }
    if (pause(1)==0) return; // Pause
  }
  Pince.write(Ouvert);
  MoteurDroit.writeMicroseconds(MDStop - 45);
  MoteurGauche.writeMicroseconds(MGStop + 40);

}

//------------------FIN DE MATCH--------------------------------

void FinMatch()
{
  // Arrêt des moteurs
  MoteurDroit.writeMicroseconds(MDStop);
  MoteurGauche.writeMicroseconds(MGStop);
  delay(500) ;
  // Coupure total des moteurs
  MoteurDroit.detach() ;
  MoteurGauche.detach() ;
  Pince.detach() ;
  //Notification
  Serial.write("Fin de match");
  // Mise en boucle infinie
  while (1==1){
  }
}

//---------------Delais sans arrêt-----------------------------
// Cette fonction crée un délais tout en vérifiant le port serie
// Elle renvoie '1' si le délais est dépassé
// Elle renvoie '0' si le délais a été interrompu par une communication
// Pour activer cette fonction mettre cette ligne à la place de delay :
//            if (pause(2000)==0) return; // Pause de 2000ms
// Cette fonction est utile pour des while également

int pause(int tps)
{
  int Temp = 0 ; // init de la variable "Temp" qui sera comparée à "tps"
  
  while(Temp < tps)
  {
    Temp += 1;  //Incrémente la variable de temps
    // Vérifie la disponibilité d'une info disponible
    if (Serial.available() > 0) 
    {
      Comportement = Serial.parseInt() ;    // Stock l'information
      Serial.println(Comportement);         // Echo de l'information
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

