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
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_L3GD20_U.h>

// Déclaration des variables globales :
// Déclaration des E/S
// Actionneurs :

const int PinMoteurDroit = 5 ;
const int PinMoteurGauche = 4 ;
const int PinPince = 6 ;
const int PinLanceur = 7 ;
const int PinSelecteur = 10;

// Capteurs :

const int CaptLigneDroite = 11;
const int CaptLigneGauche = 12;

const int Tirette =A0 ;

/* Assign a unique ID to this sensor at the same time */
Adafruit_L3GD20_Unified gyro = Adafruit_L3GD20_Unified(20);

// Déclaration des Variables :

const int MDStop = 1454;
const int MDAv = 1000;
const int MDAr = 1800;

const int MGStop = 1483;
const int MGAv = 1800;
const int MGAr = 1000;

const int Ouvert = 150 ;
const int Ferme = 30 ;
const int Prend = 56 ;

const int Arret = 90;
const int VMax = 109;

const int Charge = 90;
const int Lance = 130;


//Variable tampons :
int Capt = 0 ;
volatile int Comportement = 0 ;
const int Ok = 1250 ; //Constante qui défini le Ok sur un comportement ( aka "fin")
int FinBalle = 0;
int CompteurBalle = 0;

int inByte;

int VitesseDroite;
int VitesseGauche;

int ValLigneDroite;
int ValLigneGauche;

int VitesseBalle;
int PositionSelecteur;

int FinDemiTour = 0;

float Position = 0.0L;
float Val = 0.0L;

float AngleDemande = 0;

// Déclaration des servomoteurs :

Servo MoteurDroit;
Servo MoteurGauche;
Servo Pince;
Servo Lanceur;
Servo Selecteur;

// Déclaration du timer de temps de match

Timer TpsMatch ;


//---------------------------------------------------------
//SETUP
void setup()
{
  // Initialisation des E/S
  pinMode(Tirette,INPUT);
  pinMode(CaptLigneDroite,INPUT);
  pinMode(CaptLigneGauche,INPUT);
  // Initialisation de la liaison Série
  Serial1.begin(9600);
  //Active l'auto range
  gyro.enableAutoRange(true);
  //Initialise le capteur
  if(!gyro.begin(GYRO_RANGE_2000DPS))
  {
    /* There was a problem detecting the L3GD20 ... check your connections */
    while(1);
  }
  delay (1000);
  

  //Attente de début de match
  while(digitalRead(Tirette)==HIGH)
  {
    delay(50); // Anti rebond
    //Attendre le tirage de la tirette
  }
  while(digitalRead(Tirette)==LOW)
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
  Pince.write(Ferme);
  Lanceur.attach(PinLanceur);
  Selecteur.attach(PinSelecteur);
  Selecteur.write(130);


}

//---------------------------------------------------------
//            | BOUCLE PRINCIPALE |
//---------------------------------------------------------
void loop()
{
  // Mise à jour du Temps de Match
  TpsMatch.update() ;
  //Lecture du port Serie
  ReadSerial();
  // Selection du Comportement
  switch (Comportement) 
  {
    case 0:
      Stop();
      break;
    case 1:
      Deplacement();
      break;
    case 2:
      SuivreLigneAvant();
      break;
    case 3:
      OuvrePince();
      break;
    case 4:
      FermePince();
      break;
    case 5:
      PrendPince();
      break;
    case 6:
      LanceBalle(6);
      break;
    case 7:
      SuivreLigneArriere();
      break;
    case 8:
      DemiTourJaune();
      break;
    case 69:
      FinMatch();
      break;
    case 9:
      DemiTour(2.75);
      break;
    case 10:
    //Angle
      break;
    case 666:
      Test();
      break;
  }
}
//----------------------Fonction de test-----------------------
void Test ()
{
  while(1==1)
  {
    ReadSerial();
    Lanceur.write(VMax);
    delay(1000);
    Selecteur.write(Lance);
    delay(1000);
    Selecteur.write(Charge);
  }
}
//----------------------LanceBalle-----------------------
void LanceBalle (int Balle)
{
  if (FinBalle == 0)
  {
    Lanceur.write(VMax);
    pause(400);
    if (CompteurBalle<Balle)
    {
      Selecteur.write(Charge);
      pause(300);
      Selecteur.write(Lance);
      pause(100);
      CompteurBalle++;
    }
    else
    {
      Lanceur.write(Arret);
      FinBalle = 1;
      Serial1.println('O');
      pause(100);
    }
  }
}
//----------------------Fonction de stop-----------------------
void Stop ()
{
  MoteurDroit.writeMicroseconds(MDStop);
  MoteurGauche.writeMicroseconds(MGStop);
  Lanceur.write(Arret);
}
//-----------------Fonction de deplacement-------------------
void Deplacement ()
{
  MoteurDroit.attach(PinMoteurDroit);
  MoteurGauche.attach(PinMoteurGauche);
  
  MoteurDroit.writeMicroseconds(VitesseDroite);
  MoteurGauche.writeMicroseconds(VitesseGauche);
}
//--------------Demi Tour Ligne Jaune------------------
void DemiTourJaune()
{
  if(FinDemiTour==0)
  {
    MoteurDroit.writeMicroseconds(MDStop+11);
    MoteurGauche.writeMicroseconds(MGStop+6);
    pause(3000);
    while (digitalRead(CaptLigneDroite) == true)
    { pause(1); }
    MoteurDroit.writeMicroseconds(MDStop);
    MoteurGauche.writeMicroseconds(MGStop);
    pause(1000);
    FinDemiTour = 1 ;
  }
  Serial1.println('O');
  FinDemiTour=0;
  Comportement = 0;
  pause(100);
}

//--------------Demi Tour------------------
void DemiTour(float Angle)
{
  Position = 0;
  if(FinDemiTour==0)
  {
    while (Position <= Angle)
    {
    MoteurDroit.writeMicroseconds(MDStop+11);
    MoteurGauche.writeMicroseconds(MGStop+6);
    Gyroscope();
    }
    MoteurDroit.writeMicroseconds(MDStop);
    MoteurGauche.writeMicroseconds(MGStop);
    pause(1000);
    FinDemiTour = 1 ;
  }
  Serial1.println('O');
  FinDemiTour=0;
  Comportement = 0;
  pause(100);
}

//--------------Fonction de suivi de ligne Avant------------------
void SuivreLigneAvant()
{
  //Lit les capteurs
  ValLigneDroite = digitalRead (CaptLigneDroite);
  ValLigneGauche = digitalRead (CaptLigneGauche);
  
  //Comportement selon les capteurs
  if (ValLigneDroite == false && ValLigneGauche == false)
  {
    MoteurDroit.writeMicroseconds(MDStop-25);
    MoteurGauche.writeMicroseconds(MGStop+20);
  }
  else if (ValLigneDroite == true && ValLigneGauche == true)
  {
    MoteurDroit.writeMicroseconds(MDStop-25);
    MoteurGauche.writeMicroseconds(MGStop+20);
  }
  else if (ValLigneDroite == false && ValLigneGauche == true)
  {
    MoteurDroit.writeMicroseconds(MDStop);
    MoteurGauche.writeMicroseconds(MGStop+20);
  }
  else if (ValLigneDroite == true && ValLigneGauche == false)
  {
    MoteurDroit.writeMicroseconds(MDStop-25);
    MoteurGauche.writeMicroseconds(MGStop);
  }
  
}
//--------------Fonction de suivi de ligne Arriere------------------
void SuivreLigneArriere()
{
  //Lit les capteurs
  ValLigneDroite = digitalRead (CaptLigneDroite);
  ValLigneGauche = digitalRead (CaptLigneGauche);
  
  //Comportement selon les capteurs
  if (ValLigneDroite == false && ValLigneGauche == false)
  {
    MoteurDroit.writeMicroseconds(MDStop+11);
    MoteurGauche.writeMicroseconds(MGStop-6);
  }
  else if (ValLigneDroite == true && ValLigneGauche == true)
  {
    MoteurDroit.writeMicroseconds(MDStop+11);
    MoteurGauche.writeMicroseconds(MGStop-6);
  }
  else if (ValLigneDroite == false && ValLigneGauche == true)
  {
    MoteurDroit.writeMicroseconds(MDStop-11);
    MoteurGauche.writeMicroseconds(MGStop+5);
    pause(2000);
  }
  else if (ValLigneDroite == true && ValLigneGauche == false)
  {
    MoteurDroit.writeMicroseconds(MDStop-9);
    MoteurGauche.writeMicroseconds(MGStop+6);
    pause(2000);
  }
  
}

//------------------Ferme la pince--------------------------------
void FermePince()
{ Pince.write(Ferme); }

//------------------Ouvre la pince--------------------------------
void OuvrePince()
{ Pince.write(Ouvert); }

//------------------Prendre un feu--------------------------------
void PrendPince()
{ Pince.write(Prend); }

//------------------FIN DE MATCH--------------------------------

void FinMatch()
{
  // Arrêt des moteurs
  MoteurDroit.writeMicroseconds(MDStop);
  MoteurGauche.writeMicroseconds(MGStop);
  delay(1000) ;
  // Coupure total des moteurs
  MoteurDroit.detach() ;
  MoteurGauche.detach() ;
  Pince.detach() ;
  //Notification
  Serial.print("Fin de match");
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
    if (Serial1.available() > 0) 
    {
      ReadSerial(); // Lire la liason série
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



//--------------------------Lecture Serie----------------------

void ReadSerial()
{
  if (Serial1.available() > 0) 
  {
    inByte = Serial1.read();
    switch (inByte) {
      //--------------------Vitesse----------------------------
    case 'P':
      VitesseDroite = Serial1.parseInt();                                  //Recupere la vitesse demandée
      VitesseGauche = Serial1.parseInt();                                  //Recupere la vitesse demandée
      
      break;
    //-------------------Comportement--------------------------
    case 'C':
      Comportement = Serial1.parseInt();
      
      break;
    //------------------------Angle----------------------------
    case 'A':
      AngleDemande = Serial1.parseInt();
      
      break;
    }
  }
}

//-------------Gyroscope-------------------
void Gyroscope()
{
  sensors_event_t event; 
  gyro.getEvent(&event);
  Val = event.gyro.z ;
  Position = Position + (Val+0.03)*0.01 ;
  delay(10);
}

