//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Imaging.pngimage.hpp>
#include <IdBaseComponent.hpp>
#include <IdComponent.hpp>
#include <IdCustomTCPServer.hpp>
#include <IdTCPClient.hpp>
#include <IdTCPConnection.hpp>
#include <IdTCPServer.hpp>
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
	TNotebook *Notebook1;
	TPanel *MainMenu;
	TImage *Image1;
	TButton *Gra;
	TButton *Wyjscie;
	TButton *Online;
	TButton *Ustawienia;
	TPanel *PanelGry;
	TPaintBox *PaintBox1;
	TButton *WyjdzZGry;
	TPanel *PanelUstawienia;
	TLabel *Label1;
	TButton *WyjdzZUstawien;
	TLabeledEdit *AdresIP;
	TLabeledEdit *Port;
	TButton *PrzyciskSerwer;
	TMemo *Konsola;
	TEdit *Polecenie;
	TButton *WyslijPolecenie;
	TTimer *clientThread;
	TLabel *Kolej;
	TLabel *Twoje;
	TIdTCPClient *client;
	TIdTCPServer *server;
	void __fastcall GraClick(TObject *Sender);
	void __fastcall WyjscieClick(TObject *Sender);
	void __fastcall WejdzDoMenu(TObject *Sender);
	void __fastcall PaintBox1Paint(TObject *Sender);
	void __fastcall PaintBox1MouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
	void __fastcall OnlineClick(TObject *Sender);
	void __fastcall UstawieniaClick(TObject *Sender);
	void __fastcall PrzyciskSerwerClick(TObject *Sender);
	void __fastcall WyslijPolecenieClick(TObject *Sender);

private:	// User declarations
public:		// User declarations
	__fastcall TForm1(TComponent* Owner);
    void reset_gry();
	bool zapisuj_lokacje(int x, int y);
	bool zmien_gracza();
	char sprawdz();
	int	 wygranaKomunikat( char gracz );

	void zapiszUstawienia();
	void wczytajUstawienia();

	void serverCreate();
	void serverConfugre();
	void serverStart();
	void serverStop();
	void __fastcall serverDetectConnect(TIdContext *AContext);
	void __fastcall serverDetectDisconnect(TIdContext *AContext);
	void __fastcall serverReadMessage(TIdContext *AContext);
	void serverSendMessage( String wiadomosc );

	void clientCreate();
	void clientConfigure();
	void clientStart();
	void clientStop();
	void __fastcall clientDetectConnect(TObject *Sender);
	void __fastcall clientDetectDisconnect(TObject *Sender);
	void __fastcall clientReadMessage(TObject *Sender);
    void clientSendMessage( String wiadomosc );
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
