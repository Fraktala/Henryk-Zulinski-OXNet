//---------------------------------------------------------------------------

#include <vcl.h>
#include <Registry.hpp>
#pragma hdrstop

#include "Unit1.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;

char	wyniki[3][3];
int		wygranePola[3][2];
char	gracz = 'o';
char    wygranyGracz = ' ';
String	wygrana;

bool    serverMode = false;
bool    online = false;
char    onlinegracz = ' ';
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
	: TForm(Owner)
{
	serverCreate();
	clientCreate();
	wczytajUstawienia();
}

//---------------------------------------------------------------------------
void __fastcall TForm1::GraClick(TObject *Sender)
{
	reset_gry();
	Notebook1->ActivePage = "Game";
    online = false;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::OnlineClick(TObject *Sender)
{
    Notebook1->ActivePage = "Game";
	online = true;
	if (!serverMode) { clientStart(); }
	reset_gry();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::UstawieniaClick(TObject *Sender)
{
Notebook1->ActivePage = "Ustawienia";
}
//---------------------------------------------------------------------------
void __fastcall TForm1::WyjscieClick(TObject *Sender)
{
Application->Terminate();
}

//---------------------------------------------------------------------------
void __fastcall TForm1::WejdzDoMenu(TObject *Sender)
{
Notebook1->ActivePage = "MainMenu";
zapiszUstawienia();
}

//---------------------------------------------------------------------------
void __fastcall TForm1::PrzyciskSerwerClick(TObject *Sender)
{
	if (!server->Active) { serverStart(); }
	else { serverStop(); }
}

//---------------------------------------------------------------------------

void __fastcall TForm1::WyslijPolecenieClick(TObject *Sender)
{
	if (server->Active) { serverSendMessage( Polecenie->Text ); }
	if (client->Connected() ) { clientSendMessage( Polecenie->Text ); }
    Polecenie->Text = "";
}

//---------------------------------------------------------------------------
void __fastcall TForm1::PaintBox1Paint(TObject *Sender)
{
	int podzialX = PaintBox1->Width / 3;
	int podzialY = PaintBox1->Height / 3;

	// Rysowanie Siatki
	PaintBox1->Canvas->Pen->Width=1;
	PaintBox1->Canvas->Brush->Color = RGB(50, 50, 50);
	PaintBox1->Canvas->Pen->Color = RGB(255, 216, 55);
	PaintBox1->Canvas->MoveTo( podzialX*1, podzialY*0 );
	PaintBox1->Canvas->LineTo( podzialX*1, podzialY*3 );

	PaintBox1->Canvas->MoveTo( podzialX*2, podzialY*0 );
	PaintBox1->Canvas->LineTo( podzialX*2, podzialY*3 );

	PaintBox1->Canvas->MoveTo( podzialX*0, podzialY*1 );
	PaintBox1->Canvas->LineTo( podzialX*3, podzialY*1 );

	PaintBox1->Canvas->MoveTo( podzialX*0, podzialY*2 );
	PaintBox1->Canvas->LineTo( podzialX*3, podzialY*2 );

	// Rysowanie pozycji graczy
	int szerokoscPola = PaintBox1->Width /3;
	int wysokoscPola = PaintBox1->Height /3;
	int rozmiar = 0;
	int ix = 0;
	int iy = 0;

	if ( szerokoscPola < wysokoscPola ) { rozmiar= szerokoscPola; }
	else { rozmiar = wysokoscPola; }

	for (ix=0; ix<3; ix++) {
		for (iy=0; iy<3; iy++) {

			//ShowMessage( IntToStr(ix) + " " + IntToStr(iy) );
			int startX = szerokoscPola*ix + ( szerokoscPola/2 - rozmiar/2 );
			int startY = wysokoscPola*iy + ( wysokoscPola/2 - rozmiar/2 );

			if (wyniki[ix][iy]=='o') {
				PaintBox1->Canvas->Ellipse( startX, startY, startX+rozmiar, startY+rozmiar );
			}
			if (wyniki[ix][iy]=='x') {
				PaintBox1->Canvas->MoveTo(startX, startY);
				PaintBox1->Canvas->LineTo(startX+rozmiar, startY+rozmiar);
				PaintBox1->Canvas->MoveTo(startX+rozmiar, startY);
				PaintBox1->Canvas->LineTo(startX,startY+rozmiar);
			}

		}
	}

}

//---------------------------------------------------------------------------
void __fastcall TForm1::PaintBox1MouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
		  int X, int Y)
{
	// Sprawdzenie gdzie klink�� gracz
	int podzialX = PaintBox1->Width / 3;
	int podzialY = PaintBox1->Height / 3;
	int rozmiar = 0;
	int x;
	int y;

	if (podzialX < podzialY) { rozmiar = podzialX; }
	else { rozmiar = podzialY; }

	if ( X > podzialX*0 && X < podzialX*1 ) { x=0;}
	else if ( X > podzialX*1 && X < podzialX*2 ) { x=1;}
	else if ( X > podzialX*2 && X < podzialX*3 ) { x=2;}

	if ( Y > podzialY*0 && Y < podzialY*1 ) { y=0;}
	else if ( Y > podzialY*1 && Y < podzialY*2 ) { y=1;}
	else if ( Y > podzialY*2 && Y < podzialY*3 ) { y=2;}

	// Gra Online (sprawdzenie czy jest kolej gracza)
	if (online) {
		if (gracz != onlinegracz) { return; }	// nie twoja kolej
	}

	// Zpaisanie pozycji gracza / zmiana gracza / sprawdzenie wyniku
	if ( zapisuj_lokacje(x, y) ) {
		zmien_gracza();
		wygranyGracz = sprawdz();
		PaintBox1->Repaint();
	} else {
		return;
	}

	// Gra Online (wys�anie informacji o zmianie tury)
	if (online) {
		// Je�eli gracz jest serwerem
		if (serverMode) {
			serverSendMessage( "/nextTurn " + IntToStr(x) + ":" + IntToStr(y) );
		// Je�eli gracz jest klientem
		} else {
			clientSendMessage( "/nextTurn " + IntToStr(x) + ":" + IntToStr(y) );
		}
    }

	// Je�eli jest wygrana, wy�wietl komunikat
	int wynik = wygranaKomunikat( wygranyGracz );
	if (wynik == 0) {
		Notebook1->ActivePage = "MainMenu";
		if(online){
			if(!serverMode) { clientStop(); }
		}
	}
	if (wynik == 1) {
		reset_gry();
		PaintBox1->Repaint();
	}
}

//---------------------------------------------------------------------------
void TForm1::reset_gry() {
	PaintBox1->Enabled = true;

	if (online) { Twoje->Visible = true; }
	else { Twoje->Visible = false; }

	// Reset p�l do gry
	int ix, iy=0;
	for (ix=0; ix<3; ix++){
		for (iy=0; iy<3; iy++) {
				wyniki[ix][iy]=' ';
			}
		}

	// Reset koordynat�w wygranych p�l
	for (ix=0; ix<3; ix++){
		for (iy=0; iy<2; iy++) {
				wygranePola[ix][iy]=-1;
			}
		}

	// Konfiguracja gry Online (pocz�tek wybranie graczy)
	if (online) {

		// Je�eli gracz jest serwerem
		if (serverMode) {
			Randomize();
			int r = Random(2);				// random 2 [ 0..1 ]
			if (r==0) {
				onlinegracz = 'o';
				serverSendMessage( "/initPlayer x" );
			}
			else if (r==1) {
				onlinegracz = 'x';
				serverSendMessage( "/initPlayer o" );
			}
			gracz = onlinegracz;
		// Je�eli gracz jest klientem
		} else {
			PaintBox1->Enabled = false;
		}

	// Konfiguracja gry Offline (pocz�tek wybieranie gracza)
	} else {
		gracz = 'o';
    }
}

//---------------------------------------------------------------------------
bool TForm1::zapisuj_lokacje(int x, int y)
{
	// Zapisanie pozycji gracza w tablicy
	if (wyniki[x][y] != ' ') {return false;}
	wyniki[x][y]= gracz;
	return true;
}

//---------------------------------------------------------------------------
bool TForm1::zmien_gracza()
{
	// Zmiana gracza
	if (online) { Twoje->Caption = "Twoje: " + UpperCase( (String)onlinegracz ); }
	if (gracz=='o'){gracz='x';}
	else if(gracz=='x'){gracz='o';}
    Kolej->Caption = "Kolej: " + UpperCase( (String)gracz );
}

//---------------------------------------------------------------------------
char TForm1::sprawdz(){

	if ((wyniki[0][0]==wyniki[0][1]) && (wyniki[0][0]==wyniki[0][2]) && (wyniki[0][0]!=' ')){
		wygranePola[0][0]=0;
		wygranePola[0][1]=0;
		wygranePola[1][0]=0;
		wygranePola[1][1]=1;
		wygranePola[2][0]=0;
		wygranePola[2][1]=2;
		return wyniki[0][0];
	}
	if ((wyniki[1][0]==wyniki[1][1]) && (wyniki[1][0]==wyniki[1][2]) && (wyniki[1][0]!=' ')){
		wygranePola[0][0]=1;
		wygranePola[0][1]=0;
		wygranePola[1][0]=1;
		wygranePola[1][1]=1;
		wygranePola[0][0]=1;
		wygranePola[2][1]=2;
		return wyniki[1][0];
	}
	if ((wyniki[2][0]==wyniki[2][1]) && (wyniki[2][0]==wyniki[2][2]) && (wyniki[2][0]!=' ')){
		wygranePola[0][0]=2;
		wygranePola[0][1]=0;
		wygranePola[1][0]=2;
		wygranePola[1][1]=1;
		wygranePola[0][0]=2;
		wygranePola[2][1]=2;
		return wyniki[2][0];
	}


	if ((wyniki[0][0]==wyniki[1][0]) && (wyniki[0][0]==wyniki[2][0]) && (wyniki[2][0]!=' ')){
		wygranePola[0][0]=0;
		wygranePola[0][1]=0;
		wygranePola[1][0]=1;
		wygranePola[1][1]=0;
		wygranePola[0][0]=2;
		wygranePola[2][1]=0;
		return wyniki[0][0];
	}
	if ((wyniki[0][1]==wyniki[1][1]) && (wyniki[0][1]==wyniki[2][1]) && (wyniki[0][1]!=' ')){
		wygranePola[0][0]=0;
		wygranePola[0][1]=1;
		wygranePola[1][0]=1;
		wygranePola[1][1]=1;
		wygranePola[0][0]=2;
		wygranePola[2][1]=1;
		return wyniki[0][1];
	}
	if ((wyniki[0][2]==wyniki[1][2]) && (wyniki[0][2]==wyniki[2][2]) && (wyniki[0][2]!=' ')){
		wygranePola[0][0]=0;
		wygranePola[0][1]=2;
		wygranePola[1][0]=1;
		wygranePola[1][1]=2;
		wygranePola[0][0]=2;
		wygranePola[2][1]=2;
		return wyniki[0][2];
	}


	if ((wyniki[1][1]==wyniki[0][0]) && (wyniki[1][1]==wyniki[2][2]) && (wyniki[1][1]!=' ')){
		wygranePola[0][0]=1;
		wygranePola[0][1]=1;
		wygranePola[1][0]=0;
		wygranePola[1][1]=0;
		wygranePola[0][0]=2;
		wygranePola[2][1]=2;
		return wyniki[1][1];
	}
	if ((wyniki[1][1]==wyniki[2][0]) && (wyniki[1][1]==wyniki[0][2]) && (wyniki[1][1]!=' ')){
		wygranePola[0][0]=1;
		wygranePola[0][1]=1;
		wygranePola[1][0]=2;
		wygranePola[1][1]=0;
		wygranePola[0][0]=0;
		wygranePola[2][1]=2;
		return wyniki[1][1];
	}

	int ix,iy;
	char control = 'c';
	for (ix=0; ix<3; ix++) {
		for (iy=0; iy<3; iy++) {
			if ( wyniki[ix][iy] == ' ' ) { control = ' '; }
		}
	}
    if ( control == 'c' ) { return 'r'; }

	return ' ';
}

//---------------------------------------------------------------------------
int TForm1::wygranaKomunikat( char gracz ) {
    // Wyswietlenie wynikow
	wygrana = "Wygrana: "+String(gracz)+"\n czy chcesz zagra� jeszcze raz";
	LPCWSTR teksWygranej = wygrana.c_str();

	if ( gracz!=' '){
		if (gracz == 'r') {
			wygrana = "Remis! \n czy chcesz zagra� jeszcze raz";
			teksWygranej = wygrana.c_str();
        }

		int result = MessageBox(NULL, teksWygranej, L"KONIEC GRY",  MB_YESNO);

		switch (result) {
			case IDYES:
				return 1;
				break;

			case IDNO:
				return 0;
				break;

		}
	}
	return -1;
}

//---------------------------------------------------------------------------










//---------------------------------------------------------------------------
void TForm1::zapiszUstawienia() {
	TRegistry *regKey = new TRegistry;
	regKey->RootKey = HKEY_CURRENT_USER;

    regKey->OpenKey("HenrykZulinskiOX", true);
	regKey->WriteString("AdresIP", AdresIP->Text);
	regKey->WriteString("Port", Port->Text);
	regKey->CloseKey();
}
//---------------------------------------------------------------------------
void TForm1::wczytajUstawienia() {
	TRegistry *regKey = new TRegistry;
	regKey->RootKey = HKEY_CURRENT_USER;

	regKey->OpenKey("HenrykZulinskiOX", true);
	AdresIP->Text = regKey->ReadString("AdresIP");
	Port->Text = regKey->ReadString("Port");
	regKey->CloseKey();

	if (AdresIP->Text.IsEmpty()) { AdresIP->Text = "127.0.0.1"; }
    if (Port->Text.IsEmpty()) { Port->Text = "4801"; }
}
//---------------------------------------------------------------------------










//---------------------------------------------------------------------------
void TForm1::serverCreate() {
	server->OnConnect = &serverDetectConnect;
	server->OnDisconnect = &serverDetectDisconnect;
	server->OnExecute = &serverReadMessage;
}
//---------------------------------------------------------------------------
void TForm1::serverConfugre() {
	server->Bindings->Clear();
	server->Bindings->Add();
	server->Bindings->Items[0]->IP = AdresIP->Text;
	server->Bindings->Items[0]->Port = StrToInt( Port->Text );
}
//---------------------------------------------------------------------------
void TForm1::serverStart() {
	try {
		serverConfugre();
		Konsola->Lines->Add( "( i ) Serwer skonfigurowany !" );
		Konsola->Lines->Add( "AdresIP: " + AdresIP->Text );
		Konsola->Lines->Add( "Port: " + Port->Text );
	} catch( Exception *exc ) {
        Konsola->Lines->Add( "( ! ) B��d podczas konfiguracji serwera !" );
		MessageBox(NULL, L"B��d podczas konfiguracji serwera", L"B��d Serwera", MB_ICONERROR + MB_OK);
	}

	try {
		server->Active = true;
		serverMode = true;
		Konsola->Lines->Add( "( i ) Serwer uruchomiony !" );
	} catch( Exception *exc ) {
        Konsola->Lines->Add( "( ! ) B��d podczas uruchamiania serwera !" );
		MessageBox(NULL, L"B��d podczas uruchamiania serwera", L"B��d Serwera", MB_ICONERROR + MB_OK);
	}
}
//---------------------------------------------------------------------------
void TForm1::serverStop() {
	try {
		server->Active = false;
		serverMode = false;
        Konsola->Lines->Add( "( i ) Serwer wy��czony !" );
	} catch( Exception *exc ) {
		Konsola->Lines->Add( "( ! ) B��d podczas wy��czania serwera !" );
		MessageBox(NULL, L"B��d podczas wy��czania serwera", L"B��d Serwera", MB_ICONERROR + MB_OK);
	}
}
//---------------------------------------------------------------------------
void __fastcall TForm1::serverDetectConnect(TIdContext *AContext) {
	try {
		String adresIP = AContext->Connection->Socket->Binding->IP;
		Konsola->Lines->Add( "( i ) Klient w�a�nie si� pod��czy� !" );
		Konsola->Lines->Add( "Adres IP: " + adresIP );

		////////////////////////////////////////////////////////////////////
		// Je�eli gracz si� pod��czy po wej�ciu do gry wys�anie konfiguracji
		if (Notebook1->ActivePage == "Game") {
			Randomize();
			int r = Random(2);
			if (r==0) {
				onlinegracz = 'o';
				serverSendMessage( "/initPlayer x" );
			}
			else if (r==1) {
				onlinegracz = 'x';
				serverSendMessage( "/initPlayer o" );
			}
			gracz = onlinegracz;
		}
		////////////////////////////////////////////////////////////////////

	} catch( Exception *exc ) {
        Konsola->Lines->Add( "( ! ) B��d podczas pod��czania klienta !" );
        MessageBox(NULL, L"B��d podczas pod��czania klienta", L"B��d Serwera", MB_ICONERROR + MB_OK);
    }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::serverDetectDisconnect(TIdContext *AContext) {
    try {
		String adresIP = AContext->Connection->Socket->Binding->IP;
		Konsola->Lines->Add( "( i ) Klient w�a�nie si� od��czy� !" );
		Konsola->Lines->Add( "Adres IP: " + adresIP );
	} catch( Exception *exc ) {
		Konsola->Lines->Add( "( ! ) B��d podczas od��czania klienta !" );
		MessageBox(NULL, L"B��d podczas od��czania klienta", L"B��d Serwera", MB_ICONERROR + MB_OK);
	}
}
//---------------------------------------------------------------------------
void __fastcall TForm1::serverReadMessage(TIdContext *AContext) {
	String wiadomosc = "";

	try {
		wiadomosc = AContext->Connection->IOHandler->ReadLn(TIdTextEncoding_Default);
		if (wiadomosc != "") {
			Konsola->Lines->Add( "( ! ) Odebrano wiadomo�� !" );
			Konsola->Lines->Add( wiadomosc );
		}
    } catch( Exception *exc ) {
		Konsola->Lines->Add( "( ! ) B��d podczas odbierania wiadomo�ci !" );
		MessageBox(NULL, L"B��d podczas odbierania wiadomo�ci", L"B��d Serwera", MB_ICONERROR + MB_OK);
	}

	/////////////////////////////////////////////////////////////////////////
	if (wiadomosc != "") {
		// Jako serwer pobranie informacji o kolejnej turze
		if (wiadomosc.SubString0(0,9) == "/nextTurn" ) {
			int x = StrToInt( wiadomosc.SubString0(10,1) );
			int y = StrToInt( wiadomosc.SubString0(12,1) );

			// Zpaisanie pozycji gracza / zmiana gracza / sprawdzenie wyniku
			if ( zapisuj_lokacje(x, y) ) {
				zmien_gracza();
				wygranyGracz = sprawdz();
				PaintBox1->Repaint();
			} else {
				return;
			}

            // Je�eli jest wygrana, wy�wietl komunikat
			int wynik = wygranaKomunikat( wygranyGracz );
			if (wynik == 0) {
				Notebook1->ActivePage = "MainMenu";
			}
			if (wynik == 1) {
				reset_gry();
				PaintBox1->Repaint();
			}

		}
	}
    /////////////////////////////////////////////////////////////////////////
}
//---------------------------------------------------------------------------
void TForm1::serverSendMessage( String wiadomosc ) {
	TIdContext *polaczenie;
	TList *listaKlientow = server->Contexts->LockList();

	try {
		for (int i = 0; i < listaKlientow->Count; i++) {
			polaczenie = (TIdContext*) listaKlientow->Items[i];
			polaczenie->Connection->IOHandler->WriteLn( wiadomosc );
		}
		Konsola->Lines->Add( "( ! ) Wyslana wiadomo�� !" );
		Konsola->Lines->Add( wiadomosc );
	} catch( Exception *exc ) {
		Konsola->Lines->Add( "( ! ) B��d podczas wysy�ania wiadomo�ci !" );
		MessageBox(NULL, L"B��d podczas wysy�ania wiadomo�ci", L"B��d Serwera", MB_ICONERROR + MB_OK);
	}

	server->Contexts->UnlockList();
}
//---------------------------------------------------------------------------










//---------------------------------------------------------------------------
void TForm1::clientCreate() {
	client->OnConnected = &clientDetectConnect;
    client->OnDisconnected = &clientDetectDisconnect;
	clientThread->Enabled = false;                      //timer off
	clientThread->Interval = 100;                       //100 ms
	clientThread->OnTimer = &clientReadMessage;
}
//---------------------------------------------------------------------------
void TForm1::clientConfigure() {
	client->Host = AdresIP->Text;
	client->Port = StrToInt( Port->Text );
}
//---------------------------------------------------------------------------
void TForm1::clientStart() {
	try { clientConfigure(); }
	catch( Exception *exc ) {
		Notebook1->ActivePage = "MainMenu";
		MessageBox(NULL, L"B��d podczas konfiguracji klienta", L"B��d Klienta", MB_ICONERROR + MB_OK);
	}

	try { client->Connect(); }
	catch( Exception *exc ) {
		Notebook1->ActivePage = "MainMenu";
		MessageBox(NULL, L"B��d podczas pod��czania do serwera", L"B��d Klienta", MB_ICONERROR + MB_OK);
	}
}
//---------------------------------------------------------------------------
void TForm1::clientStop() {
	try { client->Disconnect(); }
	catch( Exception *exc ) { MessageBox(NULL, L"B��d podczas od�aczania klienta", L"B��d Klienta", MB_ICONERROR + MB_OK); }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::clientDetectConnect(TObject *Sender) {
	clientThread->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::clientDetectDisconnect(TObject *Sender) {
    clientThread->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::clientReadMessage(TObject *Sender) {
    String wiadomosc = "";

	try {
		if ( client->IOHandler->InputBufferIsEmpty() ) {
			client->IOHandler->CheckForDataOnSource(0);
			client->IOHandler->CheckForDisconnect();
			if ( client->IOHandler->InputBufferIsEmpty() ) { return; }
        }
		wiadomosc = client->IOHandler->ReadLn( TIdTextEncoding_Default );
	} catch( Exception *exc ) {
		clientThread->Enabled = false;
        MessageBox(NULL, L"B��d podczas odbierania wiadomo�ci", L"B��d Klienta", MB_ICONERROR + MB_OK);
        Notebook1->ActivePage = "MainMenu";
		return;
	}

	/////////////////////////////////////////////////////////////////////////
	if (wiadomosc != "") {
    	// Jako klient pobranie informacji o rozpocz�ciu gry
		if (wiadomosc.SubString(0,11) == "/initPlayer" ) {
			PaintBox1->Enabled = true;
			onlinegracz = wiadomosc.SubString(13,1).c_str()[0];
			if (onlinegracz == 'o') { gracz = 'x'; }
			else if (onlinegracz == 'x') { gracz = 'o'; }
			MessageBox(NULL, L"Serwer rozpocz�� gr�", L"Rozgrywka", MB_ICONEXCLAMATION + MB_OK);

		}
		// Jako klient pobranie informacji o kolejnej turze
		if (wiadomosc.SubString0(0,9) == "/nextTurn" ) {
			int x = StrToInt( wiadomosc.SubString0(10,1) );
			int y = StrToInt( wiadomosc.SubString0(12,1) );

			// Zpaisanie pozycji gracza / zmiana gracza / sprawdzenie wyniku
			if ( zapisuj_lokacje(x, y) ) {
				zmien_gracza();
				wygranyGracz = sprawdz();
				PaintBox1->Repaint();
			} else {
				return;
			}

            // Je�eli jest wygrana, wy�wietl komunikat
			int wynik = wygranaKomunikat( wygranyGracz );
			if (wynik == 0) {
				Notebook1->ActivePage = "MainMenu";
				clientStop();
			}
			if (wynik == 1) {
				reset_gry();
				PaintBox1->Repaint();
			}

        }
	}
	/////////////////////////////////////////////////////////////////////////
}
//---------------------------------------------------------------------------
void TForm1::clientSendMessage( String wiadomosc ) {
	try { client->IOHandler->WriteLn( wiadomosc, TIdTextEncoding_Default ); }
	catch( Exception *exc ) { MessageBox(NULL, L"B��d podczas wysy�ania wiadomo�ci", L"B��d Klienta", MB_ICONERROR + MB_OK); }
}
//---------------------------------------------------------------------------

