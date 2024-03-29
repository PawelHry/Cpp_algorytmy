#include "NXT++.h"
#include <iostream>
#include <conio.h>
using namespace std;
using namespace NXT;

Comm::NXTComm comm;

//PRZED U¯YCIEM SPRZETU DOK£ADNIE SPRAWDZAMY: 
//DO KTÓRYCH PORTÓW WEJŒCIOWYCH (IN) POD£¥CZONE S¥ SENSORY,\
//DO KTÓRYCH PORTÓW WYJŒCIOWYCH (OUT) POD£¥CZONE S¥ SERVO-MOTORY,
//STARAMY SIÊ PISAC KOD Z MO¯LIWOŒCI¥ ZATRZYMANIA ROBOTA W KA¯DEJ CHWILI,
//ROBOT POWIENIEN BYC PRZEMIESZCZONY W MIEJSCE TESTÓW ZA POŒREDNICTWEM ZDALNEGO STEROWANIA,

struct kola{
	int k1;
	int k2;
	int dcte;
};

char decyzjaManualna(){
	if(kbhit()) {
		return _getch();
	}
	else {
		return ' ';
	}
}

int convert(int k ,int x){
	x=x-k;
	if ( x > 180 ){
		x=x-360;
	}
	else  if ( x < -180){
		x = 360 + x ;
	}
	return x;
}


/*
int convert(int k, int orient){
	if (k<180){
		if (orient <= k+180){
			orient = orient-k;
		}
		else{
			orient = (orient -k) - 360;
		}
	}
	if (k>=180){
		if (orient >= k - 360){
			orient = orient-k;
		}
		else{
			orient = 360-(k-orient);
			}
	}
	return orient;
}
*/

kola steer(kola jazda,int k,int orient){
	int k1 = jazda.k1;
	int k2 = jazda.k2;
	kola wynik;
	int cte = convert(k, orient);
	int ksi = cte - (jazda.dcte / 0.5);
	if (cte<=0){
		if(abs(cte)>k1){
			k2=0;
		}
		else{
			k2=k1+(ksi*cte);
		}
	}
	if (cte>0){
		if(cte>k2){
			k1=0;
		}
		else{
		k1=k2-(ksi*cte);
		}
	}

	if (k1 > 30){
		k1 = 30;
	}
	if (k2 > 30){
		k2 = 30;
	}
	if (k1 < 20){
		k1 = 20;
	}
	if (k2 < 20 ){
		k2 = 20;
	}

	if (cte < 160 && cte > -160){
		if (k1 > 20){
			k2 = k1*(-1);
		}
		if (k2 > 20){
			k1 = k2*(-1);
		}
	}
	wynik.k1 = k1;
	wynik.k2 = k2;
	wynik.dcte = cte;
	return wynik;
}


int main()
{
	srand(time(NULL));

	cout << "Szukanie sygnalu... to moze zajac chwile.\n";
	if(NXT::OpenBT(&comm)) //initialize the NXT and continue if it succeeds
	{
		cout<< "Sygnal znaleziony!";
		cout<<"\nPoziom baterii = "<<NXT::BatteryLevel(&comm);
		cout<<"\nDostepna pamiec flash = "<<NXT::GetAvailableFlash(&comm);

		cout<< "\np - polnoc" << "\n";
		cout<<"o - poludnie"<< "\n";
		cout<<"w - wschod" << "\n";
		cout<<"z - zachod" << "\n";
		cout<<"s - wprowadzony" << "\n";
		
		int x=30;//szybkoœæ jazdy w przód i ty³
		int xtemp=30;//potrzebna do sprawdzania czy predkosc sie zmienila

		int y=20;//szybkoœæ obrotu w lewo, prawo
		int ytemp=20;//potrzebna do sprawdzania czy predkosc sie zmienila

		char last_value;//zapamietana ostatnia instrukcja steruj¹ca

        //Przed uzyciem sprawdzamy numer portu do którego jest podl¹czony sensor, 
		NXT::Sensor::SetTouch(&comm, IN_1);//inicjacja sensora dotyku

		//pocz¹tek programu
		NXT::StartProgram(&comm,"program1");

		//zmienna u¿ywana do sterowania
		char decyzja;

		/*
		//w³¹czenie sensoru koloru
		NXT::Sensor::SetColor(&comm, IN_3, 'n');
		int color;
		*/
		// w³¹czenie sonaru
		NXT::Sensor::SetSonar(&comm, IN_1);
		int sonar;
		int zmiana = 0;
		char ostatnia = ' ';
		kola jade;
		jade.k1 = 30;
		jade.k2 = 30;
		jade.dcte = 0;

		do{

		Wait(10);
		decyzja = decyzjaManualna();
		if (decyzja == ' '){
			decyzja = ostatnia;
		}
		int orientacja = 2* NXT::Sensor::GetSonarValue(&comm, IN_1);

		// s
		int wprowadz = 70;
		wprowadz = (wprowadz + 180)%360;

		// p
		int polnoc = 180; // dazy do 0
		polnoc = (polnoc + 180)%360;

		// o
		int poludnie = 0; // dazy do 180
		poludnie = (poludnie + 180)%360;
		// w
		int wschod = 270; // dazy do 90
		wschod = (wschod + 180)%360;

		// z
		int zachod = 90; // dazy do 270
		zachod = (zachod + 180)%360;

		kola jazda;

		if(decyzja=='p' || decyzja=='P'){
			jazda = steer(jade, polnoc, orientacja);
			jade.dcte = jazda.dcte;
			NXT::Motor::SetForward(&comm, OUT_B, jazda.k1);
			NXT::Motor::SetForward(&comm, OUT_C, jazda.k2);
			ostatnia = 'p';
		}

		if(decyzja=='o' || decyzja=='O'){
			jazda = steer(jade, poludnie, orientacja);
			jade.dcte = jazda.dcte;
			NXT::Motor::SetForward(&comm, OUT_B, jazda.k1);
			NXT::Motor::SetForward(&comm, OUT_C, jazda.k2);
			ostatnia = 'o';
		}

		if(decyzja=='w' || decyzja=='W'){
			jazda = steer(jade, wschod, orientacja);
			jade.dcte = jazda.dcte;
			NXT::Motor::SetForward(&comm, OUT_B, jazda.k1);
			NXT::Motor::SetForward(&comm, OUT_C, jazda.k2);
			ostatnia = 'w';
		}

		if(decyzja=='z' || decyzja=='Z'){
			jazda = steer(jade, zachod, orientacja);
			jade.dcte = jazda.dcte;
			NXT::Motor::SetForward(&comm, OUT_B, jazda.k1);
			NXT::Motor::SetForward(&comm, OUT_C, jazda.k2);
			ostatnia = 'z';
		}

		if(decyzja=='s' || decyzja=='s'){
			jazda = steer(jade, wprowadz, orientacja);
			jade.dcte = jazda.dcte;
			NXT::Motor::SetForward(&comm, OUT_B, jazda.k1);
			NXT::Motor::SetForward(&comm, OUT_C, jazda.k2);
			ostatnia = 's';
		}


		if(decyzja=='K' || decyzja=='k')
		{
		NXT::Motor::Stop(&comm, OUT_B, 0);
		NXT::Motor::Stop(&comm, OUT_C, 0);
		NXT::Motor::Stop(&comm, OUT_A, 0);
		NXT::Sensor::SetSonarOff(&comm, IN_1);
		/*
		//wy³¹czenie sensora koloru
		NXT::Sensor::SetColorOff(&comm,IN_3);
		*/
		break;
		}
			continue;
		}while(decyzja!='k' && decyzja!='K');

		NXT::StopProgram(&comm);
	}
	NXT::Close(&comm); //close communication with NXT

	cout<<"\n\nPress ENTER to close program";
	getchar();

	return 0;
}








//opcje dotyczace Mindstorm NXT v. 1.0

//if(decyzja=='L' || decyzja=='l')
// {
//  NXT::Sensor::SetLight(&comm, IN_3, 1);
//  Wait(50);
//  cout<<"LightValue = "<<NXT::Sensor::GetValue(&comm, IN_3);
//  Wait(50);
//  NXT::Sensor::SetLight(&comm, IN_3, 0);
// }

 //if(decyzja=='O' || decyzja=='o')
// {
// NXT::Sensor::SetSound(&comm, IN_2);
// Wait(50);
// cout<<"SoundValue = "<<NXT::Sensor::GetValue(&comm, IN_2);
// }
